// Author(s): Jan Friso Groote. Based on pbes/tools/pbesrewr by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/tools/presrewr.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TOOLS_PRESREWR_H
#define MCRL2_PRES_TOOLS_PRESREWR_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/one_point_rule_preprocessor.h"
// #include "mcrl2/pbes/detail/bqnf_traverser.h"
// #include "mcrl2/pbes/detail/ppg_rewriter.h"
// #include "mcrl2/pbes/detail/ppg_traverser.h"
#include "mcrl2/pres/io.h"
#include "mcrl2/pres/normalize.h"
#include "mcrl2/pres/pres_rewriter_type.h"
#include "mcrl2/pres/rewriters/simplify_rewriter.h"
#include "mcrl2/pres/rewrite.h"
#include "mcrl2/pres/rewriter.h"
// #include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
// #include "mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h"

namespace mcrl2 {

namespace pres_system {

void presrewr(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format& input_format,
              const utilities::file_format& output_format,
              const data::rewrite_strategy rewrite_strategy,
              pres_rewriter_type rewriter_type)
{
  // load the pres
  pres p;
  load_pres(p, input_filename, input_format);

  // data rewriter
  data::rewriter datar(p.data(), rewrite_strategy);

  // pres rewriter
  switch (rewriter_type)
  {
    case simplify:
    {
      // simplify_quantifiers_data_rewriter<data::rewriter> presr(datar);
      simplify_data_rewriter<data::rewriter> presr(datar);
      pres_rewrite(p, presr);
      break;
    }
    /* case quantifier_all:
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
      simplify_data_rewriter<data::rewriter> simp(datar);
      pres_rewrite(p, simp);
      break;
    }
    case pfnf:
    {
      pfnf_rewriter presr;
      pres_system::normalize(p);
      pres_rewrite(p, presr);
      break;
    }
    case ppg:
    {
      //bool bqnf = detail::is_bqnf(p);
      //std::clog << "bqnf_traverser says: p is " << (bqnf ? "" : "NOT ") << "in BQNF." << std::endl;
      bool ppg = detail::is_ppg(p);
      if (ppg)
      {
        mCRL2log(log::verbose) << "PRES is already a PPG." << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "Rewriting..." << std::endl;
        pres q = detail::to_ppg(p);
        mCRL2log(log::verbose) << "Rewriting done." << std::endl;
        ppg = detail::is_ppg(q);
        if (!ppg)
        {
          throw(std::runtime_error("The result PRES if not a PPG!"));
        }
        p = q;
      }
      break;
    }
    case bqnf_quantifier:
    {
      bqnf_rewriter presr;
      pres_rewrite(p, presr);
      break;
    }
    case prover:
    default:
    {
      // Just ignore.
      assert(false);  // The PRES rewriter cannot be activated through
      // the commandline. So, we cannot end up here.
      break;
    } */
  }
  save_pres(p, output_filename, output_format);
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TOOLS_PRESREWR_H
