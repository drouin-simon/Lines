PROJECT( GraphicsEngine )
cmake_minimum_required(VERSION 3.1)

find_package(OpenGL)
add_library(${PROJECT_NAME} "GraphicsEngine.h" "include/GraphicsEngineManager.h" "src/OpenGLGraphicsEngine.cpp" "src/GraphicsEngineManager.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)

include_directories( include )