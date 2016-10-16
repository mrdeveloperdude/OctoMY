#ifndef HELLOGLCLVIEWRENDERER_HPP
#define HELLOGLCLVIEWRENDERER_HPP

#include "../libcl/widgets/CLGLViewRenderer.hpp"
#include "../libutil/utility/IncludeOpenGL.hpp"


#include <QtGlobal>


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

#endif // HELLOGLCLVIEWRENDERER_HPP
