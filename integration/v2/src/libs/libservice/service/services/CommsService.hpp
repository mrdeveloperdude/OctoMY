#ifndef COMMSSERVICE_HPP
#define COMMSSERVICE_HPP


#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

class CommsChannel;

/**
 * @brief The CommsService class is what it says on the tin; basically a wrapper to make CommsChannel appear as a service.
 */

class CommsService: public ServiceWrapper<CommsChannel>
{
public:
	CommsService(QSharedPointer<CommsChannel> comms, QStringList dependencies= {});

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<CommsChannel> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};

#endif
// COMMSSERVICE_HPP
