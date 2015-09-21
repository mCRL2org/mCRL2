configure_file(include/mcrl2/utilities/toolset_version_const.h.in mcrl2/utilities/toolset_version_const.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mcrl2/utilities/toolset_version_const.h
        DESTINATION ${MCRL2_INCLUDE_PATH}/mcrl2/utilities
        COMPONENT Headers)

add_mcrl2_library(utilities
  INSTALL_HEADERS TRUE
  NOTEST     ## The utilities must also be tested. So, this line ought to be removed.
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
      workarounds_qt.cpp
    DEPENDS
      mcrl2_utilities
      QtCore
      QtGui
      QtWidgets
  )
endif()
