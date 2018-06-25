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

  /* make the save project action enabled whenever a change is made */
  saveProjectAction->setEnabled(false);
  connect(fileSystem, SIGNAL(hasChanges(bool)), saveProjectAction,
          SLOT(setEnabled(bool)));

  /* make the toolbar buttons enabled or disabled depending on whether processes
   *   are running */
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
  /* ask the user for a project name */
  bool ok;
  QString projectName = QInputDialog::getText(
      this, "New project", "Project name:", QLineEdit::Normal, "", &ok,
      Qt::WindowCloseButtonHint);

  /* if user pressed ok, create the project and save the specification and
   *   properties in it */
  if (ok)
  {
    QString error = fileSystem->newProject(projectName);

    if (error.isEmpty())
    {
      setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
    }
    else
    {
      /* if there was an error, tell the user */
      QMessageBox* msgBox =
          new QMessageBox(QMessageBox::Information, "New Project", error,
                          QMessageBox::Ok, this, Qt::WindowCloseButtonHint);
      msgBox->exec();
    }
  }
}

void MainWindow::actionOpenProject()
{
  QStringList projects = fileSystem->getAllProjects();

  if (projects.isEmpty())
  {
    QMessageBox* msgBox = new QMessageBox(
        QMessageBox::Information, "Open Project", "No projects found",
        QMessageBox::Ok, this, Qt::WindowCloseButtonHint);
    msgBox->exec();
  }
  else
  {
    bool ok;
    QString projectName =
        QInputDialog::getItem(this, "Open Project", "Project name:", projects,
                              0, false, &ok, Qt::WindowCloseButtonHint);

    /* if user pressed ok, open the project by setting the specification and the
     * properties in the window */
    if (ok)
    {
      std::list<Property*> properties = fileSystem->openProject(projectName);
      propertiesDock->setToNoProperties();
      for (Property* property : properties)
      {
        propertiesDock->addProperty(property);
      }
      saveProjectAction->setEnabled(false);
      setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
    }
  }
}

void MainWindow::actionOpenExampleProject()
{
  /* Not yet implemented */
}

bool MainWindow::actionSaveProject()
{
  /* if we have a project open, we have a location to save in so we can simply
   *   save, else use save as */
  if (fileSystem->projectOpened())
  {
    fileSystem->saveSpecification();
    propertiesDock->saveAllProperties();
    saveProjectAction->setEnabled(false);
    return true;
  }
  else
  {
    return actionSaveProjectAs();
  }
}

bool MainWindow::actionSaveProjectAs()
{
  /* ask the user for a project name */
  bool ok;
  QString projectName = QInputDialog::getText(
      this, "Save Project As", "Project name:", QLineEdit::Normal, "", &ok,
      Qt::WindowCloseButtonHint);

  /* if user pressed ok, create the project and save the specification and
   *   properties in it */
  if (ok)
  {
    QString error = fileSystem->newProject(projectName);
    if (error.isEmpty())
    {
      setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
      return actionSaveProject();
    }
    else
    {
      /* if there was an error, tell the user */
      QMessageBox* msgBox =
          new QMessageBox(QMessageBox::Information, "Save Project As", error,
                          QMessageBox::Ok, this, Qt::WindowCloseButtonHint);
      msgBox->exec();
    }
  }
  return false;
}

void MainWindow::actionAddProperty()
{
  addPropertyDialog = new AddEditPropertyDialog(true, propertiesDock, this);
  /* if adding was succesful (Add button was pressed), add the property to the
   *   properties dock */
  if (addPropertyDialog->exec())
  {
    Property* property = new Property(addPropertyDialog->getPropertyName(),
                                      addPropertyDialog->getPropertyText());
    propertiesDock->addProperty(property);
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
  /* Not yet implemented */
}

void MainWindow::actionSimulate()
{
  /* Not yet implemented */
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
      this, "Open Project", "Project name:", reductionNames, 0, false, &ok,
      Qt::WindowCloseButtonHint);

  /* if user pressed ok, create a reduced lts */
  if (ok)
  {
    LtsReduction reduction;
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
