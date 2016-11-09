#ifndef CLGLVIEWRENDERER_HPP
#define CLGLVIEWRENDERER_HPP

#include "../libutil/utility/IncludeOpenGL.hpp"
#include "../libutil/utility/GLContext.hpp"

// Interface for GLCL enabled rendering to be shown in GLCLView
class CLGLViewRenderer
{
public:

	explicit CLGLViewRenderer();


	virtual ~CLGLViewRenderer();

public:

	virtual void initialize(GLContext &ctx)=0;
	virtual void resize(QSize sz)=0;
	virtual bool setRunning(bool running, bool block=false)=0;
	virtual bool isRunning() const =0;
	virtual void renderFrame() =0;
	virtual GLuint pbo() =0;

};

#endif // CLGLVIEWRENDERER_HPP
