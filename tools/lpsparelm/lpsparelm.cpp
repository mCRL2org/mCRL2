// Author(s): J.van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsparelm.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/lps/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using namespace mcrl2::utilities;

class lps_parelm_tool : public input_output_tool
{
    typedef input_output_tool super;

  public:

    lps_parelm_tool() : super(
        "lpsparelm",
        "Wieger Wesselink and Jeroen van der Wulp; Frank Stappers and Tim Willemse",
        "remove unused parameters from an LPS",
        "Remove unused parameters from the linear process specification (LPS) in INFILE "
        "and write the result to OUTFILE. If INFILE is not present, stdin is used. If "
        "OUTFILE is not present, stdout is used.")
    {
    }

    bool run()
    {
      lps::lpsparelm(input_filename(),
                     output_filename()
                    );
      return true;
    }

};

class lps_parelm_gui_tool: public mcrl2_gui_tool<lps_parelm_tool>
{
  public:
    lps_parelm_gui_tool()
    {    }
};

int main(int argc, char** argv)
{
  return lps_parelm_gui_tool().execute(argc, argv);
}

