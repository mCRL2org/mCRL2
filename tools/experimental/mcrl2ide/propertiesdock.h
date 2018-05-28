#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include "propertywidget.h"

#include <QDockWidget>
#include <QVBoxLayout>

class PropertiesDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit PropertiesDock(QWidget *parent);

    void addProperty(QString propertyName, QString propertyText);
    void deleteProperty(PropertyWidget *property);

private:
    bool noProperties;
    QWidget *innerDockWidget;
    QVBoxLayout *propertiesLayout;

    void setToNoProperties();
};

#endif // PROPERTIESDOCK_H
