// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "settingsdock.h"

ComboboxHandler::ComboboxHandler(QComboBox *combobox, Settings::SettingBool &setting):
  QObject(combobox),
  m_combobox(combobox),
  m_setting(&setting)
{
  connect(m_combobox, SIGNAL(activated(int)), this, SLOT(stateChanged(int)));
  connect(m_setting, SIGNAL(changed(bool)), this, SLOT(setState()));
  setState();
}

void ComboboxHandler::stateChanged(int state)
{
  m_setting->setValue(state != 0);
}

void ComboboxHandler::setState()
{
  m_combobox->setCurrentIndex(m_setting->value() ? 1 : 0);
}

SettingsDock::SettingsDock(QWidget *parent, Settings *settings):
  QWidget(parent),
  m_settings(settings)
{
  m_ui.setupUi(this);

  connect(m_ui.stateSize, SIGNAL(valueChanged(int)), this, SLOT(stateSizeChanged(int)));
  connect(&m_settings->stateSize, SIGNAL(changed(float)), this, SLOT(setStateSize(float)));
  setStateSize(m_settings->stateSize.value());

  connect(m_ui.clusterHeight, SIGNAL(valueChanged(int)), this, SLOT(clusterHeightChanged(int)));
  connect(&m_settings->clusterHeight, SIGNAL(changed(float)), this, SLOT(setClusterHeight(float)));
  setClusterHeight(m_settings->clusterHeight.value());

  setupSpinbox(m_ui.branchRotation, m_settings->branchRotation);
  setupSpinbox(m_ui.branchTilt, m_settings->branchTilt);

  connect(m_ui.accuracy, SIGNAL(valueChanged(int)), this, SLOT(accuracyChanged(int)));
  connect(&m_settings->quality, SIGNAL(changed(int)), this, SLOT(setAccuracy(int)));
  setAccuracy(m_settings->quality.value());

  new ComboboxHandler(m_ui.stateRanking, m_settings->stateRankStyleCyclic);
  new ComboboxHandler(m_ui.clusterPositioning, m_settings->fsmStyle);
  new ComboboxHandler(m_ui.statePositioning, m_settings->statePosStyleMultiPass);
  new ComboboxHandler(m_ui.visualizationStyle, m_settings->clusterVisStyleTubes);
}

void SettingsDock::stateSizeChanged(int value)
{
  m_settings->stateSize.setValue(value / 10.0f);
}

void SettingsDock::setStateSize(float value)
{
  m_ui.stateSize->setValue((int)(value * 10.0f));
}

void SettingsDock::clusterHeightChanged(int value)
{
  m_settings->clusterHeight.setValue(value / 10.0f);
}

void SettingsDock::setClusterHeight(float value)
{
  m_ui.clusterHeight->setValue((int)(value * 10.0f));
}

void SettingsDock::accuracyChanged(int value)
{
  m_settings->quality.setValue(value * 2);
}

void SettingsDock::setAccuracy(int value)
{
  m_ui.accuracy->setValue(value / 2);
}

void SettingsDock::setupSpinbox(QSpinBox *spinbox, Settings::SettingInt &setting)
{
  connect(spinbox, SIGNAL(valueChanged(int)), &setting, SLOT(setValue(int)));
  connect(&setting, SIGNAL(changed(int)), spinbox, SLOT(setValue(int)));
  spinbox->setValue(setting.value());
}
