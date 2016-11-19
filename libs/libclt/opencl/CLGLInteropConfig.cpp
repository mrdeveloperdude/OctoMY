#include "CLGLInteropConfig.hpp"

CLGLInteropConfig::CLGLInteropConfig(bool doGLInterop, GLuint width, GLuint height)
	: mDoGLInterop(doGLInterop)
	, mWidth(width)
	, mHeight(height)
{

}


CLGLInteropConfig::CLGLInteropConfig(bool doGLInterop, QSize size)
	: CLGLInteropConfig(doGLInterop, size.width(), size.height())
{

}

bool CLGLInteropConfig::doGLInterop() const
{
	return mDoGLInterop;
}
GLuint CLGLInteropConfig::width() const
{
	return mWidth;
}
GLuint CLGLInteropConfig::height() const
{
	return mHeight;
}
