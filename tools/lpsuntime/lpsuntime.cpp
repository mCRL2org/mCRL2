// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsuntime.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/lps/tools.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class untime_tool: public input_output_tool
{
  protected:

    typedef input_output_tool super;

  public:

    untime_tool()
      : super(
        "lpsuntime",
        "Jeroen Keiren",
        "remove time from an LPS",
        "Remove time from the linear process specification (LPS) in INFILE and write the "
        "result to OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not "
        "present, stdout is used.")
    {}

    bool run()
    {
      mcrl2::lps::lpsuntime(m_input_filename, m_output_filename);
      return true;
    }

};

class untime_gui_tool: public mcrl2_gui_tool<untime_tool>
{
  public:
    untime_gui_tool()
    {
    }
};

int main(int argc, char** argv)
{
  return untime_gui_tool().execute(argc, argv);
}
