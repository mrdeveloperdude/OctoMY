#include "KeyPrivate.hpp"

#include "utility/Standard.hpp"
#include "utility/ScopedTimer.hpp"
#include "SecurityConstants.hpp"

#include <QCryptographicHash>

int KeyPrivate::mKCT=0;

KeyPrivate::KeyPrivate( QVariantMap map, bool isPublic )
	: KeyPrivate(isPublic?(map.contains("publicKey")?map["publicKey"].toString():""): (map.contains("privateKey")?map["privateKey"].toString():""), isPublic)
/*
	: mKey(isPublic?"":map.contains("privateKey")?map["privateKey"].toString():"")
	, mPubKey(map.contains("publicKey")?map["publicKey"].toString():"")
	, mKID(mKCT++)
	, mValidPrivate(false)
	, mValidPublic(false)
	*/
{
	OC_METHODGATE();

	//qDebug()<<"KeyPrivate::KeyPrivate( QVariantMap key, bool isPublic ): map="<<map<<", mKey="<<mKey<<", mPubKey="<<mPubKey<<", isPublic="<<isPublic <<" for " <<mKID<<"/"<<mKCT;
	//parse(isPublic);
}


KeyPrivate::KeyPrivate( QString key, bool isPublic )
	: mKey(isPublic?"":key)
	, mPubKey(isPublic?key:"")
	, mKID(mKCT++)
	, mValidPrivate(false)
	, mValidPublic(false)
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"KeyPrivate::KeyPrivate( QString key, bool isPublic ):"<<key<<isPublic <<" for " <<mKID<<"/"<<mKCT;
	parse(isPublic);
	mInitialized=true;
}


KeyPrivate::KeyPrivate(quint32 bits)
	: mKID(mKCT++)
	, mValidPrivate(false)
	, mValidPublic(false)
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"KeyPrivate::KeyPrivate(quint32 bits):"<<bits<<" for " <<mKID<<"/"<<mKCT;
	ScopedTimer timer("Key Generation");
	mPKI.generateKeyPair(bits);
	if(mPKI.isValid()) {
		mKey=mPKI.getPEMKey();
		mPubKey=mPKI.getPEMPubkey();
		mID=hash(mPubKey);
		mValidPrivate=true;
		mValidPublic=true;
	} else {
		qWarning()<<"ERROR: keywas not valid after generation";
	}
	mInitialized=true;
}


void KeyPrivate::parse(bool publicOnly)
{
	//ScopedTimer timer("Key Parsing");
	mValidPrivate=false;
	mValidPublic=false;

	mID="";
	if(publicOnly) {
		QByteArray raw=mPubKey.toUtf8();
		if(0!=mPKI.parsePublicKey(raw)) {
			qWarning()<<"ERROR: Could not parse pubkey: "<<raw;
			return;
		}
		mValidPublic=true;
	} else {
		QByteArray raw=mKey.toUtf8();
		if(""!=raw) {
			if(0!=mPKI.parseKey(raw)) {
				qWarning()<<"ERROR: Could not parse key: "<<raw;
				return;
			}
			mPubKey=mPKI.getPEMPubkey();
			if(mPubKey.isEmpty()) {
				qWarning()<<"ERROR: Could not get pubkey from key: "<<raw;
				return;
			}
		} else {
			qWarning()<<"ERROR: Private key text was empty";
			return;
		}
		mValidPrivate=true;
		mValidPublic=true;
	}
	mID=hash(mPubKey);
}

KeyPrivate::KeyPrivate()
	: mKID(mKCT++)
{
	OC_METHODGATE();
	//	qDebug()<<"KeyPrivate::KeyPrivate()" <<" for " <<mKID<<"/"<<mKCT;
}

KeyPrivate::~KeyPrivate()
{
	OC_METHODGATE();
	//	qDebug()<<"KeyPrivate::~KeyPrivate()" <<" for " <<mKID<<"/"<<mKCT;
}


QString KeyPrivate::hash(QString input)
{
	OC_FUNCTIONGATE();
	QCryptographicHash ch(OCTOMY_KEY_HASH);
	ch.addData(input.toUtf8());
	return ch.result().toHex().toUpper();
}
