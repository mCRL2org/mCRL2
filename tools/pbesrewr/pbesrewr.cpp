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
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesrewr.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;
using utilities::pbes_rewriter_tool;

class pbes_rewr_tool: public pbes_rewriter_tool
{
  public:
    pbes_rewr_tool()
      : pbes_rewriter_tool(
          "pbesrewr",
          "Jan friso Groote, Wieger Wesselink",
          "Rewrite the PBES in INFILE, remove quantified variables and write the resulting PBES to OUTFILE. "
          "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."
        )
    {}

    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;
      
      if (core::gsVerbose)
      {
        std::cout << "pbesrewr parameters:" << std::endl;
        std::cout << "  input file:         " << m_input_filename << std::endl;
        std::cout << "  output file:        " << m_output_filename << std::endl;
        std::cout << "  pbes rewriter:      " << m_pbes_rewriter_type << std::endl;
      }
    
      // load the pbes
      pbes<> p;
      p.load(m_input_filename);
      
      // data rewriter
      data::rewriter datar = create_rewriter(p.data());

      // pbes rewriter
      switch (rewriter_type())
      {
        case simplify:
        {
          simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);    
          pbesrewr(p, pbesr);
          break;
        }
        case quantifier_all:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = true;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
          pbesrewr(p, pbesr);
          break;
        }
        case quantifier_finite:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = false;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
          pbesrewr(p, pbesr);
          break;
        }
      }
      
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
