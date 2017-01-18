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
include_directories(${linesCoreIncludeDirs})
include_directories(${linesNetworkIncludeDirs})
include_directories(${linesDesktopLibIncludeDirs})

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
  set( MACOSX_BUNDLE_BUNDLE_NAME "Lines" )
  set( MACOSX_BUNDLE_INFO_STRING "Lines" )
  set( MACOSX_BUNDLE_ICON_FILE "lines_icon.icns" )
  set( MACOSX_BUNDLE_VERSION ${LINES_MAJOR_VERSION}.${LINES_MINOR_VERSION}.${LINES_PATCH_VERSION} )
  set( MACOSX_BUNDLE_SHORT_VERSION_STRING ${LINES_MAJOR_VERSION}.${LINES_MINOR_VERSION}.${LINES_PATCH_VERSION} )
  set( MACOSX_BUNDLE_LONG_VERSION_STRING "${LINES_MAJOR_VERSION}.${LINES_MINOR_VERSION}.${LINES_PATCH_VERSION} - ${LINES_VERSION_QUALIFIER} rev. ${LINES_GIT_HASH}" )
  set( MACOSX_BUNDLE_COPYRIGHT ${LINES_COPYRIGHT} )
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/LinesInfo.plist.in ${CMAKE_CURRENT_BINARY_DIR}/LinesInfo.plist )

  # Deal with the icon
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/lines_icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set( Drw_src ${Drw_src} ${CMAKE_CURRENT_SOURCE_DIR}/lines_icon.icns)

  # make sure we create an app bundle and not a standalone executable
  set( executable_flags MACOSX_BUNDLE )

ENDIF(APPLE)
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
# Create and link executable
#
set( Drw_all_src ${Drw_src} ${Drw_hdr} ${Drw_moc} ${Drw_hdr_ui} )
add_executable( ${ExecName} ${EXECUTABLE_FLAG} ${Drw_all_src} )
target_link_libraries( ${ExecName} linesDesktopLib linesNetwork linesCore svl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${QT_LIBRARIES} )
qt5_use_modules( ${ExecName} OpenGL Widgets Network )

if( NOT APPLE )
	target_link_libraries( ${ExecName} GLee )
endif( NOT APPLE )
#--------------------------------------------------------------------------------

if (APPLE)
    set_target_properties( ${ExecName} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/LinesInfo.plist )
endif()


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
add_dependencies( ${ExecName} svl linesDesktopLib )
if( NOT APPLE )
	add_dependencies( ${ExecName} GLee )
endif( NOT APPLE )
