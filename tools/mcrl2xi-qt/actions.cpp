#include "actions.h"
#include "ui_actions.h"

actions::actions(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::actions)
{
    ui.setupUi(this);
}

actions::~actions()
{
    delete ui;
}

void actions::onParseAndTypeCheck()
{

}
