cmake_minimum_required(VERSION 3.15)
project(IUiBG_Lab03 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# W SFML 3 komponenty muszą być pisane wielką literą: Graphics, Window, System
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(lab03 main.cpp)

target_link_libraries(lab03 PRIVATE SFML::Graphics SFML::Window SFML::System)