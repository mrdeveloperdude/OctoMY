#include "RemoteClientCourierSet.hpp"

#include "client/remote/RemoteClient.hpp"

#include "uptime/MethodGate.hpp"

RemoteClientCourierSet::RemoteClientCourierSet(QString fullID, RemoteClient &client)
	: CourierSet()
	, mClient(client)

{
	OC_METHODGATE();
}




RemoteClientCourierSet::~RemoteClientCourierSet()
{
	OC_METHODGATE();
}



