#ifndef GEOMETRYENGINE_HPP
#define GEOMETRYENGINE_HPP


#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GeometryEngine : protected QOpenGLFunctions{
public:
	GeometryEngine();
	virtual ~GeometryEngine();

	void drawCubeGeometry(QOpenGLShaderProgram *program);

private:
	void initCubeGeometry();

	QOpenGLBuffer arrayBuf;
	QOpenGLBuffer indexBuf;
};

#endif // GEOMETRYENGINE_HPP
