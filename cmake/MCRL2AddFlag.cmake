include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# Adds the given flag to the CXX flags in all configurations, if it is accepted by the compiler.
function(mcrl2_add_cxx_flag FLAG)
  check_cxx_compiler_flag(${FLAG} CXX_${FLAG}_ACCEPTED)

  if(CXX_${FLAG}_ACCEPTED)
    # This is not really nice, but here we keep track of jittyc compile flags.
    set(MCRL2_JITTYC_ARGUMENTS "${FLAG} ${MCRL2_JITTYC_ARGUMENTS}" CACHE INTERNAL "")

    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_cxx_debug_flag FLAG)
  check_cxx_compiler_flag(${FLAG} CXX_${FLAG}_ACCEPTED)

  if(CXX_${FLAG}_ACCEPTED)
    add_compile_options($<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_c_flag FLAG)
  check_c_compiler_flag(${FLAG} C_${FLAG}_ACCEPTED)

  if(C_${FLAG}_ACCEPTED)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_c_debug_flag FLAG)
  check_c_compiler_flag(${FLAG} C_${FLAG}_ACCEPTED)

  if(C_${FLAG}_ACCEPTED)
    add_compile_options($<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_link_options FLAGS)
  check_cxx_compiler_flag(${FLAGS} LINKER_${FLAGS}_ACCEPTED)

  if(LINKER_${FLAGS}_ACCEPTED)
    add_compile_options(${FLAGS})
  endif()
endfunction()

function(mcrl2_add_debug_link_options FLAGS)
  check_cxx_compiler_flag(${FLAGS} LINKER_${FLAGS}_ACCEPTED)

  if(LINKER_${FLAGS}_ACCEPTED)
    add_compile_options($<$<CONFIG:Debug>:${FLAGS}>)
  endif()
endfunction()