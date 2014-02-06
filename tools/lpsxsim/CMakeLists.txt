add_mcrl2_tool(lpsxsim
  SOURCES
    mainwindow.ui
    main.cpp
    mainwindow.cpp
    simulation.cpp
  DEPENDS
    mcrl2_atermpp      HEADER_ONLY
    mcrl2_core         HEADER_ONLY
    mcrl2_data         HEADER_ONLY
    mcrl2_lps
    mcrl2_utilities    HEADER_ONLY
    mcrl2_utilities_qt
    QtCore
    QtWidgets
)
