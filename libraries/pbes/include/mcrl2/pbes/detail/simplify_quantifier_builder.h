// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/simplify_quantifier_builder.h
/// \brief Simplifying rewriter for pbes expressions.

#ifndef MCRL2_PBES_DETAIL_SIMPLIFY_QUANTIFIER_BUILDER_H
#define MCRL2_PBES_DETAIL_SIMPLIFY_QUANTIFIER_BUILDER_H

#include <set>
#include <utility>
#include "mcrl2/utilities/detail/optimized_logic_operators.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/pbes/pbes_expr_builder.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

// Simplifying PBES rewriter.
template <typename Term, typename DataRewriter, typename SubstitutionFunction = no_substitution>
struct simplify_quantifier_builder: public pbes_expr_builder<Term, SubstitutionFunction>
{
  typedef pbes_expr_builder<Term, SubstitutionFunction>                      super;
  typedef SubstitutionFunction                                               argument_type;
  typedef typename super::term_type                                          term_type;
  typedef typename core::term_traits<term_type>::data_term_type              data_term_type;
  typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;
  typedef typename core::term_traits<term_type>::variable_sequence_type      variable_sequence_type;
  typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
  typedef core::term_traits<Term> tr;

  const DataRewriter& m_data_rewriter;

  /// \brief Is called in the case rewriting is done with a substitution function.
  /// \param d A data term
  /// \param sigma A substitution function
  /// \return The rewrite result
  template <typename T>
  data_term_type rewrite(data_term_type d, T& sigma)
  {
    return m_data_rewriter(d, sigma);
  }

  /// \brief Is called in the case rewriting is done without a substitution function.
  /// \param d A data term
  /// \return The rewrite result
  data_term_type rewrite(data_term_type d, no_substitution&)
  {
    return m_data_rewriter(d);
  }

  /// \brief Constructor.
  /// \param rewr A data rewriter
  simplify_quantifier_builder(const DataRewriter& rewr)
    : m_data_rewriter(rewr)
  { }

  /// \brief Visit data_expression node
  /// Visit data expression node.
  /// \param x A term
  /// \param d A data term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_data_expression(const term_type& /* x */, const data_term_type& d, SubstitutionFunction& sigma)
  {
    return tr::dataterm2term(rewrite(d, sigma));
  }

  /// \brief Visit true node
  /// Visit true node.
  /// \param x A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_true(const term_type& /* x */, SubstitutionFunction& /* sigma */)
  {
    return tr::true_();
  }

  /// \brief Visit false node
  /// Visit false node.
  /// \param x A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_false(const term_type& /* x */, SubstitutionFunction& /* sigma */)
  {
    return tr::false_();
  }

  /// \brief Visit not node
  /// Visit not node.
  /// \param x A term
  /// \param n A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_not(const term_type& /* x */, const term_type& n, SubstitutionFunction& sigma)
  {
    term_type arg = super::visit(n, sigma);
    return utilities::optimized_not(arg);
  }

  /// \brief Visit and node
  /// Visit and node.
  /// \param x A term
  /// \param left A term
  /// \param right A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_and(const term_type& /* x */, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    term_type l = super::visit(left, sigma);
    term_type r = super::visit(right, sigma);
    return utilities::optimized_and(l, r);
  }

  /// \brief Visit or node
  /// Visit or node.
  /// \param x A term
  /// \param left A term
  /// \param right A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_or(const term_type& /* x */, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    term_type l = super::visit(left, sigma);
    term_type r = super::visit(right, sigma);
    return utilities::optimized_or(l, r);
  }

  /// \brief Visit imp node
  /// Visit imp node.
  /// \param x A term
  /// \param left A term
  /// \param right A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_imp(const term_type& /* x */, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    term_type l = super::visit(left, sigma);
    term_type r = super::visit(right, sigma);
    return utilities::optimized_imp(l, r);
  }

  /// \brief Visit forall node
  /// Visit forall node.
  /// \param x A term
  /// \param variables A sequence of variables
  /// \param phi A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_forall(const term_type& /* x */, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
  {
    term_type result;
    term_type t = super::visit(phi, sigma);

    if (variables.empty())
    {
      return tr::true_();
    }
    else if (tr::is_not(t))
    {
      result = utilities::optimized_not(utilities::optimized_exists(variables, tr::arg(t), true));
    }
    if (tr::is_and(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      result = utilities::optimized_and(utilities::optimized_forall(variables, l, true), utilities::optimized_forall(variables, r, true));
    }
    else if (tr::is_or(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      data::variable_list lv = tr::set_intersection(variables, tr::free_variables(l));
      data::variable_list rv = tr::set_intersection(variables, tr::free_variables(r));
      if (lv.empty())
      {
        result = utilities::optimized_or(l, utilities::optimized_forall(rv, r, true, false));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_or(r, utilities::optimized_forall(lv, l, true, false));
      }
      else
      {
        result = utilities::optimized_forall(variables, t, true);
      }
    }
    else
    {
      result = utilities::optimized_forall(variables, t, true);
    }
    return result;
  }

  /// \brief Visit exists node
  /// Visit exists node.
  /// \param x A term
  /// \param variables A sequence of variables
  /// \param phi A term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_exists(const term_type& /* x */, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
  {
    term_type result;
    term_type t = super::visit(phi, sigma);

    if (variables.empty())
    {
      return tr::false_();
    }
    else if (tr::is_not(t))
    {
      result = utilities::optimized_not(utilities::optimized_forall(variables, tr::arg(t), true));
    }
    else if (tr::is_or(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      result = utilities::optimized_or(utilities::optimized_exists(variables, l, true), utilities::optimized_exists(variables, r, true));
    }
    else if (tr::is_and(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      data::variable_list lv = tr::set_intersection(variables, tr::free_variables(l));
      data::variable_list rv = tr::set_intersection(variables, tr::free_variables(r));
      if (lv.empty())
      {
        result = utilities::optimized_and(l, utilities::optimized_exists(rv, r, true, false));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_and(r, utilities::optimized_exists(lv, l, true, false));
      }
      else
      {
        result = utilities::optimized_exists(variables, t, true);
      }
    }
    else
    {
      result = utilities::optimized_exists(variables, t, true);
    }
    return result;
  }

  /// \brief Visit propositional_variable node
  /// Visit propositional variable node.
  /// \param x A term
  /// \param v A propositional variable
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_propositional_variable(const term_type& /* x */, const propositional_variable_type& v, SubstitutionFunction& sigma)
  {
    std::vector<data_term_type> d;
    data_term_sequence_type e = tr::param(v);
    for (typename data_term_sequence_type::const_iterator i = e.begin(); i != e.end(); ++i)
    {
      d.push_back(rewrite(*i, sigma));
    }
    term_type result = tr::prop_var(tr::name(v), d.begin(), d.end());
    return result;
  }

  /// \brief Applies this builder to the term x.
  /// \param x A term
  /// \return The rewrite result
  term_type operator()(const term_type& x)
  {
    SubstitutionFunction tmp;
    return super::visit(x, tmp);
  }

  /// \brief Applies this builder to the term x, with substitution sigma.
  /// \param x A term
  /// \param sigma A substitution function
  /// \return The rewrite result
  term_type operator()(const term_type& x, SubstitutionFunction sigma)
  {
    return super::visit(x, sigma);
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SIMPLIFY_QUANTIFIER_BUILDER_H
