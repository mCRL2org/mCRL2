#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new CodeEditor(this, true);
    setCentralWidget(textEdit);

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

    fileMenu->addAction("New Project", this, &MainWindow::actionNewProject);
    fileMenu->addSeparator();
    fileMenu->addAction("Open Project", this, &MainWindow::actionOpenProject);
    fileMenu->addAction("Open Example Project", this, &MainWindow::actionOpenExampleProject);
    fileMenu->addSeparator();
    fileMenu->addAction("Save Project", this, &MainWindow::actionSaveProject);
    fileMenu->addAction("Save Project As", this, &MainWindow::actionSaveProjectAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Add Property", this, &MainWindow::actionAddProperty);

    /* Create the Edit menu */
    QMenu *editMenu = menuBar()->addMenu("Edit");

    editMenu->addAction("Undo", this, &MainWindow::actionUndo);
    editMenu->addAction("Redo", this, &MainWindow::actionRedo);
    editMenu->addSeparator();
    editMenu->addAction("Find and Replace", this, &MainWindow::actionFindAndReplace);
    editMenu->addSeparator();
    editMenu->addAction("Cut", this, &MainWindow::actionCut);
    editMenu->addAction("Copy", this, &MainWindow::actionCopy);
    editMenu->addAction("Paste", this, &MainWindow::actionPaste);
    editMenu->addAction("Delete", this, &MainWindow::actionDelete);
    editMenu->addAction("Select All", this, &MainWindow::actionSelectAll);

    /* Create the View Menu (actions are added in setupDocks())*/
    viewMenu = menuBar()->addMenu("View");

    /* Create the Actions menu */
    QMenu *actionsMenu = menuBar()->addMenu("Actions");

    actionsMenu->addAction("Parse", this, &MainWindow::actionParse);
    actionsMenu->addAction("Simulate", this, &MainWindow::actionSimulate);
    actionsMenu->addSeparator();
    actionsMenu->addAction("Create LTS", this, &MainWindow::actionCreateLTS);
    actionsMenu->addAction("Create reduced LTS", this, &MainWindow::actionCreateReducedLTS);
    actionsMenu->addAction("Abort LTS creation", this, &MainWindow::actionAbortLTSCreation);
    actionsMenu->addSeparator();
    actionsMenu->addAction("Verify all Properties", this, &MainWindow::actionVerifyAllProperties);
    actionsMenu->addAction("Abort verification", this, &MainWindow::actionAbortVerification);
}

void MainWindow::setupToolbar()
{
    QToolBar *toolbar = addToolBar("Actions");
    toolbar->setIconSize(QSize(48, 48));

    /* create each toolbar item by adding an icon and an action */
    const QIcon newProjectIcon = QIcon(":/icons/new_project.png");
    QAction *newProjectAction = new QAction(newProjectIcon, "New Project", this);
    connect(newProjectAction, &QAction::triggered, this, &MainWindow::actionNewProject);
    toolbar->addAction(newProjectAction);

    const QIcon openProjectIcon = QIcon(":/icons/open_project.png");
    QAction *openProjectAction = new QAction(openProjectIcon, "Open Project", this);
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::actionOpenProject);
    toolbar->addAction(openProjectAction);

    const QIcon saveProjectIcon = QIcon(":/icons/save_project.png");
    QAction *saveProjectAction = new QAction(saveProjectIcon, "Save Project", this);
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::actionSaveProject);
    toolbar->addAction(saveProjectAction);

    toolbar->addSeparator();

    const QIcon parseIcon = QIcon(":/icons/parse_correct.png");
    QAction *parseAction = new QAction(parseIcon, "Parse", this);
    connect(parseAction, &QAction::triggered, this, &MainWindow::actionParse);
    toolbar->addAction(parseAction);

    const QIcon simulateIcon = QIcon(":/icons/simulate.png");
    QAction *simulateAction = new QAction(simulateIcon, "Simulate", this);
    connect(simulateAction, &QAction::triggered, this, &MainWindow::actionSimulate);
    toolbar->addAction(simulateAction);

    toolbar->addSeparator();

    const QIcon createLTSIcon = QIcon(":/icons/create_LTS.png");
    QAction *createLTSAction = new QAction(createLTSIcon, "Create LTS", this);
    connect(createLTSAction, &QAction::triggered, this, &MainWindow::actionCreateLTS);
    toolbar->addAction(createLTSAction);

    const QIcon createReducedLTSIcon = QIcon(":/icons/create_reduced_LTS.png");
    QAction *createReducedLTSAction = new QAction(createReducedLTSIcon, "Create Reduced LTS", this);
    connect(createReducedLTSAction, &QAction::triggered, this, &MainWindow::actionCreateReducedLTS);
    toolbar->addAction(createReducedLTSAction);

    const QIcon abortLTSCreationIcon = QIcon(":/icons/abort_LTS_creation.png");
    QAction *abortLTSCreationAction = new QAction(abortLTSCreationIcon, "Abort LTS Creation", this);
    connect(abortLTSCreationAction, &QAction::triggered, this, &MainWindow::actionAbortLTSCreation);
    toolbar->addAction(abortLTSCreationAction);

    toolbar->addSeparator();

    const QIcon addPropertyIcon = QIcon(":/icons/add_property.png");
    QAction *addPropertyAction = new QAction(addPropertyIcon, "Add Property", this);
    connect(addPropertyAction, &QAction::triggered, this, &MainWindow::actionAddProperty);
    toolbar->addAction(addPropertyAction);

    const QIcon verifyAllPropertiesIcon = QIcon(":/icons/verify_all.png");
    QAction *verifyAllPropertiesAction = new QAction(verifyAllPropertiesIcon, "Verify All Properties", this);
    connect(verifyAllPropertiesAction, &QAction::triggered, this, &MainWindow::actionVerifyAllProperties);
    toolbar->addAction(verifyAllPropertiesAction);

    const QIcon abortVerificationIcon = QIcon(":/icons/abort_verification.png");
    QAction *abortVerificationAction = new QAction(abortVerificationIcon, "Abort Verification", this);
    connect(abortVerificationAction, &QAction::triggered, this, &MainWindow::actionAbortVerification);
    toolbar->addAction(abortVerificationAction);
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
    /* if adding was succesfull (Add button was pressed), add the property to the properties dock */
    if (addPropertyDialog->exec()){
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
    /* Not yet implemented */
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
