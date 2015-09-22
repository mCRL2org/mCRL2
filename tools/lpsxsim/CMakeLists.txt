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
    mcrl2_lps
    mcrl2_gui
    QtCore
    QtGui
    QtWidgets
)
