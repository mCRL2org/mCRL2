cmake_minimum_required(VERSION 3.14)

project(sylvan
    VERSION 1.8.0
    DESCRIPTION "Sylvan, a parallel decision diagram library"
    HOMEPAGE_URL "https://github.com/trolando/sylvan"
    LANGUAGES C CXX
)

# Add the Sylvan library target
add_subdirectory(src)