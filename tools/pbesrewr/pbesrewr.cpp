// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesrewr.cpp

#include <iostream>
#include <string>
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/pbes_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using utilities::tools::input_output_tool;
using data::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;

class pbes_rewriter : public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > > >
{
  protected:
    typedef pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > > > super;

    /// \brief Returns the types of rewriters that are available for this tool.
    std::set<pbes_system::pbes_rewriter_type> available_rewriters() const
    {
      std::set<pbes_system::pbes_rewriter_type> result = super::available_rewriters();
      result.insert(pbes_system::quantifier_one_point);
      return result;
    }

  public:
    pbes_rewriter()
      : super(
        "pbesrewr",
        "Jan Friso Groote and Wieger Wesselink",
        "rewrite and simplify a PBES",
        "Rewrite the PBES in INFILE, remove quantified variables and write the resulting PBES to OUTFILE. "
        "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."
      )
    {}

    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;

      mCRL2log(verbose) << "pbesrewr parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  pbes rewriter:      " << m_pbes_rewriter_type << std::endl;

      pbes_system::pbesrewr(input_filename(),
                            output_filename(),
                            pbes_input_format(),
                            pbes_output_format(),
                            rewrite_strategy(),
                            rewriter_type()
                           );
      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbes_rewriter().execute(argc, argv);
}
