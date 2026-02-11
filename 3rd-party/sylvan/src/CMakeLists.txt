set(SYLVAN_HDRS
    sylvan.h
    sylvan_bdd.h
    sylvan_cache.h
    sylvan_config.h
    sylvan_common.h
    sylvan_hash.h
    sylvan_int.h
    sylvan_ldd.h
    sylvan_ldd_int.h
    sylvan_mt.h
    sylvan_mtbdd.h
    sylvan_mtbdd_int.h
    sylvan_obj.hpp
    sylvan_stats.h
    sylvan_table.h
    sylvan_tls.h
    sylvan_zdd.h
    sylvan_zdd_int.h
)

target_sources(sylvan
  PRIVATE
    sha2.c
    sylvan_bdd.c
    sylvan_cache.c
    sylvan_common.c
    sylvan_hash.c
    sylvan_ldd.c
    sylvan_mt.c
    sylvan_mtbdd.c
    sylvan_obj.cpp
    sylvan_refs.c
    sylvan_sl.c
    sylvan_stats.c
    sylvan_table.c
    sylvan_zdd.c
    ${SYLVAN_HDRS}
)

set_target_properties(sylvan PROPERTIES VERSION ${sylvan_VERSION} SOVERSION ${sylvan_VERSION_MAJOR})

# For mCRL2 this is necessary since Sylvan is linked into a dynamic library.
set_target_properties(sylvan PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_features(sylvan PUBLIC c_std_11 cxx_std_11)

option(SYLVAN_NATIVE_OPT "Enable -march=native for compiler optimizations" OFF)

target_include_directories(sylvan PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:include>
)

find_package(Threads REQUIRED)
target_link_libraries(sylvan PUBLIC Threads::Threads)
target_link_libraries(sylvan PUBLIC lace::lace)
if(NOT WIN32)
    target_link_libraries(sylvan PUBLIC m)
endif()

option(SYLVAN_USE_MMAP "Let Sylvan use mmap to allocate (virtual) memory" ON)
if(SYLVAN_USE_MMAP)
    include(CheckSymbolExists)
    check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
    if(NOT HAVE_MMAP)
        message(WARNING " mmap not found: disabling mmap support")
        set(SYLVAN_USE_MMAP OFF)
    else()
        target_compile_definitions(sylvan PUBLIC SYLVAN_USE_MMAP)
    endif()
endif()

# Do we want to collect BDD statistics?
option(SYLVAN_STATS "Let Sylvan collect statistics at runtime" OFF)
if(SYLVAN_STATS)
    target_compile_definitions(sylvan PUBLIC SYLVAN_STATS)
endif()

set_target_properties(sylvan PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)