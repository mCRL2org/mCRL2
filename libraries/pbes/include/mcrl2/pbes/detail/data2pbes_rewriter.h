// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/data2pbes_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_DATA2PBES_REWRITER_H
#define MCRL2_PBES_DETAIL_DATA2PBES_REWRITER_H

#include "mcrl2/pbes/builder.h"

namespace mcrl2 {

namespace pbes_system {

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
  typedef pbes_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  bool is_not(const data::data_expression& x) const
  {
    return data::sort_bool::is_not_application(x);
  }

  bool is_and(const data::data_expression& x) const
  {
    return data::sort_bool::is_and_application(x);
  }

  bool is_or(const data::data_expression& x) const
  {
    return data::sort_bool::is_or_application(x);
  }

  bool is_imp(const data::data_expression& x) const
  {
    return data::sort_bool::is_implies_application(x);
  }

  bool is_forall(const data::data_expression& x) const
  {
    return data::is_forall(x);
  }

  bool is_exists(const data::data_expression& x) const
  {
    return data::is_exists(x);
  }

  data::data_expression arg0(const data::data_expression& x) const
  {
    return data::application(x).argument(0);
  }

  data::data_expression left(const data::data_expression& x) const
  {
    return data::application(x).left();
  }

  data::data_expression right(const data::data_expression& x) const
  {
    return data::application(x).right();
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
      return not_(data2pbes(arg0(x)));
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

  pbes_expression operator()(const pbes_expression& x)
  {
    if (data::is_data_expression(x))
    {
      return data2pbes(data::data_expression(x));
    }
    else
    {
      return super::operator()(x);
    }
  }
};

template <typename T>
T data2pbes(const T& x,
            typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
           )
{
  return core::make_apply_builder<data2pbes_builder>()(x);
}

template <typename T>
void data2pbes(T& x,
               typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
              )
{
  core::make_apply_builder<data2pbes_builder>()(x);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_DATA2PBES_REWRITER_H
