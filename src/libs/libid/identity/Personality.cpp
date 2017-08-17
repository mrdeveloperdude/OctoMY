#include "Personality.hpp"

#include "security/SecurityConstants.hpp"

#include "rng/RNG.hpp"

#include <QCryptographicHash>

Personality::Personality(QString id)
	: mID(id)
	, mRNG(nullptr)
{

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
	QCryptographicHash ch(OCTOMY_KEY_HASH);
	ch.addData((mID+key).toUtf8());
	QString nextKey=ch.result().toHex().toUpper();
	Personality ret(nextKey);
	return ret;
}


QString Personality::id() const
{
	return mID;
}



