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
#include <QMessageBox>
#include "toolaction.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  m_ui.setupUi(this);

  connect(m_ui.dockWidgetOutput, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QString, QDateTime, QString, QString)));

  m_catalog.load();
  initFileBrowser();

  createToolMenu();
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

void MainWindow::createToolMenu()
{
  QMenu *menuTools = new QMenu(m_ui.mnuMain);
  menuTools->setTitle("&Tool Information");

  QStringList cats = m_catalog.getCategories();
  for (int i = 0; i < cats.size(); i++)
  {
    QMenu *menuCat = new QMenu(menuTools);
    menuCat->setTitle(cats.at(i));
    menuTools->addMenu(menuCat);
    QList<ToolInformation> tools = m_catalog.getTools(cats.at(i));
    for (int i = 0; i < tools.count(); i++)
    {
      ToolInformation tool = tools.at(i);
      ToolAction* actTool = new ToolAction(tool, menuCat);
      menuCat->addAction(actTool);
      connect(actTool, SIGNAL(triggered()), this, SLOT(onToolInfo()));
    }
  }
  m_ui.mnuMain->addMenu(menuTools);
}

MainWindow::~MainWindow()
{

}

void MainWindow::onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage)
{
  m_ui.statusBar->showMessage(formattedMessage, 5000);
}

void MainWindow::onToolInfo()
{
  ToolAction* act = dynamic_cast<ToolAction*>(QObject::sender());
  QString message;
  message += "<h1>" + act->getInformation().getName() + "</h1>";
  message += "<p>" + act->getInformation().getDescription().replace("\n", "<br>") + "</p>";
  message += "<p>Written by " + act->getInformation().getAuthor()  + "</p>";
  QMessageBox::information(this, "Tool Information", message);
}



