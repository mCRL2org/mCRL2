#include <QApplication>
#include <QDateTime>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    qsrand(QDateTime::currentDateTime().toTime_t());

    return app.exec();
}
