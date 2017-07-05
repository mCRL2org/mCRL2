// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SETTINGSDOCK_H
#define SETTINGSDOCK_H

#include "ui_settingsdock.h"

#include "settings.h"
#include <QDialog>

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

class SettingsDock : public QWidget
{
  Q_OBJECT

  public:
    SettingsDock(QWidget *parent, Settings *settings);

  private slots:
    void stateSizeChanged(int value);
    void setStateSize(float value);
    void clusterHeightChanged(int value);
    void setClusterHeight(float value);
    void accuracyChanged(int value);
    void setAccuracy(int value);

  private:
    void setupSpinbox(QSpinBox *spinbox, Settings::SettingInt &setting);

  private:
    Ui::SettingsDock m_ui;
    Settings *m_settings;
};

#endif
