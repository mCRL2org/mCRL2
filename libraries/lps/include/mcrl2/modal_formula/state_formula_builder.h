// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_builder.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_STATE_FORMULA_BUILDER_H
#define MCRL2_PBES_STATE_FORMULA_BUILDER_H

#include "mcrl2/core/print.h"
#include "mcrl2/exception.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace state_formulas {

//--- start generated visitor ---//
/// \brief Modifying visitor class for expressions.
///
/// During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns state_formula(), the recursion is continued
/// in the children of this node, otherwise not.
/// An arbitrary additional argument may be passed during the recursion.
template <typename Arg = void>
struct state_formula_builder
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief Returns true if the expression is not equal to state_formula().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \param x A expression
  /// \return True if the term is not equal to state_formula()
  bool is_finished(const state_formula& x)
  {
    return x != state_formula();
  }

  /// \brief Destructor.
  virtual ~state_formula_builder()
  { }
              
  /// \brief Visit true node
  /// \return The result of visiting the node
  virtual state_formula visit_true(const true_& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit false node
  /// \return The result of visiting the node
  virtual state_formula visit_false(const false_& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit not node
  /// \return The result of visiting the node
  virtual state_formula visit_not(const not_& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit and node
  /// \return The result of visiting the node
  virtual state_formula visit_and(const and_& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit or node
  /// \return The result of visiting the node
  virtual state_formula visit_or(const or_& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit imp node
  /// \return The result of visiting the node
  virtual state_formula visit_imp(const imp& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit forall node
  /// \return The result of visiting the node
  virtual state_formula visit_forall(const forall& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit exists node
  /// \return The result of visiting the node
  virtual state_formula visit_exists(const exists& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit must node
  /// \return The result of visiting the node
  virtual state_formula visit_must(const must& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit may node
  /// \return The result of visiting the node
  virtual state_formula visit_may(const may& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit yaled node
  /// \return The result of visiting the node
  virtual state_formula visit_yaled(const yaled& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit yaled_timed node
  /// \return The result of visiting the node
  virtual state_formula visit_yaled_timed(const yaled_timed& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit delay node
  /// \return The result of visiting the node
  virtual state_formula visit_delay(const delay& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit delay_timed node
  /// \return The result of visiting the node
  virtual state_formula visit_delay_timed(const delay_timed& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit variable node
  /// \return The result of visiting the node
  virtual state_formula visit_variable(const variable& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit nu node
  /// \return The result of visiting the node
  virtual state_formula visit_nu(const nu& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }
              
  /// \brief Visit mu node
  /// \return The result of visiting the node
  virtual state_formula visit_mu(const mu& /* x */ , Arg& /* a */)
  {
    return state_formula();
  }

  /// \brief Visits the nodes of the expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals state_formula(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \param x A expression
  /// \param a An additional argument for the recursion
  /// \return The visit result
  state_formula visit(const state_formula& x, Arg& a)
  {
#ifdef MCRL2_STATE_FORMULA_BUILDER_DEBUG
  std::cerr << "<visit>" << core::pp(x) << std::endl;
#endif
    state_formula result;
    if (is_true(x))
    {
      result = visit_true(x, a);
      if (!is_finished(result))
      {
        result = true_();
      }
    }
    else if (is_false(x))
    {
      result = visit_false(x, a);
      if (!is_finished(result))
      {
        result = false_();
      }
    }
    else if (is_not(x))
    {
      result = visit_not(x, a);
      if (!is_finished(result))
      {
        result = not_(visit(not_::construct(x).operand(), a));
      }
    }
    else if (is_and(x))
    {
      result = visit_and(x, a);
      if (!is_finished(result))
      {
        result = and_(visit(and_(x).left(), a), visit(and_(x).right(), a));
      }
    }
    else if (is_or(x))
    {
      result = visit_or(x, a);
      if (!is_finished(result))
      {
        result = or_(visit(or_(x).left(), a), visit(or_(x).right(), a));
      }
    }
    else if (is_imp(x))
    {
      result = visit_imp(x, a);
      if (!is_finished(result))
      {
        result = imp(visit(imp(x).left(), a), visit(imp(x).right(), a));
      }
    }
    else if (is_forall(x))
    {
      result = visit_forall(x, a);
      if (!is_finished(result))
      {
        result = forall(forall(x).variables(), visit(forall(x).operand(), a));
      }
    }
    else if (is_exists(x))
    {
      result = visit_exists(x, a);
      if (!is_finished(result))
      {
        result = exists(exists(x).variables(), visit(exists(x).operand(), a));
      }
    }
    else if (is_must(x))
    {
      result = visit_must(x, a);
      if (!is_finished(result))
      {
        result = must(must(x).formula(), visit(must(x).operand(), a));
      }
    }
    else if (is_may(x))
    {
      result = visit_may(x, a);
      if (!is_finished(result))
      {
        result = may(may(x).formula(), visit(may(x).operand(), a));
      }
    }
    else if (is_yaled(x))
    {
      result = visit_yaled(x, a);
      if (!is_finished(result))
      {
        result = yaled();
      }
    }
    else if (is_yaled_timed(x))
    {
      result = visit_yaled_timed(x, a);
      if (!is_finished(result))
      {
        result = yaled_timed(yaled_timed(x).time_stamp());
      }
    }
    else if (is_delay(x))
    {
      result = visit_delay(x, a);
      if (!is_finished(result))
      {
        result = delay();
      }
    }
    else if (is_delay_timed(x))
    {
      result = visit_delay_timed(x, a);
      if (!is_finished(result))
      {
        result = delay_timed(delay_timed(x).time_stamp());
      }
    }
    else if (is_variable(x))
    {
      result = visit_variable(x, a);
      if (!is_finished(result))
      {
        result = variable(variable(x).name(), variable(x).arguments());
      }
    }
    else if (is_nu(x))
    {
      result = visit_nu(x, a);
      if (!is_finished(result))
      {
        result = nu(nu(x).name(), nu(x).assignments(), visit(nu(x).operand(), a));
      }
    }
    else if (is_mu(x))
    {
      result = visit_mu(x, a);
      if (!is_finished(result))
      {
        result = mu(mu(x).name(), mu(x).assignments(), visit(mu(x).operand(), a));
      }
    }
    
#ifdef MCRL2_STATE_FORMULA_BUILDER_DEBUG
  std::cerr << "<visit result>" << core::pp(result) << std::endl;
#endif
    return result;
  }
};

/// \brief Modifying visitor class for expressions.
///
/// If a visit_<node> function returns true, the recursion is continued
/// in the children of this node, otherwise not.
template <>
struct state_formula_builder<void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief Returns true if the expression is not equal to state_formula().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \param x A expression
  /// \return True if the term is not equal to state_formula()
  bool is_finished(const state_formula& x)
  {
    return x != state_formula();
  }

  /// \brief Destructor.
  virtual ~state_formula_builder()
  { }
              
  /// \brief Visit true node
  /// \return The result of visiting the node
  virtual state_formula visit_true(const true_& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit false node
  /// \return The result of visiting the node
  virtual state_formula visit_false(const false_& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit not node
  /// \return The result of visiting the node
  virtual state_formula visit_not(const not_& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit and node
  /// \return The result of visiting the node
  virtual state_formula visit_and(const and_& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit or node
  /// \return The result of visiting the node
  virtual state_formula visit_or(const or_& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit imp node
  /// \return The result of visiting the node
  virtual state_formula visit_imp(const imp& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit forall node
  /// \return The result of visiting the node
  virtual state_formula visit_forall(const forall& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit exists node
  /// \return The result of visiting the node
  virtual state_formula visit_exists(const exists& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit must node
  /// \return The result of visiting the node
  virtual state_formula visit_must(const must& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit may node
  /// \return The result of visiting the node
  virtual state_formula visit_may(const may& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit yaled node
  /// \return The result of visiting the node
  virtual state_formula visit_yaled(const yaled& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit yaled_timed node
  /// \return The result of visiting the node
  virtual state_formula visit_yaled_timed(const yaled_timed& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit delay node
  /// \return The result of visiting the node
  virtual state_formula visit_delay(const delay& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit delay_timed node
  /// \return The result of visiting the node
  virtual state_formula visit_delay_timed(const delay_timed& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit variable node
  /// \return The result of visiting the node
  virtual state_formula visit_variable(const variable& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit nu node
  /// \return The result of visiting the node
  virtual state_formula visit_nu(const nu& /* x */ )
  {
    return state_formula();
  }
              
  /// \brief Visit mu node
  /// \return The result of visiting the node
  virtual state_formula visit_mu(const mu& /* x */ )
  {
    return state_formula();
  }


  /// \brief Visits the nodes of the expression and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  state_formula visit(const state_formula& x)
  {
#ifdef MCRL2_STATE_FORMULA_BUILDER_DEBUG
  std::cerr << "<visit>" << core::pp(x) << std::endl;
#endif
    state_formula result;
    if (is_true(x))
    {
      result = visit_true(x);
      if (!is_finished(result))
      {
        result = true_();
      }
    }
    else if (is_false(x))
    {
      result = visit_false(x);
      if (!is_finished(result))
      {
        result = false_();
      }
    }
    else if (is_not(x))
    {
      result = visit_not(x);
      if (!is_finished(result))
      {
        result = not_(visit(not_::construct(x).operand()));
      }
    }
    else if (is_and(x))
    {
      result = visit_and(x);
      if (!is_finished(result))
      {
        result = and_(visit(and_(x).left()), visit(and_(x).right()));
      }
    }
    else if (is_or(x))
    {
      result = visit_or(x);
      if (!is_finished(result))
      {
        result = or_(visit(or_(x).left()), visit(or_(x).right()));
      }
    }
    else if (is_imp(x))
    {
      result = visit_imp(x);
      if (!is_finished(result))
      {
        result = imp(visit(imp(x).left()), visit(imp(x).right()));
      }
    }
    else if (is_forall(x))
    {
      result = visit_forall(x);
      if (!is_finished(result))
      {
        result = forall(forall(x).variables(), visit(forall(x).operand()));
      }
    }
    else if (is_exists(x))
    {
      result = visit_exists(x);
      if (!is_finished(result))
      {
        result = exists(exists(x).variables(), visit(exists(x).operand()));
      }
    }
    else if (is_must(x))
    {
      result = visit_must(x);
      if (!is_finished(result))
      {
        result = must(must(x).formula(), visit(must(x).operand()));
      }
    }
    else if (is_may(x))
    {
      result = visit_may(x);
      if (!is_finished(result))
      {
        result = may(may(x).formula(), visit(may(x).operand()));
      }
    }
    else if (is_yaled(x))
    {
      result = visit_yaled(x);
      if (!is_finished(result))
      {
        result = yaled();
      }
    }
    else if (is_yaled_timed(x))
    {
      result = visit_yaled_timed(x);
      if (!is_finished(result))
      {
        result = yaled_timed(yaled_timed(x).time_stamp());
      }
    }
    else if (is_delay(x))
    {
      result = visit_delay(x);
      if (!is_finished(result))
      {
        result = delay();
      }
    }
    else if (is_delay_timed(x))
    {
      result = visit_delay_timed(x);
      if (!is_finished(result))
      {
        result = delay_timed(delay_timed(x).time_stamp());
      }
    }
    else if (is_variable(x))
    {
      result = visit_variable(x);
      if (!is_finished(result))
      {
        result = variable(variable(x).name(), variable(x).arguments());
      }
    }
    else if (is_nu(x))
    {
      result = visit_nu(x);
      if (!is_finished(result))
      {
        result = nu(nu(x).name(), nu(x).assignments(), visit(nu(x).operand()));
      }
    }
    else if (is_mu(x))
    {
      result = visit_mu(x);
      if (!is_finished(result))
      {
        result = mu(mu(x).name(), mu(x).assignments(), visit(mu(x).operand()));
      }
    }
    
#ifdef MCRL2_STATE_FORMULA_BUILDER_DEBUG
  std::cerr << "<visit result>" << core::pp(result) << std::endl;
#endif
    return result;
  }
};
//--- end generated visitor ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_PBES_STATE_FORMULA_BUILDER_H
