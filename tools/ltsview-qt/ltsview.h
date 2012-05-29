// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file
/// \brief Contains the LTSView application class.

#ifndef LTSVIEW_H
#define LTSVIEW_H

#include <QColor>
#include <QObject>

#include <wx/wx.h>
#include <wx/app.h>
#include <string>

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

#include "enums.h"
#include "ltsmanager.h"
#include "settings.h"

class LogMessenger;
class GLCanvas;
class LTS;
class MainFrame;
class MarkManager;
class Visualizer;

class LTSView :
  public QObject,
  public mcrl2::utilities::wx::tool< LTSView, mcrl2::utilities::tools::input_tool >
{
  Q_OBJECT

    typedef mcrl2::utilities::wx::tool< LTSView,
            mcrl2::utilities::tools::input_tool > super;

  private:
    LogMessenger* log;
    MainFrame* mainFrame;

  public:

    LTSView();
    ~LTSView();
    virtual bool run();
    bool parse_command_line(int argc, wxChar** argv);

    void printHelp(std::string const& name);
    void printVersion();

    DECLARE_EVENT_TABLE()
};

class LTSView_gui_tool: public mcrl2::utilities::mcrl2_gui_tool<LTSView>
{
  public:
    LTSView_gui_tool()
    {
      //m_gui_options["no-state"] = create_checkbox_widget();
    }
};

DECLARE_APP(LTSView_gui_tool)

#endif
