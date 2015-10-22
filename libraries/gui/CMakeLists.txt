if(Qt5Core_FOUND AND Qt5Widgets_FOUND)
  add_mcrl2_library(gui
    INSTALL_HEADERS TRUE
    SOURCES
      logwidget.ui
      logwidget.cpp
      extendedtabwidget.cpp
      numberedtextedit.cpp
      persistentfiledialog.cpp
      workarounds_qt.cpp
    DEPENDS
      mcrl2_gui
      mcrl2_utilities
      QtCore
      QtGui
      QtWidgets
  )
endif()
