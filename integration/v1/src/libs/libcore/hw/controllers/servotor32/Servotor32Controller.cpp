#include "Servotor32Controller.hpp"

#include "hw/serial/SerialSettings.hpp"
#include "hw/controllers/servotor32/Servotor32ControllerWidget.hpp"
#include "utility/Standard.hpp"


#include <QDebug>
#include <QtGlobal>
#include <QStringBuilder>
#include <QBitArray>
#include <QSerialPort>


Servotor32Controller::Servotor32Controller(QObject *parent)
	: IActuatorController("Servotor32", parent)
	, mReads(0)
	, mWidget(nullptr)
	, mSerialInterface(OC_NEW QSerialPort(this))
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
	/*
	if(!connect(&mSerialSettings,SIGNAL(settingsChanged()), this, SLOT(onSerialSettingsChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	*/
}

Servotor32Controller::~Servotor32Controller()
{
	//ASIMOV: Limp all actuators before closing shop to avoid frying them if they are trying to reach impossible positions
	limpAll();
}

void Servotor32Controller::configure(SerialSettings &serialSettings)
{
	qDebug()<<"Servotor32Controller configured with new serial settings: "<< serialSettingsToString (serialSettings);
	closeSerialPort();
	mSerialSettings=serialSettings;
	openSerialPort();
	writeData("\n");
}


void Servotor32Controller::openSerialPort()
{
	SerialSettings p;
	mSerialInterface->setPortName(p.name);
	mSerialInterface->setBaudRate(p.baudRate);
	mSerialInterface->setDataBits(p.dataBits);
	mSerialInterface->setParity(p.parity);
	mSerialInterface->setStopBits(p.stopBits);
	mSerialInterface->setFlowControl(p.flowControl);
	if (mSerialInterface->open(QIODevice::ReadWrite)) {
		qDebug()<<tr("Connected to %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate()).arg(p.stringDataBits()).arg(p.stringParity()).arg(p.stringStopBits()).arg(p.stringFlowControl());
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

	if (error != QSerialPort::NoError) {
		qDebug()<<"SERVOTOR32 SERIAL ERROR "<<error<<": "<<mSerialInterface->errorString();
	}
	if (error == QSerialPort::ResourceError) {
		qDebug()<<"Critical Error"<<mSerialInterface->errorString();
		closeSerialPort();
	}
}



void Servotor32Controller::onSerialSettingsChanged()
{
	{
		QSignalBlocker(this);
		closeSerialPort();
		openSerialPort();
	}
	emit settingsChanged();
}


// IActuatorController interface
//////////////////////////////////////////////


void Servotor32Controller::setConnected(bool open)
{
	if(open) {
		openSerialPort();
		qDebug()<<"OPENED servotor32 version "<<version();
	} else {
		closeSerialPort();
	}
}

bool Servotor32Controller::isConnected()
{
	return mSerialInterface->isOpen();
}


void Servotor32Controller::limp(QBitArray &flags)
{
	//Trivial reject: limp ALL
	if(isConnected()) {
		const quint32 sz=flags.size();
		if((quint32)flags.count(true)==sz) {
			limpAll();
		} else {
			QString data;
			for(quint32 i=0; i<sz; ++i) {
				if(flags.testBit(i)) {
					data += QLatin1Literal("#") +  QString::number(i) + QLatin1String("L\n");
				} else {
					data += QLatin1Literal("#") +  QString::number(i) + QLatin1String("P") + QString::number(mAccumulatedPosition[i]) + QLatin1String("\n");
				}
			}
			qDebug()<<"LIMP: "<<data<<" for "<<flags;
			writeData(data.toLatin1());
		}
	} else {
		qWarning()<<"ERROR: Trying to limp subset of actuators via serial when not connected";
	}
}



void Servotor32Controller::limp(quint8 index, bool limp)
{
	if(isConnected()) {
		if(index>actuatorCount()) {
			return;
		}
		QString data;
		if(limp) {
			data += QLatin1Literal("#") +  QString::number(index) + QLatin1String("L\n");
		} else {
			data += QLatin1Literal("#") +  QString::number(index) + QLatin1String("P") + QString::number(mAccumulatedPosition[index]) + QLatin1String("\n");
		}
		qDebug()<<"LIMP SINGLE: "<<data;
		writeData(data.toLatin1());
	} else {
		qWarning()<<"ERROR: Trying to limp single atuator via serial when not connected";
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
		qWarning()<<"ERROR: Trying to move actuators with serial when not connected";
	}
}

void Servotor32Controller::move(quint8 i, qreal value)
{
	const quint32 p=(quint32)(qBound(-1.0, value, 1.0)*1000.0+1500.0);
	//Skip unecessary communication if value did not change
	if(mAccumulatedPosition[i]!=p) {
		mAccumulatedPosition[i]=p;
		if(0==mSerialInterface->bytesToWrite()) {
			syncMove();
		}
	}
}

void Servotor32Controller::limpAll()
{
	if(isConnected()) {
		writeData("K\n");
	} else {
		qWarning()<<"ERROR: Trying to limp all actuators via serial when not connected";
	}
}



void Servotor32Controller::centerAll()
{
	if(isConnected()) {
		writeData("C\n");
	} else {
		qWarning()<<"ERROR: Trying to center actuators using serial when not connected";
	}
}

QString Servotor32Controller::version()
{
	if(isConnected()) {
		writeData("V\n");
	} else {
		qWarning()<<"ERROR: Trying to get version via serial when not connected";
	}
	return "TODO: FIX ME";
}


void Servotor32Controller::debug()
{
	if(isConnected()) {
		writeData("D\n");
	} else {
		qWarning()<<"ERROR: Trying to debug with serial when not connected";
	}

}





quint8 Servotor32Controller::maxActuatorsSupported()
{
	return 32;
}



quint8 Servotor32Controller::actuatorCount()
{
	return mAccumulatedPosition.size();
}


QString Servotor32Controller::actuatorName(quint8)
{
	//TODO: Implement
	return "IMPLEMENT ME!";
}

qreal Servotor32Controller::actuatorValue(quint8 index)
{
	if(index>=mAccumulatedPosition.size()) {
		return 0.0f;
	}
	return mAccumulatedPosition[index];
}




qreal Servotor32Controller::actuatorDefault(quint8 )
{
	//There is no concept of default value so we simply say center
	return 0.5f;
}




QWidget *Servotor32Controller::configurationWidget()
{
	if(nullptr==mWidget) {
		mWidget=OC_NEW Servotor32ControllerWidget(nullptr);
		if(nullptr!=mWidget) {
			mWidget->configure(this);
		}
	}
	return mWidget;
}







QVariantMap Servotor32Controller::configuration()
{
	QVariantMap map;
	return map;

}

void Servotor32Controller::setConfiguration(QVariantMap &configuration)
{

}
