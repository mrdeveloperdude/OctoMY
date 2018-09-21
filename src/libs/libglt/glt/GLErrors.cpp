#include "GLErrors.hpp"

#include <QOpenGLDebugLogger>

#include <QOpenGLDebugMessage>
#include <QOpenGLContext>

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
#ifdef GL_STACK_OVERFLOW
	, { GL_STACK_OVERFLOW, "stack overflow" }
#endif
#ifdef GL_STACK_UNDERFLOW
	, { GL_STACK_UNDERFLOW, "stack underflow" }
#endif
	, { GL_OUT_OF_MEMORY, "out of memory" }
#ifdef GL_TABLE_TOO_LARGE
	, { GL_TABLE_TOO_LARGE, "table too large" }
#endif
#ifdef GL_EXT_framebuffer_object
	, { GL_INVALID_FRAMEBUFFER_OPERATION_EXT, "invalid framebuffer operation" }
#endif

	, { static_cast<GLuint>(~0), nullptr } /* end of list indicator */
};



static const GLubyte*
gluErrorString(GLenum errorCode)
{
	int i;
	for (i = 0; errors[i].string; i++) {
		if (errors[i].token == errorCode) {
			return reinterpret_cast<const GLubyte *> (errors[i].string);
		}
	}
	return nullptr;
}



bool spewGLErrors(QString file, int line, QString func)
{
	int ct=0;
	GLenum err=glGetError();
	while (err != GL_NO_ERROR) {
		const char * s= reinterpret_cast<const char *>(gluErrorString(err));
		qWarning().noquote().nospace()<<file<<"::"<<func<<":"<<line<<" ["<<ct<<"] GL-ERROR: "<<QString::fromLatin1(s,static_cast<int>(strlen(s)));
		ct++;
		err = glGetError();
	}
	//qDebug()<<"GLSPEW: @"<<line<<" GAVE  "<<ct<<" ERRORS";
	return ct>0;
}


bool enableGLLogger(QOpenGLContext *ctx)
{
	if(nullptr != ctx && ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
		qDebug()<<"OpenGL Debugging ENABLED";
		QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(ctx);
		logger->initialize();
		QObject::connect(logger, &QOpenGLDebugLogger::messageLogged, ctx, [=](const QOpenGLDebugMessage &message) {
			if(message.severity()<= QOpenGLDebugMessage::MediumSeverity) {
				qDebug().noquote().nospace()<<"OPENGL-ERROR: src="<<message.source()<<", msg="<<message.message()<<", type="<<message.type()<<"";
			}
		});
		logger->startLogging();
		return true;
	} else {
		qWarning()<<"OpenGL Debugging DISABLED";
	}
	return false;
}
