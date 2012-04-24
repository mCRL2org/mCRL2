#include "documentmanager.h"
#include "ui_documentmanager.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

DocumentManager::DocumentManager(QWidget *parent) :
  QWidget(parent)
{
  m_ui.setupUi(this);
}

QTextEdit* DocumentManager::createEditor(QString title)
{

  QWidget *tabwidget = new QWidget(m_ui.tabWidget);
  m_ui.tabWidget->addTab(tabwidget, title);

  QTextEdit *editor = new QTextEdit(tabwidget);

  QVBoxLayout *layout = new QVBoxLayout(tabwidget);
  layout->setMargin(0);
  layout->addWidget(editor);
  tabwidget->setLayout(layout);

  emit(documentCreated(editor));

  m_ui.tabWidget->setCurrentWidget(tabwidget);

  return editor;
}

QTextEdit* DocumentManager::currentEditor()
{
  QWidget *tabwidget = m_ui.tabWidget->currentWidget();
  return tabwidget->findChild<QTextEdit *>();
}

QWidget* DocumentManager::currentTab()
{
  return m_ui.tabWidget->currentWidget();
}

void DocumentManager::newFile()
{
  this->createEditor(tr("Untitled"));
}


void DocumentManager::openFile(QString fileName)
{
  QFile file(fileName);

  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    QFileInfo fileInfo(fileName);
    QTextEdit *editor = this->createEditor(fileInfo.baseName());
    editor->setPlainText(file.readAll());
    file.close();
  }
}

void DocumentManager::saveFile(QString fileName)
{
  QFile file(fileName);

  if (file.open(QFile::WriteOnly | QFile::Text))
  {
    file.write((const char *)this->currentEditor()->toPlainText().toAscii().data());
    file.close();
  }
}
