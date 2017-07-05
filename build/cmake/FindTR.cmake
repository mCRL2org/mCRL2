include(FindPackageHandleStandardArgs)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rd-party/tr)
set(TR_LIBRARY "tr")
set(TR_INCLUDE ${CMAKE_SOURCE_DIR}/3rd-party/tr/include)

set(TR_LIBRARIES ${TR_LIBRARY})
set(TR_INCLUDE_DIR ${TR_INCLUDE})

find_package_handle_standard_args(TR DEFAULT_MSG TR_LIBRARY TR_INCLUDE)

mark_as_advanced(TR_LIBRARY TR_INCLUDE)

