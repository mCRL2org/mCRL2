// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "toolinformation.h"
#include "mcrl2/utilities/logger.h"
#include <QTextStream>

ToolInformation::ToolInformation(QString name, QString input, QString output)
  : m_name(name), m_input(input), m_output(output), m_valid(false)
{
  QProcess *toolProcess = new QProcess();

  QDir appDir = QDir(QCoreApplication::applicationDirPath());

#ifdef __APPLE__
  appDir.cdUp();
  appDir.cdUp();
#endif

  QString executable = appDir.absoluteFilePath(m_name);
#ifdef _WIN32
  executable.append(".exe");
#endif
#ifdef __APPLE__
  executable.append(".app/Contents/MacOS/"+ m_name);
#endif

  toolProcess->start(executable, QStringList("--generate-xml"), QIODevice::ReadOnly);
  if (!toolProcess->waitForFinished(3000))
  {
    mCRL2log(mcrl2::log::error) << toolProcess->errorString().toStdString() << " (" << m_name.toStdString() << ")" << std::endl;
    delete toolProcess;
    return;
  }
  QByteArray xmlText = toolProcess->readAllStandardOutput();
  delete toolProcess;

  QString errorMsg;
  if(!m_xml.setContent(xmlText, false, &errorMsg))
  {
    mCRL2log(mcrl2::log::error) << "Could not parse XML output of " << m_name.toStdString() << ": " << errorMsg.toStdString() << std::endl;
    return;
  }

  QDomElement root = m_xml.documentElement();
  if(root.tagName() != "tool")
  {
    mCRL2log(mcrl2::log::error) << "XML output of " << m_name.toStdString() << " contains no valid tool information" << std::endl;
    return;
  }

  m_valid = true;

  QDomNode node = root.firstChild();
  while (!node.isNull()) {
    QDomElement e = node.toElement();
    if (e.tagName() == "description") {
      QByteArray data;
      QTextStream out(&data);
      e.save(out, 2);
      m_desc = QString(data);
    }
    if (e.tagName() == "author") {
      m_author = e.text();
    }
    node = node.nextSibling();
  }

  appDir.mkdir("xml");

  if (!appDir.exists("xml/"+m_name+".xml"))
  {
    QFile data(appDir.absoluteFilePath("xml/"+m_name+".xml"));
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        m_xml.save(out, 2);
    }
  }

}











