#include "AppRenderingSettingsProvider.hpp"

#include "uptime/MethodGate.hpp"
#include "glt/IncludeOpenGL.hpp"


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

Qt::ApplicationAttribute AppRenderingSettingsProvider::applicationAttributes()
{
	OC_METHODGATE();
	return Qt::OCTOMY_QT_OGL_APP_ATTRIBUTE;
}
