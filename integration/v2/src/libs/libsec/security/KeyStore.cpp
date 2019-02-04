/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

//#include "qpolarssl/qpolarsslpki.hpp"
#include "KeyStore.hpp"
#include "utility/string/String.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/Assert.hpp"

#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>


/////////////////////////////////////////////////////////////////////////////

KeyStore::KeyStore(QObject *parent)
	: QObject(parent)
	, mDirty(true)
	, mDoBootstrap(false)
{
	OC_METHODGATE();
	setObjectName("KeyStore");
}


KeyStore::~KeyStore()
{
	OC_METHODGATE();
}

void KeyStore::configure(QString filename, bool doBootstrap, KeySecurityPolicy policy)
{
	OC_METHODGATE();
	mBackend=QSharedPointer<JsonAsyncBackend>(OC_NEW JsonAsyncBackend());
	if(!mBackend.isNull()) {
		mBackend->configure(filename);
	}
	mStore.configure(mBackend, sharedFromThis());
	mDirty=(true);
	mDoBootstrap=(doBootstrap);
	mPolicy=(policy);

	synchronize([](ASEvent<QVariantMap> &se) {
		Q_UNUSED(se);
		//qDebug()<<"KeyStore::KeyStore() SELF ASSESMENT: SYNC WAS OK=";
	});
}


void KeyStore::clear()
{
	OC_METHODGATE();
	clear([](ASEvent<QVariantMap> &ase) {
		Q_UNUSED(ase);
	});
}

void KeyStore::save()
{
	OC_METHODGATE();
	save([](ASEvent<QVariantMap> &ase) {
		Q_UNUSED(ase);
	});
}

void KeyStore::load()
{
	OC_METHODGATE();
	load([](ASEvent<QVariantMap> &ase) {
		Q_UNUSED(ase);
	});
}


bool KeyStore::setFrontend(QVariantMap map)
{
	OC_METHODGATE();
	//qDebug()<<"KEYSTORE FROM MAP";
	mDirty=true;
	bool ok=true;
	mLocalKey=nullptr;
	mAssociates.clear();
	if(map.contains("localKey")) {
		QVariantMap localMap=map["localKey"].toMap();
		QSharedPointer<Key> localKey(OC_NEW Key(localMap, false));
		if(localKey->isValid(false)) {
			mLocalKey=localKey;
		} else {
			ok=false;
		}
	}
	if(ok) {
		QVariantList remoteList=map["remoteKeys"].toList();
		for(QVariantList::iterator b=remoteList.begin(), e=remoteList.end(); b!=e; ++b) {
			QVariantMap remote=(*b).toMap();
			if(remote.contains("key")) {
				QVariantMap  keyMap=remote["key"].toMap();
				QSharedPointer<Key> peerKey= QSharedPointer<Key>(OC_NEW Key(keyMap, true));
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

QVariantMap KeyStore::getFrontend(bool &ok)
{
	OC_METHODGATE();
	ok=true;
	if(mDirty) {
		mDirty=false;
		//qDebug()<<"KEYSTORE TO MAP";
		mCache = QVariantMap();
		if(!mLocalKey.isNull()) {
			mCache["localKey"]=mLocalKey->toVariantMap(false);
		}
		QVariantList remotes;
		for(QMap<QString, QSharedPointer<Key> >::const_iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
			QVariantMap remote;
			QString id=b.key();
			auto key=b.value();
			if(nullptr!=key) {
				QVariantMap val=key->toVariantMap(true);
				//qDebug()<<"SAVING REMOTE KEYPAIR "<<id<<"="<<val;
				remote["id"]=id;
				remote["key"]=val;
				remotes.push_back(remote);
			}
		}
		mCache["remoteKeys"]=remotes;
	}
	return mCache;
}



bool KeyStore::generateFrontend()
{
	OC_METHODGATE();
	mDirty=true;
	mAssociates.clear();
	mLocalKey.clear();
	if(mDoBootstrap) {
		//ScopedTimer st("local key generation");
		const auto bits=mPolicy.bits();
		qDebug()<<"KeyStore: bootstrapping started with "<<bits<<"bits";
		mLocalKey=QSharedPointer<Key>(OC_NEW Key(bits));
	}
	// We only return true if we actually generated, or else asyncstore will be confused
	return mDoBootstrap;
}


bool KeyStore::clearFrontend()
{
	OC_METHODGATE();
	mDirty=true;
	mAssociates.clear();
	mLocalKey.clear();
	return true;
}

bool KeyStore::bootstrapEnabled()
{
	OC_METHODGATE();
	return mDoBootstrap;
}

void KeyStore::setBootstrapEnabled(bool doBootstrap)
{
	OC_METHODGATE();
	mDoBootstrap = doBootstrap;
}


QSharedPointer<Key> KeyStore::localKey()
{
	OC_METHODGATE();
	return mLocalKey;
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
	QSharedPointer<Key> key(OC_NEW Key(pubkeyPEM, true));
	OC_ASSERT(!key.isNull());
	if(!key.isNull() ) {
		if( key->isValid(true) ) {
			mDirty=true;
			mAssociates.insert(key->id(), key);
		}
	}
}

QSharedPointer<Key> KeyStore::pubKeyForID(const QString &id)
{
	OC_METHODGATE();
	if(!ready()) {
		qWarning()<<"WARNING: returning empty key for id="<<id<<" because keystore not ready..";
		return nullptr;
	}
	QMap<QString, QSharedPointer<Key> >::iterator f=mAssociates.find(id);
	if(mAssociates.end()==f) {
		return nullptr;
	}
	return f.value();
}



void KeyStore::dump()
{
	OC_METHODGATE();
	KeyStore &ks=*this;
	qDebug()<<"KEYSTORE DUMP:";
	qDebug().nospace() <<" + fn="<<ks.filename();
	qDebug().nospace() <<" + fexists="<<ks.fileExists();
	qDebug().nospace() <<" + local-key:";
	if(mLocalKey.isNull()) {
		qDebug().nospace()<<"    x null";
	} else {
		qDebug().nospace()<<"    x " << mLocalKey->key()<< ": "<<mLocalKey->pubKey();
	}
	qDebug().nospace() <<" + peer-keys:";
	for(QMap<QString, QSharedPointer<Key> >::iterator b=ks.mAssociates.begin(), e=ks.mAssociates.end(); b!=e; ++b) {
		QSharedPointer<Key> remoteKey=b.value();
		if(remoteKey.isNull()) {
			qDebug().nospace()<<"    x null";
		} else {
			qDebug().nospace()<<"    x " << remoteKey->key()<< ": "<<remoteKey->pubKey();
		}
	}
}

AsyncStore<QVariantMap> &KeyStore::store()
{
	OC_METHODGATE();
	return mStore;
}

QString KeyStore::toString()
{
	OC_METHODGATE();

	QString out="KeyStore{ fn="+filename()+", fexists="+(fileExists()?"true":"false")+", ready=" + (static_cast<const bool>(ready())?"true":"false")+ ", peer-keys:[";
	for(QMap<QString, QSharedPointer<Key> >::iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
		QString key=b.key();
		//b.value();
		out=out+" + " +key;
	}
	out=out+"]}";
	return out;
}



QString KeyStore::filename() const
{
	OC_METHODGATE();
	return mStore.filename();
}

bool KeyStore::fileExists() const
{
	OC_METHODGATE();
	return mStore.fileExists();
}

bool KeyStore::ready()
{
	OC_METHODGATE();
	return mStore.ready();
}




void KeyStore::waitForSync()
{
	OC_METHODGATE();
	mStore.synchronize().waitForFinished();
}


const QDebug &operator<<(QDebug &d, KeyStore &ks)
{
	OC_FUNCTIONGATE();
	d.nospace().noquote() << ks.toString();
	return d.maybeSpace();
}
