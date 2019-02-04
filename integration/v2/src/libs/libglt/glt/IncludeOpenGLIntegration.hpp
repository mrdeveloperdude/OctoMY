#ifndef INCLUDEOPENGLINTEGRATION_HPP
#define INCLUDEOPENGLINTEGRATION_HPP


// For GL-CL interop we need access to real GLX/CGL/WGL

// TODO: Add CGL/WGL for OSX/Microsoft respectively when we are working towards those platforms

#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
//define something for Windows (64-bit only)
#else
//define something for Windows (32-bit only)
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#   error "Unknown Apple platform"
#endif
#elif __linux__
// linux
#include <GL/glx.h>
#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#   error "Unknown compiler"
#endif


#endif // INCLUDEOPENGLINTEGRATION_HPP
