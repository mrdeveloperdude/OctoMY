#include "Servotor32Controller.hpp"



#include "SerialSettingsWidget.hpp"
#include "../libutil/utility/Standard.hpp"


#include <QDebug>
#include <QtGlobal>
#include <QStringBuilder>
#include <QBitArray>


Servotor32Controller::Servotor32Controller(QObject *parent)
	: IServoController("Servotor32", parent)
	, mSerialSettings(new SerialSettingsWidget)
	, mSerialInterface(new QSerialPort(this))
	, mReads(0)
{

	qRegisterMetaType<QSerialPort::SerialPortError>();
	if(!connect(mSerialInterface, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onSerialHandleError(QSerialPort::SerialPortError)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(mSerialInterface, SIGNAL(readyRead()), this, SLOT(onSerialReadData()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(mSerialInterface, SIGNAL(bytesWritten(qint64)), this, SLOT(onSerialDataWritten(qint64)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(mSerialSettings,SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}

Servotor32Controller::~Servotor32Controller()
{
	//ASIMOV: Limp all servos before closing shop to avoid frying them if they are trying to reach impossible positions
	killAll();
	delete mSerialSettings;
}

void Servotor32Controller::configure()
{
	if(nullptr!=mSerialSettings) {
		qDebug()<<"SETTINGS SHOW";
		mSerialSettings->show();
	}
}


void Servotor32Controller::openSerialPort()
{
	SerialSettings p = mSerialSettings->settings();
	mSerialInterface->setPortName(p.name);
	mSerialInterface->setBaudRate(p.baudRate);
	mSerialInterface->setDataBits(p.dataBits);
	mSerialInterface->setParity(p.parity);
	mSerialInterface->setStopBits(p.stopBits);
	mSerialInterface->setFlowControl(p.flowControl);
	if (mSerialInterface->open(QIODevice::ReadWrite)) {
		qDebug()<<tr("Connected to %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits).arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
		emit connectionChanged();
	} else {
		qDebug()<<"ERROR OPENING: "<<mSerialInterface->errorString();
	}
}

void Servotor32Controller::closeSerialPort()
{
	if (mSerialInterface->isOpen()) {
		mSerialInterface->close();
		emit connectionChanged();
	}
	qDebug()<<"Disconnected";
}



// NOTE: This will carry out the actual writing when serial signals there is an opportunity.
//	     The data for move is accumulated and consolidated by one or more move() commands.
void Servotor32Controller::syncMove()
{
	if(isConnected()) {
		QString data;
		//qDebug()<<"SYNC-MOVE: "<< QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		const quint32 sz=mAccumulatedPosition.size();
		for(quint32 i=0; i<sz; ++i) {
			if(mDirtyMoveFlags.testBit(i)) {
				data += "#" +QString::number(i) + "P" + QString::number(mAccumulatedPosition[i]) +"\n";
			}
		}
		writeData(data.toLatin1());
		mDirtyMoveFlags.fill(false);
	} else {
		qWarning()<<"ERROR: Trying to syncMove with serial when not connected";
	}
}


void Servotor32Controller::writeData(const QByteArray &data)
{
	if(0==data.size()) {
		qWarning()<<"ERROR: WRITING CALLED WITH NO DATA";
	} else {
		const qint64 accepted=mSerialInterface->write(data);
		if(accepted<0) {
			qWarning()<<"ERROR: write was not accepted. ERROR="<<mSerialInterface->errorString();
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
		QByteArray data = mSerialInterface->readAll();
		mInputBuffer.append(data);
		static const QString sep("\n\r");
		static const quint32 sepSz=sep.size();
		const int lio=mInputBuffer.indexOf(sep);
		mReads++;
		// Output non-empty lines
		if(lio > 0) {
			QByteArray part=mInputBuffer.left(lio);
			mInputBuffer=mInputBuffer.mid(lio+sepSz);
			qDebug()<<QString::fromLatin1(part)<< " (over "<<mReads<<" reads)";
			mReads=0;
		}
		// Simply skip empty lines
		else if(0 == lio) {
			qDebug()<<"SKIPPING EMPTY SERIAL IO READ";
			mInputBuffer=mInputBuffer.mid(sepSz);
		}
		//TODO:parse and handle data
	} else {
		qWarning()<<"ERROR: Trying to read data from serial when not connected";
	}
}



void Servotor32Controller::onSerialDataWritten( qint64 written)
{
	qint64 left=mSerialInterface->bytesToWrite();
	//qDebug()<<"DATA WRITTEN: "<<written <<" (left:"<<left<<") ";
	if(left<=0 && mDirtyMoveFlags.count(true)) {
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
		qDebug()<<"Critical Error"<<mSerialInterface->errorString();
		closeSerialPort();
	}
}

void Servotor32Controller::onSettingsChanged()
{
	{
		QSignalBlocker(this);
		closeSerialPort();
		openSerialPort();
		mSerialSettings->hide();
		//version();
		writeData("\n");
	}
	emit settingsChanged();
}



// IServoController interface
//////////////////////////////////////////////


void Servotor32Controller::setConnected(bool open)
{
	if(open) {
		openSerialPort();
		fetchVersionData();
	} else {
		closeSerialPort();
	}
}

bool Servotor32Controller::isConnected()
{
	return mSerialInterface->isOpen();
}


void Servotor32Controller::kill(QBitArray &flags)
{
	//Trivial reject: kill ALL
	if(isConnected()) {
		const quint32 sz=flags.size();
		if((quint32)flags.count(true)==sz) {
			killAll();
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
		qWarning()<<"ERROR: Trying to kill subset of servos via serial when not connected";
	}
}

// NOTE: This will simply collect the latest data. The actual writing is done in syncMove when serial signals there is an opportunity.
// TODO: look at binary extension introduced in 2.1 version of hexy firmware to improve performance
void Servotor32Controller::move(Pose &pose)
{
	//qreal pos[1]= {0.0};	quint32 flags;
	if(isConnected()) {
		const quint32 sz=pose.size();
		if(mAccumulatedPosition.size()<sz) {
			mAccumulatedPosition.resize(sz);
			mDirtyMoveFlags.resize(sz);
		}
		for(quint32 i=0; i<sz; ++i) {
			const quint32 p=(quint32)(qBound(-1.0, pose.value(i), 1.0)*1000.0+1500.0);
			//Skip unecessary communication if value did not change
			if(mAccumulatedPosition[i]!=p) {
				mAccumulatedPosition[i]=p;
				mDirtyMoveFlags.setBit(i);
			}
		}
		//qDebug()<<"MOVE: flags="<< QString("%1").arg( flags, 16, 2, QChar('0'))<< ", dirtyMoveFlags="<<QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		if(mDirtyMoveFlags.count(true)>0 && 0==mSerialInterface->bytesToWrite()) {
			syncMove();
		}
	} else {
		qWarning()<<"ERROR: Trying to move servo with serial when not connected";
	}
}


void Servotor32Controller::killAll()
{
	if(isConnected()) {
		writeData("K\n");
	} else {
		qWarning()<<"ERROR: Trying to kill all servos via serial when not connected";
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

void Servotor32Controller::fetchVersionData()
{
	if(isConnected()) {
		writeData("V\n");
	} else {
		qWarning()<<"ERROR: Trying to get version via serial when not connected";
	}

}


void Servotor32Controller::fetchDebugData()
{
	if(isConnected()) {
		writeData("D\n");
	} else {
		qWarning()<<"ERROR: Trying to debug with serial when not connected";
	}

}





quint32 Servotor32Controller::maximumServosSupported()
{
	return 32;
}
