// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find_quantifier_variables.h
/// \brief 


#ifndef MCRL2_DATA_FIND_QUANTIFIER_VARIABLES_H
#define MCRL2_DATA_FIND_QUANTIFIER_VARIABLES_H

#include "mcrl2/data/traverser.h"

namespace mcrl2::data {

/// Visitor for collecting the quantifier variables that occur in a pbes expression.
struct find_quantifier_variables_traverser: public data_expression_traverser<find_quantifier_variables_traverser>
{
  using super = data_expression_traverser<find_quantifier_variables_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<data::variable> result;

  void enter(const forall& x)
  {
    auto const& v = x.variables();
    result.insert(v.begin(), v.end());
  }

  void enter(const exists& x)
  {
    auto const& v = x.variables();
    result.insert(v.begin(), v.end());
  }
};


inline
std::set<variable> find_quantifier_variables(const data_expression& x)
{
  find_quantifier_variables_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_FIND_QUANTIFIER_VARIABLES_H