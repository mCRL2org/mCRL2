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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void actionNewProject();
    void actionOpenProject();
    void actionOpenExampleProject();
    void actionSaveProject();
    void actionSaveProjectAs();
    void actionAddProperty();

    void actionUndo();
    void actionRedo();
    void actionFindAndReplace();
    void actionCut();
    void actionCopy();
    void actionPaste();
    void actionDelete();
    void actionSelectAll();

    void actionParse();
    void actionSimulate();
    void actionCreateLTS();
    void actionCreateReducedLTS();
    void actionAbortLTSCreation();
    void actionVerifyAllProperties();
    void actionAbortVerification();

private:
    void setupMenuBar();
    void setupToolbar();
    void setDocksToDefault();
    void setupDocks();

    QMenu *viewMenu;
    QToolBar *toolbar;
    QPlainTextEdit *textEdit;
    PropertiesDock *propertiesDock;
    ConsoleDock *consoleDock;
    RewriteDock *rewriteDock;
    SolveDock *solveDock;
};



#endif // MAINWINDOW_H
