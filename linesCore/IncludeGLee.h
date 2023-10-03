#ifndef __IncludeGLee_h_
#define __IncludeGLee_h_

#if (defined(_WIN32) && !defined(NOMINMAX))
#define NOMINMAX
#include <Windows.h>
#endif

#if !(defined(__APPLE__) && defined(__MACH__))
#define NOMINMAX
#include <GLee.h>
#endif

#endif
