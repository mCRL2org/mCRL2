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

ToolCatalog::ToolCatalog()
{
  generateFileTypes();
}

void ToolCatalog::generateFileTypes()
{
  // Only add those file types for which we cannot use
  // the identity
  m_filetypes.insert("fsm", "lts");
  m_filetypes.insert("aut", "lts");
  m_filetypes.insert("dot", "lts");
  m_filetypes.insert("svc", "lts");
  m_filetypes.insert("gm",  "bes");
  m_filetypes.insert("cwi", "bes");
  m_filetypes.insert("bes", "pbes");
  m_filetypes.insert("gm",  "pbes");
  m_filetypes.insert("cwi", "pbes");
}

QStringList ToolCatalog::fileTypes(QString extension)
{
  QStringList result = m_filetypes.values(extension);
  result.push_front(extension);
  return result;
}

void ToolCatalog::load()
{
  typedef QMap<QString, QList<ToolInformation> > categorymap;

  QDir toolsetDir = QDir(QCoreApplication::applicationDirPath());
  if (toolsetDir.dirName().toLower() == "bin")
    toolsetDir.cdUp();

  QString catalogFilename = toolsetDir.filePath("share/mcrl2/tool_catalog.xml");

  QFile file(catalogFilename);
  if(!file.open( QFile::ReadOnly ))
  {
    file.setFileName(":/share/mcrl2/tool_catalog.xml");
    if (!file.open(QFile::ReadOnly))
    {
      mCRL2log(mcrl2::log::error) << "Could not open XML file: " << catalogFilename.toStdString() << std::endl;
      return;
    }
    mCRL2log(mcrl2::log::warning) << "Could not open XML file: " << catalogFilename.toStdString() << ", using embedded copy instead." << std::endl;;
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
      categorymap::iterator icat = m_categories.find(cat);
      if (icat == m_categories.end())
        icat = m_categories.insert(cat, QList<ToolInformation>());

      ToolInformation toolinfo(e.attribute("name"), e.attribute("input_format"), e.attribute("input_format1"), e.attribute("output_format", ""), e.attribute("gui", "").toLower() == "true");
      toolinfo.load();
      icat.value().append(toolinfo);
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
  return m_categories.value(category);
}

/*
QList<ToolInformation> ToolCatalog::tools(QString category, QString extension)
{
  QStringList inputTypes = fileTypes(extension);
  QList<ToolInformation> all = tools(category);
  QList<ToolInformation> ret;
  for (QList<ToolInformation>::iterator tool = all.begin(); tool != all.end(); ++tool)
  {
    for (QStringList::const_iterator it = inputTypes.begin(); it != inputTypes.end(); ++it)
    {
      if (tool->input.contains(*it)) {
        ret.append(*tool);
        break;
      }
    }
  }
  return ret;
}
*/
