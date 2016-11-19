#ifndef GLCONTEXT_HPP
#define GLCONTEXT_HPP

#include <QString>

class QOpenGLContext;
class QSurface;
class QThread;

class GLContext
{
private:
	QOpenGLContext* glctx;
	QSurface *surf;

public:
	explicit GLContext(QOpenGLContext* glctx=nullptr, QSurface *surf=nullptr);
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
};




#endif //GLCONTEXT_HPP
