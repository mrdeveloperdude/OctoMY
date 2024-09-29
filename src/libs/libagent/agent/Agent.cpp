#include "Agent.hpp"

#include "agent/AgentWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "app/AppContext.hpp"

#include "AgentConfigStore.hpp"

#include "hardware/controllers/IActuatorController.hpp"
#include "hardware/controllers/ActuatorControllerFactory.hpp"

#include <QDebug>


/*!
	\class Agent
	\brief The Agent application
	\since 1.0
	\ingroup app
	
	Agent represents the robot. It is a possibly autonomous entity that roams
	this world together with us.
	
	\sa AgentMain, Remote, Hub, Zoo
*/

Agent::Agent()
	: mNodeConfigureHelper("Agent", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	, mAgentConfigStore(OC_NEW AgentConfigStore())
{
	OC_METHODGATE();
	//qDebug()<<"Agent()";
	// NOTE: Please do not put code here that generates events. Instead put them in *Configure*() or *Activate*()
}

//! \fn
Agent::~Agent()
{
	OC_METHODGATE();
	//qDebug()<<"~Agent()";
}

//! \fn void Agent::nodeConfigure()
void Agent::nodeConfigure()
{
	OC_METHODGATE();
	//qDebug()<<"nodeConfigure()";
	if(mNodeConfigureHelper.configure()) {
		auto ctx=context();
		if(!ctx.isNull()) {
			//QSharedPointer<AgentWindow> mWindow;
			mAgentConfigStore->configure(ctx->baseDir()+ "/agent_config.json");

			//QSharedPointer<IActuatorController> mActuatorController;

		}
	} else {
		qWarning()<<"ERROR: No context, closing";
		emit nodeRequestExit(EXIT_FAILURE);
	}
}

//! \fn void Agent::nodeActivate()
void Agent::nodeActivate(const bool on)
{
	OC_METHODGATE();
	//qDebug()<<"nodeActivate(on="<<on<<")";
	if(on) {
		if(mNodeConfigureHelper.activate(on)) {
			// Initialize Agent Configuration Store

			mAgentConfigStore->activate(on);
			mAgentConfigStore->synchronize([](QSharedPointer<SimpleDataStore> store, bool ok) {
				//qDebug()<<"mAgentConfigStore synced";
				Q_UNUSED(store);
				Q_UNUSED(ok);
			});
			emit nodeActivateChanged(on);
		}
	} else {
		if(mNodeConfigureHelper.isActivatedAsExpected()) {
			if(!mWindow.isNull()) {
				mWindow.clear();
			}
			mAgentConfigStore->activate(on);
			emit nodeActivateChanged(on);
		}
	}
}



QSharedPointer<NodeWindow> Agent::nodeWindow()
{
	OC_METHODGATE();
	//qDebug()<<"nodeWindow()";
	if(mWindow.isNull()) {
		QSharedPointer<Agent> sp=qSharedPointerCast<Agent>(sharedThis());
		if(!sp.isNull()) {
			mWindow = QSharedPointer<AgentWindow>::create(nullptr);
			if(!mWindow.isNull()) {
				mWindow->nodeWindowConfigure(sp);
			} else {
				qWarning()<<"ERROR: Could not allocate AgentWindow";
			}
		} else {
			qWarning()<<"ERROR: Shared pointer to this was null for agent";
		}
	}
	return mWindow;
}


NodeRole Agent::nodeRole()
{
	OC_METHODGATE();
	return ROLE_AGENT;
}


NodeType Agent::nodeType()
{
	OC_METHODGATE();
	return TYPE_AGENT;
}


QSharedPointer<Node> Agent::sharedThis()
{
	OC_METHODGATE();
	//return QEnableSharedFromThis<Agent>::sharedFromThis();
	return sharedFromThis();
}


void Agent::setPanic(bool panic)
{
	OC_METHODGATE();
	if(mPanic != panic) {
		mPanic = panic;
		//qWarning()<<"Panic set to: "<<mPanic;
	}
}


bool Agent::panic()
{
	OC_METHODGATE();
	return mPanic;
}


QSharedPointer<AgentConfigStore> Agent::configurationStore()
{
	OC_METHODGATE();
	return mAgentConfigStore;
}


QSharedPointer<IActuatorController> Agent::actuatorController()
{
	OC_METHODGATE();
	return mActuatorController;
}


void Agent::unloadController()
{
	OC_METHODGATE();
	// Unload old controller
	if(nullptr != mActuatorController) {
		//qDebug()<<"Unloading old actuator controller";
		mActuatorController->setConnected(false);
		mActuatorController->deleteLater();
		mActuatorController=nullptr;
	}

}


void Agent::reloadController()
{
	OC_METHODGATE();
	unloadController();
	if(!mAgentConfigStore.isNull()) {
		QSharedPointer<AgentConfig> config = mAgentConfigStore->agentConfig();
		if(!config.isNull()) {
			QString controllerName = config->controllerName().trimmed();
			if(!controllerName.isEmpty()) {
				//qDebug()<<"Attempting to generate actuator controller of type "<<controllerName;
				ActuatorControllerFactory factory;
				mActuatorController=factory.controllerFactory(controllerName);
				if(!mActuatorController.isNull()) {
					//qDebug()<<"Actuator controller created, configuring";
					QVariantMap controllerConfig = config->controllerConfig();
					mActuatorController->setConfiguration(controllerConfig);
					mActuatorController->setConnected(true);
				}
			} else {
				qDebug()<<"ERROR: No actuator controller named in agent config";
			}
		} else {
			qDebug()<<"ERROR: No agent config";
		}
	} else {
		qDebug()<<"ERROR: No agent config store";
	}
}
