// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QDialog>
#include "ui_copydialog.h"

class CopyDialog : public QDialog
{
    Q_OBJECT
    
  public:
    explicit CopyDialog(QWidget *parent = 0);

    void init(int count, bool move = false);

  public slots:
    void setFile(int num, QString filename);


  private:
    Ui::CopyDialog m_ui;
    int m_count;
    bool m_move;

};

#endif // COPYDIALOG_H
