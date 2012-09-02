// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "copydialog.h"

CopyDialog::CopyDialog(QWidget *parent) :
  QDialog(parent),
  m_move(false)
{
  m_ui.setupUi(this);
}

void CopyDialog::init(int count, bool move)
{
   m_count = count;
   m_move = move;
}

void CopyDialog::setFile(int num, QString filename)
{
  QString caption = (m_move ? tr("Moving: '%1'") : tr("Copying: '%1'"));
  if (0 < num && num <= m_count)
    m_ui.pbFiles->setValue(100*num/m_count);
  m_ui.lblCopy->setText(caption.arg(filename));
}
