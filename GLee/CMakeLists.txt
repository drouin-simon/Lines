project( GLee )

set( LibName GLee )
set( lib_src  GLee.c )
set( lib_hdr GLee.h )

add_library( ${LibName} ${lib_src} ${lib_hdr} )

#================================
# Define include dir for
# dependent projects
#================================
set( GLeeIncludeDir ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Include directory for GLee .h files" )
MARK_AS_ADVANCED( GLeeIncludeDir )

