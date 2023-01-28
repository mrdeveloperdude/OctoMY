#ifndef INCLUDEOPENGL_HPP
#define INCLUDEOPENGL_HPP


// This file is used to maintain DRY principle while using OpenGL in Qt
// This is done through macro MAGIC outright!

// Between these lines is the only place where you get to fiddle my friend!
////////////////////////////////////////////////////////////////////////////////
//
# define OCTOMY_QT_OGL_VERSION_MAJOR   2
# define OCTOMY_QT_OGL_VERSION_MINOR   0
# define OCTOMY_QT_OGL_PROFILE         Core
# define OCTOMY_QT_OGL_DEPTH_BUFFER    24
# define OCTOMY_QT_OGL_STENSIL_BUFFER  0
# define OCTOMY_QT_OGL_SWAP_INTERVAL	  1
# define OCTOMY_QT_OGL_SWAP_BEHAVIOUR  DefaultSwapBehavior
# define OCTOMY_QT_OGL_USE_DESKTOP     1
//
////////////////////////////////////////////////////////////////////////////////



#if OCTOMY_QT_OGL_USE_DESKTOP == 1
#define OCTOMY_QT_OGL_APP_ATTRIBUTE AA_UseDesktopOpenGL
#define OCTOMY_QT_OGL_RENDERABLE_TYPE OpenGL
#else
#define OCTOMY_QT_OGL_APP_ATTRIBUTE AA_UseOpenGLES
#define OCTOMY_QT_OGL_RENDERABLE_TYPE OpenGLES
#endif

// Necessary hack. See this: http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
#define COMBINE6(A,B,C,D,E,F) A##B##C##D##E##F
#define COMBINE4(A,B,C,D) A##B##C##D
#define OCTOMY_QT_OGL_FUNCTIONS_CLASS_TEMP(MAJ,MIN,PRO) COMBINE6(QOpenGLFunctions_,MAJ,_,MIN,_,PRO)
#define OCTOMY_QT_OGL_OLD_FUNCTIONS_CLASS_TEMP(MAJ,MIN) COMBINE4(QOpenGLFunctions_,MAJ,_,MIN)

#if OCTOMY_QT_OGL_VERSION_MAJOR > 2
#define OCTOMY_QT_OGL_FUNCTIONS_CLASS OCTOMY_QT_OGL_FUNCTIONS_CLASS_TEMP(OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR, OCTOMY_QT_OGL_PROFILE)
#else
#define OCTOMY_QT_OGL_FUNCTIONS_CLASS OCTOMY_QT_OGL_OLD_FUNCTIONS_CLASS_TEMP(OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR)
#endif

#define STRINGIFY_TEMP(A) #A
#define STRINGIFY(B) STRINGIFY_TEMP(B)

#define COMBINE2(A,B) A##B
#define TEMP_PROFILE(A,B) COMBINE2(A, B)
#define OCTOMY_QT_OGL_SURFACE_PROFILE TEMP_PROFILE(OCTOMY_QT_OGL_PROFILE, Profile)

//#define OCTOMY_QT_OGL_PROFILE_A(arg)      #arg
//#define OCTOMY_QT_OGL_PROFILE_B(name) OCTOMY_QT_OGL_PROFILE_A(name)
#define OCTOMY_QT_OGL_PROFILE_STR STRINGIFY(OCTOMY_QT_OGL_PROFILE)
//OCTOMY_QT_OGL_PROFILE_B(OCTOMY_QT_OGL_PROFILE)


#include <QOpenGLWidget>

// This right here is a result:
#include STRINGIFY(OCTOMY_QT_OGL_FUNCTIONS_CLASS)

//#include <QOpenGLFunctions>
//#include <QtOpenGLExtensions/QtOpenGLExtensions>
#include <QtOpenGL>


#endif //INCLUDEOPENGL_HPP
