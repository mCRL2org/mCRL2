#ifndef ADDEDITPROPERTYDIALOG_H
#define ADDEDITPROPERTYDIALOG_H

#include <QDialog>

namespace Ui {
class AddEditPropertyDialog;
}

/**
 * @brief The AddEditPropertyDialog class defines the dialog used to add or edit a property
 */
class AddEditPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    AddEditPropertyDialog(bool add, QWidget *parent = 0, QString propertyName = "", QString propertyText = "");
    ~AddEditPropertyDialog();

    /**
     * @brief getPropertyName get the string filled in as the property name
     * @return the property name string
     */
    QString getPropertyName();

    /**
     * @brief AddEditPropertyDialog::getPropertyText get the string filled in as the property text
     * @return the preoperty text string
     */
    QString getPropertyText();

private:
    Ui::AddEditPropertyDialog *ui;
};

#endif // ADDEDITPROPERTYDIALOG_H
