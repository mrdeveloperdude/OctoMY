#ifndef KEY_HPP
#define KEY_HPP

#include "../libpki/qpolarsslpki.hpp"

#include <QByteArray>
#include <QScopedPointer>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>

/*
  Key represents a key as part of Public Key Infrastructure based on RSA
  It has a QtStyle private class so it can easily be copied by value
  ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
*/


class Key;


/*
  Qt Style private class for Key
*/
class KeyPrivate{

	private:
		Q_DISABLE_COPY(KeyPrivate)
		//		Q_DECLARE_PUBLIC(Key)

	protected:

		QString mKey;
		QString mPubKey;
		QString mID;
		qpolarssl::Pki mPKI;
		static int mKCT;
		int mKID;

	private:
		static QString hash(QString input);

	public:

		// Parse public or private key in PEM format
		explicit KeyPrivate(QString key, bool isPublic = true);
		// Generate key-pair of given bit length
		explicit KeyPrivate(quint32 bits);

		virtual ~KeyPrivate();

		// Friendships
	public:

		friend class Key;
};

////////////////////////////////////////////////////////////////////////

class Key{
		// OctoMY™ sanctioned values for key-generation
	public:

		static const quint32 OCTOMY_KEY_BITS=4096;
		static const QCryptographicHash::Algorithm OCTOMY_KEY_HASH=QCryptographicHash::Sha512;

	protected:

		inline explicit Key() Q_DECL_NOTHROW
		//: d_ptr(new KeyPrivate())
		{
			qDebug()<<"inline explicit Key() Q_DECL_NOTHROW";
		}
	public:
		virtual ~Key()
		{
			qDebug()<<"virtual ~Key()";
		}


		// All that pimpl swagger
	private:
		QScopedPointer<KeyPrivate>  d_ptr;
		Q_DECLARE_PRIVATE(Key)
	public:
		explicit Key(QString key, bool isPublic);
		explicit Key(quint32 bits);
		explicit Key(KeyPrivate &dd);

		Key(const Key & other);
		Key(Key && other);

		Key & operator=(Key other);
		bool operator==(Key &other);
		bool operator!=(Key &other);

		friend void swap(Key& first, Key& second) /* nothrow */;

		// Actual usable methods
	public:

		inline QString key()
		{
			Q_D(Key);
			return d->mKey;
		}

		inline QString pubKey()
		{
			Q_D(Key);
			return d->mPubKey;
		}

		inline QString id()
		{
			Q_D(Key);
			return d->mID;
		}


		inline int kid()
		{
			Q_D(Key);
			return d->mKID;
		}

		inline int kct()
		{
			Q_D(Key);
			return d->mKCT;
		}

};







#endif // KEY_HPP
