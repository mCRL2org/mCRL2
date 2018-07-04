// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QInputDialog>
#include <QDesktopWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
  specificationEditor = new CodeEditor(this);
  setCentralWidget(specificationEditor);

  fileSystem = new FileSystem(specificationEditor, this);
  processSystem = new ProcessSystem(fileSystem);

  setupMenuBar();
  setupToolbar();
  setupDocks();

  processSystem->setConsoleDock(consoleDock);

  findAndReplaceDialog = new FindAndReplaceDialog(specificationEditor, this);

  /* make saving a project only enabled whenever there are changes */
  saveProjectAction->setEnabled(false);
  connect(fileSystem, SIGNAL(hasChanges(bool)), saveProjectAction,
          SLOT(setEnabled(bool)));

  /* make the tool buttons enabled or disabled depending on whether processes
   *   are running */
  connect(processSystem->getProcessThread(ProcessType::Parsing),
          SIGNAL(isRunning(bool)), parseAction, SLOT(setDisabled(bool)));
  connect(processSystem->getProcessThread(ProcessType::Simulation),
          SIGNAL(isRunning(bool)), simulateAction, SLOT(setDisabled(bool)));
  connect(processSystem->getProcessThread(ProcessType::LtsCreation),
          SIGNAL(isRunning(bool)), createLtsAction, SLOT(setDisabled(bool)));
  connect(processSystem->getProcessThread(ProcessType::LtsCreation),
          SIGNAL(isRunning(bool)), createReducedLtsAction,
          SLOT(setDisabled(bool)));
  connect(processSystem->getProcessThread(ProcessType::LtsCreation),
          SIGNAL(isRunning(bool)), abortLtsCreationAction,
          SLOT(setEnabled(bool)));
  connect(processSystem->getProcessThread(ProcessType::Verification),
          SIGNAL(isRunning(bool)), abortVerificationAction,
          SLOT(setEnabled(bool)));

  /* reset the propertiesdock when the project is saved */
  connect(fileSystem, SIGNAL(changesSaved()), propertiesDock,
          SLOT(resetAllPropertyWidgets()));

  /* set the title of the main window */
  setWindowTitle("mCRL2 IDE - Unnamed project");
  resize(QSize(QDesktopWidget().availableGeometry(this).width() * 0.5,
               QDesktopWidget().availableGeometry(this).height() * 0.75));

  ltsCreationProcessid = -1;
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenuBar()
{
  /* Create the File menu */
  QMenu* fileMenu = menuBar()->addMenu("File");

  newProjectAction =
      fileMenu->addAction("New Project", this, SLOT(actionNewProject()));
  newProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
  newProjectAction->setIcon(QIcon(":/icons/new_project.png"));

  fileMenu->addSeparator();

  openProjectAction =
      fileMenu->addAction("Open Project", this, SLOT(actionOpenProject()));
  openProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  openProjectAction->setIcon(QIcon(":/icons/open_project.png"));

  openExampleProjectAction = fileMenu->addAction(
      "Open Example Project", this, SLOT(actionOpenExampleProject()));

  fileMenu->addSeparator();

  saveProjectAction =
      fileMenu->addAction("Save Project", this, SLOT(actionSaveProject()));
  saveProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  saveProjectAction->setIcon(QIcon(":/icons/save_project.png"));

  saveProjectAsAction =
      fileMenu->addAction("Save Project As", this, SLOT(actionSaveProjectAs()));

  fileMenu->addSeparator();

  addPropertyAction =
      fileMenu->addAction("Add Property", this, SLOT(actionAddProperty()));
  addPropertyAction->setIcon(QIcon(":/icons/add_property.png"));

  /* Create the Edit menu */
  QMenu* editMenu = menuBar()->addMenu("Edit");

  undoAction = editMenu->addAction("Undo", specificationEditor, SLOT(undo()));
  undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));

  redoAction = editMenu->addAction("Redo", specificationEditor, SLOT(redo()));
  redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

  editMenu->addSeparator();

  findAndReplaceAction = editMenu->addAction("Find and Replace", this,
                                             SLOT(actionFindAndReplace()));
  findAndReplaceAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

  editMenu->addSeparator();

  cutAction = editMenu->addAction("Cut", specificationEditor, SLOT(cut()));
  cutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));

  copyAction = editMenu->addAction("Copy", specificationEditor, SLOT(copy()));
  copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));

  pasteAction =
      editMenu->addAction("Paste", specificationEditor, SLOT(paste()));
  pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));

  deleteAction =
      editMenu->addAction("Delete", specificationEditor, SLOT(deleteChar()));
  deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

  selectAllAction =
      editMenu->addAction("Select All", specificationEditor, SLOT(selectAll()));
  selectAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));

  /* Create the View Menu (actions are added in setupDocks())*/
  viewMenu = menuBar()->addMenu("View");

  /* Create the Actions menu */
  QMenu* actionsMenu = menuBar()->addMenu("Actions");

  parseAction = actionsMenu->addAction("Parse", this, SLOT(actionParse()));
  parseAction->setIcon(QIcon(":/icons/parse_correct.png"));

  simulateAction =
      actionsMenu->addAction("Simulate", this, SLOT(actionSimulate()));
  simulateAction->setIcon(QIcon(":/icons/simulate.png"));

  actionsMenu->addSeparator();

  createLtsAction =
      actionsMenu->addAction("Create LTS", this, SLOT(actionCreateLts()));
  createLtsAction->setIcon(QIcon(":/icons/create_LTS.png"));

  createReducedLtsAction = actionsMenu->addAction(
      "Create reduced LTS", this, SLOT(actionCreateReducedLts()));
  createReducedLtsAction->setIcon(QIcon(":/icons/create_reduced_LTS.png"));

  abortLtsCreationAction = actionsMenu->addAction(
      "Abort LTS creation", this, SLOT(actionAbortLtsCreation()));
  abortLtsCreationAction->setIcon(QIcon(":/icons/abort_LTS_creation.png"));

  actionsMenu->addSeparator();

  verifyAllPropertiesAction = actionsMenu->addAction(
      "Verify all Properties", this, SLOT(actionVerifyAllProperties()));
  verifyAllPropertiesAction->setIcon(QIcon(":/icons/verify_all.png"));

  abortVerificationAction = actionsMenu->addAction(
      "Abort verification", this, SLOT(actionAbortVerification()));
  abortVerificationAction->setIcon(QIcon(":/icons/abort_verification.png"));
}

void MainWindow::setupToolbar()
{
  toolbar = addToolBar("Actions");
  toolbar->setIconSize(QSize(48, 48));

  /* create each toolbar item by adding the actions */
  toolbar->addAction(newProjectAction);
  toolbar->addAction(openProjectAction);
  toolbar->addAction(saveProjectAction);
  toolbar->addSeparator();
  toolbar->addAction(parseAction);
  toolbar->addAction(simulateAction);
  toolbar->addSeparator();
  toolbar->addAction(createLtsAction);
  toolbar->addAction(createReducedLtsAction);
  toolbar->addAction(abortLtsCreationAction);
  toolbar->addSeparator();
  toolbar->addAction(addPropertyAction);
  toolbar->addAction(verifyAllPropertiesAction);
  toolbar->addAction(abortVerificationAction);

  /* disable the abort actions since they should only be used when something is
   *   running */
  abortLtsCreationAction->setEnabled(false);
  abortVerificationAction->setEnabled(false);
}

void MainWindow::setDocksToDefault()
{
  addDockWidget(propertiesDock->defaultArea, propertiesDock);
  addDockWidget(consoleDock->defaultArea, consoleDock);
  addDockWidget(rewriteDock->defaultArea, rewriteDock);
  addDockWidget(solveDock->defaultArea, solveDock);

  propertiesDock->setFloating(false);
  consoleDock->setFloating(false);
  rewriteDock->setFloating(false);
  solveDock->setFloating(false);

  propertiesDock->show();
  consoleDock->show();
  rewriteDock->hide();
  solveDock->hide();
}

void MainWindow::setupDocks()
{
  /* instantiate the docks */
  propertiesDock = new PropertiesDock(processSystem, fileSystem, this);
  consoleDock = new ConsoleDock(this);
  rewriteDock = new RewriteDock(this);
  solveDock = new SolveDock(this);

  /* add toggleable option in the view menu for each dock */
  viewMenu->addAction(propertiesDock->toggleViewAction());
  viewMenu->addAction(consoleDock->toggleViewAction());
  viewMenu->addAction(rewriteDock->toggleViewAction());
  viewMenu->addAction(solveDock->toggleViewAction());

  /* place the docks in the default dock layout */
  setDocksToDefault();

  /* add option to view menu to put all docks back to their default layout */
  viewMenu->addSeparator();
  viewMenu->addAction("Revert to default layout", this,
                      SLOT(setDocksToDefault()));
}

void MainWindow::actionNewProject()
{
  QString projectName = fileSystem->newProject();
  if (!projectName.isEmpty())
  {
    setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
  }
}

void MainWindow::actionOpenProject()
{
  Project project = fileSystem->openProject();

  /* if successful, put the properties in the properties dock and set the window
   *   title */
  if (!(project.projectName.isEmpty()))
  {
    propertiesDock->setToNoProperties();
    for (Property* property : project.properties)
    {
      propertiesDock->addProperty(property);
    }
    setWindowTitle(QString("mCRL2 IDE - ").append(project.projectName));
  }
}

void MainWindow::actionOpenExampleProject()
{
  /* Not yet implemented */
}

void MainWindow::actionSaveProject()
{
  QString projectName = fileSystem->saveProject();
  if (!projectName.isEmpty())
  {
    setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
  }
}

void MainWindow::actionSaveProjectAs()
{
  QString projectName = fileSystem->saveProjectAs();
  if (!projectName.isEmpty())
  {
    setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
  }
}

void MainWindow::actionAddProperty()
{
  /* we require a project to be made if no project has been opened */
  if (!fileSystem->projectOpened())
  {
    actionNewProject();
  }

  /* if successful, allow a property to be added */
  if (fileSystem->projectOpened())
  {
    Property* property = fileSystem->newProperty(processSystem);
    if (property != NULL)
    {
      propertiesDock->addProperty(property);
    }
  }
}

void MainWindow::actionFindAndReplace()
{
  if (findAndReplaceDialog->isVisible())
  {
    findAndReplaceDialog->setFocus();
    findAndReplaceDialog->activateWindow();
  }
  else
  {
    findAndReplaceDialog->show();
  }
}

void MainWindow::actionParse()
{
  processSystem->parseSpecification();
}

void MainWindow::actionSimulate()
{
  simulationProcessid = processSystem->simulate();
}

void MainWindow::actionCreateLts()
{
  ltsCreationProcessid = processSystem->createLts(LtsReduction::None);
}

void MainWindow::actionCreateReducedLts()
{
  QStringList reductionNames;
  for (auto const item : LTSREDUCTIONNAMES)
  {
    reductionNames << item.second;
  }

  /* ask the user what reduciton to use */
  bool ok;
  QString reductionName = QInputDialog::getItem(
      this, "Create reduced LTS", "Reduction:", reductionNames, 0, false, &ok,
      Qt::WindowCloseButtonHint);

  /* if user pressed ok, create a reduced lts */
  if (ok)
  {
    LtsReduction reduction = LtsReduction::None;
    for (auto const item : LTSREDUCTIONNAMES)
    {
      if (item.second == reductionName)
      {
        reduction = item.first;
      }
    }

    ltsCreationProcessid = processSystem->createLts(reduction);
  }
}

void MainWindow::actionAbortLtsCreation()
{
  processSystem->abortProcess(ltsCreationProcessid);
}

void MainWindow::actionVerifyAllProperties()
{
  propertiesDock->verifyAllProperties();
}

void MainWindow::actionAbortVerification()
{
  processSystem->abortAllProcesses(ProcessType::Verification);
}
