PROJECT( GraphicsEngine )
cmake_minimum_required(VERSION 3.1)

# Use Opengl
if ( NOT ANDROID)
	find_package(OpenGL)
endif()


include_directories( include )
add_library(${PROJECT_NAME} "GraphicsEngine.h" "src/OpenGLGraphicsEngine.cpp" "src/OpenGLESGraphicsEngine.cpp")

if (ANDROID)
	target_link_libraries(${PROJECT_NAME})
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
endif()