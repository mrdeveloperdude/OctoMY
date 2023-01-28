#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include <QTest>
#include <QApplication>
#include <QDirIterator>

#define OC_TEST_RESOURCES_BASE ":/icons/"


#define QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
#define QTEST_ADD_GPU_BLACKLIST_SUPPORT



#define OC_TEST_ICON(testType, objectType) \
	Q_INIT_RESOURCE(test_resources); \
	QDirIterator it(":", QDirIterator::Subdirectories); \
	qDebug()<<"RESOURCES: "; \
	while (it.hasNext()) { \
		qDebug()<<" + " << it.next(); \
	} \
	QIcon icon; \
	icon.addFile(QStringLiteral(OC_TEST_RESOURCES_BASE #testType ".svg"), QSize(), QIcon::Normal, QIcon::Off); \
	app.setWindowIcon(icon); \



#define OC_TEST_END(testType, objectType) \
	objectType ob; \
	QTEST_SET_MAIN_SOURCE_PATH \
	const auto ret=QTest::qExec(&ob, argc, argv);	\
	qDebug()<<"Stopping " #testType " for " #objectType; \
	return ret; \

#ifdef OC_USE_LIB_EXT_OPENCL

#include "glt/IncludeOpenGL.hpp"

// this is the equivalent of the TEST_MAIN macro supplied by QTest except that it supports OpenGL

#define OC_TEST_MAIN_OGL(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
	//QApplication::setAttribute(Qt::OCTOMY_QT_OGL_APP_ATTRIBUTE); \
	//QApplication::setAttribute(Qt::AA_ShareOpenGLContexts); \
	QSurfaceFormat defFormat=QSurfaceFormat::defaultFormat(); \
	QSurfaceFormat format = defFormat; \
	format.setVersion( OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR ); \
	format.setProfile( QSurfaceFormat::OCTOMY_QT_OGL_SURFACE_PROFILE ); \
	format.setOption(QSurfaceFormat::DebugContext); \
	format.setDepthBufferSize(OCTOMY_QT_OGL_DEPTH_BUFFER); \
	format.setStencilBufferSize(OCTOMY_QT_OGL_STENSIL_BUFFER); \
	format.setSwapBehavior(QSurfaceFormat::OCTOMY_QT_OGL_SWAP_BEHAVIOUR); \
	format.setSwapInterval(OCTOMY_QT_OGL_SWAP_INTERVAL); \
	format.setRenderableType(QSurfaceFormat::OCTOMY_QT_OGL_RENDERABLE_TYPE); \
	QSurfaceFormat::setDefaultFormat(format); \
	qDebug() << " ### Default surface was changed from: ";  \
	qDebug() << defFormat<<", renderableType=" << defFormat.renderableType(); \
	qDebug() << " ###                               to: "; \
	qDebug() << format<<", renderableType=" << format.renderableType(); \
	QApplication app(argc, argv); \
	qDebug()<<"Starting " #testType " for " #objectType; \
	app.setAttribute(Qt::AA_Use96Dpi, true); \
	OC_TEST_ICON(testType, objectType) \
	QTEST_ADD_GPU_BLACKLIST_SUPPORT \
	QTEST_DISABLE_KEYPAD_NAVIGATION \
	OC_TEST_END(testType, objectType) \
}



#else
// OC_USE_LIB_EXT_OPENCL


#define OC_TEST_MAIN_OGL(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
	qDebug() << " ### OC_TEST_MAIN_OGL disabled beacuse no OC_USE_LIB_EXT_OPENCL, aborting...";  \
	return 0; \
}



#endif
// OC_USE_LIB_EXT_OPENCL


// this is the equivalent of the QTEST_MAIN macro supplied by QTest
#define OC_TEST_MAIN(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
	QApplication::setAttribute(Qt::AA_Use96Dpi, true); \
	QApplication app(argc, argv); \
	qDebug()<<"Starting " #testType " for " #objectType; \
	OC_TEST_ICON(testType, objectType) \
	QTEST_DISABLE_KEYPAD_NAVIGATION \
	QTEST_ADD_GPU_BLACKLIST_SUPPORT \
	OC_TEST_END(testType, objectType) \
}


// this is the equivalent of the QTEST_GUILESS_MAIN macro supplied by QTest
#define OC_TEST_MAIN_GUILESS(testType, objectType) \
int main(int argc, char *argv[]) \
{ \
	QApplication::setAttribute(Qt::AA_Use96Dpi, true); \
	QCoreApplication app(argc, argv); \
	qDebug()<<"Starting " #testType " for " #objectType; \
	OC_TEST_ICON(testType, objectType) \
	OC_TEST_END(testType, objectType) \
}





#endif
// TESTCOMMON_HPP
