// Author(s): Wieger Wesselink, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes2data_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_PBES2DATA_REWRITER_H
#define MCRL2_PBES_REWRITERS_PBES2DATA_REWRITER_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pbes/builder.h"



namespace mcrl2::pbes_system {

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
  using super = pbes_expression_builder<Derived>;
  using super::apply;

  data::data_expression pbes2data(const pbes_expression& x) const
  {
    if (is_not(x))
    {
      return data::not_(pbes2data(accessors::arg(x)));
    }
    else if (is_and(x))
    {
      return data::and_(pbes2data(accessors::left(x)), pbes2data(accessors::right(x)));
    }
    else if (is_or(x))
    {
      return data::or_(pbes2data(accessors::left(x)), pbes2data(accessors::right(x)));
    }
    else if (is_imp(x))
    {
      return data::imp(pbes2data(accessors::left(x)), pbes2data(accessors::right(x)));
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

  template <class T>
  void apply(T& result, const pbes_expression& x)
  {
    result = pbes2data(x);
  }
};

template <typename T>
data::data_expression pbes2data(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  data::data_expression result;
  core::make_apply_builder<pbes2data_builder>().apply(result, x);
  return result;
}

template <typename T>
void pbes2data(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_apply_builder<pbes2data_builder>().update(x);
}

} // namespace detail

class pbes2data_rewriter
{
  public:
    /// \brief The term type
    using term_type = pbes_expression;

    /// \brief The variable type
    using variable_type = data::variable;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return detail::pbes2data(x);
    }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_PBES2DATA_REWRITER_H
