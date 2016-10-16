#ifndef INCLUDEOPENGL_HPP
#define INCLUDEOPENGL_HPP


//This file is used to maintain DRY principle while using OpenGL in Qt

// This is the only place where you get to fiddle my friend!
#define OCTOMY_QT_OGL_VERSION_MAJOR 4
#define OCTOMY_QT_OGL_VERSION_MINOR 0
#define OCTOMY_QT_OGL_PROFILE Core


//Necessary hack. See this: http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
#define COMBINE6(A,B,C,D,E,F) A##B##C##D##E##F
#define OCTOMY_QT_OGL_FUNCTIONS_CLASS_TEMP(MAJ,MIN,PRO) COMBINE6(QOpenGLFunctions_,MAJ,_,MIN,_,PRO)

#define OCTOMY_QT_OGL_FUNCTIONS_CLASS OCTOMY_QT_OGL_FUNCTIONS_CLASS_TEMP(OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR, OCTOMY_QT_OGL_PROFILE)

#define STRINGIFY_TEMP(A) #A
#define STRINGIFY(B) STRINGIFY_TEMP(B)

#define COMBINE2(A,B) A##B
#define TEMP_PROFILE(A,B) COMBINE2(A, B)
#define OCTOMY_QT_OGL_SURFACE_PROFILE TEMP_PROFILE(OCTOMY_QT_OGL_PROFILE, Profile)

#include <QOpenGLWidget>

//This right here is a result:
#include STRINGIFY(OCTOMY_QT_OGL_FUNCTIONS_CLASS)

//#include <QOpenGLFunctions>
#include <QtOpenGLExtensions/QtOpenGLExtensions>


#endif //INCLUDEOPENGL_HPP
