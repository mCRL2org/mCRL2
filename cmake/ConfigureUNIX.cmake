# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(MCRL2AddFlag)

##---------------------------------------------------
## Add option for profiling support
##---------------------------------------------------

option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
if( MCRL2_ENABLE_PROFILING )
  add_compile_options(-pg)
  add_link_options(-pg)
endif()

##---------------------------------------------------
## Enables additional standard library checks.
##---------------------------------------------------

if (MCRL2_ENABLE_STD_CHECKS)
  if (UNIX AND NOT APPLE)
    # Enable libstdc++ debug checks.
    add_compile_definitions(_GLIBCXX_DEBUG=1)
    add_compile_definitions(_GLIBCXX_DEBUG_PEDANTIC=1)
    add_compile_definitions(_GLIBCXX_ASSERTIONS=1)

    if (MCRL2_ENABLE_STD_CHECKS_BACKTRACE)
      add_compile_definitions(_GLIBCXX_DEBUG_BACKTRACE=1)
    endif()
  else()
    # For libc++ (the LLVM standard library, what a naming scheme) there is also a debug mode
    add_compile_definitions(_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG)
    add_compile_definitions(_LIBCPP_ABI_BOUNDED_ITERATORS=1)
    add_compile_definitions(_LIBCPP_ABI_BOUNDED_ITERATORS_IN_VECTOR=1)
    add_compile_definitions(_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STRING=1)
    add_compile_definitions(_LIBCPP_ABI_BOUNDED_UNIQUE_PTR=1)
    add_compile_definitions(_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STD_ARRAY=1)
    add_compile_definitions(_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS=1)    
  endif()
endif()

##---------------------------------------------------
## Set C++ compile flags
##---------------------------------------------------

# Enables various warnings.
mcrl2_add_cxx_flag(-Wall)
mcrl2_add_cxx_flag(-Wno-inline)
mcrl2_add_cxx_flag(-fno-strict-overflow)
mcrl2_add_cxx_flag(-pipe)
mcrl2_add_cxx_flag(-pedantic)

mcrl2_add_cxx_debug_flag(-W)
mcrl2_add_cxx_debug_flag(-Wextra)
mcrl2_add_cxx_debug_flag(-Wunused-variable)
mcrl2_add_cxx_debug_flag(-Wunused-parameter)
mcrl2_add_cxx_debug_flag(-Wunused-function)
mcrl2_add_cxx_debug_flag(-Wunused)
mcrl2_add_cxx_debug_flag(-Wno-system-headers)
mcrl2_add_cxx_debug_flag(-Woverloaded-virtual)
mcrl2_add_cxx_debug_flag(-Wwrite-strings)
mcrl2_add_cxx_debug_flag(-ftrapv)

# Change the thread local storage model to 'initial-exec', which applies additional
# optimisations which assume that the shared library will never be dynamically loaded.
mcrl2_add_cxx_flag(-ftls-model=initial-exec)

# Disable the procedure linkage table, which is used for lazy loading.
mcrl2_add_cxx_flag(-fno-plt)

if(NOT ${MCRL2_IS_CLANG})
  # Disable interposition, which allows inlining external definitions.
  mcrl2_add_cxx_flag(-fno-semantic-interposition)
else()
  # Change to lld as linker.
  if(MCRL2_ENABLE_LLD)
    mcrl2_add_cxx_flag(-fuse-ld=lld)
  endif()
endif()

# This prevents warnings in the dnj bisimulation algorithm.
mcrl2_add_cxx_flag(-Wno-switch)

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_compile_options(-fsanitize=address,undefined,leak)
  mcrl2_add_cxx_flag(-fno-omit-frame-pointer)
  mcrl2_add_cxx_flag(-fsanitize-address-use-after-scope)
  mcrl2_add_cxx_flag(-fno-sanitize-recover=undefined,address)
endif()

if(MCRL2_ENABLE_MEMORYSANITIZER)
  add_compile_options(-fsanitize=memory)
  mcrl2_add_cxx_flag(-fno-omit-frame-pointer)
endif()

if(MCRL2_ENABLE_THREADSANITIZER)
  add_compile_options(-fsanitize=thread)
  mcrl2_add_cxx_flag(-fno-omit-frame-pointer)
endif()

##---------------------------------------------------
## Set linker flags
##---------------------------------------------------

if(APPLE)
  if (NOT DEFINED MCRL2_OSX_STACK_SIZE)
    # Stack size set to 512MB for Mac, which is the maximal size for an arm64 mac. The Maximal size for an x86_64 is 8GB.
    # This number is the number of bytes in Hex
    set (MCRL2_OSX_STACK_SIZE "20000000" CACHE STRING "Maximum stack size allowed on an arm64 Mac in number of bytes in hex (20000000 equals 512MB, for an x86_64 mac this can be 8GB).")
  endif()

  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-stack_size,${MCRL2_OSX_STACK_SIZE}")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
elseif(NOT ${MCRL2_IS_CLANG})
  mcrl2_add_link_options(-Wl,--as-needed)

  mcrl2_add_debug_link_options(-Wl,--warn-unresolved-symbols,--warn-once)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_link_options(-fsanitize=address,undefined,leak)
endif()

if(MCRL2_ENABLE_MEMORYSANITIZER)
  add_link_options(-fsanitize=memory)
endif()

if(MCRL2_ENABLE_THREADSANITIZER)
  add_link_options(-fsanitize=thread)
endif()

##---------------------------------------------------
## Set C compilation flags, see above for documentation
##---------------------------------------------------

mcrl2_add_c_flag(-pipe)
mcrl2_add_c_flag(-ftls-model=initial-exec)
mcrl2_add_c_flag(-fno-plt)
mcrl2_add_c_flag(-ftrapv)

# Explicitly disable some warnings for C third party libraries
if(MCRL2_IS_CLANG)
  mcrl2_add_c_flag(-Wno-c99-extensions)
  mcrl2_add_c_flag(-Wno-gnu-zero-variadic-macro-arguments)
  mcrl2_add_c_flag(-Wno-zero-length-array)
else()
  mcrl2_add_c_flag(-Wno-strict-aliasing)
endif()

if(NOT ${MCRL2_IS_CLANG})
  mcrl2_add_c_flag(-fno-semantic-interposition)
else()
  # Change to lld as linker.
  if(MCRL2_ENABLE_LLD)
    mcrl2_add_c_flag(-fuse-ld=lld)
  endif()
endif()