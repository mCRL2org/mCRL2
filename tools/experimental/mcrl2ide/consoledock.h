#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QDockWidget>

class ConsoleDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::BottomDockWidgetArea;

    explicit ConsoleDock(QWidget *parent);
};

#endif // CONSOLEDOCK_H
