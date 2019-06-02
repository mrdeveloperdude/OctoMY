#include "Personality.hpp"

#include "security/SecurityConstants.hpp"
#include "random/RNG.hpp"

#include "uptime/MethodGate.hpp"

#include <QCryptographicHash>


const QString Personality::COLORS="colors";
const QString Personality::IDENTICON="identicon";
const QString Personality::IRIS="iris";


Personality::Personality(QString id)
	: mRNG(nullptr)
{
	OC_METHODGATE();
	setID(id, "");
}

Personality::Personality(QString id, QString key)
	: mRNG(nullptr)
{
	OC_METHODGATE();
	setID(id, key);
}

Personality::Personality( const Personality& other )
	  // We don't copy this to avoid all sorts of nasty problems (it is regenerated anyways...)
	: mRNG(nullptr)
{
	OC_METHODGATE();
	setID(other.mID, "");
}



Personality::~Personality()
{
	OC_METHODGATE();
	mID="";
	delete mRNG;
	mRNG=nullptr;
}



void Personality::reset()
{
	OC_METHODGATE();
	if(nullptr!=mRNG) {
		mRNG->init(mID.toUtf8());
	}
}

RNG &Personality::rng()
{
	OC_METHODGATE();
	if(nullptr==mRNG) {
		mRNG=RNG::sourceFactory("mt");
		reset();
	}
	return *mRNG;
}

Personality Personality::subPersonality(QString key)
{
	OC_METHODGATE();
	return Personality(mID, key);
}

void Personality::setID(QString id, QString key)
{
	OC_METHODGATE();
	if(key.isEmpty()) {
		mID=id;
	} else {
		QCryptographicHash ch(OCTOMY_KEY_HASH);
		ch.addData((id+key).toUtf8());
		mID=ch.result().toHex().toUpper();
	}
}

QString Personality::id() const
{
	OC_METHODGATE();
	return mID;
}
