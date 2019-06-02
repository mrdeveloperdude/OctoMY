#ifndef EYERENDRER_HPP
#define EYERENDRER_HPP

#include <QVector2D>
#include <QSharedPointer>
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
	qreal mBlink;
	qreal mSlant;

	QVector2D mSpecPos1;
	QVector2D mSpecPos2;
	QBrush mLidBrush;
	QBrush mScaleraBrush;
	QBrush mIrisBrush;
	QBrush mPupilBrush;
	QBrush mSpecularBrush;

	qreal mIrisRadius;
	qreal mPupilRadius;

	QImage mIrisImage;

public:

	explicit EyeRendrer(QVector2D mCenter, qreal mSlant, QColor irisColor=QColor("#2d8ac9"));
	virtual ~EyeRendrer();
	void update();
	void setBlink(qreal);
	void setExpression(QVector2D, QVector2D, QVector2D);
	void setColor(QColor irisColor=QColor("#2d8ac9"));
	void setIrisImage(QImage);
	void setSteer(QVector2D);
	void paint(QPainter &painter);


	qreal irisRadius();
	qreal pupilRadius();

	bool isDirty();

};


#endif // EYERENDRER_HPP
