/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP


#include "../libpki/qpolarsslpki.hpp"

#include "Key.hpp"
#include "security/PortableID.hpp"

#include "basic/GenerateRunnable.hpp"

#include "basic/AsyncStore.hpp"


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

	Key mLocalKey;
	QMap<QString, Key > mPeers;

	friend class GenerateRunnable<KeyStore>;

public:
	explicit KeyStore(QString="", QObject *parent=nullptr);
	virtual ~KeyStore();


protected:
	void bootstrapWorkerImpl() override;
public:
	void load() override;
	void save() override;

public:

	void clear();

public:

	void hookSignals(QObject &ob);

	void unhookSignals(QObject &ob);

	inline Key &localKey()
	{
		return mLocalKey;
	}

	inline PortableID localPortableID()
	{
		PortableID pid;
		//pid.setName("Arne");
		//pid.setGender("Male");
		pid.setID(mLocalKey.id());
		pid.setBirthDate(QFileInfo(mFilename).created().toMSecsSinceEpoch());
		//pid.setType(DiscoveryType type);
		return pid;
	}

	// Sign message with our private key
	QByteArray sign(const QByteArray &source);

	// Verify signature with our pub-key
	bool verify(const QByteArray &message, const QByteArray &signature);

	// Verify signature with pub-key of tier recognized by fiven fingerprint id
	bool verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature);

	// Check if we have pub-key for tier identified by give fingerprint ID
	bool hasPubKeyForID(const QString &id);

	// Set pub-key for tier identified by give fingerprint ID to given UTF8 encoded string containing pubkey PEM format
	void setPubKeyForID(const QString &pubkeyPEM);

	// return pub-key for tier identified by give fingerprint ID
	Key pubKeyForID(const QString &id);

	friend const QDebug &operator<<(QDebug &d, KeyStore &ks);

signals:

	void keystoreReady(bool);

};



const QDebug &operator<<(QDebug &d, KeyStore &ks);

#endif // KEYSTORE_HPP
