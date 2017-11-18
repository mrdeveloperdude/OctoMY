#include "ClientList.hpp"

#include "discovery/AddressBook.hpp"

#include "comms/CommsSessionDirectory.hpp"

#include "comms/CommsSession.hpp"
#include "basic/Associate.hpp"


ClientList::ClientList()
{
	OC_METHODGATE();
}



QMap<QString, QSharedPointer<Client> > ClientList::toMapByID()
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Client> > map;
	for(QSharedPointer<Client> client: *this) {
		if(!client.isNull()) {
			QSharedPointer<Associate> nodeAssociate=client->nodeAssociate();
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

void ClientList::syncToAddressBook(AddressBook &ab)
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Client> > existingMap=toMapByID();
	// Remove what isn't there
	for(QSharedPointer<Client> client: *this) {
		if(!client.isNull()) {
			QSharedPointer<Associate> nodeAssociate=client->nodeAssociate();
			if(!nodeAssociate.isNull()) {
				QString id=nodeAssociate->id();
				if(!ab.hasAssociate(id)) {
					existingMap.remove(id);
				}
			}
		}
	}
	// Add what's missing
	QMap<QString, QSharedPointer<Associate> > &all=ab.all();
	QList<QString> keys=all.keys();
	for(QString key: keys) {
		if(!existingMap.contains(key)) {
			//TODO: Handle node and nodeass
//			existingMap[key]=all[key]->toClient();
		}
	}
	// Make it so
	clear();
	append(existingMap.values());
}

QSharedPointer<Client> ClientList::byID(QString id)
{
	OC_METHODGATE();
	for(QSharedPointer<Client> client: *this) {
		if(!client.isNull()) {
			QSharedPointer<Associate> nodeAssociate=client->nodeAssociate();
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
		now=QDateTime::currentMSecsSinceEpoch();
	}
	//TODO: Turn into constant or setting:
	const quint64 lastActive=now-(1000*60);//One minute ago.
	QSet<QSharedPointer<CommsSession> > sessions = ( (honeyMoon)? sessionDirectory.all() : sessionDirectory.byActiveTime(lastActive));
	qDebug()<<"HoneyMoon enabled="<<honeyMoon<< " gave "<<sessions.size()<<" sessions";
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



QList<QWidget * > ClientList::widgets()
{
	OC_METHODGATE();
	int idx=0;
	QList<QWidget * > out;
	for(QSharedPointer<Client> client: *this) {
		QWidget *widget=nullptr;
		if(!client.isNull()) {
			widget=client->widget();
		}
		out[idx++]=widget;
	}
	return out;
}




void ClientList::setAllCouriersRegistered(const bool reg)
{
	OC_METHODGATE();
	for(QSharedPointer<Client> client: *this) {
		if(!client.isNull()) {
			client->setCourierRegistration(reg);
		}
	}
}


void ClientList::updateCourierRegistration()
{
	OC_METHODGATE();
	for(QSharedPointer<Client> client: *this) {
		if(!client.isNull()) {
			client->updateCourierRegistration();
		}
	}
}
