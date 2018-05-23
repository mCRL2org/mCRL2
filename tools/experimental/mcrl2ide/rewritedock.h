#ifndef REWRITEDOCK_H
#define REWRITEDOCK_H

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

class RewriteDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit RewriteDock(QWidget *parent);

private:
    QLineEdit *expressionEntry;
    QPushButton *rewriteButton;
    QPushButton *rewriteAbortButton;
    QPlainTextEdit *rewriteResult;
};

#endif // REWRITEDOCK_H
