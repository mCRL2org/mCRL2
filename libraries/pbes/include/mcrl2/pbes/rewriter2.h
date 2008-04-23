// Author(s): Jan Friso Groote, Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter2.h
/// \brief Pbes expression rewriters.

#ifndef MCRL2_PBES_REWRITER2_H
#define MCRL2_PBES_REWRITER2_H

#include <boost/shared_ptr.hpp>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/gauss.h"

namespace mcrl2 {

namespace pbes_system {

class simplify_rewriter_jfg
{
  data::rewriter datar;
  
  public:
    simplify_rewriter_jfg(const data::data_specification& data)
      : datar(data)
    { }
    
    pbes_expression operator()(pbes_expression p)
    {
      return pbes_expression_rewrite_and_simplify(p, datar.get_rewriter());
    }
};

class substitute_rewriter_jfg
{
  data::rewriter& datar_;
  const data::data_specification& data_spec;
 
  public:
    substitute_rewriter_jfg(data::rewriter& datar, const data::data_specification& data)
      : datar_(datar), data_spec(data)
    { }
    
    pbes_expression operator()(pbes_expression p)
    {
      return pbes_expression_substitute_and_rewrite(p, data_spec, datar_.get_rewriter(), false);
    }   
};

class pbessolve_rewriter
{
  data::rewriter& datar_;
  const data::data_specification& data_spec;
  int n;
  data_variable_list fv;
  boost::shared_ptr<BDD_Prover> prover;
 
  public:
    pbessolve_rewriter(data::rewriter& datar, const data::data_specification& data, RewriteStrategy rewrite_strategy, SMT_Solver_Type solver_type)
      : datar_(datar),
        data_spec(data),
        n(0),
        prover(new BDD_Prover(data_spec, rewrite_strategy, 0, false, solver_type, false))
    { }

    pbes_expression operator()(pbes_expression p)
    {
      return pbes_expression_simplify(p, &n, &fv, prover.get());
    }   
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_JFG_REWRITER_H
