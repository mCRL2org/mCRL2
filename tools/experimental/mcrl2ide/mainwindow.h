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

#include "filesystem.h"
#include "processsystem.h"
#include "propertiesdock.h"
#include "consoledock.h"
#include "rewritedock.h"
#include "solvedock.h"
#include "addeditpropertydialog.h"
#include "findandreplacedialog.h"
#include "codeeditor.h"

#include <QMainWindow>

class FileSystem;

/**
 * @brief The MainWindow class defines the main window off the application
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
  /**
   * @brief MainWindow Constructor
   * @param parent The parent of this widget
   */
  MainWindow(QWidget* parent = 0);
  ~MainWindow();

  public slots:
  /**
   * @brief actionNewProject Allows the user to create a new project
   * @param askToSave Whether the user should be asked to save before creating a
   *   new project if the specification has been modified
   */
  void actionNewProject(bool askToSave = true);

  /**
   * @brief actionOpenProject Allows the user to open a project
   */
  void actionOpenProject();

  /**
   * @brief actionNewProject Allows the user to open an example project
   */
  void actionOpenExampleProject();

  /**
   * @brief actionNewProject Allows the user to save a project
   */
  void actionSaveProject();

  /**
   * @brief actionNewProject Allows the user to save a new project under a new
   *   name
   */
  void actionSaveProjectAs();

  /**
   * @brief actionAddProperty Allows the user to add a property
   */
  void actionAddProperty();

  /**
   * @brief actionAddPropertyResult Handles the result of adding a property
   */
  void actionAddPropertyResult();

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
   * @brief actionCreateLts Allows the user to create the lts of the current
   * specification
   */
  void actionCreateLts();

  /**
   * @brief actionCreateReducedLts Allows the user to create a reduced lts of
   *   the current specification
   */
  void actionCreateReducedLts();

  /**
   * @brief actionAbortLtsCreation Allows the user to abort lts creation
   */
  void actionAbortLtsCreation();

  /**
   * @brief actionVerifyAllProperties Allows the user to verify all defined
   *   properties on the current specification
   */
  void actionVerifyAllProperties();

  /**
   * @brief actionAbortVerification Allows the user to abort verification
   */
  void actionAbortVerification();

  /**
   * @brief setDocksToDefault Puts all docks in their default location
   */
  void setDocksToDefault();

  protected:
  /**
   * @brief closeEvent On closing the main window, asks to save if there are
   *   changes
   */
  void closeEvent(QCloseEvent* event) override;

  private:
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

  QAction* newProjectAction;
  QAction* openProjectAction;
  QAction* openExampleProjectAction;
  QAction* saveProjectAction;
  QAction* saveProjectAsAction;
  QAction* addPropertyAction;

  QAction* undoAction;
  QAction* redoAction;
  QAction* findAndReplaceAction;
  QAction* cutAction;
  QAction* copyAction;
  QAction* pasteAction;
  QAction* deleteAction;
  QAction* selectAllAction;

  QAction* parseAction;
  QAction* simulateAction;
  QAction* createLtsAction;
  QAction* createReducedLtsAction;
  QAction* abortLtsCreationAction;
  QAction* verifyAllPropertiesAction;
  QAction* abortVerificationAction;

  QMenu* viewMenu;
  QToolBar* toolbar;
  CodeEditor* specificationEditor;
  PropertiesDock* propertiesDock;
  ConsoleDock* consoleDock;
  RewriteDock* rewriteDock;
  SolveDock* solveDock;

  FindAndReplaceDialog* findAndReplaceDialog;
  AddEditPropertyDialog* addPropertyDialog;

  FileSystem* fileSystem;
  ProcessSystem* processSystem;

  int ltsCreationProcessid;
  int simulationProcessid;
};

#endif // MAINWINDOW_H
