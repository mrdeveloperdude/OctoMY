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
		ret=new WheeledMtlobe();
	} else if("tracked"==type) {
		ret=new TrackedMtlobe();
	} else if("legged"==type) {
		ret=new LeggedMtlobe();
	} else if("hovering"==type) {
		ret=new HoveringMtlobe();
	}
	return ret;
}
