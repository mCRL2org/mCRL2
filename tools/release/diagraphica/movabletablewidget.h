// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
    explicit MovableTableWidget(QWidget* parent = nullptr);

  protected:
    int sizeHintForColumn(int column) const override;
    void paintEvent(QPaintEvent* e) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    int dropRow(QPoint pos);

  private:
    int m_lineRow;

  signals:
    void itemMoved(int oldIndex, int newIndex);
    
  public slots:
    void resizeColumnToContents(int column);
    void resizeColumnsToContents();

};

#endif // MOVABLETABLEWIDGET_H
