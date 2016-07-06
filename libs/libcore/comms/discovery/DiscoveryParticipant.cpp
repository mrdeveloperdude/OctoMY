#include "DiscoveryParticipant.hpp"
#include <QDateTime>

#include <QRegularExpression>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>




//////////////////////////////////////////////////////////////////////////

DiscoveryParticipant::DiscoveryParticipant()
{

}

DiscoveryParticipant::DiscoveryParticipant(QString publicKey, QString public_address, quint16 public_port, QString local_address, quint16 local_port, DiscoveryRole role, DiscoveryType type)
//: assoc()
{

}




DiscoveryParticipant::DiscoveryParticipant(QVariantMap map)
	: assoc(map, true)
{
}





DiscoveryParticipant::DiscoveryParticipant(const DiscoveryParticipant &o)
	: assoc(o.assoc)
	, pins(pins)
{

}


DiscoveryParticipant::~DiscoveryParticipant()
{

}


bool DiscoveryParticipant::operator==(const DiscoveryParticipant &o) const
{
	return assoc == o.assoc;
}
/*
bool DiscoveryParticipant::isSet()
{
	return (0!=localPort);
}
*/

bool DiscoveryParticipant::isValidServer()
{
	return (!pins.isEmpty()) && assoc.isValid() ;
}


bool DiscoveryParticipant::isValidClient()
{
	return assoc.isValid();
}


QVariantMap DiscoveryParticipant::toVariantMap()
{
	QVariantMap map=assoc.toVariantMap();
	map["pins"]=pins;
	return map;
}



QString DiscoveryParticipant::toString()
{

	return assoc.toString()+", pins:"+pins.join(";");

}

const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


void DiscoveryParticipant::clearPins()
{
	pins.clear();
}


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
	assoc.clearTrust();
}

void DiscoveryParticipant::addTrust(QString trust)
{
	assoc.addTrust(trust);
}

/*
QString DiscoveryParticipant::fullPublicAddress()
{
	return assoc.publicAddress();
}

QString DiscoveryParticipant::fullLocalAddress()
{
	return localAddress+":"+QString::number(localPort);
}

*/

ClientSignature DiscoveryParticipant::clientSignature(){
	return ClientSignature(0,0,assoc.localAddress().ip,assoc.localAddress().port);
}
