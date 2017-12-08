#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include <QTest>
#include <QApplication>

#include "glt/IncludeOpenGL.hpp"


#define OC_TEST_MAIN(testType, objectType) \
QT_BEGIN_NAMESPACE \
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
QT_END_NAMESPACE \
int main(int argc, char *argv[]) \
{ \
	Q_INIT_RESOURCE(icons); \
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
	icon.addFile(QStringLiteral(":/icons/" #testType ".svg"), QSize(), QIcon::Normal, QIcon::Off); \
	app.setWindowIcon(icon); \
	QTEST_ADD_GPU_BLACKLIST_SUPPORT \
	QTEST_DISABLE_KEYPAD_NAVIGATION \
	objectType ob; \
	QTEST_SET_MAIN_SOURCE_PATH \
	const auto ret=QTest::qExec(&ob, argc, argv);	\
	qDebug()<<"Stopping " #testType " for " #objectType; \
	return ret; \
}




#endif // TESTCOMMON_HPP
