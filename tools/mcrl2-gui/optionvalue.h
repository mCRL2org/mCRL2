// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef OPTIONVALUE_H
#define OPTIONVALUE_H

#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "filepicker.h"

#include "toolinformation.h"

class OptionValue
{
public:
    OptionValue(ToolOption option, QCheckBox *cbEnabled);
    OptionValue(ToolOption option, QCheckBox *cbEnabled, QButtonGroup *argValue);
    OptionValue(ToolOption option, QCheckBox *cbEnabled, QLineEdit *argValue);
    OptionValue(ToolOption option, QCheckBox *cbEnabled, QSpinBox *argValue, QCheckBox *argEnabled = NULL);
    OptionValue(ToolOption option, QCheckBox *cbEnabled, QDoubleSpinBox *argValue, QCheckBox *argEnabled = NULL);
    OptionValue(ToolOption option, QCheckBox *cbEnabled, FilePicker *argValue);
    OptionValue(ToolOption option, QCheckBox *cbEnabled, QCheckBox *argValue);

    QString value();
private:
    ToolOption m_option;
    QCheckBox *m_enabled;
    QObject *m_value;
    QCheckBox *m_argEnabled;

};

#endif // OPTIONVALUE_H







