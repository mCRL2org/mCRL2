add_library(sylvan STATIC)
add_library(sylvan::sylvan ALIAS sylvan)

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
  PUBLIC
    sylvan.h
    sylvan_bdd.h
    sylvan_bdd.hpp
    sylvan_cache.h
    sylvan_config.h
    sylvan_common.h
    sylvan_hash.h
    sylvan_int.h
    sylvan_ldd.h
    sylvan_ldd.hpp
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

target_include_directories(sylvan PUBLIC .)
target_compile_options(sylvan PRIVATE -Wno-pedantic)

option(SYLVAN_USE_MMAP "Let Sylvan use mmap to allocate (virtual) memory" ON)
if(SYLVAN_USE_MMAP)
    include(CheckSymbolExists)
    check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
    if(NOT HAVE_MMAP)
        message(WARNING " mmap not found: disabling mmap support")
        set(SYLVAN_USE_MMAP OFF)
    else()
        set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "SYLVAN_USE_MMAP")
    endif()
endif()

# Do we want to collect BDD statistics?
option(SYLVAN_STATS "Let Sylvan collect statistics at runtime" OFF)
if(SYLVAN_STATS)
    set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "SYLVAN_STATS")
endif()
