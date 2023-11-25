set( ExecName Lines )

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( Drw_src  main.cpp 
		MainWindow.cpp 
		PlaybackControlerWidget.cpp 
                ExportDialog.cpp
                LinesPreferencesWidget.cpp )

set( Drw_hdr MainWindow.h  
		PlaybackControlerWidget.h 
                ExportDialog.h
                LinesPreferencesWidget.h )

# Include dir for DrawLib
include_directories(${linesCoreIncludeDirs})
include_directories(${linesNetworkIncludeDirs})
include_directories(${linesDesktopLibIncludeDirs})

# Include bin directory for moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${glew_SOURCE_DIR} ${glew_BINARY_DIR})

# Create classes from Qt ui files
set( Drw_ui ExportDialog.ui LinesPreferencesWidget.ui )
qt_wrap_ui( Drw_hdr_ui ${Drw_ui} )

# moc Qt source file without a ui file
qt_wrap_cpp( Drw_moc ${Drw_hdr})

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

if( WIN32 )
	set( executable_flags WIN32 )
        set( Drw_src ${Drw_src} Lines.rc )
endif()

#--------------------------------------------------------------------------------
# Create and link executable
#--------------------------------------------------------------------------------
set( Drw_all_src ${Drw_src} ${Drw_hdr} ${Drw_moc} ${Drw_hdr_ui} )
add_executable( ${ExecName} ${executable_flags} ${Drw_all_src} )
target_link_libraries( ${ExecName} linesDesktopLib linesNetwork linesCore GraphicsEngine svl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${QT_LIBRARIES} )
target_link_libraries(${ExecName} OpenGL Widgets Network)
if (APPLE)
    set_target_properties( ${ExecName} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/LinesInfo.plist )
else()
    target_link_libraries( ${ExecName} OpenGL::GL libglew_static)
endif()


#--------------------------------------------------------------------------------
# INSTALL
#--------------------------------------------------------------------------------
#PrintVariables()

# Run Qt deployment tool for each platform
get_target_property( QMakeLocation Qt${QT_VERSION_MAJOR}::qmake LOCATION)
get_filename_component( QtBinLocation ${QMakeLocation} DIRECTORY )
if( APPLE )
    set( MacDeployQtExec ${QtBinLocation}/macdeployqt )
    set( BuiltExecFullPath ${CMAKE_CURRENT_BINARY_DIR}/${ExecName}.app )
    set( BuiltExecFullPathExec ${BuiltExecFullPath}/Contents/MacOS/Lines )
    file( TIMESTAMP ${BuiltExecFullPathExec} ExecDate %Y-%m-%d )
    set( DmgFileName ${ExecName}-v${LINES_MAJOR_VERSION}.${LINES_MINOR_VERSION}.${LINES_PATCH_VERSION}-${LINES_VERSION_QUALIFIER}-${ExecDate}-macOS-${CMAKE_OSX_DEPLOYMENT_TARGET}-${GIT_COMMIT_HASH_SHORT}.dmg )
    install( CODE "file( COPY ${BuiltExecFullPath} DESTINATION ${CMAKE_INSTALL_PREFIX} )
        execute_process( COMMAND ${MacDeployQtExec} ${CMAKE_INSTALL_PREFIX}/${ExecName}.app -dmg )
        execute_process( COMMAND mv ${CMAKE_INSTALL_PREFIX}/${ExecName}.dmg ${CMAKE_INSTALL_PREFIX}/${DmgFileName} )" )
elseif( WIN32 )
   install( TARGETS ${ExecName} DESTINATION . )
   # on Windows, we use WinDeployQt. CPack below is creating an installer using NSIS
   include(${CMAKE_EXT_DIR}/windeployqt.cmake)
   windeployqt( ${ExecName} . )
endif()

#--------------------------------------------------------------------------------

#================================
# Packaging
#================================
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lines animation")
set( CPACK_PACKAGE_NAME ${ExecName} )
set( CPACK_PACKAGE_CONTACT "Simon Drouin")
set( CPACK_PACKAGE_VENDOR "linesanimation.org")
set( CPACK_PACKAGE_VERSION_MAJOR ${LINES_MAJOR_VERSION})
set( CPACK_PACKAGE_VERSION_MINOR ${LINES_MINOR_VERSION})
set( CPACK_PACKAGE_VERSION_PATCH ${LINES_PATCH_VERSION})
if( APPLE )
    set( CPACK_GENERATOR "DragNDrop" )
elseif( WIN32 )
	set( CPACK_GENERATOR NSIS )
else()
    set( CPACK_GENERATOR "TGZ")
endif()
set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${LINES_VERSION_QUALIFIER}-${GIT_COMMIT_HASH_SHORT}-${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)
