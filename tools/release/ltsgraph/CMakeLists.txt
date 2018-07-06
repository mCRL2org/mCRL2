if(WIN32)
  add_definitions(-D__STDC_VERSION__=199901L)
endif()

add_mcrl2_tool(ltsgraph
  MENUNAME "LTSGraph"
  DESCRIPTION "Visualisation tool for small state spaces (mCRL2 toolset)"
  ICON "mcrl2-green"
  SOURCES
    mainwindow.ui
    glwidget.ui
    springlayout.ui
    information.ui
    dimensionsdialog.ui
    ltsgraph.qrc
    main.cpp
    mainwindow.cpp
    glwidget.cpp
    glscene.cpp
    graph.cpp
    springlayout.cpp
    information.cpp
    dimensionsdialog.cpp
  DEPENDS
    mcrl2_lts
    mcrl2_gui
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Widgets
    Qt5::Xml
    ${OPENGL_LIBRARIES}
  INCLUDE
    ${OPENGL_INCLUDE_DIR}
)
