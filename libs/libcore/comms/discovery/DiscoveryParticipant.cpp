#include "DiscoveryParticipant.hpp"
#include <QDateTime>

#include <QRegularExpression>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>




//////////////////////////////////////////////////////////////////////////

DiscoveryParticipant::DiscoveryParticipant()
	: isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}

/*
DiscoveryParticipant::DiscoveryParticipant(QString publicKey, QString public_address, quint16 public_port, QString local_address, quint16 local_port, DiscoveryRole role, DiscoveryType type)
: assoc(put something in here)
	: isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}
*/



DiscoveryParticipant::DiscoveryParticipant(QVariantMap map)
	: assoc(map, true)
	, isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}





DiscoveryParticipant::DiscoveryParticipant(const DiscoveryParticipant &o)
	: assoc(o.assoc)
	, pins(o.pins)
	, isDeleted(false)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
}


DiscoveryParticipant::~DiscoveryParticipant()
{
	isDeleted=true;
}


bool DiscoveryParticipant::operator==(const DiscoveryParticipant &o) const
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return assoc == o.assoc;
}
/*
bool DiscoveryParticipant::isSet()
{
OC_METHODGATE();
	return (0!=localPort);
}
*/

bool DiscoveryParticipant::isValidForServer()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return (!pins.isEmpty()) && assoc.isValid() ;
}


bool DiscoveryParticipant::isValidForClient()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return assoc.isValid();
}


QVariantMap DiscoveryParticipant::toVariantMap()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	QVariantMap map=assoc.toVariantMap();
	map["pins"]=pins;
	return map;
}



QString DiscoveryParticipant::toString()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return assoc.toString()+", pins:"+pins.join(";");
}

const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


void DiscoveryParticipant::clearPins()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	pins.clear();
}


void DiscoveryParticipant::addPin(QString pin)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
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
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	assoc.clearTrust();
}

void DiscoveryParticipant::addTrust(QString trust)
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	assoc.addTrust(trust);
}


ClientSignature DiscoveryParticipant::clientSignature()
{
	OC_METHODGATE();
	Q_ASSERT(!isDeleted);
	return ClientSignature(assoc.id(), assoc.publicAddress());
}

bool DiscoveryParticipant::updateFromServer(QVariantMap &map, bool trustedSource)
{
	return assoc.update(map, trustedSource);
}




const QDebug &operator<<(QDebug &d, DiscoveryParticipant &part)
{
	OC_FUNCTIONGATE();
	Q_ASSERT(!part.isDeleted);
	d.nospace() << "DiscoveryParticipant( assoc="<<part.toString()<<")";
	return d.maybeSpace();
}

