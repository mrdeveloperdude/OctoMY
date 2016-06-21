#include "DiscoveryClientStore.hpp"

#include "utility/Utility.hpp"

#include <QFile>
#include <QThreadPool>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QVariantMap>
#include <QVariantList>

DiscoveryClientStore::DiscoveryClientStore(QObject *parent, QString fn)
	: QObject(parent)
	, ready(false)
	, error(false)
	, filename(fn)
{
	if(QFile(filename).exists()){
		load();
	}
}



DiscoveryClientStore::~DiscoveryClientStore()
{
	save();
}


void DiscoveryClientStore::bootstrap()
{
	// QThreadPool takes ownership and deletes runnable automatically after completion
	QThreadPool *tp=QThreadPool::globalInstance();
	if(0!=tp){
		const bool ret=tp->tryStart(new GenerateRunnable<DiscoveryClientStore>(*this));
		if(ret){
			//qDebug()<<"DiscoveryClientStore: Successfully started background thread";
			return;
		}
	}
	// Fall back to single threaded wday
	qDebug()<<"DiscoveryClientStore: Falling back to serial bootstrap";
	bootstrapWorker();
}


void DiscoveryClientStore::bootstrapWorker()
{
	QFile f(filename);
	if(!f.exists()){
		qDebug()<<"DiscoveryClientStore: no store file found, saving for first time";
		save();
	}
	load();
}


void DiscoveryClientStore::load()
{
	//qDebug()<<"DiscoveryClientStore: Loading from file";
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(filename);
	QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
	if (QJsonParseError::NoError != jsonError.error){
		qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<filename;
		error=true;
	}
	else{
		//qDebug()<<"PARSED JSON: "<<doc.toJson();
		QVariantMap map = doc.object().toVariantMap();
		QVariantList remotes=map["participants"].toList();
		for(QVariantList::iterator b=remotes.begin(), e=remotes.end(); b!=e; ++b){
			DiscoveryParticipant *peer=new DiscoveryParticipant((*b).toMap());
			peers[peer->ID]=peer;
		}
		ready=true;
	}
	emit discoveryStoreReady();
}

void DiscoveryClientStore::save()
{
	qDebug()<<"DiscoveryClientStore: Saving to file: "<<filename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	QVariantList remotes;
	for(QMap<QString, DiscoveryParticipant *>::const_iterator b=peers.begin(), e=peers.end(); b!=e; ++b){
		remotes.push_back(b.value()->toVariantMap());
	}
	map["participants"]=remotes;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	utility::stringToFile(filename,doc.toJson());
}




bool DiscoveryClientStore::hasParticipant(const QString &id)
{
	return (peers.find(id)!=peers.end());
}



DiscoveryParticipant * DiscoveryClientStore::getParticipant(const QString &id)
{
	if(hasParticipant(id)){
		return peers[id];
	}
	return nullptr;
}



void DiscoveryClientStore::setParticipant(DiscoveryParticipant *participant)
{
	if(nullptr!=participant){
		peers[participant->ID]=participant;
	}
}


QMap<QString, DiscoveryParticipant *> &DiscoveryClientStore::getParticipants()
{
	return peers;
}
