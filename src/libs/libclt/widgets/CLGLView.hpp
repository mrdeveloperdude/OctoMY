#ifndef CLGLVIEW_HPP
#define CLGLVIEW_HPP

#include <QtGlobal>
#include <QWidget>

#ifndef OC_USE_LIB_EXT_OPENCL

#include "glt/IncludeOpenGL.hpp"
#include "glt/GLContext.hpp"

// Flat out dummy for the times when OpenCL is not available.
// Hopefully this will result in an empty/black widget in place of
// whatever goodies was supposed to be there.
class CLGLView : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit CLGLView(QWidget *parent = nullptr);
	virtual ~CLGLView();

};

#else

#include "glt/IncludeOpenGL.hpp"
#include "utility/graphics/FPSCalculator.hpp"
#include "utility/ui/ArcBall.hpp"
#include "glt/GLContext.hpp"

#include <QMatrix4x4>

class QOpenGLDebugLogger;
class QOpenGLContext;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class CLGLViewRenderer;

/*!
 * \brief The GLCLView class extends QOpenGLWidget in a way that allows display of
 * the content of a PBO shared with OpenCL running in a separate thread.
 *
 * Although this is kind of complex, for proper GL-CL interop in Qt this is
 * the best way to do it. For more backround, please have a look at
 * \l { http://stackoverflow.com/questions/39542187/what-is-the-correct-way-to-get-opencl-to-play-nice-with-opengl-in-qt5 } { this page }.
 */
class CLGLView : public QOpenGLWidget, protected OCTOMY_QT_OGL_FUNCTIONS_CLASS
{
	Q_OBJECT
private:
	QOpenGLDebugLogger *logger;
	QOpenGLBuffer *canvasVBO;
	QOpenGLTexture *canvasTexture;
	QOpenGLShaderProgram *canvasProgram;
	QOpenGLVertexArrayObject *canvasVAO;
	QMatrix4x4 canvasMatrix;
	GLContext mSharingGLContext;
	FPSCalculator fps;
	QTimer timer;
	bool renderEnabled;
	bool displayEnabled;
	ArcBall arcBall;
	CLGLViewRenderer * mRenderer;
	QSize mLastResize;

public:
	explicit CLGLView(QWidget *parent = nullptr);
	virtual ~CLGLView();

public:
	void setRenderer(CLGLViewRenderer * mRenderer);
	GLContext &sharingGLContext();

	//QOpenGLWidget interface
public:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

	//QWidget interface
private:
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
//Helper
	QVector2D eventToNormalizedVector(QMouseEvent *event);

private:
	void initLogging();
	void initCanvas();
	void initCTX();

private:
	void updateCamera();

public slots:
	void onRenderToggle(bool);
	void onDisplayToggle(bool);

signals:
	void arcBallChange(QMatrix4x4 mat);
	void glInitialized();
};

#endif
// OC_USE_LIB_EXT_OPENCL

#endif
// CLGLVIEW_HPP
