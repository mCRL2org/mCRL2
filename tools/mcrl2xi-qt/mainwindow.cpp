// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{

  m_ui.setupUi(this);

  connect(m_ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
  connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
  connect(m_ui.actionSave_As, SIGNAL(triggered()), this, SLOT(onSaveAs()));

  connect(m_ui.documentManager, SIGNAL(documentCreated(DocumentWidget*)), this, SLOT(formatDocument(DocumentWidget*)));
}

MainWindow::~MainWindow()
{
  Highlighter *highlighter;
  foreach (highlighter, this->highlighters) {
    delete highlighter;
  }
}

void MainWindow::onNew()
{
  m_ui.documentManager->newFile();
}

void MainWindow::onOpen()
{
  QString fileName(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
                                                tr("mCRL2 specification (*.mcrl2 *.txt )")));
  if (!fileName.isNull()) {
    m_ui.documentManager->openFile(fileName);
  }
}

void MainWindow::onSave()
{
  QString fileName = m_ui.documentManager->currentFileName();
  if (fileName.isNull()) {
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                            tr("mCRL2 specification (*.mcrl2 *.txt )"));
  }
  if (!fileName.isNull()) {
    m_ui.documentManager->saveFile(fileName);
  }
}

void MainWindow::onSaveAs()
{
  QString fileName(QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                tr("mCRL2 specification (*.mcrl2 *.txt )")));
  if (!fileName.isNull()) {
    m_ui.documentManager->saveFile(fileName);
  }
}

void MainWindow::formatDocument(DocumentWidget *document)
{
  QTextEdit *editor = document->getEditor();
  editor->setWordWrapMode(QTextOption::NoWrap);

  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);

  editor->setFont(font);
  Highlighter *highlighter = new Highlighter(editor->document());
  this->highlighters.append(highlighter);
}
