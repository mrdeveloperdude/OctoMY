#include "GLvector.hpp"

#include "audio/utils/AudioUtils.hpp"


void GLvector::random(GLfloat mi, GLfloat ma){
	const auto r = (ma - mi);
	fX = mi + frand() * r;
	fY = mi + frand() * r;
	fZ = mi + frand() * r;
}
