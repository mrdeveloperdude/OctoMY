#include "Qt3DWindow.hpp"

#include <QKeyEvent>
#include <QGuiApplication>
#include <QOpenGLContext>

Qt3DWindow::Qt3DWindow(QScreen *screen):
	QWindow(screen)

{
	setSurfaceType(QSurface::OpenGLSurface);

	QSurfaceFormat format;
	if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
		format.setVersion(4, 3);
		format.setProfile(QSurfaceFormat::CoreProfile);
	}
	format.setDepthBufferSize( 24 );
	format.setSamples( 4 );
	setFormat(format);
	create();
}

Qt3DWindow::~Qt3DWindow()
{
}




///////////



#ifdef USE_QT3D
#include "SceneModifier.hpp"



#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>


#include "Qt3DWindow.hpp"

#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtGui/QScreen>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRenderer/qtorusmesh.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qsceneloader.h>

#include <Qt3DCore/qscaletransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qlookattransform.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qforwardrenderer.h>

#endif //USE_QT3D
QWidget *getQ3DWidget(){
	Qt3DWindow *window = new Qt3DWindow();
	QWidget *container = QWidget::createWindowContainer(window);
	Qt3D::QAspectEngine engine;

	engine.registerAspect(new Qt3D::QRenderAspect());
	/*
	Qt3D::QInputAspect *input = new Qt3D::QInputAspect;
	engine.registerAspect(input);
	*/
	engine.initialize();
	QVariantMap data;
	data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(window)));
	data.insert(QStringLiteral("eventSource"), QVariant::fromValue(window));
	engine.setData(data);

	// Root entity
	Qt3D::QEntity *rootEntity = new Qt3D::QEntity();
/*
	// Camera
	Qt3D::QCamera *cameraEntity = new Qt3D::QCamera(rootEntity);
	cameraEntity->setObjectName(QStringLiteral("cameraEntity"));


	input->setCamera(cameraEntity);

	// FrameGraph
	Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
	Qt3D::QForwardRenderer *forwardRenderer = new Qt3D::QForwardRenderer();

	forwardRenderer->setCamera(cameraEntity);
	forwardRenderer->setClearColor(QColor(QRgb(0x4d4d4f)));
	frameGraph->setActiveFrameGraph(forwardRenderer);

	// Setting the FrameGraph
	rootEntity->addComponent(frameGraph);
*/
	 //SceneModifier *modifier = new SceneModifier(rootEntity);

	// Set root object of the scene
	engine.setRootEntity(rootEntity);

	return container;
}






#ifdef USE_QT3D
	//According to the link below, native widget support for Qt3D will not be available until at least Qt5.8 or later
	//Untill that time comes we will keep back the qt3d enthusiasm and write generalized 3D code that is easy to port when the day comes
	//http://stackoverflow.com/questions/35074830/show-qt3d-stuff-inside-qwidget-in-qt5
	QWidget *w=getQ3DWidget();
	Q_ASSERT(0!=w);
	this->layout()->addWidget(w);
#endif
