#ifndef HELLOGLCLVIEWRENDERER_HPP
#define HELLOGLCLVIEWRENDERER_HPP

#include "../libcl/widgets/CLGLViewRenderer.hpp"
#include "../libutil/utility/IncludeOpenGL.hpp"


#include <QtGlobal>

#ifndef USE_OPENCL

class HelloGLCLViewRenderer : public CLGLViewRenderer
{
private:


public:
	explicit HelloGLCLViewRenderer();
	virtual ~HelloGLCLViewRenderer();


	// GLCLViewRenderer Interface
public:

	virtual void initialize(GLContext &ctx) Q_DECL_OVERRIDE{}
	virtual void resize(QSize sz) Q_DECL_OVERRIDE{}
	virtual bool setRunning(bool running, bool block=false) Q_DECL_OVERRIDE{return false;}
	virtual bool isRunning() const  Q_DECL_OVERRIDE{return false;}
	virtual void renderFrame()  Q_DECL_OVERRIDE{}
	virtual GLuint pbo() Q_DECL_OVERRIDE{return 0;}

};


#else

class CLThreadManager;

/*!
 * \brief The HelloGLCLViewRenderer class is a simple example OpenCL rendrer
 * for use with GLCLView.
 */

class HelloGLCLViewRenderer : public CLGLViewRenderer
{
private:

	bool mRunning;
	GLuint  mPBO;
	CLThreadManager *mThreadManager;

public:
	explicit HelloGLCLViewRenderer();
	virtual ~HelloGLCLViewRenderer();


	// GLCLViewRenderer Interface
public:

	virtual void initialize(GLContext &ctx) Q_DECL_OVERRIDE;
	virtual void resize(QSize sz) Q_DECL_OVERRIDE;
	virtual bool setRunning(bool running, bool block=false) Q_DECL_OVERRIDE;
	virtual bool isRunning() const  Q_DECL_OVERRIDE;
	virtual void renderFrame()  Q_DECL_OVERRIDE;
	virtual GLuint pbo() Q_DECL_OVERRIDE;

};

#endif

#endif // HELLOGLCLVIEWRENDERER_HPP
