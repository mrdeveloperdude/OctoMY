#include "Servotor32Controller.hpp"



#include "SerialSettingsWidget.hpp"
#include "../libutil/utility/Standard.hpp"


#include <QDebug>
#include <QtGlobal>
#include <QStringBuilder>
#include <QBitArray>


Servotor32Controller::Servotor32Controller(QObject *parent)
	: IServoController("Servotor32", parent)
	, settings(new SerialSettingsWidget)
	, serial(new QSerialPort(this))
	, lastPos{0}
	, dirtyMoveFlags(0xFFFFFFFF)
{

	if(!connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onSerialHandleError(QSerialPort::SerialPortError)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(serial, SIGNAL(readyRead()), this, SLOT(onSerialReadData()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(serial, SIGNAL(bytesWritten(qint64)), this, SLOT(onSerialDataWritten(qint64)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(settings,SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}

Servotor32Controller::~Servotor32Controller()
{
	//ASIMOV: Limp all servos before closing shop to avoid frying them if they are trying to reach impossible positions
	killAll();
	delete settings;
}

void Servotor32Controller::configure()
{
	if(nullptr!=settings) {
		qDebug()<<"SETTINGS SHOW";
		settings->show();
	}
}


void Servotor32Controller::openSerialPort()
{
	SerialSettings p = settings->settings();
	serial->setPortName(p.name);
	serial->setBaudRate(p.baudRate);
	serial->setDataBits(p.dataBits);
	serial->setParity(p.parity);
	serial->setStopBits(p.stopBits);
	serial->setFlowControl(p.flowControl);
	if (serial->open(QIODevice::ReadWrite)) {
		qDebug()<<tr("Connected to %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits).arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
		emit connectionChanged();
	} else {
		qDebug()<<"ERROR OPENING: "<<serial->errorString();
	}
}

void Servotor32Controller::closeSerialPort()
{
	if (serial->isOpen()) {
		serial->close();
		emit connectionChanged();
	}
	qDebug()<<"Disconnected";
}


bool Servotor32Controller::isConnected()
{
	return serial->isOpen();
}


void Servotor32Controller::syncMove()
{
	if(isConnected()) {
		QString data;
		//qDebug()<<"SYNC-MOVE: "<< QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		for(quint32 i=0; i<SERVO_COUNT; ++i) {
			if((dirtyMoveFlags & (0x1<<i))>0) {
				data += "#" +QString::number(i) + "P" + QString::number(lastPos[i]) +"\n";
			}
		}
		writeData(data.toLatin1());
		dirtyMoveFlags=0;
	} else {
		qWarning()<<"ERROR: Trying to syncMove with serial when not connected";
	}
}


void Servotor32Controller::writeData(const QByteArray &data)
{
	if(0==data.size()) {
		qWarning()<<"ERROR: WRITING CALLED WITH NO DATA";
	} else {
		const qint64 accepted=serial->write(data);
		if(accepted<0) {
			qWarning()<<"ERROR: write was not accepted. ERROR="<<serial->errorString();
		} else {
			if(data.size()>accepted) {
				qWarning()<<"ERROR: Data truncation occurred, serial port would not accept more";
			}
			//serial->flush();
			//qDebug()<<"WRITING: "<<data.size()<<" accepted "<<accepted;
		}
	}
}




// Serial IO slots
//////////////////////////////////////////////

void Servotor32Controller::onSerialReadData()
{
	if(isConnected()) {
		QByteArray data = serial->readAll();
		qDebug()<<data;
		//TODO:parse and handle data
	} else {
		qWarning()<<"ERROR: Trying to read data from serial when not connected";
	}
}



void Servotor32Controller::onSerialDataWritten( qint64 written)
{
	qint64 left=serial->bytesToWrite();
	//qDebug()<<"DATA WRITTEN: "<<written <<" (left:"<<left<<") ";
	if(left<=0 && dirtyMoveFlags >0) {
		syncMove();
	}
}


void Servotor32Controller::onSerialHandleError(QSerialPort::SerialPortError error)
{
	/*
	NoError,
	DeviceNotFoundError,
	PermissionError,
	OpenError,
	ParityError,
	FramingError,
	BreakConditionError,
	WriteError,
	ReadError,
	ResourceError,
	UnsupportedOperationError,
	UnknownError,
	TimeoutError,
	NotOpenError
			*/
	qDebug()<<"HANDLED ERROR: "<<error;
	if (error == QSerialPort::ResourceError) {
		qDebug()<<"Critical Error"<<serial->errorString();
		closeSerialPort();
	}
}

void Servotor32Controller::onSettingsChanged()
{
	{
		QSignalBlocker(this);
		closeSerialPort();
		openSerialPort();
		settings->hide();
		//version();
		writeData("\n");
	}
	emit settingsChanged();
}



// IServoController interface
//////////////////////////////////////////////

void Servotor32Controller::kill(QBitArray &flags)
{
	//Trivial reject: kill ALL
	if(isConnected()) {
		const quint32 sz=flags.size();
		if((quint32)flags.count(true)==sz) {
			writeData("K\n");
		} else {
			QString data;
			for(quint32 i=0; i<sz; ++i) {
				if(flags.testBit(i)) {
					data += QLatin1Literal("#") +  QString::number(i) + QLatin1String("L\n");
				}
			}
			qDebug()<<"KILL: "<<data<<" foir "<<flags;
			writeData(data.toLatin1());
		}
	} else {
		qWarning()<<"ERROR: Trying to kill servo via serial when not connected";
	}
}


void Servotor32Controller::centerAll()
{
	if(isConnected()) {
		writeData("C\n");
	} else {
		qWarning()<<"ERROR: Trying to center servos using serial when not connected";
	}

}


void Servotor32Controller::version()
{
	if(isConnected()) {
		writeData("V\n");
	} else {
		qWarning()<<"ERROR: Trying to get version via serial when not connected";
	}

}


void Servotor32Controller::debug()
{
	if(isConnected()) {
		writeData("D\n");
	} else {
		qWarning()<<"ERROR: Trying to debug with serial when not connected";
	}

}

//TODO: look at binary extension introduced in 2.1 version of hexy firmware to improve performance
void Servotor32Controller::move(Pose &pose)
{
	qreal pos[1]= {0.0};
	quint32 flags;
	if(isConnected()) {
		for(quint32 i=0; i<SERVO_COUNT; ++i) {
			quint32 flag=(0x1<<i);
			if((flags & flag)>0) {
				const quint32 p=(quint32)(qBound(-1.0, pos[i], 1.0)*1000.0+1500.0);
				//Skip unecessary communication if value did not change
				if(lastPos[i]!=p) {
					lastPos[i]=p;
					dirtyMoveFlags|=flag;
				}
			}
		}
		//qDebug()<<"MOVE: flags="<< QString("%1").arg( flags, 16, 2, QChar('0'))<< ", dirtyMoveFlags="<<QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		if(dirtyMoveFlags>0 && 0==serial->bytesToWrite()) {
			syncMove();
		}
	} else {
		qWarning()<<"ERROR: Trying to move servo with serial when not connected";
	}

}
