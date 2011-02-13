set( theProjectName GlutTemplateDrawLib )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# Include dir for DrawLib
include_directories(${DRAWLIB_INCLUDE_DIR})

add_dependencies( ${ExecName} svl Draw )

SET( theProject_SRC main.cpp )

ADD_EXECUTABLE( ${theProjectName} ${theProject_SRC} )
TARGET_LINK_LIBRARIES( ${theProjectName} svl Draw ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES}  )
