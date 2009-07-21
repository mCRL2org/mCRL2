// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file main.cpp
//
// Main file of the GraPE application

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2gen/mcrl2gen.h"
#include "grape_frame.h"

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/utilities/input_tool.h"

#include <iostream>

#define NAME   "grape"
#define AUTHOR "Remco Blewanus, Thorstin Crijns, Diana Koenraadt, Bas Luksenburg, Jonathan Nelisse, Hans Poppelaars and Bram Schoenmakers"

/**
 * \brief toolset version string
 **/
using namespace grape::grapeapp;
using namespace grape::mcrl2gen;
using namespace mcrl2::core;
using namespace mcrl2::utilities::tools;

class grape_app: public mcrl2::utilities::wx::tool< grape_app, input_tool >
{  
  friend class mcrl2::utilities::wx::tool< grape_app, input_tool >;

  private:

    std::vector< std::string > developers() {
      static char const* developer_names[] = {"Remco Blewanus", "Thorstin Crijns",
           "Diana Koenraadt", "Bas Luksenburg", "Jonathan Nelisse", "Hans Poppelaars", "Bram Schoenmakers"};

      return std::vector< std::string >(&developer_names[0], &developer_names[7]);
    }

    std::vector< std::string > documenters() {
      return std::vector< std::string >(1, "Hans Poppelaars");
    }

  public:
    grape_app() : mcrl2::utilities::wx::tool< grape_app, input_tool >("GraPE",    
                  "graphical editing environment for mCRL2 process specifications",
                  "Graphical Process Editor for mCRL2.",
                  "Graphical editing environment for mCRL2 process specifications. "
                  "If INFILE is supplied, it is loaded as a GraPE specification.",
                  developers(),
                  "",
                  documenters()) { 
    }

  bool run()
  {
    // create a new frame using the input file
    grape_frame *frame = new grape_frame( wxString(input_filename().c_str(), wxConvUTF8) );
    SetTopWindow(frame);

    wxInitAllImageHandlers();

    return true;
  }
};

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                                  HINSTANCE hPrevInstance,
                                  wxCmdLineArgType lpCmdLine,
                                  int nCmdShow) {

  MCRL2_ATERM_INIT(0, lpCmdLine);
  gsSetVerboseMsg();
  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#endif

int main(int argc, char** argv)
{
  init_mcrl2libs(argc, argv);
  return wxEntry(argc, argv);
}

IMPLEMENT_APP_NO_MAIN(grape_app)
