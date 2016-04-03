project( DrawLib )

set( LibName Draw )

set( DrawLib_src  drwDrawingWidget.cpp
		drwToolbox.cpp
		drwLineTool.cpp
		drwGlslShader.cpp
		drwCamera.cpp
		Scene.cpp
		Frame.cpp
		Node.cpp
		Primitive.cpp
		line.cpp  
                wideline.cpp
		drwDrawableTexture.cpp
        drwDrawingContext.cpp
		TabletStateWidget.cpp
		PlaybackControler.cpp 
		drwCommand.cpp
		drwCommandDatabase.cpp
		drwNetworkConnection.cpp
        drwNetworkServer.cpp
		drwNetworkClient.cpp
        drwNetworkManager.cpp
		drwNetworkServerFinder.cpp
		drwNetworkConnectDialog.cpp
		drwDisplaySettings.cpp
		drwEditionState.cpp 
		drwCommandDispatcher.cpp 
		drwBitmapExporter.cpp
		drwCursor.cpp 
		drwGradientWidget.cpp
        drwLineToolViewportWidget.cpp
        drwFpsCounter.cpp
        drwDrawingEngine.cpp
        drwGLRenderer.cpp 
	drwGlobalLineParams.cpp 
	drwAutoGui.cpp )

set( DrawLib_hdr_moc 	drwDrawingWidget.h 
			drwWidgetObserver.h
			drwToolbox.h
			drwLineTool.h
			TabletStateWidget.h
			PlaybackControler.h
			drwCommandDatabase.h 
			Scene.h
                        drwCursor.h
			drwNetworkConnection.h
            drwNetworkServer.h
			drwNetworkClient.h
            drwNetworkManager.h
			drwNetworkServerFinder.h
			drwNetworkConnectDialog.h
			drwDisplaySettings.h
			drwEditionState.h 
			drwCommandDispatcher.h 
			drwBitmapExporter.h
            drwGradientWidget.h
            drwDrawingEngine.h
            drwGLRenderer.h 
	    drwGlobalLineParams.h
            drwAutoGui.h )

set( DrawLib_hdr        linesversion.h
			drwGlslShader.h
			drwCamera.h
			Frame.h
			line.h  
			Node.h
			Primitive.h
			LinePrimitive.h
			wideline.h
			drwDrawableTexture.h
			drwDrawingContext.h
			drwCommand.h
			drwDataArray.h
			PolygonData.h
            drwLineToolViewportWidget.h
            drwFpsCounter.h )

# Include bin directory for ui_ files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Create classes from Qt ui files
set( Drw_ui TabletStateWidget.ui drwNetworkConnectDialog.ui )
qt5_wrap_ui( DrawLib_hdr_ui ${Drw_ui} )

# moc Qt source file without a ui file
qt5_wrap_cpp( DrawLib_moc ${DrawLib_hdr_moc} )

# Create and link executable
set( DrawLib_all_src ${DrawLib_src} ${DrawLib_moc} ${DrawLib_hdr} ${DrawLib_hdr_moc} ${DrawLib_hdr_ui} )
add_library( ${LibName} ${DrawLib_all_src} )

#================================
# Define include dir for
# dependent projects
#================================
set( include_dir_content ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
SET( DRAWLIB_INCLUDE_DIR ${include_dir_content} CACHE PATH "Include directory for DrawLib .h files" )
MARK_AS_ADVANCED( DRAWLIB_INCLUDE_DIR )

#================================
# Explicit dependencies
#================================
ADD_DEPENDENCIES( ${LibName} svl )
