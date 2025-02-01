#include "CarrierAddressFactory.hpp"

#include "CarrierAddressBluetooth.hpp"
#include "CarrierAddressLocal.hpp"
#include "CarrierAddressUDP.hpp"


CarrierAddressFactory::CarrierAddressFactory(){
	
}


QSharedPointer<CarrierAddress> CarrierAddressFactory::fromMap(const QVariantMap &map){
	const auto type = CarrierAddressTypeFromString(map["type"].toString());
	switch(type){
		case BLUETOOTH: return QSharedPointer<CarrierAddressBluetooth>::create(map);
		case LOCAL: return QSharedPointer<CarrierAddressLocal>::create(map);
		case UDP: return QSharedPointer<CarrierAddressUDP>::create(map);
		default:
		case INVALID_CARRIER_ADDRESS:
			break;
	}
	return nullptr;
}

void CarrierAddressFactory::toMap(const CarrierAddress &address, QVariantMap &map){
	const auto type = address.type();
	switch(type){
	case BLUETOOTH: static_cast<CarrierAddressBluetooth>(address).toVariantMap(map);
		case LOCAL: static_cast<CarrierAddressLocal>(address).toVariantMap(map);
		//case UDP: static_cast<CarrierAddressUDP>(address).toVariantMap(map);
		default:
		case INVALID_CARRIER_ADDRESS:
			break;
	}
}


QSharedPointer<CarrierAddress> CarrierAddressFactory::fromString(const QString &string){
	// TODO: Implement
	Q_UNUSED(string);
	return nullptr;
}
