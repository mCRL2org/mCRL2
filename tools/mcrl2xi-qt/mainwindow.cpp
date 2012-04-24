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

  connect(m_ui.documentManager, SIGNAL(documentCreated(QTextEdit*)), this, SLOT(setupEditor(QTextEdit*)));
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
  QString fileName(QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                tr("mCRL2 specification (*.mcrl2 *.txt )")));
  if (!fileName.isNull()) {
    m_ui.documentManager->saveFile(fileName);
  }
}

void MainWindow::setupEditor(QTextEdit *editor)
{
  editor->setWordWrapMode(QTextOption::NoWrap);

  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);

  editor->setFont(font);
  Highlighter *highlighter = new Highlighter(editor->document());
  this->highlighters.append(highlighter);
}
