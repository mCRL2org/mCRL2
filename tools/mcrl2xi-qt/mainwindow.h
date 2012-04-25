// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "ui_mainwindow.h"
#include "highlighter.h"
#include "documentwidget.h"

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void formatDocument(DocumentWidget *document);

private slots:
  void onNew();
  void onOpen();
  void onSave();
  void onSaveAs();

private:
    Ui::MainWindow m_ui;
    QVector<Highlighter*> highlighters;
};

#endif // MAINWINDOW_H
