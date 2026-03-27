set( LibName linesNetwork )

include_directories( ${linesCoreIncludeDirs} )

project( ${LibName} )

set( ${LibName}_src drwNetworkClient.cpp drwNetworkConnection.cpp drwNetworkManager.cpp 
	drwNetworkServerFinder.cpp drwNetworkServer.cpp )

set( ${LibName}_hdr_moc drwNetworkClient.h drwNetworkConnection.h drwNetworkManager.h 
	drwNetworkServerFinder.h drwNetworkServer.h )

set( ${LibName}_hdr  )

# moc Qt source file without a ui file
qt_wrap_cpp( ${LibName}_moc ${${LibName}_hdr_moc} )

# Create and link executable
set( ${LibName}_all_src ${${LibName}_src} ${${LibName}_moc} ${${LibName}_hdr} ${${LibName}_hdr_moc} )
add_library( ${LibName} ${${LibName}_all_src} )

# Define include dir for this lib
set( linesNetworkIncludeDirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )

