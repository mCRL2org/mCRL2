add_mcrl2_tool(lpsxsim
  MENUNAME "LPSXSim"
  DESCRIPTION "Linear process simulator (mCRL2 toolset)"
  ICON "mcrl2-red"
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
