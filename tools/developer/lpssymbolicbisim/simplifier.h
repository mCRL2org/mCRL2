// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H
#define MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H

#include "mcrl2/data/rewriter.h"

namespace mcrl2
{
namespace data
{

class simplifier
{

protected:
  rewriter rewr;
  rewriter proving_rewr;
  std::map< data_expression, data_expression > cache;

  virtual data_expression simplify_expression(const data_expression& expr) = 0;

public:
  simplifier(rewriter r, rewriter pr)
  : rewr(r)
  , proving_rewr(pr)
  {}

  lambda apply(const lambda& expr, const mutable_indexed_substitution<> sigma)
  {
    data_expression rewritten = rewr(proving_rewr(expr.body(),sigma));

    std::map< data_expression, data_expression >::const_iterator res = cache.find(rewritten);
    if(res != cache.end())
    {
      return lambda(expr.variables(), res->second);
    }
    data_expression simpl(simplify_expression(rewritten));
    cache.insert(std::make_pair(rewritten, simpl));

    return lambda(expr.variables(), simpl);
  }
};


} // namespace mcrl2
} // namespace data

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H