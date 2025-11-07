cmake_minimum_required(VERSION 3.13)

project(lace
    VERSION 1.5.1
    DESCRIPTION "Lace, a work-stealing framework for multi-core fork-join parallelism"
    HOMEPAGE_URL "https://github.com/trolando/lace"
    LANGUAGES C CXX
)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(LACE_PIE_TIMES "Let Lace record Pie times" OFF)
option(LACE_COUNT_TASKS "Let Lace record the number of tasks" OFF)
option(LACE_COUNT_STEALS "Let Lace count #steals and #leaps" OFF)
option(LACE_COUNT_SPLITS "Let Lace count #splits" OFF)
option(LACE_USE_HWLOC "Let Lace pin threads/memory using libhwloc" OFF)
option(LACE_USE_MMAP "Let Lace use mmap to allocate memory" ON)

if(LACE_USE_MMAP)
    include(CheckSymbolExists)
    check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
    if(NOT HAVE_MMAP)
        message(WARNING " mmap not found: disabling mmap support")
	set(LACE_USE_MMAP OFF)
    endif()
endif()

add_library(lace STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lace.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lace.h
)
add_library(lace14 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lace14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lace14.h
)

add_library(lace::lace ALIAS lace)
add_library(lace::lace14 ALIAS lace14)

target_compile_features(lace PUBLIC c_std_11)
target_compile_features(lace14 PUBLIC c_std_11)

target_include_directories(lace
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(lace14
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lace PUBLIC pthread)
target_link_libraries(lace14 PUBLIC pthread)

if(LACE_USE_HWLOC)
    find_package(HWLOC REQUIRED)
    target_link_libraries(lace PUBLIC ${HWLOC_LIBRARIES})
    target_link_libraries(lace14 PUBLIC ${HWLOC_LIBRARIES})
endif()

configure_file(src/lace_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/lace_config.h @ONLY)