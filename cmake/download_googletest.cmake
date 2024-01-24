include(ExternalProject)

ExternalProject_Add(googletest_lib
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           release-1.12.1
  GIT_SHALLOW       ON
  SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
  BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
  LOG_DOWNLOAD      ON
  LOG_BUILD         ON
)

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include"
)

link_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/googletest-build/lib"
)
