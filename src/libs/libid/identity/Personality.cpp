#include "Personality.hpp"

#include "security/SecurityConstants.hpp"
#include "rng/RNG.hpp"

#include <QCryptographicHash>


const QString Personality::COLORS="colors";
const QString Personality::IDENTICON="identicon";
const QString Personality::IRIS="iris";


Personality::Personality(QString id)
	: mID(id)
	, mRNG(nullptr)
{

}

Personality::Personality(QString id, QString key)
	: mRNG(nullptr)
{
	setID(id, key);
}


Personality::~Personality()
{
	delete mRNG;
}

void Personality::reset()
{
	if(nullptr!=mRNG) {
		mRNG->init(mID.toUtf8());
	}
}

RNG &Personality::rng()
{
	if(nullptr==mRNG) {
		mRNG=RNG::sourceFactory("mt");
		reset();
	}
	return *mRNG;
}

Personality Personality::subPersonality(QString key)
{
	return Personality(mID, key);
}

void Personality::setID(QString id, QString key)
{
	QCryptographicHash ch(OCTOMY_KEY_HASH);
	ch.addData((id+key).toUtf8());
	mID=ch.result().toHex().toUpper();
}

QString Personality::id() const
{
	return mID;
}
