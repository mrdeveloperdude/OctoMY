#ifndef HELLOGLCLVIEWRENDERER_HPP
#define HELLOGLCLVIEWRENDERER_HPP

#include "../libclt/widgets/CLGLViewRenderer.hpp"
#include "../libutil/utility/IncludeOpenGL.hpp"


#include <QtGlobal>

#ifndef EXTERNAL_LIB_OPENCL

class HelloGLCLViewRenderer : public CLGLViewRenderer
{
private:


public:
	explicit HelloGLCLViewRenderer() {}

	virtual ~HelloGLCLViewRenderer() {}


	// GLCLViewRenderer Interface
public:
	virtual void resize(QSize sz) Q_DECL_OVERRIDE;
	virtual bool setRendering(bool running, bool block=false) Q_DECL_OVERRIDE;
	virtual bool isRendering() const  Q_DECL_OVERRIDE
	{
		return false;
	}
	virtual void renderFrame()  Q_DECL_OVERRIDE;
	virtual GLuint pbo() Q_DECL_OVERRIDE;
	virtual QString getRendererSpec() Q_DECL_OVERRIDE;
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

#endif // HELLOGLCLVIEWRENDERER_HPP
