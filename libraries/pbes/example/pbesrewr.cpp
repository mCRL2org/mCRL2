// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesrewr.cpp

#include "boost.hpp" // precompiled headers

#include <iostream>
#include <string>
#include "mcrl2/utilities/filter_tool_with_rewriter.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesrewr.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;

class pbes_rewr_tool: public utilities::filter_tool_with_rewriter
{
  public:
    pbes_rewr_tool()
      : filter_tool_with_rewriter(
          "pbesrewr",
          "Jan friso Groote, Wieger Wesselink",
          "Rewrite the PBES in INFILE, remove quantified variables and write the resulting PBES to OUTFILE. "
          "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."
        )
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cout << "pbesrewr parameters:" << std::endl;
        std::cout << "  input file:         " << m_input_filename << std::endl;
        std::cout << "  output file:        " << m_output_filename << std::endl;
      }

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      // data rewriter
      data::rewriter datar = create_rewriter(p.data());

      // pbes rewriter
      simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> pbesr(datar);

      // apply the rewriter
      pbesrewr(p, pbesr);

      // save the result
      p.save(m_output_filename);

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pbes_rewr_tool tool;
  return tool.execute(argc, argv);
}
