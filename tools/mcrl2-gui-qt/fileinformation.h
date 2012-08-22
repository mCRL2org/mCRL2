// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef FILEINFORMATION_H
#define FILEINFORMATION_H

#include <QWidget>
#include "ui_fileinformation.h"

class FileInformation : public QWidget
{
    Q_OBJECT
    
public:
    explicit FileInformation(QString filename, QWidget *parent = 0);
    
private:
    QString sizeString(qint64 size);
    void addRow(QString name, QString value);

    QString m_filename;
    Ui::FileInformation m_ui;
};

#endif // FILEINFORMATION_H
