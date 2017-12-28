/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP


#include "Key.hpp"
#include "PortableID.hpp"
#include "basic/GenerateRunnable.hpp"
#include "basic/AsyncStore.hpp"
#include "KeySecurityPolicy.hpp"

#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QMutex>
#include <QSharedPointer>
#include <QString>


class KeyStore: public AsyncStore
{
	Q_OBJECT
private:

	QSharedPointer<Key> mLocalKey;
	QMap<QString, QSharedPointer<Key> > mAssociates;

	KeySecurityPolicy mPolicy;

	friend class GenerateRunnable<KeyStore>;

public:
	explicit KeyStore(QString="", KeySecurityPolicy policy=KeySecurityPolicy(), QObject *parent=nullptr);
	virtual ~KeyStore();


protected:
	void bootstrapWorkerImpl() override;
public:
	void load() override;
	void save() override;

public:

	void clear();

public:

	void setHookSignals(QObject &ob, bool hook);

	inline QSharedPointer<Key> localKey()
	{
		OC_METHODGATE();
		return mLocalKey;
	}

	inline PortableID localPortableID()
	{
		OC_METHODGATE();
		PortableID pid;
		//pid.setName("Arne");
		//pid.setGender("Male");
		if(nullptr!=mLocalKey) {
			pid.setID(mLocalKey->id());
		}
		pid.setBirthDate(QFileInfo(mFilename).created().toMSecsSinceEpoch());
		//pid.setType(DiscoveryType type);
		return pid;
	}


	void dump();


	// Sign message with our private key
	QByteArray sign(const QByteArray &source);

	// Verify signature with our pub-key
	bool verify(const QByteArray &message, const QByteArray &signature);

	// Verify signature with pub-key of node recognized by fiven fingerprint id
	bool verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature);

	// Check if we have pub-key for node identified by give fingerprint ID
	bool hasPubKeyForID(const QString &id);

	// Set pub-key for node identified by given fingerprint ID to given UTF-8 encoded string containing pubkey PEM format
	void setPubKeyForID(const QString &pubkeyPEM);

	// return pub-key for node identified by give fingerprint ID
	QSharedPointer<Key> pubKeyForID(const QString &id);
signals:

	void keystoreReady(bool);


public:

	friend const QDebug &operator<<(QDebug &d, KeyStore &ks);


};



const QDebug &operator<<(QDebug &d, KeyStore &ks);

#endif // KEYSTORE_HPP
