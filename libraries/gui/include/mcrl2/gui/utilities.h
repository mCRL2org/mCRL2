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

#include <QList>
#include <QWidget>
#include <QTextEdit>

namespace mcrl2::gui::qt
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

/**
 * @brief Implements QList<T>(begin, end) for Qt versions before 5.14
 */
template<typename T, typename Iterator>
QList<T> makeQList(Iterator begin, Iterator end)
{
    QList<T> result;
    for (auto it = begin; it != end; ++it)
    {
        result.append(*it);
    }
    return result;
}

/**
 * @brief Implements QList<T>.swapItemsAt(from, to) for Qt versions before 5.13
 */
template<typename T>
void swapItemsAt(QList<T>& input, std::size_t from, std::size_t to)
{
#if QT_VERSION < QT_VERSION_CHECK(5,13,0)
    input.swap(from, to);
#else
    input.swapItemsAt(from, to);
#endif
}

} // namespace mcrl2::gui::qt

#endif // MCRL2_GUI_UTILITIES_H
