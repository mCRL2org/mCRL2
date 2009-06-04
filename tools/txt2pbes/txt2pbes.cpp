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
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes.h"

using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::data::detail;

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
      ATermAppl result = NULL;
      //parse specification
      if (input_filename().empty()) {
        //parse specification from stdin
        gsVerboseMsg("parsing input from stdin...\n");
        result = parse_pbes_spec(std::cin);
      } else {
        //parse specification from input_filename()
        std::ifstream instream(input_filename().c_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream.is_open()) {
          throw mcrl2::runtime_error("cannot open input file '" + input_filename() + "'");
        }
        gsVerboseMsg("parsing input file '%s'...\n", input_filename().c_str());
        result = parse_pbes_spec(instream);
        instream.close();
      }
      if (result == NULL) {
        throw mcrl2::runtime_error("parsing failed");
      }
      //type check the result
      gsVerboseMsg("type checking...\n");
      result = type_check_pbes_spec(result);
      if (result == NULL) {
        throw mcrl2::runtime_error("type checking failed");
      }
      //check if PBES is monotonic
      try {
        gsVerboseMsg("checking monotonicity...\n");
        mcrl2::pbes_system::pbes<> p(result);
        //TODO replace by a more sophisticated check
        p.normalize();
      }
      catch (std::exception&) {
        throw mcrl2::runtime_error("PBES is not monotonic");
      }
      //store the result
      if (output_filename().empty()) {
        gsVerboseMsg("saving result to stdout...\n");
      } else {
        gsVerboseMsg("saving result to '%s'...\n", output_filename().c_str());
      }
      mcrl2::pbes_system::pbes<> p(result);
      p.save(output_filename());
      return true;
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return txt2pbes_tool().execute(argc, argv);
}
