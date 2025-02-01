#include "CommsSession.hpp"

#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"
#include "utility/random/Random.hpp"

#include "app/log/LogDestination.hpp"
#include "CommsChannel.hpp"
#include "security/Key.hpp"


#include <QDateTime>

#include <QtGlobal>

///////////////////////////////////////////////////////////////////////////



CommsSession::CommsSession(QSharedPointer<Key> key)
	: mKey(key)
	, mLocalSessionID(INVALID_SESSION_ID) // INVALID_SESSION_ID means no valid session id selected yet
	, mRemoteSessionID(INVALID_SESSION_ID) // INVALID_SESSION_ID means we did not receive a valid session id yet
	, mLastSendTime(0)
	, mLastReceiveTime(mLastSendTime)
	, mDisconnectTimeoutAccumulator(0.0)
	, mDisconnectTimeout(3.0)
	, mExpireTimeoutAccumulator(0)
	, mExpireTimeout(5*60*1000) // 5 minutes of inactivity means the session may be pruned (by default, value may change)
	, mDeltaTime(0.0f)
	, mIdleAccumulator(0.0)
	, mIdlePacketsSent(0)
	, mConnected(false)
	, mLastConnected(false)
	, mExpired(false)
	, mInitialized(false)
	, mOurSynNonce(0)
	, mOurSynAckNonce(0)
	, mOurAckNonce(0)
	, mTheirLastNonce(0)
{
	OC_METHODGATE();
	if(nullptr == key) {
		qWarning()<<"ERROR: key was nullptr";
	}
	// SECURITY: Prevent accidental usage of un-initialized nonce values
	createOurSynNonce();
	createOurSynAckNonce();
	createOurAckNonce();
	mInitialized=true;
}


CommsSession::~CommsSession()
{
	OC_METHODGATE();
}


ReliabilitySystem &CommsSession::reliabilitySystem()
{
	OC_METHODGATE();
	return mReliabilitySystem;
}

FlowControl &CommsSession::flowControl()
{
	OC_METHODGATE();
	return mFlowControl;
}

qint64 CommsSession::lastSendTime() const
{
	OC_METHODGATE();
	return mLastSendTime;
}

qint64 CommsSession::lastRecieveTime() const
{
	OC_METHODGATE();
	return mLastReceiveTime;
}

float CommsSession::timeoutAccumulator() const
{
	OC_METHODGATE();
	// TODO: Normalize types
	return static_cast<float>(mDisconnectTimeoutAccumulator);
}

float CommsSession::deltaTime() const
{
	OC_METHODGATE();
	return mDeltaTime;
}

float CommsSession::idleAccumulator() const
{
	OC_METHODGATE();
	return mIdleAccumulator;
}

quint32 CommsSession::idlePacksSent() const
{
	OC_METHODGATE();
	return mIdlePacketsSent;
}

QSharedPointer<Key> CommsSession::key() const
{
	OC_METHODGATE();
	return mKey;
}

bool CommsSession::connected() const
{
	OC_METHODGATE();
	return mConnected;
}

bool CommsSession::lastConnected() const
{
	OC_METHODGATE();
	return mLastConnected;
}


HandshakeState &CommsSession::handshakeState()
{
	OC_METHODGATE();
	return mHandshakeState;
}

bool CommsSession::established() const
{
	OC_METHODGATE();
	return mHandshakeState.isDone();
}



bool CommsSession::expired() const
{
	OC_METHODGATE();
	return mExpired;
}


SESSION_ID_TYPE CommsSession::localSessionID() const
{
	OC_METHODGATE();
	return mLocalSessionID;
}


SESSION_ID_TYPE CommsSession::remoteSessionID() const
{
	OC_METHODGATE();
	return mRemoteSessionID;
}


QString CommsSession::fullID() const
{
	OC_METHODGATE();
	if(!mInitialized) {
		qWarning()<<"ERROR: !mInitialized in fullID()";
		return "";
	}
	if(nullptr==mKey) {
		qWarning()<<"ERROR: mKey was nullptr in fullID()";
		return "";
	}
	QString id=mKey->id();
	return id;
}

QSharedPointer<CarrierAddress> CommsSession::address() const
{
	OC_METHODGATE();
	return mAddress;
}


void CommsSession::setLocalSessionID(SESSION_ID_TYPE s)
{
	OC_METHODGATE();
	mLocalSessionID=s;
}

void CommsSession::setRemoteSessionID(SESSION_ID_TYPE s)
{
	OC_METHODGATE();
	mRemoteSessionID=s;
}



SESSION_NONCE_TYPE CommsSession::createOurSynNonce()
{
	OC_METHODGATE();
	// SECURITY: Use proper secure prng here
	mOurSynNonce=static_cast<SESSION_NONCE_TYPE>((utility::random::qrand()>>(1<<sizeof(int)))|utility::random::qrand());
	return mOurSynNonce;
}

SESSION_NONCE_TYPE CommsSession::createOurSynAckNonce()
{
	OC_METHODGATE();
	// SECURITY: Use proper secure prng here
	mOurSynAckNonce = static_cast<SESSION_NONCE_TYPE>((utility::random::qrand()>>(1<<sizeof(int)))|utility::random::qrand());
	return mOurSynAckNonce;
}


SESSION_NONCE_TYPE CommsSession::createOurAckNonce()
{
	OC_METHODGATE();
	// SECURITY: Use proper secure prng here
	mOurAckNonce = static_cast<SESSION_NONCE_TYPE>((utility::random::qrand()>>(1<<sizeof(int)))|utility::random::qrand());
	return mOurAckNonce;
}

SESSION_NONCE_TYPE CommsSession::ourSynNonce() const
{
	OC_METHODGATE();
	return mOurSynNonce;
}

SESSION_NONCE_TYPE CommsSession::ourSynAckNonce() const
{
	OC_METHODGATE();
	return mOurSynAckNonce;
}

SESSION_NONCE_TYPE CommsSession::ourAckNonce() const
{
	OC_METHODGATE();
	return mOurAckNonce;
}

void CommsSession::setTheirLastNonce(SESSION_NONCE_TYPE theirNonce)
{
	OC_METHODGATE();
	mTheirLastNonce = theirNonce;
}

SESSION_NONCE_TYPE CommsSession::theirLastNonce() const
{
	OC_METHODGATE();
	return mTheirLastNonce;
}



void CommsSession::setAddress(QSharedPointer<CarrierAddress> address)
{
	OC_METHODGATE();
	mAddress = address;
}

void CommsSession::setExpired()
{
	OC_METHODGATE();
	mExpired = true;
}



void CommsSession::countSend(qint64 written)
{
	OC_METHODGATE();
	const qint64 now=utility::time::currentMsecsSinceEpoch<qint64>();
	if(mLastSendTime<=0) {
		mLastSendTime=now-1;
	}
	const qint64 delta=now-mLastSendTime;
	mLastSendTime=now;
	mDeltaTime=delta/1000.0f;
	// TODO: Normalize qreal vs float vs all the types
	mDisconnectTimeoutAccumulator += static_cast<qreal>(mDeltaTime);
	mExpireTimeoutAccumulator+=static_cast<quint64>(qMax(0LL, delta));
	if (mConnected && ( mDisconnectTimeoutAccumulator > mDisconnectTimeout )) {
		qDebug()<<"SESSION "<< signatureToString() <<" disconnected";
		mConnected=false;
	}
	if (!mExpired && ( mExpireTimeoutAccumulator > mExpireTimeout )) {
		qDebug()<<"SESSION "<< signatureToString() <<" expired";
		mExpired=true;
	}
	if(mConnected) {
		mReliabilitySystem.update(mDeltaTime);
		mFlowControl.update( static_cast<quint64>(mDeltaTime), static_cast<quint64>(mReliabilitySystem.roundTripTime() * 1000.0f) );
	}
	if(mConnected!=mLastConnected) {
		mLastConnected=mConnected;
		mFlowControl.reset();
		qDebug()<<"SESSION: New flow state: " <<(mConnected?"CONNECTED":"DISCONNECTED")<< " for "<<signatureToString();
	}
	mReliabilitySystem.packetSent( static_cast<qint32>(written));
}

void CommsSession::receive()
{
	OC_METHODGATE();
	mLastReceiveTime=utility::time::currentMsecsSinceEpoch<qint64>();
	mConnected=true;
	mDisconnectTimeoutAccumulator = 0.0;
	mExpireTimeoutAccumulator=0;
}

bool CommsSession::idle()
{
	OC_METHODGATE();
	const float sendRate = mFlowControl.sendRate();
	if( mDeltaTime > (1.0f / sendRate) ) {
		qDebug()<<"SENDING IDLE PACKET "<<mIdlePacketsSent;
		mIdleAccumulator=0.0f;
		mIdlePacketsSent++;
		return mConnected;
	}
	return false;
}




const QString CommsSession::signatureToString() const
{
	OC_METHODGATE();
	return QString::number(mLocalSessionID, 16)
		   + "-"
		   + (mAddress.isNull()?"NULL":mAddress->toString())
		   + "("
		   + fullID()
		   + ")";
}

QString CommsSession::summary(QString sep) const
{
	OC_METHODGATE();
	QString out;
	QTextStream ts(&out);
	ts << "ID: " << signatureToString();
	ts << ", NET: " << (mAddress.isNull()?"NULL":mAddress->toString());
	ts << ", DELTA: " << mDeltaTime << sep;
	ts << ", TOA: " << mDisconnectTimeoutAccumulator << sep;
	ts << mFlowControl.toString(sep);
	return out;
}


QString CommsSession::toString() const
{
	OC_METHODGATE();
	QString out;
	QTextStream ts(&out);
	ts
		<< "sig="
		<< QString::number(mLocalSessionID, 16)
		<< "-"
		<< (mAddress.isNull()?"NULL":mAddress->toString())
		<< "("
		<< fullID()
		<< ")"
		<< ", delta: "
		<< mDeltaTime
		<< ", TOA: "
		<< mDisconnectTimeoutAccumulator;
	return out;
}


const QString CommsSession::listText() const
{
	OC_METHODGATE();
	QString name=summary(" - ");
	return name;
}

quint64 CommsSession::lastActiveTime() const
{
	OC_METHODGATE();
	// TODO: See if sending should count, since we may be sending to deaf ears
	return static_cast<quint64>(qMax(mLastSendTime, mLastReceiveTime));
	//return mLastReceiveTime;
}

////////////////////////////



QVariantMap CommsSession::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["lastSendTime"]=mLastSendTime;
	map["lastReceiveTime"]=mLastReceiveTime;
	return map;
}



void CommsSession::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	mLastSendTime=( map["lastSendTime"].toInt());
	mLastReceiveTime=( map["lastReceiveTime"].toInt());
}
