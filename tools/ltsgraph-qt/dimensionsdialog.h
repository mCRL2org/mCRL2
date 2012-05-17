// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DIMENSIONSDIALOG_H
#define DIMENSIONSDIALOG_H

#include <QDialog>

namespace Ui {
  class DimensionsDialog;
}

class DimensionsDialog : public QDialog
{
    Q_OBJECT
    
  public:
    explicit DimensionsDialog(QWidget *parent = 0);
    ~DimensionsDialog();
    int resultWidth();
    int resultHeight();
    
  private:
    Ui::DimensionsDialog *m_ui;

};

#endif // DIMENSIONSDIALOG_H
