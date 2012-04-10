#include "mcrl2xi_qt_actions.h"
#include "ui_mcrl2xi_qt_actions.h"

mcrl2xi_qt_actions::mcrl2xi_qt_actions(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::mcrl2xi_qt_actions)
{
    ui->setupUi(this);
}

mcrl2xi_qt_actions::~mcrl2xi_qt_actions()
{
    delete ui;
}

void mcrl2xi_qt_actions::onParseAndTypeCheck()
{

}
