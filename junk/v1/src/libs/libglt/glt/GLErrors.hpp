#ifndef GLERRORS_HPP
#define GLERRORS_HPP

#include <QString>


class QOpenGLContext;

#define GLSPEWERROR spewGLErrors(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC)




extern bool spewGLErrors(QString file="", int line=0, QString func="");


extern bool enableGLLogger(QOpenGLContext *ctx);

#endif // GLERRORS_HPP
