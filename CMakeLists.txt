cmake_minimum_required(VERSION 2.8.11)
project(stackvm C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options("-Wall")
add_compile_options("-Werror")
add_compile_options("--std=gnu11")

add_executable(stackvm main.c)
