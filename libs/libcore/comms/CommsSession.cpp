#include "CommsSession.hpp"

#include "basic/LogDestination.hpp"
#include "CommsChannel.hpp"
#include "security/Key.hpp"


#include <QDateTime>


/*

  Node

   + Agent -> AgentControls -> CourierSet

   + Remote -> ClientWidget

  CommsChannel

  CommsClient

  CommsClientDirectory

  CommsClientSignature

  CommsSession - rogue - handshake(exchange full ID and 64bit nonceID, agree on security protocols and bandwidth limits)

  NodeAssociate

  ReliabilitySystem

  FlowControl

  Courier

	A packet is defined as initial when it contains a session ID of 0
	A packet is defined as broken when it does not adhere to the protocol by displaying a lack/excess of data or data in the wrong format
	A packet is defined as whole when it is not broken
	A packet is defined as hacked when it is whole according to comms protocol but broken according to tamper protocol

	Comms protocol is the language of OctoMY™ in network communication.
	Tamper protocol is an extra layer of protection beside comms protocol to detect attempts to tamper with communications. It has validation checks that are not necessary by the comms protocol, but add tells to the authenticity of the data (would the real implementation send data in this way)?

	A valid handshake to start a new session looks like this (similar in concept to the 3-way TCP handshake):

	1. A sends SYN to B:
		Hi B. Here is my FULL-ID + NONCE ENCODED WITH YOUR PUBKEY.

	2. B answers SYN-ACK to A:
		Hi A. HERE IS MY FULL-ID + SAME NONCE + NEW NONCE ENCODED WITH YOUR PUBKEY back atch'a.

	3. A answers ACK to B:
		Hi again B. FULL-ID + NONCES WELL RECEIVED.

	At this point session is established


	Comms have two parts, the intrinsic part and the courier part.
	The courier part is reserved for the application layer that wish to conduct communication using comms channel.
	The intrinsic part is reserved for internal affairs of commschannel.
	Intrinsic and courier parts of commschannel should not depend directly on one another and their respective implementation should not be mixed.

	Intrinsic parts of comms include the following:

	 + Session management
		 + Session initiation/handshake
		 + Exchange of transmission control data
		 + Session tear-down
	 + Bandwidth management
		 + Detection of available bandwidth
		 + Throttling to avoid excessive bandwidth use
		 + Continuous monitoring and ajustment of protocol parameters optimize flow of packets (including the priority and timing of couriers)
	 + Encryption management
		 + Signing and sign verification of un-encrypted protocol text based on client RSA keypairs
		 + Generation and exchange of encryption keys based on client RSA keypairs
		 + Generation of security primitives such as nonces
		 + Encryption and decryption of protocol text
	 + Reliability management
		 + Maintaining of continued UDP connection over unreliable network components such as consumer grade routers by the dispatch of necessary communication with STUN services, or sending of antler packets.
		 + Detection and removal of duplicate and corrupt packets
		 + Reordering of ordered packet sequences
		 + Detection and re-sending of missing packets



	Please note that the the expensive and complex intrinsic features such as
	relibability and encryption of CommsChannel are invoked only when needed.

	When the amount of data needed for intrinsic features is extensive, separate
	"intrinsic packets" will be sent, while other lesse in size intrinsic data
	such as counters will instead accompany each packet. Protocol dictates when
	such dedicated packets will be needed or not, and changes in this part of the
	protocol should not affect the higher level courier interface.

*/



///////////////////////////////////////////////////////////////////////////

//Client::Client(QHostAddress host, quint16 port, LogDestination *log):
//	host(host)
//  , port(port)
//  , hash(generateHash(host,port))

CommsSession::CommsSession(CommsSignature signature, Key &key)
	: mSignature(signature)
	, mKey(key)
	, mLastSendTime(0)
	, mLastReceiveTime(mLastSendTime)
	, mConnected(false)
	, mLastConnected(false)
	, mDisconnectTimeoutAccumulator(0.0f)
	, mDisconnectTimeout(3.0)
	, mExpireTimeoutAccumulator(0)
	, mExpireTimeout(5*60*1000) // 5 minutes of inactivity means the session may be pruned (by default, value may change)
	, mDeltaTime(0.0f)
	, mIdleAccumulator(0.0)
	, mIdlePacketsSent(0)
	, mExpired(false)
	, mEstablished(false)

{

}


CommsSession::~CommsSession()
{

}


CommsSignature &CommsSession::signature()
{
	return mSignature;
}

ReliabilitySystem &CommsSession::reliabilitySystem()
{
	return mReliabilitySystem;
}

FlowControl &CommsSession::flowControl()
{
	return mFlowControl;
}
qint64 CommsSession::lastSendTime()
{
	return mLastSendTime;
}

qint64 CommsSession::lastRecieveTime()
{
	return mLastReceiveTime;
}

bool CommsSession::connected()
{
	return mConnected;
}

bool CommsSession::lastConnected()
{
	return mLastConnected;
}

float CommsSession::timeoutAccumulator()
{
	return mDisconnectTimeoutAccumulator;
}
float CommsSession::deltaTime()
{
	return mDeltaTime;
}

float CommsSession::idleAccumulator()
{
	return mIdleAccumulator;
}
quint32 CommsSession::idlePacksSent()
{
	return mIdlePacketsSent;
}


Key &CommsSession::key()
{
	return mKey;
}


bool CommsSession::expired()
{
	return mExpired;
}


bool CommsSession::established()
{
	return mEstablished;
}


void CommsSession::setExpired()
{
	mExpired=true;
}



void CommsSession::setEstablished()
{
	mEstablished=true;
}

void CommsSession::countSend(qint64 written)
{
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	if(mLastSendTime<=0) {
		mLastSendTime=now-1;
	}
	const qint64 delta=now-mLastSendTime;
	mLastSendTime=now;
	mDeltaTime=delta/1000.0f;
	mDisconnectTimeoutAccumulator += mDeltaTime;
	mExpireTimeoutAccumulator+=(qint64)qMax((qint64)0, delta);
	if (mConnected && ( mDisconnectTimeoutAccumulator > mDisconnectTimeout )) {
		qDebug()<<"SESSION "<< mSignature.toString() <<" disconnected";
		mConnected=false;
	}
	if (!mExpired && ( mExpireTimeoutAccumulator > mExpireTimeout )) {
		qDebug()<<"SESSION "<< mSignature.toString() <<" expired";
		mExpired=true;
	}
	if(mConnected) {
		mReliabilitySystem.update(mDeltaTime);
		mFlowControl.update( mDeltaTime, mReliabilitySystem.roundTripTime() * 1000.0f );
	}
	if(mConnected!=mLastConnected) {
		mLastConnected=mConnected;
		mFlowControl.reset();
		qDebug()<<"SESSION: New flow state: " <<(mConnected?"CONNECTED":"DISCONNECTED")<< " for "<<mSignature.toString();
	}
	mReliabilitySystem.packetSent(written);
}

void CommsSession::receive()
{
	mLastReceiveTime=QDateTime::currentMSecsSinceEpoch();
	mConnected=true;
	mDisconnectTimeoutAccumulator = 0.0f;
	mExpireTimeoutAccumulator=0;
}

bool CommsSession::idle()
{
	const float sendRate = mFlowControl.sendRate();
	if( mDeltaTime > (1.0f / sendRate) ) {
		qDebug()<<"SENDING IDLE PACKET "<<mIdlePacketsSent;
		mIdleAccumulator=0.0f;
		mIdlePacketsSent++;
		return mConnected;
	}
	return false;
}


QString CommsSession::summary(QString sep) const
{
	QString out;
	QTextStream ts(&out);
	ts << "ID: "<< mSignature.shortHandID();
	ts << ", NET: "<<mSignature.address().toString();
	ts << ", DELTA: "<<mDeltaTime<<sep;
	ts << ", TOA: "<<mDisconnectTimeoutAccumulator<<sep;
	ts << mFlowControl.toString(sep);
	return out;
}

QString CommsSession::toString() const
{
	QString out;
	QTextStream ts(&out);
	ts << "sig="<< mSignature.toString()<<", delta: "<<mDeltaTime<<", TOA: "<<mDisconnectTimeoutAccumulator;
	return out;
}


const QString CommsSession::listText() const
{
	QString name=summary(" - ");
	return name;
}

quint64 CommsSession::lastActiveTime() const
{
	// Sending does not count, because we may be sending to deaf ears
	//return qMax(lastSendTime, lastReceiveTime);
	return mLastReceiveTime;
}

quint64 CommsSession::getShortHandID() const
{
	return mSignature.shortHandID();
}
////////////////////////////
