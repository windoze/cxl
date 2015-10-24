project(cxxstuff)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)
IF(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
ENDIF(NOT CMAKE_BUILD_TYPE)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SRC_LIST "${CMAKE_SOURCE_DIR}/test/*.[ch]pp")
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS})
