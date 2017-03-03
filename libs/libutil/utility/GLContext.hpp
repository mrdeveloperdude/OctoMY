#ifndef GLCONTEXT_HPP
#define GLCONTEXT_HPP

#include <QString>

class QOpenGLContext;
class QSurface;
class QThread;

class GLContext
{
private:
	QOpenGLContext* mGLCtx;
	QSurface *mSurface;

public:
	GLContext(QOpenGLContext* ctx=nullptr, QSurface *surf=nullptr);
	GLContext(const GLContext &other);
	GLContext(GLContext &other);
	virtual ~GLContext();

public:

	void setSurface(QSurface &surf);
	QSurface *surface();
	QOpenGLContext *context();
	void currentize();
	void uncurrentize();

	void moveToThread(QThread &th);

	QString toString();

	QString operator()()
	{
		return toString();
	}
};




#endif //GLCONTEXT_HPP
