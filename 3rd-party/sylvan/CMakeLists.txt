cmake_minimum_required(VERSION 3.17)

project(sylvan
    VERSION 1.9.1
    DESCRIPTION "Sylvan, a parallel decision diagram library"
    HOMEPAGE_URL "https://github.com/trolando/sylvan"
    LANGUAGES C CXX
)

add_library(sylvan STATIC)
add_library(sylvan::sylvan ALIAS sylvan)

string(COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    sylvan_is_top_level)

# Add the sources for Sylvan
add_subdirectory(src)