// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/gui/qt_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mainwindow.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using mcrl2::data::tools::rewriter_tool;

typedef gui::qt::qt_tool<rewriter_tool<tools::input_tool> > mcrl2xi_base;

class mcrl2xi_tool : public mcrl2xi_base
{
  public:
    mcrl2xi_tool():
      mcrl2xi_base("mCRL2xi",
                   "Rimco Boudewijns and Frank Stappers",
                   "graphical mCRL2 data specification editor",
                   "A graphical mCRL2 data specification editor.",
                   "A graphical mCRL2 data specification editor.",
                   "http://www.mcrl2.org/")
    {}

    bool run()
    {
      QThread *atermThread = new QThread;
      atermThread->start();
      atermThread->setPriority(QThread::IdlePriority);

      MainWindow *window = new MainWindow(atermThread, rewrite_strategy());

      if (!m_input_filename.empty())
      {
        window->openDocument(QString::fromStdString(m_input_filename));
      }

      return show_main_window(window);
    }
};

int main(int argc, char *argv[])
{
  return mcrl2xi_tool().execute(argc, argv);
}
