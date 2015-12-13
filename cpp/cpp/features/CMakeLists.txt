project(parser)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -pthread -msse2 -msse3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3 -pthread -msse2 -msse3 -mavx")

LINK_DIRECTORIES("/home/linux12341234/stxxl/build/lib")
include_directories("/home/linux12341234/libs/vlfeat-0.9.20")
include_directories("/home/linux12341234/stxxl/include")

file(GLOB SRC_LIST main.cpp qwe.cpp)

set(LINK_LIBRARIES
	boost_system
	boost_filesystem
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(
	${PROJECT_NAME} -Wl,-E
	${LINK_LIBRARIES}
)
