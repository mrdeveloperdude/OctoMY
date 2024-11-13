#include "LobeFactory.hpp"

#include "lobe/HoveringLobe.hpp"
#include "lobe/LeggedLobe.hpp"
#include "lobe/Lobe.hpp"
#include "lobe/TrackedLobe.hpp"
#include "lobe/WheeledLobe.hpp"
#include "uptime/MethodGate.hpp"

LobeFactory::LobeFactory()
{
	OC_METHODGATE();
}


QSharedPointer<Lobe> LobeFactory::createLobe(const QString &type)
{
	OC_METHODGATE();
	QSharedPointer<Lobe> ret;
	auto const t = type.trimmed().toLower();
	if("none" == t) {
		ret = nullptr;
	}
	else if("wheeled" == t) {
		ret = QSharedPointer<WheeledLobe>::create();
	}
	else if("tracked" == t) {
		ret = QSharedPointer<TrackedLobe>::create();
	}
	else if("legged" == t) {
		ret = QSharedPointer<LeggedLobe>::create();
	}
	else if("hovering" == t) {
		ret = QSharedPointer<HoveringLobe>::create();
	}
	return ret;
}



QStringList LobeFactory::availableLobes(){
	QStringList list;
	list << "wheeled";
	list << "legged";
	list << "tracked";
	list << "hovering";
	return list;
}
