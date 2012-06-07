// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLINFORMATION_H
#define TOOLINFORMATION_H

#include <QString>
#include <QObject>
#include <QProcess>
#include <QWidget>
#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>

struct ToolInformation
{
    ToolInformation(QString name, QString input, QString output);

    bool hasOutput() { return !output.isEmpty(); }

    QString name, input, output, desc, author;
    bool valid;
};

#endif // TOOLINFORMATION_H
