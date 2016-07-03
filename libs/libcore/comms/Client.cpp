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

void Client::countSend(qint64 written){
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	if(lastSendTime<=0) {
		lastSendTime=now-1;
	}
	const qint64 delta=now-lastSendTime;
	lastSendTime=now;
	deltaTime=delta/1000.0f;
	timeoutAccumulator += deltaTime;
	if ( timeoutAccumulator > TIMEOUT_TRESHOLD ) {
		qDebug()<<"CLIENT "<<QString::number(getHash(),16) <<" timed out";
		connected=false;
	}
	if(connected){
		reliabilitySystem.update(deltaTime);
		flowControl.update( deltaTime, reliabilitySystem.roundTripTime() * 1000.0f );
	}
	if(connected!=lastConnected){
		lastConnected=connected;
		flowControl.reset();
		appendLog("CLIENT: New flow state: " +QString(connected?"CONNECTED":"DISCONNECTED")+ " for "+QString::number(getHash(),16));
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
	ts << "ID: "<<QString::number(getHash(),16);
	ts << ", NET: "<<signature.host.toIPv4Address()<<":"<<signature.port;
	ts << ", DELTA: "<<deltaTime<<sep;
	ts << ", TOA: "<<timeoutAccumulator<<sep;
	ts << flowControl.getSummary(sep);
	return out;
}


const QString Client::getListText() const{
	QString name=getSummary(" - ");
	return name;
}


quint64 Client::getHash() const{
	return (quint64)signature.executable| ((quint64)signature.platform <<32);
}



////////////////////////////





void ClientDirectory::insert(Client *c){
	if(0!=c && 0==getByHash(c->signature.platform, c->signature.executable) && 0==getByHost(c->signature.host, c->signature.port)){
		const quint64 hash=c->signature.executable| ((quint64)c->signature.platform <<32);
		const quint64 host=c->signature.host.toIPv4Address()| ((quint64)c->signature.port <<32);
		byHash.insert(hash, c);
		byHost.insert(host, c);
		all.insert(c);
		emit clientAdded(c);
	}
	else{
		qWarning()<<"ERROR: could not insert client: "<<c;
	}
}



Client *ClientDirectory::getByHash(const quint32 platform, const quint32 executable, const bool addIfMissing){
	const quint64 hash=executable| ((quint64)platform <<32);
	QMap<quint64, Client *> ::const_iterator it=byHash.find(hash);
	if(byHash.end()==it){
		if(addIfMissing){
			Client *c=new Client(ClientSignature(platform,executable));
			insert(c);
			return c;
		}
		return 0;
	}
	return it.value();
}

Client *ClientDirectory::getByHost(const QHostAddress host,const  quint16 port, const bool addIfMissing){
	const quint64 hash=host.toIPv4Address()| ((quint64)port <<32);
	QMap<quint64, Client *> ::const_iterator it=byHost.find(hash);
	if(byHost.end()==it){
		if(addIfMissing){
			Client *c=new Client(ClientSignature(0,0,host,port));
			insert(c);
			return c;
		}
		return 0;
	}
	return it.value();
}


Client *ClientDirectory::getBest(const ClientSignature signarture, const bool addIfMissing){
	return getBest(signarture.platform,signarture.executable,signarture.host, signarture.port,addIfMissing);
}

Client *ClientDirectory::getBest(const quint32 platform, const quint32 executable, const QHostAddress host,const  quint16 port, const bool addIfMissing){
	Client *c=getByHost(host,port);
	if(0==c){
		c=getByHash(platform,executable);
	}
	if(0==c && addIfMissing){
		c=new Client(ClientSignature(platform,executable,host,port));
		insert(c);
	}
	return c;
}


int ClientDirectory::count(){
	return all.size();
}


