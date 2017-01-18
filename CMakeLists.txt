PROJECT( Lines )

cmake_minimum_required(VERSION 2.6)

#----------------------------------------
# Include BOOST dir if available
include_directories( ${BOOST_ROOT} )

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

#----------------------------------------
# configure version and git hash
set( LINES_MAJOR_VERSION 1 )
set( LINES_MINOR_VERSION 0 )
set( LINES_PATCH_VERSION 0 )
set( LINES_VERSION_QUALIFIER "Beta" )
set( LINES_BUILD_QUALIFIER "" )
set( LINES_PROTOCOL_VERSION 0 )
set( LINES_COPYRIGHT "2010 Simon Drouin" )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/linesversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/linesversion.h )

# creates githash.h file to get the githash in the program
add_custom_target( gitversion ALL DEPENDS version_git_hash )
add_custom_command(OUTPUT version_git_hash COMMAND ${CMAKE_COMMAND}
                        -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/getgithash.cmake)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/githash.h PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
#----------------------------------------

# Set flags to make a bundle on MAC
if(APPLE)
  SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
else(APPLE)
  add_subdirectory( GLee )
  include_directories( ${GLeeIncludeDir} )
endif(APPLE)

add_subdirectory( linesCore )
add_subdirectory( linesNetwork )
add_subdirectory( linesDesktopLib )
add_subdirectory( linesDesktop )
