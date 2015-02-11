// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bestranslate.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/io.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::pbes_system;
using namespace mcrl2::bes::tools;
using namespace mcrl2::bes;
using namespace mcrl2;

class bestranslate_tool: public bes_input_tool<bes_output_tool<input_output_tool> >
{
  private:
    typedef bes_input_tool<bes_output_tool<input_output_tool> > super;

  public:
    bestranslate_tool()
      : super("bestranslate", "Jeroen Keiren",
              "translate a BES between various formats",
              "Translate BES in INFILE to OUTFILE converting between the formats"
              "specified in the filename. If OUTFILE is not present, stdout is"
              "used. If INFILE is not present, stdin is used."
             )
    {}

    bool run()
    {
      boolean_equation_system bes;
      load_bes(bes, input_filename(), bes_input_format());
      save_bes(bes, output_filename(), bes_output_format());
      return true;
    }
};


int main(int argc, char* argv[])
{
  return bestranslate_tool().execute(argc, argv);
}

