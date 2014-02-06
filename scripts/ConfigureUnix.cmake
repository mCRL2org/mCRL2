if(UNIX AND NOT WINDOWS)
  return()
endif()

##---------------------------------------------------
## Set Shared Build  
##---------------------------------------------------
option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON) 

##---------------------------------------------------
## Add option for profiling support
##---------------------------------------------------

option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
if( MCRL2_ENABLE_PROFILING )
  add_definitions( -pg )
  set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
endif(MCRL2_ENABLE_PROFILING)

##---------------------------------------------------
## Add option for Man page generation
##---------------------------------------------------

option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" ON)
message(STATUS "MCRL2_MAN_PAGES: ${MCRL2_MAN_PAGES}")

##---------------------------------------------------
## Install application icons
##---------------------------------------------------

install(DIRECTORY ${CMAKE_SOURCE_DIR}/build/packaging/icons/hicolor DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons)

set(MCRL2_BIN_DIR "bin")
