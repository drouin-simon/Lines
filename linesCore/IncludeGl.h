#ifndef __IncludeGl_h_
#define __IncludeGl_h_

// This file is use for cross-platform inclusion of OpenGl header files
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif


#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif
