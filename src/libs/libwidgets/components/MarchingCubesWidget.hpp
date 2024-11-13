#ifndef MARCHINGCUBESWIDGET_HPP
#define MARCHINGCUBESWIDGET_HPP

#ifndef ANDROID
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QTimer>

#include "MarchingFields.hpp"

const static float FULL_CIRCLE{360.0f};

class MarchingCubesWidget : public QOpenGLWidget
{
	Q_OBJECT
private:
	GLenum    ePolygonMode{GL_FILL};
	GLint     iDataSetSize{24};
	GLfloat   fStepSize{1.0f/iDataSetSize};
	GLfloat   fTime{0.0f};
	GLfloat   fTargetValue{0.5f};
	GLboolean bSpin{true};
	GLboolean bMove{true};
	GLboolean bLight{true};
	GLvector  sSourcePoint[3];
	QTimer   *m_timer;
	Field    *mField{nullptr};
	GLfloat   fPitch{FULL_CIRCLE/8.0f};
	GLfloat   fYaw{FULL_CIRCLE/4.0f};

public:
	explicit MarchingCubesWidget(QWidget *parent = nullptr);
	~MarchingCubesWidget() = default;

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void keyPressEvent(QKeyEvent *event) override;

private:
	inline void vGetNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ);
	inline void marchCube(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale);
	inline void marchingCubes();
};

#include "MarchingData.hpp"




inline GLfloat fGetOffset(GLfloat fValue1, GLfloat fValue2, GLfloat fValueDesired)
{
	GLdouble fDelta = fValue2 - fValue1;
	
	if(fDelta == 0.0)
	{
		return 0.5;
	}
	return (fValueDesired - fValue1)/fDelta;
}


inline void vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
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


inline void vGetColor(GLvector &rfColor, GLvector &rfPosition, GLvector &rfNormal)
{
	Q_UNUSED(rfPosition);
	GLfloat fX = rfNormal.fX;
	GLfloat fY = rfNormal.fY;
	GLfloat fZ = rfNormal.fZ;
	rfColor.fX = (fX > 0.0 ? fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0);
	rfColor.fY = (fY > 0.0 ? fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0);
	rfColor.fZ = (fZ > 0.0 ? fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0);
}


inline void MarchingCubesWidget::vGetNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ)
{
	static const float margin{0.01f};
	rfNormal.fX = mField->density({fX - margin, fY, fZ}) - mField->density({fX + margin, fY, fZ});
	rfNormal.fY = mField->density({fX, fY - margin, fZ}) - mField->density({fX, fY + margin, fZ});
	rfNormal.fZ = mField->density({fX, fY, fZ - margin}) - mField->density({fX, fY, fZ + margin});
	vNormalizeVector(rfNormal, rfNormal);
}




inline void MarchingCubesWidget::marchCube(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale)
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


inline void MarchingCubesWidget::marchingCubes()
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


#endif // ANDROID

#endif // MARCHINGCUBESWIDGET_HPP
