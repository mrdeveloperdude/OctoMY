#include "CarrierAddress.hpp"

#include "uptime/MethodGate.hpp"

#include <QDataStream>

CarrierAddress::CarrierAddress(const CarrierAddressType type)
	: mType(type)
{
	OC_METHODGATE();
}
bool CarrierAddress::operator==(const CarrierAddress &o) const
{
	OC_METHODGATE();
	return equal(o);
}

bool CarrierAddress::operator!=(const CarrierAddress &o) const
{
	OC_METHODGATE();
	return !equal(o);
}

bool CarrierAddress::is(const CarrierAddressType &type) const{
	OC_METHODGATE();
	return type == mType;
}

CarrierAddressType CarrierAddress::type() const{
	OC_METHODGATE();
	return mType;
}

const QDebug &operator<<(QDebug &d, const CarrierAddress &na)
{
	OC_FUNCTIONGATE();
	d.nospace().noquote() << na.toString();
	return d.maybeSpace();
}

QDebug &operator<<(QDebug &d, CarrierAddress &na)
{
	OC_FUNCTIONGATE();
	d.nospace().noquote() << na.toString();
	return d.maybeSpace();
}


QDataStream &operator<<(QDataStream &ds, const CarrierAddress &addr)
{
	OC_FUNCTIONGATE();
	addr.toStream(ds);
	return ds;
}


QDataStream &operator>>(QDataStream &ds, CarrierAddress &addr)
{
	OC_FUNCTIONGATE();
	addr.fromStream(ds);
	return ds;
}


CarrierAddress::operator QString() const
{
	OC_METHODGATE();
	return toString();
}


CarrierAddress::operator QString()
{
	OC_METHODGATE();
	return toString();
}
