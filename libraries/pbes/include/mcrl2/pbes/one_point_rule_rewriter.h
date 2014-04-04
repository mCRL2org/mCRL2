// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/one_point_rule_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ONE_POINT_RULE_REWRITER_H
#define MCRL2_PBES_ONE_POINT_RULE_REWRITER_H

#include <cassert>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/detail/one_point_rule_preprocessor.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/detail/data2pbes_rewriter.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
namespace detail {

// \pre left and right are data variables
// if left not in preferred_left_hand_sides and right in preferred_left_hand_sides, then left and right will be swapped,
void sort_variables(data::data_expression& left, data::data_expression& right, const std::set<data::variable>& preferred_left_hand_sides)
{
  if (!utilities::detail::contains(preferred_left_hand_sides, core::static_down_cast<const data::variable&>(left)) &&
       utilities::detail::contains(preferred_left_hand_sides, core::static_down_cast<const data::variable&>(right)))
  {
    std::swap(left, right);
  }
}

// Attempt to write x as (v = e), with v a data variable and e a data expression.
// In case of (v = v), variables appearing in preferred_left_hand_sides will always be put in the left hand side of the result.
// Returns true if succeeded.
inline
bool is_data_equality(const pbes_expression& x, const std::set<data::variable>& preferred_left_hand_sides, data::variable& v, data::data_expression& e)
{
  if (data::is_data_expression(x))
  {
    // check if the term x corresponds to (v == e), with v a variable.
    if (data::is_equal_to_application(data::data_expression(x)))
    {
      data::data_expression left = data::binary_left(atermpp::aterm_cast<data::application>(x));
      data::data_expression right = data::binary_right(atermpp::aterm_cast<data::application>(x));
      if (data::is_variable(left))
      {
        if (data::is_variable(right))
        {
          sort_variables(left, right, preferred_left_hand_sides);
        }
        v = core::static_down_cast<const data::variable&>(left);
        e = right;
        return true;
      }
      else if (data::is_variable(right))
      {
        v = core::static_down_cast<const data::variable&>(right);
        e = left;
        return true;
      }
    }
    // check if the term x corresponds to v, with v a boolean variable.
    else if (data::is_variable(x))
    {
      const data::variable& vx = atermpp::aterm_cast<const data::variable>(x);
      v = vx;
      e = data::sort_bool::true_();
      return true;
    }
    // check if the term x corresponds to !v, with v a boolean variable.
    else if (data::sort_bool::is_not_application(x))
    {
      const data::data_expression& operand = core::static_down_cast<const data::data_expression&>(data::application(x)[0]);
      if (data::is_variable(operand))
      {
        const data::variable& voperand = atermpp::aterm_cast<const data::variable>(operand);
        v = voperand;
        e = data::sort_bool::false_();
        return true;
      }
    }
  }
  // check if the term x corresponds to !v, with v a boolean variable.
  else if (is_not(x))
  {
    pbes_expression operand = not_(atermpp::aterm_appl(x)).operand();
    if (data::is_variable(operand))
    {
      const data::variable& voperand = atermpp::aterm_cast<const data::variable>(operand);
      v = voperand;
      e = data::sort_bool::false_();
      return true;
    }
  }
  return false;
}

// Attempt to write x as (v != e), with v a data variable and e a data expression.
// Returns true if succeeded.
inline
bool is_data_inequality(const pbes_expression& x, const std::set<data::variable>& preferred_left_hand_sides, data::variable& v, data::data_expression& e)
{
  if (data::is_data_expression(x))
  {
    // check if the term x corresponds to (v != e), with v a variable.
    if (data::is_not_equal_to_application(data::data_expression(x)))
    {
      data::data_expression left = data::binary_left(atermpp::aterm_cast<data::application>(x));
      data::data_expression right = data::binary_right(atermpp::aterm_cast<data::application>(x));
      if (data::is_variable(left))
      {
        if (data::is_variable(right))
        {
          sort_variables(left, right, preferred_left_hand_sides);
        }
        v = core::static_down_cast<const data::variable&>(left);
        e = right;
        return true;
      }
      else if (data::is_variable(right))
      {
        v = core::static_down_cast<const data::variable&>(right);
        e = left;
        return true;
      }
    }
    // check if the term x corresponds to v, with v a boolean variable.
    else if (data::is_variable(x))
    {
      const data::variable& vx = atermpp::aterm_cast<const data::variable>(x);
      v = vx;
      e = data::sort_bool::false_();
      return true;
    }
    else if (data::sort_bool::is_not_application(x))
    {
      data::data_expression operand(x[0]);
      if (data::is_variable(operand))
      {
        const data::variable& voperand = atermpp::aterm_cast<const data::variable>(operand);
        v = voperand;
        e = data::sort_bool::true_();
        return true;
      }
    }
  }
  else if (is_not(x))
  {
    pbes_expression operand = not_(atermpp::aterm_appl(x)).operand();
    if (data::is_variable(operand))
    {
      const data::variable& voperand = atermpp::aterm_cast<const data::variable>(operand);
      v = voperand;
      e = data::sort_bool::true_();
      return true;
    }
  }
  return false;
}

// @pre: v is not in sigma
// 1) apply [v := e] to all right hand sides of sigma
// 2) add [v := e] to sigma
inline
void update_substitution(data::mutable_map_substitution<>& sigma, const data::variable& v, const data::data_expression& e)
{
  data::mutable_map_substitution<> v_e;
  v_e[v] = e;
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    i->second = data::replace_variables_capture_avoiding(i->second, v_e, data::substitution_variables(v_e));
  }
  sigma[v] = e;
}

template <typename Derived>
struct one_point_rule_rewrite_builder: public pbes_system::pbes_expression_builder<Derived>
{
  typedef pbes_system::pbes_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // convert !val(d) to val(!d)
  pbes_expression operator()(const not_& x)
  {
    if (is_data(x.operand()))
    {
      const data::data_expression& d = atermpp::aterm_cast<data::data_expression>(x.operand());
      data::detail::one_point_rule_preprocessor R;
      return R(data::sort_bool::not_(d));
    }
    return x;
  }

  pbes_expression operator()(const imp& x)
  {
    pbes_expression result = derived()(or_(not_(x.left()), x.right()));
    mCRL2log(log::debug, "one_point_rewriter") << pbes_system::pp(x) << " -> " << pbes_system::pp(result) << std::endl;
    return result;
  }

  pbes_expression operator()(const exists& x)
  {
    mCRL2log(log::debug, "one_point_rewriter") << "x = " << pbes_system::pp(x) << std::endl;
    pbes_expression body = derived()(x.body());
    std::set<pbes_expression> terms = pbes_expr::split_and(body, true);
    mCRL2log(log::debug, "one_point_rewriter") << "  split_and(x.body()) = " << core::detail::print_set(terms) << std::endl;
    data::mutable_map_substitution<> sigma;
    std::set<data::variable> variables = atermpp::convert< std::set<data::variable> >(x.variables());
    std::vector< std::set<pbes_expression>::iterator > to_be_removed;

    for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
    {
      data::variable v;
      data::data_expression e;
      if (is_data_equality(*i, variables, v, e))
      {
        mCRL2log(log::debug, "one_point_rewriter") << "  subterm " << *i << " is a data equality" << std::endl;
        if (variables.find(v) != variables.end()) // only consider quantifier variables
        {
          e = data::replace_variables_capture_avoiding(e, sigma, data::substitution_variables(sigma));
          data::data_expression f = sigma(v);
          if (f == v)
          {
            update_substitution(sigma, v, e);
            std::set<data::variable> fe = data::find_free_variables(e);
            if (fe.find(v) == fe.end())
            {
              mCRL2log(log::debug, "one_point_rewriter") << "  subterm " << *i << " causes variable " << v << " to be removed " << std::endl;
              variables.erase(v); // make sure the same variable can only be removed once
              to_be_removed.push_back(i);
            }
          }
          else
          {
            // v is already in sigma, check if e is an improvement over f
            std::set<data::variable> e_variables = data::find_free_variables(e);
            std::set<data::variable> f_variables = data::find_free_variables(f);
            if (e_variables.size() < f_variables.size() &&
                std::includes(f_variables.begin(), f_variables.end(), e_variables.begin(), e_variables.end())
               )
            {
              sigma[v] = e;
            }
          }
        }
      }
    }

    mCRL2log(log::debug, "one_point_rewriter") << "  sigma = " << data::print_substitution(sigma) << " variables = " << core::detail::print_set(variables) << std::endl;

    pbes_expression result;
    if (!sigma.empty())
    {
      for (std::vector< std::set<pbes_expression>::iterator >::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
      {
        terms.erase(*i);
      }
      pbes_expression body = pbes_expr::join_and(terms.begin(), terms.end());
      body = pbes_system::replace_variables_capture_avoiding(body, sigma, data::substitution_variables(sigma));
      if (variables.empty())
      {
        result = body;
      }
      else
      {
        result = exists(data::variable_list(variables.begin(), variables.end()), body);
      }
    }
    else
    {
      result = exists(x.variables(), body);
    }
    mCRL2log(log::debug, "one_point_rewriter") << pbes_system::pp(x) << " ---> " << pbes_system::pp(result) << std::endl;
    assert(data::detail::set_intersection(std::set<data::variable>(x.variables().begin(), x.variables().end()), pbes_system::find_free_variables(result)).empty());
    return result;
  }

  pbes_expression operator()(const forall& x)
  {
    mCRL2log(log::debug, "one_point_rewriter") << "x = " << pbes_system::pp(x) << std::endl;
    pbes_expression body = derived()(x.body());
    std::set<pbes_expression> terms = pbes_expr::split_or(body, true);
    mCRL2log(log::debug, "one_point_rewriter") << "  split_or(x.body()) = " << core::detail::print_set(terms) << std::endl;
    data::mutable_map_substitution<> sigma;
    std::set<data::variable> variables = atermpp::convert< std::set<data::variable> >(x.variables());
    std::vector< std::set<pbes_expression>::iterator > to_be_removed;

    for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
    {
      data::variable v;
      data::data_expression e;
      if (is_data_inequality(*i, variables, v, e))
      {
        mCRL2log(log::debug, "one_point_rewriter") << "  subterm " << *i << " is a data inequality" << std::endl;
        if (variables.find(data::variable(v)) != variables.end()) // only consider quantifier variables
        {
          e = data::replace_variables_capture_avoiding(e, sigma, data::substitution_variables(sigma));
          update_substitution(sigma, v, e);
          std::set<data::variable> fe = data::find_free_variables(e);
          if (fe.find(v) == fe.end())
          {
            mCRL2log(log::debug, "one_point_rewriter") << "  subterm " << *i << " causes variable " << v << " to be removed " << std::endl;
            variables.erase(v); // make sure the same variable can only be removed once
            to_be_removed.push_back(i);
          }
        }
      }
    }

    mCRL2log(log::debug, "one_point_rewriter") << "  sigma = " << data::print_substitution(sigma) << " variables = " << core::detail::print_set(variables) << std::endl;

    pbes_expression result;
    if (!sigma.empty())
    {
      for (auto i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
      {
        terms.erase(*i);
      }
      pbes_expression body = pbes_expr::join_or(terms.begin(), terms.end());
      body = pbes_system::replace_variables_capture_avoiding(body, sigma, data::substitution_variables(sigma));
      if (variables.empty())
      {
        result = body;
      }
      else
      {
        result = forall(data::variable_list(variables.begin(), variables.end()), body);
      }
    }
    else
    {
      result = forall(x.variables(), body);
    }
    mCRL2log(log::debug, "one_point_rewriter") << pbes_system::pp(x) << " ---> " << pbes_system::pp(result) << std::endl;
    assert(data::detail::set_intersection(std::set<data::variable>(x.variables().begin(), x.variables().end()), pbes_system::find_free_variables(result)).empty());
    return result;
  }
};

} // namespace detail
/// \endcond

/// \brief A rewriter that applies one point rule quantifier elimination to a PBES.
class one_point_rule_rewriter
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
      return core::make_apply_builder<detail::one_point_rule_rewrite_builder>()(detail::data2pbes(normalize(x)));
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ONE_POINT_RULE_REWRITER_H
