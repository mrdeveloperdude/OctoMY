#ifndef TESTCLGLVIEWRENDERER_HPP
#define TESTCLGLVIEWRENDERER_HPP

#include "test/Common.hpp"

#include "clt/CLGLViewRenderer.hpp"

class CLThreadManager;


class TestCLGLViewRenderer:public CLGLViewRenderer
{
private:
	CLThreadManager *mThreadManager;
	bool mDone;

public:
	explicit TestCLGLViewRenderer();
	virtual ~TestCLGLViewRenderer() Q_DECL_OVERRIDE ;
	void setManager(CLThreadManager *man);
	void initialize(GLContext &ctx);

	// CLGLViewRenderer Interface
public:
	void resize(QSize sz) Q_DECL_OVERRIDE;
	bool setRendering(bool running, bool block=false) Q_DECL_OVERRIDE;
	bool isRendering() const Q_DECL_OVERRIDE;
	void renderFrame() Q_DECL_OVERRIDE;
	GLuint pbo() Q_DECL_OVERRIDE;
	QString getRendererSpec() Q_DECL_OVERRIDE;

};

#endif
// TESTCLGLVIEWRENDERER_HPP
