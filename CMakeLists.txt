cmake_minimum_required(VERSION 2.8.11)

project(stackvm C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Boost
set(BOOST_ROOT $ENV{BOOST_DIR})
find_package(Boost 1.55.0 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

# CppUnit
find_package(CppUnit REQUIRED)
include_directories(${CPPUNIT_INCLUDE_DIR})

# compile flags
add_compile_options("--std=c++11")
add_compile_options("-Wall")
add_compile_options("-Werror")

# directories
add_subdirectory(src)
add_subdirectory(stackvm)
add_subdirectory(tst)