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
	: signature(signature)
	, log(log)
	, lastSendTime(0)
	, lastReceiveTime(lastSendTime)
	, connected(false)
	, lastConnected(false)
	, timeoutAccumulator(0.0f)
	, deltaTime(0.0f)
	, idleAccumulator(0.0)
	, idlePacketsSent(0)
{

}

void Client::countSend(qint64 written)
{
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	if(lastSendTime<=0) {
		lastSendTime=now-1;
	}
	const qint64 delta=now-lastSendTime;
	lastSendTime=now;
	deltaTime=delta/1000.0f;
	timeoutAccumulator += deltaTime;
	if ( timeoutAccumulator > TIMEOUT_TRESHOLD ) {
		qDebug()<<"CLIENT "<< signature.toString() <<" timed out";
		connected=false;
	}
	if(connected) {
		reliabilitySystem.update(deltaTime);
		flowControl.update( deltaTime, reliabilitySystem.roundTripTime() * 1000.0f );
	}
	if(connected!=lastConnected) {
		lastConnected=connected;
		flowControl.reset();
		appendLog("CLIENT: New flow state: " +QString(connected?"CONNECTED":"DISCONNECTED")+ " for "+signature.toString());
	}
	reliabilitySystem.packetSent(written);
}

void Client::receive()
{
	lastReceiveTime=QDateTime::currentMSecsSinceEpoch();
	connected=true;
	timeoutAccumulator = 0.0f;
}

void Client::appendLog(QString msg)
{
	if(0!=log) {
		log->appendLog(msg);
	}
}

bool Client::idle()
{
	const float sendRate = flowControl.sendRate();
	if( deltaTime > (1.0f / sendRate) ) {
		appendLog("SENDING IDLE PACKET "+QString::number(idlePacketsSent));
		idleAccumulator=0.0f;
		idlePacketsSent++;
		return connected;
	}
	return false;
}


QString Client::getSummary(QString sep) const
{
	QString out;
	QTextStream ts(&out);
	ts << "ID: "<< signature.shortHandID();
	ts << ", NET: "<<signature.address().toString();
	ts << ", DELTA: "<<deltaTime<<sep;
	ts << ", TOA: "<<timeoutAccumulator<<sep;
	ts << flowControl.getSummary(sep);
	return out;
}

QString Client::toString() const
{
	QString out;
	QTextStream ts(&out);
	ts << "sig="<< signature.toString()<<", delta: "<<deltaTime<<", TOA: "<<timeoutAccumulator;
	return out;
}


const QString Client::getListText() const
{
	QString name=getSummary(" - ");
	return name;
}

/*
quint64 Client::getHash() const{
	return (quint64)signature.executable| ((quint64)signature.platform <<32);
}
*/

const quint64 Client::lastActiveTime() const
{
	// Sending does not count, because we may be sending to deaf ears
	//return qMax(lastSendTime, lastReceiveTime);
	return lastReceiveTime;
}

quint64 Client::getShortHandID() const
{
	return signature.shortHandID();
}
////////////////////////////



