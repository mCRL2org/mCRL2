// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesparelm.cpp

#include "boost.hpp" // precompiled headers

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <string>
#include "mcrl2/utilities/filter_tool.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parelm.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;

class pbes_parelm_tool: public utilities::filter_tool
{
  protected:
    void add_options(interface_description& clinterface)
    {
      clinterface.add_option("compute-conditions", "compute propagation conditions", 'c');
    }

  public:
    pbes_parelm_tool()
      : filter_tool(
          "pbesparelm",
          "Wieger Wesselink",
          "Reads a file containing a pbes, and applies parameter elimination to it. If OUTFILE "
          "is not present, standard output is used. If INFILE is not present, standard input is used."
        )
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::clog << "pbesparelm parameters:" << std::endl;
        std::clog << "  input file:         " << m_input_filename << std::endl;
        std::clog << "  output file:        " << m_output_filename << std::endl;
      }
    
      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      pbes_parelm_algorithm algorithm;
      algorithm.run(p);
      
      // save the result
      p.save(m_output_filename);
      
      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pbes_parelm_tool tool;
  return tool.execute(argc, argv);
}
