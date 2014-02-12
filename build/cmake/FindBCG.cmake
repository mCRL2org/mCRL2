find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

if(CADP_INSTALL_PATH AND NOT CADP_ARCH)
  execute_process(COMMAND ${CADP_INSTALL_PATH}/com/arch
    OUTPUT_VARIABLE CADP_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CADP_ARCH ${CADP_ARCH} CACHE STRING "CADP architecture")
  mark_as_advanced(CADP_ARCH)
endif()

find_library(BCG_LIB
  NAMES BCG
  HINTS ${CADP_INSTALL_PATH}/bin.${CADP_ARCH})

if(BCG_LIB)
  get_filename_component(BCG_BIN_DIR ${BCG_LIB} PATH)
endif()

find_library(BCG_IO_LIB
  NAMES BCG_IO
  HINTS ${CADP_INSTALL_PATH}/bin.${CADP_ARCH}
        ${CADP_BIN_DIR})

find_path(BCG_INCLUDES
  bcg_user.h
  HINTS ${CADP_INSTALL_PATH}/incl ${BCG_BIN_DIR}/../incl)

mark_as_advanced(BCG_LIB BCG_IO_LIB BCG_INCLUDES)

set(BCG_LIBRARIES ${BCG_LIB} ${BCG_IO_LIB})
set(BCG_INCLUDE_DIRS ${BCG_INCLUDES})

find_package_handle_standard_args(BCG DEFAULT_MSG
  BCG_LIBRARIES BCG_INCLUDE_DIRS)

if(BCG_FOUND)
  set(CADP_INSTALL_PATH ${BCG_BIN_DIR})
endif()
set(CADP_INSTALL_PATH "${CADP_INSTALL_PATH}" CACHE PATH "Path to CADP installation (required for BCG support)")
