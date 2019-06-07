#include "ArcBall.hpp"
#include <QtMath>
#include <QVector3D>

#include <QtMath>
ArcBall::ArcBall()
	: dn(false)
	, zoomLevel(1.0)
{

}


void ArcBall::pressed(bool down, QVector2D v)
{
	dn=down;
	if(down) {
		oldPos=v;
		newPos=v;
		downPos=v;
		update();
	}
}

void ArcBall::move(QVector2D v)
{
	if(dn) {
		newPos=v;
		update();
	}
}


void ArcBall::zoom(qreal delta)
{
	zoomLevel+=delta/1000.0;
	zoomLevel=qMax(0.01,zoomLevel);
	update();
//	qDebug()<<"ZOOM: "<<	zoomLevel<< delta;
}


QVector3D ArcBall::getArcBallVector(QVector2D pt2)

{
	QVector3D pt(pt2.x(),-pt2.y(),0.0);
	// compute z-coordinates
	float xySquared = pt.x() * pt.x() + pt.y() * pt.y();
	if(xySquared <= 1.0) {
		pt.setZ(qSqrt(1.0 - xySquared));
	} else {
		pt.normalize();
	}
	return pt;
}



QMatrix4x4 ArcBall::getRelativeMatrix()
{
	return rotMat;
}

QMatrix4x4 ArcBall::getAbsoluteMatrix()
{
	return absoluteMat;;
}


bool ArcBall::isDown()
{
	return dn;
}


void ArcBall::update()
{
	const QVector3D v = getArcBallVector(oldPos);
	const QVector3D u = getArcBallVector(newPos);
	const qreal angle = qAcos(qMin(1.0f, QVector3D::dotProduct(u,v)));
	QVector3D rotAxis = QVector3D::crossProduct(v,u);
	QMatrix4x4 eye2ObjSpaceMat = rotMat.inverted();
	QVector3D objSpaceRotAxis = eye2ObjSpaceMat * rotAxis;
	rotMat.setToIdentity();
	rotMat.rotate(qRadiansToDegrees(angle)*4, objSpaceRotAxis);
	relativeMat=rotMat;
	relativeMat.scale(zoomLevel);
	absoluteRotMat*=rotMat;
	absoluteMat=absoluteRotMat;
	absoluteMat.scale(zoomLevel);
	oldPos=newPos;
}
