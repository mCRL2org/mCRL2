// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/extendedtabwidget.h"
#include <QTabBar>
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

using namespace mcrl2::utilities::qt;

ExtendedTabWidget::ExtendedTabWidget(QWidget *parent) :
  QTabWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);
  tabBar()->installEventFilter(this);
}

bool ExtendedTabWidget::eventFilter(QObject *target, QEvent *event) {
  if(target == tabBar()) {
    if(event->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
      if (mouseEvent->button() == Qt::RightButton)
      {
        QPoint position = mouseEvent->pos();
        int clickedItem = -1;
        for (int i=0; i < tabBar()->count(); i++)
        {
          if ( tabBar()->tabRect(i).contains( position ) )
          {
            clickedItem = i;
            break;
          }
        }
        if (clickedItem != -1)
        {
          QMenu menu(this);
          menu.addAction("Close tab");
          menu.addSeparator();
          menu.addAction("Close all tabs");
          menu.addAction("Close other tabs");
          menu.addAction("Close tabs on the left");
          menu.addAction("Close tabs on the right");
          QAction* act = menu.exec(mapToGlobal(position));
          if (act != 0)
          {
            if (act->text() == "Close tab")
            {
              emit(tabCloseRequested(clickedItem));
            }
            if (act->text() == "Close all tabs")
            {
              for (int i = count()-1; i >= 0; i--)
              {
                  emit(tabCloseRequested(i));
              }
            }
            if (act->text() == "Close other tabs")
            {
              for (int i = count()-1; i >= 0; i--)
              {
                if (i != clickedItem)
                  emit(tabCloseRequested(i));
              }
            }
            if (act->text() == "Close tabs on the left")
            {
              for (int i = clickedItem-1; i >= 0; i--)
              {
                emit(tabCloseRequested(i));
              }
            }
            if (act->text() == "Close tabs on the right")
            {
              for (int i = count()-1; i > clickedItem; i--)
              {
                emit(tabCloseRequested(i));
              }
            }
          }
          return true;
        }
      }
    }
  }
  return false;
}

void ExtendedTabWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->matches(QKeySequence::Close) && currentIndex() != -1)
  {
    emit(tabCloseRequested(currentIndex()));
  }
  else
  {
    QTabWidget::keyPressEvent(event);
  }
}
