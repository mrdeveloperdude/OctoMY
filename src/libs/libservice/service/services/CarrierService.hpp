#ifndef CARRIERSERVICE_HPP
#define CARRIERSERVICE_HPP


#include "comms/CommsCarrier.hpp"
#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QSharedPointer>

class Node;

/**
 * @brief The CarrierService class is what it says on the tin; basically a wrapper to make Carrier appear as a service.
 */

class CarrierService: public ServiceWrapper<CommsCarrier>
{
private:
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	CarrierService(QSharedPointer<CommsCarrier> carrier, QStringList dependencies= {});

public:
	void configure(QSharedPointer<Node> node);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<CommsCarrier> wrapee, bool on, ServiceActivatedCallback callBack) override;

};

#endif
// CARRIERSERVICE_HPP
