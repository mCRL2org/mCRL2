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
#include <string>
#include <utility>
#include "mcrl2/utilities/optimized_boolean_operators.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/bes/print.h"

namespace mcrl2
{

namespace bes
{

namespace detail
{

/// \brief Visitor class for visiting the nodes of a boolean expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
/// An arbitrary additional argument may be passed during the recursion.
// TODO: rebuilding expressions with ATerms is very expensive. So it is probably
// more efficient to  check if the children of a node have actually changed,
// before rebuilding it.
template <typename Term, typename Arg = void>
struct boolean_expr_builder
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief The term type
  typedef typename core::term_traits<Term>::term_type term_type;

  /// \brief The variable type
  typedef typename core::term_traits<Term>::variable_type variable_type;

  /// \brief Returns true if the term is not equal to term_type().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \param x A term
  /// \return True if the term is not equal to term_type()
  bool is_finished(const term_type& x)
  {
    return x != term_type();
  }

  /// \brief Destructor.
  virtual ~boolean_expr_builder()
  { }

  /// \brief Visit true node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_true(const term_type& /* x */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit false node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_false(const term_type& /* x */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit not node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_not(const term_type& /* x */, const term_type& /* arg */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit and node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_and(const term_type& /* x */, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit or node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_or(const term_type& /* x */, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit imp node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_imp(const term_type& /* x */, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit var node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_var(const term_type& /* x */, const variable_type& /* v */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit unknown node
  /// By default a mcrl2::runtime_error exception will be generated.
  /// \param e A term
  /// \return The result of visiting the node
  virtual term_type visit_unknown(const term_type& e, Arg& /* arg */)
  {
    throw mcrl2::runtime_error(std::string("error in boolean_expr_builder::visit() : unknown boolean expression ") + e.to_string());
    return term_type();
  }

  /// \brief Visits the nodes of the boolean expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals term_type(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \param e A term
  /// \param arg1 An additional argument for the recursion
  /// \return The visit result
  term_type visit(term_type e, Arg& arg1)
  {
    typedef core::term_traits<term_type> tr;

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
    mCRL2log(debug) << "<visit>" << tr::pp(e) << std::endl;
#endif

    term_type result;

    if (tr::is_true(e))
    {
      result = visit_true(e, arg1);
      if (!is_finished(result))
      {
        result = e;
      }
    }
    else if (tr::is_false(e))
    {
      result = visit_false(e, arg1);
      if (!is_finished(result))
      {
        result = e;
      }
    }
    else if (tr::is_not(e))
    {
      term_type n = tr::arg(e);
      result = visit_not(e, n, arg1);
      if (!is_finished(result))
      {
        result = utilities::optimized_not(visit(n, arg1));
      }
    }
    else if (tr::is_and(e))
    {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_and(e, l, r, arg1);
      if (!is_finished(result))
      {
        result = utilities::optimized_and(visit(l, arg1), visit(r, arg1));
      }
    }
    else if (tr::is_or(e))
    {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_or(e, l, r, arg1);
      if (!is_finished(result))
      {
        result = utilities::optimized_or(visit(l, arg1), visit(r, arg1));
      }
    }
    else if (tr::is_imp(e))
    {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_imp(e, l, r, arg1);
      if (!is_finished(result))
      {
        result = utilities::optimized_imp(visit(l, arg1), visit(r, arg1));
      }
    }
    else if (tr::is_variable(e))
    {
      result = visit_var(e, e, arg1);
      if (!is_finished(result))
      {
        result = e;
      }
    }
    else
    {
      result = visit_unknown(e, arg1);
      if (!is_finished(result))
      {
        result = e;
      }
    }

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
    mCRL2log(debug) << "<visit result>" << tr::pp(result) << std::endl;
#endif

    return result;
  }
};

/// \brief Visitor class for visiting the nodes of a boolean expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
template <typename Term>
struct boolean_expr_builder<Term, void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief The term type
  typedef typename core::term_traits<Term>::term_type term_type;

  /// \brief Returns true if the term is not equal to term_type().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \param x A term
  /// \return True if the term is not equal to term_type()
  bool is_finished(const term_type& x)
  {
    return x != term_type();
  }

  /// \brief Destructor.
  virtual ~boolean_expr_builder()
  { }

  /// \brief Visit true node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_true(const term_type& /* x */)
  {
    return term_type();
  }

  /// \brief Visit false node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_false(const term_type& /* x */)
  {
    return term_type();
  }

  /// \brief Visit not node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_not(const term_type& /* x */, const term_type& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit and node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_and(const term_type& /* x */, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// \brief Visit or node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_or(const term_type& /* x */, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// \brief Visit imp node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_imp(const term_type& /* x */, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// \brief Visit unknown node
  /// By default a mcrl2::runtime_error exception will be generated.
  /// \param e A term
  /// \return The result of visiting the node
  virtual term_type visit_unknown(const term_type& e)
  {
    throw mcrl2::runtime_error(std::string("error in boolean_expr_builder::visit() : unknown boolean expression ") + e.to_string());
    return term_type();
  }

  /// \brief Visits the nodes of the boolean expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals term_type(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \param e A term
  /// \return The visit result
  term_type visit(term_type e)
  {
    /// \brief The term traits type
    typedef core::term_traits<term_type> tr;

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
    mCRL2log(debug) << "<visit>" << tr::pp(e) << " " << e << std::endl;
#endif

    term_type result;

    if (tr::is_true(e))
    {
      result = visit_true(e);
      if (!is_finished(result))
      {
        result = e;
      }
    }
    else if (tr::is_false(e))
    {
      result = visit_false(e);
      if (!is_finished(result))
      {
        result = e;
      }
    }
    else if (tr::is_not(e))
    {
      term_type n = tr::arg(e);
      result = visit_not(e, n);
      if (!is_finished(result))
      {
        result = utilities::optimized_not(visit(n));
      }
    }
    else if (tr::is_and(e))
    {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_and(e, l, r);
      if (!is_finished(result))
      {
        result = utilities::optimized_and(visit(l), visit(r));
      }
    }
    else if (tr::is_or(e))
    {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_or(e, l, r);
      if (!is_finished(result))
      {
        result = utilities::optimized_or(visit(l), visit(r));
      }
    }
    else if (tr::is_imp(e))
    {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_imp(e, l, r);
      if (!is_finished(result))
      {
        result = utilities::optimized_imp(visit(l), visit(r));
      }
    }
    else
    {
      result = visit_unknown(e);
      if (!is_finished(result))
      {
        result = e;
      }
    }

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
    mCRL2log(debug) << "<visit result>" << tr::pp(result) << " " << result << std::endl;
#endif

    return result;
  }
};

struct no_substitution
{
};

// Simplifying rewriter.
template <typename Term, typename Arg = no_substitution>
struct boolean_simplify_builder: public boolean_expr_builder<Term, Arg>
{
  typedef boolean_expr_builder<Term, Arg> super;
  typedef Arg                                argument_type;
  typedef typename super::term_type          term_type;
  typedef core::term_traits<Term> tr;

  /// \brief Visit true node
  /// \param x A term
  /// \param arg An additional argument for the recursion
  /// \return The result of visiting the node
  term_type visit_true(const term_type& /* x */, Arg& /* arg */)
  {
    return tr::true_();
  }

  /// \brief Visit false node
  /// \param x A term
  /// \param arg An additional argument for the recursion
  /// \return The result of visiting the node
  term_type visit_false(const term_type& /* x */, Arg& /* arg */)
  {
    return tr::false_();
  }

  /// \brief Visit not node
  /// \param x A term
  /// \param n A term
  /// \param arg An additional argument for the recursion
  /// \return The result of visiting the node
  term_type visit_not(const term_type& /* x */, const term_type& n, Arg& /* arg */)
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
  term_type visit_and(const term_type& /* x */, const term_type& left, const term_type& right, Arg& arg)
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
  term_type visit_or(const term_type& /* x */, const term_type& left, const term_type& right, Arg& arg)
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
  term_type visit_imp(const term_type& /* x */, const term_type& left, const term_type& right, Arg& arg)
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
    return super::visit(x, tmp);
  }

  /// \brief Applies this builder to the term x, with argument arg.
  /// \param x A term
  /// \param arg An additional argument for the recursion
  /// \return The result of simplifying the term x
  term_type operator()(const term_type& x, Arg& arg)
  {
    return super::visit(x, arg);
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BOOLEAN_SIMPLIFY_BUILDER_H
