// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./txt2pbes.cpp
/// \brief Parse a textual description of a PBES.

#include "boost.hpp" // precompiled headers

#define NAME "txt2pbes"
#define AUTHOR "Aad Mathijssen"

//C++
#include <cstdio>
#include <fstream>
#include <string>

//mCRL2 specific
#include "mcrl2/core/text_utility.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class txt2pbes_tool: public input_output_tool
{
  public:
    txt2pbes_tool()
      : input_output_tool(NAME, AUTHOR,
          "parse a textual description of a PBES",
          "Parse the textual description of a PBES from INFILE and write it to OUTFILE. "
          "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.\n\n"
          "The textual description should adhere to the BNF syntax description at "
          "<http://www.mcrl2.org/mcrl2/wiki/index.php/Language_reference/PBES_syntax>."
        )
    {}

    bool run()
    {
      std::string text = core::read_text(input_filename());
      pbes_system::pbes<> p = pbes_system::txt2pbes(text);
      p.save(output_filename());
      return true;
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return txt2pbes_tool().execute(argc, argv);
}
