// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef FILEPICKER_H
#define FILEPICKER_H

#include <QWidget>
#include "ui_filepicker.h"

namespace Ui {
  class FilePicker;
}

class FilePicker : public QWidget
{
    Q_OBJECT
    
  public:
    explicit FilePicker(QWidget *parent = 0);
    ~FilePicker();

    QString text() { return ui->value->text(); }

  public slots:
    void onBrowse();
    void setText(QString value) { ui->value->setText(value); }
    
  protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

  private:
    Ui::FilePicker *ui;

  private slots:
    void onTextChanged(QString value) { emit(textChanged(value)); }

  signals:
    void textChanged(QString value);

};

#endif // FILEPICKER_H
