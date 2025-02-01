#ifndef CARRIERADDRESSTYPE_HPP
#define CARRIERADDRESSTYPE_HPP

#include <QDebug>


enum CarrierAddressType: quint8{
	  INVALID_CARRIER_ADDRESS = 0
	, BLUETOOTH
	, LOCAL
	, UDP
};


QDebug &operator<<(QDebug &d, const CarrierAddressType &);


QString CarrierAddressTypeToString(CarrierAddressType s);


CarrierAddressType CarrierAddressTypeFromString(QString s);


#endif // CARRIERADDRESSTYPE_HPP
