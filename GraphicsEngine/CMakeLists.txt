PROJECT( GraphicsEngine )
cmake_minimum_required(VERSION 3.1)

# Use Opengl
if ( NOT ANDROID)
	find_package(OpenGL)
endif()


include_directories( include )
if (ANDROID)
	add_library(${PROJECT_NAME} "GraphicsEngine.h" "include/GraphicsEngineManager.h" "src/GraphicsEngineManager.cpp" "src/OpenGLESGraphicsEngine.cpp")
	target_link_libraries(${PROJECT_NAME})
else()
	add_library(${PROJECT_NAME} "GraphicsEngine.h" "include/GraphicsEngineManager.h" "src/OpenGLGraphicsEngine.cpp" "src/GraphicsEngineManager.cpp")
	target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
endif()