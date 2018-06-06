#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "propertiesdock.h"
#include "consoledock.h"
#include "rewritedock.h"
#include "solvedock.h"
#include "addeditpropertydialog.h"
#include "findandreplacedialog.h"
#include "codeeditor.h"
#include "filesystem.h"
#include "processsystem.h"

#include <QMainWindow>

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
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    /**
     * @brief actionNewProject Allows the user to create a new project
     */
    void actionNewProject();

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
     * @brief actionNewProject Allows the user to save a new project under a new name
     */
    void actionSaveProjectAs();

    /**
     * @brief actionAddProperty Allows the user to add a property
     */
    void actionAddProperty();

    /**
     * @brief actionFindAndReplace Allows the user to find and replace strings in the text editor
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
     * @brief actionCreateLTS Allows the user to create the LTS of the current specification
     */
    void actionCreateLTS();

    /**
     * @brief actionCreateReducedLTS Allows the user to create a reduced LTS of the current specification
     */
    void actionCreateReducedLTS();

    /**
     * @brief actionAbortLTSCreation Allows the user to abort LTS creation
     */
    void actionAbortLTSCreation();

    /**
     * @brief actionVerifyAllProperties Allows the user to verify all defined properties on the current specification
     */
    void actionVerifyAllProperties();

    /**
     * @brief actionAbortVerification Allows the user to abort verification
     */
    void actionAbortVerification();

private:
    /**
     * @brief setupMenuBar Creates the menubar, also creates the actions and their icons and shortcuts (if applicable)
     */
    void setupMenuBar();

    /**
     * @brief setupToolbar Creates the toolbar
     */
    void setupToolbar();

    /**
     * @brief setDocksToDefault Puts all docks in their default location
     */
    void setDocksToDefault();

    /**
     * @brief setupDocks Creates the docks
     */
    void setupDocks();

    QAction *newProjectAction;
    QAction *openProjectAction;
    QAction *openExampleProjectAction;
    QAction *saveProjectAction;
    QAction *saveProjectAsAction;
    QAction *addPropertyAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *findAndReplaceAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectAllAction;

    QAction *parseAction;
    QAction *simulateAction;
    QAction *createLTSAction;
    QAction *createReducedLTSAction;
    QAction *abortLTSCreationAction;
    QAction *verifyAllPropertiesAction;
    QAction *abortVerificationAction;

    QMenu *viewMenu;
    QToolBar *toolbar;
    CodeEditor *specificationEditor;
    PropertiesDock *propertiesDock;
    ConsoleDock *consoleDock;
    RewriteDock *rewriteDock;
    SolveDock *solveDock;

    FindAndReplaceDialog *findAndReplaceDialog;
    AddEditPropertyDialog *addPropertyDialog;

    FileSystem *fileSystem;
    ProcessSystem *processSystem;
};



#endif // MAINWINDOW_H
