cmake_minimum_required(VERSION 3.10)
project(Paint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Szukamy najnowszego SFML 3.x
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui-SFML.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE imgui)

# W SFML 3 nazwy docelowe (targets) są z dużej litery i używają namespace'u
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
elseif (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()