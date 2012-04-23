// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include <QApplication>

MainWindow::MainWindow()
{
  m_ui.setupUi(this);

  m_ui.traceTable->resizeColumnToContents(0);
  m_ui.traceTable->resizeColumnToContents(1);

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui.actionLoadTrace, SIGNAL(triggered()), this, SLOT(loadTrace()));
  connect(m_ui.actionSaveTrace, SIGNAL(triggered()), this, SLOT(saveTrace()));
  connect(m_ui.actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
  connect(m_ui.actionPlayTrace, SIGNAL(triggered()), this, SLOT(playTrace()));
  connect(m_ui.actionRandomPlay, SIGNAL(triggered()), this, SLOT(randomPlay()));
  connect(m_ui.actionStop, SIGNAL(triggered()), this, SLOT(stopPlay()));
  connect(m_ui.actionSetPlayDelay, SIGNAL(triggered()), this, SLOT(setPlayDelay()));
  connect(m_ui.actionContents, SIGNAL(triggered()), this, SLOT(contents()));
  connect(m_ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
}


void MainWindow::open()
{
}

void MainWindow::loadTrace()
{
}

void MainWindow::saveTrace()
{
}

void MainWindow::playTrace()
{
}

void MainWindow::randomPlay()
{
}

void MainWindow::stopPlay()
{
}

void MainWindow::setPlayDelay()
{
}

void MainWindow::contents()
{
}

void MainWindow::about()
{
}
