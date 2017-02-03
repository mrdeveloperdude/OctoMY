#include "ActuatorControllerFactory.hpp"

#include "IActuatorController.hpp"
#include "Servotor32Controller.hpp"
#include "ArduMYController.hpp"

ActuatorControllerFactory::ActuatorControllerFactory()
{

}


IActuatorController *ActuatorControllerFactory::controllerFactory(QString type)
{
	IActuatorController *ret=nullptr;
	type=type.trimmed().toLower();
	if("servotor32"==type) {
		ret=new Servotor32Controller();
	} else if("ardumy"==type) {
		ret=new ArduMYController();
	}
	return ret;
}
