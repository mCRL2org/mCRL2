if(NOT (UNIX AND NOT APPLE))
  return()
endif()

##---------------------------------------------------
## Set Shared Build  
##---------------------------------------------------
option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON) 

##---------------------------------------------------
## Toggle profile build
##---------------------------------------------------

option(MCRL2_ENABLE_PROFILING "Enable/disable profiling support" OFF)
if( MCRL2_ENABLE_PROFILING )
  add_definitions( -pg )
  set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
endif(MCRL2_ENABLE_PROFILING)

##---------------------------------------------------
## Toggle Man page generation 
##---------------------------------------------------
## Setup man page generation.
## Man pages are generated by individual tools.
##---------------------------------------------------

option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" ON)
message(STATUS "MCRL2_MAN_PAGES: ${MCRL2_MAN_PAGES}")


set(MCRL2_BIN_DIR "bin")
