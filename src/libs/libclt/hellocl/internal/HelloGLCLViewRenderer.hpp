#ifndef HELLOGLCLVIEWRENDERER_HPP
#define HELLOGLCLVIEWRENDERER_HPP

#include "clt/CLGLViewRenderer.hpp"
#include "glt/IncludeOpenGL.hpp"

#include <QtGlobal>

#ifndef OC_USE_LIB_EXT_OPENCL

class HelloGLCLViewRenderer : public CLGLViewRenderer
{
private:

public:
	explicit HelloGLCLViewRenderer() {}

	virtual ~HelloGLCLViewRenderer() {}


	// GLCLViewRenderer Interface
public:
	void resize(QSize) Q_DECL_OVERRIDE{

	}


	bool setRendering(bool running, bool block=false) Q_DECL_OVERRIDE{
		return false;
	}


	virtual bool isRendering() const  Q_DECL_OVERRIDE
	{
		return false;
	}


	void renderFrame()  Q_DECL_OVERRIDE {

	}


	GLuint pbo() Q_DECL_OVERRIDE {
		return 0;
	}


	QString getRendererSpec() Q_DECL_OVERRIDE{
		return "NOT_IMPLEMENTED";
	}

};


#else

class CLThreadManager;

/*!
 * \brief The HelloGLCLViewRenderer class is a simple example OpenCL rendrer
 * for use with GLCLView.
 *
 * It workstogether with HelloCLWorkerFactory and HelloCLWorker
 */

class HelloGLCLViewRenderer : public CLGLViewRenderer
{
private:

	//bool mRunning;
	GLuint  mPBO;
	CLThreadManager *mThreadManager;

public:
	explicit HelloGLCLViewRenderer();
	virtual ~HelloGLCLViewRenderer();

	void initialize(GLContext &ctx);

	// GLCLViewRenderer Interface
public:
	void resize(QSize sz) Q_DECL_OVERRIDE;
	bool setRendering(bool running, bool block=false) Q_DECL_OVERRIDE;
	bool isRendering() const  Q_DECL_OVERRIDE;
	void renderFrame()  Q_DECL_OVERRIDE;
	GLuint pbo() Q_DECL_OVERRIDE;
	QString getRendererSpec() Q_DECL_OVERRIDE;

};

#endif
// OC_USE_LIB_EXT_OPENCL

#endif
// HELLOGLCLVIEWRENDERER_HPP

