// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "propertiesdock.h"

#include <QMainWindow>

PropertiesDock::PropertiesDock(ProcessSystem* processSystem,
                               FileSystem* fileSystem,
                               FindAndReplaceDialog* findAndReplaceDialog,
                               QWidget* parent)
    : QDockWidget("Properties", parent), processSystem(processSystem),
      fileSystem(fileSystem), findAndReplaceDialog(findAndReplaceDialog)
{
  /* create the properties layout */
  propertiesLayout = new QVBoxLayout();
  propertiesLayout->setAlignment(Qt::AlignTop);
  setToNoProperties();

  QWidget* innerScrollWidget = new QWidget();
  innerScrollWidget->setLayout(propertiesLayout);

  innerDockWidget = new QScrollArea();
  innerDockWidget->setWidget(innerScrollWidget);
  innerDockWidget->setWidgetResizable(true);
  innerDockWidget->setFrameShape(QFrame::NoFrame);

  this->setWidget(innerDockWidget);

  connect(fileSystem, SIGNAL(propertyAdded(Property)), this,
          SLOT(addProperty(Property)));
}

PropertiesDock::~PropertiesDock()
{
  propertiesLayout->deleteLater();
  innerDockWidget->deleteLater();
}

void PropertiesDock::setToNoProperties()
{
  /* empty the layout */
  QLayoutItem* item;
  while ((item = propertiesLayout->takeAt(0)))
  {
    QWidget* propertyWidget = item->widget();
    propertiesLayout->removeWidget(propertyWidget);
    if (propertyWidget != nullptr)
    {
      propertyWidget->deleteLater();
    }
  }
  /* show a QLabel that tells the user that no properties have been defined */
  QLabel* noPropertiesLabel = new QLabel("No properties have been defined");
  propertiesLayout->addWidget(noPropertiesLabel);

  propertyWidgets.clear();
}

void PropertiesDock::addProperty(const Property& property)
{
  if (propertyWidgets.empty())
  {
    /* remove the QLabel */
    QWidget* label = propertiesLayout->takeAt(0)->widget();
    propertiesLayout->removeWidget(label);
    label->deleteLater();
    propertiesLayout->addStretch(1);
  }

  /* add the property to the rest */
  PropertyWidget* propertyWidget = new PropertyWidget(
      property, processSystem, fileSystem, findAndReplaceDialog, this);
  propertiesLayout->insertWidget(propertiesLayout->count() - 1, propertyWidget);
  propertyWidgets.push_back(propertyWidget);
  connect(propertyWidget, SIGNAL(deleteMe(PropertyWidget*)), this,
          SLOT(deletePropertyWidget(PropertyWidget*)));
}

void PropertiesDock::deletePropertyWidget(PropertyWidget* propertyWidget)
{
  propertiesLayout->removeWidget(propertyWidget);
  if (propertiesLayout->isEmpty())
  {
    this->setToNoProperties();
  }
  propertyWidgets.remove(propertyWidget);
  propertyWidget->deleteLater();
}

void PropertiesDock::verifyAllProperties()
{
  for (PropertyWidget* propertyWidget : propertyWidgets)
  {
    propertyWidget->actionVerify();
  }
}

void PropertiesDock::resetAllPropertyWidgets()
{
  for (PropertyWidget* propertyWidget : propertyWidgets)
  {
    propertyWidget->resetWidget();
  }
}
