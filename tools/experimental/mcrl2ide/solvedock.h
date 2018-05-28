#ifndef SOLVEDOCK_H
#define SOLVEDOCK_H

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

/**
 * @brief The SolveDock class defines the dock where the user can solve data expressions
 */
class SolveDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    /**
     * @brief SolveDock Constructor
     * @param parent The parent of this widget
     */
    explicit SolveDock(QWidget *parent);

private:
    QLineEdit *expressionEntry;
    QPushButton *solveButton;
    QPushButton *solveAbortButton;
    QPlainTextEdit *solveResult;
};

#endif // SOLVEDOCK_H
