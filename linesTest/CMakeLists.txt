cmake_minimum_required(VERSION 3.10)
project(linesTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${linesCoreIncludeDirs})
include_directories(${linesNetworkIncludeDirs})
include_directories(${linesDesktopLibIncludeDirs})
include_directories(${GraphicsEngineIncludeDirs})

set(linesTest_src main.cpp)

add_executable(linesTest ${linesTest_src})

if (ANDROID)
    find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

    target_link_libraries(linesTest 
        linesDesktopLib 
        linesNetwork 
        linesCore 
        GraphicsEngine 
        svl 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Network 
        Qt6::Widgets 
    )
else()
    find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

    target_link_libraries(linesTest 
        linesDesktopLib 
        linesNetwork 
        linesCore 
        GraphicsEngine 
        svl 
        ${OPENGL_gl_LIBRARY} 
        ${OPENGL_glu_LIBRARY} 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Network 
        Qt6::Widgets 
        Qt6::OpenGL
        ${GLEW_LIBRARIES}
    )
endif()


