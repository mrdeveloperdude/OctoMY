#ifndef CLGLINTEROPCONFIG_HPP
#define CLGLINTEROPCONFIG_HPP

#include "glt/IncludeOpenGL.hpp"
#include "glt/GLContext.hpp"

#include <QSize>

/**
 * @brief The CLGLInteropConfig class answers the simple questions
 * "will we in the context of this CLThreadManager instance do any
 * interop between OpenGL and OpenCL?"
 * In the case that the answer is "Yes", it also carries the size of the canvas
 * in which CL & GL will interchange data, as well as the GLContext object
 * used during sharing.
 *
 */
class CLGLInteropConfig
{
private:
	GLContext *mSharingContext;
	QSize mSize;

public:
	CLGLInteropConfig(GLContext *sc=nullptr, QSize sz=QSize());
	CLGLInteropConfig(CLGLInteropConfig &other);
	CLGLInteropConfig(const CLGLInteropConfig &other);

	virtual ~CLGLInteropConfig();

public:
	bool doGLInterop() const;
	GLuint width() const;
	GLuint height() const;
	QSize size() const;
	GLContext *sharingContext() const;
};

#endif
// CLGLINTEROPCONFIG_HPP
