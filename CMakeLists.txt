PROJECT( Draw )

cmake_minimum_required(VERSION 2.6)

#----------------------------------------
# Find Boost lib (note: if not found: set BOOST_ROOT var in CMake GUI)
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREAD OFF)
#set(Boost_ADDITIONAL_VERSIONS "1.37.0" "1.38.0" "1.39.0" "1.40.0" "1.41.0" "1.42.0" “1.55.0” ) 
#set( BOOST_ROOT "/usr/lib" CACHE PATH "Base directory that contains the Boost lib" )
#find_package(Boost)
#if(Boost_FOUND)
#  include_directories( ${Boost_INCLUDE_DIR} )
#endif(Boost_FOUND)
#----------------------------------------

#----------------------------------------
# Use Svl
add_subdirectory(svl)
include_directories( ${svlIncludeDir} )
#----------------------------------------

#----------------------------------------
# Use Base lib
#add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../Base ${CMAKE_CURRENT_BINARY_DIR}/Base )
#include_directories( ${BaseIncludeDir} )
#----------------------------------------

#----------------------------------------
# Use Qt
#find_package(Qt4 REQUIRED)
#set(QT_USE_QTOPENGL TRUE)
#set(QT_USE_QTNETWORK TRUE)
#include(${QT_USE_FILE})
#include_directories(${QT_INCLUDE_DIR})
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

#----------------------------------------
# find and setup RtAudio
find_path( RtAudio_Include_Dir RtAudio.h )
if( NOT RtAudio_Include_Dir )
    message( FATAL " Couldn't find RtAudio. Specify RtAudio_Include_Dir variable" )
endif()
include_directories( ${RtAudio_Include_Dir} )
link_directories( ${RtAudio_Include_Dir} )
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
