include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# Check whether VALUE is contained in LISTSTR, which is a space-separated list
# If it is present, VAR is set to TRUE when the macro finishes
macro(list_contains VAR VALUE LISTSTR)
  set(${VAR})
  string(REGEX MATCHALL "[^ ]+" LISTVAR "${LISTSTR}")
  foreach(OTHER IN LISTS LISTVAR)
    if("${VALUE}" STREQUAL "${OTHER}")
      set(${VAR} TRUE)
    endif()
  endforeach()
endmacro()

# Convert a flag into the proper format for check_c_compiler_flag
# Example: -Wunused-variable becomes CXX_ACCEPTS_WUNUSED_VARIABLE
macro(flagvar FLAG FLAGNAME PREFIX)
  string(REPLACE "-" "_" RESULT "${FLAG}")
  string(REPLACE "/" "_" RESULT "${RESULT}")
  string(REPLACE "," "_" RESULT "${RESULT}")
  string(REPLACE "=" "_" RESULT "${RESULT}")
  string(REPLACE "+" "P" RESULT "${RESULT}")
  string(TOUPPER "${PREFIX}_ACCEPTS${RESULT}" ${FLAGNAME})
endmacro()

# Try to add a flag to the build for C files. This macro first checks whether
# the compiler supports that flag and whether the flag was already present.
# The macro supports an additional argument to specify flags that a flag should
# only be added in a specific build mode.
macro(try_add_c_flag FLAG)
  flagvar(${FLAG} FLAGNAME C)
  # Check whether the compiler supports the flag
  check_c_compiler_flag(${FLAG} ${FLAGNAME})
  if("${ARGV1}" STREQUAL "")
    # Use the variable that stores flags for all builds
    set(FLAGSNAME "CMAKE_C_FLAGS")
  else()
    # Use the variable that stores flags for the ${ARGV1} build
    set(FLAGSNAME "CMAKE_C_FLAGS_${ARGV1}")
  endif()
  # Check whether the flag is already present in CMAKE_C_FLAGS (or its build-
  # specific variant).
  list_contains(ALREADY_PRESENT ${FLAG} "${${FLAGSNAME}}")
  if("${${FLAGNAME}}" AND NOT ALREADY_PRESENT)
    # Append the flag to the list of flags
    set(${FLAGSNAME} "${${FLAGSNAME}} ${FLAG}")
  endif()
endmacro()

# Try to add a flag to the build for C++ files. This macro first checks whether
# the compiler supports that flag and whether the flag was already present.
# The macro supports an additional argument to specify flags that a flag should
# only be added in a specific build mode.
macro(try_add_cxx_flag FLAG)
  flagvar(${FLAG} FLAGNAME CXX)
  # Check whether the compiler supports the flag
  check_cxx_compiler_flag(${FLAG} ${FLAGNAME})
  if("${ARGV1}" STREQUAL "")
    # Use the variable that stores flags for all builds
    set(FLAGSNAME "CMAKE_CXX_FLAGS")
  else()
    # Use the variable that stores flags for the ${ARGV1} build
    set(FLAGSNAME "CMAKE_CXX_FLAGS_${ARGV1}")
  endif()
  # Check whether the flag is already present in CMAKE_C_FLAGS (or its build-
  # specific variant).
  list_contains(ALREADY_PRESENT ${FLAG} "${${FLAGSNAME}}")
  if("${${FLAGNAME}}" AND NOT ALREADY_PRESENT)
    # Append the flag to the list of flags
    set(${FLAGSNAME} "${${FLAGSNAME}} ${FLAG}")
  endif()
endmacro()
