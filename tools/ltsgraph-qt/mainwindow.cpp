// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  m_ui.setupUi(this);

  m_opendialog = new QFileDialog(this, tr("Open file"), QString(),
                                 tr("Labelled transition systems (*.lts *.aut *.fsm *.dot)"));
  connect(m_ui.actionOpen, SIGNAL(triggered()), m_opendialog, SLOT(exec()));
  connect(m_opendialog, SIGNAL(fileSelected(QString)), this, SLOT(onOpen(const QString&)));
}

MainWindow::~MainWindow()
{
  delete m_opendialog;
}

void MainWindow::onOpen(const QString& filename)
{
  //m_graph.load(filename, m_ui.glWidget->width() / 2.0, m_ui.glWidget->height() / 2.0);

}
