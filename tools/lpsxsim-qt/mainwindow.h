// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPSXSIM_MAINWINDOW_H
#define LPSXSIM_MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "simulation.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow();

    protected slots:
    void open();
    void loadTrace();
    void saveTrace();
    void playTrace();
    void randomPlay();
    void stopPlay();
    void setPlayDelay();
    void contents();
    void about();

    protected:
    Ui::MainWindow m_ui;
    Simulation *m_simulation;
};

#endif
