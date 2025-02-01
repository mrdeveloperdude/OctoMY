#ifndef CARRIERADDRESSFACTORY_HPP
#define CARRIERADDRESSFACTORY_HPP

#include "uptime/SharedPointerWrapper.hpp"

class CarrierAddress;

class CarrierAddressFactory
{
public:
	CarrierAddressFactory();

public:
	QSharedPointer<CarrierAddress> fromMap(const QVariantMap &map);
	void toMap(const CarrierAddress &address, QVariantMap &map);
	QSharedPointer<CarrierAddress> fromString(const QString &string);

};

#endif // CARRIERADDRESSFACTORY_HPP
