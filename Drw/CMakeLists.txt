project( LinesProject )

set( ExecName Lines )

set( Drw_src  main.cpp 
		MainWindow.cpp 
		PrimitiveToolOptionWidget.cpp 
		PlaybackControlerWidget.cpp 
		DisplaySettingsWidget.cpp 
		ExportDialog.cpp )

set( Drw_hdr MainWindow.h 
		PrimitiveToolOptionWidget.h 
		PlaybackControlerWidget.h 
		DisplaySettingsWidget.h
		ExportDialog.h )

# Include dir for DrawLib
include_directories(${DRAWLIB_INCLUDE_DIR})

# Include bin directory for moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Create classes from Qt ui files
set( Drw_ui ExportDialog.ui )
qt5_wrap_ui( Drw_hdr_ui ${Drw_ui} )

# moc Qt source file without a ui file
qt5_wrap_cpp( Drw_moc ${Drw_hdr})

#--------------------------------------------------------------------------------
# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  set( MACOSX_BUNDLE_INFO_STRING "Lines" )
  set( MACOSX_BUNDLE_ICON_FILE "lines_icon.icns" )
  set( MACOSX_BUNDLE_SHORT_VERSION_STRING 0.9 )
  set( MACOSX_BUNDLE_VERSION 0.9 )
  set( MACOSX_BUNDLE_LONG_VERSION_STRING 0.9 )
  set( MACOSX_BUNDLE_BUNDLE_VERSION 0.9 )
  set( MACOSX_BUNDLE_COPYRIGHT "2010 Simon Drouin" )
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/lines_icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET( Drw_src ${Drw_src} ${CMAKE_CURRENT_SOURCE_DIR}/lines_icon.icns)
ENDIF(APPLE)
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
# Create and link executable
#
set( Drw_all_src ${Drw_src} ${Drw_hdr} ${Drw_moc} ${Drw_hdr_ui} )
add_executable( ${ExecName} ${EXECUTABLE_FLAG} ${Drw_all_src} )
target_link_libraries( ${ExecName} Draw svl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${QT_LIBRARIES} rtaudio )
qt5_use_modules( ${ExecName} OpenGL Widgets Network )

if( NOT APPLE )
	target_link_libraries( ${ExecName} GLee )
endif( NOT APPLE )
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
# INSTALL
#
set( APPS ${CMAKE_INSTALL_PREFIX}/bin/${ExecName} )
IF(APPLE)
  set( APPS ${CMAKE_INSTALL_PREFIX}/${ExecName}.app )
ENDIF(APPLE)
IF(WIN32)
  set( APPS "\${CMAKE_INSTALL_PREFIX}/bin/\${ExecName}.exe")
ENDIF(WIN32)
INSTALL( TARGETS ${ExecName} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime )

# directories to look for dependencies
SET(DIRS ${QT_LIBRARY_DIR})
INSTALL(CODE "include(BundleUtilities) 
    fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")" COMPONENT Runtime)

IF( APPLE )
   IF (QT_MAC_USE_COCOA)                 
        INSTALL(CODE "execute_process(COMMAND cp -R \"${QT_QTGUI_LIBRARY}/Resources/qt_menu.nib\" \"${APPS}/Contents/Resources/\" )" )
    ENDIF(QT_MAC_USE_COCOA)
ENDIF(APPLE)
#--------------------------------------------------------------------------------


#================================
# Explicit dependencies
#================================
add_dependencies( ${ExecName} svl Draw )
if( NOT APPLE )
	add_dependencies( ${ExecName} GLee )
endif( NOT APPLE )
