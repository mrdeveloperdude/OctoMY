#include "DiscoveryParticipant.hpp"
#include <QDateTime>

#include <QRegularExpression>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>


DiscoveryParticipant::DiscoveryParticipant()
	: publicPort(0)
	, localPort(0)
	, role(ROLE_UNKNOWN)
	, type(TYPE_UNKNOWN)
	, lastTime (0)
{

}

DiscoveryParticipant::DiscoveryParticipant(QString publicKey, QString public_address, quint16 public_port, QString local_address, quint16 local_port, DiscoveryRole role, DiscoveryType type)
	: publicKey(publicKey.trimmed())
	, publicAddress(public_address)
	, publicPort(public_port)
	, localAddress(local_address)
	, localPort(local_port)
	, role(role)
	, type(type)
	, lastTime (QDateTime::currentMSecsSinceEpoch())
{
	generateID();
}




DiscoveryParticipant::DiscoveryParticipant(QVariantMap map)
	: publicKey(map["publicKey"].toString().trimmed())
	, publicAddress(map["publicAddress"].toString().trimmed())
	, publicPort(map["publicPort"].toInt())
	, localAddress(map["localAddress"].toString().trimmed())
	, localPort(map["localPort"].toInt())
	, role(DiscoveryRoleFromString(map["role"].toString()))
	, type(DiscoveryTypeFromString(map["type"].toString()))
	, lastTime (QDateTime::currentMSecsSinceEpoch())//TODO: Should this come from map too?
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


bool DiscoveryParticipant::operator==(const DiscoveryParticipant &o) const
{
	return o.publicAddress == publicAddress
			&& o.publicPort == publicPort
			&& o.localAddress == localAddress
			&& o.localPort == localPort;
	//Disregard lastTime on purpose
}

bool DiscoveryParticipant::isSet()
{
	return (0!=localPort);
}


bool DiscoveryParticipant::isValidServer()
{
	return (
				(!publicKey.trimmed().isEmpty())
				&& (!pins.isEmpty())
				&& (
					(!publicAddress.trimmed().isEmpty() && 0!=publicPort)
					|| (!localAddress.trimmed().isEmpty() && 0!=localPort)
					)
				);
}


bool DiscoveryParticipant::isValidClient()
{
	return (
				(!publicKey.trimmed().isEmpty())
				&& (
					(!publicAddress.trimmed().isEmpty() && 0!=publicPort)
					|| (!localAddress.trimmed().isEmpty() && 0!=localPort)
					)
				);
}



QVariantMap DiscoveryParticipant::toVariantMap()
{
	QVariantMap map;
	map["publicAddress"]=publicAddress;
	map["publicPort"]=publicPort;
	map["localAddress"]=localAddress;
	map["localPort"]=localPort;
	map["lastTime"]=lastTime;
	map["pins"]=pins;
	map["publicKey"]=publicKey;
	map["role"]=DiscoveryRoleToString(role);
	map["type"]=DiscoveryTypeToString(type);
	return map;
}

const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters

void DiscoveryParticipant::addPin(QString pin)
{
	if(rePin.match(pin).hasMatch()){
		//qDebug()<<"ACCEPTED PIN:" <<pin;
		pins<<pin;
	}
	else {
		//qDebug()<<"Pin "<<pin<<" did not match validation:";
	}
}


void DiscoveryParticipant::clearTrust()
{
	trusts.clear();
}

void DiscoveryParticipant::clearPins()
{
	pins.clear();
}

void DiscoveryParticipant::addTrust(QString trust)
{
	trusts<<trust;
}


void DiscoveryParticipant::generateID()
{
	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(publicKey.toUtf8());
	ID=hash.result().toHex().toLower();
}
