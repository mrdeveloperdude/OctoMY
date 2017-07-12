#include "KeyPrivate.hpp"

#include "utility/Standard.hpp"
#include "utility/ScopedTimer.hpp"
#include "SecurityConstants.hpp"

#include <QCryptographicHash>

int KeyPrivate::mKCT=0;

KeyPrivate::KeyPrivate( QVariantMap map, bool isPublic )
	: KeyPrivate(isPublic?(map.contains("publicKey")?map["publicKey"].toString():""): (map.contains("privateKey")?map["privateKey"].toString():""), isPublic)
{
	OC_METHODGATE();
	qDebug()<<"KeyPrivate::KeyPrivate( QVariantMap key, bool isPublic ): map="<<map<<", mKey="<<mKey<<", mPubKey="<<mPubKey<<", isPublic="<<isPublic <<" for " <<mKID<<"/"<<mKCT;
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
	qDebug()<<"KeyPrivate::KeyPrivate( QString key, bool isPublic ):"<<key<<isPublic <<" for " <<mKID<<"/"<<mKCT;
	qDebug()<<"isPublic="<<isPublic;
	qDebug()<<"mKey="<<mKey;
	qDebug()<<"mPubKey="<<mPubKey;
	parse(isPublic);
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
	generate(bits);
}




KeyPrivate::KeyPrivate()
	: mKID(mKCT++)
	, mValidPrivate(false)
	, mValidPublic(false)
	, mInitialized(false)
{
	OC_METHODGATE();
	//	qDebug()<<"KeyPrivate::KeyPrivate()" <<" for " <<mKID<<"/"<<mKCT;
}


KeyPrivate::~KeyPrivate()
{
	OC_METHODGATE();
	if(!mInitialized) {
		qWarning()<<"ERROR: dtor called before initialized in KeyPrivate";
	}
	mKey.clear();
	mPubKey.clear();
	mID.clear();
	mPKI.reset();
	mKID=-1;
	mValidPrivate=false;
	mValidPublic=false;
	mInitialized=false;
	//	qDebug()<<"KeyPrivate::~KeyPrivate()" <<" for " <<mKID<<"/"<<mKCT;
}



QString KeyPrivate::hash(QString input)
{
	OC_FUNCTIONGATE();
	QCryptographicHash ch(OCTOMY_KEY_HASH);
	ch.addData(input.toUtf8());
	return ch.result().toHex().toUpper();
}


void KeyPrivate::parse(bool publicOnly)
{
	//ScopedTimer timer("Key Parsing");
	mValidPrivate=false;
	mValidPublic=false;
	mID="";
	if(publicOnly) {
		QByteArray raw=mPubKey.toUtf8();
		int ret=mPKI.parsePublicKey(raw);
		if(0==ret) {
			mValidPublic=true;
		} else {
			qDebug()<<"ERROR: "<<ret<<"="<<qPolarDescribeError(ret);
			qWarning()<<"ERROR: Could not parse pubkey: "<<raw;
		}
	} else {
		QByteArray raw=mKey.toUtf8();
		if(!raw.isEmpty()) {
			if(0==mPKI.parseKey(raw)) {
				mPubKey=mPKI.getPEMPubkey();
				if(!mPubKey.isEmpty()) {
					mValidPrivate=true;
					mValidPublic=true;
				} else {
					qWarning()<<"ERROR: Could not get pubkey from key: "<<raw;
				}
			} else {
				qWarning()<<"ERROR: Could not parse key: "<<raw;
			}
		} else {
			qWarning()<<"ERROR: Private key text was empty";
		}
	}
	if(mPKI.isValid()) {
		mID=hash(mPubKey);
	} else {
		qWarning()<<"ERROR: key was not valid after parsing";
	}
	mInitialized=true;
}


void KeyPrivate::generate(quint32 bits)
{
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




QString KeyPrivate::describe()
{
	OC_METHODGATE();
	QString pkiName=QString(mPKI.name());
	bool pkiValid=           mPKI.isValid();
	size_t    pkiSizeBits=      mPKI.keySizeBits();
	size_t         pkiSizeBytes= mPKI.keySizeBytes();
//	bool            pkiCanXXX=mPKI.canDo(TPki type);
//	TPki            pkiType=mPKI.type();

	QString ret="";
	ret+= "          mKey="+mKey;
	ret+="\n      mPubKey="+mPubKey;
	ret+="\n          mID="+mID;
	ret+="\n         mKCT="+QString::number(mKCT);
	ret+="\n         mKID="+QString::number(mKID);
	ret+="\nmValidPrivate="+QString(mValidPrivate?"true ":"false");
	ret+="\n mValidPublic="+QString(mValidPublic?"true ":"false");
	ret+="\n mInitialized="+QString(mInitialized?"true ":"false");
	ret+="\n     pkiName="+pkiName;
	ret+="\n    pkiValid="+QString(pkiValid?"true ":"false");
	ret+="\n pkiSizeBits="+QString::number(pkiSizeBits);
	ret+="\npkiSizeBytes="+QString::number(pkiSizeBytes);




	QStringList list=ret.split("\n",QString::SkipEmptyParts);
	for(QString str:list) {
		qDebug()<<str;
	}
	//qpolarssl::Pki mPKI;


	return ret;
}
