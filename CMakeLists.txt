cmake_minimum_required(VERSION 2.8.12)

if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

project (Voyager C CXX)

set(VOYAGER_VERSION_MAJOR 1)
set(VOYAGER_VERSION_MINOR 0)
set(VOYAGER_VERSION_PATCH 3)
set(VOYAGER_VERSION_STRING ${VOYAGER_VERSION_MAJOR}.${VOYAGER_VERSION_MINOR}.${VOYAGER_VERSION_PATCH})

function(exclude OUTPUT INPUT)
  set(EXCLUDES ${ARGN})
  foreach(EXCLUDE ${EXCLUDES})
    list(REMOVE_ITEM INPUT "${EXCLUDE}")
  endforeach()
  set(${OUTPUT} ${INPUT} PARENT_SCOPE)
endfunction(exclude)

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage." FORCE)
endif()

if(NOT CMAKE_INSTALL_DIR)
  set(CMAKE_INSTALL_DIR "/usr/local")
endif()

option(BUILD_RPC "Build voyager rpc" ON)
option(BUILD_HTTP "Build voyager http" ON)
option(BUILD_TESTS "Build voyager perftests and unittests" ON)
option(BUILD_EXAMPLES "Build voyager examples" ON)
option(BUILD_SHARED_LIBS "Build voyager shared libraries" ON)

set(CXX_FLAGS
  -g
  -Wall
  -Wextra
  -Wconversion
  -Wno-unused-parameter
  -Woverloaded-virtual
  -Wpointer-arith
  -Wwrite-strings
  -Wshadow
  -Wno-sign-conversion
  -std=c++11
  )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBARARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(voyager)

if (BUILD_EXAMPLES)
  add_subdirectory(examples/sudoku)
  add_subdirectory(examples/pingpong)
  add_subdirectory(benchmarks)
endif()
