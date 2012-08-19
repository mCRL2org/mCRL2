// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EXTENDEDTABWIDGET_H
#define EXTENDEDTABWIDGET_H

#include <QTabWidget>

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

#endif // EXTENDEDTABWIDGET_H
