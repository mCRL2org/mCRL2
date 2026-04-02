# ── Library target ────────────────────────────────────────────────────────────

# Respects BUILD_SHARED_LIBS (default OFF, set in root CMakeLists.txt)
add_library(sylvan)
add_library(sylvan::sylvan ALIAS sylvan)

# ── Sources ──────────────────────────────────────────────────────────────────

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
    sylvan_refs.c
    sylvan_sl.c
    sylvan_stats.c
    sylvan_table.c
    sylvan_zdd.c
    ${SYLVAN_HDRS}
)

# ── Optional C++ bindings ────────────────────────────────────────────────────
 
if(SYLVAN_BUILD_CPP)
    list(APPEND SYLVAN_HDRS sylvan_obj.hpp)
    target_sources(sylvan PRIVATE sylvan_obj.cpp sylvan_obj.hpp)
endif()

# ── Optional GMP support ────────────────────────────────────────────────────
 
option(SYLVAN_GMP "Include custom MTBDD type GMP" OFF)
if(SYLVAN_GMP)
    find_package(GMP REQUIRED)
    list(APPEND SYLVAN_HDRS sylvan_gmp.h)
    target_sources(sylvan PRIVATE sylvan_gmp.c sylvan_gmp.h)
    target_link_libraries(sylvan PUBLIC GMP::GMP)
endif()

# ── Compile features ────────────────────────────────────────────────────────
 
target_compile_features(sylvan PUBLIC c_std_11)
if(SYLVAN_BUILD_CPP)
    target_compile_features(sylvan PUBLIC cxx_std_11)
endif()

# ── Target properties ───────────────────────────────────────────────────────
 
# PIC: required for shared libraries, optional for static (e.g. embedding
# Sylvan in a downstream shared library).
if(BUILD_SHARED_LIBS OR SYLVAN_ENABLE_PIC)
    set_target_properties(sylvan PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
 
# Shared library versioning (ignored for static builds)
if(BUILD_SHARED_LIBS)
    set_target_properties(sylvan PROPERTIES
        VERSION   ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# Maurice: I don't know what this does, but removing it avoids warning.
# set_target_properties(sylvan PROPERTIES PUBLIC_HEADER "${SYLVAN_HDRS}")

# ── Compiler flags ──────────────────────────────────────────────────────────
 
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(sylvan PRIVATE
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra -Wpedantic -pipe>
        $<$<CONFIG:Release>:-pipe>
        $<$<BOOL:${SYLVAN_NATIVE_OPT}>:-march=native>
    )
elseif(MSVC)
    target_compile_options(sylvan PRIVATE
        $<$<CONFIG:Debug>:/Od /W4 /Zi>
    )
endif()

# ── Include directories ─────────────────────────────────────────────────────
 
target_include_directories(sylvan PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:include>
)

# ── Link dependencies ───────────────────────────────────────────────────────
 
find_package(Threads REQUIRED)
target_link_libraries(sylvan PUBLIC Threads::Threads)
target_link_libraries(sylvan PUBLIC lace::lace)
if(NOT WIN32)
    target_link_libraries(sylvan PUBLIC m)
endif()

# ── Platform feature detection ───────────────────────────────────────────────
 
option(SYLVAN_USE_MMAP "Let Sylvan use mmap to allocate (virtual) memory" ON)
if(SYLVAN_USE_MMAP)
    include(CheckSymbolExists)
    check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
    if(NOT HAVE_MMAP)
        message(WARNING "mmap not found: disabling mmap support")
        set(SYLVAN_USE_MMAP OFF)
    else()
        target_compile_definitions(sylvan PUBLIC SYLVAN_USE_MMAP)
    endif()
endif()

# ── Optional runtime statistics ──────────────────────────────────────────────
 
option(SYLVAN_STATS "Let Sylvan collect statistics at runtime" OFF)
if(SYLVAN_STATS)
    target_compile_definitions(sylvan PUBLIC SYLVAN_STATS)
endif()

# ── Hide tunables when consumed as a subproject ──────────────────────────────

if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
        SYLVAN_GMP
        SYLVAN_ENABLE_PIC
        SYLVAN_NATIVE_OPT
        SYLVAN_USE_MMAP
        SYLVAN_STATS
    )
endif()

# ── Installation ─────────────────────────────────────────────────────────────
 
include(GNUInstallDirs)
 
install(TARGETS sylvan
    EXPORT sylvan-targets
    ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
 
install(EXPORT sylvan-targets
    FILE      sylvan-targets.cmake
    NAMESPACE sylvan::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
)

# ── CMake package config ─────────────────────────────────────────────────────
 
include(CMakePackageConfigHelpers)
 
if(SYLVAN_GMP)
    set(SYLVAN_CONFIG_FIND_GMP "find_dependency(GMP REQUIRED)")
else()
    set(SYLVAN_CONFIG_FIND_GMP "")
endif()
 
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/sylvan-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/sylvan-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
)
 
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/sylvan-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)
 
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/sylvan-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/sylvan-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
)
 
if(SYLVAN_GMP)
    install(FILES ${PROJECT_SOURCE_DIR}/cmake/FindGMP.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
    )
endif()

# ── pkg-config ───────────────────────────────────────────────────────────────
 
if(SYLVAN_GMP)
    set(PKGC_REQUIRES "lace >= 1.6.0 gmp")
else()
    set(PKGC_REQUIRES "lace >= 1.6.0")
endif()
 
# -lm is needed by consumers for static linking; Threads are already pulled
# in transitively via the lace.pc Requires line.
set(PKGC_LIBS_PRIVATE "")
if(NOT WIN32 AND NOT APPLE)
    set(PKGC_LIBS_PRIVATE "-lm")
endif()
 
if(NOT MSVC)
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/sylvan.pc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/sylvan.pc"
        @ONLY
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sylvan.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()
