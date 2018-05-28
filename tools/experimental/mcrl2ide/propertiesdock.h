#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include "propertywidget.h"

#include <QDockWidget>
#include <QVBoxLayout>

/**
 * @brief The PropertiesDock class defines the dock that contains the properties
 */
class PropertiesDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit PropertiesDock(QWidget *parent);


    /**
     * @brief PropertiesDock::addProperty adds a property to the dock
     * @param propertyName the name of the property
     * @param propertyText the body of the property
     */
    void addProperty(QString propertyName, QString propertyText);

    /**
     * @brief PropertiesDock::deleteProperty removes a property from the dock
     * @param property the property to remove
     */
    void deleteProperty(PropertyWidget *property);

private:
    bool noProperties;
    QWidget *innerDockWidget;
    QVBoxLayout *propertiesLayout;


    /**
     * @brief PropertiesDock::setToNoProperties empties the dock
     */
    void setToNoProperties();
};

#endif // PROPERTIESDOCK_H
