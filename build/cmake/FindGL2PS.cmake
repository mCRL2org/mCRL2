include(FindPackageHandleStandardArgs)

find_library(GL2PS_LIBRARY NAMES gl2ps)
find_path(GL2PS_INCLUDE "gl2ps.h")

if(NOT (GL2PS_LIBRARY AND GL2PS_INCLUDE))
  add_subdirectory(${CMAKE_SOURCE_DIR}/3rd-party/gl2ps)
  set(GL2PS_LIBRARY "gl2ps")
  set(GL2PS_INCLUDE ${CMAKE_SOURCE_DIR}/3rd-party/gl2ps/include)
else()
  set(GL2PS_EXTERNAL TRUE)
endif()

set(GL2PS_LIBRARIES ${GL2PS_LIBRARY} )
set(GL2PS_INCLUDE_DIR ${GL2PS_INCLUDE} )

find_package_handle_standard_args(gl2ps DEFAULT_MSG GL2PS_LIBRARY GL2PS_INCLUDE)

mark_as_advanced( GL2PS_LIBRARY GL2PS_INCLUDE )
