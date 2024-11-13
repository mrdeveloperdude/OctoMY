#include "AgentConfigService.hpp"


#include "agent/AgentConfigStore.hpp"


AgentConfigService::AgentConfigService(QSharedPointer<AgentConfigStore> agentConfig, QStringList dependencies)
	: ServiceWrapper<AgentConfigStore>(agentConfig, "AgentConfig", dependencies)
	, mConfigureHelper("AgentConfigService", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
{
	OC_METHODGATE();
	qDebug() << "ACS CTOR";
}


void AgentConfigService::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		qDebug() << "ACS CONFIG";
	}
}


void AgentConfigService::serviceWrapperActivate(QSharedPointer<AgentConfigStore> agentConfig, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	qDebug() << "ACS ACTIVATE" << on;
	if(on) {
		if(mConfigureHelper.activate(on)) {
			// Initialize Agent Configuration Store
			agentConfig->activate(on, [ agentConfig, callBack, on](bool ok) {
				if(nullptr!=callBack) {
					callBack(on, ok);
				}
			});
			agentConfig->load();
		}
	} else {
		if(mConfigureHelper.isActivatedAsExpected()) {
			agentConfig->save();
			agentConfig->activate(on, [ agentConfig, callBack, on](bool ok) {
				if(nullptr!=callBack) {
					callBack(on, ok);
				}
			});
			mConfigureHelper.activate(on);
		}
	}
}
