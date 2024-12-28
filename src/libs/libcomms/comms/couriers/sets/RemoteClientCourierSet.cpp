#include "RemoteClientCourierSet.hpp"

#include "client/remote/RemoteClient.hpp"

#include "uptime/MethodGate.hpp"

RemoteClientCourierSet::RemoteClientCourierSet(QString fullID, RemoteClient &client)
	: CourierSet()
	, mClient(client)

{
	OC_METHODGATE();
	Q_UNUSED(fullID);
	Q_UNUSED(mClient);
}




RemoteClientCourierSet::~RemoteClientCourierSet()
{
	OC_METHODGATE();
}



