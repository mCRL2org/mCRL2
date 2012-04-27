// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QFileDialog>
#include <QTextEdit>
#include <QMessageBox>

#include "mainwindow.h"
#include "threadparent.h"
#include "rewriter.h"
#include "solver.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{

  m_ui.setupUi(this);
  m_parser = new Parser();

  //All menu items
  connect(m_ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
  connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
  connect(m_ui.actionSave_As, SIGNAL(triggered()), this, SLOT(onSaveAs()));
  connect(m_ui.actionExit, SIGNAL(triggered()), this, SLOT(onExit()));

  connect(m_ui.actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
  connect(m_ui.actionRedo, SIGNAL(triggered()), this, SLOT(onRedo()));
  connect(m_ui.actionCut, SIGNAL(triggered()), this, SLOT(onCut()));
  connect(m_ui.actionCopy, SIGNAL(triggered()), this, SLOT(onCopy()));
  connect(m_ui.actionPaste, SIGNAL(triggered()), this, SLOT(onPaste()));
  connect(m_ui.actionDelete, SIGNAL(triggered()), this, SLOT(onDelete()));
  connect(m_ui.actionSelect_All, SIGNAL(triggered()), this, SLOT(onSelectAll()));

  connect(m_ui.actionFind, SIGNAL(triggered()), this, SLOT(onFind()));
  connect(m_ui.actionWrap_mode, SIGNAL(triggered()), this, SLOT(onWrapMode()));
  connect(m_ui.actionReset_perspective, SIGNAL(triggered()), this, SLOT(onResetPerspective()));

  //All button functionality
  connect(m_ui.buttonParse, SIGNAL(clicked()), this, SLOT(onParse()));
  connect(m_parser, SIGNAL(parsed()), this, SLOT(parsed()));

  connect(m_ui.buttonRewrite, SIGNAL(clicked()), this, SLOT(onRewrite()));

  connect(m_ui.buttonSolve, SIGNAL(clicked()), this, SLOT(onSolve()));
  connect(m_ui.buttonSolveAbort, SIGNAL(clicked()), this, SLOT(onSolveAbort()));

  //Documentmanager events
  connect(m_ui.documentManager, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseRequest(int)));
  connect(m_ui.documentManager, SIGNAL(documentCreated(DocumentWidget*)), this, SLOT(formatDocument(DocumentWidget*)));

  connect(m_ui.dockWidgetOutput, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QString, QDateTime, QString, QString)));
}

MainWindow::~MainWindow()
{
  m_parser->deleteLater();
}


bool MainWindow::saveDocument(DocumentWidget *document)
{
  QString fileName = document->getFileName();
  if (fileName.isNull()) {
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                            tr("mCRL2 specification (*.mcrl2 *.txt )"));
  }
  if (!fileName.isNull()) {
    m_ui.documentManager->saveFile(fileName);
    return true;
  }
  return false;
}

void MainWindow::openDocument(QString fileName)
{
  if (!fileName.isNull()) {
    m_ui.documentManager->openFile(fileName);
  }
}


void MainWindow::formatDocument(DocumentWidget *document)
{
  QTextEdit *editor = document->getEditor();
  editor->setWordWrapMode(QTextOption::NoWrap);

  QFont font;
  font.setFamily("Monospace");
  font.setFixedPitch(true);

  editor->setFont(font);
  Highlighter *highlighter = new Highlighter(editor->document());

  ThreadParent<Rewriter> *rewriter = new ThreadParent<Rewriter>(document);
  QMetaObject::invokeMethod(rewriter->getThread(), "setRewriter", Qt::QueuedConnection, Q_ARG(QString, QString("jitty")));
  connect(rewriter->getThread(), SIGNAL(rewritten(QString)), this, SLOT(rewritten(QString)));

  ThreadParent<Solver> *solver = new ThreadParent<Solver>(document);
  QMetaObject::invokeMethod(solver->getThread(), "setRewriter", Qt::QueuedConnection, Q_ARG(QString, QString("jitty")));
  connect(solver->getThread(), SIGNAL(solvedPart(QString)), this, SLOT(solvedPart(QString)));
  connect(solver->getThread(), SIGNAL(solved()), this, SLOT(solved()));
}

bool MainWindow::onCloseRequest(int index)
{
  DocumentWidget *document = m_ui.documentManager->getDocument(index);

  if (!document->isModified()) {
    m_ui.documentManager->closeDocument(index);
    return true;
  }

  int ret = QMessageBox::question ( this, tr("Specification modified"), tr("Do you want to save your modifications?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
  switch(ret)
  {
    case QMessageBox::Yes:
      if (this->saveDocument(document))
      {
        m_ui.documentManager->closeDocument(index);
      }
      break;
    case QMessageBox::No:
      m_ui.documentManager->closeDocument(index);
      break;
    case QMessageBox::Cancel:
      return false;
      break;
  }

  return true;
}

void MainWindow::onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage)
{
  m_ui.statusBar->showMessage(formattedMessage, 5000);
}


void MainWindow::onNew()
{
  m_ui.documentManager->newFile();
}

void MainWindow::onOpen()
{
  QString fileName(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
                                                tr("mCRL2 specification (*.mcrl2 *.txt )")));
  this->openDocument(fileName);
}

void MainWindow::onSave()
{
  this->saveDocument(m_ui.documentManager->currentDocument());
}

void MainWindow::onSaveAs()
{
  QString fileName(QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                tr("mCRL2 specification (*.mcrl2 *.txt )")));
  if (!fileName.isNull()) {
    m_ui.documentManager->saveFile(fileName);
  }
}

void MainWindow::onExit()
{
  this->close();
}

void MainWindow::onUndo()
{
  m_ui.documentManager->currentDocument()->getEditor()->undo();
}

void MainWindow::onRedo()
{
  m_ui.documentManager->currentDocument()->getEditor()->redo();
}

void MainWindow::onCut()
{
  m_ui.documentManager->currentDocument()->getEditor()->cut();
}

void MainWindow::onCopy()
{
  m_ui.documentManager->currentDocument()->getEditor()->copy();
}

void MainWindow::onPaste()
{
  m_ui.documentManager->currentDocument()->getEditor()->paste();
}

void MainWindow::onDelete()
{
  m_ui.documentManager->currentDocument()->getEditor()->textCursor().deleteChar();
}

void MainWindow::onSelectAll()
{
  m_ui.documentManager->currentDocument()->getEditor()->selectAll();
}

void MainWindow::onFind()
{
  //TODO
}

void MainWindow::onWrapMode()
{
  //TODO
}

void MainWindow::onResetPerspective()
{
  //TODO
}


void MainWindow::onParse()
{
  m_ui.buttonParse->setEnabled(false);
  DocumentWidget *document = m_ui.documentManager->currentDocument();
  QMetaObject::invokeMethod(m_parser, "parse", Qt::QueuedConnection, Q_ARG(QString, document->getEditor()->toPlainText()));
}

void MainWindow::parsed()
{
  m_ui.buttonParse->setEnabled(true);
}

void MainWindow::onRewrite()
{
  m_ui.buttonRewrite->setEnabled(false);
  m_ui.editRewriteOutput->clear();
  DocumentWidget *document = m_ui.documentManager->currentDocument();

  // findChild<ThreadParent<Rewriter> *> seems to match all objects instead of only those
  // with type findChild<ThreadParent<Rewriter> *>, "Rewriter" as objectName was added to find the appropriate object
  // findChild<...> without using templates works correctly, this could be a Qt bug

  ThreadParent<Rewriter> *rewriterparent = document->findChild<ThreadParent<Rewriter> *>("Rewriter");
  QMetaObject::invokeMethod(rewriterparent->getThread(), "rewrite", Qt::QueuedConnection, Q_ARG(QString, document->getEditor()->toPlainText()), Q_ARG(QString, m_ui.editRewriteExpr->text()));
}

void MainWindow::rewritten(QString output)
{
  m_ui.editRewriteOutput->setPlainText(output);
  m_ui.buttonRewrite->setEnabled(true);
}

void MainWindow::onSolve()
{
  m_ui.buttonSolve->setEnabled(false);
  m_ui.buttonSolveAbort->setEnabled(true);
  m_ui.editSolveOutput->clear();
  DocumentWidget *document = m_ui.documentManager->currentDocument();

  // findChild<ThreadParent<Solver> *> seems to match all objects instead of only those
  // with type findChild<ThreadParent<Solver> *>, "Solver" as objectName was added to find the appropriate object
  // findChild<...> without using templates works correctly, this could be a Qt bug

  ThreadParent<Solver> *solverparent = document->findChild<ThreadParent<Solver> *>("Solver");
  QMetaObject::invokeMethod(solverparent->getThread(), "solve", Qt::QueuedConnection, Q_ARG(QString, document->getEditor()->toPlainText()), Q_ARG(QString, m_ui.editSolveExpr->text()));
}

void MainWindow::onSolveAbort()
{
  DocumentWidget *document = m_ui.documentManager->currentDocument();

  // findChild<ThreadParent<Solver> *> seems to match all objects instead of only those
  // with type findChild<ThreadParent<Solver> *>, "Solver" as objectName was added to find the appropriate object
  // findChild<...> without using templates works correctly, this could be a Qt bug

  ThreadParent<Solver> *solverparent = document->findChild<ThreadParent<Solver> *>("Solver");
  QMetaObject::invokeMethod(solverparent->getThread(), "abort", Qt::QueuedConnection);

}

void MainWindow::solvedPart(QString output)
{
  m_ui.editSolveOutput->appendPlainText(output);
}

void MainWindow::solved()
{
  m_ui.buttonSolveAbort->setEnabled(false);
  m_ui.buttonSolve->setEnabled(true);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
  for (int i = 0; i < m_ui.documentManager->documentCount(); i++)
  {
    if (!this->onCloseRequest(i))
    {
      event->ignore();
      return;
    }
  }
  event->accept();
}
