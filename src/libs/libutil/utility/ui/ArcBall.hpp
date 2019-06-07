#ifndef ARCBALL_HPP
#define ARCBALL_HPP

#include <QVector2D>

#include <QMatrix4x4>

class ArcBall
{
private:
	bool dn;
	QVector2D downPos;
	qreal zoomLevel;
	QVector2D oldPos;
	QVector2D newPos;
	QMatrix4x4 rotMat;
	QMatrix4x4 relativeMat;
	QMatrix4x4 absoluteRotMat;
	QMatrix4x4 absoluteMat;
public:
	ArcBall();

public:
	void pressed(bool down, QVector2D v);
	void move(QVector2D v);
	void zoom(qreal delta);

	bool isDown();

	QMatrix4x4 getAbsoluteMatrix();
	QMatrix4x4 getRelativeMatrix();

private:

	QVector3D getArcBallVector(QVector2D pt);

	void update();


};

#endif // ARCBALL_HPP
