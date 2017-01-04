#ifndef KEY_HPP
#define KEY_HPP

#include "../libpki/qpolarsslpki.hpp"
#include "../libutil/utility/Standard.hpp"

#include "SecurityConstants.hpp"

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
class KeyPrivate
{

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
	bool mValidPrivate;
	bool mValidPublic;

public:
	static QString hash(QString input);

private:

	void parse(bool isPublic);
public:



	// Parse public or private key in PEM format
	explicit KeyPrivate(QVariantMap key, bool isPublic);
	// Parse public or private key in PEM format
	explicit KeyPrivate(QString key, bool isPublic);
	// Generate key-pair of given bit length
	explicit KeyPrivate(quint32 bits);
	//Empty key
	explicit KeyPrivate();

	virtual ~KeyPrivate();

	// Friendships
public:

	friend class Key;
};

////////////////////////////////////////////////////////////////////////

class Key
{


public:
	inline explicit Key() Q_DECL_NOTHROW
:
	d_ptr(new KeyPrivate())
	{
		OC_METHODGATE();
		//qDebug()<<"inline explicit Key() Q_DECL_NOTHROW";
	}

public:
	virtual ~Key()
	{
		OC_METHODGATE();
		//qDebug()<<"virtual ~Key()";
	}


	// All that pimpl swagger
private:
	QScopedPointer<KeyPrivate>  d_ptr;
	Q_DECLARE_PRIVATE(Key)
public:


	explicit Key(QVariantMap map, bool isPublic);
	explicit Key(QString key, bool isPublic);
	explicit Key(quint32 bits);
	explicit Key(KeyPrivate &dd);

	Key(const Key & other);
	Key(Key && other);

	Key & operator=(Key other);
	bool operator==(Key &other);
	bool operator!=(Key &other);

	friend void swap(Key& first, Key& second) /* nothrow */;

public:
	inline static QString hash(QString input)
	{
		return KeyPrivate::hash(input);
	}


	// Actual usable methods
public:

	inline QString key()
	{
		OC_METHODGATE();
		Q_D(Key);
		return d->mKey;
	}

	inline QString pubKey()
	{
		OC_METHODGATE();
		Q_D(Key);
		return d->mPubKey;
	}

	inline QString id() const
	{
		OC_METHODGATE();
		const Q_D(Key);
		return d->mID;
	}


	inline int kid()
	{
		OC_METHODGATE();
		Q_D(Key);
		return d->mKID;
	}

	inline int kct()
	{
		OC_METHODGATE();
		Q_D(Key);
		return d->mKCT;
	}

	inline QVariantMap toVariantMap(bool onlyPublic)
	{
		OC_METHODGATE();
		QVariantMap map;
		if(!onlyPublic) {
			map["privateKey"]=key();
		}
		map["publicKey"]=pubKey();
		map["id"]=id();
		return map;
	}

	inline QString toString()
	{
		OC_METHODGATE();
		Q_D(Key);
		OC_ASSERT(nullptr!=d);
		return "Key: "+id()+" priv="+(key().isEmpty()?"UNSET":"SET")+ (d->mValidPrivate?"(VALID)":"(INVALID)") +" pub="+(pubKey().isEmpty()?"UNSET":"SET")+ (d->mValidPublic?"(VALID)":"(INVALID)");
	}

	inline bool isValid(bool onlyPublic)
	{
		OC_METHODGATE();
		Q_D(Key);
		OC_ASSERT(nullptr!=d);
		return ( onlyPublic ? (d->mValidPublic) : (d->mValidPublic&&d->mValidPrivate) );
	}

	// Sign message with our private key
	inline QByteArray sign(const QByteArray &source)
	{
		OC_METHODGATE();
		Q_D(Key);
		OC_ASSERT(nullptr!=d);
		return d->mPKI.sign(source, OCTOMY_KEY_HASH_POLAR);
	}


	// Verify signature with our pub-key
	inline bool verify(const QByteArray &message, const QByteArray &signature)
	{
		OC_METHODGATE();
		Q_D(Key);
		OC_ASSERT(nullptr!=d);
		return (0==d->mPKI.verify(message, signature, OCTOMY_KEY_HASH_POLAR));
	}


	// Encrypt a message with out pub-key. Returns empty array as failure.
	// NOTE: This method is expensive, and is only intended for the purpose of processing small amounts of data. If you need to process larger amounts of data, consider using in conjunction with faster symetric encryption (pass the keys to use using RSA encryption and the text using symetric encryption)
	inline QByteArray encrypt(const QByteArray& data)
	{
		OC_METHODGATE();
		Q_D(Key);
		OC_ASSERT(nullptr!=d);
		return d->mPKI.encrypt(data);
	}

	// Decrypt a message with out private-key. Returns empty array as failure.
	// NOTE: This method is expensive, and is only intended for the purpose of processing small amounts of data. If you need to process larger amounts of data, consider using in conjunction with faster symetric encryption (pass the keys to use using RSA encryption and the text using symetric encryption)
	inline QByteArray decrypt(const QByteArray& data)
	{
		OC_METHODGATE();
		Q_D(Key);
		OC_ASSERT(nullptr!=d);
		return d->mPKI.decrypt(data);
	}


};







#endif // KEY_HPP
