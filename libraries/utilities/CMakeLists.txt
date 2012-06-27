# Authors: Frank Stappers and Aad Mathijssen
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# ##################################
# ## mcrl2_utilities ##
# ##################################

project(mcrl2_utilities)
cmake_minimum_required(VERSION 2.6)

# Trick to add header files to projects in IDEs like Visual Studio and XCode
FILE(GLOB HEADERS "include/mcrl2/utilities/*.h")
FILE(GLOB DETAIL_HEADERS "include/mcrl2/utilities/detail/*.h")
add_custom_target(${PROJECT_NAME}_headers
  SOURCES ${HEADERS} ${DETAIL_HEADERS}
)

set(mcrl2_utilities_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/command_line_interface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/toolset_version.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/text_utility.cpp
)

set(mcrl2_utilities_src ${mcrl2_utilities_src}  PARENT_SCOPE)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/source/toolset_version.cpp
  PROPERTIES COMPILE_DEFINITIONS MCRL2_VERSION="${MCRL2_VERSION} \(${CMAKE_BUILD_TYPE}\)"
)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

add_library(${PROJECT_NAME}
  ${mcrl2_utilities_src}
)

target_link_libraries(${PROJECT_NAME}
  ${Boost_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})

# ##############################
# ##    mcrl2_utilities_wx    ##
# ##############################

if(MCRL2_ENABLE_GUI_TOOLS)

project(mcrl2_utilities_wx)
cmake_minimum_required(VERSION 2.6)

set(mcrl2_utilities_wx_src
  ${CMAKE_CURRENT_SOURCE_DIR}/source/spinctrlfloat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/colorbutton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/font_renderer.cpp
)

set(mcrl2_utilities_wx_src ${mcrl2_utilities_wx_src} PARENT_SCOPE)

include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/build/precompile)
include_directories(${CMAKE_SOURCE_DIR}/build/workarounds)

add_library(${PROJECT_NAME}
  ${mcrl2_utilities_wx_src}
)

target_link_libraries(${PROJECT_NAME}
  ${wxWidgets_LIBRARIES}
  ${OPENGL_LIBRARY}
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})



find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtMain)
if (QT4_FOUND)

# ##############################
# ##    mcrl2_utilities_qt    ##
# ##############################

project(mcrl2_utilities_qt)
cmake_minimum_required(VERSION 2.6)


set(mcrl2_utilities_qt_ui
  ${CMAKE_CURRENT_SOURCE_DIR}/source/logwidget.ui
)
qt4_wrap_ui(mcrl2_utilities_qt_ui_h ${mcrl2_utilities_qt_ui})

set(mcrl2_utilities_qt_h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mcrl2/utilities/atermthread.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mcrl2/utilities/logwidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mcrl2/utilities/qt_tool.h
)
qt4_wrap_cpp(mcrl2_utilities_qt_h_moc ${mcrl2_utilities_qt_h})

set(mcrl2_utilities_qt_src
  ${mcrl2_utilities_qt_ui_h}
  ${mcrl2_utilities_qt_h_moc}
  ${CMAKE_CURRENT_SOURCE_DIR}/source/atermthread.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/logwidget.cpp
)

#set(mcrl2_utilities_qt_src ${mcrl2_utilities_qt_src} PARENT_SCOPE)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries/aterm/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/atermpp/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/core/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/data/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lps/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lts/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trace/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${QT_INCLUDES})

add_library(${PROJECT_NAME}
  ${mcrl2_utilities_qt_src}
)

target_link_libraries(${PROJECT_NAME}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  mcrl2_aterm
)

# add install target:
install(TARGETS ${PROJECT_NAME} DESTINATION ${MCRL2_LIB_DIR})


endif(QT4_FOUND)


endif(MCRL2_ENABLE_GUI_TOOLS)
