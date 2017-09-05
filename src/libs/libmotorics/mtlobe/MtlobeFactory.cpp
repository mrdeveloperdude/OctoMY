#include "MtlobeFactory.hpp"

#include "Mtlobe.hpp"

#include "WheeledMtlobe.hpp"
#include "LeggedMtlobe.hpp"
#include "TrackedMtlobe.hpp"
#include "HoveringMtlobe.hpp"

MtlobeFactory::MtlobeFactory()
{

}




Mtlobe *MtlobeFactory::mtlobeFactory(QString type)
{
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
