if(NOT APPLE)
  return()
endif()

set(CMAKE_EXE_LINKER_FLAGS "-framework Carbon ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-framework Carbon -single_module ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "-framework Carbon ${CMAKE_SHARED_LINKER_FLAGS}")

option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON) 
option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" ON)
