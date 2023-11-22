set( LibName linesDesktopLib )

include_directories( ${linesCoreIncludeDirs} )
include_directories( ${linesNetworkIncludeDirs} )

set( DrawLib_src LinesApp.cpp drwDrawingWidget.cpp DrawingWidgetMT.cpp drwsimplifiedtoolbar.cpp SideToolbar.cpp
        TabletStateWidget.cpp drwNetworkConnectDialog.cpp
        drwBitmapExporter.cpp drwGradientWidget.cpp ClickScrollWidget.cpp
        drwLineToolViewportWidget.cpp drwFpsCounter.cpp
        drwGlobalLineParams.cpp drwAutoGui.cpp TabletStateTestingArea.cpp drwAspectRatioWidget.cpp )

set( DrawLib_hdr_moc LinesApp.h drwDrawingWidget.h DrawingWidgetMT.h drwsimplifiedtoolbar.h SideToolbar.h TabletStateWidget.h
        drwNetworkConnectDialog.h drwBitmapExporter.h drwGradientWidget.h ClickScrollWidget.h
        drwGlobalLineParams.h drwAutoGui.h TabletStateTestingArea.h drwAspectRatioWidget.h )

set( DrawLib_hdr drwLineToolViewportWidget.h drwFpsCounter.h )

# Include bin directory for ui_ files
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create classes from Qt ui files
set( Drw_ui TabletStateWidget.ui drwNetworkConnectDialog.ui drwsimplifiedtoolbar.ui SideToolbar.ui )
qt5_wrap_ui( DrawLib_hdr_ui ${Drw_ui} )

# moc Qt source file without a ui file
qt5_wrap_cpp( DrawLib_moc ${DrawLib_hdr_moc} )

# make rules to create c++ source from Qt resource files
set( LinesLib_rcs lineslibresources.qrc )
qt5_add_resources( LinesLib_rcs_src ${LinesLib_rcs})

# Create and link executable
set( DrawLib_all_src ${DrawLib_src} ${DrawLib_moc} ${DrawLib_hdr} ${DrawLib_hdr_moc} ${DrawLib_hdr_ui} ${LinesLib_rcs_src} )
add_library( ${LibName} ${DrawLib_all_src} )

target_link_libraries( ${LibName} libglew_static )

#================================
# Define include dir for dependent projects
#================================
set( linesDesktopLibIncludeDirs ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )

#================================
# Explicit dependencies
#================================
ADD_DEPENDENCIES( ${LibName} svl linesCore linesNetwork GraphicsEngine )
