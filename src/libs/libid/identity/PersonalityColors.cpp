#include "PersonalityColors.hpp"

#include "uptime/MethodGate.hpp"

#include "random/RNG.hpp"

PersonalityColors::PersonalityColors(QString id)
{
	OC_METHODGATE();
	setID(id);
}

void PersonalityColors::setID(QString id)
{
	OC_METHODGATE();
	if(id.isEmpty()) {
		// Generate the no-op personality colors
		mP1=(1.0);
		mP2=(0.5);
		mP3=(0.0);
		mBodyColorHigh=(Qt::red);
		mBodyColorLow=(Qt::green);
		mBackgroundColorHigh=(Qt::blue);
		mBackgroundColorLow=(Qt::yellow);
		mLimbColor=(Qt::magenta);
	} else {
		mPersonality=Personality(id, Personality::COLORS);
		RNG &rng=mPersonality.rng();
		mP1=(rng.generateReal1());
		mP2=(rng.generateReal1());
		mP3=(rng.generateReal1());
		mBodyColorHigh=(QColor::fromHslF(mP1,0.75,0.55));
		mBodyColorLow=(QColor::fromHslF(mP1,0.75,0.425));
		mBackgroundColorHigh=(QColor::fromHslF(mP2,0.75,0.25));
		mBackgroundColorLow=(QColor::fromHslF(mP2,0.75,0.20));
		mLimbColor=(QColor::fromHslF(mP3,0.75,0.55));
	}
}

Personality PersonalityColors::personality()
{
	OC_METHODGATE();
	return mPersonality;
}


qreal PersonalityColors::p1() const
{
	OC_METHODGATE();
	return mP1;
}

qreal PersonalityColors::p2() const
{
	OC_METHODGATE();
	return mP2;
}

qreal PersonalityColors::p3() const
{
	OC_METHODGATE();
	return mP3;
}

QColor PersonalityColors::bodyColorHigh() const
{
	OC_METHODGATE();
	return mBodyColorHigh;
}

QColor PersonalityColors::bodyColorLow() const
{
	OC_METHODGATE();
	return mBodyColorLow;
}

QColor PersonalityColors::backgroundColorHigh() const
{
	OC_METHODGATE();
	return mBackgroundColorHigh;
}

QColor PersonalityColors::backgroundColorLow() const
{
	OC_METHODGATE();
	return mBackgroundColorLow;
}

QColor PersonalityColors::limbColor() const
{
	OC_METHODGATE();
	return mLimbColor;
}
