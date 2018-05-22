#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupMenuBar();
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

    /* Create the View Menu */
    QMenu *viewMenu = menuBar()->addMenu("View");

    /*
     * viewMenu->addAction(propertiesDock->toggleViewAction());
     * viewMenu->addAction(consoleDock->toggleViewAction());
     * viewMenu->addAction(rewriteDock->toggleViewAction());
     * viewMenu->addAction(solveDock->toggleViewAction());
    */

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
