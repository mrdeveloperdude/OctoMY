#ifndef CLGLVIEWRENDERER_HPP
#define CLGLVIEWRENDERER_HPP

#include "../libutil/utility/IncludeOpenGL.hpp"
#include "../libutil/utility/GLContext.hpp"


/**
 * @brief The CLGLViewRenderer class is an interface for allowing GLCL enabled
 * rendering to be shown in GLCLView.
 *
 */

class CLGLViewRenderer
{
public:

	explicit CLGLViewRenderer();


	virtual ~CLGLViewRenderer();


	// CLGLViewRenderer Interface
public:

	// Called by CLGLView to set the new size of openGL viewport
	virtual void resize(QSize sz)=0;
	// Called by CLGLView to set wether or not we are in a rendering loop and
	// can expect renderFrame to be called
	virtual bool setRendering(bool running, bool block=false)=0;
	// Called by CLGLView and others to probe wether or not we are in a rendering loop
	virtual bool isRendering() const =0;
	// Called by CLGLView at every frame before content of PBO are displayed
	virtual void renderFrame() =0;
	// Called by CLGLView at every frame to get PBO
	virtual GLuint pbo() =0;
	// Called by CLGLView and others to get the name of this rendrer
	virtual QString getRendererSpec() =0;

};

#endif // CLGLVIEWRENDERER_HPP
