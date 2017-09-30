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
	save();
}



void AddressBook::bootstrapWorkerImpl()
{
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
				setParticipant(peer);
			}
			mReady=true;
		}
	}
	//qDebug()<<"EMITTING storeReady";
	emit addressbookReady(!mError);
}

void AddressBook::save()
{
	//qDebug()<<"NodeAssociateStore: Saving to file: "<<mFilename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	QVariantList remotes;
	for(QMap<QString, QSharedPointer<Associate> >::const_iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
		remotes.push_back(b.value()->toVariantMap());
	}
	map["peers"]=remotes;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	QString str=doc.toJson();

	utility::stringToFile(mFilename,str);
}




bool AddressBook::hasAssociate(const QString &id)
{
	return (mAssociates.find(id)!=mAssociates.end());
}


int AddressBook::associateCount() const
{
	return mAssociates.size();
}

QSharedPointer<Associate> AddressBook::associateByID(const QString &id)
{
	if(hasAssociate(id)) {
		return mAssociates[id];
	}
	QSharedPointer<Associate> ret;
	return ret;
}


QSharedPointer<Associate> AddressBook::removeAssociate(const QString &id)
{
	QSharedPointer<Associate> ret;
	if(hasAssociate(id)) {
		ret=mAssociates[id];
		mAssociates.remove(id);
		emit associateRemoved(id);
		emit associatesChanged();
	}
	return ret;
}



void AddressBook::setParticipant(QSharedPointer<Associate> participant)
{
	if(nullptr!=participant) {
		auto id=participant->id();
		qDebug()<<"REGISTERING PARTICIPANT WITH ID: "<<id;
		const bool isNew=!hasAssociate(id);
		mAssociates[id]=participant;
		if(isNew) {
			emit associateAdded(id);
			emit associatesChanged();
		}
	}
	else{
		qWarning()<<"PARTICIPANT WAS NULL";
	}
}


QMap<QString, QSharedPointer<Associate> > &AddressBook::all()
{
	return mAssociates;
}


// Forward the async storeReady signal
void AddressBook::hookSignals(QObject &ob)
{
	if(!connect(this, SIGNAL(addressbookReady(bool)), &ob, SLOT(onPeerStoreReady(bool)),OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<ob.objectName();
	} else {
		//qDebug()<<"HOOKING peerStoreReady";
	}
	if(!connect(this,SIGNAL(associateAdded(QString)),&ob,SLOT(onPeerAdded(QString)),OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(associateRemoved(QString)),&ob,SLOT(onPeerRemoved(QString)),OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(associatesChanged()),&ob,SLOT(onPeersChanged()),OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<ob.objectName();
	}
}


void AddressBook::unHookSignals(QObject &ob)
{
	if(!disconnect(this, SIGNAL(addressbookReady(bool)), &ob, SLOT(onPeerStoreReady(bool)))) {
		qWarning()<<"Could not disconnect "<<ob.objectName();
	} else {
		//qDebug()<<"UN-HOOKING peerStoreReady";
	}
	if(!disconnect(this,SIGNAL(associateAdded(QString)),&ob,SLOT(onPeerAdded(QString)))) {
		qWarning()<<"Could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(associateRemoved(QString)),&ob,SLOT(onPeerRemoved(QString)))) {
		qWarning()<<"Could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(associatesChanged()),&ob,SLOT(onPeersChanged()))) {
		qWarning()<<"Could not disconnect "<<ob.objectName();
	}
}
