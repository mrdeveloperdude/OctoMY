#include "CompasWidget.hpp"

#include "uptime/MethodGate.hpp"

#include "uptime/New.hpp"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

CompasWidget::CompasWidget(QWidget *parent)
	: QOpenGLWidget(parent)
	, m_xRot(0)
	, m_yRot(0)
	, m_zRot(0)
	, m_program(nullptr)
	  //
	, m_offscreenSurface(nullptr)
	, m_program2(nullptr)
	, m_fbo2(nullptr)
	, m_vbo2(nullptr)
	, m_vao2(nullptr)
	, m_matrixLoc(0)
{
	OC_METHODGATE();
	m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
	// --transparent causes the clear color to be transparent. Therefore, on systems that
	// support it, the widget will become transparent apart from the logo.
	m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
	if (m_transparent) {
		setAttribute(Qt::WA_TranslucentBackground);
	}
}

CompasWidget::~CompasWidget()
{
	OC_METHODGATE();
	cleanup();
	delete m_offscreenSurface;
	m_offscreenSurface=(nullptr);
	delete m_program;
	m_program=(nullptr);
	delete m_program2;
	m_program2=(nullptr);
	delete m_fbo2;
	m_fbo2=(nullptr);
	delete m_vbo2;
	m_vbo2=(nullptr);
	delete m_vao2;
	m_vao2=(nullptr);
}

QSize CompasWidget::minimumSizeHint() const
{
	OC_METHODGATE();
	return QSize(50, 50);
}

QSize CompasWidget::sizeHint() const
{
	OC_METHODGATE();
	return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
	OC_FUNCTIONGATE();
	while (angle < 0) {
		angle += 360 * 16;
	}
	while (angle > 360 * 16) {
		angle -= 360 * 16;
	}
}

void CompasWidget::setXRotation(int angle)
{
	OC_METHODGATE();
	qNormalizeAngle(angle);
	if (angle != m_xRot) {
		m_xRot = angle;
		emit xRotationChanged(angle);
		update();
	}
}

void CompasWidget::setYRotation(int angle)
{
	OC_METHODGATE();
	qNormalizeAngle(angle);
	if (angle != m_yRot) {
		m_yRot = angle;
		emit yRotationChanged(angle);
		update();
	}
}

void CompasWidget::setZRotation(int angle)
{
	OC_METHODGATE();
	qNormalizeAngle(angle);
	if (angle != m_zRot) {
		m_zRot = angle;
		emit zRotationChanged(angle);
		update();
	}
}

void CompasWidget::cleanup()
{
	OC_METHODGATE();
	makeCurrent();
	m_logoVbo.destroy();
	delete m_program;
	m_program = nullptr;
	doneCurrent();
}

static const char *vertexShaderSourceCore =
	"#version 150\n"
	"in vec4 vertex;\n"
	"in vec3 normal;\n"
	"out vec3 vert;\n"
	"out vec3 vertNormal;\n"
	"uniform mat4 projMatrix;\n"
	"uniform mat4 mvMatrix;\n"
	"uniform mat3 normalMatrix;\n"
	"void main() {\n"
	"   vert = vertex.xyz;\n"
	"   vertNormal = normalMatrix * normal;\n"
	"   gl_Position = projMatrix * mvMatrix * vertex;\n"
	"}\n";

static const char *fragmentShaderSourceCore =
	"#version 150\n"
	"in highp vec3 vert;\n"
	"in highp vec3 vertNormal;\n"
	"out highp vec4 fragColor;\n"
	"uniform highp vec3 lightPos;\n"
	"void main() {\n"
	"   highp vec3 L = normalize(lightPos - vert);\n"
	"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
	"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
	"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
	"   fragColor = vec4(col, 1.0);\n"
	"}\n";

static const char *vertexShaderSource =
	"attribute vec4 vertex;\n"
	"attribute vec3 normal;\n"
	"varying vec3 vert;\n"
	"varying vec3 vertNormal;\n"
	"uniform mat4 projMatrix;\n"
	"uniform mat4 mvMatrix;\n"
	"uniform mat3 normalMatrix;\n"
	"void main() {\n"
	"   vert = vertex.xyz;\n"
	"   vertNormal = normalMatrix * normal;\n"
	"   gl_Position = projMatrix * mvMatrix * vertex;\n"
	"}\n";

static const char *fragmentShaderSource =
	"varying highp vec3 vert;\n"
	"varying highp vec3 vertNormal;\n"
	"uniform highp vec3 lightPos;\n"
	"void main() {\n"
	"   highp vec3 L = normalize(lightPos - vert);\n"
	"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
	"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
	"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
	"   gl_FragColor = vec4(col, 1.0);\n"
	"}\n";


static const char *vertexShaderVideoStreamSource =
	"attribute highp vec4 vertex;\n"
	"attribute lowp vec2 coord;\n"
	"varying lowp vec2 v_coord;\n"
	"uniform highp mat4 matrix;\n"
	"void main() {\n"
	"   v_coord = coord;\n"
	"   gl_Position = matrix * vertex;\n"
	"}\n";
static const char *fragmentShaderVideoStreamSource =
	"varying lowp vec2 v_coord;\n"
	"uniform sampler2D sampler;\n"
	"void main() {\n"
	"   gl_FragColor = vec4(texture2D(sampler, v_coord).rgb, 1.0);\n"
	"}\n";

void CompasWidget::initializeGL()
{
	OC_METHODGATE();
	// In this example the widget's corresponding top-level window can change
	// several times during the widget's lifetime. Whenever this happens, the
	// QOpenGLWidget's associated context is destroyed and a new one is created.
	// Therefore we have to be prepared to clean up the resources on the
	// aboutToBeDestroyed() signal, instead of the destructor. The emission of
	// the signal will be followed by an invocation of initializeGL() where we
	// can recreate all resources.
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &CompasWidget::cleanup);

	initializeOpenGLFunctions();
	glClearColor(0, 0, 0, m_transparent ? 0 : 1);

	m_program = OC_NEW QOpenGLShaderProgram;
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
	m_program->bindAttributeLocation("vertex", 0);
	m_program->bindAttributeLocation("normal", 1);
	m_program->link();

	m_program->bind();
	m_projMatrixLoc = m_program->uniformLocation("projMatrix");
	m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
	m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
	m_lightPosLoc = m_program->uniformLocation("lightPos");

	// Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
	// implementations this is optional and support may not be present
	// at all. Nonetheless the below code works in all cases and makes
	// sure there is a VAO when one is needed.
	m_vao.create();
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

	// Setup our vertex buffer object.
	m_logoVbo.create();
	m_logoVbo.bind();
	m_logoVbo.allocate(m_logo.constData(), static_cast<int>( static_cast<unsigned int>(m_logo.count()) * sizeof(GLfloat)));

	// Store the vertex attribute bindings for the program.
	setupVertexAttribs();

	// Our camera never changes in this example.
	m_camera.setToIdentity();
	m_camera.translate(0, 0, -1);

	// Light position is fixed.
	m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

	m_program->release();



	QSize sz=size();
	m_fbo2 = OC_NEW QOpenGLFramebufferObject(sz, QOpenGLFramebufferObject::CombinedDepthStencil);



	m_program2 = OC_NEW QOpenGLShaderProgram;
	m_program2->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderVideoStreamSource);
	m_program2->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderVideoStreamSource);
	m_program2->bindAttributeLocation("vertex", 0);
	m_program2->bindAttributeLocation("coord", 1);
	m_program2->link();
	m_matrixLoc = m_program->uniformLocation("matrix");

	m_vao2 = OC_NEW QOpenGLVertexArrayObject;
	m_vao2->create();
	m_vao2->bind();

	m_vbo2 = OC_NEW QOpenGLBuffer;
	m_vbo2->create();
	m_vbo2->bind();

	GLfloat v[] = {
		-0.5, 0.5, 0.5, 0.5,-0.5,0.5,-0.5,-0.5,0.5,
		0.5, -0.5, 0.5, -0.5,0.5,0.5,0.5,0.5,0.5,
		-0.5, -0.5, -0.5, 0.5,-0.5,-0.5,-0.5,0.5,-0.5,
		0.5, 0.5, -0.5, -0.5,0.5,-0.5,0.5,-0.5,-0.5,

		0.5, -0.5, -0.5, 0.5,-0.5,0.5,0.5,0.5,-0.5,
		0.5, 0.5, 0.5, 0.5,0.5,-0.5,0.5,-0.5,0.5,
		-0.5, 0.5, -0.5, -0.5,-0.5,0.5,-0.5,-0.5,-0.5,
		-0.5, -0.5, 0.5, -0.5,0.5,-0.5,-0.5,0.5,0.5,

		0.5, 0.5,  -0.5, -0.5, 0.5,  0.5,  -0.5,  0.5,  -0.5,
		-0.5,  0.5,  0.5,  0.5,  0.5,  -0.5, 0.5, 0.5,  0.5,
		-0.5,  -0.5, -0.5, -0.5, -0.5, 0.5,  0.5, -0.5, -0.5,
		0.5, -0.5, 0.5,  0.5,  -0.5, -0.5, -0.5,  -0.5, 0.5
	};
	GLfloat texCoords[] = {
		0.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,
		1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,
		1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,
		0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,

		1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,
		0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,
		0.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,
		1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,

		0.0f,1.0f, 1.0f,0.0f, 1.0f,1.0f,
		1.0f,0.0f, 0.0f,1.0f, 0.0f,0.0f,
		1.0f,0.0f, 1.0f,1.0f, 0.0f,0.0f,
		0.0f,1.0f, 0.0f,0.0f, 1.0f,1.0f
	};

	const int vertexCount = 36;
	m_vbo2->allocate(sizeof(GLfloat) * vertexCount * 5);
	m_vbo2->write(0, v, sizeof(GLfloat) * vertexCount * 3);
	m_vbo2->write(sizeof(GLfloat) * vertexCount * 3, texCoords, sizeof(GLfloat) * vertexCount * 2);
	m_vbo2->release();

	if (m_vao2->isCreated()) {
		setupVertexAttribs2();
	}
	m_vao2->release();
}


void CompasWidget::setupVertexAttribs()
{
	OC_METHODGATE();
	m_logoVbo.bind();
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<const void *>(3 * sizeof(GLfloat)));
	m_logoVbo.release();
}


void CompasWidget::setupVertexAttribs2()
{
	OC_METHODGATE();
	m_vbo2->bind();
	m_program2->enableAttributeArray(0);
	m_program2->enableAttributeArray(1);
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>(36 * 3 * sizeof(GLfloat)));
	m_vbo2->release();
}


void CompasWidget::paintGL()
{
	OC_METHODGATE();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	m_world.setToIdentity();
	m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
	m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
	m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

	/*


	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
	m_program->bind();
	m_program->setUniformValue(m_projMatrixLoc, m_proj);
	m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
	QMatrix3x3 normalMatrix = m_world.normalMatrix();
	m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

	glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());

	m_program->release();
	*/

	QOpenGLFunctions *f = context()->functions();
	const bool OC_NEWFrameReady=true;
	if (OC_NEWFrameReady) {//new frame ready
		f->glFrontFace(GL_CW); // because our cube's vertex data is such

		f->glBindTexture(GL_TEXTURE_2D, m_fbo2->texture());

		m_program2->bind();
		QOpenGLVertexArrayObject::Binder vaoBinder(m_vao2);
		// If VAOs are not supported, set the vertex attributes every time.
		if (!m_vao2->isCreated()) {
			setupVertexAttribs2();
		}

		static GLfloat angle = 0;
		QMatrix4x4 m;
		m.translate(0, 0, -20);
		m.rotate(90, 0, 0, 1);
		m.rotate(angle, 0.5, 1, 0);
		angle += 0.5f;

		m_program2->setUniformValue(m_matrixLoc, m_proj * m);

		// Draw the cube.
		f->glDrawArrays(GL_TRIANGLES, 0, 36);

		m_program2->release();

	}


}

void CompasWidget::resizeGL(int w, int h)
{
	OC_METHODGATE();
	m_proj.setToIdentity();
	m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void CompasWidget::mousePressEvent(QMouseEvent *event)
{
	OC_METHODGATE();
	m_lastPos = event->pos();
}

void CompasWidget::mouseMoveEvent(QMouseEvent *event)
{
	OC_METHODGATE();
	auto pos=event->pos();
	int dx = pos.x() - m_lastPos.x();
	int dy = pos.y() - m_lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(m_xRot + 8 * dy);
		setYRotation(m_yRot + 8 * dx);
	} else if (event->buttons() & Qt::RightButton) {
		setXRotation(m_xRot + 8 * dy);
		setZRotation(m_zRot + 8 * dx);
	}
	m_lastPos = event->pos();
}
