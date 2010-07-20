cmake_minimum_required(VERSION 2.6)

project( DrwNetworkLog )

# Include dir for DrawLib
include_directories(${DRAWLIB_INCLUDE_DIR})

# Include bin directory for moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set( receiver_src drwNetworkLog.cpp receiver.cpp )
set( receiver_hdr receiver.h )

# moc Qt source file without a ui file
qt4_wrap_cpp( receiver_moc ${receiver_hdr})

# Create and link executable
set( receiver_all_src ${receiver_src} ${receiver_hdr} ${receiver_moc} )
add_executable( DrwNetworkLog ${EXECUTABLE_FLAG} ${receiver_all_src} )
target_link_libraries( DrwNetworkLog Draw ${QT_LIBRARIES} )

#================================
# Explicit dependencies
#================================
add_dependencies( ${ExecName} svl Draw )
