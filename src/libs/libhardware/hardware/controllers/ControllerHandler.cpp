#include "ControllerHandler.hpp"

#include "agent/Agent.hpp"
#include "agent/AgentConfig.hpp"
#include "agent/AgentConfigStore.hpp"
#include "hardware/controllers/ControllerFactory.hpp"
#include "hardware/controllers/IController.hpp"
#include "uptime/MethodGate.hpp"


ControllerHandler::ControllerHandler(QObject *parent)
	: QObject{parent}
{}


void ControllerHandler::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
}


QString ControllerHandler::controllerName()
{
	if(!mAgent.isNull()){
		auto configStore = mAgent->configurationStore();
		if(!configStore.isNull()) {
			auto config = configStore->agentConfig();
			if(!config.isNull()) {
				return config->controllerName();
			}
		}
	}
	return QString();
}


void ControllerHandler::unloadController()
{
	OC_METHODGATE();
	// Unload old controller
	if(nullptr != mController) {
		//qDebug()<<"Unloading old actuator controller";
		mController->setConnected(false);
		mController = nullptr;
		emit controllerChanged();
	}
}


void ControllerHandler::loadController()
{
	OC_METHODGATE();
	if(mController.isNull()) {
		if(!mAgent.isNull()) {
			auto config = mAgent->configuration();
			if(!config.isNull()) {
				QString controllerName = config->controllerName().trimmed();
				if(!controllerName.isEmpty()) {
					ControllerFactory factory;
					auto controller = factory.controllerFactory(controllerName);
					if(mController != controller){
						// Store config for old controller
						storeControllerConfig();
						mController = controller;
						// Load config for new controller
						loadControllerConfig();
						emit controllerChanged();
					}
				} else {
					qWarning() << "ERROR: No actuator controller named in agent config";
				}
			} else {
				qWarning() << "ERROR: No agent config";
			}
		} else {
			qWarning() << "ERROR: No agent config store";
		}
	}
}


void ControllerHandler::loadControllerConfig()
{
	if(!mController.isNull()) {
		if(!mAgent.isNull()) {
			auto config = mAgent->configuration();
			if(!config.isNull()) {
				auto controllerConfig = config->controllerConfig();
				mController->setConfiguration(controllerConfig);
			} else {
				qWarning() << "ERROR: No agent config";
			}
		} else {
			qWarning() << "ERROR: No agent config store";
		}
	}
}


void ControllerHandler::storeControllerConfig()
{
	if(!mController.isNull()) {
		if(!mAgent.isNull()) {
			auto config = mAgent->configuration();
			if(!config.isNull()) {
				auto controllerConfig = mController->configuration();
				config->setControllerConfig(controllerConfig);
			} else {
				qWarning() << "ERROR: No agent config";
			}
		} else {
			qWarning() << "ERROR: No agent config store";
		}
	}
}


void ControllerHandler::reloadController()
{
	OC_METHODGATE();
	unloadController();
	loadController();
}


QSharedPointer<IController> ControllerHandler::controller()
{
	OC_METHODGATE();
	return mController;
}


// Interface
/////////////////////////////////////////////////////////////////////////


ACTUATOR_INDEX ControllerHandler::actuatorCount(bool includeLimp, bool includeUnlimp, bool includeEnabled, bool includeDisabled)
{
	OC_METHODGATE();
	ACTUATOR_INDEX ret{0};
	if(!mController.isNull()){
		auto total = mController->actuatorCount();
		for(ACTUATOR_INDEX index = 0; index < total; ++index){
			const auto limp = mController->isLimp(index);
			const auto enabled = mController->isEnabled(index);
			if((includeLimp && limp) || (includeUnlimp && !limp) || (includeEnabled && enabled) || (includeDisabled && !enabled) ){
				ret++;
			}
		}
	}
	return ret;
}


void ControllerHandler::toggleLimpAll(bool limp)
{
	OC_METHODGATE();
	if(!mController.isNull()){
		auto total = mController->actuatorCount();
		for(ACTUATOR_INDEX index = 0; index < total; ++index){
			mController->setLimp(index, limp);
		}
	}
}


void ControllerHandler::toggleEnableAll(bool enable)
{
	OC_METHODGATE();
	if(!mController.isNull()){
		auto total = mController->actuatorCount();
		for(ACTUATOR_INDEX index = 0; index < total; ++index){
			mController->setEnabled(index, enable);
		}
	}
}


bool ControllerHandler::actuatorsSupported()
{
	OC_METHODGATE();
	if(!mController.isNull()){
		return mController->maxActuatorsSupported() > 0;
	}
	return false;
}


bool ControllerHandler::sensorsSupported()
{
	OC_METHODGATE();
	if(!mController.isNull()){
		return mController->maxSensorsSupported() > 0;
	}
	return false;
}


bool ControllerHandler::lobesSupported()
{
	OC_METHODGATE();
	if(!mController.isNull()){
		return mController->lobesSupported();
	}
	return false;
}
