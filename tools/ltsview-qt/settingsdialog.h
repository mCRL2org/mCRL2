// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
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

class ComboboxHandler : public QObject
{
  Q_OBJECT

  public:
    ComboboxHandler(QComboBox *combobox, Settings::SettingBool &setting);

  protected slots:
    void stateChanged(int state);
    void setState();

  private:
    QComboBox *m_combobox;
    Settings::SettingBool *m_setting;
};

class SettingsDialog : public QDialog
{
  Q_OBJECT

  public:
    SettingsDialog(QWidget *parent, Settings *settings);

  private slots:
    void stateSizeChanged(int value);
    void setStateSize(float value);
    void clusterHeightChanged(int value);
    void setClusterHeight(float value);
    void accuracyChanged(int value);
    void setAccuracy(int value);

  private:
    void setupSpinbox(QSpinBox *spinbox, Settings::SettingInt &setting);
    void setupCheckbox(QCheckBox *checkbox, Settings::SettingBool &setting);
    void setupCombobox(QComboBox *combobox, Settings::SettingBool &setting);

  private:
    Ui::SettingsDialog m_ui;
    Settings *m_settings;
};

#endif
