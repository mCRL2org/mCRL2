// Author(s): XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/simplify_rewriter.h
/// \brief rewriting propositional variables

#ifndef MCRL2_PBES_REWRITERS_PROPOSITIONAL_VARIABLE_REWRITER_H
#define MCRL2_PBES_REWRITERS_PROPOSITIONAL_VARIABLE_REWRITER_H

#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <template <class> class Builder, class Derived, class Map, class Justification>
struct add_propositional_variable_rewriter: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  typedef core::term_traits<pbes_expression> tr;

  const Map &map;
  Justification &justification;

  add_propositional_variable_rewriter(const Map& m, Justification &j)
    : map(m), justification(j)
  {}

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    auto expr = map.find(x);
    if (expr == map.end())
    {
      return x;
    }
    else
    {
      justification.push_back(x);
      return expr->second;
    }
  }
};

template <typename Map, typename Justification>
struct propositional_variable_rewriter_builder: public add_propositional_variable_rewriter<simplify_builder, propositional_variable_rewriter_builder<Map, Justification>, Map, Justification>
{
  typedef add_propositional_variable_rewriter<simplify_builder, propositional_variable_rewriter_builder<Map, Justification>, Map, Justification> super;
  using super::super;
  propositional_variable_rewriter_builder(const Map& m, Justification &j)
    : super(m, j)
  {}
};

} // namespace detail

template <typename Map, typename Justification>
struct propositional_variable_rewriter
{
  const Map& map;
  Justification &justification;

  propositional_variable_rewriter(const Map& m, Justification &j)
    : map(m), justification(j)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    return detail::propositional_variable_rewriter_builder<Map, Justification>(map, justification).apply(x);
  }
};

template <typename Map, typename Justification>
propositional_variable_rewriter<Map, Justification>
make_propositional_variable_rewriter(const Map &m, Justification &j)
{
  return propositional_variable_rewriter<Map, Justification>(m, j);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_PROPOSITIONAL_VARIABLE_REWRITER_H
