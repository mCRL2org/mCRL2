# Try to find HWLOC
# Once done this will define:
# - HWLOC_FOUND
# - HWLOC_INCLUDE_DIRS
# - HWLOC_LIBRARIES
# - HWLOC_DEFINITIONS
# And an imported target:
# - HWLOC::hwloc

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_HWLOC QUIET hwloc)
endif()

set(HWLOC_DEFINITIONS ${PC_HWLOC_CFLAGS_OTHER})

find_path(HWLOC_INCLUDE_DIR hwloc.h
  HINTS
    ${PC_HWLOC_INCLUDEDIR}
    ${PC_HWLOC_INCLUDE_DIRS}
)

find_library(HWLOC_LIBRARY NAMES hwloc
  HINTS
    ${PC_HWLOC_LIBDIR}
    ${PC_HWLOC_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HWLOC DEFAULT_MSG HWLOC_LIBRARY HWLOC_INCLUDE_DIR)

if(HWLOC_FOUND)
  set(HWLOC_INCLUDE_DIRS ${HWLOC_INCLUDE_DIR})
  set(HWLOC_LIBRARIES ${HWLOC_LIBRARY})

  if(NOT TARGET HWLOC::hwloc)
    add_library(HWLOC::hwloc UNKNOWN IMPORTED)
    set_target_properties(HWLOC::hwloc PROPERTIES
      IMPORTED_LOCATION "${HWLOC_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${HWLOC_INCLUDE_DIR}"
      INTERFACE_COMPILE_OPTIONS "${HWLOC_DEFINITIONS}"
    )
  endif()
endif()

mark_as_advanced(HWLOC_INCLUDE_DIR HWLOC_LIBRARY)