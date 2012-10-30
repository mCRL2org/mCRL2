// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file txt2pbes.cpp
/// \brief Parse a textual description of a BES.

#include "boost.hpp" // precompiled headers

#define NAME "txt2bes"
#define AUTHOR "Wieger Wesselink"

//C++
#include <cstdio>
#include <fstream>
#include <string>

//mCRL2 specific
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/pbesinstconversion.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class txt2pbes_tool: public input_output_tool
{
    typedef input_output_tool super;

  public:
    txt2pbes_tool()
      : super(NAME, AUTHOR,
              "parse a textual description of a BES",
              "Parse the textual description of a BES from INFILE and write it to OUTFILE. "
              "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."
             )
    {}

    bool run()
    {
      pbes_system::pbes<> p;
      if (input_filename().empty())
      {
        p = pbes_system::txt2pbes(std::cin);
      }
      else
      {
        std::ifstream from(input_filename().c_str());
        p = pbes_system::txt2pbes(from);
      }
      bes::boolean_equation_system<> b = pbes_system::pbesinstconversion(p);
      b.save(output_filename());
      return true;
    }
};

class txt2pbes_gui_tool: public mcrl2_gui_tool<txt2pbes_tool>
{
  public:
    txt2pbes_gui_tool() {}
};


int main(int argc, char** argv)
{
  return txt2pbes_gui_tool().execute(argc, argv);
}
