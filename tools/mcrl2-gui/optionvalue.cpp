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

void OptionValue::onValueChange()
{
  if (m_enabled != NULL)
  {
    m_enabled->setChecked(true);
  }
  if (m_argEnabled != NULL)
  {
    m_argEnabled->setChecked(true);
  }
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(NULL), m_argEnabled(NULL)
{
  assert(!option.hasArgument());
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QButtonGroup *argValue, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == EnumArgument);
  QWidget::connect(argValue, SIGNAL(buttonClicked(int)), this, SLOT(onValueChange()));
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QLineEdit *argValue, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == StringArgument || option.argument.type == LevelArgument);
  QWidget::connect(argValue, SIGNAL(textChanged(QString)), this, SLOT(onValueChange()));
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QSpinBox *argValue, QCheckBox *argEnabled, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(argEnabled)
{
  assert(option.hasArgument());
  assert(option.argument.type == IntegerArgument);
  assert((argEnabled == NULL) != option.argument.optional);
  QWidget::connect(argValue, SIGNAL(valueChanged(int)), this, SLOT(onValueChange()));
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QDoubleSpinBox *argValue, QCheckBox *argEnabled, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(argEnabled)
{
  assert(option.hasArgument());
  assert(option.argument.type == RealArgument);
  assert((argEnabled == NULL) != option.argument.optional);
  QWidget::connect(argValue, SIGNAL(valueChanged(double)), this, SLOT(onValueChange()));
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, FilePicker *argValue, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == FileArgument);
  QWidget::connect(argValue, SIGNAL(textChanged(QString)), this, SLOT(onValueChange()));
}

OptionValue::OptionValue(ToolOption option, QCheckBox *cbEnabled, QCheckBox *argValue, QObject* parent) :
  QObject(parent), m_option(option), m_enabled(cbEnabled), m_value(argValue), m_argEnabled(NULL)
{
  assert(option.hasArgument());
  assert(option.argument.type == BooleanArgument);
  QWidget::connect(argValue, SIGNAL(stateChanged(int)), this, SLOT(onValueChange()));
}

QString OptionValue::value()
{
  QString output("");

  if ((m_enabled == NULL || m_enabled->isChecked()) &&
      (m_option.argument.type != EnumArgument ||
       dynamic_cast<QButtonGroup*>(m_value)->checkedButton() != NULL))
  {
    output.append("--").append(m_option.nameLong);

    if (m_option.hasArgument() &&
        (m_argEnabled == NULL || m_argEnabled->isChecked()))
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













