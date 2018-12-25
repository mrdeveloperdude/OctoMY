#ifndef GEOMETRYENGINE_HPP
#define GEOMETRYENGINE_HPP

#ifdef USE_GEOMETRY_ENGINE

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

#endif
// USE_GEOMETRY_ENGINE


#endif // GEOMETRYENGINE_HPP

