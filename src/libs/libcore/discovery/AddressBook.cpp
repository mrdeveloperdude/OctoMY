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

AddressBook::AddressBook(QString fn, QObject *parent)
	: AsyncStore(fn, parent)
{
	OC_METHODGATE();
	setObjectName("NodeAssociateStore");
	//qDebug()<<"NodeAssociateStore() file="<<fn;
	// Forward the async storeReady signal
	if(!connect( this, SIGNAL(storeReady(bool)), SIGNAL(addressbookReady(bool)), OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<objectName();
	} else {
		//qDebug()<<"FORWARDING storeReady -> peerStoreReady";
	}
}



AddressBook::~AddressBook()
{
	OC_METHODGATE();
	save();
}



void AddressBook::bootstrapWorkerImpl()
{
	OC_METHODGATE();
	//qDebug()<<"KeyStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()) {
		qDebug()<<"NodeAssociateStore: no keystore file found, saving";
		save();
	}
	load();
}


void AddressBook::load()
{
	OC_METHODGATE();
	//qDebug()<<"NodeAssociateStore: Loading from file "<<mFilename;
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(mFilename);
	if(raw.size()<1) {
		//Let empty data pass because it is valid the first time app starts
		qWarning() << "WARNING: Data read from file "<<mFilename<< " was empty";
		mReady=true;
	} else {
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<mFilename;
			mError=true;
		} else {
			//qDebug()<<"PARSED JSON: "<<doc.toJson();
			mAssociates.clear();
			QVariantMap map = doc.object().toVariantMap();
			QVariantList peers=map["peers"].toList();
			for(QVariantList::iterator b=peers.begin(), e=peers.end(); b!=e; ++b) {
				QSharedPointer<Associate> peer=QSharedPointer<Associate>(OC_NEW Associate((*b).toMap()));
				upsertAssociate(peer);
			}
			mReady=true;
		}
	}
	//qDebug()<<"EMITTING storeReady";
	emit addressbookReady(!mError);
}

void AddressBook::save()
{
	OC_METHODGATE();
	//qDebug()<<"NodeAssociateStore: Saving to file: "<<mFilename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	QVariantList remotes;
	for(QMap<QString, QSharedPointer<Associate> >::const_iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
		remotes.push_back(b.value()->toVariantMap());
	}
	map["peers"]=remotes;
	utility::variantToJsonFile(mFilename, map);
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
		if(!connect(this, SIGNAL(addressbookReady(bool)), &ob, SLOT(onAddressBookReady(bool)),OC_CONTYPE)) {
			qWarning()<<"Could not connect "<<ob.objectName();
		} else {
			//qDebug()<<"HOOKING peerStoreReady";
		}
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
		if(!disconnect(this, SIGNAL(addressbookReady(bool)), &ob, SLOT(onAddressBookReady(bool)))) {
			qWarning()<<"Could not disconnect "<<ob.objectName();
		} else {
			//qDebug()<<"UN-HOOKING peerStoreReady";
		}
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

