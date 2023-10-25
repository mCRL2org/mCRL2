mcrl2_add_gui_tool(lpsxsim
  MENUNAME "LPSXSim"
  DESCRIPTION "Linear process simulator (mCRL2 toolset)"
  ICON "mcrl2-red"
  SOURCES
    mainwindow.ui
    lpsxsim.qrc
    main.cpp
    mainwindow.cpp
    simulation.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_lts
    mcrl2_gui
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
