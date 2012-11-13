// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "movabletablewidget.h"
#include <QPainter>
#include <QHeaderView>

MovableTableWidget::MovableTableWidget(QWidget *parent) :
  QTableWidget(parent), m_lineRow(-1)
{
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setDragDropMode(QAbstractItemView::InternalMove);
  setDragDropOverwriteMode(false);
}

int MovableTableWidget::sizeHintForColumn(int column) const
{
  ensurePolished();

  QStyleOptionViewItem option = viewOptions();

  int hint = 0;
  QModelIndex index;
  for (int row = 0; row <= rowCount(); ++row) {

      index = indexFromItem(item(row, column));
      hint = qMax(hint, itemDelegate(index)->sizeHint(option, index).width());
  }

  return showGrid() ? hint + 1 : hint;
}

void MovableTableWidget::paintEvent(QPaintEvent *e)
{
  QTableWidget::paintEvent(e);

  if (!viewport()->paintingActive() && m_lineRow != -1 && columnCount() > 0 && rowCount() > 0)
  {
    int lineY = (m_lineRow < rowCount() ? visualItemRect(item(m_lineRow, 0)).top()-1 : visualItemRect(item(rowCount()-1, 0)).bottom()+1);
    int lineX = visualItemRect(item(0, columnCount()-1)).right();
    QPainter* painter = new QPainter(this->viewport());
    painter->setPen(QPen(QBrush(Qt::black), 3));
    painter->drawLine(QPoint(0, lineY), QPoint(lineX, lineY));
    painter->end();
  }
}

void MovableTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source() != this)
  {
    event->ignore();
    return;
  }

  event->accept();
}

void MovableTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->source() != this)
  {
    event->ignore();
    return;
  }

  if (hasAutoScroll())
  {
    QAbstractItemView::dragMoveEvent(event);
  }

  m_lineRow = dropRow(event->pos());
  viewport()->repaint();

  event->accept();
}

void MovableTableWidget::dropEvent(QDropEvent *event)
{
  if (event->source() != this)
  {
    event->ignore();
    return;
  }

  int targetRow = dropRow(event->pos());

  QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
  QDataStream stream(&encoded, QIODevice::ReadOnly);

  QMap<int, QMap<int, QString> > rows;
  while (!stream.atEnd())
  {
    int row, col;
    QMap<int,  QVariant> roleDataMap;
    stream >> row >> col >> roleDataMap;

    rows[row][col] = roleDataMap[0].toString();
  }

  QList<int> rowkeys = rows.keys();
  qSort(rowkeys);

  for (int row = rowkeys.count()-1; 0 <= row ; row--)
  {
    int rowIndex = rowkeys[row];
    if (rowIndex < targetRow)
    {
      targetRow--;
    }
    removeRow(rowIndex);
  }

  for (int row = 0; row < rowkeys.count(); ++row)
  {
    QMap<int, QString> cols = rows[rowkeys[row]];
    QList<int> colkeys = cols.keys();

    insertRow(targetRow+row);
    for (int col = 0; col < colkeys.count(); ++col)
    {
      QString value = cols[colkeys[col]];
      setItem(targetRow+row, col, new QTableWidgetItem(value));
      setItemSelected(item(targetRow+row, col), true);
    }
    emit(itemMoved(rowkeys[row],targetRow+row));
  }


  m_lineRow = -1;
}

int MovableTableWidget::dropRow(QPoint pos)
{
  int result = rowAt(pos.y());
  if (result == -1)
  {
    result = rowCount();
  }
  else if (columnCount() > 0)
  {
    QRect itemRect = visualItemRect(item(result, 0));
    if (itemRect.bottom() - pos.y() < pos.y() - itemRect.top())
    {
      result++;
    }
  }
  return result;
}

void MovableTableWidget::resizeColumnToContents(int column)
{
  int content = sizeHintForColumn(column);
  int header = horizontalHeader()->sectionSizeHint(column);
  horizontalHeader()->resizeSection(column, qMax(content, header));
}

void MovableTableWidget::resizeColumnsToContents()
{
  for (int i = 0; i < columnCount(); ++i)
  {
    resizeColumnToContents(i);
  }
}
