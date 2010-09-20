// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_BOOLEAN_EXPRESSION_VISITOR_H
#define MCRL2_PBES_BOOLEAN_EXPRESSION_VISITOR_H

#include "mcrl2/exception.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/bes/boolean_expression.h"

namespace mcrl2 {

namespace bes {

/// \brief A visitor class for boolean expressions. There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
template <typename Arg=void>
struct boolean_expression_visitor
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief The term type
  typedef typename core::term_traits<boolean_expression>::term_type term_type;

  /// \brief The variable instantiation type
  typedef typename core::term_traits<boolean_expression>::variable_type variable_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~boolean_expression_visitor()
  { }

  /// \brief Visit true node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_true(const term_type& /* e */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave true node
  /// Leave true node.
  virtual void leave_true()
  {}

  /// \brief Visit false node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_false(const term_type& /* e */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave false node
  /// Leave false node.
  virtual void leave_false()
  {}

  /// \brief Visit not node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_not(const term_type& /* e */, const term_type& /* arg */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave not node
  /// Leave not node.
  virtual void leave_not()
  {}

  /// \brief Visit and node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_and(const term_type& /* e */, const term_type& /* left */, const term_type& /* right */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave and node
  /// Leave and node.
  virtual void leave_and()
  {}

  /// \brief Visit or node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_or(const term_type& /* e */, const term_type& /* left */, const term_type& /* right */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave or node
  /// Leave or node.
  virtual void leave_or()
  {}

  /// \brief Visit imp node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_imp(const term_type& /* e */, const term_type& /* left */, const term_type& /* right */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave imp node
  /// Leave imp node.
  virtual void leave_imp()
  {}

  /// \brief Visit variable node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_var(const term_type& /* e */, const variable_type& /* v */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave variable node
  /// Leave variable node.
  virtual void leave_var()
  {}

  /// \brief Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param e A term
  /// \param a An additional argument for the recursion
  void visit(const term_type& e, Arg& a)
  {
    typedef core::term_traits<boolean_expression> tr;

    if (tr::is_true(e)) {
      visit_true(e, a);
      leave_true();
    } else if (tr::is_false(e)) {
      visit_false(e, a);
      leave_false();
    } else if (tr::is_not(e)) {
      term_type n = tr::arg(e);
      bool result = visit_not(e, n, a);
      if (result) {
        visit(n, a);
      }
      leave_not();
    } else if (tr::is_and(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      bool result = visit_and(e, l, r, a);
      if (result) {
        visit(l, a);
        visit(r, a);
      }
      leave_and();
    } else if (tr::is_or(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      bool result = visit_or(e, l, r, a);
      if (result) {
        visit(l, a);
        visit(r, a);
      }
      leave_or();
    } else if (tr::is_imp(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      bool result = visit_imp(e, l, r, a);
      if (result) {
        visit(l, a);
        visit(r, a);
      }
      leave_imp();
    }
    else if(tr::is_variable(e)) {
      visit_var(e, e, a);
      leave_var();
    }
  }
};

/// \brief A visitor class for boolean expressions. There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
template <>
struct boolean_expression_visitor<void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief The term type
  typedef core::term_traits<boolean_expression>::term_type term_type;

  /// \brief The variable instantiation type
  typedef core::term_traits<boolean_expression>::variable_type variable_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~boolean_expression_visitor()
  { }

  /// \brief Visit true node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_true(const term_type& /* e */)
  {
    return continue_recursion;
  }

  /// \brief Leave true node
  /// Leave true node.
  virtual void leave_true()
  {}

  /// \brief Visit false node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_false(const term_type& /* e */)
  {
    return continue_recursion;
  }

  /// \brief Leave false node
  /// Leave false node.
  virtual void leave_false()
  {}

  /// \brief Visit not node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_not(const term_type& /* e */, const term_type& /* arg */)
  {
    return continue_recursion;
  }

  /// \brief Leave not node
  /// Leave not node.
  virtual void leave_not()
  {}

  /// \brief Visit and node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_and(const term_type& /* e */, const term_type& /* left */, const term_type& /* right */)
  {
    return continue_recursion;
  }

  /// \brief Leave and node
  /// Leave and node.
  virtual void leave_and()
  {}

  /// \brief Visit or node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_or(const term_type& /* e */, const term_type& /* left */, const term_type& /* right */)
  {
    return continue_recursion;
  }

  /// \brief Leave or node
  /// Leave or node.
  virtual void leave_or()
  {}

  /// \brief Visit imp node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_imp(const term_type& /* e */, const term_type& /* left */, const term_type& /* right */)
  {
    return continue_recursion;
  }

  /// \brief Leave imp node
  /// Leave imp node.
  virtual void leave_imp()
  {}

  /// \brief Visit variable node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_var(const term_type& /* e */, const variable_type& /* v */)
  {
    return continue_recursion;
  }

  /// \brief Leave variable node
  /// Leave variable node.
  virtual void leave_var()
  {}

  /// \brief Visits the nodes of the pbes expressionnd calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param e A term
  void visit(const term_type& e)
  {
    typedef core::term_traits<boolean_expression> tr;

    if (tr::is_true(e)) {
      visit_true(e);
      leave_true();
    } else if (tr::is_false(e)) {
      visit_false(e);
      leave_false();
    } else if (tr::is_not(e)) {
      term_type n = tr::arg(e);
      bool result = visit_not(e, n);
      if (result) {
        visit(n);
      }
      leave_not();
    } else if (tr::is_and(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      bool result = visit_and(e, l, r);
      if (result) {
        visit(l);
        visit(r);
      }
      leave_and();
    } else if (tr::is_or(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      bool result = visit_or(e, l, r);
      if (result) {
        visit(l);
        visit(r);
      }
      leave_or();
    } else if (tr::is_imp(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      bool result = visit_imp(e, l, r);
      if (result) {
        visit(l);
        visit(r);
      }
      leave_imp();
    }
    else if(tr::is_variable(e)) {
      visit_var(e, e);
      leave_var();
    }
  }
};

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_PBES_BOOLEAN_EXPRESSION_VISITOR_H
