cmake_minimum_required(VERSION 2.6)

set( ProjectName TestTimelineWidget )
project( ${ProjectName} )

set( ExecName ${ProjectName} )

set( Proj_src  main.cpp drwScene.cpp drwTimelineWidget.cpp )
set( Proj_hdr drwScene.h drwTimelineWidget.h )

# Include bin directory for moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#----------------------------------------
# Use Qt
#----------------------------------------
find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})
#----------------------------------------

# moc Qt source file without a ui file
qt4_wrap_cpp( Proj_moc ${Proj_hdr})

# Create and link executable
set( Proj_all_src ${Proj_src} ${Proj_hdr} ${Proj_moc} )
add_executable( ${ExecName} ${EXECUTABLE_FLAG} ${Proj_all_src} )
target_link_libraries( ${ExecName} ${QT_LIBRARIES} )
