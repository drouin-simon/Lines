#ifndef __IncludeGl_h_
#define __IncludeGl_h_

// This file is use for cross-platform inclusion of OpenGl header files
#if (defined(_WIN32) && !defined(NOMINMAX))
#define NOMINMAX
#include <Windows.h>
#endif


#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
//#define GL_GLEXT_PROTOTYPES
//#include <GLES3/gl3.h>
#include <GL/glu.h>
#endif

#endif
