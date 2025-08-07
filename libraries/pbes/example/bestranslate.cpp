// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bestranslate.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::tools;
using namespace mcrl2;

class bestranslate_tool: public pbes_input_tool<pbes_output_tool<input_output_tool> >
{
  private:
    using super = pbes_input_tool<pbes_output_tool<input_output_tool>>;

  public:
    bestranslate_tool()
      : super("bestranslate", "Jeroen Keiren",
              "translate a BES between various formats",
              "Translate BES in INFILE to OUTFILE converting between the formats"
              "specified in the filename. If OUTFILE is not present, stdout is"
              "used. If INFILE is not present, stdin is used."
             )
    {}

    bool run() override
    {
      pbes bes;
      load_pbes(bes, input_filename(), pbes_input_format());
      save_pbes(bes, output_filename(), pbes_output_format());
      return true;
    }
};


int main(int argc, char* argv[])
{
  return bestranslate_tool().execute(argc, argv);
}

