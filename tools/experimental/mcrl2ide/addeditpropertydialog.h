#ifndef ADDEDITPROPERTYDIALOG_H
#define ADDEDITPROPERTYDIALOG_H

#include <QDialog>

namespace Ui {
class AddEditPropertyDialog;
}

class AddEditPropertyDialog : public QDialog
{
    Q_OBJECT

public:
    AddEditPropertyDialog(bool add, QWidget *parent = 0, QString propertyName = "", QString propertyText = "");
    ~AddEditPropertyDialog();

    QString getPropertyName();
    QString getPropertyText();

private:
    Ui::AddEditPropertyDialog *ui;
};

#endif // ADDEDITPROPERTYDIALOG_H
