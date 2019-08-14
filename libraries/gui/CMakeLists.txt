if(Qt5Core_FOUND AND Qt5Widgets_FOUND)
  add_mcrl2_library(gui
    INSTALL_HEADERS TRUE
    SOURCES
      arcball.cpp
      logwidget.ui
      logwidget.cpp
      extendedtabwidget.cpp
      codeeditor.cpp
      persistentfiledialog.cpp
    DEPENDS
      mcrl2_utilities
      Qt5::Core
      Qt5::Gui
      Qt5::Widgets
      Qt5::OpenGL
      ${OPENGL_LIBRARIES}
    INCLUDE
      ${OPENGL_INCLUDE_DIR}
  )
endif()
