include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# Adds the given flag to the CXX flags in all configurations, if it is accepted by the compiler.
function(mcrl2_add_cxx_flag FLAG)
  string(REPLACE "=" "_" FLAG_ACCEPTED  "CXX_${FLAG}_ACCEPTED")
  check_cxx_compiler_flag(${FLAG} ${FLAG_ACCEPTED})

  if(${FLAG_ACCEPTED})
    # This is not really nice, but here we keep track of jittyc compile flags.
    set(MCRL2_JITTYC_ARGUMENTS "${FLAG} ${MCRL2_JITTYC_ARGUMENTS}" CACHE INTERNAL "")

    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_cxx_debug_flag FLAG)
  string(REPLACE "=" "_" FLAG_ACCEPTED  "CXX_DEBUG_${FLAG}_ACCEPTED")
  check_cxx_compiler_flag(${FLAG} FLAG_ACCEPTED)

  if(${FLAG_ACCEPTED})
    add_compile_options($<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_c_flag FLAG)
  string(REPLACE "=" "_" FLAG_ACCEPTED  "C_${FLAG}_ACCEPTED")
  check_c_compiler_flag(${FLAG} ${FLAG_ACCEPTED})

  if(${FLAG_ACCEPTED})
    add_compile_options($<$<COMPILE_LANGUAGE:C>:${FLAG}>)
  endif()
endfunction()

function(mcrl2_add_c_debug_flag FLAG)
  string(REPLACE "=" "_" FLAG_ACCEPTED "C_DEBUG_${FLAG}_ACCEPTED")
  check_c_compiler_flag(${FLAG} ${FLAG_ACCEPTED})

  if(${FLAG_ACCEPTED})
    add_compile_options($<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:${FLAG}>)
  endif()
endfunction()

# Adds linker flags whenever they are supported by the compiler.
function(mcrl2_add_link_options FLAGS)
  check_cxx_compiler_flag(${FLAGS} LINKER_${FLAGS}_ACCEPTED)

  if(LINKER_${FLAGS}_ACCEPTED)
    add_compile_options(${FLAGS})
  endif()
endfunction()

# Adds linker options for the Debug configuration.
function(mcrl2_add_debug_link_options FLAGS)
  check_cxx_compiler_flag(${FLAGS} LINKER_${FLAGS}_ACCEPTED)

  if(LINKER_${FLAGS}_ACCEPTED)
    add_compile_options($<$<CONFIG:Debug>:${FLAGS}>)
  endif()
endfunction()

# Adds compile definitions for the Debug configuration.
function(add_debug_compile_definitions FLAGS)
  add_compile_definitions($<$<CONFIG:Debug>:${FLAGS}>)
endfunction()
