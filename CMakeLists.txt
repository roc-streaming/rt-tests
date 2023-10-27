cmake_minimum_required(VERSION 3.0.0)

project(rt-tests)

include(ExternalProject)

ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           release-1.11.0
  GIT_SHALLOW       ON
  SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
  BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
  LOG_DOWNLOAD      ON
  LOG_BUILD         ON
)

include(FetchContent)

FetchContent_Declare(
  subprocess
  GIT_REPOSITORY https://github.com/arun11299/cpp-subprocess.git
  GIT_TAG master
)

FetchContent_MakeAvailable(subprocess)

include(FetchContent)

FetchContent_Declare(
  libsndfile
  GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(libsndfile)

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include" 
)

link_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/googletest-build/lib"
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
  tests/command_line.cpp
)

add_dependencies(rt-tests googletest)

find_package(Threads)

target_link_libraries(rt-tests
  roc
  gtest
  gtest_main
  subprocess
  sndfile
  ${CMAKE_THREAD_LIBS_INIT})

add_custom_command(TARGET rt-tests POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
     "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
     "${PROJECT_SOURCE_DIR}/compile_commands.json"
     COMMENT "Copying compile_commands.json to project root")
