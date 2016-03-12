#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "../libpki/qpolarsslpki.hpp"

class KeyStore{
	private:

		qpolarssl::Pki local_pki;
		QMap<QString, QSharedPointer<qpolarssl::Pki>> peer_pki;

	public:
		explicit KeyStore();
		virtual ~KeyStore();

	private:
		void bootstrap();
		void load();
		void save();

	public:

		//Sign message with our private key
		QByteArray sign(const QByteArray &source);
		//Verify signature with our pubkey
		bool verify(const QByteArray &message, const QByteArray &signature);

		//Verify signature with pubkey of tier recognized by fiven fingerprint id
		bool verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature);

		//Check if we have pubkey for tier identified by give fingerprint ID
		bool hasPubKeyForFingerprint(const QString &fingerprint);
		//Set pubkey for tier identified by give fingerprint ID to given UTF8 encoded string containing pubkey PEM format
		void setPubKeyForFingerprint(const QString &fingerprint,const QString &pubkeyPEM);

		QString getLocalPublicKey();
		QString getLocalPrivateKey();

};

#endif // KEYSTORE_HPP







