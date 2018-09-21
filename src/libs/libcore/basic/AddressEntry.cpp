#include "AddressEntry.hpp"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QDateTime>
#include <QVariantMap>


AddressEntry::AddressEntry(NetworkAddress address, QString description, quint64 created, quint64 lastSuccess, quint64 lastError, quint64 numSuccessful, quint64 numErraneous)
	: address(address)
	, description(description)
	, created(created)
	, lastSuccess(lastSuccess)
	, lastError(lastError)
	, numSuccessful(numSuccessful)
	, numErraneous(numErraneous)
{

}
AddressEntry::AddressEntry(QVariantMap map)
	: address( map["address"].toMap() )
	, description(map["description"].toString())
	, created(utility::variantToMs(map["createdMS"]))
	, lastSuccess(utility::variantToMs(map["lastSuccessMS"]))
	, lastError(utility::variantToMs(map["lastErrorMS"]))
	, numSuccessful(map["numSuccessful"].toULongLong())
	, numErraneous(map["numErraneous"].toULongLong())
{

}

//TODO: Implement this compeltel,y and write test for it, lazy basterd

quint64 AddressEntry::score(QHostAddress dgw) const
{
	// Trivial reject for useless entries
	if(!address.isValid()) {
		return 0;
	}
	quint64 out=1;
	// Prefer ipv4 because it is simpler
	if(address.isIPv4()) {
		out++;
	}
	// Prefer addresses on same network as supplied address (meant to be default gateway or other network you like)
	if(!dgw.isNull()) {
		out+=utility::addressCloseness(dgw, address.ip());
	}
	// Prefer non-loopback addresses
	if(!address.ip().isLoopback()) {
		out++;
	}
	// Prefer addresses that have connected successfull at least one
	if(numSuccessful > 0) {
		out ++;
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
	return qMax(created, qMax(lastSuccess, lastError));
}


void AddressEntry::tried(bool successful, quint64 now)
{
	if(0==now) {
		now = utility::currentMsecsSinceEpoch<quint64>();
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
	QVariantMap map;
	map["address"]=address.toVariantMap();
	map["description"]=description;
	map["createdMS"]=utility::msToVariant(created);
	map["lastSuccessMS"]=utility::msToVariant(lastSuccess);
	map["lastErrorMS"]=utility::msToVariant(lastError);
	map["numSuccessful"]=numSuccessful;
	map["numErraneous"]=numErraneous;
	return map;
}



QString AddressEntry::toString()
{
	OC_METHODGATE();
	return address.toString()
		   +", description: "+description
		   +", createdMS: "+utility::formattedDateFromMS(created)
		   +", lastSuccessMS: "+utility::formattedDateFromMS(lastSuccess)
		   +", lastErrorMS: "+utility::formattedDateFromMS(lastError)
		   +", createdMS: "+utility::formattedDateFromMS(created)
		   +", numSuccessful: "+utility::formattedDateFromMS(numSuccessful)
		   +", numErraneous: "+utility::formattedDateFromMS(numErraneous)
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
