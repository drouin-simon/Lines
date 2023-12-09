#include "../include/GraphicsEngineManager.h"

static IGraphicsEngine* instance = nullptr;

IGraphicsEngine* GraphicsEngineManager::getGraphicsEngine()
{
	if (instance == nullptr) {
		instance = new GraphicsEngine();
	}

	return instance;
}

void GraphicsEngineManager::deleteGraphicsEngine()
{
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}