#ifndef ACTUATORCONTROLLERFACTORY_HPP
#define ACTUATORCONTROLLERFACTORY_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QString>

class IActuatorController;

class ActuatorControllerFactory
{

public:
	ActuatorControllerFactory();

public:
	QSharedPointer<IActuatorController> controllerFactory(QString type);
};

#endif
// ACTUATORCONTROLLERFACTORY_HPP
