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
      OpenGL::GL
  )

  if(APPLE)
    # This is a macOS hack to ensure that this framework is being deployed.
    # Even though Qt6::Gui also uses it it was being ignored.
    target_link_libraries(mcrl2_gui PUBLIC Qt6::DBus)
  endif()

  # Enable the CMake build system to automatically run MOC/UIC/RCC on source files that need it.
  set_target_properties(mcrl2_gui PROPERTIES AUTOMOC TRUE)
  set_target_properties(mcrl2_gui PROPERTIES AUTOUIC TRUE)
  set_target_properties(mcrl2_gui PROPERTIES AUTORCC TRUE)
endif()
