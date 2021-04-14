cmake_minimum_required(VERSION 3.10)

project(gaga VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test_gaga "test/test_gaga.cpp")
