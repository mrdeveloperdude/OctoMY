#include "ActuatorControllerFactory.hpp"

#include "IActuatorController.hpp"

#include "hw/controllers/ardumy/ArduMYController.hpp"
#include "hw/controllers/servotor32/Servotor32Controller.hpp"

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
