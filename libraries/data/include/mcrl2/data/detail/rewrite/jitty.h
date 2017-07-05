// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jitty.h

#ifndef __REWR_JITTY_H
#define __REWR_JITTY_H

#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/rewrite/strategy_rule.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

class RewriterJitty: public Rewriter
{
  public:
    typedef Rewriter::substitution_type substitution_type;

    RewriterJitty(const data_specification& data_spec, const used_data_equation_selector &);
    virtual ~RewriterJitty();

    rewrite_strategy getStrategy();

    data_expression rewrite(const data_expression &term, substitution_type &sigma);

  private:
    std::size_t max_vars;

    std::map< function_symbol, data_equation_list > jitty_eqns;
    std::vector<strategy> jitty_strat;
    std::size_t MAX_LEN; 
    data_expression rewrite_aux(const data_expression& term, substitution_type& sigma);
    void build_strategies();

    data_expression rewrite_aux_function_symbol(
                      const function_symbol& op,
                      const data_expression& term,
                      substitution_type& sigma);

    /* Auxiliary function to take care that the array jitty_strat is sufficiently large
       to access element i */
    void make_jitty_strat_sufficiently_larger(const std::size_t i);
    strategy create_strategy(const data_equation_list& rules1);
    void rebuild_strategy();
};

/// \brief removes auxiliary expressions this_term_is_in_normal_form from data_expressions that are being rewritten.
/// \detail The function below is intended to remove the auxiliary function this_term_is_in_normal_form from a term
///         such that it can for instance be pretty printed. This auxiliary function is used internally in terms
///         when rewriting to avoid to rewrite too often.
data_expression remove_normal_form_function(const data_expression& t);
}
}
}

#endif
