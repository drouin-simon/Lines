project( Flp )

set( ExecName Flp )

set( Flp_src  main.cpp MainWindow.cpp )
set( Flp_hdr )
set( Flp_hdr_moc MainWindow.h )

# Include dir for DrawLib
include_directories(${DRAWLIB_INCLUDE_DIR})

# Include bin directory for moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# moc Qt source file without a ui file
qt5_wrap_cpp( Flp_moc ${Flp_hdr_moc} )

#--------------------------------------------------------------------------------
# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  set( MACOSX_BUNDLE_INFO_STRING "Flp" )
  #set( MACOSX_BUNDLE_ICON_FILE "lines_icon.icns" )
  set( MACOSX_BUNDLE_SHORT_VERSION_STRING 0.9 )
  set( MACOSX_BUNDLE_VERSION 0.9 )
  set( MACOSX_BUNDLE_LONG_VERSION_STRING 0.9 )
  set( MACOSX_BUNDLE_BUNDLE_VERSION 0.9 )
  set( MACOSX_BUNDLE_COPYRIGHT "2010 Simon Drouin" )
  # set where in the bundle to put the icns file
  #SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/lines_icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  #SET( Drw_src ${Drw_src} ${CMAKE_CURRENT_SOURCE_DIR}/lines_icon.icns)
  SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
ENDIF(APPLE)
#--------------------------------------------------------------------------------


# Create and link executable
set( Flp_all_src ${Flp_src} ${Flp_hdr} ${Flp_hdr_moc} ${Flp_moc} )
add_executable( ${ExecName} ${EXECUTABLE_FLAG} ${Flp_all_src} )
qt5_use_modules( ${ExecName} Widgets OpenGL )
target_link_libraries( ${ExecName} svl Draw ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} )

# Install
install_programs( /bin FILES ${ExecName} )

#================================
# Explicit dependencies
#================================
add_dependencies( ${ExecName} svl Draw )
