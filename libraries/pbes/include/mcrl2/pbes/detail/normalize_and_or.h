// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/normalize_and_or.h
/// \brief Function to normalize 'and' and 'or' sub expressions.

#ifndef MCRL2_PBES_DETAIL_NORMALIZE_AND_OR_H
#define MCRL2_PBES_DETAIL_NORMALIZE_AND_OR_H

#include <set>
#include <utility>
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  // Simplifying PBES rewriter.
  struct normalize_and_or_builder: public pbes_expression_builder<pbes_expression>
  {
    typedef pbes_expression_builder<pbes_expression>                           super;
    typedef super::term_type                                          term_type;
    typedef core::term_traits<term_type>::data_term_type              data_term_type;
    typedef core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;
    typedef core::term_traits<term_type>::variable_sequence_type      variable_sequence_type;
    typedef core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
    typedef core::term_traits<term_type> tr;

    /// \brief Splits a disjunction into a sequence of operands
    /// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
    /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
    /// function symbol.
    /// \param expr A PBES expression
    /// \return A sequence of operands
    atermpp::multiset<pbes_expression> split_or(const pbes_expression& expr)
    {
      using namespace accessors;
      atermpp::multiset<pbes_expression> result;
      core::detail::split(expr, std::insert_iterator<atermpp::multiset<pbes_expression> >(result, result.begin()), tr::is_or, left, right);
      return result;
    }
    
    /// \brief Splits a conjunction into a sequence of operands
    /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
    /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
    /// function symbol.
    /// \param expr A PBES expression
    /// \return A sequence of operands
    atermpp::multiset<pbes_expression> split_and(const pbes_expression& expr)
    {
      using namespace accessors;
      atermpp::multiset<pbes_expression> result;
      core::detail::split(expr, std::insert_iterator<atermpp::multiset<pbes_expression> >(result, result.begin()), tr::is_and, left, right);
      return result;
    }

    pbes_expression normalize(const pbes_expression& x)
    {
    	typedef core::term_traits<pbes_expression> tr;
    	if (tr::is_and(x))
      {
      	atermpp::multiset<pbes_expression> s = split_and(x);
      	return pbes_expr::join_and(s.begin(), s.end());
      }
    	else if (tr::is_or(x))
      {
      	atermpp::multiset<pbes_expression> s = split_or(x);
      	return pbes_expr::join_or(s.begin(), s.end());
      }
      return x;
    }

    /// \brief Visit not node
    /// Visit not node.
    /// \param x A term
    /// \param n A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_not(const term_type& x, const term_type& n)
    {
      term_type t = normalize(visit(t));
    	return tr::not_(t);
    }

    /// \brief Visit and node
    /// Visit and node.
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_and(const term_type& x, const term_type& left, const term_type& right)
    {
      term_type l = normalize(super::visit(left));
      term_type r = normalize(super::visit(right));
      return tr::and_(l, r);
    }

    /// \brief Visit or node
    /// Visit or node.
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_or(const term_type& x, const term_type& left, const term_type& right)
    {
      term_type l = normalize(super::visit(left));
      term_type r = normalize(super::visit(right));
      return tr::or_(l, r);
    }

    /// \brief Visit imp node
    /// Visit imp node.
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_imp(const term_type& x, const term_type& left, const term_type& right)
    {
      term_type l = normalize(super::visit(left));
      term_type r = normalize(super::visit(right));
      return tr::imp(l, r);
    }

    /// \brief Visit forall node
    /// Visit forall node.
    /// \param x A term
    /// \param variables A sequence of variables
    /// \param phi A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_forall(const term_type& x, const variable_sequence_type& variables, const term_type& phi)
    {
      term_type t = normalize(visit(phi));
      return tr::forall(variables, t);
    }

    /// \brief Visit exists node
    /// Visit exists node.
    /// \param x A term
    /// \param variables A sequence of variables
    /// \param phi A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_exists(const term_type& x, const variable_sequence_type& variables, const term_type& phi)
    {
      term_type t = normalize(visit(phi));
      return tr::exists(variables, t);
    }

    /// \brief Applies this builder to the term x.
    /// \param x A term
    /// \return The normalilzed term
    term_type operator()(const term_type& x)
    {
      return normalize(visit(x));
    }
  };

  inline
  pbes_expression normalize_and_or(const pbes_expression& x)
  {
  	normalize_and_or_builder builder;
  	return builder(x);
  }

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_NORMALIZE_AND_OR_H
