#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>

#include "../libpki/qpolarsslpki.hpp"


class AtomicBoolean{
	private:
		QMutex mMutex;
		bool mBool;
		Q_DISABLE_COPY(AtomicBoolean)

	public:
		AtomicBoolean(bool v):mMutex(),mBool(false){
			set(v);
		}
		bool set(bool v) {
			QMutexLocker ml(&mMutex);
			return mBool=v;
		}

		bool get() {
			QMutexLocker ml(&mMutex);
			return mBool;
		}


		void operator=(const bool &b){
			set(b);
		}

		operator bool(){
			return get();
		}

};

class GenerateKeyRunnable;

class KeyStore: public QObject{
		Q_OBJECT
	private:

		qpolarssl::Pki local_pki;
		QMap<QString, QSharedPointer<qpolarssl::Pki>> peer_pki;
		AtomicBoolean ready;
		QString fn;
		quint32 keyBits;

		friend class GenerateKeyRunnable;

	public:
		explicit KeyStore(QObject *parent=0);
		virtual ~KeyStore();

	private:
		void bootstrap();
		void load();
		void save();


	private:
		static KeyStore *instance;
	public:
		static  KeyStore & getInstance();


	public:

		bool isReady(){
			return ready;
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
		void setPubKeyForFingerprint(const QString &fingerprint,const QString &pubkeyPEM);

		QString getLocalPublicKey();
		QString getLocalPrivateKey();

	signals:

		void keystoreReady();

};

#endif // KEYSTORE_HPP







