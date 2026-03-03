cmake_minimum_required(VERSION 3.17)

project(sylvan
    VERSION 1.9.3
    DESCRIPTION "Sylvan, a parallel decision diagram library"
    HOMEPAGE_URL "https://github.com/trolando/sylvan"
    LANGUAGES C CXX
)

add_library(sylvan STATIC)
add_library(sylvan::sylvan ALIAS sylvan)

# Check if top level
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(SYLVAN_IS_TOP_LEVEL ON)
else()
    set(SYLVAN_IS_TOP_LEVEL OFF)
endif()

# Default build type
if(SYLVAN_IS_TOP_LEVEL AND NOT CMAKE_CONFIGURATION_TYPES AND CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Add CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# Dependencies
# First check if lace is a target already, for example if we are a subproject,
# and lace is already added.
# Then try to find lace in the OS using find_package.
# If that fails, then download lace.
if(NOT TARGET lace)
  find_package(lace 1.5.1 QUIET)
  if(NOT lace_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        lace
        GIT_REPOSITORY https://github.com/trolando/lace.git
        GIT_TAG        v1.5.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(lace)
  endif()
endif()

option(SYLVAN_BUILD_CPP "Build Sylvan CPP header" ON)
option(SYLVAN_BUILD_TESTS "Build Sylvan tests" ${SYLVAN_IS_TOP_LEVEL})
option(SYLVAN_BUILD_EXAMPLES "Build Sylvan examples" ${SYLVAN_IS_TOP_LEVEL})
option(SYLVAN_BUILD_DOCS "Build Sylvan documentation" OFF)

if(NOT SYLVAN_IS_TOP_LEVEL)
    mark_as_advanced(SYLVAN_BUILD_TESTS SYLVAN_BUILD_EXAMPLES SYLVAN_BUILD_DOCS SYLVAN_BUILD_CPP)
endif()

# Add the sources for Sylvan
add_subdirectory(src)

# Build and add tests
if(SYLVAN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Build examples
if(SYLVAN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Make documentation
if(SYLVAN_BUILD_DOCS)
    configure_file("docs/conf.py.in" "docs/conf.py" @ONLY)
    find_package(Sphinx REQUIRED)
    Sphinx_add_targets(sylvan ${CMAKE_CURRENT_BINARY_DIR}/docs ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_target(update_gh_pages COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/UpdateGHPages.cmake")
    add_dependencies(update_gh_pages sylvan_html)
endif()


