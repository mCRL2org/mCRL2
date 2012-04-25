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

DocumentWidget* DocumentManager::currentDocument()
{
  return dynamic_cast<DocumentWidget *>(m_ui.tabWidget->currentWidget());
}

DocumentWidget* DocumentManager::findDocument(QString fileName)
{
  for (int i = 0; i < m_ui.tabWidget->count(); i++) {
    if (dynamic_cast<DocumentWidget *>(m_ui.tabWidget->widget(i))->getFileName() == fileName)
      return dynamic_cast<DocumentWidget *>(m_ui.tabWidget->widget(i));
  }
  return 0;
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

  document = this->createDocument(fileInfo.baseName());
  document->openFile(fileName);
}

void DocumentManager::saveFile(QString fileName)
{
  QFileInfo fileInfo(fileName);
  m_ui.tabWidget->setTabText(m_ui.tabWidget->currentIndex(), fileInfo.baseName());
  this->currentDocument()->saveFile(fileName);
}
