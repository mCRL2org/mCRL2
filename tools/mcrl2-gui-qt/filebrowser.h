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
    void setCatalog(ToolCatalog catalog);

  public slots:
    void onToolSelected();
    void onRemoveRequested(QString filename = QString());

  signals:
    void openToolInstance(QString filename, ToolInformation tool);

  protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

  private:
    void createContextMenu(QFileInfo info);
    void copyDirectory(QString oldPath, QString newPath, bool move = false);
    bool askOverwrite(QString filename);


    QFileSystemModel m_model;
    QMenu *m_context;
    ToolCatalog m_catalog;

    QModelIndex m_pastefile;
    bool m_cut;
    bool m_copy;

    CopyThread m_copythread;
    CopyDialog m_copydialog;
    
};

#endif // FILEBROWSER_H
