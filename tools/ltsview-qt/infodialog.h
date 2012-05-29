// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include "ui_infodialog.h"
#include "ltsmanager.h"
#include "markmanager.h"

class InfoDialog : public QDialog
{
  Q_OBJECT

  public:
    InfoDialog(QWidget *parent, LtsManager *ltsManager, MarkManager *markManager);

  protected slots:
    void ltsChanged();
    void markStatisticsChanged();
    void selectionChanged();

  private:
    Ui::InfoDialog m_ui;
    LtsManager *m_ltsManager;
    MarkManager *m_markManager;
};

#endif
