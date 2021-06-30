// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "propertiesdock.h"
#include "findandreplacedialog.h"

#include <QMainWindow>

class FileSystem;

/**
 * @brief The MainWindow class defines the main window of the application
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
  /**
   * @brief MainWindow Constructor
   * @param inputProjectFilePath The file path to a project file to open
   * @param parent The parent of this widget
   */
  explicit MainWindow(const QString& inputProjectFilePath, QWidget* parent = 0);
  ~MainWindow();

  public slots:
  /**
   * @brief setDocksToDefault Puts all docks in their default location
   */
  void setDocksToDefault();

  /**
   * @brief onNewProjectOpened Changes the user interface whenever a new project
   *   has been opened
   */
  void onNewProjectOpened();

  /**
   * @brief onEnterSpecificationOnlyMode Changes the user interface whenever the
   *   IDE enters specification only mode
   */
  void onEnterSpecificationOnlyMode();

  /**
   * @brief actionNewProject Allows the user to create a new project
   */
  void actionNewProject();

  /**
   * @brief actionOpenProject Allows the user to open a project
   * @param inputProjectFilePath An optional project file to open the project of
   */
  void actionOpenProject(const QString& inputProjectFilePath = "");

  /**
   * @brief actionSave Allows the user to save a project or specification
   */
  void actionSave();

  /**
   * @brief actionSaveAs Allows the user to save a project or specification
   *   under a new name
   */
  void actionSaveAs();

  /**
   * @brief actionOpenProjectFolderInExplorer Allows the user to open the
   *   project folder in the native file explorer
   */
  void actionOpenProjectFolderInExplorer();

  /**
   * @brief actionImportProperties Allows the user to import property files to
   *   this project
   */
  void actionImportProperties();

  /**
   * @brief actionOpenMcrl2gui Allows the user to open mcrl2-gui
   */
  void actionOpenMcrl2gui();

  /**
   * @brief actionFindAndReplace Allows the user to find and replace strings in
   *   the text editor
   */
  void actionFindAndReplace();

  /**
   * @brief actionParse Allows the user to parse the current specification
   */
  void actionParse();

  /**
   * @brief actionSimulate Allows the user to simulate the current specification
   */
  void actionSimulate();

  /**
   * @brief actionShowLts Allows the user to create the lts of the current
   *   specification
   */
  void actionShowLts();

  /**
   * @brief actionShowReducedLts Allows the user to create a reduced lts of
   *   the current specification
   */
  void actionShowReducedLts();

  /**
   * @brief actionAddProperty Allows the user to add a property
   */
  void actionAddProperty();

  /**
   * @brief actionVerifyAllProperties Allows the user to verify all defined
   *   properties on the current specification
   */
  void actionVerifyAllProperties();

  /**
   * @brief askToSaveChanges Asks the user whether to save changes, saves the
   *   project if yes was answered
   * context In what context this is asked
   * @return Whether to proceed (true) or to cancel (false)
   */
  bool askToSaveChanges(QString context);

  /**
   * @brief changeToolButtons Whenever the IDE changes between nothing opened,
   *   specification only mode and project mode, change the file actions
   * @param specificationOnlyMode Whether the IDE changed to specification only
   *   mode
   */
  void changeFileButtons(bool specificationOnlyMode);

  /**
   * @brief changeToolButtons Whenever a thread is running, change corresponding
   *   actions from start to abort; vice versa when a thread has stopped running
   * @param toAbort Whether actions should be changed from start to abort or
   *   vice versa (whether a thread is running or not)
   * @param processType The process type of the thread
   */
  void changeToolButtons(bool toAbort, ProcessType processType);

  protected:
  /**
   * @brief event Handles events
   * - On gaining focus, ask to reload the specification if it was modified
   *     outside the IDE
   * - On closing, ask to save if there are changes and do cleanup
   * @return Whether the event is accepted
   */
  bool event(QEvent* event) override;

  private:
  QAction* newProjectAction;
  QAction* openProjectAction;

  QAction* saveAction;
  QString saveProjectText = "Save Project";
  QIcon saveProjectIcon = QIcon(":/icons/save_project.png");
  QString saveSpecificationText = "Save Specification";
  QIcon saveSpecificationIcon = QIcon(":/icons/save_specification.png");

  QAction* saveAsAction;
  QString saveProjectAsText = "Save Project As";
  QString saveSpecificationAsText = "Save Specification As";

  QAction* openProjectFolderInExplorerAction;
  QAction* importPropertiesAction;
  QAction* openGuiAction;
  QAction* exitAction;

  QAction* undoAction;
  QAction* redoAction;
  QAction* findAndReplaceAction;
  QAction* cutAction;
  QAction* copyAction;
  QAction* pasteAction;
  QAction* deleteAction;
  QAction* selectAllAction;

  QAction* zoomInAction;
  QAction* zoomOutAction;

  QAction* parseAction;
  QString parseStartText = "Parse Specification";
  QIcon parseStartIcon = QIcon(":/icons/parse_start.png");
  QString parseAbortText = "Abort Parsing";
  QIcon parseAbortIcon = QIcon(":/icons/parse_abort.png");

  QAction* simulateAction;
  QString simulateStartText = "Simulate Specification";
  QIcon simulateStartIcon = QIcon(":/icons/simulate_start.png");
  QString simulateAbortText = "Abort creating Simulation";
  QIcon simulateAbortIcon = QIcon(":/icons/simulate_abort.png");

  QAction* showLtsAction;
  QString showLtsStartText = "Show State Space";
  QIcon showLtsStartIcon = QIcon(":/icons/create_LTS_start.png");
  QString showLtsAbortText = "Abort State Space generation";
  QIcon showLtsAbortIcon = QIcon(":/icons/create_LTS_abort.png");

  QAction* showReducedLtsAction;
  QString showReducedLtsStartText = "Show reduced State Space";
  QIcon showReducedLtsStartIcon = QIcon(":/icons/create_reduced_LTS_start.png");
  QString showReducedLtsAbortText = "Abort reduced State Space generation";
  QIcon showReducedLtsAbortIcon = QIcon(":/icons/create_reduced_LTS_abort.png");

  QAction* addPropertyAction;

  QAction* verifyAllPropertiesAction;
  QString verifyAllPropertiesStartText = "Verify all Properties";
  QIcon verifyAllPropertiesStartIcon = QIcon(":/icons/verify_all_start.png");
  QString verifyAllPropertiesAbortText = "Abort Verification";
  QIcon verifyAllPropertiesAbortIcon = QIcon(":/icons/verify_all_abort.png");

  QMenu* viewMenu;
  QMenu* saveIntermediateFilesMenu;
  QToolBar* toolbar;
  mcrl2::gui::qt::CodeEditor* specificationEditor;
  PropertiesDock* propertiesDock;
  ConsoleDock* consoleDock;

  FindAndReplaceDialog* findAndReplaceDialog;
  AddEditPropertyDialog* addPropertyDialog;

  FileSystem* fileSystem;
  ProcessSystem* processSystem;

  QSettings* settings;

  bool lastLtsHasReduction;
  bool reloadIsBeingHandled = false;

  /**
   * @brief setupMenuBar Creates the menubar, also creates the actions and their
   *   icons and shortcuts (if applicable)
   */
  void setupMenuBar();

  /**
   * @brief setupToolbar Creates the toolbar
   */
  void setupToolbar();

  /**
   * @brief setupDocks Creates the docks
   */
  void setupDocks();

  /**
   * @brief assertProjectOpened Forces the user to make a project if no project
   *   was opened
   * @return Whether a project has been successfully made
   */
  bool assertProjectOpened();

  /**
   * @brief assertSpecificationOpened Forces the user to make a project if no
   *   specification was opened
   * @return Whether a specification has been successfully made
   */
  bool assertSpecificationOpened();
};

#endif // MAINWINDOW_H
