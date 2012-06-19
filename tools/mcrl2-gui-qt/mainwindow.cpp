// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mainwindow.h"
#include "mcrl2/utilities/logger.h"
#include <QMessageBox>
#include "toolaction.h"
#include "toolinstance.h"
#include "fileinformation.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  m_ui.setupUi(this);

  connect(m_ui.actionNew_File, SIGNAL(triggered()), m_ui.treeFiles, SLOT(onNewFile()));
  connect(m_ui.actionNew_Folder, SIGNAL(triggered()), m_ui.treeFiles, SLOT(onNewFile()));
  connect(m_ui.actionOpen_File, SIGNAL(triggered()), m_ui.treeFiles, SLOT(onOpenFile()));
  connect(m_ui.actionDelete_File, SIGNAL(triggered()), m_ui.treeFiles, SLOT(onDeleteFile()));
  connect(m_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(m_ui.actionReset_perspective, SIGNAL(triggered()), this, SLOT(onResetPerspective()));


  connect(m_ui.dockWidgetOutput, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QString, QDateTime, QString, QString)));
  connect(m_ui.tabInstances, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequest(int)));
  connect(m_ui.treeFiles, SIGNAL(openToolInstance(QString, ToolInformation)), this, SLOT(createToolInstance(QString, ToolInformation)));
  connect(m_ui.treeFiles, SIGNAL(openProperties(QString)), this, SLOT(createFileInformation(QString)));

  m_catalog.load();
  m_ui.treeFiles->setCatalog(m_catalog);

  createToolMenu();
  m_state = saveState();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onResetPerspective()
{
  m_state = saveState();
}

void MainWindow::createToolMenu()
{
  QMenu *menuTools = new QMenu(m_ui.mnuMain);
  menuTools->setTitle("&Tool Information");

  QStringList cats = m_catalog.categories();
  for (int i = 0; i < cats.size(); i++)
  {
    QMenu *menuCat = new QMenu(menuTools);
    menuCat->setTitle(cats.at(i));
    menuTools->addMenu(menuCat);
    QList<ToolInformation> tools = m_catalog.tools(cats.at(i));
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

void MainWindow::createToolInstance(QString filename, ToolInformation info)
{
  ToolInstance* toolInstance = new ToolInstance(filename, info, m_ui.tabInstances);
  int index = m_ui.tabInstances->addTab(toolInstance, info.name);
  connect(toolInstance, SIGNAL(titleChanged(QString)), this, SLOT(onTabTitleChanged(QString)));
  connect(toolInstance, SIGNAL(colorChanged(QColor)), this, SLOT(onTabColorChanged(QColor)));
  m_ui.tabInstances->setCurrentIndex(index);
}

void MainWindow::createFileInformation(QString filename)
{
  QFileInfo info = QFileInfo(filename);
  QString title = (info.isRoot() ? info.absolutePath() : info.fileName());
  FileInformation* fileInformation = new FileInformation(filename, m_ui.tabInstances);
  int index = m_ui.tabInstances->addTab(fileInformation, title);
  m_ui.tabInstances->setCurrentIndex(index);
}

void MainWindow::onLogOutput(QString /*level*/, QString /*hint*/, QDateTime /*timestamp*/, QString /*message*/, QString formattedMessage)
{
  m_ui.statusBar->showMessage(formattedMessage, 5000);
}

void MainWindow::onToolInfo()
{
  ToolAction* act = dynamic_cast<ToolAction*>(QObject::sender());
  QString message;
  message += "<h1>" + act->information().name + "</h1>";
  message += "<p>" + act->information().desc + "</p>";
  message += "<p>Written by " + act->information().author  + "</p>";
  QMessageBox::information(this, "Tool Information", message);
}

void MainWindow::onTabTitleChanged(QString title)
{
  ToolInstance* toolInstance = dynamic_cast<ToolInstance*>(QObject::sender());
  int index = m_ui.tabInstances->indexOf(toolInstance);
  if (index > -1)
  {
    m_ui.tabInstances->setTabText(index, title);
  }
}

void MainWindow::onTabColorChanged(QColor color)
{
  ToolInstance* toolInstance = dynamic_cast<ToolInstance*>(QObject::sender());
  int index = m_ui.tabInstances->indexOf(toolInstance);
  if (index > -1)
  {
    QPixmap pm(12, 12);
    pm.fill(color);
    m_ui.tabInstances->setTabIcon(index, pm);
  }
}

void MainWindow::onTabCloseRequest(int index)
{
  ToolInstance* toolInstance = dynamic_cast<ToolInstance*>(m_ui.tabInstances->widget(index));
  delete toolInstance;
  FileInformation* fileInformation = dynamic_cast<FileInformation*>(m_ui.tabInstances->widget(index));
  delete fileInformation;
}

