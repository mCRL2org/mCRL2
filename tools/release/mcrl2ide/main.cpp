// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/gui/qt_tool.h"
#include "mainwindow.h"

using namespace mcrl2;

typedef gui::qt::qt_tool<utilities::tools::input_tool> mcrl2ide_base;

class mcrl2ide_tool : public mcrl2ide_base
{
  public:
  mcrl2ide_tool()
      : mcrl2ide_base("mCRL2ide", "Olav Bunte",
                      "A graphical IDE for creating, analyzing, visualizing "
                      "and verifying mCRL2 specifications.",
                      "A graphical IDE for creating, analyzing, visualizing "
                      "and verifying mCRL2 specifications.",
                      "A graphical IDE for creating, analyzing, visualizing "
                      "and verifying mCRL2 specifications.",
                      "http://www.mcrl2.org/web/user_manual/tools/release/"
                      "mcrl2ide.html")
  {
  }

  bool run()
  {
    MainWindow window(QString::fromStdString(m_input_filename));
    return show_main_window(window);
  }
};

int main(int argc, char* argv[])
{
  return mcrl2ide_tool().execute(argc, argv);
}
