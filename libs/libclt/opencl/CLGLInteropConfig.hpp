#ifndef CLGLINTEROPCONFIG_HPP
#define CLGLINTEROPCONFIG_HPP

#include "../libutil/utility/IncludeOpenGL.hpp"

class CLGLInteropConfig
{
private:
	bool mDoGLInterop;
	GLuint mWidth;
	GLuint mHeight;
public:
	explicit CLGLInteropConfig(bool doGLInterop=false, GLuint width=0, GLuint height=0);
	explicit CLGLInteropConfig(bool doGLInterop, QSize size);

public:

	bool doGLInterop() const;
	GLuint width() const;
	GLuint height() const;
};

#endif // CLGLINTEROPCONFIG_HPP
