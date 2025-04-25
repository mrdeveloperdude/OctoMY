#ifndef COMMSSERVICE_HPP
#define COMMSSERVICE_HPP


#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include "uptime/SharedPointerWrapper.hpp"

class Comms;
class Node;

/**
 * @brief The CommsService class is what it says on the tin; basically a wrapper to make Comms appear as a service.
 */

class CommsService: public ServiceWrapper<Comms>
{
private:
	ConfigureHelper mConfigureHelper;

public:
	CommsService(QSharedPointer<Comms> comms, QStringList dependencies= {});

public:
	void configure(/*QSharedPointer<Node> node*/);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<Comms> wrapee, bool on, ServiceActivatedCallback callBack) override;

};

#endif
// COMMSSERVICE_HPP
