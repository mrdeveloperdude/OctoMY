#ifndef DISCOVERYMANDATE_HPP
#define DISCOVERYMANDATE_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QDebug>

struct DiscoveryMandate{
	const bool connecting{false};
	const bool useZoo{false};
	const bool useNetwork{false};
	const bool useBluetooth{false};
	const bool useAudio{false};
	const bool useNFC{false};
	const bool useCamera{false};
	
	DiscoveryMandate(bool connecting=false, bool useZoo=false, bool useNetwork=false, bool useBluetooth=false, bool useAudio=false, bool useNFC=false, bool useCamera=false)
		: connecting(connecting)
		, useZoo(useZoo)
		, useNetwork(useNetwork)
		, useBluetooth(useBluetooth)
		, useAudio(useAudio)
		, useNFC(useNFC)
		, useCamera(useCamera)
	{
	}
};



QDebug operator<<(QDebug dbg, const DiscoveryMandate &mandate);
QDebug operator<<(QDebug dbg, QSharedPointer<const DiscoveryMandate> mandate);


#endif // DISCOVERYMANDATE_HPP
