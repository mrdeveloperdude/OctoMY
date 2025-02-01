#include "CarrierAddressBluetooth.hpp"

#include "uptime/MethodGate.hpp"
#include "CarrierAddressType.hpp"

#include <QDataStream>


CarrierAddressBluetooth::CarrierAddressBluetooth()
	: CarrierAddress(CarrierAddressType::BLUETOOTH)
{
	OC_METHODGATE();
	// qDebug() << "CREATING CarrierAddressBluetooth by default results in address=" << mBluetoothAddress;
}

CarrierAddressBluetooth::CarrierAddressBluetooth(const CarrierAddressBluetooth &other)
	: CarrierAddress(CarrierAddressType::BLUETOOTH)
	, mBluetoothAddress(other.mBluetoothAddress)
{
	OC_METHODGATE();
	// qDebug() << "CREATING CarrierAddressBluetooth from other(copy) results in address=" << mBluetoothAddress;
}


CarrierAddressBluetooth::CarrierAddressBluetooth(const QString &address)
	: CarrierAddress(CarrierAddressType::BLUETOOTH)
	, mBluetoothAddress(address)
{
	OC_METHODGATE();
	// qDebug() << "CREATING CarrierAddressBluetooth directly from address=" << address;
}

CarrierAddressBluetooth::CarrierAddressBluetooth(const quint64 address)
	: CarrierAddress(CarrierAddressType::BLUETOOTH)
	, mBluetoothAddress(address)
{
	OC_METHODGATE();
	// qDebug() << "CREATING CarrierAddressBluetooth directly from address=" << address;
}


CarrierAddressBluetooth::CarrierAddressBluetooth(QVariantMap map)
	: CarrierAddress(CarrierAddressType::BLUETOOTH)
	, mBluetoothAddress(map["address"].toString())
{
	OC_METHODGATE();
	// qDebug() << "CREATING CarrierAddressBluetooth from map: "<<map<<" results in address=" << mBluetoothAddress;
}


CarrierAddressBluetooth::~CarrierAddressBluetooth()
{
	OC_METHODGATE();
	// qDebug() << "DELETING CarrierAddressBluetooth with address=" << mBluetoothAddress;
}


/////////////////

QBluetoothAddress CarrierAddressBluetooth::address() const
{
	OC_METHODGATE();
	return mBluetoothAddress;
}



void CarrierAddressBluetooth::setAddress(const QBluetoothAddress &address)
{
	OC_METHODGATE();
	mBluetoothAddress = address;
}



/////////////////



bool CarrierAddressBluetooth::isValid() const
{
	OC_METHODGATE();
	return !mBluetoothAddress.isNull();
}


bool CarrierAddressBluetooth::equal(const CarrierAddress &o) const
{
	OC_METHODGATE();
	if(o.type() != mType){
		return false;
	}
	auto udp_o = static_cast<const CarrierAddressBluetooth &>(o);
	return udp_o.mBluetoothAddress == mBluetoothAddress;
}


void CarrierAddressBluetooth::toVariantMap(QVariantMap &map) const
{
	OC_METHODGATE();
	map["type"] = CarrierAddressTypeToString(mType);
	map["address"] = mBluetoothAddress.toUInt64();
}


QVariantMap CarrierAddressBluetooth::toVariantMap() const
{
	OC_METHODGATE();
	QVariantMap map;
	toVariantMap(map);
	return map;
}


QString CarrierAddressBluetooth::toString() const
{
	OC_METHODGATE();
	return mBluetoothAddress.toString();
}


// TODO: Add support for IPv6
void CarrierAddressBluetooth::fromString(QString str, bool allowOnlyAddress)
{
	OC_METHODGATE();
	Q_UNUSED(allowOnlyAddress);
	mBluetoothAddress = QBluetoothAddress(str);
}


void CarrierAddressBluetooth::toStream(QDataStream &ds) const
{
	OC_METHODGATE();
	ds << mBluetoothAddress.toUInt64();
}


void CarrierAddressBluetooth::fromStream(QDataStream &ds)
{
	OC_METHODGATE();
	quint64 i{0};
	ds >> i;
	mBluetoothAddress = QBluetoothAddress(i);
}
