cmake_minimum_required(VERSION 3.16)

PROJECT( Lines )

# Include some CMake utilities
set( CMAKE_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/printvariables.cmake )

#----------------------------------------
# Use Svl
add_subdirectory(svl)
include_directories( ${svlIncludeDir} )
#----------------------------------------

#----------------------------------------
# Use Qt 6
find_package( Qt6 6.10 COMPONENTS Widgets OpenGL OpenGLWidgets Network REQUIRED )
include_directories( ${Qt6Widgets_INCLUDE_DIRS} )
include_directories( ${Qt6OpenGL_INCLUDE_DIRS} )
include_directories( ${Qt6OpenGLWidgets_INCLUDE_DIRS} )
include_directories( ${Qt6Network_INCLUDE_DIRS} )
#----------------------------------------

#----------------------------------------
# Qt 6 requires C++17
set( CMAKE_CXX_STANDARD 17 )
#----------------------------------------

# OpenGL is provided by Qt6::OpenGL and Qt6::OpenGLWidgets
#----------------------------------------

#----------------------------------------
# configure version and git hash
set( LINES_MAJOR_VERSION 1 )
set( LINES_MINOR_VERSION 3 )
set( LINES_PATCH_VERSION 0 )
set( LINES_VERSION_QUALIFIER Beta )
set( LINES_BUILD_QUALIFIER "" )
set( LINES_PROTOCOL_VERSION 0 )
set( LINES_COPYRIGHT "2010 Simon Drouin" )
option( LINES_MASTER_BUILD "Does this build enable Master functionality?" OFF )
if( LINES_MASTER_BUILD )
    set( LINES_DEFINE_MASTER_BUILD "#define LINES_MASTER_BUILD" )
else()
    set( LINES_DEFINE_MASTER_BUILD "" )
endif()
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/linesversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/linesversion.h )

# Find git hash
set( GIT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/getgithash.cmake )

# creates githash.h file to get the githash in the program
add_custom_target( gitversion ALL DEPENDS version_git_hash )
add_custom_command(OUTPUT version_git_hash COMMAND ${CMAKE_COMMAND}
                         -DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}
                         -DGIT_COMMIT_HASH_SHORT=${GIT_COMMIT_HASH_SHORT}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configuregithash.cmake)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/githash.h PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
#----------------------------------------

# Need Glee for OpenGL if not on MacOS
if( NOT APPLE )
  add_subdirectory( GLee )
  include_directories( ${GLeeIncludeDir} )
endif()

add_subdirectory( linesCore )
add_subdirectory( linesNetwork )
add_subdirectory( linesDesktopLib )
add_subdirectory( linesDesktop )
