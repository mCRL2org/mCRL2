mcrl2_add_gui_tool(mcrl2xi
  MENUNAME "mCRL2 XI"
  DESCRIPTION "Graphical editor/interpreter for mCRL2 files"
  ICON "mcrl2-red"
  SOURCES
    mainwindow.ui
    findreplacedialog.ui
    icons/icons.qrc
    mcrl2xi.qrc
    main.cpp
    mainwindow.cpp
    documentmanager.cpp
    documentwidget.cpp
    parser.cpp
    rewriter.cpp
    solver.cpp
    findreplacedialog.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_gui
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
