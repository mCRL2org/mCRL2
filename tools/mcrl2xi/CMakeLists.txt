add_mcrl2_tool(mcrl2xi
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
    mcrl2_atermpp      HEADER_ONLY
    mcrl2_core         HEADER_ONLY
    mcrl2_data         HEADER_ONLY
    mcrl2_lps
    mcrl2_process      HEADER_ONLY
    mcrl2_utilities    HEADER_ONLY
    mcrl2_utilities_qt
    QtCore
    QtWidgets
)
