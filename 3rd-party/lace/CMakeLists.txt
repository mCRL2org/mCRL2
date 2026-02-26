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

target_compile_options(lace PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pipe -march=native
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra -Wpedantic>>
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od /Wall /Zi>>
)

target_compile_options(lace14 PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pipe -march=native
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra -Wpedantic>>
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od /Wall /Zi>>
)

target_link_libraries(lace PUBLIC pthread)
target_link_libraries(lace14 PUBLIC pthread)

if(LACE_USE_HWLOC)
    find_package(HWLOC REQUIRED)
    target_link_libraries(lace PUBLIC ${HWLOC_LIBRARIES})
    target_link_libraries(lace14 PUBLIC ${HWLOC_LIBRARIES})
endif()

set_target_properties(lace PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)

set_target_properties(lace14 PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)

configure_file(src/lace_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/lace_config.h @ONLY)

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

include(GNUInstallDirs)

install(TARGETS lace lace14
    EXPORT lace-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lace.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lace14.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lace_config.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT lace-targets
    FILE lace-targets.cmake
    NAMESPACE lace::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lace
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lace-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lace
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config-version.cmake
    VERSION ${LACE_VERSION}
    COMPATIBILITY SameMinorVersion
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/lace.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/lace.pc" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/lace14.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/lace14.pc" @ONLY)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lace
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/lace.pc
        ${CMAKE_CURRENT_BINARY_DIR}/lace14.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)   

export(EXPORT lace-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-targets.cmake
    NAMESPACE lace::
)