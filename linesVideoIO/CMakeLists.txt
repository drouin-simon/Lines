set( LibName linesVideoIO )

project( ${LibName} )

set( ${LibName}_src VideoEncoder.cpp )
set( ${LibName}_hdr VideoEncoder.h )

# Create and link executable
add_library( ${LibName} ${${LibName}_src} ${${LibName}_hdr} )
target_link_libraries( ${LibName} ${FFMPEG_LIBRARIES} )
target_include_directories( ${LibName} PRIVATE ${FFMPEG_INCLUDE_DIR} ) 


