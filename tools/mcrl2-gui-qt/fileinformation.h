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

namespace Ui {
class FileInformation;
}

class FileInformation : public QWidget
{
    Q_OBJECT
    
public:
    explicit FileInformation(QString filename, QWidget *parent = 0);
    ~FileInformation();
    
private:
    QString sizeString(qint64 size);
    void addRow(QString name, QString value);

    QString m_filename;
    Ui::FileInformation *ui;
};

#endif // FILEINFORMATION_H
