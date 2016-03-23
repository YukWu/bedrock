
cmake_minimum_required( VERSION 2.8 )

project( bedrock )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(SOURCE_FILES ThreadPool.cpp)

add_library(bedrock STATIC ${SOURCE_FILES})

target_link_libraries(bedrock pthread)
