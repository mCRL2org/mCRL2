// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "documentmanager.h"
#include "ui_documentmanager.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "documentwidget.h"

DocumentManager::DocumentManager(QWidget *parent) :
  QWidget(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
  connect(m_ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseRequest(int)));
}

void DocumentManager::showEvent(QShowEvent *event)
{
  // This cannot be done in the constructor because
  // the documentCreated signal is fired before the connection can be made
  if (m_ui.tabWidget->count() == 0)
    this->newFile();
}

DocumentWidget* DocumentManager::createDocument(QString title)
{

  DocumentWidget *document = new DocumentWidget(m_ui.tabWidget);
  m_ui.tabWidget->addTab(document, title);

  emit(documentCreated(document));

  m_ui.tabWidget->setCurrentWidget(document);
  return document;
}

int DocumentManager::documentCount()
{
  return m_ui.tabWidget->count();
}

int DocumentManager::indexOf(DocumentWidget *document)
{
  return m_ui.tabWidget->indexOf(document);
}

DocumentWidget* DocumentManager::getDocument(int index)
{
  if (index < 0 || index >= documentCount())
    return 0;
  return dynamic_cast<DocumentWidget *>(m_ui.tabWidget->widget(index));
}

DocumentWidget* DocumentManager::findDocument(QString fileName)
{
  for (int i = 0; i < this->documentCount(); i++) {
    if (this->getDocument(i)->getFileName() == fileName)
      return this->getDocument(i);
  }
  return 0;
}

void DocumentManager::closeDocument(int index)
{
  DocumentWidget* document = this->getDocument(index);
  emit documentClosed(document);
  delete document;

  if (m_ui.tabWidget->count() == 0)
    this->newFile();
}

DocumentWidget* DocumentManager::currentDocument()
{
  return dynamic_cast<DocumentWidget *>(m_ui.tabWidget->currentWidget());
}

QString DocumentManager::currentFileName()
{
  DocumentWidget* document = this->currentDocument();
  return document->getFileName();
}

void DocumentManager::newFile()
{
  this->createDocument(tr("Untitled"));
}

void DocumentManager::openFile(QString fileName)
{

  QFileInfo fileInfo(fileName);
  DocumentWidget *document = this->findDocument(fileName);

  if (document) {
    m_ui.tabWidget->setCurrentWidget(document);
    return;
  }

  for (int i = 0; i < documentCount(); i++)
  {
    if (getDocument(i)->getFileName() == QString() && !getDocument(i)->isModified())
    {
      document = getDocument(i);
      m_ui.tabWidget->setCurrentWidget(document);
      m_ui.tabWidget->setTabText(indexOf(document), fileInfo.baseName());
      break;
    }
  }

  if (!document)
    document = this->createDocument(fileInfo.baseName());

  document->openFile(fileName);
}

void DocumentManager::saveFile(QString fileName)
{
  QFileInfo fileInfo(fileName);
  m_ui.tabWidget->setTabText(m_ui.tabWidget->currentIndex(), fileInfo.baseName());
  this->currentDocument()->saveFile(fileName);
}
