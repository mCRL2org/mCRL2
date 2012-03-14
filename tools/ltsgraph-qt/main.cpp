#include <QtGui/QApplication>
#include "mainwindow.h"
#include "mcrl2/aterm/aterm1.h"

#define _GNU_SOURCE 1
#include <fenv.h>
static void __attribute__ ((constructor))
trapfpe ()
{
  // Enable some exceptions.  At startup all exceptions are masked.
  // feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
  feenableexcept (0);
}


int main(int argc, char *argv[])
{
    aterm::ATerm stack;
    aterm::ATinit(argc, argv, &stack);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
