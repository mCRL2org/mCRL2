// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_visitor.h
/// \brief Visitor class for pbes expressions.

#ifndef MCRL2_PBES_PBES_EXPRESSION_VISITOR_H
#define MCRL2_PBES_PBES_EXPRESSION_VISITOR_H

#include "mcrl2/core/term_traits.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief A visitor class for pbes expressions. There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
template <typename Term, typename Arg=void>
struct pbes_expression_visitor
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief The term type
  typedef typename core::term_traits<Term>::term_type term_type;

  /// \brief The data term type
  typedef typename core::term_traits<Term>::data_term_type data_term_type;

  /// \brief The variable sequence type
  typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;

  /// \brief The propositional variable instantiation type
  typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~pbes_expression_visitor()
  { }

  /// \brief Visit data_expression node
  /// \param e A term
  /// \param d A data term
  /// \return The result of visiting the node
  virtual bool visit_data_expression(const term_type& e, const data_term_type& d, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave data_expression node
  /// Leave data expression node.
  virtual void leave_data_expression()
  {}

  /// \brief Visit true node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_true(const term_type& e, Arg& /* a */)
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
  virtual bool visit_false(const term_type& e, Arg& /* a */)
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
  virtual bool visit_not(const term_type& e, const term_type& /* arg */, Arg& /* a */)
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
  virtual bool visit_and(const term_type& e, const term_type& /* left */, const term_type& /* right */, Arg& /* a */)
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
  virtual bool visit_or(const term_type& e, const term_type& /* left */, const term_type& /* right */, Arg& /* a */)
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
  virtual bool visit_imp(const term_type& e, const term_type& /* left */, const term_type& /* right */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave imp node
  /// Leave imp node.
  virtual void leave_imp()
  {}

  /// \brief Visit forall node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_forall(const term_type& e, const variable_sequence_type& /* variables */, const term_type& /* expression */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave forall node
  /// Leave forall node.
  virtual void leave_forall()
  {}

  /// \brief Visit exists node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_exists(const term_type& e, const variable_sequence_type& /* variables */, const term_type& /* expression */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave exists node
  /// Leave exists node.
  virtual void leave_exists()
  {}

  /// \brief Visit propositional_variable node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_propositional_variable(const term_type& e, const propositional_variable_type& /* v */, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave propositional_variable node
  /// Leave propositional variable node.
  virtual void leave_propositional_variable()
  {}

  /// \brief Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param e A term
  /// \param a An additional argument for the recursion
  void visit(const term_type& e, Arg& a)
  {
    typedef core::term_traits<Term> tr;

    if (tr::is_data(e)) {
      visit_data_expression(e, tr::term2dataterm(e), a);
      leave_data_expression();
    } else if (tr::is_true(e)) {
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
    } else if (tr::is_forall(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      bool result = visit_forall(e, qvars, qexpr, a);
      if (result) {
        visit(qexpr, a);
      }
      leave_forall();
    } else if (tr::is_exists(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      bool result = visit_exists(e, qvars, qexpr, a);
      if (result) {
        visit(qexpr, a);
      }
      leave_exists();
    }
    else if(tr::is_prop_var(e)) {
      visit_propositional_variable(e, e, a);
      leave_propositional_variable();
    }
  }
};

/// \brief Visitor class for visiting the nodes of a pbes expression.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
template <typename Term>
struct pbes_expression_visitor<Term, void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief The term type
  typedef typename core::term_traits<Term>::term_type term_type;

  /// \brief The data term type
  typedef typename core::term_traits<Term>::data_term_type data_term_type;

  /// \brief The variable sequence type
  typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;

  /// \brief The propositional variable instantiation type
  typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~pbes_expression_visitor()
  { }

  /// \brief Visit data_expression node
  /// \param e A term
  /// \param d A data term
  /// \return The result of visiting the node
  virtual bool visit_data_expression(const term_type& e, const data_term_type& d)
  {
    return continue_recursion;
  }

  /// \brief Leave data_expression node
  /// Leave data expression node.
  virtual void leave_data_expression()
  {}

  /// \brief Visit true node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_true(const term_type& e)
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
  virtual bool visit_false(const term_type& e)
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
  virtual bool visit_not(const term_type& e, const term_type& /* arg */)
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
  virtual bool visit_and(const term_type& e, const term_type& /* left */, const term_type& /* right */)
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
  virtual bool visit_or(const term_type& e, const term_type& /* left */, const term_type& /* right */)
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
  virtual bool visit_imp(const term_type& e, const term_type& /* left */, const term_type& /* right */)
  {
    return continue_recursion;
  }

  /// \brief Leave imp node
  /// Leave imp node.
  virtual void leave_imp()
  {}

  /// \brief Visit forall node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_forall(const term_type& e, const variable_sequence_type& /* variables */, const term_type& /* expression */)
  {
    return continue_recursion;
  }

  /// \brief Leave forall node
  /// Leave forall node.
  virtual void leave_forall()
  {}

  /// \brief Visit exists node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_exists(const term_type& e, const variable_sequence_type& /* variables */, const term_type& /* expression */)
  {
    return continue_recursion;
  }

  /// \brief Leave exists node
  /// Leave exists node.
  virtual void leave_exists()
  {}

  /// \brief Visit propositional_variable node
  /// \param e A term
  /// \return The result of visiting the node
  virtual bool visit_propositional_variable(const term_type& e, const propositional_variable_type& /* v */)
  {
    return continue_recursion;
  }

  /// \brief Leave propositional_variable node
  /// Leave propositional variable node.
  virtual void leave_propositional_variable()
  {}

  /// \brief Visits the nodes of the pbes expressionnd calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param e A term
  void visit(const term_type& e)
  {
    typedef core::term_traits<Term> tr;

    if (tr::is_data(e)) {
      visit_data_expression(e, tr::term2dataterm(e));
      leave_data_expression();
    } else if (tr::is_true(e)) {
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
    } else if (tr::is_forall(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      bool result = visit_forall(e, qvars, qexpr);
      if (result) {
        visit(qexpr);
      }
      leave_forall();
    } else if (tr::is_exists(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      bool result = visit_exists(e, qvars, qexpr);
      if (result) {
        visit(qexpr);
      }
      leave_exists();
    }
    else if(tr::is_prop_var(e)) {
      visit_propositional_variable(e, e);
      leave_propositional_variable();
    }
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_VISITOR_H
