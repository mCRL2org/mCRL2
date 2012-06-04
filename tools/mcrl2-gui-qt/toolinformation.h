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

class ToolAction;

class ToolInformation
{
  public:
    ToolInformation(QString name, QString input, QString output);

    QString getName() { return m_name; }
    QString getInput() { return m_input; }
    QString getOutput() { return m_output; }
    QString getDescription() { return m_desc; }
    QString getAuthor() { return m_author; }
    QDomDocument getXML() { return m_xml; }
    bool hasOutput() { return !m_output.isEmpty(); }
    bool isValid() { return m_valid; }

  private:
    QString m_name, m_input, m_output, m_desc, m_author;
    QDomDocument m_xml;
    bool m_valid;
};

#endif // TOOLINFORMATION_H
