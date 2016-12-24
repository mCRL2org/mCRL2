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
  QTreeView(parent), m_model(this), m_menu(NULL), m_copyMode(cm_none), m_copydialog(parent)
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

  m_menu = new QMenu("&File", this);
  m_actOpenFiles = m_menu->addAction("Open");
  m_sep1 = m_menu->addSeparator();
  m_actNewFile = m_menu->addAction("&New file...");
  m_actNewFolder = m_menu->addAction("New &folder...");
  m_sep2 = m_menu->addSeparator();
  m_sep3 = m_menu->addSeparator();
  m_actDeleteFiles = m_menu->addAction("&Delete");
  m_actDeleteFiles->setShortcut(QKeySequence::Delete);
  m_actCutFiles = m_menu->addAction("Cu&t");
  m_actCutFiles->setShortcut(QKeySequence::Cut);
  m_actCopyFiles = m_menu->addAction("&Copy");
  m_actCopyFiles->setShortcut(QKeySequence::Copy);
  m_actPasteFiles = m_menu->addAction("&Paste");
  m_actPasteFiles->setShortcut(QKeySequence::Paste);
  m_actRenameFile = m_menu->addAction("&Rename...");
  m_actFileProperties = m_menu->addAction("Pr&operties...");

  connect(m_actNewFile, SIGNAL(triggered()), this, SLOT(onNewFile()));
  connect(m_actNewFolder, SIGNAL(triggered()), this, SLOT(onNewFolder()));
  connect(m_actOpenFiles, SIGNAL(triggered()), this, SLOT(onOpenFiles()));
  connect(m_actDeleteFiles, SIGNAL(triggered()), this, SLOT(onDeleteFiles()));
  connect(m_actCutFiles, SIGNAL(triggered()), this, SLOT(onCutFiles()));
  connect(m_actCopyFiles, SIGNAL(triggered()), this, SLOT(onCopyFiles()));
  connect(m_actPasteFiles, SIGNAL(triggered()), this, SLOT(onPasteFiles()));
  connect(m_actRenameFile, SIGNAL(triggered()), this, SLOT(onRenameFile()));
  connect(m_actFileProperties, SIGNAL(triggered()), this, SLOT(onFileProperties()));
  connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(onContextMenu()));
}

FileBrowser::~FileBrowser()
{
  freeToolActions();
  delete m_menu;
  delete m_actOpenFiles;
  delete m_sep1;
  delete m_actNewFile;
  delete m_actNewFolder;
  delete m_sep2;
  delete m_sep3;
  delete m_actCopyFiles;
  delete m_actCutFiles;
  delete m_actPasteFiles;
  delete m_actRenameFile;
  delete m_actFileProperties;
}

void FileBrowser::freeToolActions()
{
  for (auto cat = m_categories.begin(); cat != m_categories.end(); ++cat)
  {
    m_menu->removeAction((*cat)->menuAction());
    QList<QAction*> actions = (*cat)->actions();
    for (auto action = actions.begin(); action != actions.end(); ++action)
    {
      delete *action;
    }
    delete *cat;
  }
}

void FileBrowser::setCatalog(ToolCatalog catalog)
{
  freeToolActions();
  m_catalog = catalog;
  QStringList categories = m_catalog.categories();
  for (auto cat = categories.begin(); cat != categories.end(); ++cat)
  {
    QMenu* catMenu = new QMenu(*cat, m_menu);
    m_menu->insertMenu(m_sep3, catMenu);
    QList<ToolInformation> tools = m_catalog.tools(*cat);
    for (auto tool = tools.begin(); tool != tools.end(); ++tool)
    {
      catMenu->addAction(new ToolAction(*tool, catMenu));
      connect(catMenu->actions().back(), SIGNAL(triggered()), this, SLOT(onToolSelected()));
    }
    m_categories.append(catMenu);
  }
}

void FileBrowser::rememberSelection()
{
  QModelIndexList indexes = selectedIndexes();
  m_selectedFiles.clear();
  for (int i = 0; i < selectedIndexes().size(); i++)
  {
    m_selectedFiles.append(m_model.filePath(indexes[i]));
  }
}

void FileBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  m_menu->popup(event->globalPos());
}

void FileBrowser::onContextMenu()
{
  bool selection = !selectedIndexes().empty();
  bool singleton = selectedIndexes().size() == 1;

  m_actOpenFiles->setVisible(selection);
  m_sep1->setVisible(selection);

  bool toolsVisible = false;
  if (singleton)
  {
    QString ext = QFileInfo(m_model.filePath(selectedIndexes()[0])).suffix();
    for (auto cat = m_categories.begin(); cat != m_categories.end(); ++cat)
    {
      bool catVisible = false;
      QList<QAction*> actions = (*cat)->actions();
      for (auto tool = actions.begin(); tool != actions.end(); ++tool)
      {
        if (dynamic_cast<ToolAction*>(*tool)->information().inputMatchesAny(m_catalog.fileTypes(ext)))
        {
          (*tool)->setVisible(true);
          catVisible = true;
        }
        else
        {
          (*tool)->setVisible(false);
        }
      }
      toolsVisible = toolsVisible || catVisible;
      (*cat)->menuAction()->setVisible(catVisible);
    }
  }
  m_sep2->setVisible(toolsVisible);

  m_actDeleteFiles->setVisible(selection);
  m_actCutFiles->setVisible(selection);
  m_actCopyFiles->setVisible(selection);
  m_actPasteFiles->setVisible(selection);
  m_actRenameFile->setVisible(singleton);
  m_actFileProperties->setVisible(singleton);
}


void FileBrowser::onToolSelected()
{
  QModelIndexList indexes = selectedIndexes();
  if (!indexes.empty())
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
    if (!dir.exists())
    {
      dir = QFileInfo(m_model.filePath(indexes[0])).absoluteDir();
    }
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
    if (!dir.exists())
    {
      dir = QFileInfo(m_model.filePath(indexes[0])).absoluteDir();
    }
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


void FileBrowser::onOpenFiles()
{
  QModelIndexList indexes = selectedIndexes();
  for (auto i = indexes.begin(); i != indexes.end(); ++i)
  {
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_model.filePath(*i)));
  }
}

void FileBrowser::onDeleteFiles()
{
  QModelIndexList indexes = selectedIndexes();
  if (askRemove(indexes))
  {
    for (auto i = indexes.begin(); i != indexes.end(); ++i)
    {
      m_model.remove(*i);
    }
  }
}

void FileBrowser::onCutFiles()
{
  rememberSelection();
  m_copyMode = cm_cut;
}

void FileBrowser::onCopyFiles()
{
  rememberSelection();
  m_copyMode = cm_copy;
}

void FileBrowser::onPasteFiles()
{
  if (selectedIndexes().empty())
  {
    return;
  }

  QDir targetDir = QDir(m_model.filePath(selectedIndexes()[0]));
  m_copyMode = cm_none;
  for (auto file = m_selectedFiles.begin(); file != m_selectedFiles.end(); ++file)
  {
    QModelIndex pastefile = m_model.index(*file);
    if (pastefile.isValid())
    {
      QString newName = targetDir.absoluteFilePath(m_model.fileName(pastefile));
      if (askRemove(newName, true))
      {
        switch (m_copyMode)
        {
        case cm_cut:
            QFile::remove(newName);
            if (!QFile::rename(*file, newName))
              copyDirectory(*file, newName, true);
            break;
        case cm_copy:
            copyDirectory(*file, newName);
            break;
        default:
            break;
        }
      }
    }
  }
}

void FileBrowser::onRenameFile()
{
  QModelIndexList indexes = selectedIndexes();
  if (!indexes.empty())
  {
    edit(indexes[0]);
  }
}

void FileBrowser::onFileProperties()
{
  QModelIndexList indexes = selectedIndexes();
  if (!indexes.empty())
  {
    emit(openProperties(m_model.filePath(indexes[0])));
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
      onOpenFiles();
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
        onDeleteFiles();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
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



