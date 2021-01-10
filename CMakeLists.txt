cmake_minimum_required(VERSION 3.10)

project(ga VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(ga
	"include/ga/core/ga.h"
	"src/core/ga.cpp" 
)

add_executable(test_ga
	"test/test_ga.cpp"
)
