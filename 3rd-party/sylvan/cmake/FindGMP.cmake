# FindGMP.cmake
# Provides an imported target GMP::GMP

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_GMP QUIET gmp)
endif()

find_path(GMP_INCLUDE_DIR gmp.h
    HINTS ${PC_GMP_INCLUDEDIR} ${PC_GMP_INCLUDE_DIRS}
)

find_library(GMP_LIBRARY NAMES gmp libgmp
    HINTS ${PC_GMP_LIBDIR} ${PC_GMP_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARY GMP_INCLUDE_DIR)

if(GMP_FOUND AND NOT TARGET GMP::GMP)
    add_library(GMP::GMP UNKNOWN IMPORTED)
    set_target_properties(GMP::GMP PROPERTIES
        IMPORTED_LOCATION "${GMP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)