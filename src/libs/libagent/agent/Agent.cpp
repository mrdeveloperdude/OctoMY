#include "Agent.hpp"

#include "agent/AgentWindow.hpp"

#include "AgentConfigStore.hpp"
#include "app/AppContext.hpp"
#include "hardware/controllers/ControllerHandler.hpp"
#include "service/ServiceLevel.hpp"
#include "service/ServiceLevelManager.hpp"
#include "service/services/AgentConfigService.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>


/*!
	\class Agent
	\brief The Agent application
	\since 1.0
	\ingroup app
	
	Agent represents the robot. It is a possibly autonomous entity that roams
	this world together with us.
	
	\sa Node, AgentWindow, AgentMain, Remote, Hub, Zoo
*/

Agent::Agent()
	: mNodeConfigureHelper("Agent", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	, mAgentConfigStore(OC_NEW AgentConfigStore())
	, mAgentConfigService(OC_NEW AgentConfigService(mAgentConfigStore, QStringList{}))
	, mControllerHandler(QSharedPointer<ControllerHandler>::create())
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
		auto ctx = context();
		if(!ctx.isNull()) {
			mAgentConfigStore->configure(ctx->baseDir() + "/agent_config.json");
		}
		mAgentConfigService->configure();
		// Sneak our little service into the cogs of the "bog boy" service management system set up in node
		auto sm = serviceLevelManager();
		sm->registerService(mAgentConfigService);
		auto sl = sm->serviceLevelByName("Always");
		if(!sl.isNull()){
			sl->toggleService(mAgentConfigService->name(), true);
		}
		else{
			qWarning() << "";
		}
		if(!mControllerHandler.isNull()){
			mControllerHandler->configure(qSharedPointerCast<Agent>(sharedThis()));
		}
	} else {
		qWarning() << "ERROR: No context, closing";
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
			emit nodeActivateChanged(on);
		}
	} else {
		if(mNodeConfigureHelper.isActivatedAsExpected()) {
			if(!mWindow.isNull()) {
				mWindow.clear();
			}
			emit nodeActivateChanged(on);
			mNodeConfigureHelper.activate(on);
		}
	}
}


QSharedPointer<NodeWindow> Agent::nodeWindow()
{
	OC_METHODGATE();
	//qDebug()<<"nodeWindow()";
	if(mWindow.isNull()) {
		QSharedPointer<Agent> sp = qSharedPointerCast<Agent>(sharedThis());
		if(!sp.isNull()) {
			mWindow = QSharedPointer<AgentWindow>::create(nullptr);
			if(!mWindow.isNull()) {
				mWindow->nodeWindowConfigure(sp);
			} else {
				qWarning() << "ERROR: Could not allocate AgentWindow";
			}
		} else {
			qWarning() << "ERROR: Shared pointer to this was null for agent";
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
	// NOTE: don't cast here, cast at the site of use. This is to make all nodes "shared this compatible"
	return sharedFromThis();
}


QSharedPointer<AgentConfigStore> Agent::configurationStore()
{
	OC_METHODGATE();
	return mAgentConfigStore;
}


QSharedPointer<AgentConfig> Agent::configuration(){
	OC_METHODGATE();
	auto configStore = configurationStore();
	if(!configStore.isNull()) {
		return configStore->agentConfig();
	}
	return nullptr;
}



QSharedPointer<ControllerHandler> Agent::controllerHandler(){
	OC_METHODGATE();
	return mControllerHandler;
}
