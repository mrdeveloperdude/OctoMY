#include "AddressEntry.hpp"

#include "comms/address/CarrierAddressFactory.hpp"
#include "comms/address/CarrierAddressUDP.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/network/Network.hpp"
#include "utility/string/String.hpp"
#include "utility/time/HumanTime.hpp"

#include <QDateTime>
#include <QVariantMap>


AddressEntry::AddressEntry(QSharedPointer<CarrierAddress> address, QString description, quint64 created, quint64 lastSuccess, quint64 lastError, quint64 numSuccessful, quint64 numErraneous)
	: address(address)
	, description(description)
	, created(created)
	, lastSuccess(lastSuccess)
	, lastError(lastError)
	, numSuccessful(numSuccessful)
	, numErraneous(numErraneous)
{
	OC_METHODGATE();
}

AddressEntry::AddressEntry(QVariantMap map)
	: description(map["description"].toString())
	, created(utility::time::variantToMs(map["createdMS"]))
	, lastSuccess(utility::time::variantToMs(map["lastSuccessMS"]))
	, lastError(utility::time::variantToMs(map["lastErrorMS"]))
	, numSuccessful(map["numSuccessful"].toULongLong())
	, numErraneous(map["numErraneous"].toULongLong())
{
	OC_METHODGATE();
	CarrierAddressFactory caf;
	address = caf.fromMap(map["address"].toMap());
}

// TODO: Implement this completely and write test for it, lazy basterd

quint64 AddressEntry::score(QHostAddress dgw) const
{
	OC_METHODGATE();
	// Trivial reject for null entries
	if(!address.isNull()) {
		return 0;
	}
	// Trivial reject for useless entries
	if(!address->isValid()) {
		return 0;
	}
	quint64 out=1;
	switch(address->type()){
		case(UDP):{
			auto udp_address = qSharedPointerCast<CarrierAddressUDP>(address);
			if(udp_address.isNull()){
				break;
			}
			// Prefer ipv4 because it is simpler
			if(udp_address->isIPv4()) {
				out++;
			}
			// Prefer non-loopback addresses
			if(!udp_address->ip().isLoopback()) {
				out++;
			}
			// Prefer addresses on same network as supplied address (meant to be default gateway or other network you like)
			if(!dgw.isNull()) {
				out+=utility::network::addressCloseness(dgw, udp_address->ip());
			}
		}break;
		case LOCAL:{
		}break;
		default:
		case INVALID_CARRIER_ADDRESS:break;
	}
	// Prefer addresses that have connected successfull at least one
	if (numSuccessful > 0) {
		out++;
	}
	// Prefer addresses that have been successfull lately
	if(lastSuccess > lastError) {
		out ++;//= (qMax(created,lastSuccess) - qMax(created,lastError));
	}
	// Prefer addresses that have connected successfully most of the time
	if(numSuccessful > numErraneous) {
		out ++;//= numSuccessful - numErraneous;
	}
	return out;
}

quint64 AddressEntry::last() const
{
	OC_METHODGATE();
	return qMax(created, qMax(lastSuccess, lastError));
}


void AddressEntry::tried(bool successful, quint64 now)
{
	OC_METHODGATE();
	if(0 == now) {
		now = utility::time::currentMsecsSinceEpoch<quint64>();
	}
	if(successful) {
		lastSuccess = now;
		numSuccessful++;
	} else {
		lastError = now;
		numErraneous++;
	}
}


QVariantMap AddressEntry::toVariantMap() const
{
	OC_METHODGATE();
	QVariantMap map;
	map["address"]=address.isNull()?QVariantMap():address->toVariantMap();
	map["description"]=description;
	map["createdMS"]=utility::time::msToVariant(created);
	map["lastSuccessMS"]=utility::time::msToVariant(lastSuccess);
	map["lastErrorMS"]=utility::time::msToVariant(lastError);
	map["numSuccessful"]=numSuccessful;
	map["numErraneous"]=numErraneous;
	return map;
}



QString AddressEntry::toString()
{
	OC_METHODGATE();
	return address.isNull()?"NULL":address->toString()
		   +", description: "+description
		   +", createdMS: "+utility::string::formattedDateFromMS(created)
		   +", lastSuccessMS: "+utility::string::formattedDateFromMS(lastSuccess)
		   +", lastErrorMS: "+utility::string::formattedDateFromMS(lastError)
		   +", createdMS: "+utility::string::formattedDateFromMS(created)
		   +", numSuccessful: "+QString::number(numSuccessful)
		   +", numErraneous: "+QString::number(numErraneous)
		   ;
}



bool AddressEntry::operator==(const AddressEntry &o) const
{
	OC_METHODGATE();
	return (
			   (o.address == this->address )
			   && (o.description == this->description )
			   && (o.created == this->created )
			   && (o.lastSuccess == this->lastSuccess )
			   && (o.lastError == this->lastError )
			   && (o.numSuccessful == this->numSuccessful )
			   && (o.numErraneous == this->numErraneous )
		   );
}

bool AddressEntry::operator!=(const AddressEntry &o) const
{
	OC_METHODGATE();
	return !(o == *this);
}



const QDebug &operator<<(QDebug &d, AddressEntry &a)
{
	OC_FUNCTIONGATE();
	d.nospace() << "AddressEntry("<<a.toString()<<")";
	return d.maybeSpace();
}
