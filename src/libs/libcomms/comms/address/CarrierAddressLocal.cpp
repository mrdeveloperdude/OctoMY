#include "CarrierAddressLocal.hpp"


#include "uptime/MethodGate.hpp"
#include "CarrierAddressType.hpp"

#include <QDataStream>


#define MIN_LOCAL_ADDRESS_LENGTH (8)

CarrierAddressLocal::CarrierAddressLocal()
	: CarrierAddress(CarrierAddressType::LOCAL)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressLocal by default results in address=" << mLocalAddress;
}

CarrierAddressLocal::CarrierAddressLocal(const CarrierAddressLocal &other)
	: CarrierAddress(CarrierAddressType::LOCAL)
	, mLocalAddress(other.mLocalAddress)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressLocal from other(copy) results in address=" << mLocalAddress;
}


CarrierAddressLocal::CarrierAddressLocal(const QString &localAddress)
	: CarrierAddress(CarrierAddressType::LOCAL)
	, mLocalAddress(localAddress)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressLocal directly from ip="<<ip<<", port="<<port;
}


CarrierAddressLocal::CarrierAddressLocal(QVariantMap map)
	: CarrierAddress(CarrierAddressType::LOCAL)
	, mLocalAddress(map["address"].toString())
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressLocal from map: "<<map<<" results in address=" << mLocalAddress;
}


CarrierAddressLocal::~CarrierAddressLocal()
{
	OC_METHODGATE();
	//qDebug()<<"DELETING CarrierAddressLocal with ip="<<mIP<<", port="<<mPort;
}


/////////////////


QString CarrierAddressLocal::address() const
{
	OC_METHODGATE();
	return mLocalAddress;
}



void CarrierAddressLocal::setAddress(const QString &address)
{
	OC_METHODGATE();
	mLocalAddress = address;
}



/////////////////



bool CarrierAddressLocal::isValid() const
{
	OC_METHODGATE();
	return mLocalAddress.size() >= MIN_LOCAL_ADDRESS_LENGTH;
}


bool CarrierAddressLocal::equal(const CarrierAddress &o) const
{
	OC_METHODGATE();
	if(o.type() != mType){
		return false;
	}
	auto udp_o = static_cast<const CarrierAddressLocal &>(o);
	return udp_o.mLocalAddress == mLocalAddress;
}


void CarrierAddressLocal::toVariantMap(QVariantMap &map) const
{
	OC_METHODGATE();
	map["type"] = CarrierAddressTypeToString(mType);
	map["address"] = mLocalAddress;
}


QVariantMap CarrierAddressLocal::toVariantMap() const
{
	OC_METHODGATE();
	QVariantMap map;
	toVariantMap(map);
	return map;
}


QString CarrierAddressLocal::toString() const
{
	OC_METHODGATE();
	return mLocalAddress;
}


// TODO: Add support for IPv6
void CarrierAddressLocal::fromString(QString str, bool allowOnlyAddress)
{
	OC_METHODGATE();
	Q_UNUSED(allowOnlyAddress);
	mLocalAddress = str;
}


void CarrierAddressLocal::toStream(QDataStream &ds) const
{
	OC_METHODGATE();
	ds << mLocalAddress;
}


void CarrierAddressLocal::fromStream(QDataStream &ds)
{
	OC_METHODGATE();
	ds >> mLocalAddress;
}
