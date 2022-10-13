#ifndef ADVANCEDSPRINGLAYOUTDIALOG_H
#define ADVANCEDSPRINGLAYOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AdvancedSpringLayoutDialog;
}

class AdvancedSpringLayoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedSpringLayoutDialog(QWidget *parent = nullptr);
    ~AdvancedSpringLayoutDialog();

private:
    Ui::AdvancedSpringLayoutDialog *ui;
};

#endif // ADVANCEDSPRINGLAYOUTDIALOG_H
