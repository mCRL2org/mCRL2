add_mcrl2_tool(ltsgraph
  MENUNAME "LTSGraph"
  DESCRIPTION "Visualisation tool for small state spaces (mCRL2 toolset)"
  ICON "mcrl2-green"
  SOURCES
    camera.cpp
    dimensionsdialog.cpp
    dimensionsdialog.ui
    exportsvg.cpp
    exporttikz.cpp
    glscene.cpp
    glwidget.cpp
    glwidget.ui
    graph.cpp
    information.cpp
    information.ui
    ltsgraph.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    shaders.cpp
    layoututility.cpp
    springlayout.cpp
    settingsmanager.cpp
    springlayout.ui
    advancedspringlayoutdialog.ui
  DEPENDS
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Widgets
    Qt5::Xml
    mcrl2_gui
    mcrl2_lts
    ${OPENGL_LIBRARIES}
  INCLUDE
    ${OPENGL_INCLUDE_DIR}
)
