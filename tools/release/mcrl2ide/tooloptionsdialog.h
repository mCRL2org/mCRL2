// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLOPTIONSDIALOG_H
#define TOOLOPTIONSDIALOG_H

#include <QDialog>

namespace Ui
{
  class ToolOptionsDialog;
};

/**
 * @brief The AddEditPropertyDialog class defines the dialog used to add or edit
 *   a property
 */
class ToolOptionsDialog : public QDialog
{
  Q_OBJECT

public:
    ToolOptionsDialog(QWidget* parent);


private:  
  Ui::ToolOptionsDialog* ui;

};

#endif // TOOLOPTIONSDIALOG_H