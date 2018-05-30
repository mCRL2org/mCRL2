#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    specificationEditor = new CodeEditor(this, true);
    setCentralWidget(specificationEditor);

    setupMenuBar();
    setupToolbar();
    setupDocks();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenuBar()
{
    /* Create the File menu */
    QMenu *fileMenu = menuBar()->addMenu("File");

    newProjectAction = fileMenu->addAction("New Project", this, &MainWindow::actionNewProject);
    fileMenu->addSeparator();
    openProjectAction = fileMenu->addAction("Open Project", this, &MainWindow::actionOpenProject);
    openExampleProjectAction = fileMenu->addAction("Open Example Project", this, &MainWindow::actionOpenExampleProject);
    fileMenu->addSeparator();
    saveProjectAction = fileMenu->addAction("Save Project", this, &MainWindow::actionSaveProject);
    saveProjectAsAction = fileMenu->addAction("Save Project As", this, &MainWindow::actionSaveProjectAs);
    fileMenu->addSeparator();
    addPropertyAction = fileMenu->addAction("Add Property", this, &MainWindow::actionAddProperty);

    /* Create the Edit menu */
    QMenu *editMenu = menuBar()->addMenu("Edit");

    undoAction = editMenu->addAction("Undo", this, &MainWindow::actionUndo);
    redoAction = editMenu->addAction("Redo", this, &MainWindow::actionRedo);
    editMenu->addSeparator();
    findAndReplaceAction = editMenu->addAction("Find and Replace", this, &MainWindow::actionFindAndReplace);
    editMenu->addSeparator();
    cutAction = editMenu->addAction("Cut", this, &MainWindow::actionCut);
    copyAction = editMenu->addAction("Copy", this, &MainWindow::actionCopy);
    pasteAction = editMenu->addAction("Paste", this, &MainWindow::actionPaste);
    deleteAction = editMenu->addAction("Delete", this, &MainWindow::actionDelete);
    selectAllAction = editMenu->addAction("Select All", this, &MainWindow::actionSelectAll);

    /* Create the View Menu (actions are added in setupDocks())*/
    viewMenu = menuBar()->addMenu("View");

    /* Create the Actions menu */
    QMenu *actionsMenu = menuBar()->addMenu("Actions");

    parseAction = actionsMenu->addAction("Parse", this, &MainWindow::actionParse);
    simulateAction = actionsMenu->addAction("Simulate", this, &MainWindow::actionSimulate);
    actionsMenu->addSeparator();
    createLTSAction = actionsMenu->addAction("Create LTS", this, &MainWindow::actionCreateLTS);
    createReducedLTSAction = actionsMenu->addAction("Create reduced LTS", this, &MainWindow::actionCreateReducedLTS);
    abortLTSCreationAction = actionsMenu->addAction("Abort LTS creation", this, &MainWindow::actionAbortLTSCreation);
    actionsMenu->addSeparator();
    verifyAllPropertiesAction = actionsMenu->addAction("Verify all Properties", this, &MainWindow::actionVerifyAllProperties);
    abortVerificationAction = actionsMenu->addAction("Abort verification", this, &MainWindow::actionAbortVerification);
}

void MainWindow::setupToolbar()
{
    toolbar = addToolBar("Actions");
    toolbar->setIconSize(QSize(48, 48));

    /* create each toolbar item by adding an icon and an action */
    newProjectAction->setIcon(QIcon(":/icons/new_project.png"));
    toolbar->addAction(newProjectAction);

    openProjectAction->setIcon(QIcon(":/icons/open_project.png"));
    toolbar->addAction(openProjectAction);

    saveProjectAction->setIcon(QIcon(":/icons/save_project.png"));
    toolbar->addAction(saveProjectAction);

    toolbar->addSeparator();

    parseAction->setIcon(QIcon(":/icons/parse_correct.png"));
    toolbar->addAction(parseAction);

    simulateAction->setIcon(QIcon(":/icons/simulate.png"));
    toolbar->addAction(simulateAction);

    toolbar->addSeparator();

    createLTSAction->setIcon(QIcon(":/icons/create_LTS.png"));
    toolbar->addAction(createLTSAction);

    createReducedLTSAction->setIcon(QIcon(":/icons/create_reduced_LTS.png"));
    toolbar->addAction(createReducedLTSAction);

    abortLTSCreationAction->setIcon(QIcon(":/icons/abort_LTS_creation.png"));
    toolbar->addAction(abortLTSCreationAction);

    toolbar->addSeparator();

    addPropertyAction->setIcon(QIcon(":/icons/add_property.png"));
    toolbar->addAction(addPropertyAction);

    verifyAllPropertiesAction->setIcon(QIcon(":/icons/verify_all.png"));
    toolbar->addAction(verifyAllPropertiesAction);

    abortVerificationAction->setIcon(QIcon(":/icons/abort_verification.png"));
    toolbar->addAction(abortVerificationAction);

    /* disable the abort actions since they should only be used when something is running */
    abortLTSCreationAction->setEnabled(false);
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
    propertiesDock = new PropertiesDock(this);
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
    viewMenu->addAction("Revert to default layout", this, &MainWindow::setDocksToDefault);
}

void MainWindow::actionNewProject()
{
    /* Not yet implemented */
}

void MainWindow::actionOpenProject()
{
    /* Not yet implemented */
}

void MainWindow::actionOpenExampleProject()
{
    /* Not yet implemented */
}

void MainWindow::actionSaveProject()
{
    /* Not yet implemented */
}

void MainWindow::actionSaveProjectAs()
{
    /* Not yet implemented */
}

void MainWindow::actionAddProperty()
{
    AddEditPropertyDialog *addPropertyDialog = new AddEditPropertyDialog(true, this);
    /* if adding was succesful (Add button was pressed), add the property to the properties dock */
    if (addPropertyDialog->exec()) {
        propertiesDock->addProperty(addPropertyDialog->getPropertyName(), addPropertyDialog->getPropertyText());
    }
}


void MainWindow::actionUndo()
{
    /* Not yet implemented */
}

void MainWindow::actionRedo()
{
    /* Not yet implemented */
}

void MainWindow::actionFindAndReplace()
{
    FindAndReplaceDialog *findAndReplaceDialog = new FindAndReplaceDialog(specificationEditor, this);
    findAndReplaceDialog->show();
}

void MainWindow::actionCut()
{
    /* Not yet implemented */
}

void MainWindow::actionCopy()
{
    /* Not yet implemented */
}

void MainWindow::actionPaste()
{
    /* Not yet implemented */
}

void MainWindow::actionDelete()
{
    /* Not yet implemented */
}

void MainWindow::actionSelectAll()
{
    /* Not yet implemented */
}


void MainWindow::actionParse()
{
    /* Not yet implemented */
}

void MainWindow::actionSimulate()
{
    /* Not yet implemented */
}

void MainWindow::actionCreateLTS()
{
    /* Not yet implemented */
}

void MainWindow::actionCreateReducedLTS()
{
    /* Not yet implemented */
}

void MainWindow::actionAbortLTSCreation()
{
    /* Not yet implemented */
}

void MainWindow::actionVerifyAllProperties()
{
    /* Not yet implemented */
}

void MainWindow::actionAbortVerification()
{
    /* Not yet implemented */
}
