#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>

#include "../libpki/qpolarsslpki.hpp"
#include "basic/AtomicBoolean.hpp"
#include "Key.hpp"
#include "security/PortableID.hpp"

#include "basic/GenerateRunnable.hpp"

#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>


class KeyStore: public QObject{
		Q_OBJECT
	private:

		Key mLocalKey;
		//qpolarssl::Pki local_pki;
		QMap<QString, Key > mPeers;
		AtomicBoolean mReady;
		AtomicBoolean mError;
		AtomicBoolean mInProgress;
		QString mFilename;
		//quint32 keyBits;

		friend class GenerateRunnable<KeyStore>;

	public:
		explicit KeyStore(QString="", QObject *parent=nullptr);
		virtual ~KeyStore();

	private:

		void bootstrapWorker();
		void load();
		void save();

	public:

		void clear();

		//Make if needed, load if otherwise
		void bootstrap(bool loadOnly=false, bool runInBackground=true);

		inline bool isReady()
		{
			return mReady;
		}

		inline bool hasError()
		{
			return mError;
		}

		inline bool hasLocalKeyFile() const
		{
			return QFile(mFilename).exists();
		}

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
		bool hasPubKeyForFingerprint(const QString &fingerprint);

		// Set pub-key for tier identified by give fingerprint ID to given UTF8 encoded string containing pubkey PEM format
		void setPubKeyForFingerprint(const QString &pubkeyPEM);

		friend const QDebug &operator<<(QDebug &d, KeyStore &ks);

	signals:

		void keystoreReady(bool ok);

};



const QDebug &operator<<(QDebug &d, KeyStore &ks);

#endif // KEYSTORE_HPP







