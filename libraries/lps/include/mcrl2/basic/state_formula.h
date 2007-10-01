// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/state_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_STATE_FORMULA_H
#define MCRL2_BASIC_STATE_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/aterm_traits.h"
#include "atermpp/atermpp.h"
#include "mcrl2/basic/regular_formula.h"
#include "mcrl2/data/data.h"

namespace lps {

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
      : aterm_appl(detail::constructStateFrm())
    {}

    state_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(detail::check_rule_StateFrm(m_term));
    }

    state_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_StateFrm(m_term));
    }

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
    return state_formula(gsMakeStateFalse());
  }
  
  /// \brief Returns the expression true
  inline
  state_formula true_()
  {
    return state_formula(gsMakeStateTrue());
  }
  
  /// \brief Returns not applied to p
  inline
  state_formula not_(state_formula p)
  {
    return state_formula(gsMakeStateNot(p));
  }
  
  /// \brief Returns and applied to p and q
  inline
  state_formula and_(state_formula p, state_formula q)
  {
    return state_formula(gsMakeStateAnd(p,q));
  }
  
  /// \brief Returns or applied to p and q
  inline
  state_formula or_(state_formula p, state_formula q)
  {
    return state_formula(gsMakeStateOr(p,q));
  }
  
  /// \brief Returns imp applied to p and q
  inline
  state_formula imp(state_formula p, state_formula q)
  {
    return state_formula(gsMakeStateImp(p,q));
  }
  
  /// \brief Returns the existential quantification of the formula p over the variables in l
  /// \pre l may not be empty
  inline
  state_formula exists(data_variable_list l, state_formula p)
  {
    assert(!l.empty());
    return state_formula(gsMakeStateExists(l, p));
  }
  
  /// \brief Returns the universal quantification of the formula p over the variables in l
  /// \pre l may not be empty
  inline
  state_formula forall(data_variable_list l, state_formula p)
  {
    assert(!l.empty());
    return state_formula(gsMakeStateForall(l, p));
  }

  /// \brief Returns must applied to r and p
  inline
  state_formula must(regular_formula r, state_formula p)
  {
    return state_formula(gsMakeStateMust(r, p));
  }

  /// \brief Returns may applied to r and p
  inline
  state_formula may(regular_formula r, state_formula p)
  {
    return state_formula(gsMakeStateMay(r, p));
  }

  /// \brief Returns yaled
  inline
  state_formula yaled()
  {
    return state_formula(gsMakeStateYaled());
  }

  /// \brief Returns yaled(t)
  inline
  state_formula yaled_timed(data_expression t)
  {
    return state_formula(gsMakeStateYaledTimed(t));
  }

  /// \brief Returns delay
  inline
  state_formula delay()
  {
    return state_formula(gsMakeStateDelay());
  }

  /// \brief Returns delay(t)
  inline
  state_formula delay_timed(data_expression t)
  {
    return state_formula(gsMakeStateDelayTimed(t));
  }

  /// \brief Returns a variable with the given name and arguments
  inline
  state_formula var(identifier_string name, data_expression_list l)
  {
    return state_formula(gsMakeStateVar(name, l));
  }

  /// \brief Returns a mu expression
  inline
  state_formula mu(identifier_string name, data_assignment_list l, state_formula p)
  {
    return state_formula(gsMakeStateMu(name, l, p));
  }

  /// \brief Returns a nu expression
  inline
  state_formula nu(identifier_string name, data_assignment_list l, state_formula p)
  {
    return state_formula(gsMakeStateNu(name, l, p));
  }

  /// \brief Returns true if the term t is a data expression
  inline bool is_data(aterm_appl t) { return gsIsDataExpr(t); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(aterm_appl t) { return gsIsStateTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(aterm_appl t) { return gsIsStateFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(aterm_appl t) { return gsIsStateNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(aterm_appl t) { return gsIsStateAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(aterm_appl t) { return gsIsStateOr(t); }

  /// \brief Returns true if the term t is an implication expression
  inline bool is_imp(aterm_appl t) { return gsIsStateImp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(aterm_appl t) { return gsIsStateForall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_exists(aterm_appl t) { return gsIsStateExists(t); }

  /// \brief Returns true if the term t is a must expression
  inline bool is_must(aterm_appl t) { return gsIsStateMust(t); }

  /// \brief Returns true if the term t is a must expression
  inline bool is_may(aterm_appl t) { return gsIsStateMay(t); }

  /// \brief Returns true if the term t is a delay expression
  inline bool is_delay(aterm_appl t) { return gsIsStateDelay(t); }

  /// \brief Returns true if the term t is a timed delay expression
  inline bool is_delay_timed(aterm_appl t) { return gsIsStateDelayTimed(t); }

  /// \brief Returns true if the term t is a yaled expression
  inline bool is_yaled(aterm_appl t) { return gsIsStateYaled(t); }

  /// \brief Returns true if the term t is a timed yaled expression
  inline bool is_yaled_timed(aterm_appl t) { return gsIsStateYaledTimed(t); }

  /// \brief Returns true if the term t is a variable expression
  inline bool is_var(aterm_appl t) { return gsIsStateVar(t); }

  /// \brief Returns true if the term t is a nu expression
  inline bool is_nu(aterm_appl t) { return gsIsStateNu(t); }

  /// \brief Returns true if the term t is a mu expression
  inline bool is_mu(aterm_appl t) { return gsIsStateMu(t); }

  /// \brief Returns the argument of a data expression
  inline
  data_expression data_arg(state_formula t)
  {
    assert(gsIsDataExpr(t));
    return arg1(t);
  }
  
  /// \brief Returns the argument of a not expression
  inline
  state_formula not_arg(state_formula t)
  {
    assert(gsIsStateNot(t));
    return arg1(t);
  }
  
  /// \brief Returns the left hand side of an expression of type and/or/imp
  inline
  state_formula lhs(state_formula t)
  {
    assert(gsIsStateAnd(t) || gsIsStateOr(t) || gsIsStateImp(t));
    return arg1(t);
  }
  
  /// \brief Returns the right hand side of an expression of type and/or/imp.
  inline
  state_formula rhs(state_formula t)
  {
    assert(gsIsStateAnd(t) || gsIsStateOr(t) || gsIsStateImp(t));
    return arg2(t);
  }
  
  /// \brief Returns the variables of a quantification expression
  inline
  data_variable_list quant_vars(state_formula t)
  {
    assert(gsIsStateExists(t) || gsIsStateForall(t));
    return list_arg1(t);
  }
  
  /// \brief Returns the formula of a quantification expression
  inline
  state_formula quant_form(state_formula t)
  {
    assert(gsIsStateExists(t) || gsIsStateForall(t));
    return arg2(t);
  }
  
  /// \brief Returns the time of delay or yaled expression
  inline
  data_expression time(state_formula t)
  {
    assert(gsIsStateDelayTimed(t) || gsIsStateYaledTimed(t));
    return arg1(t);
  }
  
  /// \brief Returns the name of a variable expression
  inline
  identifier_string var_name(state_formula t)
  {
    assert(gsIsStateVar(t));
    return arg1(t);
  }
  
  /// \brief Returns the value of a variable expression
  inline
  data_expression_list var_val(state_formula t)
  {
    assert(gsIsStateVar(t));
    return list_arg2(t);
  }
  
  /// \brief Returns the name of a mu or nu expression
  inline
  identifier_string mu_name(state_formula t)
  {
    assert(gsIsStateMu(t) || gsIsStateNu(t));
    return arg1(t);
  }
  
  /// \brief Returns the parameters of a mu or nu expression
  inline
  data_assignment_list mu_params(state_formula t)
  {
    assert(gsIsStateMu(t) || gsIsStateNu(t));
    return list_arg2(t);
  }
  
  /// \brief Returns the formula of a mu or nu expression
  inline
  state_formula mu_form(state_formula t)
  {
    assert(gsIsStateMu(t) || gsIsStateNu(t));
    return arg3(t);
  }
  
  /// \brief Returns the regular formula of a must or may expression
  inline
  regular_formula mod_act(state_formula t)
  {
    assert(gsIsStateMust(t) || gsIsStateMay(t));
    return arg1(t);
  }
  
  /// \brief Returns the state formula of a must or may expression
  inline
  state_formula mod_form(state_formula t)
  {
    assert(gsIsStateMust(t) || gsIsStateMay(t));
    return arg2(t);
  }

} // namespace state_frm

} // namespace lps

/// \internal
namespace atermpp
{
using lps::state_formula;

template<>
struct aterm_traits<state_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::state_formula t)   { t.protect(); }
  static void unprotect(lps::state_formula t) { t.unprotect(); }
  static void mark(lps::state_formula t)      { t.mark(); }
  static ATerm term(lps::state_formula t)     { return t.term(); }
  static ATerm* ptr(lps::state_formula& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_BASIC_STATE_FORMULA_H
