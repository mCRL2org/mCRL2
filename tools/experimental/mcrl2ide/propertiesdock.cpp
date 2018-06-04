#include "propertiesdock.h"

#include <QMainWindow>
#include <QLabel>

PropertiesDock::PropertiesDock(FileSystem *fileSystem, QWidget *parent) : QDockWidget("Properties", parent)
{
    this->fileSystem = fileSystem;

    /* create the properties layout */
    propertiesLayout = new QVBoxLayout();
    propertiesLayout->setAlignment(Qt::AlignTop);
    setToNoProperties();
    innerDockWidget = new QWidget();
    innerDockWidget->setLayout(propertiesLayout);
    this->setWidget(innerDockWidget);
}

void PropertiesDock::setToNoProperties()
{
    /* empty the layout (is usually already empty) */
    QLayoutItem *item;
    while ((item = propertiesLayout->takeAt(0))) {
        propertiesLayout->removeItem(item);
        delete item;
    }
    /* show a QLabel that tells the user that no properties have been defined */
    QLabel *noPropertiesLabel = new QLabel("No properties have been defined");
    propertiesLayout->addWidget(noPropertiesLabel);
}

void PropertiesDock::addProperty(QString propertyName, QString propertyText)
{
    if (propertyWidgets.empty()) {
        /* remove the QLabel */
        QWidget *label = propertiesLayout->takeAt(0)->widget();
        propertiesLayout->removeWidget(label);
        delete label;
    }

    /* add the property to the rest */
    PropertyWidget *propertyWidget = new PropertyWidget(propertyName, propertyText, fileSystem, this);
    propertiesLayout->addWidget(propertyWidget);
    propertyWidgets.push_back(propertyWidget);
}

void PropertiesDock::deleteProperty(PropertyWidget *propertyWidget)
{
    propertiesLayout->removeWidget(propertyWidget);
    if (propertiesLayout->isEmpty()) {
        this->setToNoProperties();
    }
    propertyWidgets.remove(propertyWidget);
}

bool PropertiesDock::propertyNameExists(QString propertyName)
{
    for (PropertyWidget *propertyWidget : propertyWidgets) {
        if (propertyWidget->getPropertyName() == propertyName) {
            return true;
        }
    }
    return false;
}

void PropertiesDock::saveAllProperties()
{
    for (PropertyWidget *propertyWidget : propertyWidgets) {
        propertyWidget->saveProperty();
    }
}
