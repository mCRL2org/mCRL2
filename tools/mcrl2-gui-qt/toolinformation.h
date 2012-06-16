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
#include <QList>
#include <QObject>
#include <QProcess>
#include <QWidget>
#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>

enum ArgumentType
{
  StringArgument,
  LevelArgument,
  EnumArgument,
  FileArgument,
  IntegerArgument,
  RealArgument,
  BooleanArgument,
  UnknownArgument
};

struct ToolValue
{
    ToolValue() :
      standard(false)
    {}
    ToolValue(bool standard, QString nameShort, QString nameLong, QString description) :
      standard(standard),
      nameShort(nameShort),
      nameLong(nameLong),
      description(description)
    {}
    bool standard;
    QString nameShort, nameLong, description;
};

struct ToolArgument
{
    ToolArgument() :
      optional(true),
      type(UnknownArgument)
    {}
    ToolArgument(bool optional, ArgumentType type, QString name) :
      optional(optional),
      type(type),
      name(name)
    {}

    bool optional;
    ArgumentType type;
    QString name;

    QList<ToolValue> values;
};

struct ToolOption
{
    ToolOption() :
      standard(false)
    {}
    ToolOption(bool standard, QString nameShort, QString nameLong, QString description) :
      standard(standard),
      nameShort(nameShort),
      nameLong(nameLong),
      description(description)
    {}

    bool hasArgument() { return (argument.type != UnknownArgument); }

    bool standard;
    QString nameShort, nameLong, description;

    ToolArgument argument;

};

class ToolInformation
{
  public:
    ToolInformation(QString name, QString input, QString output);

    void load();
    bool hasOutput() { return !output.isEmpty(); }

    QString name, input, output, desc, author;
    bool valid;

    QList<ToolOption> options;

  private:
    void parseOptions(QDomElement optionsElement);
    ArgumentType guessType(QString type, QString name);
};

#endif // TOOLINFORMATION_H
