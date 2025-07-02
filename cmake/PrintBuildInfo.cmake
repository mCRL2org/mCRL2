# This script simply gathers the information from the configuration procedure
# and presents it to the user.

set(BUILD_TYPE "${CMAKE_BUILD_TYPE} build (")
if(BUILD_SHARED_LIBS)
  set(BUILD_TYPE "${BUILD_TYPE}Shared")
else()
  set(BUILD_TYPE "${BUILD_TYPE}Static")
endif()

if(NOT MCRL2_ENABLE_GUI_TOOLS)
  set(BUILD_TYPE "${BUILD_TYPE}, no GUI tools")
endif()

if(NOT MCRL2_ENABLE_STABLE)
  set(BUILD_TYPE "${BUILD_TYPE}, no stable tools")
endif()

if(MCRL2_ENABLE_EXPERIMENTAL)
  set(BUILD_TYPE "${BUILD_TYPE}, experimental")
endif()

if(MCRL2_ENABLE_DEPRECATED)
  set(BUILD_TYPE "${BUILD_TYPE}, deprecated")
endif()

if(MCRL2_ENABLE_DEVELOPER)
  set(BUILD_TYPE "${BUILD_TYPE}, developer")
endif()

message(STATUS "**")
message(STATUS "** Building mCRL2 ${MCRL2_VERSION} ${BUILD_TYPE})")
message(STATUS "** ")
message(STATUS "** Installing to:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "** CMake version:   ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "** C++ compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "** C compiler:      ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "** Architecture:    ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "**")
message(STATUS "** Boost:  ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.")

if(MCRL2_ENABLE_GUI_TOOLS)
  message(STATUS "** Qt:     QtCore ${Qt6Core_VERSION}, QtOpenGL ${Qt6OpenGL_VERSION}, QtWidgets ${Qt6Widgets_VERSION}, QtGui ${Qt6Gui_VERSION}, QtXml ${Qt6Xml_VERSION}.")
endif()

if(cvc3_FOUND)
  message(STATUS "** CVC3:   ${cvc3_VERSION}.")
endif()

if(BCG_FOUND)
  message(STATUS "** BCG:    found.")
endif()

if(PYTHONINTERP_FOUND)
  message(STATUS "** Python: ${PYTHON_VERSION_STRING}")
endif()

message(STATUS "**")
