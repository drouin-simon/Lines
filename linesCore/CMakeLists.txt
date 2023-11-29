set( LibName linesCore )

project( ${LibName} )

set( ${LibName}_src drwToolbox.cpp drwLineTool.cpp drwGlslShader.cpp drwCamera.cpp
        Scene.cpp Frame.cpp Node.cpp line.cpp wideline.cpp drwDrawableTexture.cpp
        drwDrawingContext.cpp drwCommand.cpp drwCommandDatabase.cpp
        drwGLRenderer.cpp lcCircle.cpp LinesCore.cpp )

set( ${LibName}_hdr_moc drwToolbox.h drwTool.h drwLineTool.h drwCommandDatabase.h
        Scene.h LinesCore.h )

set( ${LibName}_hdr drwGLRenderer.h drwDrawingSurface.h drwRemoteCommandIO.h drwGlslShader.h
        drwCamera.h Frame.h line.h Node.h Primitive.h LinePrimitive.h
	wideline.h drwDrawableTexture.h drwDrawingContext.h drwCommand.h drwDataArray.h
        PolygonData.h lcCircle.h )

# moc Qt source file without a ui file
qt_wrap_cpp( ${LibName}_moc ${${LibName}_hdr_moc} )

# Create and link executable
set( ${LibName}_all_src ${${LibName}_src} ${${LibName}_moc} ${${LibName}_hdr} ${${LibName}_hdr_moc} )
add_library( ${LibName} ${${LibName}_all_src})

#================================
# Explicit dependencies
#================================
add_dependencies( ${LibName} svl )

# Define include directory for this lib
set( linesCoreIncludeDirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
target_link_libraries( ${LibName} PRIVATE libglew_static )
