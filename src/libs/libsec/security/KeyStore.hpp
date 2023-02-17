/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP

#include "Key.hpp"
#include "KeySecurityPolicy.hpp"
#include "store/AsyncStore.hpp"
#include "store/JsonAsyncBackend.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "utility/concurrent/GenerateRunnable.hpp"

#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QMutex>
#include <QString>

struct nop {
	void operator()(...) const volatile {}
};

/**
 * @brief The KeyStore class has the following tasks:
 * + asynchronous loading and storing of our local private key key plus the publik key of associates
 * + asynchronoous generation of our local private key
 * + Look-up of keys by ID
 *
 * See also Key.hpp, LocalIdentityStore.hpp
 *
 */
class KeyStore: public QObject, public AsyncFrontend<QVariantMap>, public QEnableSharedFromThis<KeyStore>
{
	Q_OBJECT
private:
	QSharedPointer<JsonAsyncBackend> mBackend;
	AsyncStore<QVariantMap> mStore;
	QVariantMap mCache;
	bool mDirty;
	bool mDoBootstrap;
	KeySecurityPolicy mPolicy;
	QSharedPointer<Key> mLocalKey;
	QMap<QString, QSharedPointer<Key> > mAssociates;
	ConfigureHelper mConfigureHelper;

	friend class GenerateRunnable<KeyStore>;

public:
	explicit KeyStore(QObject *parent = nullptr);
	virtual ~KeyStore() Q_DECL_OVERRIDE;

public:
	void configure(QString filename = "", bool doBootstrap = false, KeySecurityPolicy policy = KeySecurityPolicy() );
	void activate(bool on, std::function<void(bool)> callBack=nullptr);

	// AsyncFrontend interface
public:
	bool clearFrontend() Q_DECL_OVERRIDE;
	bool setFrontend(QVariantMap data) Q_DECL_OVERRIDE;
	QVariantMap getFrontend(bool &ok) Q_DECL_OVERRIDE;
	bool generateFrontend() Q_DECL_OVERRIDE;

private:
	// To have this called, make sure to set boostrapping to true and then call synchronize()
	void bootstrap();

public:
	bool bootstrapEnabled();
	void setBootstrapEnabled(bool doBootstrap);

	QSharedPointer<Key> localKey();

	// Check if we have pub-key for node identified by give fingerprint ID
	bool hasPubKeyForID(const QString &id);

	// Set pub-key for node identified by given fingerprint ID to given UTF-8 encoded string containing pubkey PEM format
	void setPubKeyForID(const QString &pubkeyPEM);

	// return pub-key for node identified by give fingerprint ID
	QSharedPointer<Key> pubKeyForID(const QString &id);

	AsyncStore<QVariantMap> &store();
	void dump();
	QString toString();
	QMap<QString, QString> toMap();


public:
	QString filename() const;
	bool fileExists() const;
	bool ready();


	template <typename F>
	void status(F callBack);
	template <typename F>
	void clear(F callBack);
	void clear();
	template <typename F>
	void save(F callBack);
	void save();
	template <typename F>
	void load(F callBack);
	void load();
	template <typename F>
	void synchronize(F callBack);
	void synchronize();

	void waitForSync();

public:
	friend const QDebug &operator<<(QDebug &d, KeyStore &ks);

signals:
	void keystoreReady(bool on, bool ok);

};






////////////////////////////////////////////////////////////////////////////////

template <typename F>
void KeyStore::status(F callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mStore.status().onFinished(callBack);
	}
}


template <typename F>
void KeyStore::clear(F callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mStore.clear().onFinished(callBack);
	}
}


template <typename F>
void KeyStore::save(F callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mStore.save().onFinished(callBack);
	}
}


template <typename F>
void KeyStore::load(F callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mStore.load().onFinished(callBack);
	}
}


template <typename F>
void KeyStore::synchronize(F callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		mStore.synchronize().onFinished(callBack);
	}
}

const QDebug &operator<<(QDebug &d, KeyStore &ks);


#endif
// KEYSTORE_HPP
