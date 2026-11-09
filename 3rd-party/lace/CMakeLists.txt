cmake_minimum_required(VERSION 3.23...3.31)

project(lace
    VERSION 1.11.0
    DESCRIPTION "Lace, a work-stealing framework for multi-core fork-join parallelism"
    HOMEPAGE_URL "https://github.com/trolando/lace"
    LANGUAGES C
)

# ── Build defaults ───────────────────────────────────────────────────────────

if(lace_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

if(lace_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(lace_VERSION "${PROJECT_VERSION}" CACHE INTERNAL "Lace version")

# ── Options ──────────────────────────────────────────────────────────────────

option(BUILD_SHARED_LIBS "Build shared libraries instead of static"                  OFF)
option(LACE_ENABLE_PIC   "Build Lace with -fPIC (e.g. for linking into shared libs)" OFF)
option(LACE_NATIVE_OPT   "Enable -march=native for compiler optimizations"           ${lace_IS_TOP_LEVEL})
option(LACE_BACKOFF      "Let idle threads sleep if there is no work"                ON)
option(LACE_PIE_TIMES    "Let Lace record Pie times"                                 OFF)
option(LACE_COUNT_TASKS  "Let Lace record the number of tasks"                       OFF)
option(LACE_COUNT_STEALS "Let Lace count #steals and #leaps"                         OFF)
option(LACE_COUNT_SPLITS "Let Lace count #splits"                                    OFF)
option(LACE_HARDENING    "Keep the compiler's stack protector/clash checks in Lace" OFF)
set(LACE_DEFAULT_PIN "AUTO" CACHE STRING "Pinning mode when neither lace_set_pinning() nor LACE_PIN chooses one: AUTO, OFF, CPU, CORE, L2, L3, NUMA or ALL")
set_property(CACHE LACE_DEFAULT_PIN PROPERTY STRINGS AUTO OFF CPU CORE L2 L3 NUMA ALL)
string(TOUPPER "${LACE_DEFAULT_PIN}" LACE_DEFAULT_PIN)
if(NOT LACE_DEFAULT_PIN MATCHES "^(AUTO|OFF|CPU|CORE|L2|L3|NUMA|ALL)$")
    message(FATAL_ERROR "LACE_DEFAULT_PIN must be AUTO, OFF, CPU, CORE, L2, L3, NUMA or ALL, not ${LACE_DEFAULT_PIN}")
endif()
option(LACE_DONT_OVERCOMMIT "Default to 16-64 MB worker stacks instead of reserving 1 GB each" OFF)

if(lace_IS_TOP_LEVEL)
    option(LACE_BUILD_TESTS      "Build and run Lace tests"                          ON)
    option(LACE_BUILD_BENCHMARKS "Build Lace benchmark programs"                     ON)
    option(LACE_SANITIZE_ADDRESS "Enable AddressSanitizer"                           OFF)
    option(LACE_SANITIZE_THREAD  "Enable ThreadSanitizer"                            OFF)
    option(LACE_SANITIZE_UB      "Enable UndefinedBehaviorSanitizer"                 OFF)
endif()

if(BUILD_SHARED_LIBS)
    message(WARNING
        "Building Lace as a shared library is not recommended. PLT indirection "
        "on hot paths hurts performance, and Lace must be instantiated exactly "
        "once per process. Instead, build Lace as a static library with LACE_ENABLE_PIC "
        "and link it into your shared library or executable."
    )
endif()

# ── Platform feature detection ───────────────────────────────────────────────

include(CheckSymbolExists)
include(CMakePushCheckState)
# glibc declares sched_getaffinity and CPU_COUNT only with _GNU_SOURCE (lace.c
# defines it); without it the check fails and Lace counts every online CPU
# instead of the ones in its affinity mask (taskset, cgroups, SLURM jobs).
cmake_push_check_state()
list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
check_symbol_exists(sched_getaffinity "sched.h" HAVE_SCHED_GETAFFINITY)
cmake_pop_check_state()
check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)

if(NOT WIN32 AND NOT HAVE_MMAP)
    message(FATAL_ERROR "mmap not found (required on non-Windows platforms)")
endif()

# ── Dependencies ─────────────────────────────────────────────────────────────

if(NOT MSVC)
    find_package(Threads REQUIRED)
endif()

if(LACE_USE_HWLOC)
    message(WARNING "LACE_USE_HWLOC is no longer supported and is ignored: Lace pins its workers itself on Linux, FreeBSD and Windows")
endif()

# ── Sanitizer support (top-level builds only) ────────────────────────────────

if(lace_IS_TOP_LEVEL)
    if(LACE_SANITIZE_ADDRESS AND LACE_SANITIZE_THREAD)
        message(FATAL_ERROR "AddressSanitizer and ThreadSanitizer cannot be combined")
    endif()

    set(_san "")
    if(LACE_SANITIZE_ADDRESS)
        list(APPEND _san "address")
    endif()
    if(LACE_SANITIZE_THREAD)
        list(APPEND _san "thread")
    endif()
    if(LACE_SANITIZE_UB)
        list(APPEND _san "undefined")
    endif()

    if(_san)
        if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            message(FATAL_ERROR "Sanitizers are not supported with compiler '${CMAKE_C_COMPILER_ID}'")
        endif()
        string(REPLACE ";" "," _san_list "${_san}")
        set(_san_flags "-fsanitize=${_san_list} -fno-omit-frame-pointer")
        string(APPEND CMAKE_C_FLAGS             " ${_san_flags}")
        string(APPEND CMAKE_CXX_FLAGS           " ${_san_flags}")
        string(APPEND CMAKE_EXE_LINKER_FLAGS    " ${_san_flags}")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " ${_san_flags}")
        string(APPEND CMAKE_MODULE_LINKER_FLAGS " ${_san_flags}")
        message(STATUS "Sanitizers enabled: ${_san_list}")
    endif()
    unset(_san)
endif()

# ── Hardening ────────────────────────────────────────────────────────────────
#
# Distribution compilers (Ubuntu, Fedora, ...) enable runtime hardening by
# default: -fstack-protector-strong adds a canary check to every function with
# an address-taken local, -fstack-clash-protection probes large stack frames.
# That guards against attacks on untrusted input, and costs time in the
# scheduler (steal, leapfrog) and in benchmark tasks. Unless LACE_HARDENING is
# set, Lace's own targets (the library and the benchmarks) turn both
# off. The flags are PRIVATE: a program that uses Lace keeps its own flags,
# also for the SPAWN/SYNC code that the task macros put in its functions.
# -fcf-protection stays: mixing objects with and without it would silently
# drop CET for the whole program, and its cost is an endbr64 per function.

# Clang 18 warns about its own -march=native on CPUs with AVX10.1 ("invalid
# feature combination"), which -Werror turns into an error; older Clang does
# not know the warning, hence the check.
set(LACE_NATIVE_FLAGS "")
if(LACE_NATIVE_OPT AND CMAKE_C_COMPILER_ID MATCHES "Clang")
    include(CheckCCompilerFlag)
    set(_saved_flags "${CMAKE_REQUIRED_FLAGS}")
    string(APPEND CMAKE_REQUIRED_FLAGS " -Werror")
    check_c_compiler_flag(-Wno-invalid-feature-combination LACE_HAS_WNO_INVALID_FEATURE_COMBINATION)
    set(CMAKE_REQUIRED_FLAGS "${_saved_flags}")
    unset(_saved_flags)
    if(LACE_HAS_WNO_INVALID_FEATURE_COMBINATION)
        list(APPEND LACE_NATIVE_FLAGS -Wno-invalid-feature-combination)
    endif()
endif()

set(LACE_NO_HARDENING_FLAGS "")
if(NOT LACE_HARDENING)
    if(MSVC)
        list(APPEND LACE_NO_HARDENING_FLAGS /GS-)
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        # -Werror: Clang accepts -fno-stack-clash-protection on targets
        # without stack clash protection (arm64) but warns that it is unused.
        include(CheckCCompilerFlag)
        set(_saved_flags "${CMAKE_REQUIRED_FLAGS}")
        string(APPEND CMAKE_REQUIRED_FLAGS " -Werror")
        foreach(_flag -fno-stack-protector -fno-stack-clash-protection)
            string(MAKE_C_IDENTIFIER "LACE_HAS${_flag}" _var)
            check_c_compiler_flag(${_flag} ${_var})
            if(${_var})
                list(APPEND LACE_NO_HARDENING_FLAGS ${_flag})
            endif()
        endforeach()
        set(CMAKE_REQUIRED_FLAGS "${_saved_flags}")
        unset(_saved_flags)
        unset(_flag)
        unset(_var)
    endif()
endif()

# Does the compiler protect the stack by default? Then programs that use Lace
# pay for it in their tasks unless they turn it off (see README.md).
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    include(CheckCSourceCompiles)
    check_c_source_compiles("
#if !defined(__SSP__) && !defined(__SSP_STRONG__) && !defined(__SSP_ALL__)
#error no stack protector
#endif
int main(void) { return 0; }" LACE_COMPILER_STACK_PROTECTOR)
endif()

# ── Generated header (before targets, so FILE_SET can reference it) ──────────

configure_file(src/lace_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/lace_config.h @ONLY)

# ── Library targets ──────────────────────────────────────────────────────────

foreach(_target lace lace14)

    # ── Sources and public headers ───────────────────────────────────────
    add_library(${_target})
    add_library(lace::${_target} ALIAS ${_target})

    if(_target STREQUAL "lace")
        target_sources(${_target}
          PRIVATE src/lace.c
          PUBLIC  FILE_SET HEADERS
                  BASE_DIRS src
                  FILES src/lace.h
          PUBLIC  FILE_SET generated_headers TYPE HEADERS
                  BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
                  FILES ${CMAKE_CURRENT_BINARY_DIR}/lace_config.h
        )
    else()
        target_sources(${_target}
          PRIVATE src/lace14.c
          PUBLIC  FILE_SET HEADERS
                  BASE_DIRS src
                  FILES src/lace14.h
          PUBLIC  FILE_SET generated_headers TYPE HEADERS
                  BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
                  FILES ${CMAKE_CURRENT_BINARY_DIR}/lace_config.h
        )
    endif()

    # ── Compile features ─────────────────────────────────────────────────
    target_compile_features(${_target} PUBLIC c_std_11)

    # ── PIC ──────────────────────────────────────────────────────────────
    if(BUILD_SHARED_LIBS OR LACE_ENABLE_PIC)
        set_target_properties(${_target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    # ── Compiler flags ───────────────────────────────────────────────────

    # we're adding quite a few extra warnings, to ensure Lace as a library is compliant with the
    # C standard and won't trigger warnings or errors when included in a stricter project

    if(CMAKE_C_COMPILER_ID MATCHES "GNU")
        target_compile_options(${_target} PRIVATE
            -pipe
            -Wall -Wextra -Wpedantic -Werror
            -Wshadow # avoid potential shadowing bugs
            -Wnull-dereference # warn when we could null dereference
            -Wformat=2 # improve format string checking for logging
            -Wstrict-prototypes # functions without parameters must be void
            -Wmissing-prototypes # enforce non-static functions have prior declarations
            -Wconversion # catch implicit narrowing conversions
            -Wsign-conversion # catch implicit changes to signed/unsigned
            $<$<CONFIG:Debug>:-O0>
            $<$<BOOL:${LACE_NATIVE_OPT}>:-march=native>
        )
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_compile_options(${_target} PRIVATE
            -pipe
            -Wall -Wextra -Wpedantic -Werror
            -Wshadow # avoid potential shadowing bugs
            -Wnull-dereference # warn when we could null dereference
            -Wformat=2 # improve format string checking for logging
            -Wstrict-prototypes # functions without parameters must be void
            -Wmissing-prototypes # enforce non-static functions have prior declarations
            -Wconversion # catch implicit narrowing conversions
            -Wsign-conversion # catch implicit changes to signed/unsigned
            -Watomic-implicit-seq-cst # ensure all atomic load/store are explicit
            $<$<CONFIG:Debug>:-O0>
            $<$<BOOL:${LACE_NATIVE_OPT}>:-march=native>
            ${LACE_NATIVE_FLAGS}
        )
    elseif(MSVC)
        target_compile_options(${_target} PUBLIC
            /Zc:__STDC__
            /Zc:preprocessor
            /experimental:c11atomics
        )
        target_compile_options(${_target} PRIVATE
            /W4
            /WX
            /wd4200 # Stop MSVC from complaining about standard C99/C11 code
        )
    endif()

    target_compile_options(${_target} PRIVATE ${LACE_NO_HARDENING_FLAGS})

    # ── Link dependencies ────────────────────────────────────────────────
    if(NOT MSVC)
        target_link_libraries(${_target} PUBLIC Threads::Threads)
    endif()

    if(WIN32)
        target_link_libraries(${_target} PUBLIC synchronization)
    endif()

    # ── Shared library versioning ────────────────────────────────────────
    if(BUILD_SHARED_LIBS)
        set_target_properties(${_target} PROPERTIES
            VERSION   ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()

endforeach()

# ── Tests and benchmarks (top-level only) ────────────────────────────────────

if(lace_IS_TOP_LEVEL)
    enable_language(CXX)

    if(LACE_BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()

    if(LACE_BUILD_TESTS)
        include(CTest)
        enable_testing()
        add_subdirectory(test)
    endif()
endif()

# ── Hide options when consumed as a subproject ───────────────────────────────

if(NOT lace_IS_TOP_LEVEL)
    mark_as_advanced(
        BUILD_SHARED_LIBS
        LACE_ENABLE_PIC
        LACE_NATIVE_OPT
        LACE_BACKOFF
        LACE_PIE_TIMES
        LACE_COUNT_TASKS
        LACE_COUNT_STEALS
        LACE_COUNT_SPLITS
        LACE_DEFAULT_PIN
        LACE_DONT_OVERCOMMIT
    )
endif()

# ── Installation ─────────────────────────────────────────────────────────────

include(GNUInstallDirs)

install(TARGETS lace lace14
    EXPORT lace-targets
    ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT lace-targets
    FILE      lace-targets.cmake
    NAMESPACE lace::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lace
)

# ── CMake package config ─────────────────────────────────────────────────────

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lace-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lace
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/lace-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lace
)

# ── pkg-config ───────────────────────────────────────────────────────────────

set(_PC_EXTRA_CFLAGS "")
set(_PC_EXTRA_LIBS "")

if(MINGW)
    list(APPEND _PC_EXTRA_LIBS "-lsynchronization")
endif()

string(JOIN " " PC_EXTRA_CFLAGS ${_PC_EXTRA_CFLAGS})
string(JOIN " " PC_EXTRA_LIBS ${_PC_EXTRA_LIBS})

if(NOT MSVC)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/lace.pc.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/lace.pc" @ONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/lace14.pc.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/lace14.pc" @ONLY)
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/lace.pc
            ${CMAKE_CURRENT_BINARY_DIR}/lace14.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# ── Summary ──────────────────────────────────────────────────────────────────

# Summary, when Lace is the top-level project
if(lace_IS_TOP_LEVEL)
    message(STATUS "")
    message(STATUS "Lace ${PROJECT_VERSION} configuration summary:")
    message(STATUS "  Build type        : ${CMAKE_BUILD_TYPE}")
    message(STATUS "  Shared libraries  : ${BUILD_SHARED_LIBS}")
    message(STATUS "  Use -march=native : ${LACE_NATIVE_OPT}")
    message(STATUS "  Use -fPIC         : ${LACE_ENABLE_PIC}")
    message(STATUS "  Backoff when idle : ${LACE_BACKOFF}")
    message(STATUS "  Default pinning   : ${LACE_DEFAULT_PIN}")
    message(STATUS "  Don't overcommit  : ${LACE_DONT_OVERCOMMIT}")
    message(STATUS "  Count tasks       : ${LACE_COUNT_TASKS}")
    message(STATUS "  Count steals      : ${LACE_COUNT_STEALS}")
    message(STATUS "  Count splits      : ${LACE_COUNT_SPLITS}")
    message(STATUS "  Record PIE times  : ${LACE_PIE_TIMES}")
    if(LACE_NO_HARDENING_FLAGS)
        string(REPLACE ";" " " _flags "${LACE_NO_HARDENING_FLAGS}")
        message(STATUS "  Hardening         : OFF in Lace's own targets (${_flags})")
        unset(_flags)
    else()
        message(STATUS "  Hardening         : compiler default")
    endif()
    if(LACE_COMPILER_STACK_PROTECTOR)
        message(STATUS "  Note: this compiler enables the stack protector by default; code using")
        message(STATUS "        Lace's task macros keeps your flags (see README.md)")
    endif()
    message(STATUS "  Build tests       : ${LACE_BUILD_TESTS}")
    message(STATUS "  Build benchmarks  : ${LACE_BUILD_BENCHMARKS}")
    message(STATUS "  Sanitize address  : ${LACE_SANITIZE_ADDRESS}")
    message(STATUS "  Sanitize thread   : ${LACE_SANITIZE_THREAD}")
    message(STATUS "  Sanitize UB       : ${LACE_SANITIZE_UB}")
    message(STATUS "")
endif()
