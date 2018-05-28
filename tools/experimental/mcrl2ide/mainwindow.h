#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "propertiesdock.h"
#include "consoledock.h"
#include "rewritedock.h"
#include "solvedock.h"
#include "addeditpropertydialog.h"
#include "codeeditor.h"

/**
 * @brief The MainWindow class defines the main window off the application
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    /**
     * @brief actionNewProject allows the user to create a new project
     */
    void actionNewProject();

    /**
     * @brief actionOpenProject allows the user to open a project
     */
    void actionOpenProject();

    /**
     * @brief actionNewProject allows the user to open an example project
     */
    void actionOpenExampleProject();

    /**
     * @brief actionNewProject allows the user to save a project
     */
    void actionSaveProject();

    /**
     * @brief actionNewProject allows the user to save a new project under a new name
     */
    void actionSaveProjectAs();

    /**
     * @brief actionAddProperty allows the user to add a property
     */
    void actionAddProperty();


    /**
     * @brief actionUndo allows the user to undo the last action made in the text editor
     */
    void actionUndo();

    /**
     * @brief actionRedo allows the user to redo the last undo
     */
    void actionRedo();

    /**
     * @brief actionFindAndReplace allows the user to find and replace strings in the text editor
     */
    void actionFindAndReplace();

    /**
     * @brief actionCut allows the user to cut the selected text in the text editor
     */
    void actionCut();

    /**
     * @brief actionCopy allows the user to copy the selected text in the text editor
     */
    void actionCopy();

    /**
     * @brief actionPaste allows the user to paste text in the text editor
     */
    void actionPaste();

    /**
     * @brief actionDelete allows the user to delete selected text in the text editor
     */
    void actionDelete();

    /**
     * @brief actionSelectAll allows the user to select all text in the text editor
     */
    void actionSelectAll();


    /**
     * @brief actionParse allows the user to parse the current specification
     */
    void actionParse();

    /**
     * @brief actionSimulate allows the user to simulate the current specification
     */
    void actionSimulate();

    /**
     * @brief actionCreateLTS allows the user to create the LTS of the current specification
     */
    void actionCreateLTS();

    /**
     * @brief actionCreateReducedLTS allows the user to create a reduced LTS of the current specification
     */
    void actionCreateReducedLTS();

    /**
     * @brief actionAbortLTSCreation allows the user to abort LTS creation
     */
    void actionAbortLTSCreation();

    /**
     * @brief actionVerifyAllProperties allows the user to verify all defined properties on the current specification
     */
    void actionVerifyAllProperties();

    /**
     * @brief actionAbortVerification allows the user to abort verification
     */
    void actionAbortVerification();

private:
    /**
     * @brief setupMenuBar creates the menubar
     */
    void setupMenuBar();

    /**
     * @brief setupToolbar creates the toolbar
     */
    void setupToolbar();

    /**
     * @brief setDocksToDefault puts all docks in their default location
     */
    void setDocksToDefault();

    /**
     * @brief setupDocks creates the docks
     */
    void setupDocks();

    QMenu *viewMenu;
    QToolBar *toolbar;
    CodeEditor *textEdit;
    PropertiesDock *propertiesDock;
    ConsoleDock *consoleDock;
    RewriteDock *rewriteDock;
    SolveDock *solveDock;
};



#endif // MAINWINDOW_H
