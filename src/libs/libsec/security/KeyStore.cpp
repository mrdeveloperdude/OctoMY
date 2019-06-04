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



KeyStore::KeyStore(QObject *parent)
	: QObject(parent)
	, mDirty(true)
	, mDoBootstrap(false)
	, mConfigureHelper("KeyStore", true, true, false, true, false)
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
	if(mConfigureHelper.configure()) {
		auto me=sharedFromThis();
		if(!me.isNull()) {
			mBackend=QSharedPointer<JsonAsyncBackend>(OC_NEW JsonAsyncBackend());
			if(!mBackend.isNull()) {
				mBackend->configure(filename);
				mStore.configure(mBackend, me);
				mDirty=true;
				mDoBootstrap=doBootstrap;
				mPolicy=policy;
			} else {
				qDebug()<<"ERROR: Backend was null";
			}
		} else {
			qDebug()<<"ERROR: Frontend was null";
		}
	}
}


void KeyStore::activate(bool on, std::function<void(bool)> callBack)
{
	OC_METHODGATE();
	if(on) {
		if(mConfigureHelper.activate(on)) {
			mStore.activate(on);
			synchronize([this, callBack, on](ASEvent<QVariantMap> &se) {
				const bool ok=se.isSuccessfull();
				if(nullptr!=callBack) {
					callBack(ok);
				}
				emit keystoreReady(on, ok);
			});
		}
	} else {
		if(mConfigureHelper.isActivatedAsExpected()) {
			synchronize([this, callBack, on](ASEvent<QVariantMap> &se) {
				const bool ok=se.isSuccessfull();
				if(nullptr!=callBack) {
					callBack(ok);
				}
				emit keystoreReady(on, ok);
				mStore.activate(on);
				mConfigureHelper.activate(on);
			});
		}
	}
}


void KeyStore::clear()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		clear([](ASEvent<QVariantMap> &ase) {
			Q_UNUSED(ase);
		});
	}
}


void KeyStore::save()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		save([](ASEvent<QVariantMap> &ase) {
			Q_UNUSED(ase);
		});
	}
}


void KeyStore::load()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		load([](ASEvent<QVariantMap> &ase) {
			Q_UNUSED(ase);
		});
	}
}


bool KeyStore::setFrontend(QVariantMap map)
{
	OC_METHODGATE();
	bool ok=false;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		//qDebug()<<"KEYSTORE FROM MAP";
		mDirty=true;
		ok=true;
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
	}
	return ok;
}


QVariantMap KeyStore::getFrontend(bool &ok)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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
	return QVariantMap();
}


bool KeyStore::generateFrontend()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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
	return false;
}


bool KeyStore::clearFrontend()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mDirty=true;
		mAssociates.clear();
		mLocalKey.clear();
		return true;
	}
	return false;
}


bool KeyStore::bootstrapEnabled()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mDoBootstrap;
	}
	return false;
}


void KeyStore::setBootstrapEnabled(bool doBootstrap)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mDoBootstrap = doBootstrap;
	}
}


QSharedPointer<Key> KeyStore::localKey()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mLocalKey;
	}
	return nullptr;
}


bool KeyStore::hasPubKeyForID(const QString &id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		if(!ready()) {
			return false;
		}
		return (mAssociates.end()!=mAssociates.find(id));
	}
	return false;
}


void KeyStore::setPubKeyForID(const QString &pubkeyPEM)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
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
}


QSharedPointer<Key> KeyStore::pubKeyForID(const QString &id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
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
	return nullptr;
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
	if(mConfigureHelper.isActivatedAsExpected()) {
		// Not much we can do except put the warning and hope for the best
	}
	return mStore;
}


QString KeyStore::toString()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		QString out="KeyStore{ fn="+filename()+", fexists="+(fileExists()?"true":"false")+", ready=" + (static_cast<const bool>(ready())?"true":"false")+ ", peer-keys:[";
		for(QMap<QString, QSharedPointer<Key> >::iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
			QString key=b.key();
			//b.value();
			out=out+" + " +key;
		}
		out=out+"]}";
		return out;
	}
	return "";
}


QMap<QString, QString> KeyStore::toMap()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	if(mConfigureHelper.isActivatedAsExpected()) {
		//qDebug()<<"KEYSTORE TO MAP";
		if(!mLocalKey.isNull()) {
			map["localKey"]=mLocalKey->toString();
		} else {
			map["localKey"]="null";
		}
		int i=0;
		for(QMap<QString, QSharedPointer<Key> >::const_iterator b=mAssociates.begin(), e=mAssociates.end(); b!=e; ++b) {
			QVariantMap remote;
			QString id=b.key();
			auto key=b.value();
			map[QString("remoteKey_%1").arg(i)]=(nullptr!=key)?key->toString():"null";
			++i;
		}
	}
	return map;
}


QString KeyStore::filename() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mStore.filename();
	}
	return "";
}


bool KeyStore::fileExists() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mStore.fileExists();
	}
	return false;
}


bool KeyStore::ready()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		return mStore.ready();
	}
	return false;
}


void KeyStore::waitForSync()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mStore.synchronize().waitForFinished();
	}
}


const QDebug &operator<<(QDebug &d, KeyStore &ks)
{
	OC_FUNCTIONGATE();
	d.nospace().noquote() << ks.toString();
	return d.maybeSpace();
}
