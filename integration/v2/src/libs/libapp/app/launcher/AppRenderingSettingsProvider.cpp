#include "AppRenderingSettingsProvider.hpp"

#include "uptime/MethodGate.hpp"
#include "glt/IncludeOpenGL.hpp"

#include <QtGlobal>
#include <QApplication>


QSurfaceFormat AppRenderingSettingsProvider::surfaceFormat()
{
	OC_METHODGATE();
	QSurfaceFormat format=QSurfaceFormat::defaultFormat();
	format.setVersion( OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR );
	format.setProfile( QSurfaceFormat::OCTOMY_QT_OGL_SURFACE_PROFILE );
	format.setOption(QSurfaceFormat::DebugContext);
	format.setDepthBufferSize(OCTOMY_QT_OGL_DEPTH_BUFFER);
	format.setStencilBufferSize(OCTOMY_QT_OGL_STENSIL_BUFFER);
	format.setSwapBehavior(QSurfaceFormat::OCTOMY_QT_OGL_SWAP_BEHAVIOUR);
	format.setSwapInterval(OCTOMY_QT_OGL_SWAP_INTERVAL);
	format.setRenderableType(QSurfaceFormat::OCTOMY_QT_OGL_RENDERABLE_TYPE);
	return format;
}

QMap<Qt::ApplicationAttribute, bool> AppRenderingSettingsProvider::applicationAttributes()
{
	OC_METHODGATE();
	QMap<Qt::ApplicationAttribute, bool> out;
	out[Qt::OCTOMY_QT_OGL_APP_ATTRIBUTE]=true;
	out[Qt::AA_ShareOpenGLContexts]=true;
	return out;
}




void AppRenderingSettingsProvider::applyApplicationAttributes()
{
	OC_METHODGATE();
	QMap<Qt::ApplicationAttribute, bool> atr=applicationAttributes();
	for(QMap<Qt::ApplicationAttribute, bool>::const_iterator it = atr.constBegin(), end=atr.constEnd(); it!=end; ++it){
		QApplication::setAttribute(it.key(), it.value());
	}
}
