#include "AddressEntry.hpp"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include <QDateTime>
#include <QVariantMap>


AddressEntry::AddressEntry(QSharedPointer<NetworkAddress> address, QString description, quint64 created, quint64 lastSuccess, quint64 lastError, quint64 numSuccessful, quint64 numErraneous)
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
	: address(new NetworkAddress( map["address"].toMap()  ) )
	, description(map["description"].toString())
	, created(map["createdMS"].toDateTime().toMSecsSinceEpoch())
	, lastSuccess(map["lastSuccessMS"].toDateTime().toMSecsSinceEpoch())
	, lastError(map["lastErrorMS"].toDateTime().toMSecsSinceEpoch())
	, numSuccessful(map["numSuccessful"].toULongLong())
	, numErraneous(map["numErraneous"].toULongLong())
{

}

//TODO: Implement this compeltel,y and write test for it, lazy basterd

quint64 AddressEntry::score() const
{
	// Trivial reject for useless entries
	if(address.isNull() || !address->isValid()) {
		return 0;
	}
	quint64 out=0;
	if(lastSuccess > lastError) {
		out += (qMax(created,lastSuccess) - qMax(created,lastError));
	}
	if(numSuccessful > numErraneous) {
		out += numSuccessful - numErraneous;
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
		now = QDateTime::currentMSecsSinceEpoch();
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
	map["address"]=address.isNull()?QVariantMap():address->toVariantMap();
	map["description"]=description;
	map["createdMS"]=QDateTime::fromMSecsSinceEpoch(created);
	map["lastSuccessMS"]=QDateTime::fromMSecsSinceEpoch(lastSuccess);
	map["lastErrorMS"]=QDateTime::fromMSecsSinceEpoch(lastError);
	map["numSuccessful"]=numSuccessful;
	map["numErraneous"]=numErraneous;
	return map;
}



QString AddressEntry::toString()
{
	OC_METHODGATE();
	return address->toString()
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
	return ( (o.address.isNull() == this->address.isNull() )
			 && (*o.address.data() == *this->address.data() )
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
