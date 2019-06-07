#include "CLGLView.hpp"

#include <QtMath>

#ifndef OC_USE_LIB_EXT_OPENCL


CLGLView::CLGLView(QWidget *parent)
	: QOpenGLWidget(parent)
{
}


CLGLView::~CLGLView()
{
}

#else


#include "clt/CLGLViewRenderer.hpp"

#include "uptime/New.hpp"
#include "utility/random/Random.hpp"

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
	, mSharingGLContext(nullptr, nullptr)
//	, engineGLContext(nullptr)
	, fps("display")
	, renderEnabled(false)
	, displayEnabled(false)
	, mRenderer(nullptr)
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


void clglviewFail(QString message)
{
	qWarning().noquote().nospace()<<"CLGLVIEW-FAIL: "<<message;
}


void CLGLView::setRenderer(CLGLViewRenderer * renderer)
{
	const QString oldSpec=(nullptr!=mRenderer)?mRenderer->getRendererSpec():"NONE";
	const QString newSpec=(nullptr!=renderer)?renderer->getRendererSpec():"NONE";
	qDebug()<<"CHANGING ACTIVE RENDERER FROM "<<oldSpec<<" --> "<<newSpec;
	if(mRenderer!=renderer && nullptr!=renderer) {
		renderer->resize(mLastResize);
		renderer->setRendering(renderEnabled,false);
	}
	mRenderer=renderer;
}


GLContext &CLGLView::sharingGLContext()
{
	return mSharingGLContext;
}


void CLGLView::initializeGL()
{
	qDebug()<<"%%%% INIT GL";
	makeCurrent();
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	qDebug()<<"USING GL CONTEXT "<<ctx;
	if (nullptr==ctx) {
		qWarning("ERROR: Attempted CL-GL interop without a current OpenGL context");
		return;
	}
	if (!ctx->isValid()) {
		qWarning("ERROR: Attempted CL-GL interop without a valid GL context");
		return;
	}
	if (nullptr==ctx->surface()) {
		qWarning("ERROR: Attempted CL-GL interop without a context surface");
		return;
	}
	initializeOpenGLFunctions();

	initLogging();
	initCanvas();
	initCTX();

	emit glInitialized();
}


void CLGLView::resizeGL(int w, int h)
{
	mLastResize=QSize(w,h);
	qDebug()<<"%%%% RESIZE GL "<<mLastResize;
	makeCurrent();
	int side = qMin(w, h);
	glViewport((w - side) / 2, (h - side) / 2, side, side);

	if(nullptr!=mRenderer) {
		mRenderer->resize(mLastResize);
	}
}


void CLGLView::paintGL()
{
	fps.update();
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(timer.isActive()) {
		glClearColor(utility::random::frand(), utility::random::frand(), utility::random::frand(), 0.5f);
		if(nullptr!=mRenderer && mRenderer->isRendering()) {


			mRenderer->renderFrame();
			makeCurrent();

			GLuint pbo=mRenderer->pbo();
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
	logger = OC_NEW QOpenGLDebugLogger(ctx);
	logger->initialize();
	connect(logger, &QOpenGLDebugLogger::messageLogged, [=](const QOpenGLDebugMessage &msg) {
		qWarning().noquote().nospace()<<" ## OGL-MSG: "<<msg.message();

	});
	logger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
}


static void drawTestPattern(QImage &img, const QString &msg="")
{
	img.fill(Qt::green);
	QPainter p(&img);
	p.setPen(Qt::NoPen);


	p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

	const qreal w=img.width();
	const qreal h=img.height();
	const qreal hi=qMax(w,h);
	const qreal lo=qMin(w,h);

	p.setBrush(Qt::gray);
	p.drawEllipse(QRect((w-hi)/2,(h-hi)/2,hi,hi));

	p.setBrush(Qt::red);
	p.drawEllipse(QRect(0,0,w,h));
	p.setBrush(Qt::blue);
	p.drawEllipse(QRect((w-lo)/2,(h-lo)/2,lo,lo));

	const QPointF center((w * 30.0) / 100.0,(h*30.0)/100.0);
	const qreal radius=(qMin(w,h) * 10.0) / 100.0;
	const qreal is=(M_PI * 2.0) / 100.0;
	p.setBrush(Qt::white);
	p.setPen(Qt::NoPen);
	p.drawRect(QRectF(center-QPointF(radius,radius), QSizeF(radius*2,radius*2)));
	p.setBrush(Qt::NoBrush);
	QPen e;
	e.setWidthF(1.0);
	e.setColor(Qt::black);
	p.setPen(e);
	for(qreal i=0; i<M_PI * 2.0; i+=is) {
		p.drawLine(center,QPointF(center+QPointF(cos(i)*radius, sin(i)*radius)));
	}
	if(msg.length()>0) {
		QFont f=p.font();
		f.setBold(true);
		f.setPixelSize((w*30.0)/1000.0);
		p.setFont(f);
		QPen e;
		e.setWidthF(1.0);
		e.setColor(Qt::white);
		p.setPen(e);
		p.setBrush(Qt::black);
		p.drawText(img.rect(),Qt::AlignCenter,msg);
	}
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
		canvasVBO=OC_NEW QOpenGLBuffer;
		if(!canvasVBO->create()) {
			clglviewFail("ERROR Creating VBO");
		}
		if(!canvasVBO->bind()) {
			clglviewFail("ERROR Binding VBO");
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
		qDebug().noquote().nospace()<<"CREATED VBO WITH ID "<<canvasVBO->bufferId()<< " AND SIZE "<<w<<"x"<<h;
	}

	qDebug()<<"%%%% INIT CANVAS TEXTURE";
	if(nullptr==canvasTexture) {
		makeCurrent();
		canvasTexture=OC_NEW QOpenGLTexture(QOpenGLTexture::TargetRectangle);

		canvasTexture->setFormat( QOpenGLTexture::RGBA8U);
		canvasTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
		canvasTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
		canvasTexture->setAutoMipMapGenerationEnabled(false);

		canvasTexture->setSize(w,h);
		if(!canvasTexture->create()) {
			clglviewFail("ERROR Creating tex of size "+QString::number(w)+"x"+QString::number(h));
		}
		qDebug()<<"CREATED TEX WITH ID "<<canvasTexture->textureId()<< " AND SIZE "<<w<<"x"<<h;

		canvasTexture->bind();

		QImage img(QSize(width(),height()), QImage::Format_RGBA8888);
		drawTestPattern(img, "CLGLView dummy texture");
		canvasTexture->setData(img,QOpenGLTexture::DontGenerateMipMaps);


	}

	qDebug()<<"%%%% INIT CANVAS SHADER";
	Q_INIT_RESOURCE(gl_shaders);
	makeCurrent();
	if(nullptr==canvasProgram) {
		canvasProgram = OC_NEW QOpenGLShaderProgram;
		if(!canvasProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl_shaders/basic.vert")) {
			clglviewFail("ERROR: Could not add vertex shader from source: "+canvasProgram->log());
		}
		if(!canvasProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl_shaders/basic.frag")) {
			clglviewFail("ERROR: Could not add fragment shader from source: "+canvasProgram->log());
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

		canvasVAO=OC_NEW QOpenGLVertexArrayObject;
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


//TODO: read this https://software.intel.com/en-us/articles/opencl-and-opengl-interoperability-tutorial
//	  about setting up cl context for GL devcie etc. using clGetGLContextInfoKHR

void CLGLView::initCTX()
{
	qDebug()<<"%%%% INIT CTX";
	makeCurrent();
	QOpenGLContext *ctx=context();

	QOpenGLContext *tempCTX = OC_NEW QOpenGLContext();

	tempCTX->setFormat(ctx->format());
	tempCTX->setShareContext(ctx);
	tempCTX->create();

	qDebug()<<"MCTX: "<<tempCTX<<" is "<<(tempCTX->isValid()?"VALID":"INVALID");
	if(!tempCTX->isValid()) {
		clglviewFail("ERROR ctx not valid");
	}

	makeCurrent();
	QOffscreenSurface *osurf=OC_NEW QOffscreenSurface(nullptr);
	if(nullptr!=osurf) {
		osurf->setFormat(ctx->surface()->format());
		osurf->create();
	} else {
		clglviewFail("ERROR preparing offscreen surface for engine context");
	}
	QSurface *osurfp=osurf;
	//ctx->surface();
	if(nullptr==osurfp) {
		clglviewFail("ERROR getting surface for engine context");
	}
	doneCurrent();
	//GLContext tempSharingGLContext(tempCTX, osurfp);	mSharingGLContext=tempSharingGLContext;
	mSharingGLContext=GLContext(tempCTX, osurfp);

	// Have it be current briefly to let it settle.
	// TODO: Figure out if this is necessary or at all beneficial
	mSharingGLContext.setCurrent(true);
	mSharingGLContext.setCurrent(false);

}


void CLGLView::updateCamera()
{
	QMatrix4x4 relmat=arcBall.getRelativeMatrix();
	emit this->arcBallChange(relmat);
}


void CLGLView::onRenderToggle(bool v)
{
	renderEnabled=v;
	if(nullptr!=mRenderer) {
		mRenderer->setRendering(renderEnabled,false);
	} else {
		qWarning()<<"NO RENDERER WHILE TOGGELING RENDER";
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
// OC_USE_LIB_EXT_OPENCL
