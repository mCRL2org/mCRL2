#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QTabWidget>

/**
 * @brief The ConsoleDock class defines the dock that shows console output
 */
class ConsoleDock : public QDockWidget
{
    Q_OBJECT

public:
    const Qt::DockWidgetArea defaultArea = Qt::BottomDockWidgetArea;

    explicit ConsoleDock(QWidget *parent);

private:
    QTabWidget *consoleTabs;
    QPlainTextEdit *parseConsole;
    QPlainTextEdit *LTSConsole;
    QPlainTextEdit *verificationConsole;
};

#endif // CONSOLEDOCK_H
