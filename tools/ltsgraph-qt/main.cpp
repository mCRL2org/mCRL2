// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/aterm/aterm1.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/qt_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mainwindow.h"

using namespace mcrl2::utilities;

typedef qt::qt_tool<tools::input_tool> ltsgraph_base;
class ltsgraph_tool : public ltsgraph_base
{
  public:
    ltsgraph_tool():
      ltsgraph_base("LTSGraph",
                   "Rimco Boudewijns and Sjoerd Cranen",
                   "visualise an LTS as a graph and manipulate its layout in 2D and 3D",
                   "Tool for visualizing a labelled transition systems as a graph, and optimizing graph layout.",
                   "Draws a labelled transition system (LTS). A convential graph drawing is provided either in 2D or 3D.",
                   "http://mcrl2.org/release/user_manual/tools/ltsgraph.html")
    {}

    bool run()
    {
      MainWindow *window = new MainWindow();

      if (!m_input_filename.empty())
      {
        window->openFile(QString::fromStdString(m_input_filename));
      }

      return show_main_window(window);
    }
};

int main(int argc, char *argv[])
{
  //Separate thread not needed in this case
  aterm::ATerm stack;
  aterm::ATinit(&stack);

  return ltsgraph_tool().execute(argc, argv);
}
