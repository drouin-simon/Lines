#ifndef __IncludeGl_h_
#define __IncludeGl_h_

// This file is use for cross-platform inclusion of OpenGl header files
#if (defined(_WIN32) && !defined(NOMINMAX))
#define NOMINMAX
#include <Windows.h>
#endif


#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#elif !(defined(__ANDROID__))
    #include <GL/gl.h>
#endif

#endif
