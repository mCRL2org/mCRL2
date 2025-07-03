// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_EXTENDEDTABWIDGET_H
#define MCRL2_UTILITIES_EXTENDEDTABWIDGET_H

#include <QTabWidget>

namespace mcrl2::gui::qt
{

class ExtendedTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     * @param parent The parent QWidget for the tabwidget
     */
    explicit ExtendedTabWidget(QWidget *parent = nullptr);

    /**
     * @brief Used to create context menu for tabbar
     */
    bool eventFilter(QObject *target, QEvent *event);

    /**
     * @brief Used to close tabs with shortcut
     */
    void keyPressEvent(QKeyEvent * event);
signals:

public slots:

};

} // namespace mcrl2::gui::qt

#endif // MCRL2_UTILITIES_EXTENDEDTABWIDGET_H
