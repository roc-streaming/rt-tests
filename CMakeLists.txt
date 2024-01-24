cmake_minimum_required(VERSION 3.5)

project(rt-tests)

include("cmake/define_option.cmake")
include("cmake/download_googletest.cmake")

define_option(ROC_INCLUDE_DIR "" STRING "roc toolkit include directory")
define_option(ROC_LIB_DIR "" STRING "roc toolkit library directory")
define_option(DOWNLOAD_ROC ON BOOL "automatically download and build roc-toolkit")
define_option(ROC_BRANCH "master" STRING "roc-toolkit branch")

if(DOWNLOAD_ROC)
  include("cmake/download_roc.cmake")
else()
  add_custom_target(roc_lib) # empty target in this case
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

# serialize dependencies
set(ALL_DEPENDENCIES
  googletest_lib
  roc_lib
)
set(OTHER_DEPENDENCIES ${ALL_DEPENDENCIES})
foreach(DEPENDENCY IN LISTS ALL_DEPENDENCIES)
  list(REMOVE_ITEM OTHER_DEPENDENCIES ${DEPENDENCY})
  if(OTHER_DEPENDENCIES)
    add_dependencies(${DEPENDENCY}
      ${OTHER_DEPENDENCIES}
      )
  endif()
endforeach()

set(EXECUTABLE_OUTPUT_PATH
  ${PROJECT_SOURCE_DIR}/bin
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

add_compile_options(
  -pthread
  )
add_link_options(
  -pthread
  )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(
    -fno-omit-frame-pointer
    -funwind-tables
    -ggdb
  )
  add_compile_options(
    -fsanitize=address
    -fsanitize=undefined
  )
  add_link_options(
    -fsanitize=address
    -fsanitize=undefined
  )
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(
    -Werror
    -Wall
    -Wextra
    # enable
    -Wcast-qual
    -Wctor-dtor-privacy
    -Wdouble-promotion
    -Wfloat-conversion
    -Wformat-security
    -Wformat=2
    -Wlogical-op
    -Wmissing-declarations
    -Woverlength-strings
    -Wpointer-arith
    -Wsign-conversion
    -Wstrict-null-sentinel
    -Wuninitialized
    # disable
    -Wno-old-style-cast
    -Wno-psabi
    -Wno-system-headers
    )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
      -D_GLIBCXX_ASSERTIONS
      -D_GLIBCXX_DEBUG
      -D_GLIBCXX_SANITIZE_VECTOR
      )
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(
    -Werror
    -Wall
    -Wextra
    # enable
    -Wcast-qual
    -Wdouble-promotion
    -Wfloat-conversion
    -Wformat-security
    -Wformat=2
    -Wnull-dereference
    -Woverlength-strings
    -Woverloaded-virtual
    -Wpointer-arith
    -Wsign-conversion
    -Wuninitialized
    -Wunused
    # disable
    -Wno-cast-align
    -Wno-system-headers
    -Wno-unused-private-field
    )
endif()

add_compile_definitions(
  _USE_MATH_DEFINES
)

add_executable(rt-tests
  main.cpp
  tests/test_service_quality.cpp
)

add_dependencies(rt-tests
  ${ALL_DEPENDENCIES}
)

find_package(Threads)

target_link_libraries(rt-tests
  roc
  gtest
  gtest_main
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

if(NOT CMAKE_CROSSCOMPILING)
  add_custom_command(
    COMMENT "Copying compile_commands.json to project root"
    DEPENDS rt-tests
    OUTPUT ${PROJECT_SOURCE_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
      ${PROJECT_SOURCE_DIR}/compile_commands.json
  )
  add_custom_target(compile_commands ALL
    DEPENDS ${PROJECT_SOURCE_DIR}/compile_commands.json
  )
endif()
