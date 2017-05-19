#include "Key.hpp"

#include "KeyPrivate.hpp"

#include "utility/Standard.hpp"
#include "SecurityConstantsPrivate.hpp"

Key::Key() Q_DECL_NOTHROW
:
d_ptr(new KeyPrivate())
{
	OC_METHODGATE();
	//qDebug()<<"inline explicit Key() Q_DECL_NOTHROW";
}

Key::Key(const Key &other)
	: d_ptr(new KeyPrivate(
				(!other.d_func()->mKey.isEmpty())
				?
				(other.d_func()->mKey)
				:
				(other.d_func()->mPubKey),
				other.d_func()->mKey.isEmpty()
			)
		   )
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key(constKey &other)"<<other.d_func()->mKey<<other.d_func()->mPubKey;
}


Key::~Key()
{
	OC_METHODGATE();
	//qDebug()<<"virtual ~Key()";
}

Key::Key( QVariantMap map, bool isPublic )
	: d_ptr(new KeyPrivate(map, isPublic))
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key( QVariantMap map, bool isPublic )"<< map<<isPublic;
}


Key::Key( QString key, bool isPublic )
	: d_ptr(new KeyPrivate(key, isPublic))
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key( QString key, bool isPublic):"<<key<<isPublic;
}



Key::Key( quint32 bits )
	: d_ptr(new KeyPrivate(bits))
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key( quint32 bits ):"<<bits;
}


Key::Key(KeyPrivate &dd)
	: d_ptr(&dd)
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key(KeyPrivate &dd)";
}


Key::Key(Key && other) : Key()
{
	OC_METHODGATE();
	swap(*this, other);
	//	qDebug()<<"Key::Key(Key && other) : Key()";
}

Key & Key::operator=(Key other)
{
	OC_METHODGATE();
	swap(*this, other);
	//	qDebug()<<"Key & Key::operator=(Key other)";
	return *this;
}

bool Key::operator==(Key &other)
{
	OC_METHODGATE();
	//	qDebug()<<"bool Key::operator==(Key &other)";
	return ( (other.d_func() == this->d_func() ) || (other.key() == key() ) );
}


bool Key::operator!=(Key &other)
{
	OC_METHODGATE();
	//	qDebug()<<"bool Key::operator!=(Key &other)";
	return !operator==(other);
}

void swap(Key& first, Key& second) /* nothrow */
{
	OC_FUNCTIONGATE();
	using std::swap;
	//	qDebug()<<"void swap(Key& first, Key& second) /* nothrow */";
	swap(first.d_ptr, second.d_ptr);
}





QString Key::hash(QString input)
{
	return KeyPrivate::hash(input);
}


// Actual usable methods


QString Key::key()
{
	OC_METHODGATE();
	Q_D(Key);
	return d->mKey;
}

QString Key::pubKey()
{
	OC_METHODGATE();
	Q_D(Key);
	return d->mPubKey;
}

QString Key::id() const
{
	OC_METHODGATE();
	const Q_D(Key);
	return d->mID;
}


int Key::kid()
{
	OC_METHODGATE();
	Q_D(Key);
	return d->mKID;
}

int Key::kct()
{
	OC_METHODGATE();
	Q_D(Key);
	return d->mKCT;
}

QVariantMap Key::toVariantMap(bool onlyPublic)
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

QString Key::toString()
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return "Key: "+id()+" priv="+(key().isEmpty()?"UNSET":"SET")+ (d->mValidPrivate?"(VALID)":"(INVALID)") +" pub="+(pubKey().isEmpty()?"UNSET":"SET")+ (d->mValidPublic?"(VALID)":"(INVALID)");
}

bool Key::isValid(bool onlyPublic)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return ( onlyPublic ? (d->mValidPublic) : (d->mValidPublic&&d->mValidPrivate) );
}

// Sign message with our private key
QByteArray Key::sign(const QByteArray &source)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return d->mPKI.sign(source, OCTOMY_KEY_HASH_POLAR);
}


// Verify signature with our pub-key
bool Key::verify(const QByteArray &message, const QByteArray &signature)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return (0==d->mPKI.verify(message, signature, OCTOMY_KEY_HASH_POLAR));
}


// Encrypt a message with out pub-key. Returns empty array as failure.
// NOTE: This method is expensive, and is only intended for the purpose of processing small amounts of data. If you need to process larger amounts of data, consider using in conjunction with faster symetric encryption (pass the keys to use using RSA encryption and the text using symetric encryption)
QByteArray Key::encrypt(const QByteArray& data)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return d->mPKI.encrypt(data);
}

// Decrypt a message with out private-key. Returns empty array as failure.
// NOTE: This method is expensive, and is only intended for the purpose of processing small amounts of data. If you need to process larger amounts of data, consider using in conjunction with faster symetric encryption (pass the keys to use using RSA encryption and the text using symetric encryption)
QByteArray Key::decrypt(const QByteArray& data)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return d->mPKI.decrypt(data);
}
