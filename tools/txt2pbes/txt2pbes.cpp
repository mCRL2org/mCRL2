// Author(s): Aad Mathijssen, Wieger Wesselink
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
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class txt2pbes_tool: public input_output_tool
{
  typedef input_output_tool super;

  public:
    txt2pbes_tool()
      : super(NAME, AUTHOR,
              "parse a textual description of a PBES",
              "Parse the textual description of a PBES from INFILE and write it to OUTFILE. "
              "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.\n\n"
              "The textual description should adhere to the BNF syntax description at "
              "<http://www.mcrl2.org/mcrl2/wiki/index.php/Language_reference/PBES_syntax>."
             )
    {}

    bool run()
    {
      pbes_system::pbes<> p;
      if (input_filename().empty())
      {
        //parse specification from stdin
        mCRL2log(verbose) << "reading input from stdin..." << std::endl;
        p = pbes_system::txt2pbes(std::cin);
      }
      else
      {
        //parse specification from input filename
        mCRL2log(verbose) << "reading input from file '" <<  input_filename() << "'..." << std::endl;
        std::ifstream instream(input_filename().c_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream.is_open())
        {
          throw mcrl2::runtime_error("cannot open input file: " + input_filename());
        }
        p = pbes_system::txt2pbes(instream);
        instream.close();
      }
      if (output_filename().empty())
      {
        mCRL2log(verbose) << "writing PBES to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(verbose) << "writing PBES to file '" <<  output_filename() << "'..." << std::endl;
      }
      p.save(output_filename());
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
  MCRL2_ATERMPP_INIT(argc, argv)
  return txt2pbes_gui_tool().execute(argc, argv);
}
