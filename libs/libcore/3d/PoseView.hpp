#ifndef POSEVIEW_HPP
#define POSEVIEW_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "scene/GeometryEngine.hpp"
#include "scene/Simulation.hpp"


class PoseView : public QOpenGLWidget, protected QOpenGLFunctions{
		Q_OBJECT
	private:
		QBasicTimer timer;
		QOpenGLShaderProgram program;
		GeometryEngine *geometries;
		QOpenGLTexture *texture;
		QMatrix4x4 projection;
		QVector2D mousePressPosition;
		QVector3D rotationAxis;
		qreal angularSpeed;
		QQuaternion rotation;
		Simulation sim;

	public:
		explicit PoseView(QWidget *parent = 0);
		~PoseView();

	protected:
		void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
		void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
		void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;

		void initializeGL() Q_DECL_OVERRIDE;
		void resizeGL(int w, int h) Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;

		void initShaders();
		void initTextures();



};

#endif // POSEVIEW_HPP
