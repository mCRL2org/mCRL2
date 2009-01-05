// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/boolean_expression_builder.h
/// \brief Visitor class for rebuilding a boolean expression.

#ifndef MCRL2_PBES_BOOLEAN_EXPRESSION_BUILDER_H
#define MCRL2_PBES_BOOLEAN_EXPRESSION_BUILDER_H

#include "mcrl2/exception.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/optimized_boolean_operators.h"

namespace mcrl2 {

namespace bes {

/// \brief Visitor class for visiting the nodes of a boolean expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
/// An arbitrary additional argument may be passed during the recursion.
// TODO: rebuilding expressions with ATerms is very expensive. So it is probably
// more efficient to  check if the children of a node have actually changed,
// before rebuilding it.
template <typename Term, typename Arg = void>
struct boolean_expression_builder
{
  typedef Arg argument_type;
  typedef typename core::term_traits<Term>::term_type term_type;
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
  virtual ~boolean_expression_builder()
  { }

  /// \brief Visit true node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_true(const term_type& x, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit false node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_false(const term_type& x, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit not node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_not(const term_type& x, const term_type& /* arg */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit and node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_and(const term_type& x, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit or node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_or(const term_type& x, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit imp node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_imp(const term_type& x, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit var node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_var(const term_type& x, const variable_type& /* v */, Arg& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit unknown node
  /// By default a mcrl2::runtime_error exception will be generated.
  /// \param e A term
  /// \return The result of visiting the node
  virtual term_type visit_unknown(const term_type& e, Arg& /* arg */)
  {
    throw mcrl2::runtime_error(std::string("error in boolean_expression_builder::visit() : unknown boolean expression ") + e.to_string());
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
std::cerr << "<visit>" << tr::pp(e) << std::endl;
#endif

    term_type result;

    if (tr::is_true(e)) {
      result = visit_true(e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_false(e)) {
      result = visit_false(e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_not(e)) {
      term_type n = tr::arg(e);
      result = visit_not(e, n, arg1);
      if (!is_finished(result)) {
        result = core::optimized_not(visit(n, arg1));
      }
    } else if (tr::is_and(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_and(e, l, r, arg1);
      if (!is_finished(result)) {
        result = core::optimized_and(visit(l, arg1), visit(r, arg1));
      }
    } else if (tr::is_or(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_or(e, l, r, arg1);
      if (!is_finished(result)) {
        result = core::optimized_or(visit(l, arg1), visit(r, arg1));
      }
    } else if (tr::is_imp(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_imp(e, l, r, arg1);
      if (!is_finished(result)) {
        result = core::optimized_imp(visit(l, arg1), visit(r, arg1));
      }
    } else if (tr::is_variable(e)) {
      result = visit_var(e, e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    }
    else {
      result = visit_unknown(e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    }

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
std::cerr << "<visit result>" << tr::pp(result) << std::endl;
#endif

    return result;
  }
};

/// \brief Visitor class for visiting the nodes of a boolean expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
template <typename Term>
struct boolean_expression_builder<Term, void>
{
  typedef void argument_type;
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
  virtual ~boolean_expression_builder()
  { }

  /// \brief Visit true node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_true(const term_type& x)
  {
    return term_type();
  }

  /// \brief Visit false node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_false(const term_type& x)
  {
    return term_type();
  }

  /// \brief Visit not node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_not(const term_type& x, const term_type& /* arg */)
  {
    return term_type();
  }

  /// \brief Visit and node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_and(const term_type& x, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// \brief Visit or node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_or(const term_type& x, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// \brief Visit imp node
  /// \param x A term
  /// \return The result of visiting the node
  virtual term_type visit_imp(const term_type& x, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// \brief Visit unknown node
  /// By default a mcrl2::runtime_error exception will be generated.
  /// \param e A term
  /// \return The result of visiting the node
  virtual term_type visit_unknown(const term_type& e)
  {
    throw mcrl2::runtime_error(std::string("error in boolean_expression_builder::visit() : unknown boolean expression ") + e.to_string());
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
    typedef core::term_traits<term_type> tr;

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
std::cerr << "<visit>" << tr::pp(e) << " " << e << std::endl;
#endif

    term_type result;

    if (tr::is_true(e)) {
      result = visit_true(e);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_false(e)) {
      result = visit_false(e);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_not(e)) {
      term_type n = tr::arg(e);
      result = visit_not(e, n);
      if (!is_finished(result)) {
        result = core::optimized_not(visit(n));
      }
    } else if (tr::is_and(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_and(e, l, r);
      if (!is_finished(result)) {
        result = core::optimized_and(visit(l), visit(r));
      }
    } else if (tr::is_or(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_or(e, l, r);
      if (!is_finished(result)) {
        result = core::optimized_or(visit(l), visit(r));
      }
    } else if (tr::is_imp(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_imp(e, l, r);
      if (!is_finished(result)) {
        result = core::optimized_imp(visit(l), visit(r));
      }
    }
    else {
      result = visit_unknown(e);
      if (!is_finished(result)) {
        result = e;
      }
    }

#ifdef MCRL2_BOOLEAN_EXPRESSION_BUILDER_DEBUG
std::cerr << "<visit result>" << tr::pp(result) << " " << result << std::endl;
#endif

    return result;
  }
};

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_PBES_BOOLEAN_EXPRESSION_BUILDER_H
