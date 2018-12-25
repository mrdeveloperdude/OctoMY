#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include <QTest>
#include <QApplication>


#define TEST_RESOURCES_BASE ":/resources/icons/"

#ifdef EXTERNAL_LIB_OPENCL

#include "glt/IncludeOpenGL.hpp"

// this is the equivalent of the TEST_MAIN macro supplied by QTest except that it supports OpenGL

#define OC_TEST_MAIN_OGL(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
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
	QApplication::setAttribute(Qt::OCTOMY_QT_OGL_APP_ATTRIBUTE); \
	QApplication::setAttribute(Qt::AA_ShareOpenGLContexts); \
	QApplication app(argc, argv); \
	qDebug()<<"Starting " #testType " for " #objectType; \
	app.setAttribute(Qt::AA_Use96Dpi, true); \
	QIcon icon; \
	icon.addFile(QStringLiteral(TEST_RESOURCES_BASE #testType ".svg"), QSize(), QIcon::Normal, QIcon::Off); \
	app.setWindowIcon(icon); \
	QTEST_ADD_GPU_BLACKLIST_SUPPORT \
	QTEST_DISABLE_KEYPAD_NAVIGATION \
	objectType ob; \
	QTEST_SET_MAIN_SOURCE_PATH \
	const auto ret=QTest::qExec(&ob, argc, argv);	\
	qDebug()<<"Stopping " #testType " for " #objectType; \
	return ret; \
}



#else
// EXTERNAL_LIB_OPENCL


#define OC_TEST_MAIN_OGL(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
	qDebug() << " ### OC_TEST_MAIN_OGL disabled beacuse no EXTERNAL_LIB_OPENCL, aborting...";  \
	return 0; \
}



#endif
// EXTERNAL_LIB_OPENCL


// this is the equivalent of the QTEST_MAIN macro supplied by QTest
#define OC_TEST_MAIN(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
	QApplication app(argc, argv); \
	qDebug()<<"Starting " #testType " for " #objectType; \
	app.setAttribute(Qt::AA_Use96Dpi, true); \
	QIcon icon; \
	icon.addFile(QStringLiteral(TEST_RESOURCES_BASE #testType ".svg"), QSize(), QIcon::Normal, QIcon::Off); \
	app.setWindowIcon(icon); \
	QTEST_DISABLE_KEYPAD_NAVIGATION \
	QTEST_ADD_GPU_BLACKLIST_SUPPORT \
	objectType ob; \
	QTEST_SET_MAIN_SOURCE_PATH \
	const auto ret=QTest::qExec(&ob, argc, argv);	\
	qDebug()<<"Stopping " #testType " for " #objectType; \
	return ret; \
}


// this is the equivalent of the QTEST_GUILESS_MAIN macro supplied by QTest
#define OC_TEST_MAIN_GUILESS(testType, objectType) \
int main(int argc, char *argv[]) \
{ \
	QCoreApplication app(argc, argv); \
	qDebug()<<"Starting " #testType " for " #objectType; \
	app.setAttribute(Qt::AA_Use96Dpi, true); \
	QIcon icon; \
	icon.addFile(QStringLiteral(TEST_RESOURCES_BASE #testType ".svg"), QSize(), QIcon::Normal, QIcon::Off); \
	app.setWindowIcon(icon); \
	objectType ob; \
	QTEST_SET_MAIN_SOURCE_PATH \
	const auto ret=QTest::qExec(&ob, argc, argv);	\
	qDebug()<<"Stopping " #testType " for " #objectType; \
	return ret; \
}





#endif
// TESTCOMMON_HPP
