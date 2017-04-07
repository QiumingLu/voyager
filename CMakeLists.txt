cmake_minimum_required(VERSION 2.8.7)

if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

project (Voyager C CXX)

set(VOYAGER_VERSION_MAJOR 1)
set(VOYAGER_VERSION_MINOR 6)
set(VOYAGER_VERSION_STRING ${VOYAGER_VERSION_MAJOR}.${VOYAGER_VERSION_MINOR})

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "release")
endif()

if(NOT CMAKE_INSTALL_DIR)
  set(CMAKE_INSTALL_DIR "/usr/local")
endif()

option(BUILD_TESTS "Build voyager perftests and unittests" on)
option(BUILD_EXAMPLES "Build voyager examples" on)
option(BUILD_SHARED_LIBS "Build voyager shared libraries" on)

set(CXX_FLAGS
  -g
  -DCHECK_PTHREAD_RETURN_VALUE
  -D_FILE_OFFSET_BITS=64
  -Wall
  -Wextra
  #  -Werror
  -Wconversion
  -Wno-unused-parameter
  -Woverloaded-virtual
  -Wpointer-arith
  -Wwrite-strings
  -Wshadow
  -march=native
  -std=c++11
  -Wno-sign-conversion
  )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  list(APPEND CXX_FLAGS "-rdynamic")
endif()

if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_COMPILER "g++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_COMPILER "clang++")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBARARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(voyager/util)
add_subdirectory(voyager/port)
add_subdirectory(voyager/core)
add_subdirectory(voyager/http)

find_package(Protobuf)
if (PROTOBUF_FOUND)
    add_subdirectory(voyager/rpc)
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(examples/sudoku)
  add_subdirectory(examples/pingpong)
  add_subdirectory(benchmarks)
endif()
