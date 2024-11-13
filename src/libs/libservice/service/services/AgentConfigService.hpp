#ifndef AGENTCONFIGSERVICE_H
#define AGENTCONFIGSERVICE_H


#include "service/ServiceWrapper.hpp"

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"


class AgentConfigStore;

/**
 * @brief The AgentConfigService class is what it says on the tin; basically a wrapper to make AgentConfigStore appear as a service.
 */

class AgentConfigService: public ServiceWrapper<AgentConfigStore>
{
private:
	ConfigureHelper mConfigureHelper;

public:
	AgentConfigService(QSharedPointer<AgentConfigStore> agentConfigStore, QStringList dependencies= {});

public:
	void configure();

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<AgentConfigStore> wrapee, bool on, ServiceActivatedCallback callBack) override;

};

#endif
// AGENTCONFIGSERVICE_HPP
