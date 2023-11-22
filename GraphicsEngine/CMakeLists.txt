PROJECT( GraphicsEngine )
cmake_minimum_required(VERSION 3.1)

# Use Opengl
find_package(OpenGL)

include_directories( include )
add_library(${PROJECT_NAME} "GraphicsEngine.h" "src/OpenGLGraphicsEngine.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
