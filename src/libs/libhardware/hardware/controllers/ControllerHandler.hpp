#ifndef CONTROLLERHANDLER_HPP
#define CONTROLLERHANDLER_HPP

#include <QObject>

#include "hardware/controllers/IController.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class IController;
class Agent;

/**
 * @brief The ControllerHandler class is a wrapper around any controller implemtation
 * (subclass of IController) that fills in the common features to save implementers
 * from duplicating it for each ICohntroller instance.
 * 
 * Features are:
 *   Interpolating between targets during transition between poses when controller does not support this directly
 *   Calcualting final target positions by applying trims and reversal
 *   Persistence of controller config not handled by IController such as trim values, center point,reversal and enabled status
 */
class ControllerHandler : public QObject
{
	Q_OBJECT
private:
	QSharedPointer<Agent> mAgent;
	QSharedPointer<IController> mController;

public:
	explicit ControllerHandler(QObject *parent = nullptr);
	
public:
	void configure(QSharedPointer<Agent> controller);
	
	// Provide the name of the actuator controller selected for this agent
	QString controllerName();
	
	// Unload the currently active actuator controller
	void unloadController();
	// Load the currently active actuator controller
	void loadController();
	// Apply current config to controller
	void setControllerConfig();
	// Fetch current config from controller
	void getControllerConfig();
	// Unload and reload the currently active actuator controller from the stored configuration
	void reloadController();

	// Provide the currently active actuator controller. Options include ardumy, servotor32 etc.
	QSharedPointer<IController> controller();

// Convenience controller stuff
public:
	ACTUATOR_INDEX actuatorCount(bool includeLimp=true, bool includeUnlimp=true, bool includeEnabled=true, bool includeDisabled=true);
	
	void toggleLimpAll(bool limp);
	void toggleEnableAll(bool enable);
	
};

#endif // CONTROLLERHANDLER_HPP
