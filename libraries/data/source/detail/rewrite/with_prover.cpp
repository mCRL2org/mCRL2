// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define NAME "rewr_prover"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <memory.h>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/with_prover.h"

using namespace mcrl2::data::detail;
using namespace mcrl2::core;

namespace mcrl2
{
namespace data
{
namespace detail
{

RewriterProver::RewriterProver(const data_specification& DataSpec, mcrl2::data::rewriter::strategy strat, const used_data_equation_selector& equations_selector):
  Rewriter()
{
  prover_obj = new BDD_Prover(DataSpec, equations_selector, strat);
  rewr_obj = prover_obj->get_rewriter();
}

RewriterProver::~RewriterProver()
{
  delete prover_obj;
}

bool RewriterProver::addRewriteRule(const data_equation &Rule)
{
  return rewr_obj->addRewriteRule(Rule);
}

bool RewriterProver::removeRewriteRule(const data_equation &Rule)
{
  return rewr_obj->removeRewriteRule(Rule);
}

data_expression RewriterProver::rewrite(
            const data_expression &Term,
            substitution_type &sigma)
{
  if (mcrl2::data::data_expression(Term).sort() == mcrl2::data::sort_bool::bool_())
  {
    prover_obj->set_substitution(sigma);
    prover_obj->set_formula(Term);
    return prover_obj->get_bdd();
  }
  else
  {
    return rewr_obj->rewrite(Term,sigma);
  }
}

atermpp::aterm_appl RewriterProver::rewrite_internal(
            const atermpp::aterm_appl &Term,
            internal_substitution_type &sigma)
{
  // Code below is not very efficient, due to the translation to and from internal
  // rewrite format. This requires further investigation...
  const data_expression t=rewr_obj->fromRewriteFormat(Term);
  if (t.sort() == mcrl2::data::sort_bool::bool_())
  {
    prover_obj->set_substitution_internal(sigma);
    prover_obj->set_formula(t);
    return rewr_obj->toRewriteFormat(prover_obj->get_bdd());
  }
  else
  {
    return rewr_obj->rewrite_internal(Term,sigma);
  }
}

atermpp::aterm_appl RewriterProver::toRewriteFormat(const data_expression &Term)
{
  return rewr_obj->toRewriteFormat(Term);
}

/* data_expression RewriterProver::fromRewriteFormat(const atermpp::aterm_appl Term)
{
  return rewr_obj->fromRewriteFormat(Term);
} */

rewrite_strategy RewriterProver::getStrategy()
{
  switch (rewr_obj->getStrategy())
  {
    case jitty:
      return jitty_prover;
#ifdef MCRL2_JITTYC_AVAILABLE
    case jitty_compiling:
      return jitty_compiling_prover;
#endif
    default:
      throw mcrl2::runtime_error("invalid rewrite strategy");
  }
}

}
}
}

