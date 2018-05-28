#include "addeditpropertydialog.h"
#include "propertiesdock.h"
#include "propertywidget.h"

PropertyWidget::PropertyWidget(QString name, QString text, PropertiesDock *parent) : QWidget(parent)
{
    this->parent = parent;
    this->name = name;
    this->text = text;

    /* create the label for the property name */
    propertyNameLabel = new QLabel(name);

    /* create the verify button */
    verifyButton = new QPushButton();
    verifyButton->setIcon(QIcon(":/icons/cogwheelmedium.png")); /* placeholder */
    verifyButton->setIconSize(QSize(32, 32));
    connect(verifyButton, SIGNAL(clicked()), this, SLOT(actionVerify()));

    /* create the edit button */
    QPushButton *editButton = new QPushButton();
    editButton->setIcon(QIcon(":/icons/cogwheelmedium.png")); /* placeholder */
    editButton->setIconSize(QSize(32, 32));
    connect(editButton, SIGNAL(clicked()), this, SLOT(actionEdit()));

    /* create the delete button */
    QPushButton *deleteButton = new QPushButton();
    deleteButton->setIcon(QIcon(":/icons/cogwheelmedium.png")); /* placeholder */
    deleteButton->setIconSize(QSize(32, 32));
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

/**
 * @brief PropertyWidget::getPropertyName gets the name of the property of this widget
 * @return the name of the property
 */
QString PropertyWidget::getPropertyName()
{
    return name;
}

/**
 * @brief PropertyWidget::getPropertyText gets the text of the property of this widget
 * @return the text of the property
 */
QString PropertyWidget::getPropertyText()
{
    return text;
}

/**
 * @brief PropertyWidget::setPropertyName set the name of the property of this widget
 * @param name the new name of the property
 */
void PropertyWidget::setPropertyName(QString name)
{
    this->name = name;
    propertyNameLabel->setText(name);
}

/**
 * @brief PropertyWidget::setPropertyText set the text of the property of this widget
 * @param text the new text of the property
 */
void PropertyWidget::setPropertyText(QString text)
{
    this->text = text;
}

/**
 * @brief PropertyWidget::actionVerify allows the user to verify this property
 */
void PropertyWidget::actionVerify()
{
    /* not yet implemented */
}

/**
 * @brief PropertyWidget::actionEdit allows the user to edit this property
 */
void PropertyWidget::actionEdit()
{
    AddEditPropertyDialog *editPropertyDialog = new AddEditPropertyDialog(false, this, name, text);
    /* if editing was succesfull (Edit button was pressed), update the property and its widget */
    if (editPropertyDialog->exec()){
        name = editPropertyDialog->getPropertyName();
        text = editPropertyDialog->getPropertyText();
        propertyNameLabel->setText(name);
    }
}

/**
 * @brief PropertyWidget::actionDelete allows the user to delete this property
 */
void PropertyWidget::actionDelete()
{
    parent->deleteProperty(this);
    delete this;
}
