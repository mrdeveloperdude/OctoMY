#include "HubClientCourierSet.hpp"

#include "uptime/MethodGate.hpp"

#include "comms/CommsSignature.hpp"

#include "client/hub/HubClient.hpp"



HubClientCourierSet::HubClientCourierSet()
	: CourierSet()

{
	OC_METHODGATE();
}


HubClientCourierSet::~HubClientCourierSet()
{
	OC_METHODGATE();
}


void HubClientCourierSet::configure(QString fullID, QSharedPointer<HubClient> client)
{
	OC_METHODGATE();
	// TODO: implement
	Q_UNUSED(fullID);
	Q_UNUSED(client);
}



