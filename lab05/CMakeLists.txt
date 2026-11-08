cmake_minimum_required(VERSION 3.16)
project(lab05 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(lab05 WIN32 MACOSX_BUNDLE
    Source.cpp
    GUI.cpp
    GUIMyFrame1.cpp
    ConfigClass.cpp
    ChartClass.cpp
    vecmat.cpp
)

target_link_libraries(lab05 PRIVATE ${wxWidgets_LIBRARIES})

# SetProcessDPIAware() (used in Source.cpp) exists only on Windows -
# on other systems replace it with a no-op so the sources stay unchanged.
if(NOT WIN32)
    target_compile_options(lab05 PRIVATE "-DSetProcessDPIAware()=((void)0)")
endif()

# On macOS drawing through wxClientDC (used by the template) is not displayed;
# this helper redraws the chart from the panel's paint event instead.
if(APPLE)
    target_sources(lab05 PRIVATE macos_paint_fix.cpp)
endif()

# "cmake --build build --target run" builds and starts the program
if(APPLE)
    add_custom_target(run
        COMMAND $<TARGET_BUNDLE_DIR:lab05>/Contents/MacOS/lab05
        DEPENDS lab05
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
    add_custom_target(run
        COMMAND $<TARGET_FILE:lab05>
        DEPENDS lab05
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
