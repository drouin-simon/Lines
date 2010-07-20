project( Flp )

set( ExecName Flp )

set( Flp_src  main.cpp MainWindow.cpp )
set( Flp_hdr MainWindow.h )

# Include dir for DrawLib
include_directories(${DRAWLIB_INCLUDE_DIR})

# Include bin directory for moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# moc Qt source file without a ui file
qt4_automoc(${Flp_src})

# Create and link executable
set( Flp_all_src ${Flp_src} ${Flp_hdr} )
add_executable( ${ExecName} ${Flp_all_src} )
target_link_libraries( ${ExecName} svl vil Draw ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${QT_LIBRARIES} )

# Install
install_programs( /bin FILES ${ExecName} )

#================================
# Explicit dependencies
#================================
add_dependencies( ${ExecName} svl Draw )