#ifndef GEOMETRYENGINE_HPP
#define GEOMETRYENGINE_HPP

#include "glt/IncludeOpenGL.hpp"
#include "glt/GLErrors.hpp"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GeometryEngine : protected QOpenGLFunctions
{
private:
	QOpenGLBuffer arrayBuf;
	QOpenGLBuffer indexBuf;

public:
	GeometryEngine();
	virtual ~GeometryEngine();

	void drawCubeGeometry(QOpenGLShaderProgram *program);

private:
	void initCubeGeometry();

};

#endif // GEOMETRYENGINE_HPP

