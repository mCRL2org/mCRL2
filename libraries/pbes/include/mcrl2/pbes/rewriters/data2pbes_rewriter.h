// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/data2pbes_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_DATA2PBES_REWRITER_H
#define MCRL2_PBES_REWRITERS_DATA2PBES_REWRITER_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pbes/builder.h"



namespace mcrl2::pbes_system {

namespace detail {

// transforms outer level data operators to their pbes equivalents, for the following operators:
// x && y
// x || y
// x => y
// exists d:D. x
// forall d:D. x
template <typename Derived>
struct data2pbes_builder: public pbes_expression_builder<Derived>
{
  using super = pbes_expression_builder<Derived>;
  using super::apply;

  bool is_not(const data::data_expression& x) const
  {
    return data::is_not(x);
  }

  bool is_and(const data::data_expression& x) const
  {
    return data::is_and(x);
  }

  bool is_or(const data::data_expression& x) const
  {
    return data::is_or(x);
  }

  bool is_imp(const data::data_expression& x) const
  {
    return data::is_imp(x);
  }

  bool is_forall(const data::data_expression& x) const
  {
    return data::is_forall(x);
  }

  bool is_exists(const data::data_expression& x) const
  {
    return data::is_exists(x);
  }

  data::data_expression operand(const data::data_expression& x) const
  {
    return data::unary_operand(atermpp::down_cast<data::application>(x));
  }

  data::data_expression left(const data::data_expression& x) const
  {
    return data::binary_left(atermpp::down_cast<data::application>(x));
  }

  data::data_expression right(const data::data_expression& x) const
  {
    return data::binary_right(atermpp::down_cast<data::application>(x));
  }

  // transforms outer level data operators to their pbes equivalents, for the following operators:
  // !x
  // x && y
  // x || y
  // x => y
  // exists d:D. x
  // forall d:D. x
  pbes_expression data2pbes(const data::data_expression& x) const
  {
    if (is_not(x))
    {
      return not_(data2pbes(operand(x)));
    }
    else if (is_and(x))
    {
      return and_(data2pbes(left(x)), data2pbes(right(x)));
    }
    else if (is_or(x))
    {
      return or_(data2pbes(left(x)), data2pbes(right(x)));
    }
    else if (is_imp(x))
    {
      return imp(data2pbes(left(x)), data2pbes(right(x)));
    }
    else if (is_forall(x))
    {
      data::forall y(x);
      return forall(y.variables(), data2pbes(y.body()));
    }
    else if (is_exists(x))
    {
      data::exists y(x);
      return exists(y.variables(), data2pbes(y.body()));
    }
    return x;
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = data2pbes(x);
  }
};

template <typename T>
T data2pbes(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_apply_builder<data2pbes_builder>().apply(result, x);
  return result;
}

template <typename T>
void data2pbes(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_apply_builder<data2pbes_builder>().update(x);
}

} // namespace detail

/// \brief A rewriter that applies one point rule quantifier elimination to a PBES.
class data2pbes_rewriter
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
      return detail::data2pbes(x);
    }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_DATA2PBES_REWRITER_H
