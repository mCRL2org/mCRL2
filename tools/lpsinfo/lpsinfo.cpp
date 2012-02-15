// Author(s): Frank Stappers, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsinfo.cpp

/*include "boost.hpp" // precompiled headers*/

#define TOOLNAME "lpsinfo"
#define AUTHOR "Wieger Wesselink and Frank Stappers"

#include "mcrl2/lps/tools.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::utilities::tools;

class lpsinfo_tool: public input_tool
{
  protected:
    typedef input_tool super;

  public:
    lpsinfo_tool()
      : super(
        TOOLNAME,
        AUTHOR,
        "display basic information about an LPS",
        "Print basic information on the linear process specification (LPS) in INFILE.")
    {}

  public:

    bool run()
    {
      lpsinfo(input_filename(),
              input_file_message()
             );
      return true;
    }

};

class lpsinfo_gui_tool: public mcrl2_gui_tool<lpsinfo_tool>
{
  public:
    lpsinfo_gui_tool()
    {
      // m_gui_options["abc"] = create_textctrl_widget();
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return lpsinfo_gui_tool().execute(argc, argv);
}
