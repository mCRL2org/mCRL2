#include "addeditpropertydialog.h"
#include "ui_addeditpropertydialog.h"
#include "propertywidget.h"

AddEditPropertyDialog::AddEditPropertyDialog(bool add, QWidget *parent, QString propertyName, QString propertyText) :
    QDialog(parent),
    ui(new Ui::AddEditPropertyDialog)
{
    ui->setupUi(this);

    /* change the ui depending on whether this should be an add or edit property window */
    if (add){
        this->setWindowTitle("Add Property");
        ui->addEditButton->setText("Add");
    } else {
        this->setWindowTitle("Edit Property");
        ui->addEditButton->setText("Edit");
        ui->propertyNameField->setText(propertyName);
        ui->propertyTextField->setPlainText(propertyText);
    }

    connect(ui->addEditButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

/**
 * @brief AddEditPropertyDialog::getPropertyName get the string filled in as the property name
 * @return the property name string
 */
QString AddEditPropertyDialog::getPropertyName()
{
    return ui->propertyNameField->text();
}

/**
 * @brief AddEditPropertyDialog::getPropertyText get the string filled in as the property text
 * @return the preoperty text string
 */
QString AddEditPropertyDialog::getPropertyText()
{
    return ui->propertyTextField->toPlainText();
}

AddEditPropertyDialog::~AddEditPropertyDialog()
{
    delete ui;
}
