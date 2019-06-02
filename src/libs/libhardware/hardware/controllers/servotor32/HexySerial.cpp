#include "HexySerial.hpp"

#include "hardware/serial/SerialSettingsWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include <QDebug>
#include <QtGlobal>
#include <QStringBuilder>


HexySerial::HexySerial(QObject *parent)
	: QObject(parent)
	, settings(nullptr)
	, lastPos{0}
	, dirtyMoveFlags(0xFFFFFFFF)
{
	OC_METHODGATE();
	serial = OC_NEW QSerialPort(this);
	settings = OC_NEW SerialSettingsWidget;
	setHookSignals(true);

}

HexySerial::~HexySerial()
{
	OC_METHODGATE();
	// ASIMOV: Limp servos to avoid frying them if they are trying to reach impossible positions
	kill();

	delete settings;
	settings=nullptr;
	delete serial;
	serial=nullptr;
}

void HexySerial::configure()
{
	OC_METHODGATE();
	if(nullptr!=settings) {
		qDebug()<<"SETTINGS SHOW";
		settings->show();
	}
}

void HexySerial::setHookSignals(bool hook)
{
	OC_METHODGATE();
	if(hook) {
		if(!connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(serial, SIGNAL(readyRead()), this, SLOT(readData()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(serial, SIGNAL(bytesWritten(qint64)), this, SLOT(dataWritten(qint64)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(settings,SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	} else {
		if(!disconnect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		if(!disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		if(!disconnect(serial, SIGNAL(bytesWritten(qint64)), this, SLOT(dataWritten(qint64)))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		if(!disconnect(settings,SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
	}
}

void HexySerial::openSerialPort()
{
	OC_METHODGATE();
	SerialSettings p = settings->settings();
	serial->setPortName(p.name);
	serial->setBaudRate(p.baudRate);
	serial->setDataBits(p.dataBits);
	serial->setParity(p.parity);
	serial->setStopBits(p.stopBits);
	serial->setFlowControl(p.flowControl);
	if (serial->open(QIODevice::ReadWrite)) {
		qDebug()<<tr("Connected to %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate()).arg(p.stringDataBits()).arg(p.stringParity()).arg(p.stringStopBits()).arg(p.stringFlowControl());
		emit connectionChanged();
	} else {
		qDebug()<<"ERROR OPENING: "<<serial->errorString();
	}
}

void HexySerial::closeSerialPort()
{
	OC_METHODGATE();
	if (serial->isOpen()) {
		serial->close();
		emit connectionChanged();
	}
	qDebug()<<"Disconnected";
}


void HexySerial::writeData(const QByteArray &data)
{
	OC_METHODGATE();
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

void HexySerial::readData()
{
	OC_METHODGATE();
	if(isConnected()) {
		QByteArray data = serial->readAll();
		qDebug()<<data;
		//TODO:parse and handle data
	} else {
		qWarning()<<"ERROR: Trying to read data from serial when not connected";
	}
}



void HexySerial::dataWritten( qint64 written)
{
	OC_METHODGATE();
	Q_UNUSED(written);
	//TODO: Why isnt written used?
	qint64 left=serial->bytesToWrite();
	//qDebug()<<"DATA WRITTEN: "<<written <<" (left:"<<left<<") ";
	if(left<=0 && dirtyMoveFlags >0) {
		syncMove();
	}
}


void HexySerial::handleError(QSerialPort::SerialPortError error)
{
	OC_METHODGATE();
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

void HexySerial::onSettingsChanged()
{
	OC_METHODGATE();
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


bool HexySerial::isConnected()
{
	OC_METHODGATE();
	return serial->isOpen();
}

///// Reference: http://arcbotics.com/lessons/servotor32-commands/


void HexySerial::kill(quint32 flags)
{
	OC_METHODGATE();
	//Trivial reject: kill ALL
	if(isConnected()) {
		if(0xFFFFFFFF==flags) {
			writeData("K\n");
		} else {
			QString data;
			quint32 f=1;
			for(quint32 i=0; i<SERVO_COUNT; ++i) {
				if((flags & f)>0) {
					data += QLatin1Literal("#") +  QString::number(i) + QLatin1String("L\n");
				} else {
					//data+="LOL";
				}
				f<<=1;
			}
			qDebug()<<"KILL: "<<data<<" foir "<<flags;
			writeData(data.toLatin1());
		}
	} else {
		qWarning()<<"ERROR: Trying to kill servo via serial when not connected";
	}

}


void HexySerial::center()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("C\n");
	} else {
		qWarning()<<"ERROR: Trying to center servo using serial when not connected";
	}

}


void HexySerial::version()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("V\n");
	} else {
		qWarning()<<"ERROR: Trying to get version via serial when not connected";
	}

}


void HexySerial::debug()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("D\n");
	} else {
		qWarning()<<"ERROR: Trying to debug with serial when not connected";
	}

}


inline qreal fclamp(qreal x, qreal a, qreal b)
{
	OC_FUNCTIONGATE();
	return x < a ? a : (x > b ? b : x);
}


void HexySerial::syncMove()
{
	OC_METHODGATE();
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

//TODO: look at binary extension introduced in 2.1 version of hexy firmware to improve performance
void HexySerial::move(qreal pos[], quint32 flags)
{
	OC_METHODGATE();
	if(isConnected()) {
		for(quint32 i=0; i<SERVO_COUNT; ++i) {
			quint32 flag=(0x1<<i);
			if((flags & flag)>0) {
				const quint32 p=(quint32)(fclamp(pos[i],-1.0,1.0)*1000.0+1500.0);
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
