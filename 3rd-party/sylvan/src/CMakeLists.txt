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

if(SYLVAN_BUILD_CPP)
    list(APPEND SYLVAN_HDRS sylvan_obj.hpp)
    target_sources(sylvan PRIVATE sylvan_obj.cpp sylvan_obj.hpp)
endif()

option(SYLVAN_GMP "Include custom MTBDD type GMP")
if(SYLVAN_GMP)
    # We only want to include the custom MTBDD type GMP if we actually have the GMP library
    find_package(GMP REQUIRED)
    set(SYLVAN_HDRS ${SYLVAN_HDRS} sylvan_gmp.h)
    target_sources(sylvan PRIVATE sylvan_gmp.c sylvan_gmp.h)
    target_include_directories(sylvan PRIVATE ${GMP_INCLUDE_DIR})
    target_link_libraries(sylvan PUBLIC ${GMP_LIBRARIES})
endif()

set_target_properties(sylvan PROPERTIES VERSION ${sylvan_VERSION} SOVERSION ${sylvan_VERSION_MAJOR})
set_target_properties(sylvan PROPERTIES PUBLIC_HEADER "${SYLVAN_HDRS}")

option(SYLVAN_ENABLE_PIC "Build Sylvan with -fPIC (e.g. for linking into shared libs)" OFF)
set_target_properties(sylvan PROPERTIES POSITION_INDEPENDENT_CODE ${SYLVAN_ENABLE_PIC})

target_compile_features(sylvan PUBLIC c_std_11 cxx_std_11)

option(SYLVAN_NATIVE_OPT "Enable -march=native for compiler optimizations" OFF)

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(sylvan PRIVATE
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra -pipe>
        $<$<CONFIG:Release>:-pipe>
        $<$<BOOL:${SYLVAN_NATIVE_OPT}>:-march=native>
    )
elseif(MSVC)
    target_compile_options(sylvan PRIVATE
        $<$<CONFIG:Debug>:/Od /Wall /Zi>
    )
endif()

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

if(NOT SYLVAN_IS_TOP_PROJECT)
    mark_as_advanced(SYLVAN_GMP SYLVAN_ENABLE_PIC SYLVAN_NATIVE_OPT SYLVAN_USE_MMAP SYLVAN_STATS)
endif()

set_target_properties(sylvan PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)

include(GNUInstallDirs)

install(TARGETS sylvan
    EXPORT sylvan-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT sylvan-targets
    FILE sylvan-targets.cmake
    NAMESPACE sylvan::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
)

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
    VERSION ${SYLVAN_VERSION}
    COMPATIBILITY SameMinorVersion
)

# Compute PKG_CONFIG requires line
if(SYLVAN_GMP)
    set(PKGC_REQUIRES "lace >= 1.4.2 gmp")
else()
    set(PKGC_REQUIRES "lace >= 1.4.2")
endif()

set(PKGC_LIBS_PRIVATE "")
list(APPEND PKGC_LIBS_PRIVATE -lpthread)
if(NOT APPLE)
    list(APPEND PKGC_LIBS_PRIVATE -lm)
endif()

# Join into space-separated string
string(REPLACE ";" " " PKGC_LIBS_PRIVATE "${PKGC_LIBS_PRIVATE}")

install(CODE "
  file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig\")
  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig/sylvan.pc\" \"
prefix=\${CMAKE_INSTALL_PREFIX}
exec_prefix=\\\${prefix}
libdir=\\\${prefix}/${CMAKE_INSTALL_LIBDIR}
includedir=\\\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}

Name: ${PROJECT_NAME}
Description: ${PROJECT_DESCRIPTION}
URL: ${PROJECT_HOMEPAGE_URL}
Version: ${PROJECT_VERSION}
Cflags: -I\\\${includedir}
Libs: -L\\\${libdir} -lsylvan
Libs.private: ${PKGC_LIBS_PRIVATE}
Requires: ${PKGC_REQUIRES}
\")
")
    
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/sylvan-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/sylvan-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
)

if(SYLVAN_GMP)
    install(FILES ${PROJECT_SOURCE_DIR}/cmake/FindGMP.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan)
endif()

