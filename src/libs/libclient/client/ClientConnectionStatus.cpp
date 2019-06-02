#include "ClientConnectionStatus.hpp"

#include "uptime/MethodGate.hpp"

#include "Client.hpp"

ClientConnectionStatus::ClientConnectionStatus(QSharedPointer<Client> c)
	: mClient(c)
{
	OC_METHODGATE();
}


bool ClientConnectionStatus::needsConnection()
{
	OC_METHODGATE();
	return (!mClient.isNull())?mClient->needsConnection():false;
}


bool ClientConnectionStatus::isConnected()
{
	OC_METHODGATE();
	return (!mClient.isNull())?mClient->isConnected():false;
}


void ClientConnectionStatus::setNeedsConnection(bool val)
{
	OC_METHODGATE();
	if(!mClient.isNull()) {
		mClient->setNeedsConnection(val);
	}
}


void ClientConnectionStatus::setConnected(bool val)
{
	OC_METHODGATE();
	if(!mClient.isNull()) {
		mClient->setConnected(val);
	}
}




