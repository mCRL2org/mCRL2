#ifndef MCRL2XI_QT_MAINWINDOW_H
#define MCRL2XI_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "mcrl2xi_qt_highlighter.h"

class QTextEdit;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTextEdit*   getEditor();
private slots:
    void onOpenFile();
    void onCursorChange();
private:
    Ui::MainWindow *m_ui;
    void setupEditor();
    void setSelectedEditorInChildren();

    Highlighter *highlighter;
};

#endif // MCRL2XI_QT_MAINWINDOW_H
