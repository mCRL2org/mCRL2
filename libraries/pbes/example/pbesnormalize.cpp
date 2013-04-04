// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesnormalize.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/normalize.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class pbes_normalize_tool: public input_output_tool
{
  public:
    pbes_normalize_tool()
      : input_output_tool(
        "pbesnormalize",
        "Wieger Wesselink",
        "normalize a PBES",
        "Reads a file containing a PBES, and eliminates negations and implications. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run()
    {
      mCRL2log(log::verbose) << "pbesnormalize parameters:" << std::endl;
      mCRL2log(log::verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(log::verbose) << "  output file:        " << m_output_filename << std::endl;

      // load the pbes
      pbes_system::pbes p;
      p.load(input_filename());

      // apply the algorithm
      normalize(p);

      // save the result
      p.save(output_filename());

      return true;
    }
};

int main(int argc, char** argv)
{
  pbes_normalize_tool tool;
  return tool.execute(argc, argv);
}

