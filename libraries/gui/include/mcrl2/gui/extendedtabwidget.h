// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_EXTENDEDTABWIDGET_H
#define MCRL2_UTILITIES_EXTENDEDTABWIDGET_H

#include <QTabWidget>


namespace mcrl2
{

namespace utilities
{

namespace qt
{

class ExtendedTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     * @param parent The parent QWidget for the tabwidget
     */
    explicit ExtendedTabWidget(QWidget *parent = 0);

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

} // namespace qt

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_EXTENDEDTABWIDGET_H
