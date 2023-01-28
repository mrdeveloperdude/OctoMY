#ifndef GLSTRESSWIDGET_HPP
#define GLSTRESSWIDGET_HPP

#include "glt/IncludeOpenGL.hpp"

#include "glt/GLErrors.hpp"

#include "GeometryEngine.hpp"

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class GeometryEngine;


class GLBox{
	public:
		QVector3D pos;
		QVector3D rotationAxis;
		qreal angularSpeed;
		QQuaternion rotation;
	public:
		explicit GLBox(QVector3D pos,  QVector3D rotationAxis,qreal angularSpeed):
			pos(pos)
		  , rotationAxis(rotationAxis)
		  , angularSpeed(angularSpeed)
		{

		}

		void update(){
			angularSpeed*=0.9;
			// Stop rotation when speed goes below threshold
			if (angularSpeed < 0.01) {
				int big=100000;
				angularSpeed = (qreal(qrand()%big-(big/2))/(qreal)big)*30.0;
				rotationAxis=QVector3D(qrand(),qrand(),qrand()).normalized()*2.0-QVector3D(1,1,1);
			} else {
				// Update rotation
				rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
				// Request an update
			}
		}


};

#define NUM_BOX (500)

class GLStressWidget : public QOpenGLWidget, protected OCTOMY_QT_OGL_FUNCTIONS_CLASS{
		Q_OBJECT

	private:
		qint64 lastRenderRime;
		QOpenGLShaderProgram program;
		GeometryEngine *geometries;
		QOpenGLTexture *texture;
		QMatrix4x4 projection;
		QVector2D mousePressPosition;

		GLBox *boxes[NUM_BOX];

	public:
		explicit GLStressWidget(QWidget *parent = nullptr);
		virtual ~GLStressWidget();

	public:

		void initTextures(bool f);

	protected:
		//void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
		//void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

		void initializeGL() Q_DECL_OVERRIDE;
		void resizeGL(int w, int h) Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;

		void initShaders();

};

#endif // GLSTRESSWIDGET_HPP

