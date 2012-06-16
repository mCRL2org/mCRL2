// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "filepicker.h"

#include <QFileDialog>

FilePicker::FilePicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilePicker)
{
    ui->setupUi(this);

    connect(ui->btnBrowse, SIGNAL(clicked()), this, SLOT(onBrowse()));
    connect(ui->value, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}

FilePicker::~FilePicker()
{
    delete ui;
}

void FilePicker::onBrowse()
{
  QString fileName(QFileDialog::getSaveFileName(parentWidget(), tr("Select file"), QString(),
                                                tr("All files (*.*)")));
  if (!fileName.isNull())
    ui->value->setText(fileName);
}
