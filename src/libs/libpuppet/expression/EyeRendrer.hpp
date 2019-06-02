#ifndef EYERENDRER_HPP
#define EYERENDRER_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QVector2D>
#include <QImage>
#include <QBrush>

class EyeRendrer
{
private:
	QVector2D mCenter;
	bool mDirty;
	QPolygonF mBasicEyeShapeBg;
	QPolygonF mScaleraPolygon;
	QPolygonF mIrisPolygon;
	QPolygonF mPupilPolygon;
	QPolygonF mUpperMask;
	QPolygonF mLowerMask;
	QVector2D mEyeSteer;
	QVector2D mUpperLidSteer;
	QVector2D mLowerLidSteer;
	QVector2D mSquintSteer;
	float mBlink;
	float mSlant;

	QVector2D mSpecPos1;
	QVector2D mSpecPos2;
	QBrush mLidBrush;
	QBrush mScaleraBrush;
	QBrush mIrisBrush;
	QBrush mPupilBrush;
	QBrush mSpecularBrush;

	float mIrisRadius;
	float mPupilRadius;

	QImage mIrisImage;

public:
	explicit EyeRendrer(QVector2D mCenter, float mSlant, QColor irisColor=QColor("#2d8ac9"));
	virtual ~EyeRendrer();

public:
	void update();
	void setBlink(float);
	void setExpression(QVector2D, QVector2D, QVector2D);
	void setColor(QColor irisColor=QColor("#2d8ac9"));
	void setIrisImage(QImage);
	void setSteer(QVector2D);
	void paint(QPainter &painter);

	float irisRadius();
	float pupilRadius();

	bool isDirty();

};


#endif
// EYERENDRER_HPP
