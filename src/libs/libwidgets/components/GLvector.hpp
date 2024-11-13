#ifndef GLVECTOR_HPP
#define GLVECTOR_HPP

#include <QOpenGLFunctions>

struct GLvector
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fZ;
	GLvector(GLfloat fX=0.0f, GLfloat fY=0.0f, GLfloat fZ=0.0f)
		: fX(fX)
		, fY(fY)
		, fZ(fZ)
	{
	}
	void random(GLfloat mi, GLfloat ma);
};



#endif // GLVECTOR_HPP
