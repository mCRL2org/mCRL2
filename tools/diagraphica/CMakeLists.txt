add_mcrl2_tool(diagraphica
  MENUNAME "Diagraphica"
  DESCRIPTION "Advanced state space visualisation tool from the mCRL2 toolset."
  ICON "mcrl2-green"
  SOURCES
    icons.qrc
    dofdialog.ui
    mainwindow.ui
    settingsdialog.ui
    arcdiagram.cpp
    attrdiscr.cpp
    attribute.cpp
    bundle.cpp
    cluster.cpp
    colorchooser.cpp
    combnplot.cpp
    corrlplot.cpp
    diagram.cpp
    diagrameditor.cpp
    distrplot.cpp
    dof.cpp
    dofdialog.cpp
    examiner.cpp
    graph.cpp
    main.cpp
    mainwindow.cpp
    movabletablewidget.cpp
    node.cpp
    parser.cpp
    settingsdialog.cpp
    shape.cpp
    simulator.cpp
    timeseries.cpp
    utils.cpp
    visualizer.cpp
    visutils.cpp
  DEPENDS
    mcrl2_atermpp      HEADER_ONLY
    mcrl2_core         HEADER_ONLY
    mcrl2_data         HEADER_ONLY
    mcrl2_lps
    mcrl2_lts
    mcrl2_utilities
    mcrl2_utilities_qt
    QtWidgets
    QtCore
    QtOpenGL
    QtXml
    ${OPENGL_LIBRARIES}
  INCLUDE
    ${OPENGL_INCLUDE_DIR}
)
