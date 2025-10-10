// Author(s): Jan Friso Groote. Based on pbesparelm by Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presparelm.cpp

// #define MCRL2_PRES_CONSTELM_DEBUG
// #define MCRL2_PRES_EXPRESSION_BUILDER_DEBUG

#include "mcrl2/pres/parelm.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pres/pres_output_tool.h"
#include "mcrl2/pres/pres_input_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pres_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pres_system::tools::pres_input_tool;
using pres_system::tools::pres_output_tool;

//[pres_parelm_tool
class pres_parelm_tool: public pres_input_tool<pres_output_tool<input_output_tool> >
{
  using super = pres_input_tool<pres_output_tool<input_output_tool>>;

public:
  pres_parelm_tool()
      : super("presparelm",
            "Wieger Wesselink; Simon Janssen and Tim Willemse",
            "remove unused parameters from a PRES",
            "Reads a file containing a PRES, and applies parameter elimination to it. If OUTFILE "
            "is not present, standard output is used. If INFILE is not present, standard input is used.")
  {}

  bool run() override /*< The virtual function `run` executes the tool.
                 The user has to override this function to add behavior. >*/
  {
    mCRL2log(verbose) << "presparelm parameters:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
    mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

    // load the pres
    pres p;
    load_pres(p, input_filename(), pres_input_format());

    parelm(p);

    // save the result
    save_pres(p, output_filename(), pres_output_format());

    return true;
    }
};

int main(int argc, char* argv[])
{
  return pres_parelm_tool().execute(argc, argv); /*< The function `execute` first parses the command line
                                       arguments, and then calls the function `run`. >*/
}
//]
