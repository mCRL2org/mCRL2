#include "addeditpropertydialog.h"
#include "ui_addeditpropertydialog.h"

#include <QMessageBox>

AddEditPropertyDialog::AddEditPropertyDialog(bool add, PropertiesDock *propertiesDock, QWidget *parent, QString propertyName, QString propertyText) :
    QDialog(parent),
    ui(new Ui::AddEditPropertyDialog)
{
    ui->setupUi(this);

    this->propertiesDock = propertiesDock;

    /* change the ui depending on whether this should be an add or edit property window */
    if (add) {
        windowTitle = "Add Property";
        ui->addEditButton->setText("Add");
    } else {
        windowTitle = "Edit Property";
        ui->addEditButton->setText("Edit");
        ui->propertyNameField->setText(propertyName);
        ui->propertyTextField->setPlainText(propertyText);
    }
    setWindowTitle(windowTitle);

    connect(ui->addEditButton, SIGNAL(clicked()), this, SLOT(parseAndAccept()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

QString AddEditPropertyDialog::getPropertyName()
{
    return ui->propertyNameField->text().trimmed();
}

QString AddEditPropertyDialog::getPropertyText()
{
    return ui->propertyTextField->toPlainText();
}

void AddEditPropertyDialog::parseAndAccept()
{
    QString propertyName = ui->propertyNameField->text().trimmed();
    /* show a message box if the property name field is empty */
    if (propertyName.count() == 0) {
        QMessageBox::information(this, windowTitle, "The property name may not be empty", QMessageBox::Ok);
        return;
    }

    /* show a message box if this property name already exists */
    if (propertiesDock->propertyNameExists(propertyName)) {
        QMessageBox::information(this, windowTitle, "A property with this name already exists", QMessageBox::Ok);
        return;
    }

    /* show a message box if the property text field is empty */
    if (ui->propertyTextField->toPlainText().trimmed().count() == 0) {
        QMessageBox::information(this, windowTitle, "The property text may not be empty", QMessageBox::Ok);
        return;
    }

    /* if everything is ok, accept the input */
    accept();
}

AddEditPropertyDialog::~AddEditPropertyDialog()
{
    delete ui;
}
