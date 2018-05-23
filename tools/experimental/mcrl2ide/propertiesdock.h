#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include <QDockWidget>

class PropertiesDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit PropertiesDock(QWidget *parent);
};

#endif // PROPERTIESDOCK_H
