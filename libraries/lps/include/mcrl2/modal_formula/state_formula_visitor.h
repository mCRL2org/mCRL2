// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_STATE_FORMULA_VISITOR_H
#define MCRL2_MODAL_FORMULA_STATE_FORMULA_VISITOR_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace modal {

//--- start generated visitor ---//
/// \brief Visitor class for state_formulas.
///
/// There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
template <typename Arg=void>
struct state_formula_visitor
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~state_formula_visitor()
  { }

  /// \brief Visit true_ node
  /// \return The result of visiting the node
  virtual bool visit_true_(const true_& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave true_ node
  virtual void leave_true_()
  {}

  /// \brief Visit false_ node
  /// \return The result of visiting the node
  virtual bool visit_false_(const false_& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave false_ node
  virtual void leave_false_()
  {}

  /// \brief Visit not_ node
  /// \return The result of visiting the node
  virtual bool visit_not_(const not_& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave not_ node
  virtual void leave_not_()
  {}

  /// \brief Visit and_ node
  /// \return The result of visiting the node
  virtual bool visit_and_(const and_& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave and_ node
  virtual void leave_and_()
  {}

  /// \brief Visit or_ node
  /// \return The result of visiting the node
  virtual bool visit_or_(const or_& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave or_ node
  virtual void leave_or_()
  {}

  /// \brief Visit imp node
  /// \return The result of visiting the node
  virtual bool visit_imp(const imp& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave imp node
  virtual void leave_imp()
  {}

  /// \brief Visit forall node
  /// \return The result of visiting the node
  virtual bool visit_forall(const forall& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave forall node
  virtual void leave_forall()
  {}

  /// \brief Visit exists node
  /// \return The result of visiting the node
  virtual bool visit_exists(const exists& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave exists node
  virtual void leave_exists()
  {}

  /// \brief Visit must node
  /// \return The result of visiting the node
  virtual bool visit_must(const must& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave must node
  virtual void leave_must()
  {}

  /// \brief Visit may node
  /// \return The result of visiting the node
  virtual bool visit_may(const may& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave may node
  virtual void leave_may()
  {}

  /// \brief Visit yaled node
  /// \return The result of visiting the node
  virtual bool visit_yaled(const yaled& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave yaled node
  virtual void leave_yaled()
  {}

  /// \brief Visit yaled_timed node
  /// \return The result of visiting the node
  virtual bool visit_yaled_timed(const yaled_timed& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave yaled_timed node
  virtual void leave_yaled_timed()
  {}

  /// \brief Visit delay node
  /// \return The result of visiting the node
  virtual bool visit_delay(const delay& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave delay node
  virtual void leave_delay()
  {}

  /// \brief Visit delay_timed node
  /// \return The result of visiting the node
  virtual bool visit_delay_timed(const delay_timed& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave delay_timed node
  virtual void leave_delay_timed()
  {}

  /// \brief Visit variable node
  /// \return The result of visiting the node
  virtual bool visit_variable(const variable& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave variable node
  virtual void leave_variable()
  {}

  /// \brief Visit nu node
  /// \return The result of visiting the node
  virtual bool visit_nu(const nu& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave nu node
  virtual void leave_nu()
  {}

  /// \brief Visit mu node
  /// \return The result of visiting the node
  virtual bool visit_mu(const mu& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave mu node
  virtual void leave_mu()
  {}

  /// \brief Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  /// \param a An additional argument for the recursion
  void visit(const state_formula& x, Arg& a)
  {
    if (is_true_(x))
    {
      visit_true_(true_(x), a);
      leave_true_();
    }
    else if (is_false_(x))
    {
      visit_false_(false_(x), a);
      leave_false_();
    }
    else if (is_not_(x))
    {
      bool result = visit_not_(not_(x), a);
      if (result) {
        visit(not_(x).operand(), a);
      }
      leave_not_();
    }
    else if (is_and_(x))
    {
      bool result = visit_and_(and_(x), a);
      if (result) {
        visit(and_(x).left(), a);
        visit(and_(x).right(), a);
      }
      leave_and_();
    }
    else if (is_or_(x))
    {
      bool result = visit_or_(or_(x), a);
      if (result) {
        visit(or_(x).left(), a);
        visit(or_(x).right(), a);
      }
      leave_or_();
    }
    else if (is_imp(x))
    {
      bool result = visit_imp(imp(x), a);
      if (result) {
        visit(imp(x).left(), a);
        visit(imp(x).right(), a);
      }
      leave_imp();
    }
    else if (is_forall(x))
    {
      bool result = visit_forall(forall(x), a);
      if (result) {
        visit(forall(x).operand(), a);
      }
      leave_forall();
    }
    else if (is_exists(x))
    {
      bool result = visit_exists(exists(x), a);
      if (result) {
        visit(exists(x).operand(), a);
      }
      leave_exists();
    }
    else if (is_must(x))
    {
      bool result = visit_must(must(x), a);
      if (result) {
        visit(must(x).operand(), a);
      }
      leave_must();
    }
    else if (is_may(x))
    {
      bool result = visit_may(may(x), a);
      if (result) {
        visit(may(x).operand(), a);
      }
      leave_may();
    }
    else if (is_yaled(x))
    {
      visit_yaled(yaled(x), a);
      leave_yaled();
    }
    else if (is_yaled_timed(x))
    {
      visit_yaled_timed(yaled_timed(x), a);
      leave_yaled_timed();
    }
    else if (is_delay(x))
    {
      visit_delay(delay(x), a);
      leave_delay();
    }
    else if (is_delay_timed(x))
    {
      visit_delay_timed(delay_timed(x), a);
      leave_delay_timed();
    }
    else if (is_variable(x))
    {
      visit_variable(variable(x), a);
      leave_variable();
    }
    else if (is_nu(x))
    {
      bool result = visit_nu(nu(x), a);
      if (result) {
        visit(nu(x).operand(), a);
      }
      leave_nu();
    }
    else if (is_mu(x))
    {
      bool result = visit_mu(mu(x), a);
      if (result) {
        visit(mu(x).operand(), a);
      }
      leave_mu();
    }
    
  }
};

/// \brief Visitor class for expressions.
///
/// If a visit_<node> function returns true, the recursion is continued
/// in the children of this node, otherwise not.
template <>
struct state_formula_visitor<void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~state_formula_visitor()
  { }

  /// \brief Visit true_ node
  /// \return The result of visiting the node
  virtual bool visit_true_(const true_& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave true_ node
  virtual void leave_true_()
  {}

  /// \brief Visit false_ node
  /// \return The result of visiting the node
  virtual bool visit_false_(const false_& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave false_ node
  virtual void leave_false_()
  {}

  /// \brief Visit not_ node
  /// \return The result of visiting the node
  virtual bool visit_not_(const not_& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave not_ node
  virtual void leave_not_()
  {}

  /// \brief Visit and_ node
  /// \return The result of visiting the node
  virtual bool visit_and_(const and_& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave and_ node
  virtual void leave_and_()
  {}

  /// \brief Visit or_ node
  /// \return The result of visiting the node
  virtual bool visit_or_(const or_& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave or_ node
  virtual void leave_or_()
  {}

  /// \brief Visit imp node
  /// \return The result of visiting the node
  virtual bool visit_imp(const imp& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave imp node
  virtual void leave_imp()
  {}

  /// \brief Visit forall node
  /// \return The result of visiting the node
  virtual bool visit_forall(const forall& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave forall node
  virtual void leave_forall()
  {}

  /// \brief Visit exists node
  /// \return The result of visiting the node
  virtual bool visit_exists(const exists& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave exists node
  virtual void leave_exists()
  {}

  /// \brief Visit must node
  /// \return The result of visiting the node
  virtual bool visit_must(const must& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave must node
  virtual void leave_must()
  {}

  /// \brief Visit may node
  /// \return The result of visiting the node
  virtual bool visit_may(const may& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave may node
  virtual void leave_may()
  {}

  /// \brief Visit yaled node
  /// \return The result of visiting the node
  virtual bool visit_yaled(const yaled& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave yaled node
  virtual void leave_yaled()
  {}

  /// \brief Visit yaled_timed node
  /// \return The result of visiting the node
  virtual bool visit_yaled_timed(const yaled_timed& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave yaled_timed node
  virtual void leave_yaled_timed()
  {}

  /// \brief Visit delay node
  /// \return The result of visiting the node
  virtual bool visit_delay(const delay& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave delay node
  virtual void leave_delay()
  {}

  /// \brief Visit delay_timed node
  /// \return The result of visiting the node
  virtual bool visit_delay_timed(const delay_timed& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave delay_timed node
  virtual void leave_delay_timed()
  {}

  /// \brief Visit variable node
  /// \return The result of visiting the node
  virtual bool visit_variable(const variable& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave variable node
  virtual void leave_variable()
  {}

  /// \brief Visit nu node
  /// \return The result of visiting the node
  virtual bool visit_nu(const nu& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave nu node
  virtual void leave_nu()
  {}

  /// \brief Visit mu node
  /// \return The result of visiting the node
  virtual bool visit_mu(const mu& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave mu node
  virtual void leave_mu()
  {}


  /// \brief Visits the nodes of the expression and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  void visit(const state_formula& x)
  {
    if (is_true_(x))
    {
      visit_true_(true_(x));
      leave_true_();
    }
    else if (is_false_(x))
    {
      visit_false_(false_(x));
      leave_false_();
    }
    else if (is_not_(x))
    {
      bool result = visit_not_(not_(x));
      if (result) {
        visit(not_(x).operand());
      }
      leave_not_();
    }
    else if (is_and_(x))
    {
      bool result = visit_and_(and_(x));
      if (result) {
        visit(and_(x).left());
        visit(and_(x).right());
      }
      leave_and_();
    }
    else if (is_or_(x))
    {
      bool result = visit_or_(or_(x));
      if (result) {
        visit(or_(x).left());
        visit(or_(x).right());
      }
      leave_or_();
    }
    else if (is_imp(x))
    {
      bool result = visit_imp(imp(x));
      if (result) {
        visit(imp(x).left());
        visit(imp(x).right());
      }
      leave_imp();
    }
    else if (is_forall(x))
    {
      bool result = visit_forall(forall(x));
      if (result) {
        visit(forall(x).operand());
      }
      leave_forall();
    }
    else if (is_exists(x))
    {
      bool result = visit_exists(exists(x));
      if (result) {
        visit(exists(x).operand());
      }
      leave_exists();
    }
    else if (is_must(x))
    {
      bool result = visit_must(must(x));
      if (result) {
        visit(must(x).operand());
      }
      leave_must();
    }
    else if (is_may(x))
    {
      bool result = visit_may(may(x));
      if (result) {
        visit(may(x).operand());
      }
      leave_may();
    }
    else if (is_yaled(x))
    {
      visit_yaled(yaled(x));
      leave_yaled();
    }
    else if (is_yaled_timed(x))
    {
      visit_yaled_timed(yaled_timed(x));
      leave_yaled_timed();
    }
    else if (is_delay(x))
    {
      visit_delay(delay(x));
      leave_delay();
    }
    else if (is_delay_timed(x))
    {
      visit_delay_timed(delay_timed(x));
      leave_delay_timed();
    }
    else if (is_variable(x))
    {
      visit_variable(variable(x));
      leave_variable();
    }
    else if (is_nu(x))
    {
      bool result = visit_nu(nu(x));
      if (result) {
        visit(nu(x).operand());
      }
      leave_nu();
    }
    else if (is_mu(x))
    {
      bool result = visit_mu(mu(x));
      if (result) {
        visit(mu(x).operand());
      }
      leave_mu();
    }
    
  }
};
//--- end generated visitor ---//

} // namespace modal

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_STATE_FORMULA_VISITOR_H
