cmake_minimum_required(VERSION 2.8.11)

project(stackvm C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_compile_options("--std=c++11")
add_compile_options("-Wall")
add_compile_options("-Werror")

add_subdirectory(stackvm)
