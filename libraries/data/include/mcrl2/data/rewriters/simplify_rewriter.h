// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriters/simplify_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_SIMPLIFY_REWRITE_BUILDER_H
#define MCRL2_DATA_REWRITERS_SIMPLIFY_REWRITE_BUILDER_H

#include "mcrl2/data/builder.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/detail/data_sequence_algorithm.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2 {

namespace data {

namespace detail {

template <typename Derived>
class simplify_rewrite_builder: public data_expression_builder<Derived>
{
  public:
    typedef data_expression_builder<Derived> super;

    using super::enter;
    using super::leave;
    using super::operator();

    Derived& derived()
    {
      return static_cast<Derived&>(*this);
    }

    variable_list free_variables(const data_expression& x) const
    {
      std::set<variable> v = data::find_free_variables(x);
      return variable_list(v.begin(), v.end());
    }

    bool is_not(const data_expression& x) const
    {
      return data::sort_bool::is_not_application(x);
    }

    bool is_and(const data_expression& x) const
    {
      return data::sort_bool::is_and_application(x);
    }

    bool is_or(const data_expression& x) const
    {
      return data::sort_bool::is_or_application(x);
    }

    bool is_imp(const data_expression& x) const
    {
      return data::sort_bool::is_implies_application(x);
    }

    bool is_forall(const data_expression& x) const
    {
      return data::is_forall(x);
    }

    bool is_exists(const data_expression& x) const
    {
      return data::is_exists(x);
    }

    data_expression operator()(const application& x)
    {
      data_expression result;
      derived().enter(x);
      if (is_not(x)) // x = !y
      {
        data_expression y = derived()(*x.begin());
        result = utilities::optimized_not(y);
      }
      else if (is_and(x)) // x = y && z
      {
        data_expression y = derived()(binary_left(x));
        data_expression z = derived()(binary_right(x));
        result = utilities::optimized_and(y, z);
      }
      else if (is_or(x)) // x = y || z
      {
        data_expression y = derived()(binary_left(x));
        data_expression z = derived()(binary_right(x));
        result = utilities::optimized_or(y, z);
      }
      else if (is_imp(x)) // x = y => z
      {
        data_expression y = derived()(binary_left(x));
        data_expression z = derived()(binary_right(x));
        result = utilities::optimized_imp(y, z);
      }
      else
      {
        result = super::operator()(x);
      }
      derived().leave(x);
      return result;
    }

    data_expression operator()(const forall& x) // x = forall d. y
    {
      variable_list d = forall(x).variables();
      data_expression y = derived()(forall(x).body());
      return utilities::optimized_forall(d, y, true);
    }

    data_expression operator()(const exists& x) // x = exists d. y
    {
      variable_list d = exists(x).variables();
      data_expression y = derived()(exists(x).body());
      return utilities::optimized_exists(d, y, true);
    }
};

} // namespace detail

struct simplify_rewriter: public std::unary_function<data_expression, data_expression>
{
  data_expression operator()(const data_expression& x) const
  {
    return core::make_apply_builder<detail::simplify_rewrite_builder>()(x);
  }
};

template <typename T>
void simplify(T& x, typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  core::make_update_apply_builder<data::data_expression_builder>(simplify_rewriter())(x);
}

template <typename T>
T simplify(const T& x, typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  T result = core::make_update_apply_builder<data::data_expression_builder>(simplify_rewriter())(x);
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITERS_SIMPLIFY_REWRITE_BUILDER_H

