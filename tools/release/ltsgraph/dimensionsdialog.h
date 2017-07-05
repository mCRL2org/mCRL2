// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file dimensionsdialog.h
  @author R. Boudewijns

  This file contains an QDialog which can be used to ask the user for image dimensions

*/

#ifndef DIMENSIONSDIALOG_H
#define DIMENSIONSDIALOG_H

#include <QDialog>
#include "ui_dimensionsdialog.h"

class DimensionsDialog : public QDialog
{
    Q_OBJECT

  public:

    /**
     * @brief Constructor which creates the dialog for the given parent.
     */
    explicit DimensionsDialog(QWidget* parent = nullptr);

    /**
     * @brief Returns the supplied width, only valid if exec() returned QDialog::Accepted.
     */
    int resultWidth();

    /**
     * @brief Returns the supplied height, only valid if exec() returned QDialog::Accepted.
     */
    int resultHeight();

  private:
    Ui::DimensionsDialog m_ui;     ///< The user inferface of the class.

};

#endif // DIMENSIONSDIALOG_H
