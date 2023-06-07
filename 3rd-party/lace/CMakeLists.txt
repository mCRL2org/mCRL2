project(lace
    VERSION 1.3.1
    DESCRIPTION "Lace, a work-stealing framework for multi-core fork-join parallelism"
    HOMEPAGE_URL "https://github.com/trolando/lace"
    LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(CheckSymbolExists)

option(LACE_PIE_TIMES "Let Lace record Pie times" OFF)
option(LACE_COUNT_TASKS "Let Lace record the number of tasks" OFF)
option(LACE_COUNT_STEALS "Let Lace count #steals and #leaps" OFF)
option(LACE_COUNT_SPLITS "Let Lace count #splits" OFF)
option(LACE_USE_HWLOC "Let Lace pin threads/memory using libhwloc" OFF)
option(LACE_USE_MMAP "Let Lace use mmap to allocate memory" ON)

mark_as_advanced(LACE_PIE_TIMES
    LACE_COUNT_TASKS 
    LACE_COUNT_STEALS 
    LACE_COUNT_SPLITS
    LACE_USE_HWLOC
    LACE_USE_MMAP)

if(LACE_USE_MMAP)
    check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
    if(NOT HAVE_MMAP)
        message(WARNING " mmap not found: disabling mmap support")
	set(LACE_USE_MMAP OFF)
    endif()
endif()

add_library(lace STATIC)
add_library(lace::lace ALIAS lace)

target_sources(lace PRIVATE src/lace.c PUBLIC src/lace.h)
target_include_directories(lace PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(lace PRIVATE c_std_11)
target_link_libraries(lace PUBLIC pthread)

if(LACE_USE_HWLOC)
    find_package(Hwloc REQUIRED)
    target_link_libraries(lace PUBLIC ${HWLOC_LIBRARIES})
endif()

configure_file(src/lace_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/lace_config.h)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(LACE_BUILD_TESTS "Build and run Lace tests" ON)
    option(LACE_BUILD_BENCHMARKS "Build Lace benchmark programs" ON)

    if(LACE_BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()

    if(LACE_BUILD_TESTS)
        # Add test directory and tests
        include(CTest)
        enable_testing()
        add_subdirectory(test)
    endif()
endif()
