#ifndef __GraphicsEngineManager_h_
#define __GraphicsEngineManager_h_

#include "../GraphicsEngine.h"

class GraphicsEngineManager {

public:
	
	static IGraphicsEngine* getGraphicsEngine();
};

#endif // __GraphicsEngineManager_h_