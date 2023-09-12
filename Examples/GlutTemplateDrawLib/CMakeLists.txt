set( theProjectName GlutTemplateDrawLib )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# Include dir for DrawLib
include_directories(${DRAWLIB_INCLUDE_DIR})

SET( theProject_SRC main.cpp )

ADD_EXECUTABLE( ${theProjectName} ${theProject_SRC} )
target_link_libraries( ${theProjectName} svl Draw ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES}  )

add_dependencies( ${theProjectName} svl Draw )
