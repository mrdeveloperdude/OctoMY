#ifndef CLIENTFACTORY_HPP
#define CLIENTFACTORY_HPP


#include <QSharedPointer>
#include <QString>
//#include "NodeType.hpp"

class Client;
class AddressBook;
enum NodeType:quint8;

class ClientFactory
{
private:
	AddressBook &mAddressBook;
public:
	ClientFactory(AddressBook &ab);

	QSharedPointer<Client *> clientFromID(QString id);
	QSharedPointer<Client *> clientFromType(NodeType t);
};

#endif // CLIENTFACTORY_HPP
