#ifndef CLIENTDIRECTORY_HPP
#define CLIENTDIRECTORY_HPP

#include "basic/NetworkAddress.hpp"
#include "comms/ClientSignature.hpp"

#include <QObject>
#include <QMap>
#include <QSet>
#include <QSharedPointer>


class Client;

/**
  * \brief ClientDirectory holds all clients beknownst to CommChannel.
  * \note This is NOT the same as NodeAssociate, as it is not meant as an addressbook
  * but rather as the working memory during opperation (for lack of better words)
  *
  */

class ClientDirectory: public QObject{
		Q_OBJECT
	private:
		QMap<quint64, QSharedPointer<Client> > byID;
		QMap<QString, QSharedPointer<Client> > byHost;
		QSet<QSharedPointer<Client> > all;
	public:

		explicit ClientDirectory():QObject(0){}
	public:

		void insert(QSharedPointer<Client> c);
		QSharedPointer<Client> getByID(const quint64 id, const bool addIfMissing=false);
		QSharedPointer<Client> getByAddress(const NetworkAddress &address, const bool addIfMissing=false);
		QSharedPointer<Client> getBySignature(const ClientSignature &sig, const bool addIfMissing=false);

		QSet<QSharedPointer<Client> > getByActiveTime(quint64 lastActiveTime);

		int count();

	signals:
		void clientAdded(QSharedPointer<Client> );


};


#endif // CLIENTDIRECTORY_HPP
