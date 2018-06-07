#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QInputDialog>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    specificationEditor = new CodeEditor(this, true);
    setCentralWidget(specificationEditor);

    fileSystem = new FileSystem(specificationEditor);
    processSystem = new ProcessSystem(fileSystem);

    setupMenuBar();
    setupToolbar();
    setupDocks();

    processSystem->setConsoleDock(consoleDock);

    findAndReplaceDialog = new FindAndReplaceDialog(specificationEditor, this);

    setWindowTitle("mCRL2 IDE - Unnamed project");
    resize(QSize(QDesktopWidget().availableGeometry(this).width() * 0.5, QDesktopWidget().availableGeometry(this).height() * 0.75));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenuBar()
{
    /* Create the File menu */
    QMenu *fileMenu = menuBar()->addMenu("File");

    newProjectAction = fileMenu->addAction("New Project", this, &MainWindow::actionNewProject);
    newProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    newProjectAction->setIcon(QIcon(":/icons/new_project.png"));

    fileMenu->addSeparator();

    openProjectAction = fileMenu->addAction("Open Project", this, &MainWindow::actionOpenProject);
    openProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    openProjectAction->setIcon(QIcon(":/icons/open_project.png"));

    openExampleProjectAction = fileMenu->addAction("Open Example Project", this, &MainWindow::actionOpenExampleProject);

    fileMenu->addSeparator();

    saveProjectAction = fileMenu->addAction("Save Project", this, &MainWindow::actionSaveProject);
    saveProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    saveProjectAction->setIcon(QIcon(":/icons/save_project.png"));

    saveProjectAsAction = fileMenu->addAction("Save Project As", this, &MainWindow::actionSaveProjectAs);

    fileMenu->addSeparator();

    addPropertyAction = fileMenu->addAction("Add Property", this, &MainWindow::actionAddProperty);
    addPropertyAction->setIcon(QIcon(":/icons/add_property.png"));

    /* Create the Edit menu */
    QMenu *editMenu = menuBar()->addMenu("Edit");

    undoAction = editMenu->addAction("Undo", specificationEditor, &CodeEditor::undo);
    undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));

    redoAction = editMenu->addAction("Redo", specificationEditor, &CodeEditor::redo);
    redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

    editMenu->addSeparator();

    findAndReplaceAction = editMenu->addAction("Find and Replace", this, &MainWindow::actionFindAndReplace);
    findAndReplaceAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

    editMenu->addSeparator();

    cutAction = editMenu->addAction("Cut", specificationEditor, &CodeEditor::cut);
    cutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));

    copyAction = editMenu->addAction("Copy", specificationEditor, &CodeEditor::copy);
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));

    pasteAction = editMenu->addAction("Paste", specificationEditor, &CodeEditor::paste);
    pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));

    deleteAction = editMenu->addAction("Delete", specificationEditor, &CodeEditor::deleteChar);
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    selectAllAction = editMenu->addAction("Select All", specificationEditor, &CodeEditor::selectAll);
    selectAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));

    /* Create the View Menu (actions are added in setupDocks())*/
    viewMenu = menuBar()->addMenu("View");

    /* Create the Actions menu */
    QMenu *actionsMenu = menuBar()->addMenu("Actions");

    parseAction = actionsMenu->addAction("Parse", this, &MainWindow::actionParse);
    parseAction->setIcon(QIcon(":/icons/parse_correct.png"));

    simulateAction = actionsMenu->addAction("Simulate", this, &MainWindow::actionSimulate);
    simulateAction->setIcon(QIcon(":/icons/simulate.png"));

    actionsMenu->addSeparator();

    createLTSAction = actionsMenu->addAction("Create LTS", this, &MainWindow::actionCreateLTS);
    createLTSAction->setIcon(QIcon(":/icons/create_LTS.png"));

    createReducedLTSAction = actionsMenu->addAction("Create reduced LTS", this, &MainWindow::actionCreateReducedLTS);
    createReducedLTSAction->setIcon(QIcon(":/icons/create_reduced_LTS.png"));

    abortLTSCreationAction = actionsMenu->addAction("Abort LTS creation", this, &MainWindow::actionAbortLTSCreation);
    abortLTSCreationAction->setIcon(QIcon(":/icons/abort_LTS_creation.png"));

    actionsMenu->addSeparator();

    verifyAllPropertiesAction = actionsMenu->addAction("Verify all Properties", this, &MainWindow::actionVerifyAllProperties);
    verifyAllPropertiesAction->setIcon(QIcon(":/icons/verify_all.png"));

    abortVerificationAction = actionsMenu->addAction("Abort verification", this, &MainWindow::actionAbortVerification);
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
    toolbar->addAction(createLTSAction);
    toolbar->addAction(createReducedLTSAction);
    toolbar->addAction(abortLTSCreationAction);
    toolbar->addSeparator();
    toolbar->addAction(addPropertyAction);
    toolbar->addAction(verifyAllPropertiesAction);
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
    viewMenu->addAction("Revert to default layout", this, &MainWindow::setDocksToDefault);
}

void MainWindow::actionNewProject()
{
    /* ask the user for a project name */
    bool ok;
    QString projectName = QInputDialog::getText(this, "New project", "Project name:", QLineEdit::Normal, "", &ok);

    /* if ok, create the project */
    if(ok && !projectName.isEmpty()){
        fileSystem->newProject(projectName);
        setWindowTitle(QString("mCRL2 IDE - ").append(projectName));
    }
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
    /* if we have a project open, we have a location to save in so we can simply save, else use save as */
    if (fileSystem->projectOpened()) {
        fileSystem->saveSpecification();
        propertiesDock->saveAllProperties();
    } else {
        actionSaveProjectAs();
    }
}

void MainWindow::actionSaveProjectAs()
{
    /* Not yet implemented */
}

void MainWindow::actionAddProperty()
{
    addPropertyDialog = new AddEditPropertyDialog(true, propertiesDock, this);
    /* if adding was succesful (Add button was pressed), add the property to the properties dock */
    if (addPropertyDialog->exec()) {
        propertiesDock->addProperty(addPropertyDialog->getPropertyName(), addPropertyDialog->getPropertyText());
    }
}

void MainWindow::actionFindAndReplace()
{
    if (findAndReplaceDialog->isVisible()) {
        findAndReplaceDialog->setFocus();
        findAndReplaceDialog->activateWindow();
    } else {
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
    propertiesDock->verifyAllProperties();
}

void MainWindow::actionAbortVerification()
{
    /* Not yet implemented */
}
