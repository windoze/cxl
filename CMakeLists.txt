project(cxxstuff)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -ftemplate-depth=256")
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)
IF(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
ENDIF(NOT CMAKE_BUILD_TYPE)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SRC_LIST "${CMAKE_SOURCE_DIR}/test/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS})
