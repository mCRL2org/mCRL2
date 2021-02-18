// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriters/if_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_IF_REWRITER_H
#define MCRL2_DATA_REWRITERS_IF_REWRITER_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/standard.h"

namespace mcrl2 {

namespace data {

namespace detail {

// Returns f(x0, ..., x_i-1, y, x_i+1, ..., xn)
inline
application replace_argument(const application& x, std::size_t i, const data_expression& y)
{
  std::size_t j = 0;
  return application(x.head(), x.begin(), x.end(), [&](const data_expression& x_i) { return (j++ == i+1) ? y : x_i; });
}

inline
data_expression push_if_outside(const application& x)
{
  for (std::size_t i = 0; i < x.size(); i++)
  {
    if (is_if_application(x[i]))
    {
      const auto& x_i = atermpp::down_cast<application>(x[i]);
      const data_expression& b = x_i[0];
      const data_expression& t1 = x_i[1];
      const data_expression& t2 = x_i[2];
      return if_(b, push_if_outside(replace_argument(x, i, t1)), push_if_outside(replace_argument(x, i, t2)));
    }
  }
  return x;
}

template <typename Derived>
struct if_rewrite_builder: public data_expression_builder<Derived>
{
  typedef data_expression_builder<Derived> super;

  using super::apply;

  bool is_simple(const data_expression& x) const
  {
    return !is_and(x) && !is_or(x) && !is_imp(x) && !is_not(x) && !is_true(x) && !is_false(x);
  }

  data_expression apply_if(const data_expression& b, const data_expression& t1, const data_expression& t2)
  {
    if (is_true(b))
    {
      return t1;
    }
    else if (is_false(b))
    {
      return t2;
    }
    else if (is_and(b))
    {
      const data_expression& b1 = binary_left1(b);
      const data_expression& b2 = binary_right1(b);
      return if_(b1, if_(b2, t1, t2), t2);
    }
    else if (is_or(b))
    {
      const data_expression& b1 = binary_left1(b);
      const data_expression& b2 = binary_right1(b);
      return if_(b1, t1, if_(b2, t1, t2));
    }
    else if (is_imp(b))
    {
      const data_expression& b1 = binary_left1(b);
      const data_expression& b2 = binary_right1(b);
      return if_(b1, if_(b2, t1, t2), t1);
    }
    else if (is_not(b))
    {
      const data_expression& b1 = unary_operand1(b);
      return if_(b1, t2, t1);
    }
    else
    {
      assert(is_simple(b));
      if (t1 == t2)
      {
        return t1;
      }
      else if (is_if_application(t1))
      {
        const application& t1_ = atermpp::down_cast<application>(t1);
        const data_expression& c = t1_[0];
        const data_expression& u1 = t1_[1];
        const data_expression& u2 = t1_[2];
        if (b == c)
        {
          return apply_if(b, u1, t2);
        }
        else if (b > c) // use the aterm pointer comparison
        {
          assert(is_simple(c));
          return apply_if(c, apply_if(b, u1, t2), apply_if(b, u2, t2));
        }
        else
        {
          return if_(b, t1, t2);
        }
      }
      else if (is_if_application(t2))
      {
        const application& t2_ = atermpp::down_cast<application>(t2);
        const data_expression& c = t2_[0];
        const data_expression& u1 = t2_[1];
        const data_expression& u2 = t2_[2];
        if (b == c)
        {
          return apply_if(b, t1, u2);
        }
        else if (b > c) // use the aterm pointer comparison
        {
          assert(is_simple(c));
          return apply_if(c, apply_if(b, t1, u1), apply_if(b, t1, u2));
        }
        else
        {
          return if_(b, t1, t2);
        }
      }
      else
      {
        return if_(b, t1, t2);
      }
    }
  }

  data_expression apply(const application& x)
  {
    if (is_if_application(x))
    {
      data_expression b = super::apply(x[0]);
      data_expression t1 = super::apply(x[1]);
      data_expression t2 = super::apply(x[2]);
      return apply_if(b, t1, t2);
    }
    else
    {
      return push_if_outside(super::apply(x));
    }
  }
};

struct if_rewrite_with_rewriter_builder: public if_rewrite_builder<if_rewrite_with_rewriter_builder>
{
  typedef if_rewrite_builder<if_rewrite_with_rewriter_builder> super;
  using super::apply;
  using super::apply_if;

  data::rewriter& rewr;

  explicit if_rewrite_with_rewriter_builder(data::rewriter& rewr_) : rewr(rewr_)
  {}

  data_expression apply(const application& x)
  {
    data_expression result;
    if (is_if_application(x))
    {
      data_expression b = super::apply(x[0]);
      data_expression t1 = super::apply(x[1]);
      data_expression t2 = super::apply(x[2]);
      result = apply_if(b, t1, t2);
    }
    else
    {
      result = push_if_outside(atermpp::down_cast<application>(super::apply(x)));
    }
    return rewr(result);
  }
};

} // namespace detail

struct if_rewriter
{
  using argument_type = data_expression;

  data_expression operator()(const data_expression& x) const
  {
    return core::make_apply_builder<detail::if_rewrite_builder>().apply(x);
  }
};

template <typename T>
void if_rewrite(T& x, typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  core::make_update_apply_builder<data::data_expression_builder>(if_rewriter()).update(x);
}

template <typename T>
T if_rewrite(const T& x, typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  T result = core::make_update_apply_builder<data::data_expression_builder>(if_rewriter()).apply(x);
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITERS_IF_REWRITER_H
