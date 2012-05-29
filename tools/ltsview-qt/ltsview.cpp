// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsview.cpp
/// \brief Contains implementation of the LTSView application.

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QWidget>



#include "wx.hpp" // precompiled headers

#define NAME "ltsview"
#define AUTHOR "Bas Ploeger and Carst Tankink"

#include "ltsview.h"
#include <string>
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/image.h>
#include "mcrl2/lts/lts.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/logger.h"
#include "cluster.h"
#include "glcanvas.h"
#include "lts.h"
#include "mcrl2/lts/lts_io.h"
#include "mainframe.h"
#include "markmanager.h"
#include "settings.h"
#include "state.h"
#include "visualizer.h"

using namespace std;

static
std::vector< std::string > developers()
{
  static char const* developer_names[] = {"Bas Ploeger", "Carst Tankink"};
  return std::vector< std::string >(&developer_names[0], &developer_names[2]);
}


LTSView::LTSView() : super("LTSView",
                             "3D interactive visualization of a labelled transition system", // what-is
                             "Tool for interactive visualization of state transition systems.\n" // gui-specific description
                             "\n"
                             "LTSView is based on visualization techniques by Frank van Ham and Jack van Wijk.\n"
                             "See: F. van Ham, H. van de Wetering and J.J. van Wijk,\n"
                             "\"Visualization of State Transition Graphs\". "
                             "Proceedings of the IEEE Symposium on Information Visualization 2001. IEEE CS Press, pp. 59-66, 2001.\n"
                             "\n"
                             "The default colour scheme for state marking was obtained from http://www.colorbrewer.org",
                             "Start the LTSView application. If INFILE is supplied then the " // command-line description
                             "LTS in INFILE is loaded into the application.\n"
                             "\n"
                             "The input format is determined by the contents of INFILE. If that fails, "
                             "an attempt is made to force the input format based on the file extension. "
                             "The supported formats with file extensions are:\n"
                             "  Aldebaran format (CADP; *.aut);\n"
#ifdef USE_BCG
                             "  Binary Coded Graph format (CADP; *.bcg);\n"
#endif
                             "  GraphViz format (*.dot);\n"
                             "  Finite State Machine format (*.fsm);\n"
                             "  mCRL SVC format (*.svc);\n"
                             "  mCRL2 format (*.lts).",
                             developers())
{ }

IMPLEMENT_APP_NO_MAIN(LTSView_gui_tool)
IMPLEMENT_WX_THEME_SUPPORT
BEGIN_EVENT_TABLE(LTSView, wxApp)
END_EVENT_TABLE()

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              wxCmdLineArgType lpCmdLine,
                              int nCmdShow)
{

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  QApplication app(argc, argv);

  return wxEntry(argc, argv);
}
#endif

class LogMessenger : public mcrl2::log::output_policy
{
  public:
    LogMessenger(QWidget *parent): m_parent(parent) {}
    void output(const mcrl2::log::log_level_t level, const std::string& hint, const time_t timestamp, const std::string& msg)
    {
      if (level == mcrl2::log::error)
      {
        QMessageBox::critical(m_parent, "LTSView - An error occured", QString::fromStdString(msg));
      }
    }
  private:
    QWidget *m_parent;
};


bool LTSView::run()
{
  // TODO: s/0/this/
  log = new LogMessenger(0);
  mcrl2::log::logger::register_output_policy(*log);
  mainFrame = new MainFrame();

  SetTopWindow(mainFrame);
  wxInitAllImageHandlers();
  return true;
}

LTSView::~LTSView()
{
  mcrl2::log::logger::unregister_output_policy(*log);
  delete log;
}
