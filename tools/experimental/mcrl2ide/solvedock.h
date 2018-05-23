#ifndef SOLVEDOCK_H
#define SOLVEDOCK_H

#include <QDockWidget>

class SolveDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit SolveDock(QWidget *parent);
};

#endif // SOLVEDOCK_H
