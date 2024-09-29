#include "ActuatorControllerFactory.hpp"

#include "IActuatorController.hpp"

#ifdef OC_USE_FEATURE_ARDUINO
#include "ardumy/ArduMYController.hpp"
#endif
#include "hardware/controllers/servotor32/Servotor32Controller.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

ActuatorControllerFactory::ActuatorControllerFactory()
{
	OC_METHODGATE();
}


QSharedPointer<IActuatorController> ActuatorControllerFactory::controllerFactory(QString type)
{
	OC_METHODGATE();
	QSharedPointer<IActuatorController>	ret;
	type=type.trimmed().toLower();
	if("servotor32"==type) {
		ret = QSharedPointer<Servotor32Controller>::create();
	}
#ifdef OC_USE_FEATURE_ARDUINO
	else if("ardumy"==type) {
		ret = QSharedPointer<ArduMYController>::create();
	}
#endif
	return ret;
}
