#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "highlighter.h"
#include "ui_mainwindow.h"

class QTextEdit;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    QTextEdit*   getEditor();
private slots:
    void onOpen();
private:
    Ui::MainWindow m_ui;
    void setupEditor();
    void setSelectedEditorInChildren();

    Highlighter *highlighter;
};

#endif // MAINWINDOW_H
