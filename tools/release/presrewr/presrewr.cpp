// Author(s): Jan Friso Groote, Wieger Wesselink. Based on pbesrewr.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presrewr.cpp

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pres/io.h"
#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/pres/pres_output_tool.h"
#include "mcrl2/pres/pres_rewriter_tool.h"
#include "mcrl2/pres/pres_rewriter_type.h"
#include "mcrl2/pres/rewrite.h"
#include "mcrl2/pres/rewriters/dataspec_prune_rewriter.h"
#include "mcrl2/pres/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pres/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pres/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pres/rewriters/simplify_rewriter.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using pres_system::tools::pres_input_tool;
using pres_system::tools::pres_output_tool;
using pres_system::tools::pres_rewriter_tool;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

class pres_rewriter : public pres_input_tool<pres_output_tool<pres_rewriter_tool<rewriter_tool<input_output_tool> > > >
{
  protected:
    using super = pres_input_tool<pres_output_tool<pres_rewriter_tool<rewriter_tool<input_output_tool>>>>;

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

      // load the pres
      pres p;
      load_pres(p, input_filename(), pres_input_format());

      // data rewriter
      data::rewriter datar(p.data(), rewrite_strategy());

      // pres rewriter
      switch (rewriter_type())
      {
        case simplify:
        {
          // simplify_quantifiers_data_rewriter<data::rewriter> presr(datar);
          simplify_data_rewriter<data::rewriter> presr(p.data(), datar);
          pres_rewrite(p, presr);
          break;
        }
        case quantifier_all:
        {
          bool enumerate_infinite_sorts = true;
          enumerate_quantifiers_rewriter presr(datar, p.data(), enumerate_infinite_sorts);
          pres_rewrite(p, presr);
          break;
        }
        case quantifier_finite:
        {
          bool enumerate_infinite_sorts = false;
          enumerate_quantifiers_rewriter presr(datar, p.data(), enumerate_infinite_sorts);
          pres_rewrite(p, presr);
          break;
        }
        case quantifier_inside:
        {
          quantifiers_inside_rewriter presr;
          pres_rewrite(p, presr);
          break;
        }
        case quantifier_one_point:
        {
          // apply the one point rule rewriter
          one_point_rule_rewriter presr;
          bool innermost = false;
          replace_pres_expressions(p, presr, innermost); // use replace, since the one point rule rewriter does the recursion itself

          // post processing: apply the simplifying rewriter
          simplify_data_rewriter<data::rewriter> simp(p.data(), datar);
          pres_rewrite(p, simp);
          break;
        }
        case pres_rewriter_type::prune_dataspec:
        {
          dataspec_prune_rewriter rewr;
          p = rewr(p);
          break;
        }
      }
      save_pres(p, output_filename(), m_pres_output_format);
      return true;
    }

};

int main(int argc, char* argv[])
{
  return pres_rewriter().execute(argc, argv);
}
