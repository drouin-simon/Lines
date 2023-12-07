#include "../include/GraphicsEngineManager.h"

static IGraphicsEngine* instance = nullptr;

IGraphicsEngine* GraphicsEngineManager::getGraphicsEngine()
{
	if (instance == nullptr) {
		instance = new GraphicsEngine();
	}

	return instance;
}
