PROJECT( Draw )

cmake_minimum_required(VERSION 2.6)

#----------------------------------------
# Use Svl
add_subdirectory(svl)
include_directories( ${svlIncludeDir} )
#----------------------------------------

#----------------------------------------
# Use Qt 5
find_package( Qt5 COMPONENTS Widgets OpenGL Network REQUIRED )
include_directories( ${Qt5Widgets_INCLUDE_DIRS} )
include_directories( ${Qt5OpenGL_INCLUDE_DIRS} )
include_directories( ${Qt5Network_INCLUDE_DIRS} )
#----------------------------------------

#----------------------------------------
# Use Opengl
find_package(OpenGL)
#----------------------------------------

# Set flags to make a bundle on MAC
if(APPLE)
  SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
else(APPLE)
  add_subdirectory( GLee )
  include_directories( ${GLeeIncludeDir} )
endif(APPLE)


add_subdirectory( DrawLib )
add_subdirectory( Drw )
#add_subdirectory( DrwNetworkLog )
#add_subdirectory( Examples )
