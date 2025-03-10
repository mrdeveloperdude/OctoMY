#include "DiscoveryMandate.hpp"



QDebug operator<<(QDebug dbg, const DiscoveryMandate &mandate)
{
	// nospace() disables the automatic spacing between items.
	dbg.nospace() << "DiscoveryMandate{"
				  << "useZoo:" << (mandate.useZoo ? "true" : "false") << ", "
				  << "useNetwork:" << (mandate.useNetwork ? "true" : "false") << ", "
				  << "useBluetooth:" << (mandate.useBluetooth ? "true" : "false") << ", "
				  << "useAudio:" << (mandate.useAudio ? "true" : "false") << ", "
				  << "useNFC:" << (mandate.useNFC ? "true" : "false") << ", "
				  << "useCamera:" << (mandate.useCamera ? "true" : "false")
				  << "}";
	return dbg.space();
}

QDebug operator<<(QDebug dbg, QSharedPointer<const DiscoveryMandate> mandate){
	if(mandate.isNull()){
		dbg.nospace() << "NULL";
	}
	else{
		dbg.nospace() << *mandate;
	}
	return dbg.space();
}
