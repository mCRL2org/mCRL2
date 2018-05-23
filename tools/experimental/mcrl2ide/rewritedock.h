#ifndef REWRITEDOCK_H
#define REWRITEDOCK_H

#include <QDockWidget>

class RewriteDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit RewriteDock(QWidget *parent);
};

#endif // REWRITEDOCK_H
