#include "ControllerFactory.hpp"

#include "IController.hpp"

#ifdef OC_USE_FEATURE_ARDUINO
#include "ardumy/ArduMYController.hpp"
#endif
#include "hardware/controllers/servotor32/Servotor32Controller.hpp"

#include "uptime/MethodGate.hpp"

ControllerFactory::ControllerFactory()
{
	OC_METHODGATE();
}


QSharedPointer<IController> ControllerFactory::controllerFactory(const QString &type)
{
	OC_METHODGATE();
	QSharedPointer<IController> ret;
	auto t = type.trimmed().toLower();
	if("none" == t) {
		ret = nullptr;
	}
	else if("servotor32" == t) {
		ret = QSharedPointer<Servotor32Controller>::create();
	}
#ifdef OC_USE_FEATURE_ARDUINO
	else if("ardumy"==type) {
		ret = QSharedPointer<ArduMYController>::create();
	}
#endif
	return ret;
}



QStringList ControllerFactory::availableControllers(){
	QStringList list;
	list << "none";
	list << "servotor32";
#ifdef OC_USE_FEATURE_ARDUINO
	list << "ardumy";
#endif
	return list;
}
