if(NOT (UNIX AND NOT WIN32 AND NOT APPLE))
  return()
endif()

option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON) 
option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" ON)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/build/packaging/icons/hicolor DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons)