#include "ClientFactory.hpp"

#include "discovery/AddressBook.hpp"
#include "Client.hpp"
#include "AgentClient.hpp"
#include "HubClient.hpp"
#include "RemoteClient.hpp"
#include "node/NodeType.hpp"


ClientFactory::ClientFactory(AddressBook &ab)
	: mAddressBook(ab)
{

}



QSharedPointer<Client *> ClientFactory::clientFromID(QString id)
{
//mAddressBook.
	QSharedPointer<Client *> ret;
	return ret;
}



QSharedPointer<Client *> ClientFactory::clientFromType(NodeType t)
{
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
