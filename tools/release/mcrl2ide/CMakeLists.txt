mcrl2_add_gui_tool(mcrl2ide
  MENUNAME "mCRL2 IDE"
  DESCRIPTION "A graphical IDE for creating, analyzing, visualizing and verifying mCRL2 specifications"
  ICON "mcrl2-red"
  SOURCES
    addeditpropertydialog.cpp
    addeditpropertydialog.ui
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
    rewriteexpressiondock.cpp
    rewriteexpressiondock.ui
    tooloptionsdialog.cpp
    tooloptionsdialog.ui
    utilities.cpp
  DEPENDS
    mcrl2_gui
    mcrl2_lts
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
)

if(TARGET mcrl2ide)
  add_dependencies(mcrl2ide mcrl22lps mcrl2i lpsxsim lps2lts ltsconvert ltscompare ltsgraph lps2pbes pbessolve mcrl2i)
endif()
