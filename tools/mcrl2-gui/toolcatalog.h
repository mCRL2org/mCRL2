// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLCATALOG_H
#define TOOLCATALOG_H

#include <QDomDocument>
#include <QString>
#include <QCoreApplication>
#include <QFile>
#include <QMap>
#include <QList>
#include <QStringList>

#include "toolinformation.h"

class ToolCatalog
{
  public:
    ToolCatalog();
    // file types for an extension
    QStringList fileTypes(QString extension);

    void load();

    QStringList categories();

    QList<ToolInformation> tools(QString category);
    QList<ToolInformation> tools(QString category, QString extension);

  private:
    void generateFileTypes();

    QDomDocument m_xml;
    // Maps extensions to file types
    QMultiMap<QString, QString> m_filetypes;
    QMap<QString, QList<ToolInformation> > m_categories;
};

#endif // TOOLCATALOG_H
