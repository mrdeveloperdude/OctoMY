#ifndef KEY_HPP
#define KEY_HPP

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



class KeyPrivate;

class Key
{

public:
	explicit Key() Q_DECL_NOTHROW;


	virtual ~Key();


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
	static QString hash(QString input);


	// Actual usable methods
public:

	QString key();

	QString pubKey();

	QString id() const;


	int kid();

	int kct();

	QVariantMap toVariantMap(bool onlyPublic);

	QString toString();
	bool isValid(bool onlyPublic);

	// Sign message with our private key
	QByteArray sign(const QByteArray &source);


	// Verify signature with our pub-key
	bool verify(const QByteArray &message, const QByteArray &signature);


	// Encrypt a message with out pub-key. Returns empty array as failure.
	// NOTE: This method is expensive, and is only intended for the purpose of processing small amounts of data. If you need to process larger amounts of data, consider using in conjunction with faster symetric encryption (pass the keys to use using RSA encryption and the text using symetric encryption)
	QByteArray encrypt(const QByteArray& data);

	// Decrypt a message with out private-key. Returns empty array as failure.
	// NOTE: This method is expensive, and is only intended for the purpose of processing small amounts of data. If you need to process larger amounts of data, consider using in conjunction with faster symetric encryption (pass the keys to use using RSA encryption and the text using symetric encryption)
	QByteArray decrypt(const QByteArray& data);

};







#endif // KEY_HPP
