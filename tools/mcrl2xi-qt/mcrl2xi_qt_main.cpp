#include <QtGui/QApplication>
#include "mcrl2xi_qt_mainwindow.h"
#include "mcrl2/aterm/aterm.h"
#include "mcrl2/atermpp/aterm_init.h"

int main(int argc, char *argv[])
{

    // aterm::ATerm stack;
    // aterm::ATinit(&stack);
    atermpp::aterm_init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
