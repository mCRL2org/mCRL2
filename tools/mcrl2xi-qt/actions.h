#ifndef ACTIONS_H
#define ACTIONS_H

#include <QDockWidget>

namespace Ui {
class actions;
}

class actions : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit actions(QWidget *parent = 0);
    ~actions();
    
private:
    Ui::actions *ui;
    void onParseAndTypeCheck();
};

#endif // ACTIONS_H
