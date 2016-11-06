#include "ClientDirectory.hpp"

#include "Client.hpp"



void ClientDirectory::insert(QSharedPointer<Client> c)
{
	quint64 id=c->signature.shortHandID();
	QString address=c->signature.address().toString();
	if(nullptr!=c && !byID.contains(id) && !byHost.contains(address)) {
		byID.insert(id, c);
		byHost.insert(address, c);
		all.insert(c);
		emit clientAdded(c);
	} else {
		qWarning()<<"ERROR: could not insert client: "<<c;
	}
}

QSharedPointer<Client> getByID(const QString &id, const bool addIfMissing=false);
QSharedPointer<Client> getByHost(const NetworkAddress address, const bool addIfMissing=false);
QSharedPointer<Client> getBest(const ClientSignature sig, const bool addIfMissing=false);
QSharedPointer<Client> getBest(const quint32 platform, const quint32 executable, const QHostAddress host,const  quint16 port, const bool addIfMissing=false);


QSharedPointer<Client> ClientDirectory::getByID(const quint64 id, const bool addIfMissing)
{
	QMap<quint64, QSharedPointer<Client> >::const_iterator it=byID.find(id);
	if(byID.end()==it) {
		if(addIfMissing) {
			QSharedPointer<Client> c(new Client(ClientSignature(id, NetworkAddress())));
			insert(c);
			return c;
		}
		return QSharedPointer<Client>(nullptr);
	}
	return it.value();
}





QSharedPointer<Client> ClientDirectory::getByAddress(const NetworkAddress &address, const bool addIfMissing)
{
	QMap<QString, QSharedPointer<Client> >::const_iterator it=byHost.find(address.toString());
	if(byHost.end()==it) {
		if(addIfMissing) {
			QSharedPointer<Client> c(new Client(ClientSignature("", address)));
			insert(c);
			return c;
		}
		return QSharedPointer<Client>(nullptr);
	}
	return it.value();
}


QSharedPointer<Client> ClientDirectory::getBySignature(const ClientSignature &signature, const bool addIfMissing)
{
	QSharedPointer<Client> c=getByAddress(signature.address(), false);
	if(nullptr==c) {
		c=getByID(signature.shortHandID(), false);
	}
	if(nullptr==c && addIfMissing) {
		c=QSharedPointer<Client>(new Client(signature));
		insert(c);
	}
	return c;
}

QSet<QSharedPointer<Client> > ClientDirectory::getByActiveTime(quint64 lastActiveTime)
{
	QSet<QSharedPointer<Client> > ret;
	for(QSharedPointer<Client> client:all) {
		if(nullptr!=client) {
			if(client->lastActiveTime()>=lastActiveTime) {
				ret << client;
			}
		}
	}
	return ret;
}

int ClientDirectory::count()
{
	return all.size();
}
