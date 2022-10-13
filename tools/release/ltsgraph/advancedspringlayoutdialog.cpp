#include "advancedspringlayoutdialog.h"
#include "ui_advancedspringlayoutdialog.h"

AdvancedSpringLayoutDialog::AdvancedSpringLayoutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedSpringLayoutDialog)
{
    ui->setupUi(this);
}

AdvancedSpringLayoutDialog::~AdvancedSpringLayoutDialog()
{
    delete ui;
}
