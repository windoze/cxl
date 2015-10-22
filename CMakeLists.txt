project(cxxstuff)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)
IF(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
ENDIF(NOT CMAKE_BUILD_TYPE)

set(INPUT_HEADERS "${CMAKE_SOURCE_DIR}/test/main.hpp")
set(GENERATED_HEADER "${CMAKE_SOURCE_DIR}/test/main_reflected.hpp")

add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    DEPENDS ${INPUT_HEADERS}
    COMMAND "/usr/local/bin/python"
    ${CMAKE_SOURCE_DIR}/gen_reflect.py
    -I${CMAKE_SOURCE_DIR}/include
    -o"${GENERATED_HEADER}"
    ${INPUT_HEADERS}
)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SRC_LIST "${CMAKE_SOURCE_DIR}/test/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_LIST} ${GENERATED_HEADER} ${HEADERS} ${INPUT_HEADERS})
