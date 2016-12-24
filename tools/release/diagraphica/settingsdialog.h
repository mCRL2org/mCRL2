// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"

#include "settings.h"
#include <QDialog>

class ColorButtonHandler : public QObject
{
  Q_OBJECT

  public:
    ColorButtonHandler(QPushButton *button, Settings::SettingColor &setting);

  protected slots:
    void clicked();
    void setColor();

  private:
    QPushButton *m_button;
    Settings::SettingColor *m_setting;
};

class EnumHandler : public QObject
{
  Q_OBJECT

  public:
    EnumHandler(QComboBox *comboBox, Settings::SettingEnum &setting);

  protected slots:
    void stateChanged(int state) { m_setting->setValue(m_setting->items()[state].first); }
    void setValue();

  private:
    QComboBox *m_comboBox;
    Settings::SettingEnum *m_setting;
};

class DoubleSpinboxHandler : public QObject
{
  Q_OBJECT

  public:
    DoubleSpinboxHandler(QDoubleSpinBox *spinbox, Settings::SettingFloat &setting);

  protected slots:
    void changed() { m_setting->setValue(m_spinbox->value()); }
    void setValue() { m_spinbox->setValue(m_setting->value()); }

  private:
    QDoubleSpinBox *m_spinbox;
    Settings::SettingFloat *m_setting;
};

class SettingsDialog : public QDialog
{
  Q_OBJECT

  public:
    SettingsDialog(QWidget *parent, Settings *settings);

  public slots:
    void showGeneral() { m_ui.tabWidget->setCurrentIndex(0); show(); }
    void showArcDiagram() { m_ui.tabWidget->setCurrentIndex(1); show(); }

  private:
    void setupCheckbox(QCheckBox *checkbox, Settings::SettingBool &setting);
    void setupSpinbox(QSpinBox *spinbox, Settings::SettingInt &setting);

  private:
    Ui::SettingsDialog m_ui;
    Settings *m_settings;
};



#endif
