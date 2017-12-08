#include "GLStressWidget.hpp"

#include "glt/IncludeOpenGL.hpp"
#include <QMouseEvent>
#include <QSurfaceFormat>

#include <cmath>

GLStressWidget::GLStressWidget(QWidget *parent) :
	QOpenGLWidget(parent)
	, lastRenderRime(0)
	, geometries(0)
	, texture(0)
{
	int x=0,y=0;
	int sz=(int)sqrt(NUM_BOX);
	int sz2=sz/2;
	for(int i=0; i<NUM_BOX; ++i) {
		x++;
		if(x>sz) {
			x=0;
			y++;
		}

		boxes[i]=new GLBox(QVector3D( qreal(x-sz2)/sz,qreal(y-sz2)/sz,0)*4,QVector3D(0,0,1),0.0);
	}
}

GLStressWidget::~GLStressWidget()
{
	makeCurrent();
	delete texture;
	delete geometries;
	doneCurrent();
	for(int i=0; i<NUM_BOX; ++i) {
		delete boxes[i];
		boxes[i]=0;
	}
}

GLuint vao;

//#include <QOpenGLFunctions_4_3_Core>
void GLStressWidget::initializeGL()
{
	qDebug()<<"Initializing GL";
	makeCurrent();
	QOpenGLContext *ctx=context();
	enableGLLogger(ctx);
	auto fmt=format();
	qDebug() << fmt<<", renderableType=" << fmt.renderableType();
	initializeOpenGLFunctions();
	#if OCTOMY_QT_OGL_VERSION_MAJOR > 2
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	#endif

	glClearColor(0, 0, 1, 1);
	GLSPEWERROR;
	initShaders();
	initTextures(false);
	glEnable(GL_DEPTH_TEST);
	GLSPEWERROR;
	glEnable(GL_CULL_FACE);
	GLSPEWERROR;
	geometries = new GeometryEngine;
}

#include "utility/Utility.hpp"
//! [3]
void GLStressWidget::initShaders()
{
	qDebug()<<"Initializing shaders";
	makeCurrent();
	// Compile vertex shader
	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/3d/shaders/stress_test.vert.glsl")) {
		qWarning()<<"ERROR: Could not load vert shader";
		//close();
	}

	// Compile fragment shader
	else if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/3d/shaders/stress_test.frag.glsl")) {
		qWarning()<<"ERROR: Could not load frag shader";
		//close();
	}


	// Link shader pipeline
	else if (!program.link()) {
		qWarning()<<"ERROR: Could not link shaders";
		//close();
	}

	// Bind shader pipeline for use
	else if (!program.bind()) {
		qWarning()<<"ERROR: Could not bind shaders";
		//close();
	}
}

void GLStressWidget::initTextures(bool f)
{
	qDebug()<<"Initializing textures: "<<f;
	makeCurrent();
	if(nullptr!=texture) {
		texture->destroy();
		delete texture;
	}
	texture = new QOpenGLTexture(QImage(":/3d/textures/stress_test_cube_"+QString(f?"green":"red")+".svg"));
	GLSPEWERROR;
	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	GLSPEWERROR;
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	GLSPEWERROR;
	texture->setWrapMode(QOpenGLTexture::Repeat);
	GLSPEWERROR;
}

void GLStressWidget::resizeGL(int w, int h)
{
	makeCurrent();
	qDebug()<<"Resize : "<<w << "x"<<h;
	qreal aspect = qreal(w) / qreal(h ? h : 1);
	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
	projection.setToIdentity();
	projection.perspective(fov, aspect, zNear, zFar);
}


void GLStressWidget::paintGL()
{
	GLSPEWERROR;
	makeCurrent();
	GLSPEWERROR;
	//qDebug()<<"PaintGL";
	for(int i=0; i<NUM_BOX; ++i) {
		boxes[i]->update();
		GLSPEWERROR;
	}
	GLSPEWERROR;
	glClear(GL_COLOR_BUFFER_BIT);
	GLSPEWERROR;
	glClear(GL_DEPTH_BUFFER_BIT);
	GLSPEWERROR;
	texture->bind();
	GLSPEWERROR;
	for(int i=0; i<NUM_BOX; ++i) {
		QMatrix4x4 matrix;
		matrix.translate(boxes[i]->pos.x(),boxes[i]->pos.y(), -5);
		matrix.rotate(boxes[i]->rotation);
		program.setUniformValue("mvp_matrix", projection * matrix);
		GLSPEWERROR;
		program.setUniformValue("texture", 0);
		GLSPEWERROR;
		geometries->drawCubeGeometry(&program);
	}

	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	const qint64 iv=now -lastRenderRime;
	lastRenderRime=now;
	//Strive for 25 fps
	const qint64 pause= (1000/25)-iv;
	//Pause needed?
	if(pause>0) {
		QTimer::singleShot(pause,this,SLOT(update()));
	} else {
		update();
	}

}

