#ifndef GraphicsEngine_H
#define GraphicsEngine_H

#if !(defined(__ANDROID__)) 
	#include "include/IncludeGLEW.h"
#endif

#include "include/IGraphicsEngine.h"

#if (defined(__ANDROID__))
	#include "include/OpenGLESGraphicsEngine.h"
	using GraphicsEngine = OpenGLESGraphicsEngine;
#else
	#include "include/OpenGLGraphicsEngine.h"
	using GraphicsEngine = OpenGLGraphicsEngine;
#endif


#endif // GraphicsEngine_H