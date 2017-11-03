#ifndef CLIENTLIST_HPP
#define CLIENTLIST_HPP

#include "Client.hpp"

#include <QList>
#include <QSharedPointer>

/**
 * @brief The ClientList class is a convenience wrapper around QList to keep pointers to instances of Client
 */

class ClientList: public QList<QSharedPointer<Client> >
{
public:
	ClientList();
};

#endif // CLIENTLIST_HPP
