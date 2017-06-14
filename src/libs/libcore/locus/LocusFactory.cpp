#include "LocusFactory.hpp"

#include "Locus.hpp"

#include "WheeledLocus.hpp"
#include "LeggedLocus.hpp"
#include "TrackedLocus.hpp"
#include "HoveringLocus.hpp"

LocusFactory::LocusFactory()
{

}




Locus *LocusFactory::locusFactory(QString type)
{
	Locus *ret=nullptr;
	type=type.trimmed().toLower();
	if("wheeled"==type) {
		ret=new WheeledLocus();
	} else if("tracked"==type) {
		ret=new TrackedLocus();
	} else if("legged"==type) {
		ret=new LeggedLocus();
	} else if("hovering"==type) {
		ret=new HoveringLocus();
	}
	return ret;
}
