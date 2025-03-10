#ifndef KEY_HPP
#define KEY_HPP

#include <QByteArray>
//#include <QScopedPointer>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>



class KeyPrivate;

/**
 * @brief The Key class represents a key as part of Public Key Infrastructure based on RSA
 * It has a QtStyle private class so it can easily be copied by value
 * ( http://stackoverflow.com/questions/25250171/how-to-use-the-qts-pimpl-idiom )
 *
 * It provides the following features:
 * + Generation of new key
 * + Loading of existing key from string or map
 * + Saving of key to string or map
 * + using key for encryption
 * + using key for decryption
 * + using key for signing
 * + using key for verifying signatures
 * + using key for encryption
 *
 * See also KeyStore.hpp, Associate.hpp, Client.hpp, Node.hpp
 */
class Key
{
	// All that pimpl swagger
private:
	Q_DECLARE_PRIVATE(Key)
	QScopedPointer<KeyPrivate>  d_ptr;
public:
	bool mInitialized;


public:
	explicit Key() Q_DECL_NOTHROW;
	explicit Key(QVariantMap map, bool isPublic);
	explicit Key(QString key, bool isPublic);
	explicit Key(quint32 bits);
	//explicit Key(KeyPrivate &dd);
	Key(const Key & other);
	Key(Key && other);

	virtual ~Key();

public:
	Key & operator=(Key other);
	bool operator==(const Key &other) const;
	bool operator!=(const Key &other) const;

	friend void swap(Key& first, Key& second) /* nothrow */;

public:
	static QString hash(QString input);

	KeyPrivate* d_func_dbg();

	// Actual usable methods
public:
	QString key() const;
	QString pubKey() const;
	QString id(); //NOTE no const please
	QString describe();
	int kid() const;
	int kct() const;

	QVariantMap toVariantMap(bool onlyPublic);
	QMap<QString, QString> toMap(bool onlyPublic);

	QString toString();
	bool isValid(bool onlyPublic);

	bool hasPublic(bool thatIsValid=false);
	bool hasPrivate(bool thatIsValid=false);

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

	bool isEqual(Key &other) const;

	//void detach();

};


/*
bool operator== (const Key &a, const Key &b);
bool operator!= (const Key &a, const Key &b);

*/


#endif
// KEY_HPP
