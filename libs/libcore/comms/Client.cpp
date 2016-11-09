#include "Client.hpp"

#include "basic/LogDestination.hpp"
#include "CommsChannel.hpp"


#include <QDateTime>



///////////////////////////////////////////////////////////////////////////

//Client::Client(QHostAddress host, quint16 port, LogDestination *log):
//	host(host)
//  , port(port)
//  , hash(generateHash(host,port))

Client::Client(ClientSignature signature, LogDestination *log)
	: mSignature(signature)
	, mLog(log)
	, mLastSendTime(0)
	, mLastReceiveTime(mLastSendTime)
	, mConnected(false)
	, mLastConnected(false)
	, mTimeoutAccumulator(0.0f)
	, mDeltaTime(0.0f)
	, mIdleAccumulator(0.0)
	, mIdlePacketsSent(0)
{

}

void Client::countSend(qint64 written)
{
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	if(mLastSendTime<=0) {
		mLastSendTime=now-1;
	}
	const qint64 delta=now-mLastSendTime;
	mLastSendTime=now;
	mDeltaTime=delta/1000.0f;
	mTimeoutAccumulator += mDeltaTime;
	if ( mTimeoutAccumulator > TIMEOUT_TRESHOLD ) {
		qDebug()<<"CLIENT "<< mSignature.toString() <<" timed out";
		mConnected=false;
	}
	if(mConnected) {
		mReliabilitySystem.update(mDeltaTime);
		mRlowControl.update( mDeltaTime, mReliabilitySystem.roundTripTime() * 1000.0f );
	}
	if(mConnected!=mLastConnected) {
		mLastConnected=mConnected;
		mRlowControl.reset();
		appendLog("CLIENT: New flow state: " +QString(mConnected?"CONNECTED":"DISCONNECTED")+ " for "+mSignature.toString());
	}
	mReliabilitySystem.packetSent(written);
}

void Client::receive()
{
	mLastReceiveTime=QDateTime::currentMSecsSinceEpoch();
	mConnected=true;
	mTimeoutAccumulator = 0.0f;
}

void Client::appendLog(QString msg)
{
	if(0!=mLog) {
		mLog->appendLog(msg);
	}
}

bool Client::idle()
{
	const float sendRate = mRlowControl.sendRate();
	if( mDeltaTime > (1.0f / sendRate) ) {
		appendLog("SENDING IDLE PACKET "+QString::number(mIdlePacketsSent));
		mIdleAccumulator=0.0f;
		mIdlePacketsSent++;
		return mConnected;
	}
	return false;
}


QString Client::summary(QString sep) const
{
	QString out;
	QTextStream ts(&out);
	ts << "ID: "<< mSignature.shortHandID();
	ts << ", NET: "<<mSignature.address().toString();
	ts << ", DELTA: "<<mDeltaTime<<sep;
	ts << ", TOA: "<<mTimeoutAccumulator<<sep;
	ts << mRlowControl.getSummary(sep);
	return out;
}

QString Client::toString() const
{
	QString out;
	QTextStream ts(&out);
	ts << "sig="<< mSignature.toString()<<", delta: "<<mDeltaTime<<", TOA: "<<mTimeoutAccumulator;
	return out;
}


const QString Client::listText() const
{
	QString name=summary(" - ");
	return name;
}

/*
quint64 Client::getHash() const{
	return (quint64)signature.executable| ((quint64)signature.platform <<32);
}
*/
quint64 Client::lastActiveTime() const
{
	// Sending does not count, because we may be sending to deaf ears
	//return qMax(lastSendTime, lastReceiveTime);
	return mLastReceiveTime;
}

quint64 Client::getShortHandID() const
{
	return mSignature.shortHandID();
}
////////////////////////////


