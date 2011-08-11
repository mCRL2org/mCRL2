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
    RewriterJitty(const data_specification& DataSpec, const used_data_equation_selector &);
    ~RewriterJitty();

    RewriteStrategy getStrategy();

    data_expression rewrite(const data_expression term, mutable_map_substitution<> &sigma);

    atermpp::aterm_appl toRewriteFormat(const data_expression term);
    data_expression fromRewriteFormat(atermpp::aterm_appl term); 
    atermpp::aterm_appl rewrite_internal(const atermpp::aterm_appl term, mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma);

    bool addRewriteRule(const data_equation Rule);
    bool removeRewriteRule(const data_equation Rule);

  private:
    // unsigned int num_opids;
    size_t max_vars;
    bool need_rebuild;

    ATermAppl jitty_true;

    atermpp::map< ATermInt, ATermList > jitty_eqns;
    atermpp::vector < ATermList >  jitty_strat;
    atermpp::aterm_appl rewrite_aux(const atermpp::aterm_appl term, mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma);
    void build_strategies();
};
}
}
}

#endif
