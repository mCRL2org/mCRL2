cmake_minimum_required(VERSION 3.23...3.31)

project(sylvan
    VERSION 1.11.0
    DESCRIPTION "Sylvan, a parallel decision diagram library"
    HOMEPAGE_URL "https://github.com/trolando/sylvan"
    LANGUAGES C
)

# Default to Release for top-level single-config builds
if(sylvan_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Generate compile_commands.json for clangd / IDE integration
if(sylvan_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Add CMake modules path
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# ── Options ──────────────────────────────────────────────────────────────────

option(BUILD_SHARED_LIBS          "Build shared libraries (except on Windows)"       OFF)
option(SYLVAN_NATIVE_OPT          "Enable -march=native for compiler optimizations" OFF)
option(SYLVAN_ENABLE_PIC          "Build with -fPIC (for embedding in shared libs)" OFF)
option(SYLVAN_BUILD_CPP           "Build Sylvan C++ bindings"                        ON)
option(BUILD_TESTING              "Build Sylvan tests"          ${PROJECT_IS_TOP_LEVEL})
option(SYLVAN_BUILD_EXAMPLES      "Build Sylvan examples"       ${PROJECT_IS_TOP_LEVEL})
option(SYLVAN_BUILD_DOCS          "Build Sylvan documentation"                      OFF)

# Enable C++ only for the optional bindings and their test/example
if(SYLVAN_BUILD_CPP)
    enable_language(CXX)
endif()

# mCRL2: only register CTest and the Sylvan tests when Sylvan is the top-level
# project; the test directory is not vendored into mCRL2.
if(sylvan_IS_TOP_LEVEL)
    include(CTest)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
    message(STATUS "Sylvan does not currently support Windows DLLs; building a static library")
endif()

# ── Dependencies ─────────────────────────────────────────────────────────────

# Determine if Lace needs PIC (shared Sylvan or explicit PIC request). Sylvan
# is always static on Windows.
set(_sylvan_needs_pic OFF)
if((BUILD_SHARED_LIBS AND NOT WIN32) OR SYLVAN_ENABLE_PIC)
    set(_sylvan_needs_pic ON)
endif()

# First check if Lace is already a target (e.g. a parent project added it),
# then try find_package, then fall back to FetchContent.
if(NOT TARGET lace::lace AND NOT TARGET lace)
    find_package(lace 1.7.0 QUIET)
    if(NOT TARGET lace::lace AND NOT TARGET lace)
        # Normal variables override Lace's options without modifying its cache.
        set(BUILD_SHARED_LIBS OFF)
        if(_sylvan_needs_pic)
            set(LACE_ENABLE_PIC ON)
        endif()

        include(FetchContent)
        FetchContent_Declare(
            lace
            GIT_REPOSITORY https://github.com/trolando/lace.git
            GIT_TAG        v1.7.0
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(lace)

        unset(BUILD_SHARED_LIBS)
        if(_sylvan_needs_pic)
            unset(LACE_ENABLE_PIC)
        endif()
    endif()
endif()

# Installed Lace packages use the namespaced target. Keep compatibility with a
# parent project that provides only the older unnamespaced target.
if(NOT TARGET lace::lace)
    if(TARGET lace)
        add_library(lace::lace ALIAS lace)
    else()
        message(FATAL_ERROR "Lace was found, but it did not provide a lace::lace target")
    endif()
endif()

# Verify PIC compatibility when needed
if(_sylvan_needs_pic)
    get_target_property(_lace_target lace::lace ALIASED_TARGET)
    if(NOT _lace_target)
        set(_lace_target lace::lace)
    endif()
    get_target_property(_lace_imported ${_lace_target} IMPORTED)
    if(NOT _lace_imported)
        get_target_property(_lace_pic ${_lace_target} POSITION_INDEPENDENT_CODE)
        if(NOT _lace_pic)
            message(FATAL_ERROR
                "Sylvan requires PIC (BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}, "
                "SYLVAN_ENABLE_PIC=${SYLVAN_ENABLE_PIC}) but the Lace target "
                "was built without POSITION_INDEPENDENT_CODE. Rebuild Lace with "
                "-DCMAKE_POSITION_INDEPENDENT_CODE=ON or -DLACE_ENABLE_PIC=ON."
            )
        endif()
        unset(_lace_pic)
    endif()
    unset(_lace_imported)
    unset(_lace_target)
endif()

unset(_sylvan_needs_pic)

# ── Sanitizer support (top-level builds only) ────────────────────────────────

if(PROJECT_IS_TOP_LEVEL)
    option(SYLVAN_SANITIZE_ADDRESS "Enable AddressSanitizer"           OFF)
    option(SYLVAN_SANITIZE_THREAD  "Enable ThreadSanitizer"            OFF)
    option(SYLVAN_SANITIZE_UB      "Enable UndefinedBehaviorSanitizer" OFF)
    option(SYLVAN_SANITIZE_MEMORY  "Enable MemorySanitizer"            OFF)

    if(SYLVAN_SANITIZE_THREAD AND SYLVAN_SANITIZE_ADDRESS)
        message(FATAL_ERROR "AddressSanitizer and ThreadSanitizer cannot be combined")
    endif()
    if(SYLVAN_SANITIZE_MEMORY AND SYLVAN_SANITIZE_ADDRESS)
        message(FATAL_ERROR "MemorySanitizer and AddressSanitizer cannot be combined")
    endif()
    if(SYLVAN_SANITIZE_MEMORY AND SYLVAN_SANITIZE_THREAD)
        message(FATAL_ERROR "MemorySanitizer and ThreadSanitizer cannot be combined")
    endif()

    set(_san "")
    if(SYLVAN_SANITIZE_ADDRESS)
        list(APPEND _san "address")
    endif()
    if(SYLVAN_SANITIZE_THREAD)
        list(APPEND _san "thread")
    endif()
    if(SYLVAN_SANITIZE_UB)
        list(APPEND _san "undefined")
    endif()
    if(SYLVAN_SANITIZE_MEMORY)
        list(APPEND _san "memory")
    endif()

    if(_san)
        if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            message(FATAL_ERROR "Sanitizers are not supported with compiler '${CMAKE_C_COMPILER_ID}'")
        endif()
        string(REPLACE ";" "," _san_list "${_san}")
        add_library(sylvan_sanitizers INTERFACE)
        target_compile_options(sylvan_sanitizers INTERFACE
            "-fsanitize=${_san_list}"
            -fno-omit-frame-pointer
        )
        target_link_options(sylvan_sanitizers INTERFACE "-fsanitize=${_san_list}")
        message(STATUS "Sanitizers enabled: ${_san_list}")
        unset(_san_list)
    endif()
    unset(_san)
endif()

# ── Hide options when consumed as a subproject ───────────────────────────────

if(NOT sylvan_IS_TOP_LEVEL)
    mark_as_advanced(
        BUILD_SHARED_LIBS
        SYLVAN_ENABLE_PIC
        SYLVAN_BUILD_CPP
        SYLVAN_BUILD_EXAMPLES
        SYLVAN_BUILD_DOCS
    )
endif()

# ── Subdirectories ───────────────────────────────────────────────────────────

add_subdirectory(src)

if(TARGET sylvan_sanitizers)
    target_link_libraries(sylvan PUBLIC "$<BUILD_INTERFACE:sylvan_sanitizers>")
endif()

if(BUILD_TESTING AND sylvan_IS_TOP_LEVEL)
    add_subdirectory(test)
endif()

if(SYLVAN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SYLVAN_BUILD_DOCS)
    configure_file("docs/conf.py.in" "docs/conf.py" @ONLY)
    find_package(Sphinx REQUIRED)
    Sphinx_add_targets(sylvan
        ${CMAKE_CURRENT_BINARY_DIR}/docs
        ${CMAKE_CURRENT_SOURCE_DIR}/docs
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_target(update_gh_pages
        COMMAND "${CMAKE_COMMAND}" -P "${PROJECT_SOURCE_DIR}/cmake/UpdateGHPages.cmake"
    )
    add_dependencies(update_gh_pages sylvan_html)
endif()

