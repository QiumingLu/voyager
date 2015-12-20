# Install script for directory: /home/ming/GitHub/mirants/util

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/ming/GitHub/mirants/build/debug/debug-install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/ming/GitHub/mirants/build/debug/util/libmirants_util.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mirants/util" TYPE FILE FILES
    "/home/ming/GitHub/mirants/util/timestamp.h"
    "/home/ming/GitHub/mirants/util/status.h"
    "/home/ming/GitHub/mirants/util/fileutil.h"
    "/home/ming/GitHub/mirants/util/time.h"
    "/home/ming/GitHub/mirants/util/types.h"
    "/home/ming/GitHub/mirants/util/logging.h"
    "/home/ming/GitHub/mirants/util/scoped_ptr.h"
    "/home/ming/GitHub/mirants/util/stl_util.h"
    "/home/ming/GitHub/mirants/util/testharness.h"
    "/home/ming/GitHub/mirants/util/slice.h"
    "/home/ming/GitHub/mirants/util/string_util.h"
    "/home/ming/GitHub/mirants/util/logfile.h"
    "/home/ming/GitHub/mirants/util/stringprintf.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/ming/GitHub/mirants/build/debug/util/tests/cmake_install.cmake")

endif()

