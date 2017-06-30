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
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key(constKey &other)"<<other.d_func()->mKey<<other.d_func()->mPubKey;
	mInitialized=true;
	Q_D(Key);
	if(nullptr==d) {
		qWarning()<<"ERROR: Key::d was nullptr in Key::Key(const Key &other)";
	}
	OC_ASSERT(nullptr!=d);
}


Key::~Key()
{
	OC_METHODGATE();
	//qDebug()<<"virtual ~Key()";
}

Key::Key( QVariantMap map, bool isPublic )
	: d_ptr(new KeyPrivate(map, isPublic))
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key( QVariantMap map, bool isPublic )"<< map<<isPublic;
	mInitialized=true;
	Q_D(Key);
	if(nullptr==d) {
		qWarning()<<"ERROR: Key::d was nullptr in Key::Key( QVariantMap map, bool isPublic )";
	}
	OC_ASSERT(nullptr!=d);
}


Key::Key( QString key, bool isPublic )
	: d_ptr(new KeyPrivate(key, isPublic))
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key( QString key, bool isPublic):"<<key<<isPublic;
	mInitialized=true;
	Q_D(Key);
	if(nullptr==d) {
		qWarning()<<"ERROR: Key::d was nullptr in Key::Key( QString key, bool isPublic )";
	}
	OC_ASSERT(nullptr!=d);
}



Key::Key( quint32 bits )
	: d_ptr(new KeyPrivate(bits))
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key( quint32 bits ):"<<bits;
	mInitialized=true;
	Q_D(Key);
	if(nullptr==d) {
		qWarning()<<"ERROR: Key::d was nullptr in Key::Key( quint32 bits )";
	}
	OC_ASSERT(nullptr!=d);
}


Key::Key(KeyPrivate &dd)
	: d_ptr(&dd)
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"Key::Key(KeyPrivate &dd)";
	mInitialized=true;
	Q_D(Key);
	if(nullptr==d) {
		qWarning()<<"ERROR: Key::d was nullptr in Key::Key(KeyPrivate &dd)";
	}
	OC_ASSERT(nullptr!=d);
}


Key::Key(Key && other) : Key()
{
	OC_METHODGATE();
	swap(*this, other);
	//	qDebug()<<"Key::Key(Key && other) : Key()";
	Q_D(Key);
	if(nullptr==d) {
		qWarning()<<"ERROR: Key::d was nullptr in Key::Key(Key && other) : Key()";
	}
	OC_ASSERT(nullptr!=d);
}

Key & Key::operator=(Key other)
{
	OC_METHODGATE();
	swap(*this, other);
	//	qDebug()<<"Key & Key::operator=(Key other)";
	return *this;
}

bool Key::operator==(const Key &other) const
{
	OC_METHODGATE();
	//	qDebug()<<"bool Key::operator==(Key &other)";
	return ( (other.d_func() == this->d_func() ) || (other.key() == key() ) );
}


bool Key::operator!=(const Key &other) const
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


////////////////////////////////////////////////////////////////////
/// // Actual usable methods


QString Key::key() const
{
	OC_METHODGATE();
	const Q_D(Key);
	return d->mKey;
}

QString Key::pubKey() const
{
	OC_METHODGATE();
	const Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return d->mPubKey;
}

QString Key::id()//NOTE no const please
{
	OC_METHODGATE();
	Q_D(Key);
	if(!mInitialized) {
		qWarning()<<"ERROR: !mInitialized in id()";
		return "";
	}
	OC_ASSERT(nullptr!=d);
	if(nullptr!=d) {
		if(!d->mInitialized) {
			qWarning()<<"ERROR: !d->mInitialized in id()";
			return "";
		}
		return d->mID;
	} else {
		qWarning()<<"ERROR: Key::d was nullptr in id()";
		return "";
	}
}


int Key::kid() const
{
	OC_METHODGATE();
	const Q_D(Key);
	return d->mKID;
}

int Key::kct() const
{
	OC_METHODGATE();
	const Q_D(Key);
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
	const Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return "Key: "+id()+" priv="+(key().isEmpty()?"UNSET":"SET")+ (d->mValidPrivate?"(VALID)":"(INVALID)") +" pub="+(pubKey().isEmpty()?"UNSET":"SET")+ (d->mValidPublic?"(VALID)":"(INVALID)");
}

bool Key::isValid(bool onlyPublic)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	return ( onlyPublic ? (d->mValidPublic) : (d->mValidPublic && d->mValidPrivate) );
}


bool Key::hasPrivate(bool thatIsValid)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	if(d->mKey.isEmpty()) {
		return false;
	}
	if(thatIsValid) {
		return d->mValidPrivate;
	}
	return true;
}


bool Key::hasPublic(bool thatIsValid)
{
	OC_METHODGATE();
	Q_D(Key);
	OC_ASSERT(nullptr!=d);
	if(d->mPubKey.isEmpty()) {
		return false;
	}
	if(thatIsValid) {
		return d->mValidPublic;
	}
	return true;
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


/*

bool operator== (const Key &a, const Key &b)
{
	return a.operator ==(b);
}


bool operator!= (const Key &a, const Key &b)
{
	return !a.operator !=(b);
}
*/
