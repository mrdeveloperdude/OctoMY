#ifndef CLGLVIEW_HPP
#define CLGLVIEW_HPP

#include <QtGlobal>

#ifndef EXTERNAL_LIB_OPENCL

#include <QOpenGLWidget>

class CLGLView : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit CLGLView(QWidget *parent = nullptr){}
	virtual ~CLGLView(){}

	//QOpenGLWidget interface
public:
	void initializeGL() Q_DECL_OVERRIDE{}
	void resizeGL(int w, int h) Q_DECL_OVERRIDE{}
	void paintGL() Q_DECL_OVERRIDE{}
};

#else

#include "../libutil/utility/IncludeOpenGL.hpp"
#include "../libutil/utility/FPSCalculator.hpp"
#include "../libutil/utility/ArcBall.hpp"
#include "../libutil/utility/GLContext.hpp"



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
 * This sounds kind of complex, but for proper GL-CL interop in Qt this is
 * the best way to do it. For more backround, please look at \l { http://stackoverflow.com/questions/39542187/what-is-the-correct-way-to-get-opencl-to-play-nice-with-opengl-in-qt5 } { this page }.
 *
 *
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
	GLContext mGLCTX;
	FPSCalculator fps;
	QTimer timer;
	bool renderEnabled;
	bool displayEnabled;
	ArcBall arcBall;
	CLGLViewRenderer * mRenderer;

public:
	explicit CLGLView(QWidget *parent = nullptr);
	virtual ~CLGLView();

public:

	void setRenderer(CLGLViewRenderer * mRenderer);

	GLContext &glctx();

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

#endif // CLGLVIEW_HPP
