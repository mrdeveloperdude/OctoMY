#include "AddressBook.hpp"

#include "utility/Utility.hpp"

#include <QFile>
#include <QThreadPool>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QVariantMap>
#include <QVariantList>

AddressBook::AddressBook(QString filename, QObject *parent)
	: QObject(parent)
	, SimpleDataStore(filename)
{
	OC_METHODGATE();
	setObjectName("AddressBook");
}



AddressBook::~AddressBook()
{
	OC_METHODGATE();
}


bool AddressBook::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	mAssociates.clear();
	QVariantList peers=data["peers"].toList();
	for(QVariantList::iterator b=peers.begin(), e=peers.end(); b!=e; ++b) {
		QSharedPointer<Associate> peer=QSharedPointer<Associate>(OC_NEW Associate((*b).toMap()));
		upsertAssociate(peer);
	}
	return true;
}

QVariantMap AddressBook::toMap()
{
	OC_METHODGATE();
	QVariantMap map;
	QVariantList remotes;
	for(QMap<QString, QSharedPointer<Associate> >::const_iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
		remotes.push_back(b.value()->toVariantMap());
	}
	map["peers"]=remotes;
	return map;
}


bool AddressBook::fromDefault()
{
	OC_METHODGATE();
	mAssociates.clear();
	return true;
}


bool AddressBook::hasAssociate(const QString &id)
{
	OC_METHODGATE();
	return (mAssociates.find(id)!=mAssociates.end());
}


int AddressBook::associateCount() const
{
	OC_METHODGATE();
	return mAssociates.size();
}

QSharedPointer<Associate> AddressBook::associateByID(const QString &id)
{
	OC_METHODGATE();
	if(hasAssociate(id)) {
		return mAssociates[id];
	}
	QSharedPointer<Associate> ret;
	return ret;
}


QSharedPointer<Associate> AddressBook::removeAssociate(const QString &id)
{
	OC_METHODGATE();
	QSharedPointer<Associate> ret;
	if(hasAssociate(id)) {
		ret=mAssociates[id];
		mAssociates.remove(id);
		emit associateRemoved(id);
		emit associatesChanged();
	}
	return ret;
}



void AddressBook::upsertAssociate(QSharedPointer<Associate> associate)
{
	OC_METHODGATE();
	if(nullptr!=associate) {
		auto id=associate->id();
		const bool isNew=!hasAssociate(id);
		//qDebug().noquote().nospace()<<(isNew?"REGISTERING NEW":"UPDATING EXISTING")<< " ASSOCIATE WITH ID: "<<id;
		mAssociates[id]=associate;
		if(isNew) {
			emit associateAdded(id);
			emit associatesChanged();
		}
	} else {
		qWarning()<<"ASSOCIATE WAS NULL";
	}
}


QMap<QString, QSharedPointer<Associate> > &AddressBook::all()
{
	OC_METHODGATE();
	return mAssociates;
}


// Forward the async storeReady signal
void AddressBook::setHookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if(hook) {
		if(!connect(this,SIGNAL(associateAdded(QString)),&ob,SLOT(onAssociateAdded(QString)),OC_CONTYPE)) {
			qWarning()<<"Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(associateRemoved(QString)),&ob,SLOT(onAssociateRemoved(QString)),OC_CONTYPE)) {
			qWarning()<<"Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(associatesChanged()),&ob,SLOT(onAssociateChanged()),OC_CONTYPE)) {
			qWarning()<<"Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(this,SIGNAL(associateAdded(QString)),&ob,SLOT(onAssociateAdded(QString)))) {
			qWarning()<<"Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(associateRemoved(QString)),&ob,SLOT(onAssociateRemoved(QString)))) {
			qWarning()<<"Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(associatesChanged()),&ob,SLOT(onAssociateChanged()))) {
			qWarning()<<"Could not disconnect "<<ob.objectName();
		}
	}
}




const QDebug &operator<<(QDebug &d, AddressBook &ab)
{
	OC_FUNCTIONGATE();
	d.nospace() <<"AddressBook{ fn="<<ab.filename()<<", fexists="<<ab.fileExists()<<", ready="<<(const bool)ab.ready()<<", peers:[";
	for(QMap<QString, QSharedPointer<Associate> >::iterator b=ab.mAssociates.begin(), e=ab.mAssociates.end(); b!=e; ++b) {
		QString key=b.key();
		//b.value();
		d.nospace()<<" + " <<key;
	}
	d.nospace() <<"]}";
	return d.maybeSpace();
}
