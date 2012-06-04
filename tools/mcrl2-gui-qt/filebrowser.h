// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QFileSystemModel>

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QTreeView>

class FileBrowser : public QTreeView
{
    Q_OBJECT
  public:
    explicit FileBrowser(QWidget *parent = 0);

  private:
    QFileSystemModel m_model;
    
};

#endif // FILEBROWSER_H
