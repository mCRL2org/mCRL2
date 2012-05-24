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

class InfoDialog : public QDialog
{
  Q_OBJECT

  public:
    InfoDialog(QWidget *parent);

  public slots:
    void setLTSInfo(int states, int transitions, int clusters, int ranks);
    void setNumMarkedStates(int markedStates);
    void setNumMarkedTransitions(int markedTransitions);
    void setParameterNames(QStringList names);
    void setParameterValue(int parameter, QString value);
    void setParameterValues(int parameter, QStringList values);
    void setStatesInCluster(int states);
    void resetParameterNames();
    void resetParameterValues();

  private:
    Ui::InfoDialog m_ui;
};

#endif
