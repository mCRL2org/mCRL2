if(CMAKE_CFG_INTDIR STREQUAL ".")
  set(MCRL2_VERSION_STRING "${MCRL2_VERSION} (${CMAKE_BUILD_TYPE})")
else()
  set(MCRL2_VERSION_STRING "${MCRL2_VERSION} (${CMAKE_CFG_INTDIR})")
endif()
configure_file(include/mcrl2/utilities/toolset_version_const.h.in mcrl2/utilities/toolset_version_const.h)

add_mcrl2_library(utilities
  INSTALL_HEADERS TRUE
  SOURCES
    command_line_interface.cpp
    logger.cpp
    text_utility.cpp
    toolset_version.cpp
)

add_mcrl2_library(utilities_qt
  INSTALL_HEADERS TRUE
  SOURCES
    logwidget.ui
    qt_tool.cpp
    atermthread.cpp
    logwidget.cpp
    extendedtabwidget.cpp
    numberedtextedit.cpp
    persistentfiledialog.cpp
  DEPENDS
    mcrl2_utilities
    QtWidgets
    QtCore
)
