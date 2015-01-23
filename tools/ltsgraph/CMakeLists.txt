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
    main.cpp
    mainwindow.cpp
    glwidget.cpp
    glscene.cpp
    glscene_helper.cpp
    graph.cpp
    springlayout.cpp
    information.cpp
    dimensionsdialog.cpp
  DEPENDS
    mcrl2_lts
    mcrl2_utilities_qt
    QtCore
    QtGui
    QtOpenGL
    QtWidgets
    QtXml
    ${OPENGL_LIBRARIES}
    ${GL2PS_LIBRARIES}
  INCLUDE
    ${OPENGL_INCLUDE_DIR}
    ${GL2PS_INCLUDE_DIR}
)
