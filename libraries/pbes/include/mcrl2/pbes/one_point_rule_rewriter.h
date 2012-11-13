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

#include "mcrl2/data/join.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/detail/one_point_rule_preprocessor.h"
#include "mcrl2/pbes/detail/data2pbes_rewriter.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/replace.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
namespace detail {

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

  pbes_expression operator()(const imp& x)
  {
    return derived()(or_(not_(x.left()), x.right()));
  }

  pbes_expression operator()(const exists& x)
  {
    namespace a = data::detail::data_accessors;

    pbes_expression body = static_cast<Derived&>(*this)(x.body());
    std::set<pbes_expression> terms = pbes_expr::split_and(body, true);
    data::mutable_map_substitution<> sigma;
    std::set<data::variable> variables = atermpp::convert< std::set<data::variable> >(x.variables());
    std::vector< std::set<pbes_expression>::iterator > to_be_removed;

    for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
    {
      if (data::is_data_expression(*i))
      {
        // check if the term *i corresponds to (v == e), with v a quantifier variable.
        if (data::is_equal_to_application(data::data_expression(*i)))
        {
          data::data_expression left = a::left(*i);
          data::data_expression right = a::right(*i);
          if (data::is_variable(left) && variables.find(data::variable(left)) != variables.end())
          {
            sigma[data::variable(left)] = right;
            variables.erase(data::variable(left)); // make sure the same variable can only be removed once
            to_be_removed.push_back(i);
          }
          else if (data::is_variable(right) && variables.find(data::variable(right)) != variables.end())
          {
            sigma[data::variable(right)] = left;
            variables.erase(data::variable(right));
            to_be_removed.push_back(i);
          }
        }
        // check if the term *i corresponds to b, with b a boolean quantifier variable.
        else if (data::is_variable(*i))
        {
          assert(data::sort_bool::is_bool(data::variable(*i).sort()));
          data::variable b = *i;
          sigma[b] = data::sort_bool::true_();
          variables.erase(b);
          to_be_removed.push_back(i);
        }
      }
    }

    if (!sigma.empty())
    {
      for (std::vector< std::set<pbes_expression>::iterator >::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
      {
        terms.erase(*i);
      }
      pbes_expression body = pbes_expr::join_and(terms.begin(), terms.end());
      body = pbes_system::replace_free_variables(body, sigma);
      if (variables.empty())
      {
        return body;
      }
      else
      {
        return exists(data::variable_list(variables.begin(), variables.end()), body);
      }
    }

    return exists(x.variables(), body);
  }

  pbes_expression operator()(const forall& x)
  {
    namespace a = data::detail::data_accessors;

    pbes_expression body = static_cast<Derived&>(*this)(x.body());
    std::set<pbes_expression> terms = pbes_expr::split_or(body, true);
    data::mutable_map_substitution<> sigma;
    std::set<data::variable> variables = atermpp::convert< std::set<data::variable> >(x.variables());
    std::vector< std::set<pbes_expression>::iterator > to_be_removed;

    for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
    {
      if (data::is_data_expression(*i))
      {
        // check if the term *i corresponds to (v != e), with v a quantifier variable.
        if (data::is_not_equal_to_application(data::data_expression(*i)))
        {
          data::data_expression left = a::left(*i);
          data::data_expression right = a::right(*i);
          if (data::is_variable(left) && variables.find(data::variable(left)) != variables.end())
          {
            sigma[data::variable(left)] = right;
            variables.erase(data::variable(left)); // make sure the same variable can only be removed once
            to_be_removed.push_back(i);
          }
          else if (data::is_variable(right) && variables.find(data::variable(right)) != variables.end())
          {
            sigma[data::variable(right)] = left;
            variables.erase(data::variable(right));
            to_be_removed.push_back(i);
          }
        }
      }
      // check if the term *i corresponds to !b, with b a boolean quantifier variable.
      else if (is_not(*i))
      {
        pbes_expression e = not_(atermpp::aterm_appl(*i)).operand();
        if (data::is_variable(e) && variables.find(data::variable(e)) != variables.end())
        {
          data::variable b = e;
          assert(data::sort_bool::is_bool(b.sort()));
          sigma[b] = data::sort_bool::false_();
          variables.erase(b);
          to_be_removed.push_back(i);
        }
      }
    }

    if (!sigma.empty())
    {
      for (std::vector< std::set<pbes_expression>::iterator >::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
      {
        terms.erase(*i);
      }
      pbes_expression body = pbes_expr::join_or(terms.begin(), terms.end());
      body = pbes_system::replace_free_variables(body, sigma);
      if (variables.empty())
      {
        return body;
      }
      else
      {
        return forall(data::variable_list(variables.begin(), variables.end()), body);
      }
    }

    return forall(x.variables(), body);
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
      return core::make_apply_builder<detail::one_point_rule_rewrite_builder>()(detail::data2pbes(x));
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ONE_POINT_RULE_REWRITER_H
