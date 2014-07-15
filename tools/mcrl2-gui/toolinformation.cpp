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

ToolInformation::ToolInformation(QString name, QString input1, QString input2, QString output, bool guiTool)
  : name(name), input2(input2), output(output), guiTool(guiTool), valid(false)
{
  QStringList inputs = input1.split(';');
  for (QStringList::iterator it = inputs.begin(); it != inputs.end(); ++it)
    input.insert(*it);

  QDir appDir = QDir(QCoreApplication::applicationDirPath());

  #ifdef __APPLE__
    appDir.cdUp();
    appDir.cdUp();
    appDir.cdUp();
  #endif

    path = appDir.absoluteFilePath(name);
  #ifdef _WIN32
    path.append(".exe");
  #endif
  #ifdef __APPLE__
    if (guiTool)
    {
      path.append(".app/Contents/MacOS/");
      path.append(name);
    }
  #endif
}

void ToolInformation::load()
{
  QProcess toolProcess;

  toolProcess.start(path, QStringList("--generate-xml"), QIODevice::ReadOnly);
  if (!toolProcess.waitForFinished(3000))
  {
    mCRL2log(mcrl2::log::error) << "Command: " << path.toStdString() << " --generate-xml" << std::endl;
    mCRL2log(mcrl2::log::error) << toolProcess.errorString().toStdString() << " (" << name.toStdString() << ")" << std::endl;
    return;
  }
  QByteArray xmlText = toolProcess.readAllStandardOutput();

  QString errorMsg;
  QDomDocument xml;
  if(!xml.setContent(xmlText, false, &errorMsg))
  {
    mCRL2log(mcrl2::log::error) << "Could not parse XML output of " << name.toStdString() << ": " << errorMsg.toStdString() << std::endl;
    return;
  }

  QDomElement root = xml.documentElement();
  if(root.tagName() != "tool")
  {
    mCRL2log(mcrl2::log::error) << "XML output of " << name.toStdString() << " contains no valid tool information" << std::endl;
    return;
  }

  valid = true;

  QDomElement descElement = root.firstChildElement("description");
  QDomElement authorElement = root.firstChildElement("author");
  QDomElement optionsElement = root.firstChildElement("options");

  if (!descElement.isNull())
  {
    QByteArray data;
    QTextStream out(&data);
    descElement.save(out, 2);
    desc = QString(data);
  }
  else
  {
    mCRL2log(mcrl2::log::warning) << "XML output of " << name.toStdString() << " contains no description element" << std::endl;
  }

  if (!authorElement.isNull())
  {
    author = authorElement.text();
  }
  else
  {
    mCRL2log(mcrl2::log::warning) << "XML output of " << name.toStdString() << " contains no author element" << std::endl;
  }

  if (!optionsElement.isNull())
  {
    parseOptions(optionsElement);
  }
  else
  {
    mCRL2log(mcrl2::log::warning) << "XML output of " << name.toStdString() << " contains no options element" << std::endl;
  }
}

bool ToolInformation::inputMatchesAny(QStringList filetypes)
{
  for (auto it = filetypes.begin(); it != filetypes.end(); ++it)
  {
    if (input.contains(*it))
    {
      return true;
    }
  }
  return false;
}

void ToolInformation::parseOptions(QDomElement optionsElement)
{
  QDomElement optionElement = optionsElement.firstChildElement("option");

  while (!optionElement.isNull())
  {
    QString optShort = optionElement.firstChildElement("short").text();
    QString optLong = optionElement.firstChildElement("long").text();
    bool standard = (optionElement.attribute("default") == "yes") || (optLong.toLower() == "verbose");
    QString optDescription = optionElement.firstChildElement("description").text();
    ToolOption option(standard, optShort, optLong, optDescription);

    QDomElement argElement = optionElement.firstChildElement("option_argument");

    if (!argElement.isNull())
    {
      bool optional = (argElement.attribute("optional") == "yes");
      QString typeStr = argElement.attribute("type");
      QString name = argElement.firstChildElement("name").text();


      ToolArgument argument(optional, guessType(typeStr, name), name);

      QDomElement argvalsElement = argElement.firstChildElement("values");
      if (!argvalsElement.isNull() && argument.type == EnumArgument)
      {
        QDomElement valueElement = argvalsElement.firstChildElement("value");
        while (!valueElement.isNull())
        {
          bool valStandard = (valueElement.attribute("default") == "yes");
          QString valShort = valueElement.firstChildElement("short").text();
          QString valLong = valueElement.firstChildElement("long").text();
          QString valDescription = valueElement.firstChildElement("description").text();
          ToolValue toolValue(valStandard, valShort, valLong, valDescription);
          argument.values.append(toolValue);

          valueElement = valueElement.nextSiblingElement("value");
        }
      }

      option.argument = argument;
    }

    options.append(option);
    optionElement = optionElement.nextSiblingElement("option");
  }

}

ArgumentType ToolInformation::guessType(QString type, QString name)
{
  type = type.toLower();
  name = name.toLower();

  if (type == "enum")
  {
    return EnumArgument;
  }
  else if (type == "file")
  {
    return FileArgument;
  }
  else if (type == "level")                           // New type
  {
    return LevelArgument;
  }
  else if (type == "int")                             // New type
  {
    return IntegerArgument;
  }
  else if (type == "real")                            // New type
  {
    return RealArgument;
  }
  else if (type == "bool")                            // New type
  {
    return BooleanArgument;
  }
  else if (type == "mandatory" || type == "optional") // TODO: remove these types
  {
    if (name == "file")
    {
      return FileArgument;
    }
    else if (name == "num")
    {
      return IntegerArgument;
    }
    else if (name == "bool")
    {
      return BooleanArgument;
    }
    else if (name == "level")
    {
      return LevelArgument;
    }
    return StringArgument;
  }

  return InvalidArgument;
}







