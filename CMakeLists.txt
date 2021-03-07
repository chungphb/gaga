cmake_minimum_required(VERSION 3.10)

project(ga VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(GA_HEADERS
	"include/ga/core/ga.h"
	"include/ga/core/model.h"
	"include/ga/core/gene.h"
	"include/ga/core/policy.h"
)
set(GA_SOURCES
	"src/core/ga.cpp"
	"src/core/policy.cpp"
)
add_library(ga ${GA_HEADERS} ${GA_SOURCES})

add_executable(test_ga "test/test_ga.cpp")
target_link_libraries(test_ga PUBLIC ga)