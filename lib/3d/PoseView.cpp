#include "PoseView.hpp"

#include "scene/GeometryEngine.hpp"
#include "scene/Limb.hpp"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QOpenGLTexture>

#include <cmath>


PoseView::PoseView(QWidget *parent) :
	QOpenGLWidget(parent),
	geometries(0),
	texture(0),
	angularSpeed(0)
{

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);
	Limb *base=new Limb(sim,0,QVector3D(0,0,0),QVector3D(1,0,0),QVector3D(0,1,0),1,HINGE_STIFF, QVector3D(0,0,1),QVector3D(0,1,0));
	Limb *tibia=new Limb(sim,base,QVector3D(0,1,0),QVector3D(1,0,0),QVector3D(0,1,0),2,HINGE_REVOLUTE, QVector3D(0,0,1),QVector3D(0,1,0));
	Limb *femur=new Limb(sim,tibia,QVector3D(0,1,0),QVector3D(1,0,0),QVector3D(0,1,0),2,HINGE_REVOLUTE, QVector3D(0,0,1),QVector3D(0,1,0));

	sim.addLimb(base);

	sim.addLimb(tibia);
	sim.addLimb(femur);



}

PoseView::~PoseView()
{
	// Make sure the context is current when deleting the texture
	// and the buffers.
	makeCurrent();
	delete texture;
	delete geometries;
	doneCurrent();
}

void PoseView::mousePressEvent(QMouseEvent *e)
{
	// Save mouse press position
	mousePressPosition = QVector2D(e->localPos());
}

void PoseView::mouseReleaseEvent(QMouseEvent *e)
{
	// Mouse release position - mouse press position
	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

	// Rotation axis is perpendicular to the mouse position difference
	// vector
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// Accelerate angular speed relative to the length of the mouse sweep
	qreal acc = diff.length() / 100.0;

	// Calculate new rotation axis as weighted sum
	rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

	// Increase angular speed
	angularSpeed += acc;
}

void PoseView::timerEvent(QTimerEvent *)
{
	// Decrease angular speed (friction)
	angularSpeed *= 0.99;

	// Stop rotation when speed goes below threshold
	if (angularSpeed < 0.01) {
		angularSpeed = 0.0;
	} else {
		// Update rotation
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

		// Request an update
		update();
	}
}

void PoseView::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0, 0, 0, 1);

	initShaders();
	initTextures();

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Enable back face culling
	glEnable(GL_CULL_FACE);

	geometries = new GeometryEngine;

	// Use QBasicTimer because its faster than QTimer
	timer.start(1000/60, this);
}

void PoseView::initShaders()
{
	// Compile vertex shader
	qDebug()<<"----- adding framgent shader to program";
	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/3d/shaders/vshader.glsl")){
		close();
		return;
	}

	// Compile fragment shader
	qDebug()<<"----- adding vertex shader to program";
	if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/3d/shaders/fshader.glsl")){
		close();
		return;
	}

	// Link shader pipeline
	qDebug()<<"----- linking program";
	if (!program.link()){
		close();
		return;
	}

	// Bind shader pipeline for use
	qDebug()<<"----- binding program";
	if (!program.bind()){
		close();
		return;
	}
}

void PoseView::initTextures()
{
	// Load cube.png image
	texture = new QOpenGLTexture(QImage(":/3d/textures/grating_tiled.png"));

	// Set nearest filtering mode for texture minification
	texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::Repeat);
}

void PoseView::resizeGL(int w, int h)
{
	// Calculate aspect ratio
	qreal aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

	// Reset projection
	projection.setToIdentity();

	// Set perspective projection
	projection.perspective(fov, aspect, zNear, zFar);
}

void PoseView::paintGL()
{
	sim.update();
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture->bind();

	// Calculate model view transformation
	QMatrix4x4 matrix;
	matrix.translate(0.0, 0.0, -5.0);
	matrix.rotate(rotation);

	// Set modelview-projection matrix
	program.setUniformValue("mvp_matrix", projection * matrix);

	// Use texture unit 0 which contains cube.png
	program.setUniformValue("texture", 0);

	// Draw cube geometry
	geometries->drawCubeGeometry(&program);
}

