#ifndef COMPASWIDGET_HPP
#define COMPASWIDGET_HPP


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>

#include "Logo.hpp"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class CompasWidget : public QOpenGLWidget, protected QOpenGLFunctions{
		Q_OBJECT

	private:
		void setupVertexAttribs();
		void setupVertexAttribs2();
		bool m_core;
		int m_xRot;
		int m_yRot;
		int m_zRot;
		QPoint m_lastPos;
		Logo m_logo;
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

		//QOpenGLContext *m_context;
		QOffscreenSurface *m_offscreenSurface;
		QOpenGLShaderProgram *m_program2;
		QOpenGLFramebufferObject *m_fbo2;
		QOpenGLBuffer *m_vbo2;
		QOpenGLVertexArrayObject *m_vao2;
		int m_matrixLoc;

	public:
		CompasWidget(QWidget *parent = nullptr);
		~CompasWidget();

		QSize minimumSizeHint() const override;
		QSize sizeHint() const override;

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
		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int width, int height) override;
		void mousePressEvent(QMouseEvent *event) override;
		void mouseMoveEvent(QMouseEvent *event) override;



};

#endif // COMPASWIDGET_HPP
