project( DrawLib )

set( LibName Draw )

set( DrawLib_src  drwDrawingWidget.cpp
		drwDrawingWidgetInteractor.cpp
		drwToolbox.cpp
		drwLineTool.cpp
		drwGlslShader.cpp
		Camera.cpp 
		Scene.cpp
		Frame.cpp
		Node.cpp
		Primitive.cpp
		line.cpp  
		wideline.cpp
		Circle.cpp 
		drwDrawableTexture.cpp
		TabletStateWidget.cpp
		PlaybackControler.cpp 
		ImageSprite.cpp 
		ImageFlippingTool.cpp
		drwCommand.cpp
		drwCommandDatabase.cpp
		drwNetworkConnection.cpp
		drwNetworkInterface.cpp
		drwNetworkThread.cpp
		drwDisplaySettings.cpp
		drwEditionState.cpp 
		drwCommandDispatcher.cpp 
		drwBitmapExporter.cpp
		drwCursor.cpp )

set( DrawLib_hdr_moc 	drwDrawingWidget.h 
			drwWidgetObserver.h
			drwToolbox.h
			drwLineTool.h
			TabletStateWidget.h
			PlaybackControler.h
			drwCommandDatabase.h 
			Scene.h
			drwNetworkConnection.h
			drwNetworkInterface.h
			drwNetworkThread.h
			drwDisplaySettings.h
			drwEditionState.h 
			ImageFlippingTool.h
			drwCommandDispatcher.h 
			drwBitmapExporter.h )

set( DrawLib_hdr	drwDrawingWidgetInteractor.h
			drwGlslShader.h
			Camera.h
			Frame.h
			line.h  
			Node.h
			Primitive.h
			LinePrimitive.h
			wideline.h
			Circle.h 
			drwDrawableTexture.h
			drwDrawingContext.h
			ImageSprite.h
			drwCommand.h
			drwDataArray.h
			PolygonData.h
			drwCursor.h )

# Include bin directory for ui_ files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Create classes from Qt ui files
set( Drw_ui TabletStateWidget.ui )
qt4_wrap_ui( DrawLib_hdr_ui ${Drw_ui} )

# moc Qt source file without a ui file
qt4_wrap_cpp( DrawLib_moc ${DrawLib_hdr_moc} )

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
