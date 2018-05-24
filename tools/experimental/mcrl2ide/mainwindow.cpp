#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);

    setupMenuBar();
    setupToolbar();
    setupDocks();
}

MainWindow::~MainWindow()
{
}

/**
 * @brief MainWindow::setupMenuBar creates the menubar
 */
void MainWindow::setupMenuBar(){

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

/**
 * @brief MainWindow::setupToolbar creates the toolbar
 */
void MainWindow::setupToolbar(){

    QToolBar *toolbar = addToolBar("Actions");
    toolbar->setIconSize(QSize(32, 32));

    const QIcon tmp = QIcon(":/icons/cogwheelmedium.png"); /* placeholder */

    /* create each toolbar item by adding an icon and an action */
    const QIcon newProjectIcon = tmp;
    QAction *newProjectAction = new QAction(newProjectIcon, "New Project", this);
    connect(newProjectAction, &QAction::triggered, this, &MainWindow::actionNewProject);
    toolbar->addAction(newProjectAction);

    const QIcon openProjectIcon = tmp;
    QAction *openProjectAction = new QAction(openProjectIcon, "Open Project", this);
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::actionOpenProject);
    toolbar->addAction(openProjectAction);

    const QIcon saveProjectIcon = tmp;
    QAction *saveProjectAction = new QAction(saveProjectIcon, "Save Project", this);
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::actionSaveProject);
    toolbar->addAction(saveProjectAction);

    toolbar->addSeparator();

    const QIcon parseIcon = tmp;
    QAction *parseAction = new QAction(parseIcon, "Parse", this);
    connect(parseAction, &QAction::triggered, this, &MainWindow::actionParse);
    toolbar->addAction(parseAction);

    const QIcon simulateIcon = tmp;
    QAction *simulateAction = new QAction(simulateIcon, "Simulate", this);
    connect(simulateAction, &QAction::triggered, this, &MainWindow::actionSimulate);
    toolbar->addAction(simulateAction);

    toolbar->addSeparator();

    const QIcon createLTSIcon = tmp;
    QAction *createLTSAction = new QAction(createLTSIcon, "Create LTS", this);
    connect(createLTSAction, &QAction::triggered, this, &MainWindow::actionCreateLTS);
    toolbar->addAction(createLTSAction);

    const QIcon createReducedLTSIcon = tmp;
    QAction *createReducedLTSAction = new QAction(createReducedLTSIcon, "Create Reduced LTS", this);
    connect(createReducedLTSAction, &QAction::triggered, this, &MainWindow::actionCreateReducedLTS);
    toolbar->addAction(createReducedLTSAction);

    const QIcon abortLTSCreationIcon = tmp;
    QAction *abortLTSCreationAction = new QAction(abortLTSCreationIcon, "Abort LTS Creation", this);
    connect(abortLTSCreationAction, &QAction::triggered, this, &MainWindow::actionAbortLTSCreation);
    toolbar->addAction(abortLTSCreationAction);

    toolbar->addSeparator();

    const QIcon addPropertyIcon = tmp;
    QAction *addPropertyAction = new QAction(addPropertyIcon, "Add Property", this);
    connect(addPropertyAction, &QAction::triggered, this, &MainWindow::actionAddProperty);
    toolbar->addAction(addPropertyAction);

    const QIcon verifyAllPropertiesIcon = tmp;
    QAction *verifyAllPropertiesAction = new QAction(verifyAllPropertiesIcon, "Verify All Properties", this);
    connect(verifyAllPropertiesAction, &QAction::triggered, this, &MainWindow::actionVerifyAllProperties);
    toolbar->addAction(verifyAllPropertiesAction);

    const QIcon abortVerificationIcon = tmp;
    QAction *abortVerificationAction = new QAction(abortVerificationIcon, "Abort Verification", this);
    connect(abortVerificationAction, &QAction::triggered, this, &MainWindow::actionAbortVerification);
    toolbar->addAction(abortVerificationAction);
}

/**
 * @brief MainWindow::setDocksToDefault puts all docks in their default location
 */
void MainWindow::setDocksToDefault(){

    addDockWidget(propertiesDock->defaultArea, propertiesDock);
    addDockWidget(consoleDock->defaultArea, consoleDock);
    addDockWidget(rewriteDock->defaultArea, rewriteDock);
    addDockWidget(solveDock->defaultArea, solveDock);

    rewriteDock->hide();
    solveDock->hide();
}

/**
 * @brief MainWindow::setupDocks creates the docks
 */
void MainWindow::setupDocks(){

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

/**
 * @brief MainWindow::actionNewProject allows the user to create a new project
 */
void MainWindow::actionNewProject(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionOpenProject allows the user to open a project
 */
void MainWindow::actionOpenProject(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionNewProject allows the user to open an example project
 */
void MainWindow::actionOpenExampleProject(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionNewProject allows the user to save a project
 */
void MainWindow::actionSaveProject(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionNewProject allows the user to save a new project under a new name
 */
void MainWindow::actionSaveProjectAs(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionAddProperty allows the user to add a property
 */
void MainWindow::actionAddProperty(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionUndo allows the user to undo the last action made in the text editor
 */
void MainWindow::actionUndo(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionRedo allows the user to redo the last undo
 */
void MainWindow::actionRedo(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionFindAndReplace allows the user to find and replace strings in the text editor
 */
void MainWindow::actionFindAndReplace(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionCut allows the user to cut the selected text in the text editor
 */
void MainWindow::actionCut(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionCopy allows the user to copy the selected text in the text editor
 */
void MainWindow::actionCopy(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionPaste allows the user to paste text in the text editor
 */
void MainWindow::actionPaste(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionDelete allows the user to delete selected text in the text editor
 */
void MainWindow::actionDelete(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionSelectAll allows the user to select all text in the text editor
 */
void MainWindow::actionSelectAll(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionParse allows the user to parse the current specification
 */
void MainWindow::actionParse(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionSimulate allows the user to simulate the current specification
 */
void MainWindow::actionSimulate(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionCreateLTS allows the user to create the LTS of the current specification
 */
void MainWindow::actionCreateLTS(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionCreateReducedLTS allows the user to create a reduced LTS of the current specification
 */
void MainWindow::actionCreateReducedLTS(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionAbortLTSCreation allows the user to abort LTS creation
 */
void MainWindow::actionAbortLTSCreation(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionVerifyAllProperties allows the user to verify all defined properties on the current specification
 */
void MainWindow::actionVerifyAllProperties(){
    /* Not yet implemented */
}

/**
 * @brief MainWindow::actionAbortVerification allows the user to abort verification
 */
void MainWindow::actionAbortVerification(){
    /* Not yet implemented */
}
