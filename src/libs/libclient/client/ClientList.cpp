#include "ClientList.hpp"

#include "address/Associate.hpp"
#include "comms/CommsSession.hpp"
#include "comms/CommsSessionDirectory.hpp"
#include "discovery/AddressBook.hpp"
#include "node/Node.hpp"
#include "security/Key.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"


ClientList::ClientList()
{
	OC_METHODGATE();
}


ClientList::~ClientList()
{
	OC_METHODGATE();
}


QMap<QString, QSharedPointer<Client> > ClientList::toMapByID()
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Client> > map;
	for(QSharedPointer<Client> client: mClients) {
		if(!client.isNull()) {
			QSharedPointer<Associate> nodeAssociate=client->associate();
			if(!nodeAssociate.isNull()) {
				QString key=nodeAssociate->id();
				if(map.contains(key)) {
					qWarning()<<"Found client with duplicate key for "<<key;
				}
				map[key]=client;
			}
		}
	}
	return map;
}


void ClientList::syncToAddressBook(QSharedPointer<AddressBook> ab, QSharedPointer<Node> node)
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Client> > existingMap=toMapByID();
	if(!node.isNull() && !ab.isNull()) {
		//qDebug()<<"Synchronizing client list to address book with "<<ab.all().size()<<" items";
		// Remove what isn't there
		for(QSharedPointer<Client> client: mClients) {
			if(!client.isNull()) {
				QSharedPointer<Associate> nodeAssociate=client->associate();
				if(!nodeAssociate.isNull()) {
					QString id=nodeAssociate->id();
					if(!ab->hasAssociate(id)) {
						qDebug()<<" + Removing client "<<id;
						existingMap.remove(id);
					}
				}
			}
		}
		// Add what's missing
		QMap<QString, QSharedPointer<Associate> > all=ab->all();
		QList<QString> keys=all.keys();
		for(QString key: keys) {
			if(!existingMap.contains(key)) {
				QSharedPointer<Associate> ass=all[key];
				if(!ass.isNull()) {
					QSharedPointer<Client> cl=ass->toClient(node);
					if(!cl.isNull()) {
						qDebug()<<" + Adding client "<<ass->toPortableID();
						existingMap[key]=cl;
					} else {
						qDebug()<<" + Adding client ERROR: "<<ass->toPortableID();
					}
				}
			}
		}
	}
	// Make it so
	mClients.clear();
	mClients.append(existingMap.values());
}


QSharedPointer<Client> ClientList::byID(QString id)
{
	OC_METHODGATE();
	for(QSharedPointer<Client> client: mClients) {
		if(!client.isNull()) {
			QSharedPointer<Associate> nodeAssociate=client->associate();
			if(!nodeAssociate.isNull()) {
				QString id2=nodeAssociate->id();
				if(id==id2) {
					return client;
				}
			}
		}
	}
	return nullptr;
}


QSet<QSharedPointer<Client> > ClientList::withActiveSessions(CommsSessionDirectory &sessionDirectory, AddressBook &ab, quint64 now, const bool honeyMoon)
{
	OC_METHODGATE();
	QSet<QSharedPointer<Client> > out;
	if(0==now) {
		now=utility::time::currentMsecsSinceEpoch<quint64>();
	}
	//TODO: Turn into constant or setting:
	const quint64 lastActive=now-(1000*60);//One minute ago.
	QSet<QSharedPointer<CommsSession> > sessions = ( (honeyMoon)? sessionDirectory.all() : sessionDirectory.byActiveTime(lastActive));
	//qDebug()<<"HoneyMoon enabled="<<honeyMoon<< " gave "<<sessions.size()<<" sessions";
	for(QSharedPointer<CommsSession> session: sessions) {
		auto key=session->key();
		if(nullptr!=key) {
			QString id=key->id();
			if(id.size()>0) {
				QSharedPointer<Associate> peer=ab.associateByID(id);
				if(nullptr!=peer) {
					if(NodeRole::ROLE_CONTROL==peer->role()) {
						QString id=peer->id();
						out<<byID(id);
					}
				}
			}
		} else {
			qWarning()<<"ERROR: No key";
		}
	}
	return out;
}


void ClientList::clear()
{
	OC_METHODGATE();
	mClients.clear();
}


QList< QSharedPointer<ClientWidget> > ClientList::widgets()
{
	OC_METHODGATE();
	int idx=0;
	QList< QSharedPointer<ClientWidget> > out;
	const int ct=mClients.count();
	//qDebug()<<"Preparing widgets from client list of "<<ct<<" clients";
	out.reserve(ct);
	for(QSharedPointer<Client> client: mClients) {
		QSharedPointer<ClientWidget> widget;
		if(!client.isNull()) {
			//qDebug()<<" + getting widget for " << client->associate()->name();
			widget=client->widget();
		}
		//qDebug()<<" + got widget: "<<widget<< " for client "<< client<<" idx="<<idx<<" ct="<<ct;
		// NOTE: We use insert(idx, widget) instead of operator<<() to maintan the exact order of clients
		out.insert(idx,widget);
		idx++;
	}
	return out;
}


int ClientList::count() const
{
	OC_METHODGATE();
	return mClients.count();
}


void ClientList::registerAllCouriers(const bool reg)
{
	OC_METHODGATE();
	for(QSharedPointer<Client> client: mClients) {
		if(!client.isNull()) {
			client->registerCouriers(reg);
		}
	}
}


void ClientList::updateCourierRegistration()
{
	OC_METHODGATE();
	for(QSharedPointer<Client> client: mClients) {
		if(!client.isNull()) {
			client->updateCourierRegistration();
		}
	}
}
