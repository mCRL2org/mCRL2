// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/gui/utilities.h"

bool mcrl2::gui::qt::hasLightBackground(QWidget* widget)
{
  return widget->palette().window().color().lightness() >= 128;
}

void mcrl2::gui::qt::setTextEditTextColor(QTextEdit* textEdit, QColor light,
                                          QColor dark)
{
  textEdit->setTextColor(mcrl2::gui::qt::hasLightBackground(textEdit) ? light
                                                                      : dark);
}
