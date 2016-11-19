#include "GLErrors.hpp"

#include "IncludeOpenGL.hpp"

struct token_string {
	GLuint token;
	const char *string;
};

static const struct token_string errors[] = {
	{ GL_NO_ERROR, "no error" }
	, { GL_INVALID_ENUM, "invalid enumerant" }
	, { GL_INVALID_VALUE, "invalid value" }
	, { GL_INVALID_OPERATION, "invalid operation" }
	, { GL_STACK_OVERFLOW, "stack overflow" }
	, { GL_STACK_UNDERFLOW, "stack underflow" }
	, { GL_OUT_OF_MEMORY, "out of memory" }
	, { GL_TABLE_TOO_LARGE, "table too large" }
#ifdef GL_EXT_framebuffer_object
	, { GL_INVALID_FRAMEBUFFER_OPERATION_EXT, "invalid framebuffer operation" }
#endif

	, { (GLuint)~0, NULL } /* end of list indicator */
};



const GLubyte*
gluErrorString(GLenum errorCode)
{
	int i;
	for (i = 0; errors[i].string; i++) {
		if (errors[i].token == errorCode) {
			return (const GLubyte *) errors[i].string;
		}
	}
	return (const GLubyte *) 0;
}



bool spewGLErrors(int line)
{
	int ct=0;
	GLenum err=glGetError();
	while (err != GL_NO_ERROR) {
		const char * s=(const char *)gluErrorString(err);
		qWarning()<<line<<"."<<ct<<" GL-ERROR: "<<QString::fromLatin1(s,strlen(s));
		ct++;
		err = glGetError();
	}
	qDebug()<<"GLSPEW: @"<<line<<" GAVE  "<<ct<<" ERRORS";
	return ct>0;
}
