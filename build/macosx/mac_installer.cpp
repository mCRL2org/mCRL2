#include "mac_installer.h"
#include "ui_mac_installer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include <qapplication.h>

using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QString s= QDir::currentPath();
    ui->lineEdit->setText(s);

}

Dialog::~Dialog()
{
    delete ui;
}

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    Dialog dialog;
    dialog.show();
    return app.exec();
}

void Dialog::accept(){
    exportPath();
}

void Dialog::exportPath()
{
    QString m_exportPath = ui->lineEdit->text();
    if(!m_exportPath.endsWith(QDir::separator()))
    {
        m_exportPath.append(QDir::separator());
    }
    QFileInfo fi(m_exportPath);
    if( !fi.permission(QFile::WriteUser) )
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot write to the supplied directory \""+ m_exportPath+"\".\nPlease select another path.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to export symbolic links to \""+ m_exportPath+"\"?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIcon(QMessageBox::Question);
    if( msgBox.exec() == QMessageBox::No)
    {
        return;
    }

    QDir appDir = QDir(QCoreApplication::applicationDirPath());

    #ifdef __APPLE__
      appDir.cdUp();
      appDir.cdUp();
      appDir.cdUp();
    #endif

    //Get tool file names in directory
    QStringList files;
    files = appDir.entryList(QStringList(QString("*")),
                                 QDir::Files | QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot );

    try
    {
        for( QStringList::Iterator f = files.begin(); f != files.end(); f++ )
        {
            QProcess process;
            QString expPath = m_exportPath;
            QString exec("ln -s ");
            exec.append(appDir.absolutePath().append(QDir::separator().toAscii()).append(*f)).append(" ").append(expPath.append(*f));
            //std::cout << exec.toStdString() <<std::endl;
            process.execute(exec);
        }
        QMessageBox msgBox;
        msgBox.setText("Successfully exported symbolic links to \"" + m_exportPath + "\".");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        exit(EXIT_SUCCESS);

    } catch (...)
    {
        QMessageBox msgBox;
        msgBox.setText("Error during export of symbolic links.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
}

void Dialog::on_Browse_clicked()
{
    QString s = QFileDialog::getExistingDirectory(); // getOpenFileName(0,"");//this, "./", "All Files (*.*)", "open file dialog", "Choose a file..." );
    ui->lineEdit->setText(s);
}
