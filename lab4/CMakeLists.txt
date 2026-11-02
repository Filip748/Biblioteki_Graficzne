cmake_minimum_required(VERSION 3.16)
project(GFK_Lab04)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE AND EXISTS "/opt/homebrew/bin/wx-config")
    set(wxWidgets_CONFIG_EXECUTABLE "/opt/homebrew/bin/wx-config" CACHE FILEPATH "wx-config path")
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(program main.cpp)
target_link_libraries(program PRIVATE ${wxWidgets_LIBRARIES})