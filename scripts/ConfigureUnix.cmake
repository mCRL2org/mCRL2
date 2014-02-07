if(NOT (UNIX AND NOT WIN32))
  return()
endif()

##---------------------------------------------------
## Set Shared Build  
##---------------------------------------------------
option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON) 

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
