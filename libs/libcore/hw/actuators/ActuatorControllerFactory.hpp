#ifndef ACTUATORCONTROLLERFACTORY_HPP
#define ACTUATORCONTROLLERFACTORY_HPP

#include <QString>

class IActuatorController;

class ActuatorControllerFactory
{
public:
	ActuatorControllerFactory();

public:

	IActuatorController *controllerFactory(QString type);
};

#endif // ACTUATORCONTROLLERFACTORY_HPP
