#ifndef ANDROID

#include "MarchingCubesWidget.hpp"


#include <QOpenGLFunctions>
#include <QTimer>
#include <cmath>


MarchingCubesWidget::MarchingCubesWidget(QWidget *parent)
	: QOpenGLWidget(parent)
	, m_timer(new QTimer(this))
	, mField( new Birth())
{
	/*
	connect(m_timer, &QTimer::timeout, this, [=](){
		update();
	});
*/
	//m_timer->start(1000/60);
}


void MarchingCubesWidget::initializeGL()
{
	glClearColor(0.8666, 0.098, 0.098, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
	
	GLfloat afPropertiesAmbient[] = {0.50, 0.50, 0.50, 1.00};
	GLfloat afPropertiesDiffuse[] = {0.75, 0.75, 0.75, 1.00};
	GLfloat afPropertiesSpecular[] = {1.00, 1.00, 1.00, 1.00};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, afPropertiesAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, afPropertiesDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, afPropertiesSpecular);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	
	glEnable(GL_LIGHT0);
}


void MarchingCubesWidget::resizeGL(int w, int h)
{
	const GLfloat fWorldSize = 1.f;
	const GLfloat fWorldFar = 2.f;
	const GLfloat fHalfWorldSize = 0.5f * fWorldSize;
	GLfloat fAspect;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h){
		fAspect = (GLfloat)h / (GLfloat)w;
		glOrtho(-fHalfWorldSize, fHalfWorldSize, -fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -fWorldFar*fHalfWorldSize, fWorldFar*fHalfWorldSize);
	}
	else{
		fAspect = (GLfloat)w / (GLfloat)h; 
		glOrtho(-fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -fHalfWorldSize, fHalfWorldSize, -fWorldFar*fHalfWorldSize, fWorldFar*fHalfWorldSize);
	}
	
	glMatrixMode(GL_MODELVIEW);
}


void MarchingCubesWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	
	if (bMove){
			fTime +=0.0025;
	}
	if (bSpin)
	{
		//fPitch += 1.0;
		fYaw += 0.05;
	}
	mField->setState(bSpin, bMove);
	mField->update(fTime);
	//glTranslatef(0.0, 0.0, 0.5f);
	glRotatef(-fPitch, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 1.0, 0.0);
	glRotatef(fYaw, 0.0, 0.0, 1.0);
	
	glPushMatrix();
		//glTranslatef(-0.5, -0.5, -0.5);
		glTranslatef(0.0, 0.0, -0.5f);
		glBegin(GL_TRIANGLES);
		marchingCubes();
		glEnd();
	glPopMatrix();
	
	glPopMatrix();
	update();
}


void MarchingCubesWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()){
		case Qt::Key_W:{
			ePolygonMode = (ePolygonMode == GL_LINE)?GL_FILL:GL_LINE;
			qDebug()<<"POLYGON MODE"<<(ePolygonMode==GL_LINE?"LINE":"FILL");
			glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
		} break;
			
		case Qt::Key_Plus:
		case Qt::Key_Equal:{
			++iDataSetSize;
			fStepSize = 1.0 / iDataSetSize;
			qDebug()<<"iDataSetSize"<<iDataSetSize<<"fStepSize"<<fStepSize;
		} break;
			
		case Qt::Key_Minus:{
			if (iDataSetSize > 1){
				--iDataSetSize;
				fStepSize = 1.0 / iDataSetSize;
			}
			qDebug()<<"iDataSetSize"<<iDataSetSize<<"fStepSize"<<fStepSize;
		} break;
		case Qt::Key_S:
			bSpin=!bSpin;
			qDebug()<<"bSpin"<<bSpin;
			break;
		case Qt::Key_L:
			bLight=!bLight;
			qDebug()<<"bLight"<<bLight;
			if(bLight){
				glEnable(GL_LIGHTING);//use lit material color
			}
			else{
				glDisable(GL_LIGHTING);//use vertex colors
			}
			break;
		case Qt::Key_M:
			bMove=!bMove;
			qDebug()<<"bMove"<<bMove;
			break;
			
		default:
			QOpenGLWidget::keyPressEvent(event);
	}
}
#endif // ANDROID
