// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/boolean_boolean_simplify_builder.h
/// \brief Simplifying rewriter for boolean expressions.

#ifndef MCRL2_PBES_DETAIL_BOOLEAN_SIMPLIFY_BUILDER_H
#define MCRL2_PBES_DETAIL_BOOLEAN_SIMPLIFY_BUILDER_H

#include <set>
#include <utility>
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/pbes/boolean_expression_builder.h"

namespace mcrl2 {

namespace bes {

namespace detail {
  
  struct no_substitution
  {
  };

  // Simplifying rewriter.
  template <typename Term, typename Arg = no_substitution>
  struct boolean_simplify_builder: public boolean_expression_builder<Term, Arg>
  {
    typedef boolean_expression_builder<Term, Arg> super;
    typedef Arg                                argument_type;
    typedef typename super::term_type          term_type;
    typedef core::term_traits<Term> tr;

    /// \brief Visit true node
    /// \param x A term
    /// \param arg An additional argument for the recursion
    /// \return The result of visiting the node
    term_type visit_true(const term_type& x, Arg& arg)
    {
      return tr::true_();
    }

    /// \brief Visit false node
    /// \param x A term
    /// \param arg An additional argument for the recursion
    /// \return The result of visiting the node
    term_type visit_false(const term_type& x, Arg& arg)
    {
      return tr::false_();
    }

    /// \brief Visit not node
    /// \param x A term
    /// \param n A term
    /// \param arg An additional argument for the recursion
    /// \return The result of visiting the node
    term_type visit_not(const term_type& x, const term_type& n, Arg& arg)
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
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param arg An additional argument for the recursion
    /// \return The result of visiting the node
    term_type visit_and(const term_type& x, const term_type& left, const term_type& right, Arg& arg)
    {
      if (tr::is_true(left))
      {
        return super::visit(right, arg);
      }
      if (tr::is_true(right))
      {
        return super::visit(left, arg);
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
        return super::visit(left, arg);
      }
      return term_type(); // continue recursion
    }

    /// \brief Visit or node
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param arg An additional argument for the recursion
    /// \return The result of visiting the node
    term_type visit_or(const term_type& x, const term_type& left, const term_type& right, Arg& arg)
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
        return super::visit(right, arg);
      }
      if (tr::is_false(right))
      {
        return super::visit(left, arg);
      }
      if (left == right)
      {
        return super::visit(left, arg);
      }
      return term_type(); // continue recursion
    }

    /// \brief Visit imp node
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param arg An additional argument for the recursion
    /// \return The result of visiting the node
    term_type visit_imp(const term_type& x, const term_type& left, const term_type& right, Arg& arg)
    {
      if (tr::is_true(left))
      {
        return super::visit(right, arg);
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
        return super::visit(tr::not_(left), arg);
      }
      return term_type(); // continue recursion
    }

    /// \brief Applies this builder to the term x.
    /// \param x A term
    /// \return The function result
    term_type operator()(const term_type& x)
    {
      Arg tmp;
      return visit(x, tmp);
    }

    /// \brief Applies this builder to the term x, with argument arg.
    /// \param x A term
    /// \param arg An additional argument for the recursion
    /// \return The result of simplifying the term x
    term_type operator()(const term_type& x, Arg& arg)
    {
      return visit(x, arg);
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BOOLEAN_SIMPLIFY_BUILDER_H
