add_mcrl2_tool(mcrl2ide
  MENUNAME "mCRL2 IDE"
  DESCRIPTION "Graphical IDE for creating, analyzing, visualizing and verifying specifications"
  ICON "mcrl2-red"
  SOURCES
    addeditpropertydialog.cpp
    addeditpropertydialog.ui
    codeeditor.cpp
    consoledock.cpp
    filesystem.cpp
    findandreplacedialog.cpp
    findandreplacedialog.ui
    main.cpp
    mainwindow.cpp
    mcrl2-ide.qrc
    processsystem.cpp
    propertiesdock.cpp
    propertywidget.cpp
    rewritedock.cpp
    solvedock.cpp
  DEPENDS
    QtCore
    QtGui
    QtWidgets
    QtXml
)
