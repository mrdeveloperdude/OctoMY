#include "PersonalityColors.hpp"

#include "rng/RNG.hpp"

PersonalityColors::PersonalityColors(QString id)
{
	setID(id);;
}

void PersonalityColors::setID(QString id)
{
	mPersonality=Personality(id, Personality::COLORS);
	mP1=(mPersonality.rng().generateReal1());
	mP2=(mPersonality.rng().generateReal1());
	mP3=(mPersonality.rng().generateReal1());
	mBodyColorHigh=(QColor::fromHslF(mP1,0.75f,0.55f));
	mBodyColorLow=(QColor::fromHslF(mP1,0.75f,0.425f));
	mBackgroundColorHigh=(QColor::fromHslF(mP2,0.75f,0.25f));
	mBackgroundColorLow=(QColor::fromHslF(mP2,0.75f,0.20f));
	mLimbColor=(QColor::fromHslF(mP3,0.75f,0.55f));
}

Personality PersonalityColors::personality()
{
	return mPersonality;
}


qreal PersonalityColors::p1() const
{
	return mP1;
}

qreal PersonalityColors::p2() const
{
	return mP2;
}

qreal PersonalityColors::p3() const
{
	return mP3;
}

QColor PersonalityColors::bodyColorHigh() const
{
	return mBodyColorHigh;
}

QColor PersonalityColors::bodyColorLow() const
{
	return mBodyColorLow;
}

QColor PersonalityColors::backgroundColorHigh() const
{
	return mBackgroundColorHigh;
}

QColor PersonalityColors::backgroundColorLow() const
{
	return mBackgroundColorLow;
}

QColor PersonalityColors::limbColor() const
{
	return mLimbColor;
}
