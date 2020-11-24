cmake_minimum_required(VERSION 3.1)

set(ALKO_VERSION 0.0.1 CACHE STRING "LKDS ALKOTest version" FORCE)


project (
    lkds-alkotest
    VERSION ${ALKO_VERSION}
    DESCRIPTION "Working terminal for testing hardware"
    LANGUAGES C CXX
)

include(ExternalProject)

include(cmake/platform.cmake)
include(cmake/deps.cmake)
include(cmake/bundle.cmake)

set(PRE_CONFIGURE_FILE "${CMAKE_SOURCE_DIR}/src/build_info.cc.in")
set(POST_CONFIGURE_FILE "${CMAKE_BINARY_DIR}/src/build_info.cc")
include(cmake/git_watcher.cmake)

include(cmake/CMakeRC.cmake)

add_subdirectory(src)
add_subdirectory(test)
