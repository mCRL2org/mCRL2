#ifndef REWRITEDOCK_H
#define REWRITEDOCK_H

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

/**
 * @brief The RewriteDock class defines the dock where the user can rewrite data expressions
 */
class RewriteDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

    /**
     * @brief RewriteDock Constructor
     * @param parent The parent of this widget
     */
    explicit RewriteDock(QWidget *parent);

private:
    QLineEdit *expressionEntry;
    QPushButton *rewriteButton;
    QPushButton *rewriteAbortButton;
    QPlainTextEdit *rewriteResult;
};

#endif // REWRITEDOCK_H
