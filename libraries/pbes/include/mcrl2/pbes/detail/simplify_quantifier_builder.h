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
#include "mcrl2/utilities/optimized_boolean_operators.h"
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
  typedef pbes_expr_builder<Term, SubstitutionFunction>                super;
  typedef SubstitutionFunction                                               argument_type;
  typedef typename super::term_type                                          term_type;
  typedef typename core::term_traits<term_type>::data_term_type              data_term_type;
  typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;
  typedef typename core::term_traits<term_type>::variable_sequence_type      variable_sequence_type;
  typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
  typedef core::term_traits<Term> tr;

  const DataRewriter& m_data_rewriter;

  /// \brief Store the variables of the lhs and rhs of any application of and/or for later reference
  mutable variable_sequence_type lvar;
  mutable variable_sequence_type rvar;

  /// \brief A binary function with the side effect that the dependent variables of the arguments are stored
  template <typename BinaryFunction>
  struct store_variables_function
  {
    variable_sequence_type& lvar;
    variable_sequence_type& rvar;
    BinaryFunction& op;

    store_variables_function(variable_sequence_type& l, variable_sequence_type& r, BinaryFunction& o)
      : lvar(l), rvar(r), op(o)
    {}

    term_type operator()(const term_type& left, const term_type& right) const
    {
      lvar = tr::free_variables(left);
      rvar = tr::free_variables(right);
      return op(left, right);
    }
  };

  /// \brief Utility function for creating a store_variables_function
  template <typename BinaryFunction>
  store_variables_function<BinaryFunction> make_store_variables_function(variable_sequence_type& l, variable_sequence_type& r, BinaryFunction& o)
  {
    return store_variables_function<BinaryFunction>(l, r, o);
  }

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
    if (tr::is_not(n))
    {
      return super::visit(tr::arg(n), sigma);
    }
    return term_type(); // continue recursion
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
    return utilities::detail::optimized_and(l, r, make_store_variables_function(lvar, rvar, tr::and_), tr::true_(), tr::is_true, tr::false_(), tr::is_false);
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
    return utilities::detail::optimized_or(l, r, make_store_variables_function(lvar, rvar, tr::or_), tr::true_(), tr::is_true, tr::false_(), tr::is_false);
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
    return utilities::detail::optimized_imp(l, r, make_store_variables_function(lvar, rvar, tr::imp), tr::not_, tr::true_(), tr::is_true, tr::false_(), tr::is_false);
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
    typedef typename core::term_traits<data_term_type> tt;
    term_type result;
    term_type t = super::visit(phi, sigma);

    if (tr::is_not(t))
    {
      result = utilities::optimized_not(utilities::optimized_exists(tt::set_intersection(variables, tr::free_variables(t)), tr::arg(t)));
    }
    if (tr::is_and(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      data::variable_list lv = tt::set_intersection(variables, lvar);
      data::variable_list rv = tt::set_intersection(variables, rvar);
      result = utilities::optimized_and(utilities::optimized_forall(lv, l), utilities::optimized_forall(rv, r));
    }
    else if (tr::is_or(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      data::variable_list lv = tt::set_intersection(variables, lvar);
      data::variable_list rv = tt::set_intersection(variables, rvar);
      if (lv.empty())
      {
        result = utilities::optimized_or(l, utilities::optimized_forall(rv, r));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_or(r, utilities::optimized_forall(lv, l));
      }
      else
      {
        result = utilities::optimized_forall(tt::set_intersection(variables, tr::free_variables(t)), t);
      }
    }
    else
    {
      result = utilities::optimized_forall(tt::set_intersection(variables, tr::free_variables(t)), t);
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
    typedef typename core::term_traits<data_term_type> tt;
    term_type result;
    term_type t = super::visit(phi, sigma);

    if (tr::is_not(t))
    {
      result = utilities::optimized_not(utilities::optimized_forall(tt::set_intersection(variables, tr::free_variables(t)), tr::arg(t)));
    }
    else if (tr::is_or(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      data::variable_list lv = tt::set_intersection(variables, lvar);
      data::variable_list rv = tt::set_intersection(variables, rvar);
      result = utilities::optimized_or(utilities::optimized_exists(lvar, l), utilities::optimized_exists(rvar, r));
    }
    else if (tr::is_and(t))
    {
      term_type l = tr::left(t);
      term_type r = tr::right(t);
      data::variable_list lv = tt::set_intersection(variables, lvar);
      data::variable_list rv = tt::set_intersection(variables, rvar);
      if (lv.empty())
      {
        result = utilities::optimized_and(l, utilities::optimized_exists(rv, r));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_and(r, utilities::optimized_exists(lv, l));
      }
      else
      {
        result = utilities::optimized_exists(tt::set_intersection(variables, tr::free_variables(t)), t);
      }
    }
    else
    {
      result = utilities::optimized_exists(tt::set_intersection(variables, tr::free_variables(t)), t);
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
