add_mcrl2_tool(mcrl2xi
  MENUNAME "mCRL2 XI"
  DESCRIPTION "Graphical editor/interpreter for mCRL2 files"
  ICON "mcrl2-red"
  SOURCES
    mainwindow.ui
    findreplacedialog.ui
    main.cpp
    mainwindow.cpp
    highlighter.cpp
    documentmanager.cpp
    documentwidget.cpp
    parser.cpp
    rewriter.cpp
    solver.cpp
    findreplacedialog.cpp
  DEPENDS
    mcrl2_lps
    mcrl2_utilities_qt
    QtCore
    QtWidgets
)
