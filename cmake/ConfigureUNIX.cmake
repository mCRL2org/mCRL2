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
## Set C compile flags
##---------------------------------------------------

#try_mcrl2_add_c_flag(-std=c11)
mcrl2_add_c_flag(-Wall)
mcrl2_add_c_flag(-Wno-inline)
mcrl2_add_c_flag(-fno-strict-overflow)
mcrl2_add_c_flag(-pipe)
mcrl2_add_c_debug_flag(-W)

if(MCRL2_ENABLE_LINKER_LLD)
  mcrl2_add_c_flag(-fuse-ld=lld)
endif()
mcrl2_add_c_flag(-ftls-model=initial-exec)
mcrl2_add_c_flag(-fno-plt)
mcrl2_add_c_flag(-fno-semantic-interposition)

if(MCRL2_IS_CLANG)
  # Ignore specific warnings produced in Sylvan.
  mcrl2_add_c_flag(-Wno-c99-extensions)
  mcrl2_add_c_flag(-Wno-gnu-zero-variadic-macro-arguments)
  mcrl2_add_c_flag(-Wno-zero-length-array)
endif()

##---------------------------------------------------
##---------------------------------------------------

if (MCRL2_ENABLE_STD_CHECKS)
  if(MCRL2_IS_CLANG)
    # For libc++ (the LLVM standard library, what a naming scheme) there is also a debug mode
    add_compile_definitions(_LIBCPP_ENABLE_DEBUG_MODE=1)
  else()
    # Enable libstdc++ debug checks and "fortify" mode, when code correctness is optional.
    add_compile_definitions(_GLIBCXX_DEBUG)
    add_compile_definitions(_FORTIFY_SOURCE=3)

    # These debug checks require basic optimisation for some reason, otherwise it gives a warning.
    mcrl2_add_c_debug_flag(-O)
  endif()

endif()

##---------------------------------------------------
## Set C++ compile flags
##---------------------------------------------------

# This first flag is only necessary for our compiling rewriter script (that ignores 'other' build flags).
mcrl2_add_cxx_flag(-Wall)
mcrl2_add_cxx_flag(-Wno-inline)
mcrl2_add_cxx_flag(-fno-strict-overflow)
mcrl2_add_cxx_flag(-pipe)

mcrl2_add_cxx_debug_flag(-W)
mcrl2_add_cxx_debug_flag(-Wextra)
mcrl2_add_cxx_debug_flag(-Wunused-variable)
mcrl2_add_cxx_debug_flag(-Wunused-parameter)
mcrl2_add_cxx_debug_flag(-Wunused-function)
mcrl2_add_cxx_debug_flag(-Wunused)
mcrl2_add_cxx_debug_flag(-Wno-system-headers)
mcrl2_add_cxx_debug_flag(-Woverloaded-virtual)
mcrl2_add_cxx_debug_flag(-Wwrite-strings)

# Change to lld as default linker.
if(MCRL2_ENABLE_LLD)
  mcrl2_add_cxx_flag(-fuse-ld=lld)
endif()

# Make inline member functions hidden by default, allowing them to not be exported
mcrl2_add_cxx_flag(-fvisibility-inlines-hidden)

# Change the thread local storage model to 'initial-exec', which applies additional
# optimisations which assume that the shared library will never be dynamically loaded.
mcrl2_add_cxx_flag(-ftls-model=initial-exec)

# Disable the procedure linkage table, which is used for lazy loading.
mcrl2_add_cxx_flag(-fno-plt)

# Disable interposition, which allows inlining external definitions.
mcrl2_add_cxx_flag(-fno-semantic-interposition)

# This prevents warnings in the dnj bisimulation algorithm.
mcrl2_add_cxx_flag(-Wno-switch)

# Ignore specific warnings produced in Sylvan in clang.
mcrl2_add_cxx_flag(-Wno-c99-extensions)
mcrl2_add_cxx_flag(-Wno-gnu-zero-variadic-macro-arguments)
mcrl2_add_cxx_flag(-Wno-zero-length-array)

# Under GCC there are too many warnings caused by Sylvan to make this usable.
#mcrl2_add_cxx_flag(-pedantic)

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_compile_options(-fsanitize=address)
  add_compile_options(-fno-omit-frame-pointer)
endif()

if(MCRL2_ENABLE_THREADSANITIZER)
  add_compile_options(-fsanitize=thread)
  add_compile_options(-fno-omit-frame-pointer)
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

  if (NOT DEFINED CMAKE_APPLE_SILICON_PROCESSOR) 
    set(CMAKE_APPLE_SILICON_PROCESSOR "x86_64" CACHE STRING "Select whether the generated code is x86_64 or arm64" FORCE) 
  endif()
  set_property(CACHE CMAKE_APPLE_SILICON_PROCESSOR PROPERTY STRINGS "x86_64" "arm64")
  set(CMAKE_OSX_ARCHITECTURES ${CMAKE_APPLE_SILICON_PROCESSOR} CACHE INTERNAL "" FORCE)
elseif(NOT MCRL2_IS_CLANG)
  mcrl2_add_link_options(-Wl,--as-needed)

  mcrl2_add_debug_link_options(-Wl,--warn-unresolved-symbols,--warn-once)
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_link_options(-fsanitize=address)
endif()

if(MCRL2_ENABLE_THREADSANITIZER)
  add_link_options(-fsanitize=thread)
endif()
