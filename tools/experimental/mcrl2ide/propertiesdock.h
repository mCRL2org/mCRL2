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

    /**
     * @brief PropertiesDock Constructor
     * @param parent The parent of this widget
     */
    explicit PropertiesDock(QWidget *parent);

    /**
     * @brief deleteProperty Removes a property from the dock
     * @param property The property to remove
     */
    void deleteProperty(PropertyWidget *property);

public slots:
    /**
     * @brief addProperty Adds a property to the dock
     * @param propertyName The name of the property
     * @param propertyText The body of the property
     */
    void addProperty(QString propertyName, QString propertyText);

private:
    bool noProperties;
    QWidget *innerDockWidget;
    QVBoxLayout *propertiesLayout;


    /**
     * @brief setToNoProperties Empties the dock
     */
    void setToNoProperties();
};

#endif // PROPERTIESDOCK_H
