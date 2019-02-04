#include "CommsCarrier.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "utility/time/HumanTime.hpp"

#include "address/AddressList.hpp"

#include "CommsCarrier.hpp"

#include <QHostAddress>
#include <QDateTime>

Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QHostAddress *)

quint64 CommsCarrier::sTotalRxCount = 0;
quint64 CommsCarrier::sTotalTxCount = 0;

CommsCarrier::CommsCarrier(QObject *parent)
	: QObject(parent)
	, mConnected(false)
	, mRXRate("RX RATE", 10000)
	, mTXRate("TX RATE", 10000)
	, mTXOpportunityRate("OP RATE")

{
	OC_METHODGATE();
	mSendingTimer.setSingleShot(false);
	mSendingTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&mSendingTimer, SIGNAL(timeout()), this, SLOT(onOpportunityTimer()), OC_CONTYPE)) {
		qWarning()<<"Could not connect sending timer";
	}
}


CommsCarrier::~CommsCarrier()
{
	OC_METHODGATE();
}



void CommsCarrier::detectConnectionChanges(const quint64 now)
{
	OC_METHODGATE();
	const quint64 timeSinceLastRX = (now - mRXRate.mLast);
	const quint64 timeout = connectionTimeout();
	qDebug().nospace().noquote()
			<< ("DETECTIN' timeSinceLastRX=")
			<< timeSinceLastRX
			<< (", timeout=")
			<< timeout
			<< (", now=")
			<< now
			<< (", mRXRate=")
			<< mRXRate
			//<< (", mConnected=")
			//<< (mConnected?"YES":"NO");
			   ;
	const bool old=mConnected;
	if(mConnected && (timeSinceLastRX > timeout) ) {
		mConnected=false;
		qDebug()<<" #!#!#!# Connection timed out";
	} else if(!mConnected && (timeSinceLastRX <= timeout) ) {
		mConnected=true;
		qDebug()<<" #!#!#!# Connection completed";
	}
	if(old!=mConnected) {
		emit carrierConnectionStatusChanged(mConnected);
	}
}



void CommsCarrier::onOpportunityTimer()
{
	OC_METHODGATE();
	qDebug()<<"Opportunity timer";
	const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
	mTXOpportunityRate.countPacket(0, now);
	detectConnectionChanges(now);
	emit carrierSendingOpportunity(now);
}


quint64 CommsCarrier::connectionTimeout()
{
	OC_METHODGATE();
	// 1 sec  more than our minimal packet rate
	return minimalPacketInterval() + 1000;
}


void CommsCarrier::setHookCarrierSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	//qRegisterMetaType<CommsSession *>("Client *");
	if(hook) {
		if(!connect(this,SIGNAL(carrierReadyRead()),&ob,SLOT(onCarrierReadyRead()),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(carrierError(QString)),&ob,SLOT(onCarrierError(QString)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(carrierSendingOpportunity(quint64)),&ob,SLOT(onCarrierSendingOpportunity(quint64)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(carrierConnectionStatusChanged(bool)),&ob,SLOT(onCarrierConnectionStatusChanged(bool)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(this,SIGNAL(carrierReadyRead()),&ob,SLOT(onCarrierReadyRead()))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(carrierError(QString)),&ob,SLOT(onCarrierError(QString)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(carrierSendingOpportunity(quint64)),&ob,SLOT(onCarrierSendingOpportunity(quint64)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(carrierConnectionStatusChanged(bool)),&ob,SLOT(onCarrierConnectionStatusChanged(bool)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}



// CommsCarrier internal interface methods
///////////////////////////////////////////////////////////



void CommsCarrier::setListenAddress(NetworkAddress address)
{
	OC_METHODGATE();
	setAddressImp(address);
}

bool CommsCarrier::setStarted(bool start)
{
	OC_METHODGATE();
	bool success=true;
	if(start) {
		if(isStarted()) {
			setStarted(false);
		}
		success=setStartImp(true);
		if(success) {
			mSendingTimer.start();
		} else {
			setStarted(false);
		}
	} else {
		mSendingTimer.stop();
		setStartImp(false);
		mConnected=false;
		emit carrierConnectionStatusChanged(false);
	}
	return success;
}


bool CommsCarrier::isStarted() const
{
	OC_METHODGATE();
	const bool isToo=isStartedImp();
	return (mSendingTimer.isActive() && isToo);
}

bool CommsCarrier::isConnected() const
{
	OC_METHODGATE();
	return mConnected;
}


qint64 CommsCarrier::writeData(const QByteArray &datagram, const NetworkAddress &address)
{
	OC_METHODGATE();
	const qint64 len=writeDataImp(datagram, address);
	mTXRate.countPacket(len);
	sTotalRxCount++;
	return len;
}

qint64 CommsCarrier::readData(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	OC_METHODGATE();
	const qint64 len=readDataImp(data,maxlen,host,port);
	mRXRate.countPacket(len);
	sTotalTxCount++;
	return len;
}

bool CommsCarrier::hasPendingData()
{
	OC_METHODGATE();
	return hasPendingDataImp();
}

qint64 CommsCarrier::pendingDataSize()
{
	OC_METHODGATE();
	return pendingDataSizeImp();
}


QString CommsCarrier::errorString()
{
	OC_METHODGATE();
	return errorStringImp();
}


NetworkAddress CommsCarrier::address()
{
	OC_METHODGATE();
	return addressImp();
}


quint64 CommsCarrier::minimalPacketInterval()
{
	OC_METHODGATE();
	return minimalPacketIntervalImp();
}


quint64 CommsCarrier::maximalPacketInterval()
{
	OC_METHODGATE();
	return maximalPacketIntervalImp();
}


quint64 CommsCarrier::setDesiredOpportunityInterval(quint64 desiredInterval)
{
	OC_METHODGATE();
	const quint64 actualInterval=qBound(maximalPacketInterval(), desiredInterval, minimalPacketInterval());
	//qDebug()<<"NEW SCHEDULE DELAY: "<<actualInterval<<" ("<<desiredInterval<<")";
	mSendingTimer.setInterval(actualInterval);
	return actualInterval;
}

quint64 CommsCarrier::opportunityInterval()
{
	OC_METHODGATE();
	return mSendingTimer.interval();
}
