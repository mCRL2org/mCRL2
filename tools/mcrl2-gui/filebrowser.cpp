// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "filebrowser.h"
#include <QMenu>
#include <QAction>
#include <QMessageBox>

FileBrowser::FileBrowser(QWidget *parent) :
  QTreeView(parent), m_model(this), m_context(NULL), m_cut(false), m_copy(false), m_copydialog(parent)
{
  connect(&m_copydialog, SIGNAL(rejected()), &m_copythread, SLOT(cancel()));
  connect(&m_copythread, SIGNAL(started()), &m_copydialog, SLOT(open()));
  connect(&m_copythread, SIGNAL(busy(int,QString)), &m_copydialog, SLOT(setFile(int,QString)));
  connect(&m_copythread, SIGNAL(finished()), &m_copydialog, SLOT(close()));
  connect(&m_copythread, SIGNAL(remove(QString)), this, SLOT(onRemoveRequested(QString)));

  m_model.setReadOnly(false);

#ifdef Q_OS_WIN32
  m_model.setRootPath(QString());
#else
  m_model.setRootPath(QDir::rootPath());
#endif

  m_model.sort(0);

  setModel(&m_model);

  setColumnHidden( 1, true );
  setColumnHidden( 2, true );
  setColumnHidden( 3, true );

  setSortingEnabled(true);
  sortByColumn(0, Qt::AscendingOrder);

  setCurrentIndex(m_model.index(QDir::currentPath()));
}


void FileBrowser::onToolSelected()
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() == 1)
  {
    ToolAction* act = dynamic_cast<ToolAction*>(QObject::sender());
    emit(openToolInstance(m_model.filePath(indexes[0]), act->information()));
  }
}


void FileBrowser::onRemoveRequested(QString filename)
{
  QModelIndex index = m_model.index(filename);
  if (index.isValid())
  {
    m_model.remove(index);
  }
}


void FileBrowser::onNewFile()
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() == 1)
  {
    QDir dir = QDir(m_model.filePath(indexes[0]));
    QString newfile("new");
    int filenr = 0;
    while(dir.exists(newfile))
    {
      filenr++;
      newfile = QString("new_%1").arg(filenr);
    }
    QFile(dir.absoluteFilePath(newfile)).open(QIODevice::WriteOnly);

    QModelIndex index = m_model.index(dir.absoluteFilePath(newfile));
    setCurrentIndex(index);
    edit(index);
  }
}


void FileBrowser::onNewFolder()
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() == 1)
  {
    QDir dir = QDir(m_model.filePath(indexes[0]));
    QString newfile("new");
    int filenr = 0;
    while(dir.exists(newfile))
    {
      filenr++;
      newfile = QString("new_%1").arg(filenr);
    }
    dir.mkdir(newfile);

    QModelIndex index = m_model.index(dir.absoluteFilePath(newfile));
    setCurrentIndex(index);
    edit(index);
  }
}


void FileBrowser::onOpenFile()
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() == 1)
  {
    QString file = m_model.filePath(indexes[0]);
    if (QFileInfo(file).isFile())
    {
      QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }
  }
}


void FileBrowser::onDeleteFile()
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() > 0 && askRemove(indexes))
  {
    for (int i = 0; i < indexes.size(); i++)
    {
        m_model.remove(indexes[i]);
    }
  }
}


void FileBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() > 0)
  {
    event->accept();

    QStringList files;
    QList<QFileInfo> fileinfos;
    for (int i = 0; i < indexes.size(); i++)
    {
      files.append(m_model.filePath(indexes[i]));
      fileinfos.append(QFileInfo(files.last()));
    }
    createContextMenu(fileinfos);
    QAction* act = m_context->exec(event->globalPos());

    if (act != NULL)
    {
      if (act->text() == "Open" && indexes.size() == 1)
      {
        QDesktopServices::openUrl(QUrl(files[0]));
      }
      if (act->text() == "New File" && indexes.size() == 1)
      {
        onNewFile();
      }
      if (act->text() == "New Directory" && indexes.size() == 1)
      {
        onNewFolder();
      }
      if (act->text() == "Cut")
      {
        m_cut = true; m_copy = false;
        m_pastefiles = files;
      }
      if (act->text() == "Copy")
      {
        m_cut = false; m_copy = true;
        m_pastefiles = files;
      }
      if (act->text() == "Paste" && indexes.size() == 1)
      {
        for (int i = 0; i < m_pastefiles.size(); i++)
        {
          QModelIndex m_pastefile = m_model.index(m_pastefiles[i]);
          if (m_pastefile.isValid())
          {
            QString newName = QDir(m_model.filePath(indexes[0])).absoluteFilePath(m_model.fileName(m_pastefile));
            if (m_cut)
            {
              if (askRemove(newName, true))
              {
                QFile::remove(newName);
                if (!QFile::rename(m_model.filePath(m_pastefile), newName)) // Quick rename failed
                  copyDirectory(m_model.filePath(m_pastefile), newName, true);
                m_cut = false;
                m_pastefile = QModelIndex();
              }
            }
            else if (m_copy)
            {
              if (askRemove(newName, true))
              {
                copyDirectory(m_model.filePath(m_pastefile), newName);
              }
            }
          }
        }
      }
      if (act->text() == "Rename" && indexes.size() == 1)
      {
        edit(indexes[0]);
      }
      if (act->text() == "Delete")
      {
        onDeleteFile();
      }
      if (act->text() == "Properties" && indexes.size() == 1)
      {
        emit(openProperties(m_model.filePath(indexes[0])));
      }
    }
  }
}


void FileBrowser::mouseDoubleClickEvent(QMouseEvent *event)
{
  QModelIndexList indexes = selectedIndexes();
  if (indexes.size() == 1)
  {
    QString file = m_model.filePath(indexes[0]);
    if (QFileInfo(file).isFile())
    {
      onOpenFile();
      event->accept();
    }
    else
    {
      QTreeView::mouseDoubleClickEvent(event);
    }
  }
}

void FileBrowser::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete))
    {
        onDeleteFile();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}


QAction *FileBrowser::addConditionalAction(QMenu *menu, QString action, bool condition)
{
  QAction* act = menu->addAction(action);
  act->setEnabled(condition);
  return act;
}


void FileBrowser::createContextMenu(QList<QFileInfo> info)
{
  delete m_context;
  m_context = new QMenu(this);
  if (info[0].isFile())
  {
    addConditionalAction(m_context, "Open", info.size() == 1);
    m_context->addSeparator();

    QStringList cats = m_catalog.categories();
    for (int i = 0; i < cats.size(); i++)
    {
      QMenu *menuCat = new QMenu(m_context);
      menuCat->setTitle(cats.at(i));
      m_context->addMenu(menuCat);
      QList<ToolInformation> tools = m_catalog.tools(cats.at(i), info[0].suffix());
      for (int i = 0; i < tools.count(); i++)
      {
        ToolInformation tool = tools.at(i);
        ToolAction* actTool = new ToolAction(tool, menuCat);
        menuCat->addAction(actTool);
        connect(actTool, SIGNAL(triggered()), this, SLOT(onToolSelected()));
      }
      menuCat->setEnabled(tools.count() > 0 && info.size() == 1);
    }

    m_context->addSeparator();
  }
  if (info[0].isDir())
  {
    addConditionalAction(m_context, "New File", info.size() == 1);
    addConditionalAction(m_context, "New Directory", info.size() == 1);
    m_context->addSeparator();
  }
  bool drives = false;
  for (int i = 0; i < info.size(); i++)
  {
    drives = drives || QDir::drives().contains(info[i].absoluteFilePath());
  }
  addConditionalAction(m_context, "Cut", !drives);
  addConditionalAction(m_context, "Copy", !drives);
  addConditionalAction(m_context, "Paste", info.size() == 1 && info[0].isDir() && (m_cut || m_copy));

  m_context->addSeparator();
  addConditionalAction(m_context, "Rename", info.size() == 1);
  m_context->addAction("Delete");
  m_context->addSeparator();
  addConditionalAction(m_context, "Properties", info.size() == 1);

}


void FileBrowser::copyDirectory(QString oldPath, QString newPath, bool move)
{
  QStringList all;
  QStringList todo(oldPath);
  if (QFileInfo(oldPath).isDir())
  {
    QDir oldDir(oldPath), curDir;
    while (!todo.empty())
    {
      QString curPath = todo.takeFirst();
      curDir.setPath(curPath);
      QStringList entries = curDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
      for (int i = 0; i < entries.size(); i++)
      {
        QString entry = curDir.absoluteFilePath(entries.at(i));
        if (QFileInfo(entry).isDir())
        {
          todo.append(entry);
        }
        all.append(oldDir.relativeFilePath(entry));
      }
    }
  }
  else
  {
    QFileInfo info(oldPath);
    oldPath = info.absoluteDir().absolutePath();
    all.append(info.fileName());
  }
  QMetaObject::invokeMethod(&m_copythread, "init", Qt::QueuedConnection, Q_ARG(QString, oldPath), Q_ARG(QString, newPath), Q_ARG(QStringList, all), Q_ARG(bool, move));
  m_copydialog.init(all.count(), move);
  m_copythread.start(QThread::IdlePriority);
}


bool FileBrowser::askRemove(QModelIndexList files)
{
  return (QMessageBox::question (this, tr("Are you sure?"), tr("Do you want to delete the %1 selected files?").arg(files.size()),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes);
}


bool FileBrowser::askRemove(QString filename, bool copy)
{
  if (QFile::exists(filename))
  {
    QString message(QFileInfo(filename).isDir() ? tr("Do you want to %1 all files in %2?") : tr("Do you want to %1 %2?"));
    message = message.arg(copy ? "overwrite" : "delete");

    QMessageBox::StandardButton ret = QMessageBox::question (this, tr("Are you sure?"), message.arg(filename), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    return (ret == QMessageBox::Yes);
  }
  return true;
}



