#include "NodeAssociateStore.hpp"

#include "utility/Utility.hpp"

#include <QFile>
#include <QThreadPool>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QVariantMap>
#include <QVariantList>

NodeAssociateStore::NodeAssociateStore(QString fn, QObject *parent)
	: AsyncStore(fn, parent)
{
	qDebug()<<"NodeAssociateStore() file="<<fn;
}



NodeAssociateStore::~NodeAssociateStore()
{
	save();
}



void NodeAssociateStore::bootstrapWorkerImpl()
{
	qDebug()<<"KeyStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()){
		qDebug()<<"NodeAssociateStore: no keystore file found, saving";
		save();
	}
	load();
}


void NodeAssociateStore::load()
{
	qDebug()<<"NodeAssociateStore: Loading from file "<<mFilename;
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(mFilename);
	QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
	if (QJsonParseError::NoError != jsonError.error){
		qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<mFilename;
		mError=true;
	}
	else{
		//qDebug()<<"PARSED JSON: "<<doc.toJson();
		mPeers.clear();
		QVariantMap map = doc.object().toVariantMap();
		QVariantList peers=map["peers"].toList();
		for(QVariantList::iterator b=peers.begin(), e=peers.end(); b!=e; ++b){
			QSharedPointer<NodeAssociate> peer=QSharedPointer<NodeAssociate>(new NodeAssociate((*b).toMap()));
			setParticipant(peer);
		}
		mReady=true;
	}
	emit storeReady(!mError);
}

void NodeAssociateStore::save()
{
	qDebug()<<"NodeAssociateStore: Saving to file: "<<mFilename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	QVariantList remotes;
	for(QMap<QString, QSharedPointer<NodeAssociate> >::const_iterator b=mPeers.begin(), e=mPeers.end(); b!=e; ++b){
		remotes.push_back(b.value()->toVariantMap());
	}
	map["peers"]=remotes;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	utility::stringToFile(mFilename,doc.toJson());
}




bool NodeAssociateStore::hasParticipant(const QString &id)
{
	return (mPeers.find(id)!=mPeers.end());
}


const int NodeAssociateStore::getParticipantCount() const
{
	return mPeers.size();
}

QSharedPointer<NodeAssociate> NodeAssociateStore::getParticipant(const QString &id)
{
	if(hasParticipant(id)){
		return mPeers[id];
	}
	QSharedPointer<NodeAssociate> ret;
	return ret;
}


QSharedPointer<NodeAssociate> NodeAssociateStore::removeParticipant(const QString &id)
{
	QSharedPointer<NodeAssociate> ret;
	if(hasParticipant(id)){
		ret=mPeers[id];
		mPeers.remove(id);
		emit peerRemoved(id);
		emit peersChanged();
	}
	return ret;
}



void NodeAssociateStore::setParticipant(QSharedPointer<NodeAssociate> participant)
{
	if(nullptr!=participant){
		auto id=participant->id();
		//qDebug()<<"REGISTERING PARTICIPANT WITH ID: "<<id;
		const bool isNew=!hasParticipant(id);
		mPeers[id]=participant;
		if(isNew){
			emit peerAdded(id);
			emit peersChanged();
		}
	}
}


QMap<QString, QSharedPointer<NodeAssociate> > &NodeAssociateStore::getParticipants()
{
	return mPeers;
}



void NodeAssociateStore::hookSignals(QObject &ob){
	if(!connect(this, SIGNAL(storeReady(bool)), &ob, SLOT(onStoreReady(bool)),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(peerAdded(QString)),&ob,SLOT(onPeerAdded(QString)),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(peerRemoved(QString)),&ob,SLOT(onPeerRemoved(QString)),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(peersChanged()),&ob,SLOT(onPeersChanged()),OC_CONTYPE)){
		qDebug()<<"could not connect "<<ob.objectName();
	}
}


void NodeAssociateStore::unHookSignals(QObject &ob){
	if(!disconnect(this,SIGNAL(storeReady(bool)),&ob,SLOT(onStoreReady(bool)))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(peerAdded(QString)),&ob,SLOT(onPeerAdded(QString)))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(peerRemoved(QString)),&ob,SLOT(onPeerRemoved(QString)))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}
	if(!disconnect(this,SIGNAL(peersChanged()),&ob,SLOT(onPeersChanged()))){
		qDebug()<<"could not disconnect "<<ob.objectName();
	}
}
