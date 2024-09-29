#include "MarchingFields.hpp"


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
	GLfloat fHeight = 20.0*(fTime + sqrt((0.5-pt.fX)*(0.5-pt.fX) + (0.5-pt.fY)*(0.5-pt.fY)));
	fHeight = 1.5 + 0.1*(sinf(fHeight) + cosf(fHeight));
	GLdouble fResult = (fHeight - pt.fZ)*50.0;
	
	return fResult;
}


GLfloat Pond::shine(const GLvector &pt) {
	return 0.0;
}


void Pond::color(const GLvector &pt, GLvector &color) {
	Q_UNUSED(pt);
	Q_UNUSED(color);
}




////////////////////////////////////////////////////////////////////////////////

void Birth::update(GLfloat t) {
	fTime = t;
	mMap.waveStep();
	mMap.swap();
}


GLfloat Birth::density(const GLvector &pt) {
	return (1.5f - pt.fZ + mMap.sampleTiling(pt.fX, pt.fY))*50.0f;
}


GLfloat Birth::shine(const GLvector &pt) {
	return 0.0;
}


void Birth::color(const GLvector &pt, GLvector &color) {
	Q_UNUSED(pt);
	Q_UNUSED(color);
}


