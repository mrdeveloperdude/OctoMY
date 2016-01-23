#ifndef POSEVIEW_HPP
#define POSEVIEW_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

#include "QtLogo3D.hpp"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class PoseView : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	PoseView(QWidget *parent = 0);
	~PoseView();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);
	void cleanup();

signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	void setupVertexAttribs();

	bool m_core;
	int m_xRot;
	int m_yRot;
	int m_zRot;
	QPoint m_lastPos;
	QtLogo3D m_logo;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_logoVbo;
	QOpenGLShaderProgram *m_program;
	int m_projMatrixLoc;
	int m_mvMatrixLoc;
	int m_normalMatrixLoc;
	int m_lightPosLoc;
	QMatrix4x4 m_proj;
	QMatrix4x4 m_camera;
	QMatrix4x4 m_world;
	bool m_transparent;
};

#endif // POSEVIEW_HPP
