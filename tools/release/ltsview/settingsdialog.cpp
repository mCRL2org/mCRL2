// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "settingsdialog.h"
#include <QColorDialog>

ColorButtonHandler::ColorButtonHandler(QPushButton *button, Settings::SettingColor &setting):
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

SettingsDialog::SettingsDialog(QWidget *parent, Settings *settings):
  QDialog(parent),
  m_settings(settings)
{
  m_ui.setupUi(this);

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
  setupCheckbox(m_ui.navShowBackpointers, m_settings->navShowBackpointers);
  setupCheckbox(m_ui.navShowStates, m_settings->navShowStates);
  setupCheckbox(m_ui.navShowTransitions, m_settings->navShowTransitions);
  setupCheckbox(m_ui.navSmoothShading, m_settings->navSmoothShading);
  setupCheckbox(m_ui.navLighting, m_settings->navLighting);
  setupCheckbox(m_ui.navTransparency, m_settings->navTransparency);
}

void SettingsDialog::setupSpinbox(QSpinBox *spinbox, Settings::SettingInt &setting)
{
  connect(spinbox, SIGNAL(valueChanged(int)), &setting, SLOT(setValue(int)));
  connect(&setting, SIGNAL(changed(int)), spinbox, SLOT(setValue(int)));
  spinbox->setValue(setting.value());
}

void SettingsDialog::setupCheckbox(QCheckBox *checkbox, Settings::SettingBool &setting)
{
  connect(checkbox, SIGNAL(toggled(bool)), &setting, SLOT(setValue(bool)));
  connect(&setting, SIGNAL(changed(bool)), checkbox, SLOT(setChecked(bool)));
  checkbox->setChecked(setting.value());
}
