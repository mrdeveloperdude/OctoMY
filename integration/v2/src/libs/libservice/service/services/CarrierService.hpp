#ifndef CARRIERSERVICE_HPP
#define CARRIERSERVICE_HPP


#include "comms/CommsCarrier.hpp"
#include "service/ServiceWrapper.hpp"

#include <QSharedPointer>

/**
 * @brief The CarrierService class is what it says on the tin; basically a wrapper to make Carrier appear as a service.
 */

class CarrierService: public ServiceWrapper<CommsCarrier>
{
public:
	CarrierService(QSharedPointer<CommsCarrier> carrier, QStringList dependencies= {});

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<CommsCarrier> wrapee, bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;

};

#endif
// CARRIERSERVICE_HPP
