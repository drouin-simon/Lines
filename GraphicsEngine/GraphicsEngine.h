#ifndef GraphicsEngine_H
#define GraphicsEngine_H

#if !(defined(__ANDROID__)) 
	#include "include/IncludeGLEW.h"
#endif

#include "include/IGraphicsEngine.h"

#include "include/OpenGLGraphicsEngine.h"
using GraphicsEngine = OpenGLGraphicsEngine;

#endif // GraphicsEngine_H