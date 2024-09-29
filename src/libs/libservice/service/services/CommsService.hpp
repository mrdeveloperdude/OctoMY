#ifndef COMMSSERVICE_HPP
#define COMMSSERVICE_HPP


#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QSharedPointer>

class CommsChannel;
class Node;

/**
 * @brief The CommsService class is what it says on the tin; basically a wrapper to make CommsChannel appear as a service.
 */

class CommsService: public ServiceWrapper<CommsChannel>
{
private:
	ConfigureHelper mConfigureHelper;

public:
	CommsService(QSharedPointer<CommsChannel> comms, QStringList dependencies= {});

public:
	void configure(/*QSharedPointer<Node> node*/);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<CommsChannel> wrapee, bool on, ServiceActivatedCallback callBack) override;

};

#endif
// COMMSSERVICE_HPP
