#include <QtGui/QApplication>
#include "mcrl2xi_qt_mainwindow.h"
#include "mcrl2/aterm/aterm1.h"

int main(int argc, char *argv[])
{

    aterm::ATerm stack;
    aterm::ATinit(&stack);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
