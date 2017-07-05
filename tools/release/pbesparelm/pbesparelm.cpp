// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesparelm.cpp

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using bes::tools::pbes_input_tool;
using bes::tools::pbes_output_tool;

//[pbes_parelm_tool
class pbes_parelm_tool: public pbes_input_tool<pbes_output_tool<input_output_tool> >
{
    typedef pbes_input_tool<pbes_output_tool<input_output_tool> > super;

  public:
    pbes_parelm_tool()
      : super(
        "pbesparelm",
        "Wieger Wesselink; Simon Janssen and Tim Willemse",
        "remove unused parameters from a PBES",
        "Reads a file containing a PBES, and applies parameter elimination to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() /*< The virtual function `run` executes the tool.
                   The user has to override this function to add behavior. >*/
    {
      mCRL2log(verbose) << "pbesparelm parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      pbesparelm(input_filename(),
                 output_filename(),
                 pbes_input_format(),
                 pbes_output_format()
                );

      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbes_parelm_tool().execute(argc, argv); /*< The function `execute` first parses the command line
                                       arguments, and then calls the function `run`. >*/
}
//]
