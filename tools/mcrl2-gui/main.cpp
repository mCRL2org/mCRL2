// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/tool.h"
#include "mcrl2/utilities/qt_tool.h"

#include "mainwindow.h"
#include <QSplashScreen>
#include <QTimer>

using namespace mcrl2::utilities;

typedef qt::qt_tool<tools::tool> mcrl2_gui_base;

class mcrl2_gui_tool : public mcrl2_gui_base
{
  private:
    QSplashScreen *m_splash;

  public:
    mcrl2_gui_tool():
      mcrl2_gui_base("mcrl2-gui",
                   "Rimco Boudewijns",
                   "graphical front-end for mCRL2 tools",
                   "A graphical front-end for mCRL2 tools.",
                   "A graphical front-end for mCRL2 tools",
                   "http://mcrl2.org/release/user_manual/tools/mcrl2-gui.html")
    {}

    bool run()
    {

      //Create splash pixmap
      QPixmap pixmap(":/share/mcrl2/mcrl2.png");
      QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

      //Show the splashscreen for 2 seconds
      splash.show();
      QTimer::singleShot(2000, &splash, SLOT(hide()));
      MainWindow *window = new MainWindow();
      return show_main_window(window);

    }
};

int main(int argc, char *argv[])
{
  return mcrl2_gui_tool().execute(argc, argv);
}
