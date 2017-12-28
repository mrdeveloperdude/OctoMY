/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

//#include "qpolarssl/qpolarsslpki.hpp"
#include "KeyStore.hpp"
#include "utility/Utility.hpp"

#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>


/////////////////////////////////////////////////////////////////////////////


KeyStore::KeyStore(QString fn, KeySecurityPolicy policy, QObject *parent)
	: AsyncStore(fn, parent)
	, mPolicy(policy)
{
	OC_METHODGATE();
	setObjectName("KeyStore");
//qDebug()<<"KeyStore() file="<<fn;
	if(!connect( this, SIGNAL(storeReady(bool)), SIGNAL(keystoreReady(bool)), OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<objectName();
	} else {
		//qDebug()<<"FORWARDING storeReady -> keystoreReady";
	}
}


KeyStore::~KeyStore()
{
	OC_METHODGATE();
	save();
}


void KeyStore::bootstrapWorkerImpl()
{
	OC_METHODGATE();
	//qDebug()<<"KeyStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()) {
		//qDebug()<<"KEYSTORE: no keystore file found, generating local keypair and saving";
		mLocalKey=QSharedPointer<Key>(OC_NEW Key(mPolicy.bits()));
		save();
	}
	load();
}


void KeyStore::load()
{
	OC_METHODGATE();
	//qDebug()<<"KeyStore() load="<<mFilename;
	if(!QFile(mFilename).exists()) {
		//qDebug() << "File did not exist"<<mFilename;
		//Silently ignore missing file
		//mError=true;
	} else {
		//qDebug()<<"KEYSTORE: Loading from file: " << *this;
		QJsonParseError jsonError;
		QByteArray raw=utility::fileToByteArray(mFilename);
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<mFilename;
			mError=true;
		} else {
			//qDebug()<<"PARSING JSON: "<<doc.toJson();
			QVariantMap map = doc.object().toVariantMap();
			QVariantMap localMap=map["localKey"].toMap();
			//qDebug()<<"LOCAL MAP: "<<localMap;
			mLocalKey=QSharedPointer<Key>(OC_NEW Key(localMap, false));
			if(!mLocalKey->isValid(false)) {
				qWarning()<<"ERROR: local key was not valid";
				mError=true;
			} else {
				QVariantList remotes=map["remoteKeys"].toList();
				mAssociates.clear();
				for(QVariantList::iterator b=remotes.begin(), e=remotes.end(); b!=e; ++b) {
					QVariantMap remote=(*b).toMap();
					if(remote.contains("key")) {
						QVariantMap  keyMap=remote["key"].toMap();
						//qDebug()<<"Loading key from: "<<keyMap;
						QSharedPointer<Key> peerKey= QSharedPointer<Key>(OC_NEW Key(keyMap,true));
						if(!peerKey->isValid(true)) {
							qWarning()<<"ERROR: peer key was not valid";
							mError=true;
							break;
						}
						mAssociates[remote["id"].toString()]=peerKey;
					} else {
						qWarning()<<"ERROR: key had malformed json";
						mError=true;
						break;
					}
				}
			}
			mReady=true;
			//qDebug()<<"RESULT AFTER LOAD: "<<this;
		}
	}
	//qDebug()<<"EMITTING KEYSTORE READY WITH"<<(mError?"ERROR":"A-OK");
	emit storeReady(!mError);
}

void KeyStore::save()
{
	OC_METHODGATE();
	//qDebug()<<"KeyStore() save="<<mFilename;
	//qDebug()<<"KEYSTORE: Saving to file: "<<*this;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	if(nullptr!=mLocalKey) {
		map["localKey"]=mLocalKey->toVariantMap(false);
	}
	QVariantList remotes;
	for(QMap<QString, QSharedPointer<Key> >::iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
		QVariantMap remote;
		QString id=b.key();
		auto key=b.value();
		if(nullptr!=key) {
			QVariantMap val=key->toVariantMap(true);
			qDebug()<<"SAVING REMOTE KEYPAIR "<<id<<"="<<val;
			remote["id"]=id;
			remote["key"]=val;
			remotes.push_back(remote);
		}
	}
	map["remoteKeys"]=remotes;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	QString raw=doc.toJson();
	//qDebug()<<"SAVING KEYSTORE JSON: "<<raw;
	utility::stringToFile(mFilename,raw);
}


void KeyStore::clear()
{
	OC_METHODGATE();
	//qDebug()<<"KeyStore() clear="<<mFilename;
	QFile file(mFilename);
	if(file.exists()) {
		if(file.remove()) {
			//qDebug()<<"KEYSTORE: Cleared: "<<*this;
			mLocalKey=nullptr;
			mAssociates.clear();
			mReady=false;
			mError=false;
		} else {
			qWarning()<<"ERROR: Could not clear "<<*this;
		}
	} else {
		//qDebug()<<"KEYSTORE: Could not clear missing file: "<< *this;
	}

}


void KeyStore::setHookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	qDebug()<<"KEY HOOK "<<ob.objectName()<<" = "<<hook;
	if(hook) {
		if(!connect(this, SIGNAL(keystoreReady(bool)), &ob, SLOT(onKeystoreReady(bool)),OC_CONTYPE)) {
			qWarning()<<"Could not connect "<<ob.objectName();
		} else {
			//qDebug()<<"HOOKING keystoreReady";
		}
	} else {
		if(!disconnect(this, SIGNAL(keystoreReady(bool)), &ob, SLOT(onKeystoreReady(bool)))) {
			qWarning()<<"Could not disconnect "<<ob.objectName();
		} else {
			//qDebug()<<"UN-HOOKING keystoreReady";
		}
	}

}

void KeyStore::dump()
{
	OC_METHODGATE();
	KeyStore &ks=*this;
	qDebug()<<"KEYSTORE DUMP:";
	qDebug().nospace() <<" + fn="<<ks.mFilename;
	qDebug().nospace() <<" + fexists="<<ks.fileExists();
	qDebug().nospace() <<" + ready="<<(const bool)ks.mReady;
	qDebug().nospace() <<" + inProgress="<<(const bool)ks.mInProgress;
	qDebug().nospace() <<" + error="<<(const bool)ks.mError;
	qDebug().nospace() <<" + peer-keys:";
	for(QMap<QString, QSharedPointer<Key> >::iterator b=ks.mAssociates.begin(), e=ks.mAssociates.end(); b!=e; ++b) {
		QString key=b.key();
		//b.value();
		qDebug().nospace()<<"    x " <<key;
	}
}


QByteArray KeyStore::sign(const QByteArray &source)
{
	OC_METHODGATE();
	if(!mReady || (nullptr==mLocalKey)) {
		return QByteArray();
	}

	return mLocalKey->sign(source);
}


bool KeyStore::verify(const QByteArray &message, const QByteArray &signature)
{
	OC_METHODGATE();
	if(!mReady|| (nullptr==mLocalKey)) {
		return false;
	}
	return mLocalKey->verify(message, signature);
}


bool KeyStore::verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature)
{
	OC_METHODGATE();
	if(!mReady) {
		return false;
	}
	QMap<QString, QSharedPointer<Key> >::iterator f=mAssociates.find(fingerprint);
	if(mAssociates.end()==f) {
		return false;
	}
	QSharedPointer<Key> remote=f.value();
	OC_ASSERT(nullptr!=remote);
	return remote->verify(message, signature);
}


bool KeyStore::hasPubKeyForID(const QString &id)
{
	OC_METHODGATE();
	if(!mReady) {
		return false;
	}
	return (mAssociates.end()!=mAssociates.find(id));
}

void KeyStore::setPubKeyForID(const QString &pubkeyPEM)
{
	OC_METHODGATE();
	if(!mReady) {
		return;
	}
	QSharedPointer<Key> peer(OC_NEW Key(pubkeyPEM, true));
	OC_ASSERT(nullptr!=peer);
	mAssociates.insert(peer->id(), peer);
}

QSharedPointer<Key> KeyStore::pubKeyForID(const QString &id)
{
	OC_METHODGATE();
	if(!mReady) {
		qWarning()<<"WARNING: returning empty key for id="<<id<<" because keystore not ready..";
		return nullptr;
	}
	return mAssociates[id];
}

const QDebug &operator<<(QDebug &d, KeyStore &ks)
{
	OC_FUNCTIONGATE();
	d.nospace() <<"KeyStore{ fn="<<ks.mFilename<<", fexists="<<ks.fileExists()<<", ready="<<(const bool)ks.mReady<<", inProgress="<<(const bool)ks.mInProgress<<", error="<<(const bool)ks.mError<<", peer-keys:[";
	for(QMap<QString, QSharedPointer<Key> >::iterator b=ks.mAssociates.begin(), e=ks.mAssociates.end(); b!=e; ++b) {
		QString key=b.key();
		//b.value();
		d.nospace()<<" + " <<key;
	}
	d.nospace() <<"]}";
	return d.maybeSpace();
}
