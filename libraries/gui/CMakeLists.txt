if(MCRL2_ENABLE_GUI_TOOLS)

  mcrl2_add_library(mcrl2_gui
    INSTALL_HEADERS TRUE
    SOURCES
      source/arcball.cpp
      source/logwidget.ui
      source/logwidget.cpp
      source/extendedtabwidget.cpp
      source/codeeditor.cpp
      source/persistentfiledialog.cpp
      source/utilities.cpp
    DEPENDS
      mcrl2_utilities
      Qt6::Core
      Qt6::Gui
      Qt6::Widgets
      Qt6::OpenGL
      Opengl::Opengl
  )

endif()
