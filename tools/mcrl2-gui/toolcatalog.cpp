// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "toolcatalog.h"
#include "mcrl2/utilities/logger.h"

#include <QDebug>

ToolCatalog::ToolCatalog()
{
  generateFileTypes();
}

void ToolCatalog::generateFileTypes()
{
//  m_filetypes.insert("mcrl2", "mcrl2");

//  m_filetypes.insert("lps",   "lps");

//  m_filetypes.insert("lts",   "lts");
  m_filetypes.insert("fsm",   "lts");
  m_filetypes.insert("aut",   "lts");
  m_filetypes.insert("dot",   "lts");
  m_filetypes.insert("svc",   "lts");
#ifdef USE_BCG
  m_filetypes.insert("bcg",   "lts");
#endif
//  m_filetypes.insert("bes",   "bes");
  m_filetypes.insert("gm",    "bes");
  m_filetypes.insert("cwi",   "bes");
  m_filetypes.insert("pbes",  "bes");

//  m_filetypes.insert("pbes",  "pbes");

//  m_filetypes.insert("tbf",   "tbf");

//  m_filetypes.insert("gra",   "gra");

//  m_filetypes.insert("mcf",   "mcf");

//  m_filetypes.insert("trc",   "trc");

//  m_filetypes.insert("txt",   "txt");
}

QString ToolCatalog::fileType(QString extension)
{
  return m_filetypes.value(extension, extension);
}

void ToolCatalog::load()
{
  QDir toolsetDir = QDir(QCoreApplication::applicationDirPath());
  if (toolsetDir.dirName().toLower() == "bin")
    toolsetDir.cdUp();

  QString catalogFilename = toolsetDir.filePath("share/mcrl2/tool_catalog.xml");

  QFile file(catalogFilename);
  if(!file.open( QFile::ReadOnly ))
  {
    mCRL2log(mcrl2::log::error) << "Could not open XML file: " << catalogFilename.toStdString() << std::endl;
    return;
  }
  QString errorMsg;
  if(!m_xml.setContent(&file, false, &errorMsg))
  {
    file.close();
    mCRL2log(mcrl2::log::error) << "Could not parse XML file: " << errorMsg.toStdString() << std::endl;
    return;
  }
  file.close();

  QDomElement root = m_xml.documentElement();
  if(root.tagName() != "tool-catalog")
  {
    mCRL2log(mcrl2::log::error) << catalogFilename.toStdString() << " contains no valid tool catalog" << std::endl;
    return;
  }

  QDomNode node = root.firstChild();
  while (!node.isNull()) {
    QDomElement e = node.toElement();

    if (e.tagName() == "tool") {
      QString cat = e.attribute("category", "Miscellaneous");
      if (!m_categories.contains(cat))
        m_categories.insert(cat, QMap<QString, ToolInformation>());

      QMap<QString, ToolInformation> tools = m_categories.value(cat);
      ToolInformation toolinfo(e.attribute("name"), e.attribute("input_format"), e.attribute("output_format", ""), e.attribute("gui", "").toLower() == "true");
      toolinfo.load();
      tools.insert(e.attribute("name"), toolinfo);
      m_categories.insert(cat, tools);
    }

    node = node.nextSibling();
  }
}

QStringList ToolCatalog::categories()
{
  return m_categories.keys();
}

QList<ToolInformation> ToolCatalog::tools(QString category)
{
  return m_categories.value(category).values();
}

QList<ToolInformation> ToolCatalog::tools(QString category, QString extension)
{
  QString inputType = fileType(extension);
  QList<ToolInformation> all = tools(category);
  QList<ToolInformation> ret;
  for (int i = 0; i < all.count(); i++)
  {
    ToolInformation tool = all.at(i);
    if (tool.input == inputType) {
      ret.append(tool);
    }
  }
  return ret;
}
