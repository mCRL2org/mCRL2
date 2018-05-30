#include "addeditpropertydialog.h"
#include "propertiesdock.h"
#include "propertywidget.h"

#include <QMessageBox>

PropertyWidget::PropertyWidget(QString name, QString text, PropertiesDock *parent) : QWidget(parent)
{
    this->parent = parent;
    this->name = name;
    this->text = text;

    /* create the label for the property name */
    propertyNameLabel = new QLabel(name);

    /* create the verify button */
    verifyButton = new QPushButton();
    verifyButton->setIcon(QIcon(":/icons/verify.png")); /* placeholder */
    verifyButton->setIconSize(QSize(24, 24));
    verifyButton->setStyleSheet("border:none;");
    connect(verifyButton, SIGNAL(clicked()), this, SLOT(actionVerify()));

    /* create the edit button */
    QPushButton *editButton = new QPushButton();
    editButton->setIcon(QIcon(":/icons/edit.png")); /* placeholder */
    editButton->setIconSize(QSize(24, 24));
    editButton->setStyleSheet("border:none;");
    connect(editButton, SIGNAL(clicked()), this, SLOT(actionEdit()));

    /* create the delete button */
    QPushButton *deleteButton = new QPushButton();
    deleteButton->setIcon(QIcon(":/icons/delete.png")); /* placeholder */
    deleteButton->setIconSize(QSize(24, 24));
    deleteButton->setStyleSheet("border:none;");
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(actionDelete()));

    /* lay them out */
    propertyLayout = new QHBoxLayout();
    propertyLayout->setAlignment(Qt::AlignRight);
    propertyLayout->addWidget(propertyNameLabel);
    propertyLayout->addWidget(verifyButton);
    propertyLayout->addWidget(editButton);
    propertyLayout->addWidget(deleteButton);

    this->setLayout(propertyLayout);
}

QString PropertyWidget::getPropertyName()
{
    return name;
}

QString PropertyWidget::getPropertyText()
{
    return text;
}

void PropertyWidget::setPropertyName(QString name)
{
    this->name = name;
    propertyNameLabel->setText(name);
}

void PropertyWidget::setPropertyText(QString text)
{
    this->text = text;
}

void PropertyWidget::actionVerify()
{
    /* not yet implemented */
}

void PropertyWidget::actionEdit()
{
    AddEditPropertyDialog *editPropertyDialog = new AddEditPropertyDialog(false, this, name, text);

    /* if editing was succesful (Edit button was pressed), update the property and its widget */
    if (editPropertyDialog->exec()) {
        name = editPropertyDialog->getPropertyName();
        text = editPropertyDialog->getPropertyText();
        propertyNameLabel->setText(name);
    }
}

void PropertyWidget::actionDelete()
{
    /* show a message box to ask the user whether he is sure to delete the property */
    QMessageBox *msgBox = new QMessageBox();
    msgBox->setText("Are you sure you want to delete the property " + name + "?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    /* only delete the property if the user agrees */
    if (msgBox->exec() == QMessageBox::Yes) {
        parent->deleteProperty(this);
        delete this;
    }
}
