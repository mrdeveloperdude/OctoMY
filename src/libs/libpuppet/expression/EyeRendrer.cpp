#include "EyeRendrer.hpp"

#include <QPainter>

#include <QDebug>

#include <QtMath>

EyeRendrer::EyeRendrer(QVector2D center, qreal slant, QColor irisColor)
	: mCenter(center)
	, mDirty(true)
	, mSlant(slant)
	, mSpecPos1(-0.35,-0.12)
	, mSpecPos2(-0.25,+0.15)
	, mLidBrush("brown")
	, mScaleraBrush("white")
	, mIrisBrush(irisColor)
	, mPupilBrush("#BB000000")
	, mSpecularBrush("#88ffffff")
	, mIrisRadius(0.1*0.5)
	, mPupilRadius(0.1*0.25)

{

}

EyeRendrer::~EyeRendrer()
{

}

/*
x = X qCos(a) - Y sin(a)
y = Y qCos(a) + X sin(a)
		*/

/*
static void generateElipsis(QVector2D &out,qreal rx,qreal ry,qreal ang)
{
	QVector2D p(qCos(ang)*rx,sin(ang)*ry);
	out=p;
}*/

void EyeRendrer::update()
{
	if(!mDirty) {
		return;
	}
	mDirty=false;
	mScaleraPolygon.clear();
	mLowerMask.clear();
	mUpperMask.clear();
	qreal iblink=1.0-mBlink;
	qreal overlap=0.3;
	qreal r=0.1;
	qreal rx=(r+0.5*r*mSquintSteer.x());
	qreal ry=(r+0.5*r*mSquintSteer.y())*iblink;
	qreal rIrisX=rx*0.4;
	qreal rIrisY=ry*0.4;
	qreal bend=3.0;
	qreal lowerMaskRadius=0.8/(pow(bend,(qFabs(mLowerLidSteer.y())+1))-(bend-1.0));
	qreal upperMaskRadius=0.8/(pow(bend,(qFabs(mUpperLidSteer.y())+1))-(bend-1.0));
	qreal lowerMaskOverlap=overlap-mLowerLidSteer.y();
	QVector2D lowerMaskCenter(mLowerLidSteer.x()*lowerMaskRadius, qCos(mLowerLidSteer.x())*lowerMaskRadius+ry-r*lowerMaskOverlap);
	qreal upperMaskOverlap=overlap+mUpperLidSteer.y();
	QVector2D upperMaskCenter(mUpperLidSteer.x()*upperMaskRadius, qCos(mUpperLidSteer.x())*-upperMaskRadius-ry+r*upperMaskOverlap);
	for(qreal i=0; i<M_PI*2; i+=(M_PI/40.0)) {
		QVector2D lm=lowerMaskCenter+QVector2D(qCos(i)*lowerMaskRadius,qSin(i)*lowerMaskRadius);
		QVector2D um=upperMaskCenter+QVector2D(qCos(i)*upperMaskRadius,qSin(i)*upperMaskRadius);

		{
			QVector2D po(qCos(i)*rx, qSin(i)*ry);
			QVector2D p(po.x() * qCos(mSlant) - po.y() * qSin(mSlant), po.y() * qCos(mSlant) + po.x() * qSin(mSlant) );

			//generateElipsis(p,rx,ry,i);
			QVector2D lt=p-lowerMaskCenter;
			QVector2D ut=p-upperMaskCenter;
			mScaleraPolygon << (lt.length()<lowerMaskRadius?(lt.normalized()*lowerMaskRadius)+lowerMaskCenter:(ut.length()<upperMaskRadius?(ut.normalized()*upperMaskRadius)+upperMaskCenter:p)).toPointF();
			mLowerMask<< lm.toPointF();
			mUpperMask<< um.toPointF();
		}
		{
			QVector2D p(qCos(i)*rIrisX, qSin(i)*rIrisY);
			QVector2D lt=p-lowerMaskCenter;
			QVector2D ut=p-upperMaskCenter;
			mPupilPolygon << (lt.length()<lowerMaskRadius?(lt.normalized()*lowerMaskRadius)+lowerMaskCenter:(ut.length()<upperMaskRadius?(ut.normalized()*upperMaskRadius)+upperMaskCenter:p)).toPointF();
		}
	}
}

void EyeRendrer::setBlink(qreal bl)
{
	if(bl!=mBlink) {
		mDirty=true;
	}
	mBlink=bl;
}

void EyeRendrer::setExpression(QVector2D ul, QVector2D ll, QVector2D sq)
{
	mUpperLidSteer=ul+mCenter;
	mLowerLidSteer=ll+mCenter;
	mSquintSteer=sq;
	mDirty=true;
}

void EyeRendrer::setColor(QColor irisColor)
{
	mIrisBrush=QBrush(irisColor);
}

void EyeRendrer::setSteer(QVector2D ey)
{
	ey+=mCenter;
	if(!qFuzzyCompare(ey, mEyeSteer)) {
		mDirty=true;
	}
	mEyeSteer=ey;
}


void EyeRendrer::setIrisImage(QImage irisImage)
{
	mIrisImage=irisImage;
	mDirty=true;
}

void EyeRendrer::paint(QPainter &painter)
{
	//painter.setBrush(lidBrush);
	//painter.drawPolygon(lowerMask);
	//painter.drawPolygon(upperMask);

	painter.setBrush(mScaleraBrush);
	painter.drawPolygon(mScaleraPolygon);


	painter.setBrush(mIrisBrush);
	//painter.drawPolygon(irisPolygon); //TODO: Use this instead
	painter.drawEllipse((mEyeSteer-mCenter).toPointF(),mIrisRadius, mIrisRadius);

	painter.setBrush(mPupilBrush);
	painter.drawEllipse((mEyeSteer-mCenter).toPointF(), mPupilRadius, mPupilRadius);

	if(!mIrisImage.isNull()) {
		//qDebug()<<"DRAWING IRIS IMAGE "<<mIrisImage;
		QPointF ppt((mEyeSteer-mCenter).toPointF()-QPointF(mIrisRadius, mIrisRadius));
		QSizeF psz(mIrisRadius*2, mIrisRadius*2);
		QRectF rect(ppt, psz);
		painter.drawImage(rect, mIrisImage);
	}

	painter.setBrush(mSpecularBrush);
	painter.drawEllipse((mEyeSteer-mCenter+mSpecPos1*0.1).toPointF(),0.1*0.15,0.1*0.15);
	painter.drawEllipse((mEyeSteer-mCenter+mSpecPos2*0.1).toPointF(),0.1*0.08,0.1*0.08);

}

qreal EyeRendrer::irisRadius()
{
	return mIrisRadius;
}

qreal EyeRendrer::pupilRadius()
{
	return mPupilRadius;
}
