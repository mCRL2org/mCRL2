// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
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

EnumHandler::EnumHandler(QComboBox *comboBox, Settings::SettingEnum &setting):
  QObject(comboBox),
  m_comboBox(comboBox),
  m_setting(&setting)
{
  QStringList items;
  for (int i = 0; i < m_setting->items().size(); ++i)
  {
    items += m_setting->items()[i].second;
  }
  m_comboBox->clear();
  m_comboBox->insertItems(0, items);

  connect(m_comboBox, SIGNAL(activated(int)), this, SLOT(stateChanged(int)));
  connect(m_setting, SIGNAL(changed(int)), this, SLOT(setValue()));
  setValue();
}

void EnumHandler::setValue()
{
  for (int i = 0; i < m_setting->items().size(); ++i)
  {
    if (m_setting->items()[i].first == m_setting->value())
    {
      m_comboBox->setCurrentIndex(i);
      return;
    }
  }
}

DoubleSpinboxHandler::DoubleSpinboxHandler(QDoubleSpinBox *spinbox, Settings::SettingFloat &setting):
  QObject(spinbox),
  m_spinbox(spinbox),
  m_setting(&setting)
{
  connect(m_spinbox, SIGNAL(valueChanged(double)), this, SLOT(changed()));
  connect(m_setting, SIGNAL(changed(float)), this, SLOT(setValue()));
  setValue();
}


SettingsDialog::SettingsDialog(QWidget *parent, Settings *settings):
  QDialog(parent),
  m_settings(settings)
{
  m_ui.setupUi(this);

  new ColorButtonHandler(m_ui.backgroundColor, m_settings->backgroundColor);
  new ColorButtonHandler(m_ui.textColor, m_settings->textColor);
  setupSpinbox(m_ui.textSize, m_settings->textSize);
  new DoubleSpinboxHandler(m_ui.animationSpeed, m_settings->animationSpeed);
  new EnumHandler(m_ui.simulationBlendType, m_settings->blendType);

  setupCheckbox(m_ui.showClusters, m_settings->showClusters);
  setupCheckbox(m_ui.showBundles, m_settings->showBundles);
  setupCheckbox(m_ui.showClusterTree, m_settings->showClusterTree);
  setupCheckbox(m_ui.annotateClusterTree, m_settings->annotateClusterTree);
  setupCheckbox(m_ui.showBarTree, m_settings->showBarTree);
  new ColorButtonHandler(m_ui.bundleColor, m_settings->bundleColor);
  new DoubleSpinboxHandler(m_ui.bundleTransparency, m_settings->arcTransparency);
  new EnumHandler(m_ui.clusterTreeColorMap, m_settings->clusterTreeColorMap);
  new DoubleSpinboxHandler(m_ui.barTreeMagnification, m_settings->barTreeMagnification);
}

void SettingsDialog::setupCheckbox(QCheckBox *checkbox, Settings::SettingBool &setting)
{
  connect(checkbox, SIGNAL(toggled(bool)), &setting, SLOT(setValue(bool)));
  connect(&setting, SIGNAL(changed(bool)), checkbox, SLOT(setChecked(bool)));
  checkbox->setChecked(setting.value());
}

void SettingsDialog::setupSpinbox(QSpinBox *spinbox, Settings::SettingInt &setting)
{
  connect(spinbox, SIGNAL(valueChanged(int)), &setting, SLOT(setValue(int)));
  connect(&setting, SIGNAL(changed(int)), spinbox, SLOT(setValue(int)));
  spinbox->setValue(setting.value());
}
