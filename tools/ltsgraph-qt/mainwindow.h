// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainwindow.h
/// \brief Main Window of LTSGraph used as GUI

#ifndef LTSGRAPH_MAINWINDOW_H
#define LTSGRAPH_MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "ui_mainwindow.h"
#include "graph.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    // Constructor
    MainWindow(QWidget *parent = 0);

    // Destructor
    ~MainWindow();


  private:
    // UI variable
    Ui::MainWindow m_ui;

    QFileDialog* m_opendialog;
    Graph::Graph m_graph;

  private slots:
    void onOpen(const QString& filename);
};

#endif // LTSGRAPH_MAINWINDOW_H
