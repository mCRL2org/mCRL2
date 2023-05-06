// Author(s): Jan Friso Groote, Wieger Wesselink. Based on pbesrewr.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presrewr.cpp

#include "mcrl2/pres/tools.h"
#include "mcrl2/pres/tools/presrewr.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/res/pres_rewriter_tool.h"
#include "mcrl2/res/pres_input_tool.h"
#include "mcrl2/res/pres_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using res::tools::pres_input_tool;
using res::tools::pres_output_tool;
using res::tools::pres_rewriter_tool;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

class pres_rewriter : public pres_input_tool<pres_output_tool<pres_rewriter_tool<rewriter_tool<input_output_tool> > > >
{
  protected:
    typedef pres_input_tool<pres_output_tool<pres_rewriter_tool<rewriter_tool<input_output_tool> > > > super;

    /// \brief Returns the types of rewriters that are available for this tool.
    std::set<pres_system::pres_rewriter_type> available_rewriters() const override
    {
      std::set<pres_system::pres_rewriter_type> result = super::available_rewriters();
      // result.insert(pres_system::quantifier_one_point);
      return result;
    }

  public:
    pres_rewriter()
      : super(
        "presrewr",
        "Jan Friso Groote",
        "rewrite and simplify a PRES",
        "Rewrite the PRES in INFILE, remove quantified variables and write the resulting PRES to OUTFILE. "
        "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."
      )
    {}

    bool run() override
    {
      using namespace pres_system;
      using namespace utilities;

      mCRL2log(verbose) << "presrewr parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  pres rewriter:      " << m_pres_rewriter_type << std::endl;

      pres_system::presrewr(input_filename(),
                            output_filename(),
                            pres_input_format(),
                            pres_output_format(),
                            rewrite_strategy(),
                            rewriter_type()
                           );
      return true;
    }

};

int main(int argc, char* argv[])
{
  return pres_rewriter().execute(argc, argv);
}
