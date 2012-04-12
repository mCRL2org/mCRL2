#include <QtGui/QApplication>
#include "mainwindow.h"
#include "mcrl2/aterm/aterm1.h"

#include "mcrl2/utilities/logger.h"

int main(int argc, char *argv[])
{
    aterm::ATerm stack;
    aterm::ATinit(&stack);

    mcrl2::log::logger::set_reporting_level(mcrl2::log::verbose);
    mCRL2log(mcrl2::log::status) << "Long sentence.\n";
    mCRL2log(mcrl2::log::status) << "Short.\n";

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
