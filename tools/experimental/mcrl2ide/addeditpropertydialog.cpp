#include "addeditpropertydialog.h"
#include "ui_addeditpropertydialog.h"

#include <QMessageBox>

AddEditPropertyDialog::AddEditPropertyDialog(bool add, QWidget *parent, QString propertyName, QString propertyText) :
    QDialog(parent),
    ui(new Ui::AddEditPropertyDialog)
{
    ui->setupUi(this);

    /* change the ui depending on whether this should be an add or edit property window */
    if (add) {
        this->setWindowTitle("Add Property");
        ui->addEditButton->setText("Add");
    } else {
        this->setWindowTitle("Edit Property");
        ui->addEditButton->setText("Edit");
        ui->propertyNameField->setText(propertyName);
        ui->propertyTextField->setPlainText(propertyText);
    }

    connect(ui->addEditButton, SIGNAL(clicked()), this, SLOT(parseAndAccept()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

QString AddEditPropertyDialog::getPropertyName()
{
    return ui->propertyNameField->text();
}

QString AddEditPropertyDialog::getPropertyText()
{
    return ui->propertyTextField->toPlainText();
}

void AddEditPropertyDialog::parseAndAccept()
{
    QMessageBox *msgBox = new QMessageBox();
    msgBox->setStandardButtons(QMessageBox::Ok);

    /* show a message box if the property name field is empty */
    if (ui->propertyNameField->text().trimmed().count() == 0) {
        msgBox->setText("The property name may not be empty");
        msgBox->exec();
        return;
    }

    /* show a message box if the property text field is empty */
    if (ui->propertyTextField->toPlainText().trimmed().count() == 0) {
        msgBox->setText("The property text may not be empty");
        msgBox->exec();
        return;
    }

    /* if everything is ok, accept the input */
    accept();
}

AddEditPropertyDialog::~AddEditPropertyDialog()
{
    delete ui;
}
