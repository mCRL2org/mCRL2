// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "settingsdialog.h"
#include <QColorDialog>

ColorButtonHandler::ColorButtonHandler(QPushButton *button, SettingColor &setting):
  QObject(button),
  m_button(button),
  m_setting(&setting)
{
  connect(m_button, SIGNAL(clicked()), this, SLOT(clicked()));
  connect(m_setting, SIGNAL(changed(QColor)), this, SLOT(setColor()));
  setColor();
}

void ColorButtonHandler::clicked()
{
  QColor color = QColorDialog::getColor(m_setting->value(), m_button->parentWidget());
  if (color.isValid())
  {
    m_setting->setValue(color);
  }
}

void ColorButtonHandler::setColor()
{
  m_button->setAutoFillBackground(true);
  QColor color = m_setting->value();
  QString colorString = QString("rgb(") + QString::number(color.red()) + ", " + QString::number(color.green()) + ", " + QString::number(color.blue()) + ")";
  m_button->setStyleSheet(QString("background-color: ") + colorString + "; color: " + colorString + ";");
}

ComboboxHandler::ComboboxHandler(QComboBox *combobox, SettingBool &setting):
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

SettingsDialog::SettingsDialog(QWidget *parent, Settings *settings):
  QDialog(parent),
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
  setupSpinbox(m_ui.transparency, m_settings->transparency);
  new ColorButtonHandler(m_ui.backgroundColor, m_settings->backgroundColor);
  new ColorButtonHandler(m_ui.stateColor, m_settings->stateColor);
  new ColorButtonHandler(m_ui.transitionColor, m_settings->downEdgeColor);
  new ColorButtonHandler(m_ui.backpointerColor, m_settings->upEdgeColor);
  new ColorButtonHandler(m_ui.markColor, m_settings->markedColor);
  new ColorButtonHandler(m_ui.clusterColorTop, m_settings->clusterColorTop);
  new ColorButtonHandler(m_ui.clusterColorBottom, m_settings->clusterColorBottom);
  setupCheckbox(m_ui.longInterpolation, m_settings->longInterpolation);
  new ColorButtonHandler(m_ui.simulationHistoryColor, m_settings->simPrevColor);
  new ColorButtonHandler(m_ui.simulationCurrentStateColor, m_settings->simCurrColor);
  new ColorButtonHandler(m_ui.simulationSelectedColor, m_settings->simSelColor);
  new ColorButtonHandler(m_ui.simulationNextStateColor, m_settings->simPosColor);
  new ComboboxHandler(m_ui.stateRanking, m_settings->stateRankStyleCyclic);
  new ComboboxHandler(m_ui.clusterPositioning, m_settings->fsmStyle);
  new ComboboxHandler(m_ui.statePositioning, m_settings->statePosStyleMultiPass);
  new ComboboxHandler(m_ui.visualizationStyle, m_settings->clusterVisStyleTubes);
  setupCheckbox(m_ui.navShowBackpointers, m_settings->navShowBackpointers);
  setupCheckbox(m_ui.navShowStates, m_settings->navShowStates);
  setupCheckbox(m_ui.navShowTransitions, m_settings->navShowTransitions);
  setupCheckbox(m_ui.navSmoothShading, m_settings->navSmoothShading);
  setupCheckbox(m_ui.navLighting, m_settings->navLighting);
  setupCheckbox(m_ui.navTransparency, m_settings->navTransparency);
}

void SettingsDialog::stateSizeChanged(int value)
{
  m_settings->stateSize.setValue(value / 10.0f);
}

void SettingsDialog::setStateSize(float value)
{
  m_ui.stateSize->setValue((int)(value * 10.0f));
}

void SettingsDialog::clusterHeightChanged(int value)
{
  m_settings->clusterHeight.setValue(value / 10.0f);
}

void SettingsDialog::setClusterHeight(float value)
{
  m_ui.clusterHeight->setValue((int)(value * 10.0f));
}

void SettingsDialog::accuracyChanged(int value)
{
  m_settings->quality.setValue(value * 2);
}

void SettingsDialog::setAccuracy(int value)
{
  m_ui.accuracy->setValue(value / 2);
}

void SettingsDialog::setupSpinbox(QSpinBox *spinbox, SettingInt &setting)
{
  connect(spinbox, SIGNAL(valueChanged(int)), &setting, SLOT(setValue(int)));
  connect(&setting, SIGNAL(changed(int)), spinbox, SLOT(setValue(int)));
  spinbox->setValue(setting.value());
}

void SettingsDialog::setupCheckbox(QCheckBox *checkbox, SettingBool &setting)
{
  connect(checkbox, SIGNAL(toggled(bool)), &setting, SLOT(setValue(bool)));
  connect(&setting, SIGNAL(changed(bool)), checkbox, SLOT(setChecked(bool)));
  checkbox->setChecked(setting.value());
}

