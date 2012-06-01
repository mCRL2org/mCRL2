// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QFileSystemModel>

#include "mainwindow.h"
#include "mcrl2/utilities/logger.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  m_ui.setupUi(this);

  connect(m_ui.dockWidgetOutput, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QString, QDateTime, QString, QString)));

  initFileBrowser();
}

void MainWindow::initFileBrowser()
{
  QFileSystemModel *model = new QFileSystemModel(m_ui.treeFiles);
  model->setRootPath(QDir::rootPath());

  m_ui.treeFiles->setModel(model);

  m_ui.treeFiles->sortByColumn(0, Qt::AscendingOrder);
  m_ui.treeFiles->setColumnHidden( 1, true );
  m_ui.treeFiles->setColumnHidden( 2, true );
  m_ui.treeFiles->setColumnHidden( 3, true );

  //m_ui.treeFiles->setCurrentIndex(model->index(QDir::currentPath()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage)
{
  m_ui.statusBar->showMessage(formattedMessage, 5000);
}
