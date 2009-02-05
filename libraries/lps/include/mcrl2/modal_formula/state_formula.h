// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_STATE_FORMULA_H
#define MCRL2_MODAL_STATE_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/data/data_variable.h"

namespace mcrl2 {

namespace modal {

///////////////////////////////////////////////////////////////////////////////
// state_formula
/// \brief state formula
//<StateFrm>     ::= <DataExpr>
//                 | StateTrue
//                 | StateFalse
//                 | StateNot(<StateFrm>)
//                 | StateAnd(<StateFrm>, <StateFrm>)
//                 | StateOr(<StateFrm>, <StateFrm>)
//                 | StateImp(<StateFrm>, <StateFrm>)
//                 | StateForall(<DataVarId>+, <StateFrm>)
//                 | StateExists(<DataVarId>+, <StateFrm>)
//                 | StateMust(<RegFrm>, <StateFrm>)
//                 | StateMay(<RegFrm>, <StateFrm>)
//                 | StateYaled
//                 | StateYaledTimed(<DataExpr>)
//                 | StateDelay
//                 | StateDelayTimed(<DataExpr>)
//                 | StateVar(<String>, <DataExpr>*)
//                 | StateNu(<String>, <DataVarIdInit>*, <StateFrm>)
//                 | StateMu(<String>, <DataVarIdInit>*, <StateFrm>)
class state_formula: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor
    state_formula()
      : atermpp::aterm_appl(mcrl2::core::detail::constructStateFrm())
    {}

    /// \brief Constructor
    /// \param t A term
    state_formula(ATermAppl t)
      : atermpp::aterm_appl(atermpp::aterm_appl(t))
    {
      assert(mcrl2::core::detail::check_rule_StateFrm(m_term));
    }

    /// \brief Constructor
    /// \param t A term
    state_formula(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_rule_StateFrm(m_term));
    }

    /// \brief Returns true if the formula is timed.
    /// \return True if the formula is timed.
    bool has_time() const;

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    state_formula substitute(Substitution f) const
    {
      return state_formula(f(atermpp::aterm(*this)));
    }
};

///////////////////////////////////////////////////////////////////////////////
// state_formula_list
/// \brief Read-only singly linked list of state formulas
typedef atermpp::term_list<state_formula> state_formula_list;

/// Accessor functions and predicates for state formulas.
namespace state_frm
{
  /// \brief Make the value false
  /// \return The value \p false
  inline
  state_formula false_()
  {
    return state_formula(core::detail::gsMakeStateFalse());
  }

  /// \brief Make the value true
  /// \return The value \p true
  inline
  state_formula true_()
  {
    return state_formula(core::detail::gsMakeStateTrue());
  }

  /// \brief Make a negation
  /// \param p A modal formula
  /// \return The value <tt>!p</tt>
  inline
  state_formula not_(state_formula p)
  {
    return state_formula(core::detail::gsMakeStateNot(p));
  }

  /// \brief Make a conjunction
  /// \param p A modal formula
  /// \param q A modal formula
  /// \return The value <tt>p && q</tt>
  inline
  state_formula and_(state_formula p, state_formula q)
  {
    return state_formula(core::detail::gsMakeStateAnd(p,q));
  }

  /// \brief Make a disjunction
  /// \param p A modal formula
  /// \param q A modal formula
  /// \return The value <tt>p || q</tt>
  inline
  state_formula or_(state_formula p, state_formula q)
  {
    return state_formula(core::detail::gsMakeStateOr(p,q));
  }

  /// \brief Make an implication
  /// \param p A modal formula
  /// \param q A modal formula
  /// \return The value <tt>p => q</tt>
  inline
  state_formula imp(state_formula p, state_formula q)
  {
    return state_formula(core::detail::gsMakeStateImp(p,q));
  }

  /// \brief Make an existential quantification
  /// \pre l may not be empty
  /// \param l A sequence of data variables
  /// \param p A modal formula
  /// \return The value <tt>exists l.p</tt>
  inline
  state_formula exists(data::data_variable_list l, state_formula p)
  {
    assert(!l.empty());
    return state_formula(core::detail::gsMakeStateExists(l, p));
  }

  /// \brief Make a universal quantification
  /// \pre l may not be empty
  /// \param l A sequence of data variables
  /// \param p A modal formula
  /// \return The value <tt>forall l.p</tt>
  inline
  state_formula forall(data::data_variable_list l, state_formula p)
  {
    assert(!l.empty());
    return state_formula(core::detail::gsMakeStateForall(l, p));
  }

  /// \brief Returns must applied to r and p
  /// \param r A regular formula
  /// \param p A modal formula
  /// \return must applied to r and p
  inline
  state_formula must(regular_formula r, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateMust(r, p));
  }

  /// \brief Returns may applied to r and p
  /// \param r A regular formula
  /// \param p A modal formula
  /// \return may applied to r and p
  inline
  state_formula may(regular_formula r, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateMay(r, p));
  }

  /// \brief Returns yaled
  /// \return yaled
  inline
  state_formula yaled()
  {
    return state_formula(core::detail::gsMakeStateYaled());
  }

  /// \brief Returns yaled(t)
  /// \param t A data expression
  /// \return yaled(t)
  inline
  state_formula yaled_timed(data::data_expression t)
  {
    return state_formula(core::detail::gsMakeStateYaledTimed(t));
  }

  /// \brief Returns delay
  /// \return delay
  inline
  state_formula delay()
  {
    return state_formula(core::detail::gsMakeStateDelay());
  }

  /// \brief Returns delay(t)
  /// \param t A data expression
  /// \return delay(t)
  inline
  state_formula delay_timed(data::data_expression t)
  {
    return state_formula(core::detail::gsMakeStateDelayTimed(t));
  }

  /// \brief Returns a variable with the given name and arguments
  /// \param name A
  /// \param l A sequence of data expressions
  /// \return a variable with the given name and arguments
  inline
  state_formula var(core::identifier_string name, data::data_expression_list l)
  {
    return state_formula(core::detail::gsMakeStateVar(name, l));
  }

  /// \brief Returns a mu expression
  /// \param name A
  /// \param l A sequence of assignments to data variables
  /// \param p A modal formula
  /// \return a mu expression
  inline
  state_formula mu(core::identifier_string name, data::data_assignment_list l, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateMu(name, l, p));
  }

  /// \brief Returns a nu expression
  /// \param name A
  /// \param l A sequence of assignments to data variables
  /// \param p A modal formula
  /// \return a nu expression
  inline
  state_formula nu(core::identifier_string name, data::data_assignment_list l, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateNu(name, l, p));
  }

  /// \brief Returns true if the term t is a data expression
  /// \param t A term
  /// \return True if the term t is a data expression
  inline bool is_data(atermpp::aterm_appl t)
  { return core::detail::gsIsDataExpr(t); }

  /// \brief Test for the value true
  /// \param t A term
  /// \return True if it is the value \p true
  inline bool is_true(atermpp::aterm_appl t)
  { return core::detail::gsIsStateTrue(t); }

  /// \brief Test for the value false
  /// \param t A term
  /// \return True if it is the value \p false
  inline bool is_false(atermpp::aterm_appl t)
  { return core::detail::gsIsStateFalse(t); }

  /// \brief Test for a negation
  /// \param t A term
  /// \return True if it is a negation
  inline bool is_not(atermpp::aterm_appl t)
  { return core::detail::gsIsStateNot(t); }

  /// \brief Test for a conjunction
  /// \param t A term
  /// \return True if it is a conjunction
  inline bool is_and(atermpp::aterm_appl t)
  { return core::detail::gsIsStateAnd(t); }

  /// \brief Test for a disjunction
  /// \param t A term
  /// \return True if it is a disjunction
  inline bool is_or(atermpp::aterm_appl t)
  { return core::detail::gsIsStateOr(t); }

  /// \brief Test for an implication
  /// \param t A term
  /// \return True if it is an implication
  inline bool is_imp(atermpp::aterm_appl t)
  { return core::detail::gsIsStateImp(t); }

  /// \brief Test for an universal quantification
  /// \param t A term
  /// \return True if it is a universal quantification
  inline bool is_forall(atermpp::aterm_appl t)
  { return core::detail::gsIsStateForall(t); }

  /// \brief Test for an existential quantification
  /// \param t A term
  /// \return True if it is an existential quantification
  inline bool is_exists(atermpp::aterm_appl t)
  { return core::detail::gsIsStateExists(t); }

  /// \brief Returns true if the term t is a must expression
  /// \param t A term
  /// \return True if the term t is a must expression
  inline bool is_must(atermpp::aterm_appl t)
  { return core::detail::gsIsStateMust(t); }

  /// \brief Returns true if the term t is a must expression
  /// \param t A term
  /// \return True if the term t is a must expression
  inline bool is_may(atermpp::aterm_appl t)
  { return core::detail::gsIsStateMay(t); }

  /// \brief Returns true if the term t is a delay expression
  /// \param t A term
  /// \return True if the term t is a delay expression
  inline bool is_delay(atermpp::aterm_appl t)
  { return core::detail::gsIsStateDelay(t); }

  /// \brief Returns true if the term t is a timed delay expression
  /// \param t A term
  /// \return True if the term t is a timed delay expression
  inline bool is_delay_timed(atermpp::aterm_appl t)
  { return core::detail::gsIsStateDelayTimed(t); }

  /// \brief Returns true if the term t is a yaled expression
  /// \param t A term
  /// \return True if the term t is a yaled expression
  inline bool is_yaled(atermpp::aterm_appl t)
  { return core::detail::gsIsStateYaled(t); }

  /// \brief Returns true if the term t is a timed yaled expression
  /// \param t A term
  /// \return True if the term t is a timed yaled expression
  inline bool is_yaled_timed(atermpp::aterm_appl t)
  { return core::detail::gsIsStateYaledTimed(t); }

  /// \brief Returns true if the term t is a variable expression
  /// \param t A term
  /// \return True if the term t is a variable expression
  inline bool is_var(atermpp::aterm_appl t)
  { return core::detail::gsIsStateVar(t); }

  /// \brief Returns true if the term t is a nu expression
  /// \param t A term
  /// \return True if the term t is a nu expression
  inline bool is_nu(atermpp::aterm_appl t)
  { return core::detail::gsIsStateNu(t); }

  /// \brief Returns true if the term t is a mu expression
  /// \param t A term
  /// \return True if the term t is a mu expression
  inline bool is_mu(atermpp::aterm_appl t)
  { return core::detail::gsIsStateMu(t); }

  /// \brief Returns the argument of a data expression
  /// \param t A modal formula
  /// \return The argument of a data expression
  inline
  data::data_expression val(state_formula t)
  {
    assert(core::detail::gsIsDataExpr(t));
    return t;
  }

  /// \brief Returns the state formula argument of an expression of type
  /// not, mu, nu, exists, forall, must or may.
  /// \param t A modal formula
  /// \return The state formula argument of an expression of type
  inline
  state_formula arg(state_formula t)
  {
    if (core::detail::gsIsStateNot(t))
    {
      return atermpp::arg1(t);
    }
    if (core::detail::gsIsStateMu(t) || core::detail::gsIsStateNu(t))
    {
      return atermpp::arg3(t);
    }
    assert(core::detail::gsIsStateExists(t) ||
           core::detail::gsIsStateForall(t) ||
           core::detail::gsIsStateMust(t)   ||
           core::detail::gsIsStateMay(t)
          );
    return atermpp::arg2(t);
  }

  /// \brief Returns the left hand side of an expression of type and/or/imp
  /// \param t A modal formula
  /// \return The left hand side of an expression of type and/or/imp
  inline
  state_formula left(state_formula t)
  {
    assert(core::detail::gsIsStateAnd(t) || core::detail::gsIsStateOr(t) || core::detail::gsIsStateImp(t));
    return atermpp::arg1(t);
  }

  /// \brief Returns the right hand side of an expression of type and/or/imp.
  /// \param t A modal formula
  /// \return The right hand side of an expression of type and/or/imp.
  inline
  state_formula right(state_formula t)
  {
    assert(core::detail::gsIsStateAnd(t) || core::detail::gsIsStateOr(t) || core::detail::gsIsStateImp(t));
    return atermpp::arg2(t);
  }

  /// \brief Returns the variables of a quantification expression
  /// \param t A modal formula
  /// \return The variables of a quantification expression
  inline
  data::data_variable_list var(state_formula t)
  {
    assert(core::detail::gsIsStateExists(t) || core::detail::gsIsStateForall(t));
    return atermpp::list_arg1(t);
  }

  /// \brief Returns the time of a delay or yaled expression
  /// \param t A modal formula
  /// \return The time of a delay or yaled expression
  inline
  data::data_expression time(state_formula t)
  {
    assert(core::detail::gsIsStateDelayTimed(t) || core::detail::gsIsStateYaledTimed(t));
    return atermpp::arg1(t);
  }

  /// \brief Returns the name of a variable expression
  /// \param t A modal formula
  /// \return The name of a variable expression
  inline
  core::identifier_string name(state_formula t)
  {
    assert(core::detail::gsIsStateVar(t) ||
           core::detail::gsIsStateMu(t)  ||
           core::detail::gsIsStateNu(t)
          );
    return atermpp::arg1(t);
  }

  /// \brief Returns the parameters of a variable expression
  /// \param t A modal formula
  /// \return The parameters of a variable expression
  inline
  data::data_expression_list param(state_formula t)
  {
    assert(core::detail::gsIsStateVar(t));
    return atermpp::list_arg2(t);
  }

  /// \brief Returns the parameters of a mu or nu expression
  /// \param t A modal formula
  /// \return The parameters of a mu or nu expression
  inline
  data::data_assignment_list ass(state_formula t)
  {
    assert(core::detail::gsIsStateMu(t) || core::detail::gsIsStateNu(t));
    return atermpp::list_arg2(t);
  }

  /// \brief Returns the regular formula of a must or may expression
  /// \param t A modal formula
  /// \return The regular formula of a must or may expression
  inline
  regular_formula act(state_formula t)
  {
    assert(core::detail::gsIsStateMust(t) || core::detail::gsIsStateMay(t));
    return atermpp::arg1(t);
  }

  /// \cond INTERNAL_DOCS
  //
  /// \brief Function that determines if a state formula is time dependent
  struct is_timed_subterm
  {
    /// \brief Function call operator
    /// \param t A term
    /// \return The function result
    bool operator()(atermpp::aterm_appl t) const
    {
      return is_delay_timed(t) || is_yaled_timed(t) || act_frm::is_at(t);
    }
  };
  /// \endcond

} // namespace state_frm

    /// \brief Returns true if the formula is timed.
    /// \return True if the formula is timed.
    inline
    bool state_formula::has_time() const
    {
      return atermpp::find_if(*this, state_frm::is_timed_subterm()) != atermpp::aterm();
    }

} // namespace modal

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::modal::state_formula)
/// \endcond

#endif // MCRL2_MODAL_STATE_FORMULA_H
