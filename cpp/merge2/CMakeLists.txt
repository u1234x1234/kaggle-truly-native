project(parser)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -pthread -msse2 -msse3 -Dfopen64=fopen")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3 -pthread -msse2 -msse3 -mavx -Dfopen64=fopen")

#LINK_DIRECTORIES("/home/linux12341234/native/mat_gen/xgboost-0.32/wrapper")
LINK_DIRECTORIES("/root/native/xgboost/wrapper")
include_directories("/home/linux12341234/libs/vlfeat-0.9.20")
include_directories("/home/linux12341234/stxxl/include")

file(GLOB SRC_LIST main.cpp qwe.cpp)

set(LINK_LIBRARIES
	boost_system
	boost_filesystem
	xgboostwrapper
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(
	${PROJECT_NAME} -Wl,-E
	${LINK_LIBRARIES}
)
