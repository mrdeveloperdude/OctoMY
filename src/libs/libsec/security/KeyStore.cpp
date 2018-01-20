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


KeyStore::KeyStore(QString filename, bool doBootstrap, KeySecurityPolicy policy, QObject *parent)
	: QObject(parent)
	, SimpleDataStore(filename)
	, mDoBootstrap(doBootstrap)
	, mPolicy(policy)
{
	OC_METHODGATE();
	setObjectName("KeyStore");
	synchronize([this](SimpleDataStore &ds, bool ok) {

	});
}


KeyStore::~KeyStore()
{
	OC_METHODGATE();
}


bool KeyStore::fromMap(QVariantMap map)
{
	QSharedPointer<Key> localKey;
	bool ok=true;
	mLocalKey=nullptr;
	mAssociates.clear();
	if(map.contains("localKey")) {
		QVariantMap localMap=map["localKey"].toMap();
		QSharedPointer<Key> localKey(OC_NEW Key(localMap, false));
		if(!localKey->isValid(false)) {
			ok=false;
		} else {
			mLocalKey=localKey;
		}
	}
	if(ok) {
		QVariantList remoteList=map["remoteKeys"].toList();
		for(QVariantList::iterator b=remoteList.begin(), e=remoteList.end(); b!=e; ++b) {
			QVariantMap remote=(*b).toMap();
			if(remote.contains("key")) {
				QVariantMap  keyMap=remote["key"].toMap();
				QSharedPointer<Key> peerKey= QSharedPointer<Key>(OC_NEW Key(keyMap,true));
				if(!peerKey->isValid(true)) {
					qWarning()<<"ERROR: peer key was not valid";
					ok=false;
					break;
				}
				mAssociates[remote["id"].toString()]=peerKey;
			} else {
				qWarning()<<"ERROR: no key in remote keys";
				ok=false;
				break;
			}
		}
	}
	return ok;
}

QVariantMap KeyStore::toMap()
{
	QVariantMap map;
	// This is where we bootstrap by creating the local key if none exist
	if(nullptr==mLocalKey && mDoBootstrap) {
		mLocalKey=QSharedPointer<Key>(OC_NEW Key(mPolicy.bits()));
	}
	if(nullptr!=mLocalKey) {
		map["localKey"]=mLocalKey->toVariantMap(false);
	}
	QVariantList remotes;
	for(QMap<QString, QSharedPointer<Key> >::const_iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
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
	return map;
}


QSharedPointer<Key> KeyStore::localKey()
{
	OC_METHODGATE();
	return mLocalKey;
}


PortableID KeyStore::localPortableID()
{
	OC_METHODGATE();
	PortableID pid;
	//pid.setName("Arne");
	//pid.setGender("Male");
	if(nullptr!=mLocalKey) {
		pid.setID(mLocalKey->id());
	}
	pid.setBirthDate(QFileInfo(filename()).created().toMSecsSinceEpoch());
	//pid.setType(DiscoveryType type);
	return pid;
}


void KeyStore::dump()
{
	OC_METHODGATE();
	KeyStore &ks=*this;
	qDebug()<<"KEYSTORE DUMP:";
	qDebug().nospace() <<" + fn="<<ks.filename();
	qDebug().nospace() <<" + fexists="<<ks.fileExists();
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
	if(!ready() || (nullptr==mLocalKey)) {
		return QByteArray();
	}

	return mLocalKey->sign(source);
}


bool KeyStore::verify(const QByteArray &message, const QByteArray &signature)
{
	OC_METHODGATE();
	if(!ready()|| (nullptr==mLocalKey)) {
		return false;
	}
	return mLocalKey->verify(message, signature);
}


bool KeyStore::verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature)
{
	OC_METHODGATE();
	if(!ready()) {
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
	if(!ready()) {
		return false;
	}
	return (mAssociates.end()!=mAssociates.find(id));
}

void KeyStore::setPubKeyForID(const QString &pubkeyPEM)
{
	OC_METHODGATE();
	if(!ready()) {
		return;
	}
	QSharedPointer<Key> peer(OC_NEW Key(pubkeyPEM, true));
	OC_ASSERT(nullptr!=peer);
	mAssociates.insert(peer->id(), peer);
}

QSharedPointer<Key> KeyStore::pubKeyForID(const QString &id)
{
	OC_METHODGATE();
	if(!ready()) {
		qWarning()<<"WARNING: returning empty key for id="<<id<<" because keystore not ready..";
		return nullptr;
	}
	return mAssociates[id];
}

const QDebug &operator<<(QDebug &d, KeyStore &ks)
{
	OC_FUNCTIONGATE();
	d.nospace() <<"KeyStore{ fn="<<ks.filename()<<", fexists="<<ks.fileExists()<<", ready="<<(const bool)ks.ready()<<", peer-keys:[";
	for(QMap<QString, QSharedPointer<Key> >::iterator b=ks.mAssociates.begin(), e=ks.mAssociates.end(); b!=e; ++b) {
		QString key=b.key();
		//b.value();
		d.nospace()<<" + " <<key;
	}
	d.nospace() <<"]}";
	return d.maybeSpace();
}
