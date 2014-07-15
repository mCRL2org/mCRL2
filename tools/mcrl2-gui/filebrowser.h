// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QDesktopServices>
#include <QUrl>
#include <QFileSystemModel>
#include <QContextMenuEvent>
#include <QFileInfo>

#include "toolcatalog.h"
#include "toolaction.h"
#include "toolinformation.h"

#include "copydialog.h"
#include "copythread.h"

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QTreeView>

class FileBrowser : public QTreeView
{
    Q_OBJECT
  public:
    explicit FileBrowser(QWidget *parent = 0);
    ~FileBrowser();
    void setCatalog(ToolCatalog catalog);
    QMenu* menu() { return m_menu; }

  public slots:
    void onToolSelected();
    void onRemoveRequested(QString filename = QString());

  protected slots:
    void onNewFile();
    void onNewFolder();
    void onOpenFiles();
    void onDeleteFiles();
    void onCutFiles();
    void onCopyFiles();
    void onPasteFiles();
    void onRenameFile();
    void onFileProperties();
    void onContextMenu();

  signals:
    void openToolInstance(QString filename, ToolInformation tool);
    void openProperties(QString filename);

  protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

  private:
    enum CopyMode { cm_none, cm_copy, cm_cut };

    void freeToolActions();
    void rememberSelection();
    QAction* addConditionalAction(QMenu* menu, QString action, bool condition);
    void createContextMenu(QList<QFileInfo> info);
    void copyDirectory(QString oldPath, QString newPath, bool move = false);
    bool askRemove(QModelIndexList files);
    bool askRemove(QString filename, bool copy = false);

    QFileSystemModel m_model;

    QMenu* m_menu;
    QAction* m_actOpenFiles;
    QAction* m_sep1;
    QAction* m_actNewFile;
    QAction* m_actNewFolder;
    QAction* m_sep2;
    QList<QMenu*> m_categories;
    QList<ToolAction*> m_toolActions;
    QAction* m_sep3;
    QAction* m_actDeleteFiles;
    QAction* m_actCutFiles;
    QAction* m_actCopyFiles;
    QAction* m_actPasteFiles;
    QAction* m_actRenameFile;
    QAction* m_actFileProperties;

    ToolCatalog m_catalog;

    QStringList m_selectedFiles;
    CopyMode m_copyMode;

    CopyThread m_copythread;
    CopyDialog m_copydialog;
    
};

#endif // FILEBROWSER_H
