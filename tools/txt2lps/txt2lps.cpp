// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file txt2lps.cpp
/// \brief This tool reads a mcrl2 specification of a linear process,
/// and translates it directly into LPS format.

#include "boost.hpp" // precompiled headers

#define TOOLNAME "txt2lps"
#define AUTHOR "Wieger Wesselink"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/lps/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class txt2lps_tool : public input_output_tool
{
    typedef input_output_tool super;

  public:

    txt2lps_tool() : super(
        TOOLNAME,
        AUTHOR,
        "translates an mCRL2 specification of a linear process into an LPS",
        "Translates the mCRL2 specification in INFILE and writes the resulting LPS to "
        "OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not "
        "present, standard input is used.")
    {}

    bool run()
    {
      lps::txtlps(input_filename(),
                  output_filename()
                 );
      return true;
    }

};

class txt2lps_gui_tool: public mcrl2_gui_tool<txt2lps_tool>
{
  public:
    txt2lps_gui_tool() {}
};

int main(int argc, char** argv)
{
  
  atermpp::aterm_init();
  return txt2lps_gui_tool().execute(argc, argv);
}
