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
     * @brief actionUndo Allows the user to undo the last action made in the text editor
     */
    void actionUndo();

    /**
     * @brief actionRedo Allows the user to redo the last undo
     */
    void actionRedo();

    /**
     * @brief actionFindAndReplace Allows the user to find and replace strings in the text editor
     */
    void actionFindAndReplace();

    /**
     * @brief actionCut Allows the user to cut the selected text in the text editor
     */
    void actionCut();

    /**
     * @brief actionCopy Allows the user to copy the selected text in the text editor
     */
    void actionCopy();

    /**
     * @brief actionPaste Allows the user to paste text in the text editor
     */
    void actionPaste();

    /**
     * @brief actionDelete Allows the user to delete selected text in the text editor
     */
    void actionDelete();

    /**
     * @brief actionSelectAll Allows the user to select all text in the text editor
     */
    void actionSelectAll();


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
     * @brief setupMenuBar Creates the menubar
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

    QMenu *viewMenu;
    QToolBar *toolbar;
    CodeEditor *textEdit;
    PropertiesDock *propertiesDock;
    ConsoleDock *consoleDock;
    RewriteDock *rewriteDock;
    SolveDock *solveDock;
};



#endif // MAINWINDOW_H
