add_mcrl2_tool(ltsview
  SOURCES
    infodock.ui
    mainwindow.ui
    markdock.ui
    markstateruledialog.ui
    savepicturedialog.ui
    savevectordialog.ui
    settingsdialog.ui
    settingsdock.ui
    simdock.ui
    cluster.cpp
    conedb.cpp
    fsm_state_positioner.cpp
    infodock.cpp
    lts.cpp
    ltscanvas.cpp
    ltsmanager.cpp
    main.cpp
    mainwindow.cpp
    markdock.cpp
    markmanager.cpp
    markstateruledialog.cpp
    primitivefactory.cpp
    primitives.cpp
    rtree.cpp
    savepicturedialog.cpp
    savevectordialog.cpp
    settingsdialog.cpp
    settingsdock.cpp
    simdock.cpp
    simulation.cpp
    sp_state_positioner.cpp
    state.cpp
    transition.cpp
    vectors.cpp
    visobjectfactory.cpp
    visualizer.cpp
  DEPENDS
    mcrl2_atermpp      HEADER_ONLY
    mcrl2_core         HEADER_ONLY
    mcrl2_data         HEADER_ONLY
    mcrl2_lps          HEADER_ONLY
    mcrl2_lts
    mcrl2_utilities    HEADER_ONLY
    mcrl2_utilities_qt
    mcrl2_trace        HEADER_ONLY
    QtCore
    QtOpenGL
    QtWidgets
    QtXml
    ${OPENGL_LIBRARIES}
    ${GL2PS_LIBRARIES}
    ${TR_LIBRARIES}
  INCLUDE
    ${OPENGL_INCLUDE_DIR}
    ${GL2PS_INCLUDE_DIR}
    ${TR_INCLUDE_DIR}
)
