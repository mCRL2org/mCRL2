// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MOVABLETABLEWIDGET_H
#define MOVABLETABLEWIDGET_H

#include <QTableWidget>
#include <QDropEvent>

class MovableTableWidget : public QTableWidget
{
    Q_OBJECT
  public:
    explicit MovableTableWidget(QWidget *parent = 0);

  protected:
    int sizeHintForColumn(int column);
    void paintEvent(QPaintEvent *e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    int dropRow(QPoint pos);

  private:
    int m_lineRow;

  signals:
    
  public slots:
    void resizeColumnToContents(int column);
    void resizeColumnsToContents();

};

#endif // MOVABLETABLEWIDGET_H
