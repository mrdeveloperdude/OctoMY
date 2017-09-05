#include "CLTool.hpp"

#include <QOpenGLContext>
#include <QGuiApplication>
#include <QDebug>

// NOTE: These two must come last in includes
#include <qpa/qplatformnativeinterface.h>
#include "glt/IncludeOpenGLIntegration.hpp"





CLTool::CLTool()
{

}



cl_context_properties *CLTool::generateProperties( const cl_platform_id platformID, QOpenGLContext *ctx )
{

	cl_context_properties *contextProps = OC_NEW cl_context_properties[8]{ CL_CONTEXT_PLATFORM, (cl_context_properties) platformID,
				   CL_GL_CONTEXT_KHR, 0,
				   0, 0,
				   0
				 };



	 QPlatformNativeInterface *nativeIf = qGuiApp->platformNativeInterface();
	 void *dpy = nativeIf->nativeResourceForIntegration(QByteArrayLiteral("egldisplay")); // EGLDisplay
	if (nullptr!=dpy) {
		void *nativeContext = nativeIf->nativeResourceForContext("eglcontext", ctx);
		if (!nativeContext) {
			qWarning("ERROR: Failed to get the underlying EGL context from the current QOpenGLContext");
		} else {
			qDebug()<<"! ! ! GOT NATIVE EGL CONTEXT";
		}
		contextProps[3] = (cl_context_properties) nativeContext;
		contextProps[4] = CL_EGL_DISPLAY_KHR;
		contextProps[5] = (cl_context_properties) dpy;
	} else {
		dpy = nativeIf->nativeResourceForIntegration(QByteArrayLiteral("display")); // Display *
		void *nativeContext = nativeIf->nativeResourceForContext("glxcontext", ctx);
		if (!nativeContext) {
			qWarning("ERROR: Failed to get the underlying GLX context from the current QOpenGLContext");
		} else {
			qDebug()<<"! ! ! GOT NATIVE GLX CONTEXT";
		}
		contextProps[3] = (cl_context_properties) nativeContext;
		contextProps[4] = CL_GLX_DISPLAY_KHR;
		contextProps[5] = (cl_context_properties) dpy;
	}




/*
	//Build context dynamically based on returned values for GLX context and display.
	//cl_context_properties contextProps[7]= {0};
	const GLXContext glxCtx=glXGetCurrentContext();
	const Display* glxDisp=glXGetCurrentDisplay();
	// Build properties list
	int i=0;
	if(0!=glxCtx) {
		qDebug()<<"CLWORKER! Added property: GLX enabled";
		contextProps[i++]=CL_GL_CONTEXT_KHR;
		contextProps[i++]= (cl_context_properties)(glxCtx);
	} else {
		qDebug()<<"CLWORKER! Added property: GLX disabled";
	}
	if(0!=glxDisp) {
		qDebug()<<"CLWORKER! Added property: GLX Display enabled";
		contextProps[i++]=CL_GLX_DISPLAY_KHR;
		contextProps[i++]= (cl_context_properties)(glxDisp);
	} else {
		qDebug()<<"CLWORKER! Added property: GLX Display disabled";
	}
	if(0!=platformID) {
		qDebug()<<"CLWORKER! Added property: Platform enabled with ID "<<platformID;
		contextProps[i++]=CL_CONTEXT_PLATFORM;
		contextProps[i++]=(cl_context_properties)(platformID);
	} else {
		qDebug()<<"CLWORKER! Added property: Platform disabled";
	}
	//Terminate with 0
	contextProps[i++]=0;
	qDebug()<<"CLWORKER! Put a total of "<<i <<" properties in the CL context";
*/
	return contextProps;


}
