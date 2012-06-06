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

#include <QDebug>

FileBrowser::FileBrowser(QWidget *parent) :
  QTreeView(parent), m_model(this), m_context(NULL), m_cut(false), m_copy(false), m_copydialog(parent)
{
  connect(&m_copydialog, SIGNAL(rejected()), &m_copythread, SLOT(cancel()));
  connect(&m_copythread, SIGNAL(started()), &m_copydialog, SLOT(open()));
  connect(&m_copythread, SIGNAL(busy(int,QString)), &m_copydialog, SLOT(setFile(int,QString)));
  connect(&m_copythread, SIGNAL(finished()), &m_copydialog, SLOT(close()));
  connect(&m_copythread, SIGNAL(remove(QString)), this, SLOT(onRemoveRequested(QString)));

  m_model.setReadOnly(false);

  m_model.setRootPath(QDir::rootPath());
  m_model.sort(0);
  setModel(&m_model);

  setColumnHidden( 1, true );
  setColumnHidden( 2, true );
  setColumnHidden( 3, true );

  setSortingEnabled(true);

  //setCurrentIndex(m_model.index(QDir::currentPath()));

}

void FileBrowser::setCatalog(ToolCatalog catalog)
{
  m_catalog = catalog;
}

void FileBrowser::createContextMenu(QFileInfo info)
{
  delete m_context;
  m_context = new QMenu(this);

  if (info.isFile())
  {
    m_context->addAction("Open");
    m_context->addSeparator();

    QStringList cats = m_catalog.categories();
    for (int i = 0; i < cats.size(); i++)
    {
      QMenu *menuCat = new QMenu(m_context);
      menuCat->setTitle(cats.at(i));
      m_context->addMenu(menuCat);
      QList<ToolInformation> tools = m_catalog.tools(cats.at(i), info.suffix());
      for (int i = 0; i < tools.count(); i++)
      {
        ToolInformation tool = tools.at(i);
        ToolAction* actTool = new ToolAction(tool, menuCat);
        menuCat->addAction(actTool);
        connect(actTool, SIGNAL(triggered()), this, SLOT(onToolSelected()));
      }
      if (tools.count() == 0)
      {
        menuCat->setEnabled(false);
      }
    }

    m_context->addSeparator();
  }
  if (info.isDir())
  {
    m_context->addAction("New File");
    m_context->addAction("New Directory");
    m_context->addSeparator();
  }
  QAction *cut = m_context->addAction("Cut");
  QAction *copy = m_context->addAction("Copy");
  if (QDir::drives().contains(info.absoluteFilePath())) {
    cut->setEnabled(false);
    copy->setEnabled(false);
  }
  QAction *paste = m_context->addAction("Paste");
  if (!info.isDir() || (!m_cut && !m_copy))
  {
    paste->setEnabled(false);
  }
  m_context->addSeparator();
  m_context->addAction("Rename");
  m_context->addAction("Delete");
  m_context->addSeparator();
  m_context->addAction("Properties");

}

void FileBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  if (currentIndex().isValid())
  {
    event->accept();

    QString file = m_model.filePath(currentIndex());
    createContextMenu(QFileInfo(file));
    QAction* act = m_context->exec(event->globalPos());

    if (act != NULL)
    {
      if (act->text() == "Open")
      {
        QDesktopServices::openUrl(QUrl(file));
      }
      if (act->text() == "New File")
      {
        QDir dir = QDir(file);
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
      if (act->text() == "New Directory")
      {
        QDir dir = QDir(file);
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
      if (act->text() == "Cut")
      {
        m_cut = true; m_copy = false;
        m_pastefile = currentIndex();
      }
      if (act->text() == "Copy")
      {
        m_cut = false; m_copy = true;
        m_pastefile = currentIndex();
      }
      if (act->text() == "Paste")
      {
        if (m_pastefile.isValid())
        {
          QString newName = QDir(m_model.filePath(currentIndex())).absoluteFilePath(m_model.fileName(m_pastefile));
          if (m_cut)
          {
            if (askOverwrite(newName))
            {
              if (!QFile::rename(m_model.filePath(m_pastefile), newName)) // Quick rename failed
                copyDirectory(m_model.filePath(m_pastefile), newName, true);
              m_cut = false;
              m_pastefile = QModelIndex();
            }
          }
          else if (m_copy)
          {
            if (askOverwrite(newName))
            {
              if (QFileInfo(m_model.filePath(m_pastefile)).isFile())
              {
                QFile::copy(m_model.filePath(m_pastefile), newName);
              }
              else
              {
                copyDirectory(m_model.filePath(m_pastefile), newName);
              }
            }
          }
        }
      }
      if (act->text() == "Rename")
      {
        edit(currentIndex());
      }
      if (act->text() == "Delete")
      {
        qDebug() << "Delete";
        m_model.remove(currentIndex());
      }
    }
  }
}

void FileBrowser::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (currentIndex().isValid())
  {
    QString file = m_model.filePath(currentIndex());
    if (QFileInfo(file).isFile())
    {
      QDesktopServices::openUrl(QUrl(file));
      event->accept();
    }
    else
    {
      QTreeView::mouseDoubleClickEvent(event);
    }
  }
}

void FileBrowser::onToolSelected()
{
  ToolAction* act = dynamic_cast<ToolAction*>(QObject::sender());
  emit(openToolInstance(m_model.filePath(currentIndex()), act->information()));
}

void FileBrowser::onRemoveRequested(QString filename)
{
  QModelIndex index = m_model.index(filename);
  if (!index.isValid())
    index = currentIndex();

  if (index.isValid())
  {
    m_model.remove(index);
  }
}

bool FileBrowser::askOverwrite(QString filename)
{
  if (QFile::exists(filename))
  {
    QString message = (QFileInfo(filename).isDir() ? tr("Do you want to overwrite all files in %1?") : tr("Do you want to overwrite %1?"));

    QMessageBox::StandardButton ret = QMessageBox::question ( this, tr("File Exists"), message.arg(filename), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (ret == QMessageBox::Yes)
    {
      QFile::remove(filename);
      return true;
    }
    return false;
  }
  return true;
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



