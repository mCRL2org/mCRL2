// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QtGui/QApplication>
#include "mainwindow.h"
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
