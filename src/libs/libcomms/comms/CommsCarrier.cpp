#include "CommsCarrier.hpp"

#include "utility/Standard.hpp"

#include "basic/AddressList.hpp"

#include <QHostAddress>
#include <QDateTime>

quint64 CommsCarrier::sTotalRecCount = 0;
quint64 CommsCarrier::sTotalTxCount = 0;


CommsCarrier::CommsCarrier(QObject *parent)
	: QObject(parent)
	, mConnected(false)
	, mRXRate("RX RATE")
	, mTXRate("TX RATE")
	, mTXOpportunityRate("OP RATE")

{
	OC_METHODGATE();
	setObjectName("ERROR: INSTANCIATED PURE VIRTUAL CLASS SOMEHOW");
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
	qDebug()<<"DETECTIN' connected=" << mConnected << "timeSinceLastRX=" <<  timeSinceLastRX << " vs. timeout="<<timeout<< " now="<<now;
	if(mConnected && (timeSinceLastRX > timeout) ) {
		qDebug()<<"Connection timed out, stopping";
		stop();
	} else if(!mConnected && (timeSinceLastRX <= timeout) ) {
		mConnected=true;
		qDebug()<<"Connection completed";
		emit carrierConnectionStatusChanged(true);
	}
}



void CommsCarrier::onOpportunityTimer()
{
	OC_METHODGATE();
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	detectConnectionChanges(now);
	mTXOpportunityRate.countPacket(0);
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

bool CommsCarrier::start(NetworkAddress address)
{
	if(isStarted()) {
		stop();
	}
	const bool success=startImp(address);
	if(success) {
		mSendingTimer.start();
	} else {
		stop();
	}
	return success;
}

void CommsCarrier::stop()
{
	mConnected=false;
	mSendingTimer.stop();
	stopImp();
	emit carrierConnectionStatusChanged(false);
}

bool CommsCarrier::isStarted() const
{
	const bool isToo=isStartedImp();
	return (mSendingTimer.isActive() && isToo);
}

bool CommsCarrier::isConnected() const
{
	return mConnected;
}


qint64 CommsCarrier::writeData(const QByteArray &datagram, const NetworkAddress &address)
{
	const qint64 len=writeDataImp(datagram, address);
	mTXRate.countPacket(len);
	sTotalRecCount++;
	return len;
}

qint64 CommsCarrier::readData(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	const qint64 len=readDataImp(data,maxlen,host,port);
	mRXRate.countPacket(len);
	sTotalTxCount++;
	return len;
}

bool CommsCarrier::hasPendingData()
{
	return hasPendingDataImp();
}

qint64 CommsCarrier::pendingDataSize()
{
	return pendingDataSizeImp();
}


QString CommsCarrier::errorString()
{
	return errorStringImp();
}


NetworkAddress CommsCarrier::address()
{
	return addressImp();
}


quint64 CommsCarrier::minimalPacketInterval()
{
	return minimalPacketIntervalImp();
}


quint64 CommsCarrier::maximalPacketInterval()
{
	return maximalPacketIntervalImp();
}


quint64 CommsCarrier::setDesiredOpportunityInterval(quint64 desiredInterval)
{
	const quint64 actualInterval=qBound(maximalPacketInterval(), desiredInterval, minimalPacketInterval());
	//qDebug()<<"NEW SCHEDULE DELAY: "<<actualInterval<<" ("<<desiredInterval<<")";
	mSendingTimer.setInterval(actualInterval);
	return actualInterval;
}

quint64 CommsCarrier::opportunityInterval()
{
	return mSendingTimer.interval();
}
