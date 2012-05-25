#ifndef MCRL2XI_QT_ACTIONS_H
#define MCRL2XI_QT_ACTIONS_H

#include <QDockWidget>

namespace Ui {
class mcrl2xi_qt_actions;
}

class mcrl2xi_qt_actions : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit mcrl2xi_qt_actions(QWidget *parent = 0);
    ~mcrl2xi_qt_actions();
    
private:
    Ui::mcrl2xi_qt_actions *ui;
    void onParseAndTypeCheck();
};

#endif // MCRL2XI_QT_ACTIONS_H
