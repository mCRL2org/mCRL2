#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include "propertywidget.h"
#include "processsystem.h"

#include <QDockWidget>
#include <QVBoxLayout>

class PropertyWidget;

/**
 * @brief The PropertiesDock class defines the dock that contains the properties
 */
class PropertiesDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    /**
     * @brief PropertiesDock Constructor
     * @param parent The parent of this widget
     */
    PropertiesDock(ProcessSystem *processSystem, QWidget *parent);

    /**
     * @brief addProperty Adds a property to the dock
     * @param propertyName The name of the property
     * @param propertyText The body of the property
     */
    void addProperty(QString propertyName, QString propertyText);

    /**
     * @brief deleteProperty Removes a property from the dock
     * @param propertyWidget The property widget to remove
     */
    void deleteProperty(PropertyWidget *propertyWidget);

    /**
     * @brief propertyNameExists Checks whether there already exists a property with the name propertyName
     * @param propertyName The property name to check for
     * @return Whether there already exists a property with the name propertyName
     */
    bool propertyNameExists(QString propertyName);

    /**
     * @brief getAllProperties Gets all properties in this dock
     * @return All properties in this dock
     */
    std::list<Property*> getAllProperties();

    /**
     * @brief verifyAllProperties Verifies all properties in this dock
     */
    void verifyAllProperties();

private:
    ProcessSystem *processSystem;
    QWidget *innerDockWidget;
    QVBoxLayout *propertiesLayout;
    std::list<PropertyWidget*> propertyWidgets;

    /**
     * @brief setToNoProperties Empties the dock
     */
    void setToNoProperties();
};

#endif // PROPERTIESDOCK_H
