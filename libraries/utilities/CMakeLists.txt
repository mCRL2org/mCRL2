configure_file(include/mcrl2/utilities/toolset_version_const.h.in mcrl2/utilities/toolset_version_const.h)

add_mcrl2_library(utilities
  INSTALL_HEADERS TRUE
  SOURCES
    command_line_interface.cpp
    logger.cpp
    text_utility.cpp
    toolset_version.cpp
)

if(Qt5Core_FOUND AND Qt5Widgets_FOUND)
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
endif()
