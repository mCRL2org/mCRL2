add_mcrl2_tool(mcrl2-gui
  MENUNAME "mCRL2 GUI"
  DESCRIPTION "Graphical interface for the mCRL2 toolset"
  ICON "mcrl2-orange"
  SOURCES
    mainwindow.ui
    toolinstance.ui
    fileinformation.ui
    filepicker.ui
    copydialog.ui
    mcrl2-gui.qrc
    main.cpp
    mainwindow.cpp
    filebrowser.cpp
    toolaction.cpp
    toolinstance.cpp
    toolcatalog.cpp
    toolinformation.cpp
    fileinformation.cpp
    filepicker.cpp
    copydialog.cpp
    copythread.cpp
    optionvalue.cpp
  DEPENDS
    mcrl2_utilities
    mcrl2_utilities_qt
    QtCore
    QtWidgets
    QtXml
)
add_custom_target(mcrl2-gui-shared
  SOURCES
    share/mcrl2/tool_catalog.xml
)
install(FILES share/mcrl2/tool_catalog.xml DESTINATION ${MCRL2_RESOURCE_PATH})
