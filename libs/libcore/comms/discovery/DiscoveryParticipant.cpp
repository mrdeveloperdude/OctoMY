#include "DiscoveryParticipant.hpp"
#include <QDateTime>

#include <QRegularExpression>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>


DiscoveryParticipant::DiscoveryParticipant()
	: publicPort(0)
	, localPort(0)
	, lastTime (0)
{

}

DiscoveryParticipant::DiscoveryParticipant(QString publicKey, QString public_address, quint16 public_port, QString local_address, quint16 local_port)
	: publicKey(publicKey.trimmed())
	, publicAddress(public_address)
	, publicPort(public_port)
	, localAddress(local_address)
	, localPort(local_port)
	, lastTime (QDateTime::currentMSecsSinceEpoch())
{
	generateID();
}

DiscoveryParticipant::DiscoveryParticipant(const DiscoveryParticipant &o)
	: publicKey(o.publicKey)
	, publicAddress(o.publicAddress)
	, publicPort(o.publicPort)
	, localAddress(o.localAddress)
	, localPort(o.localPort)
	, lastTime (o.lastTime)
{
	generateID();
}


bool DiscoveryParticipant::operator==(const DiscoveryParticipant &o) const {
	return o.publicAddress == publicAddress
			&& o.publicPort == publicPort
			&& o.localAddress == localAddress
			&& o.localPort == localPort;
	//Disregard lastTime on purpose
}

bool DiscoveryParticipant::isSet(){
	return (0!=localPort);
}

QVariantMap DiscoveryParticipant::toVariantMap(){
	QVariantMap map;
	map["publicAddress"]=publicAddress;
	map["publicPort"]=publicPort;
	map["localAddress"]=localAddress;
	map["localPort"]=localPort;
	map["lastTime"]=lastTime;
	map["pins"]=pins;
	map["publicKey"]=publicKey;
	return map;
}

const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters

void DiscoveryParticipant::addPin(QString pin){
	if(rePin.match(pin).hasMatch()){
		qDebug()<<"ACCEPTED PIN:" <<pin;
		pins<<pin;
	}
	else {
		qWarning()<<"ERROR: pin did not match validation:" <<pin;
	}
}

void DiscoveryParticipant::generateID(){
	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(publicKey.toUtf8());
	ID=hash.result().toHex().toLower();
}
