#ifndef __IncludeGLEW_h_
#define __IncludeGLEW_h_

#if (defined(_WIN32) && !defined(NOMINMAX))
#define NOMINMAX
#include <Windows.h>
#endif

#if !(defined(__APPLE__) && defined(__MACH__))
#define NOMINMAX
#include <GL/glew.h>
//#include <EGL/egl.h>
#endif

#endif
