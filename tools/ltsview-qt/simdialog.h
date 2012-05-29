// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMDIALOG_H
#define SIMDIALOG_H

#include <QDialog>
#include "ui_simdialog.h"

#include "ltsmanager.h"
#include "simulation.h"
#include "state.h"
#include "transition.h"

class SimDialog : public QDialog
{
  Q_OBJECT

  public:
    SimDialog(QWidget *parent, LtsManager *ltsManager);

  public slots:
    void changed();
    void selectionChanged();

  protected slots:
    void start();
    void stop();
    void backtrace();
    void reset();
    void trigger();
    void undo();
    void select();

  private:
    Ui::SimDialog m_ui;
    LtsManager *m_ltsManager;
};

#endif
