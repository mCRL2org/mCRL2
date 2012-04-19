#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
//    aterm::ATerm stack;
//    aterm::ATinit(&stack);

//    mcrl2::log::logger::set_reporting_level(mcrl2::log::verbose);

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return app.exec();
}
