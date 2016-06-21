#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>

#include "../libpki/qpolarsslpki.hpp"
#include "basic/AtomicBoolean.hpp"

#include "basic/GenerateRunnable.hpp"

#include <QFile>


class KeyStore: public QObject{
		Q_OBJECT
	private:

		qpolarssl::Pki local_pki;
		QMap<QString, QSharedPointer<qpolarssl::Pki> > peer_pki;
		AtomicBoolean ready;
		AtomicBoolean error;
		QString filename;
		quint32 keyBits;

		friend class GenerateRunnable<KeyStore>;

	public:
		explicit KeyStore(QObject *parent=0, QString="");
		virtual ~KeyStore();

	private:

		void bootstrapWorker();
		void load();
		void save();


	public:

		//Make if needed, load if otherwise
		void bootstrap();

		bool isReady(){
			return ready;
		}
		bool isError(){
			return error;
		}

		bool hasLocalKeyFile(){
			return QFile(filename).exists();
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







