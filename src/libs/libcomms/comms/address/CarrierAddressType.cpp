#include "CarrierAddressType.hpp"


QDebug &operator<<(QDebug &d, const CarrierAddressType &type)
{
	
#define  CarrierAddressTypeOperatorStreamLeftCASE(A) case (A):{d.nospace() << #A;}break
	
	switch(type) {
		CarrierAddressTypeOperatorStreamLeftCASE(INVALID_CARRIER_ADDRESS);
		CarrierAddressTypeOperatorStreamLeftCASE(BLUETOOTH);
		CarrierAddressTypeOperatorStreamLeftCASE(LOCAL);
		CarrierAddressTypeOperatorStreamLeftCASE(UDP);
	default:
		d.nospace() << "UNKNOWN";
	}
	return d.maybeSpace();
#undef CarrierAddressTypeOperatorStreamLeftCASE
}


QString CarrierAddressTypeToString(CarrierAddressType s)
{
#define  CarrierAddressTypeToStringCASE(A) case (A):return #A
	switch(s) {
		CarrierAddressTypeToStringCASE(INVALID_CARRIER_ADDRESS);
		CarrierAddressTypeToStringCASE(BLUETOOTH);
		CarrierAddressTypeToStringCASE(LOCAL);
		CarrierAddressTypeToStringCASE(UDP);
	default:
		return "UNKNOWN";
	}
#undef CarrierAddressTypeToStringCASE
}



CarrierAddressType CarrierAddressTypeFromString(QString s)
{
#define  CarrierAddressTypeFromStringCASE(A) if(#A==s)return A
	CarrierAddressTypeFromStringCASE(INVALID_CARRIER_ADDRESS);
	CarrierAddressTypeFromStringCASE(BLUETOOTH);
	CarrierAddressTypeFromStringCASE(LOCAL);
	CarrierAddressTypeFromStringCASE(UDP);
	return INVALID_CARRIER_ADDRESS;
#undef CarrierAddressTypeFromStringCASE
}
