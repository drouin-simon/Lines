#ifndef __IncludeGLEW_h_
#define __IncludeGLEW_h_

#if (defined(_WIN32) && !defined(NOMINMAX))
	#define NOMINMAX
	#include <Windows.h>
#endif

#if (!defined(__ANDROID__))
	#include <GL/glew.h>
#endif

#endif // __IncludeGLEW_h_
