if(MCRL2_ENABLE_GUI_TOOLS)

  add_mcrl2_library(gui
    INSTALL_HEADERS TRUE
    SOURCES
      arcball.cpp
      logwidget.ui
      logwidget.cpp
      extendedtabwidget.cpp
      codeeditor.cpp
      persistentfiledialog.cpp
      utilities.cpp
    DEPENDS
      mcrl2_utilities
      Qt6::Core
      Qt6::Gui
      Qt6::Widgets
      Qt6::OpenGL
      ${OPENGL_LIBRARIES}
    INCLUDE
      ${OPENGL_INCLUDE_DIR}
  )

endif()
