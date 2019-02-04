#include "PunchRegistry.hpp"

#include "utility/time/HumanTime.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

PunchPartner::PunchPartner()
	: public_port(0)
	, local_port(0)
	, lastTime (0)
{
	OC_METHODGATE();
}

PunchPartner::PunchPartner(QString public_address, quint16 public_port, QString local_address, quint16 local_port)
	: public_address(public_address)
	, public_port(public_port)
	, local_address(local_address)
	, local_port(local_port)
	, lastTime (utility::time::currentMsecsSinceEpoch<quint64>())
{
	OC_METHODGATE();
}
/*
	PunchPartner(const PunchPartner &o)
		: public_address(o.public_address)
		, public_port(o.public_port)
		, local_address(o.local_address)
		, local_port(o.local_port)
		, lastTime (o.lastTime)
	{
		OC_METHODGATE();
	}
*/

bool PunchPartner::operator==(const PunchPartner &o) const
{
	OC_METHODGATE();
	return o.public_address == public_address
		   && o.public_port == public_port
		   && o.local_address == local_address
		   && o.local_port == local_port;
	//Disregard lastTime on purpose
}

bool PunchPartner::isSet()
{
	OC_METHODGATE();
	return (0!=local_port);
}

QVariantMap PunchPartner::toVariantMap()
{
	QVariantMap map;
	map["public_address"]=public_address;
	map["public_port"]=public_port;
	map["local_address"]=local_address;
	map["local_port"]=local_port;
	map["lastTime"]=lastTime;
	return map;
}




UDPPunch::UDPPunch(QString token)
	: token(token)
{
	OC_METHODGATE();

}

bool UDPPunch::set(PunchPartner p)
{
	OC_METHODGATE();
	if(!a.isSet() || a==p) {
		a=p;
		return true;
	}
	if(!b.isSet() || b==p) {
		b=p;
		return true;
	}
	return false;
}

PunchPartner UDPPunch::theOther(PunchPartner p)
{
	OC_METHODGATE();
	if(p==a) {
		return b;
	}
	return a;
}

QVariantMap UDPPunch::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["token"]=QVariant(token);
	map["a"]=a.toVariantMap();
	map["b"]=b.toVariantMap();
	return map;
}



bool PunchRegistry::update(QString punchToken, QString public_address, quint16 public_port, QString local_address, quint16 local_port)
{
	OC_METHODGATE();
	if(contains(punchToken)) {
		UDPPunch *punch=OC_NEW UDPPunch(punchToken);
		if(nullptr!=punch) {
			punch->set(PunchPartner(public_address, public_port, local_address, local_port));
		}
	} else {
		UDPPunch *punch=operator[](punchToken);
		if(nullptr!=punch) {
			if(punch->set(PunchPartner(public_address, public_port, local_address, local_port))) {
				return true;
			}
		}
	}
	return false;
}


