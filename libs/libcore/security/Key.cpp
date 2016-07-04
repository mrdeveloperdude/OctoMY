#include "Key.hpp"

#include "utility/ScopedTimer.hpp"

int KeyPrivate::mKCT=0;



KeyPrivate::KeyPrivate( QString key, bool isPublic )
	: mKey(isPublic?"":key)
	, mPubKey(isPublic?key:"")
	, mKID(mKCT++)
{

	qDebug()<<"KeyPrivate::KeyPrivate( QString key, bool isPublic ):"<<key<<isPublic <<" for " <<mKID<<"/"<<mKCT;
	ScopedTimer timer("Key Parse");
	if(isPublic){
		mPKI.parsePublicKey(mPubKey.toUtf8());
	}
	else{
		mPKI.parseKey(mKey.toUtf8());
		mPubKey=mPKI.getPEMPubkey();
	}
	mID=hash(mPubKey);
}


KeyPrivate::KeyPrivate(quint32 bits)
	: mKID(mKCT++)
{
	qDebug()<<"KeyPrivate::KeyPrivate(quint32 bits):"<<bits<<" for " <<mKID<<"/"<<mKCT;
	ScopedTimer timer("Key Generation");
	mPKI.generateKeyPair(bits);
	if(mPKI.isValid()){
		mKey=mPKI.getPEMKey();
		mPubKey=mPKI.getPEMPubkey();
		mID=hash(mPubKey);
	}
	else{
		qWarning()<<"ERROR: keywas not valid after generation";
	}
}

KeyPrivate::~KeyPrivate(){
	qDebug()<<"KeyPrivate::~KeyPrivate()" <<" for " <<mKID<<"/"<<mKCT;
}


QString KeyPrivate::hash(QString input)
{
	QCryptographicHash ch(Key::OCTOMY_KEY_HASH);
	ch.addData(input.toUtf8());
	return ch.result().toHex().toUpper();
}


//////////////////////////////////////////////////////////////////////



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
	qDebug()<<"Key::Key(constKey &other)"<<other.d_func()->mKey<<other.d_func()->mPubKey;
}

Key::Key( QString key, bool isPublic)
	: d_ptr(new KeyPrivate(key, isPublic))
{
	qDebug()<<"Key::Key( QString key, bool isPublic):"<<key<<isPublic;
}



Key::Key( quint32 bits )
	: d_ptr(new KeyPrivate(bits))
{
	qDebug()<<"Key::Key( quint32 bits ):"<<bits;
}


Key::Key(KeyPrivate &dd)
	: d_ptr(&dd)
{
	qDebug()<<"Key::Key(KeyPrivate &dd)";
}


Key::Key(Key && other) : Key() {
	swap(*this, other);
	qDebug()<<"Key::Key(Key && other) : Key()";
}

Key & Key::operator=(Key other) {
	swap(*this, other);
	qDebug()<<"Key & Key::operator=(Key other)";
	return *this;
}

bool Key::operator==(Key &other){
	qDebug()<<"bool Key::operator==(Key &other)";
	return ( (other.d_func() == this->d_func() ) || (other.key() == key() ) );
}


bool Key::operator!=(Key &other){
	qDebug()<<"bool Key::operator!=(Key &other)";
	return !operator==(other);
}

void swap(Key& first, Key& second) /* nothrow */ {
	using std::swap;
	qDebug()<<"void swap(Key& first, Key& second) /* nothrow */";
	swap(first.d_ptr, second.d_ptr);
}


