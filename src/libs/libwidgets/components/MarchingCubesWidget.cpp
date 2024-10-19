#include "MarchingCubesWidget.hpp"


#include <QOpenGLFunctions>
#include <QTimer>
#include <cmath>


#include "MarchingData.hpp"


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
	const GLfloat fHalfWorldSize = 0.5f * fWorldSize;
	GLfloat fAspect;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h){
		fAspect = (GLfloat)h / (GLfloat)w;
		glOrtho(-fHalfWorldSize, fHalfWorldSize, -fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -10*fHalfWorldSize, 10*fHalfWorldSize);
	}
	else{
		fAspect = (GLfloat)w / (GLfloat)h; 
		glOrtho(-fHalfWorldSize*fAspect, fHalfWorldSize*fAspect, -fHalfWorldSize, fHalfWorldSize, -10*fHalfWorldSize, 10*fHalfWorldSize);
	}
	
	glMatrixMode(GL_MODELVIEW);
}

/*
void MarchingCubesWidget::resizeGL(int w, int h)
{
	const GLfloat fWorldSize = 1.f;  // Full size of the world
	const GLfloat fHalfWorldSize = 0.5f * fWorldSize;  // Half size for centering around 0
	GLfloat fAspect;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h){
		// Landscape: Adjust the height to keep the aspect ratio
		fAspect = (GLfloat)h / (GLfloat)w;
		glOrtho(-fWorldSize, fWorldSize, -fWorldSize * fAspect, fWorldSize * fAspect, -10.f * fWorldSize, 10.f * fWorldSize);
	}
	else{
		// Portrait: Adjust the width to keep the aspect ratio
		fAspect = (GLfloat)w / (GLfloat)h;
		glOrtho(-fWorldSize * fAspect, fWorldSize * fAspect, -fWorldSize, fWorldSize, -10.f * fWorldSize, 10.f * fWorldSize);
	}
	
	glMatrixMode(GL_MODELVIEW);
}
*/
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


GLfloat fGetOffset(GLfloat fValue1, GLfloat fValue2, GLfloat fValueDesired)
{
	GLdouble fDelta = fValue2 - fValue1;
	
	if(fDelta == 0.0)
	{
		return 0.5;
	}
	return (fValueDesired - fValue1)/fDelta;
}


void vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
{
	GLfloat fOldLength;
	GLfloat fScale;
	
	fOldLength = sqrtf( (rfVectorSource.fX * rfVectorSource.fX) +
					   (rfVectorSource.fY * rfVectorSource.fY) +
					   (rfVectorSource.fZ * rfVectorSource.fZ) );
	
	if(fOldLength == 0.0)
	{
		rfVectorResult.fX = rfVectorSource.fX;
		rfVectorResult.fY = rfVectorSource.fY;
		rfVectorResult.fZ = rfVectorSource.fZ;
	}
	else
	{
		fScale = 1.0/fOldLength;
		rfVectorResult.fX = rfVectorSource.fX*fScale;
		rfVectorResult.fY = rfVectorSource.fY*fScale;
		rfVectorResult.fZ = rfVectorSource.fZ*fScale;
	}
}


void MarchingCubesWidget::vGetNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ)
{
	static const float margin{0.01f};
	rfNormal.fX = mField->density({fX - margin, fY, fZ}) - mField->density({fX + margin, fY, fZ});
	rfNormal.fY = mField->density({fX, fY - margin, fZ}) - mField->density({fX, fY + margin, fZ});
	rfNormal.fZ = mField->density({fX, fY, fZ - margin}) - mField->density({fX, fY, fZ + margin});
	vNormalizeVector(rfNormal, rfNormal);
}


void vGetColor(GLvector &rfColor, GLvector &rfPosition, GLvector &rfNormal)
{
	GLfloat fX = rfNormal.fX;
	GLfloat fY = rfNormal.fY;
	GLfloat fZ = rfNormal.fZ;
	rfColor.fX = (fX > 0.0 ? fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0);
	rfColor.fY = (fY > 0.0 ? fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0);
	rfColor.fZ = (fZ > 0.0 ? fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0);
}


void MarchingCubesWidget::marchCube(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale)
{
	GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
	GLfloat fOffset;
	GLvector sColor;
	GLfloat afCubeValue[8];
	GLvector asEdgeVertex[12];
	GLvector asEdgeNorm[12];
	for(iVertex = 0; iVertex < 8; iVertex++){
		afCubeValue[iVertex] = mField->density({fX + a2fVertexOffset[iVertex][0]*fScale, fY + a2fVertexOffset[iVertex][1]*fScale, fZ + a2fVertexOffset[iVertex][2]*fScale});
	}
	iFlagIndex = 0;
	for(iVertexTest = 0; iVertexTest < 8; iVertexTest++){
		if(afCubeValue[iVertexTest] <= fTargetValue){
			iFlagIndex |= 1<<iVertexTest;
		}
	}
	iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];
	
	if(iEdgeFlags == 0) {
		return;
	}
	for(iEdge = 0; iEdge < 12; iEdge++){
		if(iEdgeFlags & (1<<iEdge)){
			fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);
			asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
			asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
			asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;
			vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
		}
	}
	for(iTriangle = 0; iTriangle < 5; iTriangle++)
	{
		if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0){
			break;
		}
		for(iCorner = 0; iCorner < 3; iCorner++){
			iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
			vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
			glColor3f(sColor.fX, sColor.fY, sColor.fZ);
			glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
			glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
		}
	}
}


void MarchingCubesWidget::marchingCubes()
{
	GLint iX, iY, iZ;
	const auto half = iDataSetSize / 2;
	for(iX = -half; iX < half; iX++){
		for(iY = -half; iY < half; iY++){
			for(iZ = -half; iZ < half; iZ++){
				marchCube(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize);
			}
		}
	}
}

