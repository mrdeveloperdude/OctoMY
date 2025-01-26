#include "MarchingFields.hpp"

#include "HeightWidget.hpp"

#include "uptime/New.hpp"

////////////////////////////////////////////////////////////////////////////////

void Blobs::update(GLfloat fTime) {
	for(GLint iSourceNum = 0; iSourceNum < 3; iSourceNum++)
	{
		sSourcePoint[iSourceNum].fX = 0.5;
		sSourcePoint[iSourceNum].fY = 0.5;
		sSourcePoint[iSourceNum].fZ = 0.5;
	}
	auto fOffset = 1.0 + sinf(fTime);
	sSourcePoint[0].fX *= fOffset;
	sSourcePoint[1].fY *= fOffset;
	sSourcePoint[2].fZ *= fOffset;
}


GLfloat Blobs::density(const GLvector &pt) {
	GLdouble fResult = 0.0;
	GLdouble fDx, fDy, fDz;
	for(GLint iSourceNum = 0; iSourceNum < 3; iSourceNum++){
		fDx = pt.fX - sSourcePoint[0].fX;
		fDy = pt.fY - sSourcePoint[0].fY;
		fDz = pt.fZ - sSourcePoint[0].fZ;
		fResult += (0.5*(iSourceNum+1))/(fDx*fDx + fDy*fDy + fDz*fDz);
	}
	return fResult;
}


GLfloat Blobs::shine(const GLvector &pt) {
	Q_UNUSED(pt);
	return 0.0;
}


void Blobs::color(const GLvector &pt, GLvector &color) {
	Q_UNUSED(pt);
	Q_UNUSED(color);
}


////////////////////////////////////////////////////////////////////////////////

void Bars::update(GLfloat fTime) {
	for(GLint iSourceNum = 0; iSourceNum < 3; iSourceNum++)
	{
		sSourcePoint[iSourceNum].fX = 0.5;
		sSourcePoint[iSourceNum].fY = 0.5;
		sSourcePoint[iSourceNum].fZ = 0.5;
	}
	auto fOffset = 1.0 + sinf(fTime);
	sSourcePoint[0].fX *= fOffset;
	sSourcePoint[1].fY *= fOffset;
	sSourcePoint[2].fZ *= fOffset;
}


GLfloat Bars::density(const GLvector &pt) {
	GLdouble fResult = 0.0;
	GLdouble fDx, fDy, fDz;
	fDx = pt.fX - sSourcePoint[0].fX;
	fDy = pt.fY - sSourcePoint[0].fY;
	fResult += 0.5/(fDx*fDx + fDy*fDy);
	
	fDx = pt.fX - sSourcePoint[1].fX;
	fDz = pt.fZ - sSourcePoint[1].fZ;
	fResult += 0.75/(fDx*fDx + fDz*fDz);
	
	fDy = pt.fY - sSourcePoint[2].fY;
	fDz = pt.fZ - sSourcePoint[2].fZ;
	fResult += 1.0/(fDy*fDy + fDz*fDz);
	
	return fResult;
}


GLfloat Bars::shine(const GLvector &pt) {
	Q_UNUSED(pt);
	return 0.0;
}


void Bars::color(const GLvector &pt, GLvector &color) {
	Q_UNUSED(pt);
	Q_UNUSED(color);
}


////////////////////////////////////////////////////////////////////////////////

void Pond::update(GLfloat t) {
	fTime = t;
}


GLfloat Pond::density(const GLvector &pt) {
	const auto raidus = 40.0*(fTime + sqrt((0.5-pt.fX)*(0.5-pt.fX) + (0.5-pt.fY)*(0.5-pt.fY)));
	GLfloat fHeight = 0.01*(sinf(raidus) + cosf(raidus));
	GLdouble fResult = 0.5f + (fHeight - pt.fZ);
	
	return fResult;
}


GLfloat Pond::shine(const GLvector &pt) {
	Q_UNUSED(pt);
	return 0.0;
}


void Pond::color(const GLvector &pt, GLvector &color) {
	Q_UNUSED(pt);
	Q_UNUSED(color);
}




////////////////////////////////////////////////////////////////////////////////


GLfloat Shape::density(const GLvector &pt) const{
	const auto fDx = pt.fX - position.fX;
	const auto fDy = pt.fY - position.fY;
	const auto fDz = pt.fZ - position.fZ;
	const auto d = (fDx*fDx + fDy*fDy + fDz*fDz + 0.0000001);
	/*
	if(d>radius){
		return 0.0f;
	}
*/
	return magnitude / d;
}


Birth::Birth()
	: mMap(14, 14)
{
	mWidget = OC_NEW HeightWidget(&mMap);
	const int NUM_SHAPES{20};
	mShapes.resize(NUM_SHAPES);
	GLfloat s{0.5f};
	for(int i=0;i<NUM_SHAPES;++i){
		mShapes[i].position.random(-s, +s);
	}
}

void Birth::update(GLfloat t) {
	fTime = t;
	
	for(auto &shape:mShapes){
		shape.update();
	}
	
	mMap.waveStep();
	mMap.swap();
	
	qDebug()<<"update mima:"<<mi<<ma;
	ma=-9999999.0f;
	mi=-ma;
	
	
	mWidget->show();
	mWidget->updateSize();
	mWidget->update();
}


GLfloat Birth::density(const GLvector &pt) {
	GLfloat fHeight = 0.5f;//mMap.sampleTiling(pt.fX, pt.fY);
	GLdouble fDensity = 0.5f + (fHeight - pt.fZ);
	const auto sz = mShapes.size();
	for(const auto &shape:mShapes){
		fDensity += shape.density(pt) / sz;
	}
	
	mi=fmin(mi, fDensity);
	ma=fmax(ma, fDensity);
	return fDensity;
}


GLfloat Birth::shine(const GLvector &pt) {
	Q_UNUSED(pt);
	return 0.0;
}


void Birth::color(const GLvector &pt, GLvector &color) {
	Q_UNUSED(pt);
	Q_UNUSED(color);
}


