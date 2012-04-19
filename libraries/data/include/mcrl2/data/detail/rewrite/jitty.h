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
#include "mcrl2/atermpp/map.h"

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
    typedef Rewriter::internal_substitution_type internal_substitution_type;

    RewriterJitty(const data_specification& DataSpec, const used_data_equation_selector &);
    virtual ~RewriterJitty();

    rewrite_strategy getStrategy();

    data_expression rewrite(const data_expression &term, substitution_type &sigma);

    atermpp::aterm_appl toRewriteFormat(const data_expression &term);
    atermpp::aterm_appl rewrite_internal(const atermpp::aterm_appl &term, internal_substitution_type &sigma);

    bool addRewriteRule(const data_equation &Rule);
    bool removeRewriteRule(const data_equation &Rule);

  private:
    // unsigned int num_opids;
    size_t max_vars;
    bool need_rebuild;

    atermpp::map< atermpp::aterm_int, data_equation_list > jitty_eqns;
    atermpp::vector < ATermList >  jitty_strat;
    atermpp::aterm_appl rewrite_aux(const atermpp::aterm_appl &term, internal_substitution_type &sigma);
    void build_strategies();

    atermpp::aterm_appl rewrite_aux_function_symbol(
                      const atermpp::aterm_int &op,
                      const atermpp::aterm_appl &term,
                      internal_substitution_type &sigma);

    /* Auxiliary function to take care that the array jitty_strat is sufficiently large
       to access element i */
    void make_jitty_strat_sufficiently_larger(const size_t i);

};
}
}
}

#endif
