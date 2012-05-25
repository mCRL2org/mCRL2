#include <QtGui/QApplication>
#include "mainwindow.h"
#include "mcrl2/atermpp/aterm_init.h"

int main(int argc, char *argv[])
{
    atermpp::aterm_init();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
