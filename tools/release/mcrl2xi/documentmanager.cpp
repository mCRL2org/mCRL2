// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "documentmanager.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "documentwidget.h"

DocumentManager::DocumentManager(QWidget *parent) :
  ExtendedTabWidget(parent)
{
  setTabsClosable(true);
  setDocumentMode(true);
  setMovable(true);
  connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

void DocumentManager::showEvent(QShowEvent* /*event*/)
{
  // This cannot be done in the constructor because
  // the documentCreated signal is fired before the connection can be made
  if (count() == 0)
    newFile();
}

DocumentWidget* DocumentManager::createDocument(QString title)
{

  DocumentWidget *document = new DocumentWidget(this, m_atermThread, m_strategy);
  addTab(document, title);

  emit(documentCreated(document));

  setCurrentWidget(document);
  return document;
}

DocumentWidget* DocumentManager::getDocument(int index)
{
  if (index < 0 || index >= count())
    return 0;
  return dynamic_cast<DocumentWidget *>(widget(index));
}

DocumentWidget* DocumentManager::findDocument(QString fileName)
{
  for (int i = 0; i < count(); i++) {
    if (getDocument(i)->getFileName() == fileName)
      return getDocument(i);
  }
  return 0;
}

void DocumentManager::closeDocument(int index)
{
  if (index < 0 || index >= count())
    return;

  DocumentWidget* document = getDocument(index);
  if (document != 0)
  {
    emit documentClosed(document);
    document->deleteLater();
    removeTab(index);

    if (count() == 0)
      newFile();
  }
}

void DocumentManager::updateTitle()
{
  DocumentWidget* document = currentDocument();
  if(document->getFileName() != NULL)
  {
    QFileInfo info(document->getFileName());
    setTabText(currentIndex(), (document->isModified() ? "*" : "") + info.baseName());
  }
}

DocumentWidget* DocumentManager::currentDocument()
{
  return dynamic_cast<DocumentWidget *>(currentWidget());
}

QString DocumentManager::currentFileName()
{
  DocumentWidget* document = currentDocument();
  if (document == 0)
  {
    return QString("");
  }
  return document->getFileName();
}

void DocumentManager::newFile()
{
  createDocument(tr("Untitled"));
}

void DocumentManager::openFile(QString fileName)
{

  QFileInfo fileInfo(fileName);
  DocumentWidget *document = findDocument(fileName);

  if (document) {
    setCurrentWidget(document);
    return;
  }

  for (int i = 0; i < count(); i++)
  {
    if (getDocument(i)->getFileName() == QString() && !getDocument(i)->isModified())
    {
      document = getDocument(i);
      setCurrentWidget(document);
      setTabText(indexOf(document), fileInfo.baseName());
      break;
    }
  }

  if (document == 0)
    document = createDocument(fileInfo.baseName());

  document->openFile(fileName);
}

void DocumentManager::saveFile(int index, QString fileName)
{
  QFileInfo fileInfo(fileName);
  setTabText(index, fileInfo.baseName());
  getDocument(index)->saveFile(fileName);
}
