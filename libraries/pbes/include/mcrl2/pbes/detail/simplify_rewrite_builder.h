// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/simplify_rewrite_builder.h
/// \brief Simplifying rewriter for pbes expressions.

#ifndef MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H
#define MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H

#include <set>
#include <utility>
#include "mcrl2/utilities/optimized_boolean_operators.h"
#include "mcrl2/pbes/pbes_expr_builder.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

// Simplifying PBES rewriter.
template <typename Term, typename DataRewriter, typename SubstitutionFunction = no_substitution>
struct simplify_rewrite_builder: public pbes_expr_builder<Term, SubstitutionFunction>
{
  typedef pbes_expr_builder<Term, SubstitutionFunction>                super;
  typedef SubstitutionFunction                                               argument_type;
  typedef typename super::term_type                                          term_type;
  typedef typename core::term_traits<term_type>::data_term_type              data_term_type;
  typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;
  typedef typename core::term_traits<term_type>::variable_sequence_type variable_sequence_type;
  typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
  typedef core::term_traits<Term> tr;

  const DataRewriter& m_data_rewriter;

  /// If true, data expressions are not rewritten.
  bool m_skip_data;

  /// \brief Is called in the case rewriting is done with a substitution function.
  /// \param d A data term
  /// \param sigma A substitution function
  /// \return The rewrite result
  template <typename T>
  data_term_type rewrite(data_term_type d, T& sigma)
  {
//data_term_type result = m_data_rewriter(d, sigma);
//std::cerr << m_data_rewriter.type() << "<d>" << data::pp(d) << " => " << core::term_traits<data_term_type>::pp(result) << std::endl;
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
  simplify_rewrite_builder(const DataRewriter& rewr, bool skip_data = false)
    : m_data_rewriter(rewr),
      m_skip_data(skip_data)
  { }

  /// \brief Visit data_expression node
  /// Visit data expression node.
  /// \param x A term
  /// \param d A data term
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_data_expression(const term_type& /* x */, const data_term_type& d, SubstitutionFunction& sigma)
  {
    if (m_skip_data)
    {
      return d;
    }
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
  term_type visit_not(const term_type& /* x */, const term_type& n, SubstitutionFunction& /* sigma */)
  {
    if (tr::is_true(n))
    {
      return tr::false_();
    }
    if (tr::is_false(n))
    {
      return tr::true_();
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
    if (tr::is_true(left))
    {
      return super::visit(right, sigma);
    }
    if (tr::is_true(right))
    {
      return super::visit(left, sigma);
    }
    if (tr::is_false(left))
    {
      return tr::false_();
    }
    if (tr::is_false(right))
    {
      return tr::false_();
    }
    if (left == right)
    {
      return super::visit(left, sigma);
    }
    return term_type(); // continue recursion
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
    if (tr::is_true(left))
    {
      return tr::true_();
    }
    if (tr::is_true(right))
    {
      return tr::true_();
    }
    if (tr::is_false(left))
    {
      return super::visit(right, sigma);
    }
    if (tr::is_false(right))
    {
      return super::visit(left, sigma);
    }
    if (left == right)
    {
      return super::visit(left, sigma);
    }
    return term_type(); // continue recursion
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
    if (tr::is_true(left))
    {
      return super::visit(right, sigma);
    }
    if (tr::is_false(left))
    {
      return tr::true_();
    }
    if (tr::is_true(right))
    {
      return tr::true_();
    }
    if (left == right)
    {
      return tr::true_();
    }
    if (tr::is_false(right))
    {
      return super::visit(tr::not_(left), sigma);
    }
    return term_type(); // continue recursion
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
    term_type t = super::visit(phi, sigma);
    return utilities::optimized_forall(variables, t, true);
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
    term_type t = super::visit(phi, sigma);
    return utilities::optimized_exists(variables, t);
  }

  /// \brief Visit propositional_variable node
  /// Visit propositional variable node.
  /// \param x A term
  /// \param v A propositional variable
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_propositional_variable(const term_type& /* x */, const propositional_variable_type& v, SubstitutionFunction& sigma)
  {
    if (m_skip_data)
    {
      return v;
    }
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

#endif // MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H
