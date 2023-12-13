cmake_minimum_required(VERSION 3.5)

project(rt-tests)

include(ExternalProject)

ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           release-1.12.1
  GIT_SHALLOW       ON
  SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
  BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
  LOG_DOWNLOAD      ON
  LOG_BUILD         ON
)

include("cmake/define_option.cmake")

define_option(ROC_INCLUDE_DIR "" STRING "roc toolkit include directory")
define_option(ROC_LIB_DIR "" STRING "roc toolkit library directory")
define_option(DOWNLOAD_ROC ON BOOL "automatically download and build roc-toolkit")

if(DOWNLOAD_ROC)
  include("cmake/download_roc.cmake")
else()
  if(NOT ROC_INCLUDE_DIR STREQUAL "")
    get_filename_component(ROC_INCLUDE_DIR "${ROC_INCLUDE_DIR}" ABSOLUTE)
    message(STATUS "Using ROC_INCLUDE_DIR - ${ROC_INCLUDE_DIR}")

    include_directories(SYSTEM "${ROC_INCLUDE_DIR}")
  endif()

  if(NOT ROC_LIB_DIR STREQUAL "")
    get_filename_component(ROC_LIB_DIR "${ROC_LIB_DIR}" ABSOLUTE)
    message(STATUS "Using ROC_LIB_DIR - ${ROC_LIB_DIR}")

    link_directories("${ROC_LIB_DIR}")
  endif()

  link_libraries("roc")
endif()

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include"
  #"${CMAKE_CURRENT_BINARY_DIR}/roc-toolkit-src/src/public_api/include"
)

link_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/googletest-build/lib"
  #"${CMAKE_CURRENT_BINARY_DIR}/roc-toolkit-src/bin/x86_64-pc-linux-gnu/"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Weverything")
endif()

add_executable(rt-tests
  main.cpp
  tests/test_service_quality.cpp
)

add_dependencies(rt-tests googletest roc)

find_package(Threads)

target_link_libraries(rt-tests
  -l:libroc.a
  gtest
  gtest_main
  ${CMAKE_THREAD_LIBS_INIT})

add_custom_command(TARGET rt-tests POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
     "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
     "${PROJECT_SOURCE_DIR}/compile_commands.json"
     COMMENT "Copying compile_commands.json to project root")
