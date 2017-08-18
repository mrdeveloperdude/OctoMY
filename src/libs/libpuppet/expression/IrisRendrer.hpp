#ifndef IRISRENDRER_HPP
#define IRISRENDRER_HPP

#include "identity/Personality.hpp"

#include "Simplex.hpp"

#include <QColor>

class QRect;
class QPainter;

class IrisRendrer
{
private:
	Personality mPersonality;
	Simplex<qreal> mSimplexStreaks;
	Simplex<qreal> mSimplexRipples;
	qreal mOval=0.0;
	qreal mPupilSize=1.0;
	qreal mRingRes=1.0;
	qreal mRingStartRadius=1.0;
	qreal mContrast=1.0;
	qreal mColorFollow=1.0;
	qreal mBaseX=0.0;
	qreal mBaseY=0.0;
	qreal mToneExposure=1.0;
	qreal mToneCutoff=0.025;
	QRgb mColor1=QColor("#8294aa").rgb();
	QRgb mColor2=QColor("#752e0b").rgb();
	QRgb* mScratchBuffer=nullptr;

public:
	IrisRendrer(Personality &p);

public:

	void setPersonality(Personality &p);

	void draw(QRect &rect, QPainter &paint);
	void setParameter(quint32 id, qreal value);
};

#endif // IRISRENDRER_HPP
