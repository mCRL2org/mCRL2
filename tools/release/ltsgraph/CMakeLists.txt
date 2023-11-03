mcrl2_add_gui_tool(ltsgraph
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
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
    Qt6::Xml
    mcrl2_gui
    mcrl2_lts
    OpenGL::GL
)
