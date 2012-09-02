#ifndef MAC_INSTALLER_H
#define MAC_INSTALLER_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    
private slots:
    void on_Browse_clicked();

private:
    Ui::Dialog *ui;
    void exportPath();
    void accept();
};

#endif // MAC_INSTALLER_H
