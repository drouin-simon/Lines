PROJECT( Draw )

cmake_minimum_required(VERSION 2.6)

#----------------------------------------
# Find Boost lib (note: if not found: set BOOST_ROOT var in CMake GUI)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD OFF)
set(Boost_ADDITIONAL_VERSIONS "1.37.0" "1.38.0" "1.39.0" "1.40.0" "1.41.0" "1.42.0" ) 
find_package(Boost)
if(Boost_FOUND)
  include_directories( ${Boost_INCLUDE_DIR} )
endif(Boost_FOUND)
#----------------------------------------

#----------------------------------------
# Use Svl
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../svl ${CMAKE_CURRENT_BINARY_DIR}/svl )
include_directories( ${svlIncludeDir} )
#----------------------------------------

#----------------------------------------
# Use Base lib
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../Base ${CMAKE_CURRENT_BINARY_DIR}/Base )
include_directories( ${BaseIncludeDir} )
#----------------------------------------

#----------------------------------------
# Use Qt
find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTNETWORK TRUE)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})
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
#add_subdirectory( Flip )
add_subdirectory( Drw )
#add_subdirectory( DrwNetworkLog )
add_subdirectory( Examples )
