// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assert.h>
#include "optionvalue.h"

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled) :
  m_option(option), m_enabled(cbEnabled)
{
  assert(!option.hasArgument());
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QButtonGroup *argValue) :
  m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == EnumArgument);
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QLineEdit *argValue) :
  m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == StringArgument || option.argument.type == LevelArgument);
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QSpinBox *argValue, QCheckBox *argEnabled) :
  m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(argEnabled)
{
  assert(option.hasArgument());
  assert(option.argument.type == IntegerArgument);
  assert(option.argument.optional);
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QDoubleSpinBox *argValue, QCheckBox *argEnabled) :
  m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(argEnabled)
{
  assert(option.hasArgument());
  assert(option.argument.type == RealArgument);
  assert(option.argument.optional);
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, FilePicker *argValue) :
  m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == FileArgument);
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QCheckBox *argValue) :
  m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == BooleanArgument);
}

QString OptionValue::value()
{
  QString output("");

  if (m_enabled->isChecked())
  {
    output.append("--").append(m_option.nameLong);

    if (m_option.hasArgument() && (m_argEnabled == NULL || m_argEnabled->isChecked()))
    {
      QString argValue;

      switch (m_option.argument.type)
      {
        case StringArgument:
        case LevelArgument:
          argValue = dynamic_cast<QLineEdit*>(m_value)->text();
          break;
        case EnumArgument:
          argValue = dynamic_cast<QButtonGroup*>(m_value)->checkedButton()->text();
          break;
        case FileArgument:
          argValue = dynamic_cast<FilePicker*>(m_value)->text();
          break;
        case IntegerArgument:
          argValue = QString::number(dynamic_cast<QSpinBox*>(m_value)->value(), 'f', 10);
          break;
        case RealArgument:
          argValue = QString::number(dynamic_cast<QDoubleSpinBox*>(m_value)->value(), 'f', 10);
          break;
        case BooleanArgument:
          argValue = (dynamic_cast<QCheckBox*>(m_value)->isChecked() ? "yes" : "no");
          break;
        default:
          break;
      }
      if (!argValue.isEmpty())
      {
        output.append("=");
        if (argValue.contains(" "))
        {
          argValue = QString("\"%1\"").arg(argValue);
        }
        output.append(argValue);
      }
    }
  }

  return output;
}













