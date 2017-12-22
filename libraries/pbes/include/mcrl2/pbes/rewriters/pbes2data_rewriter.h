// Author(s): Wieger Wesselink, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes2data_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_PBES2DATA_REWRITER_H
#define MCRL2_PBES_REWRITERS_PBES2DATA_REWRITER_H

#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// transforms outer level data operators to their pbes equivalents, for the following operators:
// !x
// x && y
// x || y
// x => y
// exists d:D. x
// forall d:D. x
template <typename Derived>
struct pbes2data_builder: public pbes_expression_builder<Derived>
{
  typedef pbes_expression_builder<Derived> super;
  using super::apply;

  data::data_expression pbes2data(const pbes_expression& x) const
  {
    if (is_not(x))
    {
      return data::sort_bool::not_(pbes2data(accessors::arg(x)));
    }
    else if (is_and(x))
    {
      return data::sort_bool::and_(pbes2data(accessors::left(x)), pbes2data(accessors::right(x)));
    }
    else if (is_or(x))
    {
      return data::sort_bool::or_(pbes2data(accessors::left(x)), pbes2data(accessors::right(x)));
    }
    else if (is_imp(x))
    {
      return data::sort_bool::implies(pbes2data(accessors::left(x)), pbes2data(accessors::right(x)));
    }
    else if (is_forall(x))
    {
      forall y(x);
      return data::forall(y.variables(), pbes2data(y.body()));
    }
    else if (is_exists(x))
    {
      exists y(x);
      return data::exists(y.variables(), pbes2data(y.body()));
    }
    else if(data::is_data_expression(x))
    {
      return atermpp::down_cast<data::data_expression>(x);
    }
    else
    {
      throw mcrl2::runtime_error("PBES expression " + pp(x) + " cannot be rewritten to a data expression.");
    }
  }

  data::data_expression apply(const pbes_expression& x)
  {
    return pbes2data(x);
  }
};

template <typename T>
T pbes2data(const T& x,
            typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
           )
{
  return core::make_apply_builder<pbes2data_builder>().apply(x);
}

template <typename T>
void pbes2data(T& x,
               typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0
              )
{
  core::make_apply_builder<pbes2data_builder>().update(x);
}

} // namespace detail

class pbes2data_rewriter
{
  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return detail::pbes2data(x);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_PBES2DATA_REWRITER_H
