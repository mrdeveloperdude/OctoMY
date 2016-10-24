#include "CLGLView.hpp"

#ifndef USE_OPENCL

CLGLView::CLGLView(QWidget *parent)
	: QOpenGLWidget(parent)
{

}

CLGLView::~CLGLView()
{
}

//QOpenGLWidget interface

void CLGLView::initializeGL()
{

}
void CLGLView::resizeGL(int w, int h)
{

}
void CLGLView::paintGL()
{

}


#else

#include "../libutil/utility/Utility.hpp"

#include "../../libcl/widgets/CLGLViewRenderer.hpp"

#include <QSurfaceFormat>
#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QImage>
#include <QTimer>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1



CLGLView::CLGLView(QWidget *parent)
	: QOpenGLWidget(parent)
	, logger(nullptr)
	, canvasVBO(nullptr)
	, canvasTexture(nullptr)
	, canvasProgram(nullptr)
	, canvasVAO(nullptr)
	, mGLCTX(nullptr, nullptr)
//	, engineGLContext(nullptr)
	, fps("display")
	, renderEnabled(false)
	, displayEnabled(false)
{
	connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	timer.setInterval(1000/25);
}



CLGLView::~CLGLView()
{
	qDebug()<<"Dtor for glveiw start";
	if(nullptr!=canvasVAO) {
		qDebug()<<" + canvas VAO delete";
		canvasVAO->deleteLater();
		canvasVAO=nullptr;
	}
	if(nullptr!=canvasProgram) {
		qDebug()<<" + canvas Program delete";
		canvasProgram->deleteLater();
		canvasProgram=nullptr;
	}
	if(nullptr!=canvasTexture) {
		qDebug()<<" + canvas Texture delete";
		canvasTexture->destroy();
		delete canvasTexture;
		canvasTexture=nullptr;
	}
	if(nullptr!=canvasVBO) {
		qDebug()<<" + canvas VBO delete";
		canvasVBO->destroy();
		delete canvasVBO;
		canvasVBO=nullptr;
	}
	qDebug()<<"Dtor for glveiw end";
}


void CLGLView::setRenderer(CLGLViewRenderer * renderer)
{
	this->renderer=renderer;
}

GLContext &CLGLView::glctx()
{
	return mGLCTX;
}

void CLGLView::initializeGL()
{
	qDebug()<<"%%%% INIT GL";
	makeCurrent();
	initializeOpenGLFunctions();
	QOpenGLContext *ctx=context();
	qDebug()<<"CTX: "<<ctx<<" is "<<(ctx->isValid()?"VALID":"INVALID");
	if(!ctx->isValid()) {
		exit(1);
	}
	initLogging();
	initCanvas();
	initCTX();
}

void CLGLView::resizeGL(int w, int h)
{
	qDebug()<<"%%%% RESIZE GL";
	makeCurrent();
	int side = qMin(w, h);
	glViewport((w - side) / 2, (h - side) / 2, side, side);

	if(nullptr!=renderer) {
		renderer->resize(QSize(w,h));
	}
}



void CLGLView::paintGL()
{
	fps.update();
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(timer.isActive()) {
		glClearColor(utility::frand(), utility::frand(), utility::frand(), 0.5f);
		if(nullptr!=renderer && renderer->isRunning()) {


			renderer->renderFrame();
			makeCurrent();

			GLuint pbo=renderer->pbo();
			if(0==pbo) {
				qDebug()<<"Invalid PBO";
			} else {
				//glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // default, works for BGRA8 and RGBA32F.
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
				canvasTexture->bind();
				glTexSubImage2D(canvasTexture->target(), 0, 0, 0, canvasTexture->width(), canvasTexture->height(), GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}
		}
	}
	if(nullptr!=canvasProgram) {
		canvasTexture->bind();
		canvasProgram->bind();
		canvasVAO->bind();
		canvasProgram->setUniformValue("matrix", canvasMatrix);
		glDrawArrays(GL_TRIANGLE_FAN, 0,4);
	}

}

QVector2D CLGLView::eventToNormalizedVector(QMouseEvent *event)
{
	QVector2D out;
	if(nullptr!=event) {
		QPointF p=event->localPos();
		out.setX(((p.x()/width())-0.5)*2.0);
		out.setY(((p.y()/height())-0.5)*2.0);
		//	qDebug()<<"Normal pos: "<<out;
	}
	return out;
}



void CLGLView::mousePressEvent(QMouseEvent *event)
{
	arcBall.pressed(true, eventToNormalizedVector(event));
	updateCamera();
	event->accept();
}

void CLGLView::mouseMoveEvent(QMouseEvent *event)
{
	arcBall.move(eventToNormalizedVector(event));
	updateCamera();
	if(arcBall.isDown()) {
		update();
	}
	event->accept();
}

void CLGLView::mouseReleaseEvent(QMouseEvent *event)
{
	arcBall.pressed(false, eventToNormalizedVector(event));
	updateCamera();
	event->accept();
}


void CLGLView::wheelEvent(QWheelEvent *event)
{
	QPoint numPixels = event->pixelDelta();
	QPoint numDegrees = event->angleDelta() / 8;

	QPointF delta;
	if (!numPixels.isNull()) {
		auto s=size();
		QPointF np(numPixels);
		auto f=QPointF(s.width()/np.x(),s.height()/np.y());
		delta=QPointF(f.x(),f.y());
	} else if (!numDegrees.isNull()) {
		delta=QPointF(numDegrees);
	}
	arcBall.zoom(delta.y());
	updateCamera();
	event->accept();
	update();
}

void CLGLView::initLogging()
{
	qDebug()<<"%%%% INIT LOG";
	QOpenGLContext *ctx=context();
	makeCurrent();
	logger = new QOpenGLDebugLogger(ctx);
	logger->initialize();
	connect(logger, &QOpenGLDebugLogger::messageLogged, [=](const QOpenGLDebugMessage &msg) {
		qWarning().noquote().nospace()<<" ## OGL-MSG: "<<msg.message();

	});
	logger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
}



void CLGLView::initCanvas()
{
	const GLfloat w=width();
	const GLfloat h=height();

	qDebug()<<"%%%% INIT CANVAS VBO";

	const GLfloat canvasScale=1.0;
	canvasMatrix.ortho(-canvasScale, +canvasScale, +canvasScale, -canvasScale, 5.0f, 15.0f);
	canvasMatrix.translate(0.0f, 0.0f, -10.0f);


	if(nullptr==canvasVBO) {
		makeCurrent();
		canvasVBO=new QOpenGLBuffer;
		if(!canvasVBO->create()) {
			qWarning()<<"ERROR Creating VBO";
			exit(1);
		}
		if(!canvasVBO->bind()) {
			qWarning()<<"ERROR Binding VBO";
			exit(1);
		}
		QVector<GLfloat> vertData;
		const GLfloat s=1.0f;
		// vertex position
		vertData.append(-s);
		vertData.append(-s);
		vertData.append(0.0f);
		// texture coordinate
		vertData.append(0.0f);
		vertData.append(0.0f);


		// vertex position
		vertData.append(+s);
		vertData.append(-s);
		vertData.append(0.0f);
		// texture coordinate
		vertData.append(w);
		vertData.append(0.0f);


		// vertex position
		vertData.append(+s);
		vertData.append(+s);
		vertData.append(0.0f);
		// texture coordinate
		vertData.append(w);
		vertData.append(h);


		// vertex position
		vertData.append(-s);
		vertData.append(+s);
		vertData.append(0.0f);
		// texture coordinate
		vertData.append(0.0f);
		vertData.append(h);

		//vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);

		canvasVBO->allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
		qDebug()<<"CREATED VBO WITH ID "<<canvasVBO->bufferId()<< " AND SIZE "<<w<<"x"<<h;
	}

	qDebug()<<"%%%% INIT CANVAS TEXTURE";
	if(nullptr==canvasTexture) {
		makeCurrent();
		canvasTexture=new QOpenGLTexture(QOpenGLTexture::TargetRectangle);

		canvasTexture->setFormat( QOpenGLTexture::RGBA8U);
		canvasTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
		canvasTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
		canvasTexture->setAutoMipMapGenerationEnabled(false);

		canvasTexture->setSize(w,h);
		if(!canvasTexture->create()) {
			qWarning()<<"ERROR Creating tex of size "<<w<<"x"<<h;
			exit(1);
		}
		qDebug()<<"CREATED TEX WITH ID "<<canvasTexture->textureId()<< " AND SIZE "<<w<<"x"<<h;

		canvasTexture->bind();

		QImage img(QSize(width(),height()), QImage::Format_RGBA8888);
		{
			img.fill(Qt::green);
			QPainter p(&img);
			p.setPen(Qt::NoPen);


			p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

			const qreal w=width();
			const qreal h=height();
			const qreal hi=qMax(w,h);
			const qreal lo=qMin(w,h);

			p.setBrush(Qt::gray);
			p.drawEllipse(QRect((w-hi)/2,(h-hi)/2,hi,hi));

			p.setBrush(Qt::red);
			p.drawEllipse(QRect(0,0,w,h));
			p.setBrush(Qt::blue);
			p.drawEllipse(QRect((w-lo)/2,(h-lo)/2,lo,lo));
		}
		canvasTexture->setData(img.mirrored(),QOpenGLTexture::DontGenerateMipMaps);


	}

	qDebug()<<"%%%% INIT CANVAS SHADER";
	Q_INIT_RESOURCE(gl_shaders);
	makeCurrent();
	if(nullptr==canvasProgram) {
		canvasProgram = new QOpenGLShaderProgram;
		if(!canvasProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl_shaders/basic.vert")) {
			qWarning()<<"ERROR: Could not add vertex shader from source: "<<canvasProgram->log();
			exit(1);
		}
		if(!canvasProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl_shaders/basic.frag")) {
			qWarning()<<"ERROR: Could not add fragment shader from source: "<<canvasProgram->log();
			exit(1);
		}
		canvasProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
		canvasProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
		if(!canvasProgram->link()) {
			qWarning()<<"Could not link program "<<canvasProgram->log();
			exit (1);
		}
		if(!canvasProgram->bind()) {
			qWarning()<<"Could not bind program "<<canvasProgram->log();
			exit (1);
		}
		//program->setUniformValue("texture", 0);
	}

	qDebug()<<"%%%% INIT CANVAS VAO";
	if(nullptr==canvasVAO) {
		makeCurrent();

		canvasVAO=new QOpenGLVertexArrayObject;
		canvasVAO->create();
		canvasVAO->bind();

		canvasTexture->bind();
		if(!canvasProgram->bind()) {
			qWarning()<<"Could not bind program "<<canvasProgram->log();
			exit (1);
		}
		if(!canvasVBO->bind()) {
			qWarning()<<"Could not bind vbo";
			exit (1);
		}

		canvasProgram->setUniformValue("texture", 0);
		canvasProgram->setUniformValue("matrix", canvasMatrix);
		canvasProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		canvasProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		canvasProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		canvasProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

		canvasVAO->release();
		canvasVBO->release();
		canvasProgram->release();
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void CLGLView::initCTX()
{
	makeCurrent();
	QOpenGLContext *ctx=context();

	QOpenGLContext *mglctx = new QOpenGLContext();

	mglctx->setFormat(ctx->format());
	mglctx->setShareContext(ctx);
	mglctx->create();

	qDebug()<<"MCTX: "<<mglctx<<" is "<<(mglctx->isValid()?"VALID":"INVALID");
	if(!mglctx->isValid()) {
		exit(1);
	}

	makeCurrent();
	QOffscreenSurface *osurf=new QOffscreenSurface(nullptr);
	if(nullptr!=osurf) {
		osurf->setFormat(ctx->surface()->format());
		osurf->create();
	} else {
		qWarning()<<"ERROR preparing offscreen surface for engine context";
		exit(1);
	}
	QSurface *surf=
		osurf;
	//ctx->surface();
	if(nullptr==surf) {
		qWarning()<<"ERROR getting surface for engine context";
		exit(1);
	} else {

	}
	doneCurrent();
	GLContext tglctx(mglctx, surf);
	mGLCTX=tglctx;

	//glctx.currentize();	glctx.uncurrentize();

}




void CLGLView::updateCamera()
{
	QMatrix4x4 relmat=arcBall.getRelativeMatrix();
	emit this->arcBallChange(relmat);
}


void CLGLView::onRenderToggle(bool v)
{
	renderEnabled=v;
	if(nullptr!=renderer) {
		renderer->setRunning(renderEnabled,false);
	} else {
		qWarning()<<"NO RENDERER WHILE TOGGLIGN RENDER";
	}
}

void CLGLView::onDisplayToggle(bool v)
{
	displayEnabled=v;
	if(v) {
		timer.start();
	} else {
		timer.stop();
	}
}
#endif
