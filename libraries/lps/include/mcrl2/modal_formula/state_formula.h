// Author(s): Wieger Wesselink
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

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// state_formula
/// \brief state formula
///
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
class state_formula: public aterm_appl
{
  public:
    state_formula()
      : aterm_appl(mcrl2::core::detail::constructStateFrm())
    {}

    state_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(mcrl2::core::detail::check_rule_StateFrm(m_term));
    }

    state_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_rule_StateFrm(m_term));
    }

    /// Returns true if the formula is timed.
    ///
    bool has_time() const;

    /// \brief Applies a substitution to this state formula and returns the result
    /// The Substitution object must supply the method aterm operator()(aterm).
    template <typename Substitution>
    state_formula substitute(Substitution f) const
    {
      return state_formula(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// state_formula_list
/// \brief singly linked list of state formulas
///
typedef term_list<state_formula> state_formula_list;

/// Accessor functions and predicates for state formulas.
namespace state_frm
{ 

using atermpp::arg1;
using atermpp::arg2;
using atermpp::arg3;
using atermpp::list_arg1;
using atermpp::list_arg2;

  /// \brief Returns the expression false
  inline
  state_formula false_()
  {
    return state_formula(core::detail::gsMakeStateFalse());
  }
  
  /// \brief Returns the expression true
  inline
  state_formula true_()
  {
    return state_formula(core::detail::gsMakeStateTrue());
  }
  
  /// \brief Returns not applied to p
  inline
  state_formula not_(state_formula p)
  {
    return state_formula(core::detail::gsMakeStateNot(p));
  }
  
  /// \brief Returns and applied to p and q
  inline
  state_formula and_(state_formula p, state_formula q)
  {
    return state_formula(core::detail::gsMakeStateAnd(p,q));
  }
  
  /// \brief Returns or applied to p and q
  inline
  state_formula or_(state_formula p, state_formula q)
  {
    return state_formula(core::detail::gsMakeStateOr(p,q));
  }
  
  /// \brief Returns imp applied to p and q
  inline
  state_formula imp(state_formula p, state_formula q)
  {
    return state_formula(core::detail::gsMakeStateImp(p,q));
  }
  
  /// \brief Returns the existential quantification of the formula p over the variables in l
  /// \pre l may not be empty
  inline
  state_formula exists(data::data_variable_list l, state_formula p)
  {
    assert(!l.empty());
    return state_formula(core::detail::gsMakeStateExists(l, p));
  }
  
  /// \brief Returns the universal quantification of the formula p over the variables in l
  /// \pre l may not be empty
  inline
  state_formula forall(data::data_variable_list l, state_formula p)
  {
    assert(!l.empty());
    return state_formula(core::detail::gsMakeStateForall(l, p));
  }

  /// \brief Returns must applied to r and p
  inline
  state_formula must(regular_formula r, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateMust(r, p));
  }

  /// \brief Returns may applied to r and p
  inline
  state_formula may(regular_formula r, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateMay(r, p));
  }

  /// \brief Returns yaled
  inline
  state_formula yaled()
  {
    return state_formula(core::detail::gsMakeStateYaled());
  }

  /// \brief Returns yaled(t)
  inline
  state_formula yaled_timed(data::data_expression t)
  {
    return state_formula(core::detail::gsMakeStateYaledTimed(t));
  }

  /// \brief Returns delay
  inline
  state_formula delay()
  {
    return state_formula(core::detail::gsMakeStateDelay());
  }

  /// \brief Returns delay(t)
  inline
  state_formula delay_timed(data::data_expression t)
  {
    return state_formula(core::detail::gsMakeStateDelayTimed(t));
  }

  /// \brief Returns a variable with the given name and arguments
  inline
  state_formula var(core::identifier_string name, data::data_expression_list l)
  {
    return state_formula(core::detail::gsMakeStateVar(name, l));
  }

  /// \brief Returns a mu expression
  inline
  state_formula mu(core::identifier_string name, data::data_assignment_list l, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateMu(name, l, p));
  }

  /// \brief Returns a nu expression
  inline
  state_formula nu(core::identifier_string name, data::data_assignment_list l, state_formula p)
  {
    return state_formula(core::detail::gsMakeStateNu(name, l, p));
  }

  /// \brief Returns true if the term t is a data expression
  inline bool is_data(aterm_appl t) { return core::gsIsDataExpr(t); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(aterm_appl t) { return core::detail::gsIsStateTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(aterm_appl t) { return core::detail::gsIsStateFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(aterm_appl t) { return core::detail::gsIsStateNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(aterm_appl t) { return core::detail::gsIsStateAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(aterm_appl t) { return core::detail::gsIsStateOr(t); }

  /// \brief Returns true if the term t is an implication expression
  inline bool is_imp(aterm_appl t) { return core::detail::gsIsStateImp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(aterm_appl t) { return core::detail::gsIsStateForall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_exists(aterm_appl t) { return core::detail::gsIsStateExists(t); }

  /// \brief Returns true if the term t is a must expression
  inline bool is_must(aterm_appl t) { return core::detail::gsIsStateMust(t); }

  /// \brief Returns true if the term t is a must expression
  inline bool is_may(aterm_appl t) { return core::detail::gsIsStateMay(t); }

  /// \brief Returns true if the term t is a delay expression
  inline bool is_delay(aterm_appl t) { return core::detail::gsIsStateDelay(t); }

  /// \brief Returns true if the term t is a timed delay expression
  inline bool is_delay_timed(aterm_appl t) { return core::detail::gsIsStateDelayTimed(t); }

  /// \brief Returns true if the term t is a yaled expression
  inline bool is_yaled(aterm_appl t) { return core::detail::gsIsStateYaled(t); }

  /// \brief Returns true if the term t is a timed yaled expression
  inline bool is_yaled_timed(aterm_appl t) { return core::detail::gsIsStateYaledTimed(t); }

  /// \brief Returns true if the term t is a variable expression
  inline bool is_var(aterm_appl t) { return core::detail::gsIsStateVar(t); }

  /// \brief Returns true if the term t is a nu expression
  inline bool is_nu(aterm_appl t) { return core::detail::gsIsStateNu(t); }

  /// \brief Returns true if the term t is a mu expression
  inline bool is_mu(aterm_appl t) { return core::detail::gsIsStateMu(t); }

  /// \brief Returns the argument of a data expression
  inline
  data::data_expression val(state_formula t)
  {
    assert(core::gsIsDataExpr(t));
    return t;
  }
  
  /// \brief Returns the state formula argument of an expression of type
  /// not, mu, nu, exists, forall, must or may.
  inline
  state_formula arg(state_formula t)
  {
    if (core::detail::gsIsStateNot(t))
    {
      return arg1(t);
    }
    if (core::detail::gsIsStateMu(t) || core::detail::gsIsStateNu(t))
    {
      return arg3(t);
    }
    assert(core::detail::gsIsStateExists(t) ||
           core::detail::gsIsStateForall(t) ||
           core::detail::gsIsStateMust(t)   ||
           core::detail::gsIsStateMay(t)
          );
    return arg2(t);   
  }
  
  /// \brief Returns the left hand side of an expression of type and/or/imp
  inline
  state_formula left(state_formula t)
  {
    assert(core::detail::gsIsStateAnd(t) || core::detail::gsIsStateOr(t) || core::detail::gsIsStateImp(t));
    return arg1(t);
  }
  
  /// \brief Returns the right hand side of an expression of type and/or/imp.
  inline
  state_formula right(state_formula t)
  {
    assert(core::detail::gsIsStateAnd(t) || core::detail::gsIsStateOr(t) || core::detail::gsIsStateImp(t));
    return arg2(t);
  }
  
  /// \brief Returns the variables of a quantification expression
  inline
  data::data_variable_list var(state_formula t)
  {
    assert(core::detail::gsIsStateExists(t) || core::detail::gsIsStateForall(t));
    return list_arg1(t);
  }
  
  /// \brief Returns the time of delay or yaled expression
  inline
  data::data_expression time(state_formula t)
  {
    assert(core::detail::gsIsStateDelayTimed(t) || core::detail::gsIsStateYaledTimed(t));
    return arg1(t);
  }
  
  /// \brief Returns the name of a variable expression
  inline
  core::identifier_string name(state_formula t)
  {
    assert(core::detail::gsIsStateVar(t) ||
           core::detail::gsIsStateMu(t)  ||
           core::detail::gsIsStateNu(t)
          );
    return arg1(t);
  }
  
  /// \brief Returns the parameters of a variable expression
  inline
  data::data_expression_list param(state_formula t)
  {
    assert(core::detail::gsIsStateVar(t));
    return list_arg2(t);
  }
  
  /// \brief Returns the parameters of a mu or nu expression
  inline
  data::data_assignment_list ass(state_formula t)
  {
    assert(core::detail::gsIsStateMu(t) || core::detail::gsIsStateNu(t));
    return list_arg2(t);
  }
  
  /// \brief Returns the regular formula of a must or may expression
  inline
  regular_formula act(state_formula t)
  {
    assert(core::detail::gsIsStateMust(t) || core::detail::gsIsStateMay(t));
    return arg1(t);
  }
  
  /// \internal
  struct is_timed_subterm
  {
    bool operator()(aterm_appl t) const
    {
      return is_delay_timed(t) || is_yaled_timed(t) || act_frm::is_at(t);
    }
  };

} // namespace state_frm

  inline
  bool state_formula::has_time() const
  {
    return atermpp::find_if(*this, state_frm::is_timed_subterm()) != aterm();
  }

} // namespace modal

} // namespace mcrl2

/// \internal
namespace atermpp
{
using mcrl2::modal::state_formula;

template<>
struct aterm_traits<state_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(state_formula t)   { t.protect(); }
  static void unprotect(state_formula t) { t.unprotect(); }
  static void mark(state_formula t)      { t.mark(); }
  static ATerm term(state_formula t)     { return t.term(); }
  static ATerm* ptr(state_formula& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_MODAL_STATE_FORMULA_H
