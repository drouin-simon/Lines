PROJECT( GraphicsEngine )
cmake_minimum_required(VERSION 3.1)

# Use Opengl
find_package(OpenGL)

# Need GLEW for OpenGL if not on MacOS
if( NOT APPLE )
  find_package(GLEW REQUIRED)
  include_directories(${GLEW_INCLUDE_DIRS})
endif()

include_directories( include )
add_library(${PROJECT_NAME} "GraphicsEngine.h" "src/OpenGLGraphicsEngine.cpp")