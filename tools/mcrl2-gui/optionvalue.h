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

class OptionValue : public QObject
{
    Q_OBJECT
public:
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, QObject* parent=NULL);
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, QButtonGroup *argValue, QObject* parent=NULL);
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, QLineEdit *argValue, QObject* parent=NULL);
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, QSpinBox *argValue, QCheckBox *argEnabled = NULL, QObject* parent=NULL);
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, QDoubleSpinBox *argValue, QCheckBox *argEnabled = NULL, QObject* parent=NULL);
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, FilePicker *argValue, QObject* parent=NULL);
    explicit OptionValue(ToolOption option, QCheckBox *cbEnabled, QCheckBox *argValue, QObject* parent=NULL);

    QString value();
private:
    ToolOption m_option;
    QCheckBox *m_enabled;
    QObject *m_value;
    QCheckBox *m_argEnabled;
protected slots:
    void onValueChange();
};

#endif // OPTIONVALUE_H







