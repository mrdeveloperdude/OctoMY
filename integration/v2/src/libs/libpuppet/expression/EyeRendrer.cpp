#include "EyeRendrer.hpp"

#include "uptime/MethodGate.hpp"

#include <QPainter>
#include <QDebug>
#include <QtMath>

EyeRendrer::EyeRendrer(QVector2D center, float slant, QColor irisColor)
	: mCenter(center)
	, mDirty(true)
	, mSlant(slant)
	, mSpecPos1(-0.35f, -0.12f)
	, mSpecPos2(-0.25f, +0.15f)
	, mLidBrush("brown")
	, mScaleraBrush("white")
	, mIrisBrush(irisColor)
	, mPupilBrush("#BB000000")
	, mSpecularBrush("#88ffffff")
	, mIrisRadius(0.1f * 0.5f)
	, mPupilRadius(0.1f * 0.25f)

{
	OC_METHODGATE();
}

EyeRendrer::~EyeRendrer()
{
	OC_METHODGATE();
}

/*
x = X qCos(a) - Y sin(a)
y = Y qCos(a) + X sin(a)
		*/

/*
static void generateElipsis(QVector2D &out,float rx,float ry,float ang)
{
	QVector2D p(qCos(ang)*rx,sin(ang)*ry);
	out=p;
}*/

void EyeRendrer::update()
{
	OC_METHODGATE();
	if(!mDirty) {
		return;
	}
	mDirty=false;
	mScaleraPolygon.clear();
	mLowerMask.clear();
	mUpperMask.clear();
	float iblink=1.0f-mBlink;
	float overlap=0.3f;
	float r=0.1f;
	float rx=(r+0.5f*r*mSquintSteer.x());
	float ry=(r+0.5f*r*mSquintSteer.y())*iblink;
	float rIrisX=rx*0.4f;
	float rIrisY=ry*0.4f;
	float bend=3.0;
	float lowerMaskRadius=0.8f/(std::pow(bend,(std::fabs(mLowerLidSteer.y())+1.0f))-(bend-1.0f));
	float upperMaskRadius=0.8f/(std::pow(bend,(std::fabs(mUpperLidSteer.y())+1.0f))-(bend-1.0f));
	float lowerMaskOverlap=overlap-mLowerLidSteer.y();
	QVector2D lowerMaskCenter(mLowerLidSteer.x()*lowerMaskRadius, std::cos(mLowerLidSteer.x())*lowerMaskRadius+ry-r*lowerMaskOverlap);
	float upperMaskOverlap=overlap+mUpperLidSteer.y();
	QVector2D upperMaskCenter(mUpperLidSteer.x()*upperMaskRadius, std::cos(mUpperLidSteer.x())*-upperMaskRadius-ry+r*upperMaskOverlap);
	const float pif=static_cast<float>(M_PI);
	for(float i=0; i< pif*2.0f; i+=(pif/40.0f)) {
		QVector2D lm=lowerMaskCenter+QVector2D(std::cos(i)*lowerMaskRadius,std::sin(i)*lowerMaskRadius);
		QVector2D um=upperMaskCenter+QVector2D(std::cos(i)*upperMaskRadius,std::sin(i)*upperMaskRadius);

		{
			QVector2D po(std::cos(i)*rx, std::sin(i)*ry);
			QVector2D p(po.x() * std::cos(mSlant) - po.y() * std::sin(mSlant), po.y() * std::cos(mSlant) + po.x() * std::sin(mSlant) );

			//generateElipsis(p,rx,ry,i);
			QVector2D lt=p-lowerMaskCenter;
			QVector2D ut=p-upperMaskCenter;
			mScaleraPolygon << (lt.length()<lowerMaskRadius?(lt.normalized()*lowerMaskRadius)+lowerMaskCenter:(ut.length()<upperMaskRadius?(ut.normalized()*upperMaskRadius)+upperMaskCenter:p)).toPointF();
			mLowerMask<< lm.toPointF();
			mUpperMask<< um.toPointF();
		}
		{
			QVector2D p(std::cos(i)*rIrisX, std::sin(i)*rIrisY);
			QVector2D lt=p-lowerMaskCenter;
			QVector2D ut=p-upperMaskCenter;
			mPupilPolygon << (lt.length()<lowerMaskRadius?(lt.normalized()*lowerMaskRadius)+lowerMaskCenter:(ut.length()<upperMaskRadius?(ut.normalized()*upperMaskRadius)+upperMaskCenter:p)).toPointF();
		}
	}
}

void EyeRendrer::setBlink(float bl)
{
	OC_METHODGATE();
	if(!qFuzzyCompare(bl, mBlink) ){
		mDirty=true;
	}
	mBlink=bl;
}

void EyeRendrer::setExpression(QVector2D ul, QVector2D ll, QVector2D sq)
{
	OC_METHODGATE();
	mUpperLidSteer=ul+mCenter;
	mLowerLidSteer=ll+mCenter;
	mSquintSteer=sq;
	mDirty=true;
}

void EyeRendrer::setColor(QColor irisColor)
{
	OC_METHODGATE();
	mIrisBrush=QBrush(irisColor);
}

void EyeRendrer::setSteer(QVector2D ey)
{
	OC_METHODGATE();
	ey+=mCenter;
	if(!qFuzzyCompare(ey, mEyeSteer)) {
		mDirty=true;
	}
	mEyeSteer=ey;
}


void EyeRendrer::setIrisImage(QImage irisImage)
{
	OC_METHODGATE();
	mIrisImage=irisImage;
	mDirty=true;
}

void EyeRendrer::paint(QPainter &painter)
{
	OC_METHODGATE();
	//painter.setBrush(lidBrush);
	//painter.drawPolygon(lowerMask);
	//painter.drawPolygon(upperMask);

	painter.setBrush(mScaleraBrush);
	painter.drawPolygon(mScaleraPolygon);


	painter.setBrush(mIrisBrush);
	//painter.drawPolygon(irisPolygon); //TODO: Use this instead
	const qreal ir=static_cast<qreal>(mIrisRadius);
	const qreal pr=static_cast<qreal>(mPupilRadius);
	painter.drawEllipse((mEyeSteer-mCenter).toPointF(), ir, ir);

	painter.setBrush(mPupilBrush);
	painter.drawEllipse((mEyeSteer-mCenter).toPointF(), pr, pr);

	if(!mIrisImage.isNull()) {
		//qDebug()<<"DRAWING IRIS IMAGE "<<mIrisImage;
		QPointF ppt((mEyeSteer-mCenter).toPointF()-QPointF(ir, ir));
		QSizeF psz(ir*2, ir*2);
		QRectF rect(ppt, psz);
		painter.drawImage(rect, mIrisImage);
	}

	painter.setBrush(mSpecularBrush);
	painter.drawEllipse((mEyeSteer-mCenter+mSpecPos1*0.1f).toPointF(),0.1*0.15,0.1*0.15);
	painter.drawEllipse((mEyeSteer-mCenter+mSpecPos2*0.1f).toPointF(),0.1*0.08,0.1*0.08);

}

float EyeRendrer::irisRadius()
{
	OC_METHODGATE();
	return mIrisRadius;
}

float EyeRendrer::pupilRadius()
{
	OC_METHODGATE();
	return mPupilRadius;
}


bool EyeRendrer::isDirty()
{
	OC_METHODGATE();
	return mDirty;
}
