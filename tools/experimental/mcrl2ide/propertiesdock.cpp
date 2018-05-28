#include "propertiesdock.h"

#include <QMainWindow>
#include <QLabel>

PropertiesDock::PropertiesDock(QWidget *parent) : QDockWidget("Properties", parent)
{
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
    while((item = propertiesLayout->takeAt(0))) {
        propertiesLayout->removeItem(item);
        delete item;
    }
    /* show a QLabel that tells the user that no properties have been defined */
    QLabel *noPropertiesLabel = new QLabel("No properties have been defined");
    propertiesLayout->addWidget(noPropertiesLabel);
    noProperties = true;
}

void PropertiesDock::addProperty(QString propertyName, QString propertyText)
{
    if (noProperties){
        /* remove the QLabel */
        QWidget *label = propertiesLayout->takeAt(0)->widget();
        propertiesLayout->removeWidget(label);
        delete label;
        noProperties = false;
    }
    propertiesLayout->addWidget(new PropertyWidget(propertyName, propertyText, this));
}

void PropertiesDock::deleteProperty(PropertyWidget *property)
{
    propertiesLayout->removeWidget(property);
    if (propertiesLayout->isEmpty()){
        this->setToNoProperties();
    }
}
