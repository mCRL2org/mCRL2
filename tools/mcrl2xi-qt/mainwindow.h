#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "ui_mainwindow.h"
#include "highlighter.h"

class QTextEdit;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void setupEditor(QTextEdit *editor);

private slots:
  void onNew();
  void onOpen();
  void onSave();

private:
    Ui::MainWindow m_ui;
    QVector<Highlighter*> highlighters;
};

#endif // MAINWINDOW_H
