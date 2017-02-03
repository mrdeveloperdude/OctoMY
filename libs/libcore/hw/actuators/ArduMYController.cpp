#include "ArduMYController.hpp"

#include "SerialSettingsWidget.hpp"
#include "../arduino/ParserState.hpp"
#include "../libutil/utility/Standard.hpp"
#include "ArduMYControllerWidget.hpp"

#include <QBuffer>
#include <QDebug>

ArduMYController::ArduMYController(QObject *parent)
	: IActuatorController("ArduMY", parent)
	, mSerialInterface(new QSerialPort(this))
	, mSyncDirty(true)
	, mWidget(nullptr)
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
	mSyncTimer.setTimerType(Qt::VeryCoarseTimer);
	mSyncTimer.setInterval(1000);

	if(!connect(&mSyncTimer,&QTimer::timeout, this, [=]() {
	mSyncDirty=true;
	syncData();
	}, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}

ArduMYController::~ArduMYController()
{
	//ASIMOV: Limp all servos before closing shop to avoid frying them if they are trying to reach impossible positions
	limpAll();
}


void ArduMYController::setSerialConfig(SerialSettings settings)
{
	mSerialSettings=settings;
}

void ArduMYController::openSerialPort()
{
	mSerialInterface->setPortName(mSerialSettings.name);
	mSerialInterface->setBaudRate(mSerialSettings.baudRate);
	mSerialInterface->setDataBits(mSerialSettings.dataBits);
	mSerialInterface->setParity(mSerialSettings.parity);
	mSerialInterface->setStopBits(mSerialSettings.stopBits);
	mSerialInterface->setFlowControl(mSerialSettings.flowControl);
	qDebug()<<tr("Trying to connect to %1 : %2, %3, %4, %5, %6").arg(mSerialSettings.name).arg(mSerialSettings.stringBaudRate).arg(mSerialSettings.stringDataBits).arg(mSerialSettings.stringParity).arg(mSerialSettings.stringStopBits).arg(mSerialSettings.stringFlowControl);
	if (mSerialInterface->open(QIODevice::ReadWrite)) {
		emit connectionChanged();
		qDebug()<<"CONNECTION SUCCESSFULL";
		mSyncTimer.start();
	} else {
		qWarning()<<"ERROR OPENING: "<<mSerialInterface->errorString();
	}
}

void ArduMYController::closeSerialPort()
{
	mSyncTimer.stop();
	if (mSerialInterface->isOpen()) {
		mSerialInterface->close();
		emit connectionChanged();
	}
	qDebug()<<"Disconnected";
}



// NOTE: This will carry out the actual writing when serial signals there is an opportunity.
//	     The data for move is accumulated and consolidated by one or more move() commands.
void ArduMYController::syncData()
{
	if(isConnected()) {
		// Sync needed
		if(mSyncDirty) {
			qDebug()<<"SYNC DIRTY ";
			mSyncDirty=false;
		}
		// Count dirty
		if(mCountDirty) {
			qDebug()<<"COUNT DIRTY ";
			mCountDirty=false;
		}
		/*
		// Config dirty
		if(mDirtyConfigFlags.count(true)>0) {
			qDebug()<<"CONFIG DIRTY ";
			mDirtyConfigFlags.fill(false);
		}
		// Kill dirty
		if(mKillDirty) {
			qDebug()<<"KILL DIRTY ";
			mKillDirty=false;
		}
		// Move dirty
		if(mDirtyMoveFlags.count(true)>0) {
			qDebug()<<"MOVE DIRTY ";
			const quint32 sz=mAccumulatedPosition.size();
			QBuffer buf;
			QDataStream stream(&buf);
			stream << mDirtyMoveFlags;
			const quint32 sz2=buf.data().size();
			qDebug()<<"BITS: "<<sz<<" BYTES: "<<sz2;
			for(quint32 i=0; i<sz; ++i) {
				if(mDirtyMoveFlags.testBit(i)) {
					//data += "#" +QString::number(i) + "P" + QString::number(mAccumulatedPosition[i]) +"\n";
				}
			}
			//qDebug()<<"SYNC-MOVE: "<< QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
			mDirtyMoveFlags.fill(false);
		}
		*/
	} else {
		qWarning()<<"ERROR: Trying to syncMove with serial when not connected";
	}
}


void ArduMYController::writeData(const QByteArray &data)
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





// IServoController interface
//////////////////////////////////////////////


void ArduMYController::setConnected(bool open)
{
	if(open) {
		openSerialPort();
		qDebug()<<"Opened ArduMY controller version "<<version();
	} else {
		closeSerialPort();
	}
}

bool ArduMYController::isConnected()
{
	return mSerialInterface->isOpen();
}


void ArduMYController::setServosCount(quint8 ct)
{
	const quint8 max=maxActuatorsSupported();
	if(ct>max) {
		qWarning()<<"ERROR: Tried to set "<<ct<< " servos which is more than the maximum of " <<max;
	} else {
		const quint8 old=mActuators.size();
		if(ct!=old) {
			mActuators.resize(ct);
			if(ct>old) {
				//TODO: Initialize new ones?
			}
			mCountDirty=true;
		}
	}
}


void ArduMYController::limp(QBitArray &flags)
{
	if(isConnected()) {
		quint32 i=0;
		for(ArduMYActuator &actuator:mActuators) {
			const bool k=flags.testBit(i);
			if(k!=actuator.state.isLimp()) {
				mKillDirty=true;
				actuator.state.setLimp(k);
			}
			i++;
		}
	} else {
		qWarning()<<"ERROR: Trying to limp subset of servos via serial when not connected";
	}
}


// NOTE: This will simply collect the latest data. The actual writing is done in syncMove when serial signals there is an opportunity.
// TODO: look at binary extension introduced in 2.1 version of hexy firmware to improve performance
void ArduMYController::move(Pose &pose)
{
	//qreal pos[1]= {0.0};	quint32 flags;
	if(isConnected()) {
		/*
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
			syncData();
		}
		*/
	} else {
		qWarning()<<"ERROR: Trying to move servo with serial when not connected";
	}
}



void ArduMYController::move(quint8 i, qreal value)
{
	/*
	const quint32 p=(quint32)(qBound(-1.0, value, 1.0)*1000.0+1500.0);
	//Skip unecessary communication if value did not change
	if(mAccumulatedPosition[i]!=p) {
		mAccumulatedPosition[i]=p;
		if(0==mSerialInterface->bytesToWrite()) {
			syncMove();
		}
	}
	*/
}


QString ArduMYController::version()
{
	return "TODO: IMPLEMENT ME";
}



quint8 ArduMYController::maxActuatorsSupported()
{
	return 0xFF;
}


quint8 ArduMYController::actuatorCount()
{
	return mActuators.size();
}

QString ArduMYController::actuatorName(quint8)
{
	return "IMPLEMENT ME";
}

qreal ArduMYController::actuatorValue(quint8 index)
{
	if(index>=mActuators.size()) {
		return 0.0f;
	}
	return mActuators[index].state.value.toFloat(mActuators[index].config.representation);
}


qreal ArduMYController::actuatorDefault(quint8)
{
	return 0.0f;
}


QWidget *ArduMYController::configurationWidget()
{
	if(nullptr==mWidget) {
		mWidget=new ArduMYControllerWidget(nullptr);
		if(nullptr!=mWidget) {
			mWidget->configure(this);
		}
	}
	return mWidget;
}






QVariantMap ArduMYController::confiruation()
{
	QVariantMap map;
	return map;

}

void ArduMYController::setConfiguration(QVariantMap &configuration)
{

}























































