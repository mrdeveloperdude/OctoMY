#include "Client.hpp"
#include "comms/CommsChannel.hpp"
#include "basic/LogDestination.hpp"


#include <QDateTime>


//Client::Client(QHostAddress host, quint16 port, LogDestination *log):
//	host(host)
//  , port(port)
//  , hash(generateHash(host,port))

Client::Client(quint64 fingerprint, QHostAddress host, quint16 port, LogDestination *log):
	fingerprint(fingerprint)
 , host(host)
   , port(port)
  , log(log)
  , lastSendTime(QDateTime::currentMSecsSinceEpoch())
  , lastReceiveTime(lastSendTime)
  , connected(false)
  , lastConnected(false)
  , timeoutAccumulator(0.0f)
  , deltaTime(0.0f)
  , idleAccumulator(0.0)
  , idlePacketsSent(0)
{

}

void Client::send(qint64 written){
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	const qint64 delta=now-lastSendTime;
	lastSendTime=now;
	deltaTime=delta/1000.0f;
	timeoutAccumulator += deltaTime;
	if ( timeoutAccumulator > TIMEOUT_TRESHOLD ) {
		qDebug()<<"CLIENT "<<fingerprint<<" timed out";
		connected=false;
	}
	if(connected){
		reliabilitySystem.update(deltaTime);
		flowControl.update( deltaTime, reliabilitySystem.roundTripTime() * 1000.0f );
	}
	if(connected!=lastConnected){
		lastConnected=connected;
		flowControl.reset();
		appendLog("CLIENT: New flow state: " +QString(connected?"CONNECTED":"DISCONNECTED")+ " for "+QString::number(fingerprint));
	}
	reliabilitySystem.packetSent(written);
}

void Client::receive(){
	lastReceiveTime=QDateTime::currentMSecsSinceEpoch();
	connected=true;
	timeoutAccumulator = 0.0f;
}

void Client::appendLog(QString msg){
	if(0!=log){
		log->appendLog(msg);
	}
}

bool Client::idle(){
	const float sendRate = flowControl.sendRate();
	if( deltaTime > (1.0f / sendRate) ) {
		appendLog("SENDING IDLE PACKET "+QString::number(idlePacketsSent));
		idleAccumulator=0.0f;
		idlePacketsSent++;
		return connected;
	}
	return false;
}


QString Client::getSummary(QString sep) const {
	QString out;
	QTextStream ts(&out);
	ts << "ID: "<<fingerprint;
	ts << ", NET: "<<host.toIPv4Address()<<":"<<port;
	ts << ", DELTA: "<<deltaTime<<sep;
	ts << ", TOA: "<<timeoutAccumulator<<sep;
	ts << flowControl.getSummary(sep);
	return out;
}


const QString Client::getListText() const{
	QString name=getSummary(" - ");
	return name;
}


quint64 Client::getHash(){
	return fingerprint;
}


