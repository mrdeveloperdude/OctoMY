#include "MtlobeFactory.hpp"

#include "Mtlobe.hpp"

#include "uptime/MethodGate.hpp"

#include "uptime/New.hpp"

#include "WheeledMtlobe.hpp"
#include "LeggedMtlobe.hpp"
#include "TrackedMtlobe.hpp"
#include "HoveringMtlobe.hpp"

MtlobeFactory::MtlobeFactory()
{
	OC_METHODGATE();
}


Mtlobe *MtlobeFactory::mtlobeFactory(QString type)
{
	OC_METHODGATE();
	Mtlobe *ret=nullptr;
	type=type.trimmed().toLower();
	if("wheeled"==type) {
		ret=OC_NEW WheeledMtlobe();
	} else if("tracked"==type) {
		ret=OC_NEW TrackedMtlobe();
	} else if("legged"==type) {
		ret=OC_NEW LeggedMtlobe();
	} else if("hovering"==type) {
		ret=OC_NEW HoveringMtlobe();
	}
	return ret;
}
