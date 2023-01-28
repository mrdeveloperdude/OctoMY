#include "Servotor32Controller.hpp"

#include "hardware/serial/SerialSettings.hpp"
#include "hardware/controllers/servotor32/Servotor32ControllerWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

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
	OC_METHODGATE();
	qRegisterMetaType<QSerialPort::SerialPortError>();
	setHookSignals(true);

}

Servotor32Controller::~Servotor32Controller()
{
	OC_METHODGATE();
	// ASIMOV: Limp all actuators before closing shop to avoid frying them if they are trying to reach impossible positions
	limpAll();
}

void Servotor32Controller::configure(SerialSettings &serialSettings)
{
	OC_METHODGATE();
	qDebug()<<"Servotor32Controller configured with new serial settings: "<< serialSettingsToString (serialSettings);
	closeSerialPort();
	mSerialSettings=serialSettings;
	openSerialPort();
	writeData("\n");
}


void Servotor32Controller::setHookSignals(bool hook)
{
	OC_METHODGATE();
	if(hook) {
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
		if(!connect(&mSerialSettings, SIGNAL(settingsChanged()), this, SLOT(onSerialSettingsChanged()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		*/
	} else {
		if(!disconnect(mSerialInterface, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onSerialHandleError(QSerialPort::SerialPortError)))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		if(!disconnect(mSerialInterface, SIGNAL(readyRead()), this, SLOT(onSerialReadData()))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		if(!disconnect(mSerialInterface, SIGNAL(bytesWritten(qint64)), this, SLOT(onSerialDataWritten(qint64)))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		/*
		if(!disconnect(&mSerialSettings, SIGNAL(settingsChanged()), this, SLOT(onSerialSettingsChanged()))) {
			qWarning()<<"ERROR: Could not disconnect";
		}
		*/
	}
}

void Servotor32Controller::openSerialPort()
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	if(isConnected()) {
		QString data;
		//qDebug()<<"SYNC-MOVE: "<< QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		const int sz=mAccumulatedPosition.size();
		for(int i=0; i<sz; ++i) {
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
	OC_METHODGATE();
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


static const QString sepStr("\n\r");
static const size_t sepStrSz=sepStr.size();

static const QByteArray sepBa(sepStr.toLatin1());
static const size_t sepBaSz=sepBa.size();


// Serial IO slots
//////////////////////////////////////////////

void Servotor32Controller::onSerialReadData()
{
	OC_METHODGATE();
	if(isConnected()) {
		QByteArray data = mSerialInterface->readAll();
		mInputBuffer.append(data);
		const int lio=mInputBuffer.indexOf(sepBa);
		mReads++;
		// Output non-empty lines
		if(lio > 0) {
			QByteArray part=mInputBuffer.left(lio);
			mInputBuffer=mInputBuffer.mid(lio+sepBaSz);
			qDebug()<<QString::fromLatin1(part)<< " (over "<<mReads<<" reads)";
			mReads=0;
		}
		// Simply skip empty lines
		else if(0 == lio) {
			qDebug()<<"SKIPPING EMPTY SERIAL IO READ";
			mInputBuffer=mInputBuffer.mid(sepBaSz);
		}
		//TODO:parse and handle data
	} else {
		qWarning()<<"ERROR: Trying to read data from serial when not connected";
	}
}



void Servotor32Controller::onSerialDataWritten( qint64 written)
{
	OC_METHODGATE();
	Q_UNUSED(written);
	qint64 left=mSerialInterface->bytesToWrite();
	//qDebug()<<"DATA WRITTEN: "<<written <<" (left:"<<left<<") ";
	if(left<=0 && mDirtyMoveFlags.count(true)) {
		syncMove();
	}
}


void Servotor32Controller::onSerialHandleError(QSerialPort::SerialPortError error)
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	if(open) {
		openSerialPort();
		qDebug()<<"OPENED servotor32 version "<<version();
	} else {
		closeSerialPort();
	}
}

bool Servotor32Controller::isConnected()
{
	OC_METHODGATE();
	return mSerialInterface->isOpen();
}


void Servotor32Controller::limp(QBitArray &flags)
{
	OC_METHODGATE();
	//Trivial reject: limp ALL
	if(isConnected()) {
		const int sz=flags.size();
		if(flags.count(true)==sz) {
			limpAll();
		} else {
			QString data;
			for(int i=0; i<sz; ++i) {
				if(flags.testBit(i)) {
					data += QString::fromLatin1("#") +  QString::number(i) + QString::fromLatin1("L\n");
				} else {
					data += QString::fromLatin1("#") +  QString::number(i) + QString::fromLatin1("P") + QString::number(mAccumulatedPosition[i]) + QString::fromLatin1("\n");
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
	OC_METHODGATE();
	if(isConnected()) {
		if(index>actuatorCount()) {
			return;
		}
		QString data;
		if(limp) {
			data += QString::fromLatin1("#") +  QString::number(index) + QString::fromLatin1("L\n");
		} else {
			data += QString::fromLatin1("#") +  QString::number(index) + QString::fromLatin1("P") + QString::number(mAccumulatedPosition[index]) + QString::fromLatin1("\n");
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
	OC_METHODGATE();
	//qreal pos[1]= {0.0};	quint32 flags;
	if(isConnected()) {
		const int sz=static_cast<int>(pose.size());
		if(mAccumulatedPosition.size()<sz) {
			mAccumulatedPosition.resize(sz);
			mDirtyMoveFlags.resize(sz);
		}
		for(int i=0; i<sz; ++i) {
			// TODO: Fix float vs int issue
			const quint32 p=(qBound(-1.0, pose.value(i), 1.0)*1000.0+1500.0);
			//Skip unecessary communication if value did not change
			if(!qFuzzyCompare(mAccumulatedPosition[i], p)) {
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
	OC_METHODGATE();
	const quint32 p=(qBound(-1.0, value, 1.0)*1000.0+1500.0);
	// Skip unecessary communication if value did not change
	// TODO: Fix float vs int problem
	if(mAccumulatedPosition[i]!=p) {
		mAccumulatedPosition[i]=p;
		if(0==mSerialInterface->bytesToWrite()) {
			syncMove();
		}
	}
}

void Servotor32Controller::limpAll()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("K\n");
	} else {
		qWarning()<<"ERROR: Trying to limp all actuators via serial when not connected";
	}
}



void Servotor32Controller::centerAll()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("C\n");
	} else {
		qWarning()<<"ERROR: Trying to center actuators using serial when not connected";
	}
}

QString Servotor32Controller::version()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("V\n");
	} else {
		qWarning()<<"ERROR: Trying to get version via serial when not connected";
	}
	return "TODO: FIX ME";
}


void Servotor32Controller::debug()
{
	OC_METHODGATE();
	if(isConnected()) {
		writeData("D\n");
	} else {
		qWarning()<<"ERROR: Trying to debug with serial when not connected";
	}

}





quint8 Servotor32Controller::maxActuatorsSupported()
{
	OC_METHODGATE();
	return 32;
}



quint8 Servotor32Controller::actuatorCount()
{
	OC_METHODGATE();
	return static_cast<quint8>(mAccumulatedPosition.size());
}


QString Servotor32Controller::actuatorName(quint8)
{
	OC_METHODGATE();
	//TODO: Implement
	return "IMPLEMENT ME!";
}

qreal Servotor32Controller::actuatorValue(quint8 index)
{
	OC_METHODGATE();
	if(index>=mAccumulatedPosition.size()) {
		return 0.0;
	}
	return mAccumulatedPosition[index];
}




qreal Servotor32Controller::actuatorDefault(quint8 )
{
	OC_METHODGATE();
	//There is no concept of default value so we simply say center
	return 0.5;
}




QWidget *Servotor32Controller::configurationWidget()
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	QVariantMap map;
	return map;

}

void Servotor32Controller::setConfiguration(QVariantMap &configuration)
{
	OC_METHODGATE();
	Q_UNUSED(configuration);
}
