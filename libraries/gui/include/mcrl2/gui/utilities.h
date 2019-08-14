// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_GUI_UTILITIES_H
#define MCRL2_GUI_UTILITIES_H

#include <QWidget>
#include <QTextEdit>

namespace mcrl2
{

namespace gui
{

namespace qt
{

/**
 * @brief hasLightBackground Checks whether a widget has a light background
 * @param widget The widget to check the background of
 * @return Whether a widget has a light background
 */
bool hasLightBackground(QWidget* widget);

/**
 * @brief setTextEditTextColor Sets the text colour of a QTextEdit depending on
 *   whether the QTextEdit has a light background
 * @param textEdit The QTextEdit to set the text colour of
 * @param light The text colour in case of a light background
 * @param dark The text colour in case of a dark backgorund
 */
void setTextEditTextColor(QTextEdit* textEdit, QColor light, QColor dark);

} // namespace qt

} // namespace gui

} // namespace mcrl2

#endif // MCRL2_GUI_UTILITIES_H
