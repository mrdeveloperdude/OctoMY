#ifndef CONTROLLERFACTORY_H
#define CONTROLLERFACTORY_H

#include "uptime/SharedPointerWrapper.hpp"

#include <QString>

class IController;

class ControllerFactory
{

public:
	ControllerFactory();

public:
	QSharedPointer<IController> controllerFactory(QString type);
	QStringList availableControllers();
};

#endif
// ACTUATORCONTROLLERFACTORY_HPP
