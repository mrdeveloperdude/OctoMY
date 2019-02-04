#ifndef IRISRENDRER_HPP
#define IRISRENDRER_HPP

#include "identity/Personality.hpp"
#include "identity/PersonalityColors.hpp"
#include "security/PortableID.hpp"

#include "Simplex.hpp"

#include <QColor>

class QRect;
class QPainter;

class IrisRendrer
{
private:
	Personality mPersonality;
	PersonalityColors mColors;
	Simplex<qreal> mSimplexStreaks;
	Simplex<qreal> mSimplexRipples;
	qreal mOval;
	qreal mPupilSize;
	qreal mRingRes;
	qreal mRingStartRadius;
	qreal mContrast;
	qreal mColorFollow;
	qreal mBaseX;
	qreal mBaseY;
	qreal mToneExposure;
	qreal mToneCutoff;
	QRgb mColor1;
	QRgb mColor2;
	QRgb* mScratchBuffer;
	int psz;

	PortableID mPid;

	bool mDebugMode;

public:
	IrisRendrer();

public:

	PortableID portableID();
	void setPortableID(PortableID pid);

	void draw(QRect &rect, QPainter &paint, quint32 eyeIndex=0);
	void setParameter(quint32 id, qreal value);
};

#endif
// IRISRENDRER_HPP
