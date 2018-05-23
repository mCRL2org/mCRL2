#ifndef SOLVEDOCK_H
#define SOLVEDOCK_H

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

class SolveDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    explicit SolveDock(QWidget *parent);

private:
    QLineEdit *expressionEntry;
    QPushButton *solveButton;
    QPushButton *solveAbortButton;
    QPlainTextEdit *solveResult;
};

#endif // SOLVEDOCK_H
