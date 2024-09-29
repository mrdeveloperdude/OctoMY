#ifndef MARCHINGCUBESWIDGET_HPP
#define MARCHINGCUBESWIDGET_HPP


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
	GLint     iDataSetSize{16};
	GLfloat   fStepSize{1.0f/iDataSetSize};
	GLfloat   fTime{0.0f};
	GLfloat   fTargetValue{48.0f};
	GLboolean bSpin{true};
	GLboolean bMove{true};
	GLboolean bLight{true};
	GLvector sSourcePoint[3];
	QTimer *m_timer;
	Field *mField = new Birth();
	GLfloat fPitch{FULL_CIRCLE/8.0f};
	GLfloat fYaw{FULL_CIRCLE/4.0f};

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


#endif // MARCHINGCUBESWIDGET_HPP
