#include "ClientFactory.hpp"

#include "uptime/MethodGate.hpp"

#include "discovery/AddressBook.hpp"
#include "Client.hpp"
#include "client/agent/AgentClient.hpp"
#include "client/hub/HubClient.hpp"
#include "client/remote/RemoteClient.hpp"
#include "node/NodeType.hpp"


ClientFactory::ClientFactory(AddressBook &ab)
	: mAddressBook(ab)
{
	OC_METHODGATE();
}



QSharedPointer<Client *> ClientFactory::clientFromID(QString id)
{
	OC_METHODGATE();
	Q_UNUSED(id);
//mAddressBook.
	QSharedPointer<Client *> ret;
	return ret;
}



QSharedPointer<Client *> ClientFactory::clientFromType(NodeType t)
{
	OC_METHODGATE();
	Q_UNUSED(t);
	/*
	switch(t)	{
	case(TYPE_AGENT):
		return QSharedPointer<Client *> (new AgentClient());
	case(TYPE_REMOTE):
		return QSharedPointer<Client *> (new RemoteClient());
	case(TYPE_HUB):
		return QSharedPointer<Client *> (new HubClient());
	}
	*/
	return nullptr;
}
