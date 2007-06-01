///////////////////////////////////////////////////////////////////////////////
/// \file mucalculus.h
/// Contains data data structures for mu calculus.

#ifndef LPS_CALCULUS_H
#define LPS_CALCULUS_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/aterm_traits.h"
#include "atermpp/atermpp.h"
#include "lps/identifier_string.h"
#include "lps/detail/utility.h"
#include "lps/sort.h"
#include "lps/action.h"
#include "lps/data.h"
#include "libstruct.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm;
using atermpp::term_list;
using lps::detail::parse_variable;

///////////////////////////////////////////////////////////////////////////////
// action_formula
/// \brief action formula expression.
///
//<ActFrm>       ::= <MultAct>
//                 | <DataExpr>
//                 | ActTrue
//                 | ActFalse
//                 | ActNot(<ActFrm>)
//                 | ActAnd(<ActFrm>, <ActFrm>)
//                 | ActOr(<ActFrm>, <ActFrm>)
//                 | ActImp(<ActFrm>, <ActFrm>)
//                 | ActForall(<DataVarId>+, <ActFrm>)
//                 | ActExists(<DataVarId>+, <ActFrm>)
//                 | ActAt(<ActFrm>, <DataExpr>)
class action_formula: public aterm_appl
{
  public:
    action_formula()
      : aterm_appl(detail::constructActFrm())
    {}

    action_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(detail::check_rule_ActFrm(m_term));
    }

    action_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_ActFrm(m_term));
    }

    /// \brief Returns true if the action formula equals 'true'
    /// Note that the term will not be rewritten first.
    bool is_true() const
    {
      return gsIsActTrue(*this);
    }     

    /// \brief Returns true if the action formula equals 'false'
    /// Note that the term will not be rewritten first.
    bool is_false() const
    {
      return gsIsActFalse(*this);
    }

    /// \brief Applies a substitution to this action_formula and returns the result
    /// The Substitution object must supply the method aterm operator()(aterm).
    template <typename Substitution>
    action_formula substitute(Substitution f) const
    {
      return action_formula(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// action_formula_list
/// \brief singly linked list of data expressions
///
typedef term_list<action_formula> action_formula_list;

/// Functions for accessing attributes of action formulas.
namespace act_frm
{ 

using atermpp::arg1;
using atermpp::arg2;
using atermpp::arg3;
using atermpp::list_arg1;
using atermpp::list_arg2;

  /// \brief Returns the expression d
  inline
  action_formula data(data_expression d)
  {
    return action_formula(aterm_appl(d));
  }
  
  /// \brief Returns the expression true
  inline
  action_formula true_()
  {
    return action_formula(gsMakeActTrue());
  }
  
  /// \brief Returns the expression false
  inline
  action_formula false_()
  {
    return action_formula(gsMakeActFalse());
  }
  
  /// \brief Returns not applied to p
  inline
  action_formula not_(action_formula p)
  {
    return action_formula(gsMakeActNot(p));
  }
  
  /// \brief Returns and applied to p and q
  inline
  action_formula and_(action_formula p, action_formula q)
  {
    return action_formula(gsMakeActAnd(p,q));
  }
  
  /// \brief Returns or applied to p and q
  inline
  action_formula or_(action_formula p, action_formula q)
  {
    return action_formula(gsMakeActOr(p,q));
  }
  
  /// \brief Returns imp applied to p and q
  inline
  action_formula imp(action_formula p, action_formula q)
  {
    return action_formula(gsMakeActImp(p,q));
  }
  
  /// \brief Returns the universal quantification of the formula p over the variables in l
  inline
  action_formula forall(data_variable_list l, action_formula p)
  {
    return action_formula(gsMakeActExists(l, p));
  }

  /// \brief Returns the existential quantification of the formula p over the variables in l
  inline
  action_formula exists(data_variable_list l, action_formula p)
  {
    return action_formula(gsMakeActExists(l, p));
  }

  /// \brief Returns the 'p at d'
  inline
  action_formula at(action_formula p, data_expression d)
  {
    return action_formula(gsMakeActAt(p,d));
  }

  /// \brief Returns true if the term t is a multi action
  inline bool is_mult_act(action_formula t) { return gsIsMultAct(t); }

  /// \brief Returns true if the term t is a data expression
  inline bool is_data(action_formula t) { return gsIsDataExpr(t); }

  /// \brief Returns true if the term t is equal to true
  inline bool is_true(action_formula t) { return gsIsActTrue(t); }

  /// \brief Returns true if the term t is equal to false
  inline bool is_false(action_formula t) { return gsIsActFalse(t); }

  /// \brief Returns true if the term t is a not expression
  inline bool is_not(action_formula t) { return gsIsActNot(t); }

  /// \brief Returns true if the term t is an and expression
  inline bool is_and(action_formula t) { return gsIsActAnd(t); }

  /// \brief Returns true if the term t is an or expression
  inline bool is_or(action_formula t) { return gsIsActOr(t); }

  /// \brief Returns true if the term t is an implication expression
  inline bool is_imp(action_formula t) { return gsIsActImp(t); }

  /// \brief Returns true if the term t is a universal quantification
  inline bool is_forall(action_formula t) { return gsIsActForall(t); }

  /// \brief Returns true if the term t is an existential quantification
  inline bool is_exists(action_formula t) { return gsIsActExists(t); }

  /// \brief Returns true if the term t is an at expression
  inline bool is_at(action_formula t) { return gsIsActAt(t); }

  /// \brief Returns the parameters of an action formula
  inline
  action_list mult_params(action_formula t)
  {
    assert(gsIsMultAct(t));
    return list_arg1(t);
  }
  
  /// \brief Returns the argument of a not expression
  inline
  action_formula not_arg(action_formula t)
  {
    assert(gsIsActNot(t));
    return arg1(t);
  }
  
  /// \brief Returns the left hand side of an expression of type and/or/imp
  inline
  action_formula lhs(action_formula t)
  {
    assert(gsIsActAnd(t) || gsIsActOr(t) || gsIsActImp(t));
    return arg1(t);
  }
  
  /// \brief Returns the right hand side of an expression of type and/or/imp.
  inline
  action_formula rhs(action_formula t)
  {
    assert(gsIsActAnd(t) || gsIsActOr(t) || gsIsActImp(t));
    return arg2(t);
  }
  
  /// \brief Returns the variables of a quantification expression
  inline
  data_variable_list quant_vars(action_formula t)
  {
    assert(gsIsActExists(t) || gsIsActForall(t));
    return list_arg1(t);
  }
  
  /// \brief Returns the formula of a quantification expression
  inline
  action_formula quant_form(action_formula t)
  {
    assert(gsIsActExists(t) || gsIsActForall(t));
    return arg2(t);
  }
  
  /// \brief Returns the formula of an at expression
  inline
  action_formula at_form(action_formula t)
  {
    assert(gsIsActAt(t));
    return arg1(t);
  }
  
  /// \brief Returns the time of an at expression
  inline
  data_expression at_time(action_formula t)
  {
    assert(gsIsActAt(t));
    return arg2(t);
  }

} // namespace act_frm

///////////////////////////////////////////////////////////////////////////////
// regular_formula
/// \brief regular formula expression.
///
class regular_formula: public aterm_appl
{
  public:
    regular_formula()
      : aterm_appl(detail::constructRegFrm())
    {}

    regular_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(detail::check_rule_RegFrm(m_term));
    }

    regular_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_RegFrm(m_term));
    }

    /// \brief Applies a substitution to this regular formula and returns the result
    /// The Substitution object must supply the method aterm operator()(aterm).
    template <typename Substitution>
    regular_formula substitute(Substitution f) const
    {
      return regular_formula(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// regular_formula_list
/// \brief singly linked list of regular expressions
///
typedef term_list<regular_formula> regular_formula_list;

/*
namespace reg_frm {

  inline
  regular_formula seq(regular_formula p, regular_formula q)
  {
    return regular_formula(gsMakeRegSeq(p,q));
  }
  
  inline
  regular_formula alt(regular_formula p, regular_formula q)
  {
    return regular_formula(gsMakeRegAlt(p,q));
  }
  
  inline
  regular_formula trans(regular_formula p)
  {
    return regular_formula(gsMakeRegTrans(p));
  }
  
  inline
  regular_formula trans_or_nil(regular_formula p)
  {
    return regular_formula(gsMakeRegTransOrNil(p));
  }

  inline bool is_act(regular_formula t) { return gsIsActFrm(t); }
  inline bool is_nil(regular_formula t) { return gsIsRegNil(t); }
  inline bool is_seq(regular_formula t) { return gsIsRegSeq(t); }
  inline bool is_alt(regular_formula t) { return gsIsRegAlt(t); }
  inline bool is_trans(regular_formula t) { return gsIsRegTrans(t); }
  inline bool is_trans_or_nil(regular_formula t) { return gsIsRegTransOrNil(t); }

} // namespace reg_frm
*/

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

    /// \brief Constructor for state variables like "X(d:D,e:E)"
    /// Only works for constant sorts.
    state_formula(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_term = reinterpret_cast<ATerm>(gsMakeStateVar(identifier_string(p.first), p.second));
    }

    /// \brief Returns true if every propositional variable occurring in the formula
    /// is bound by a 'nu' and every data variable occurring in the formula is
    /// bound by a 'forall'.
    /// TODO: NOT YET IMPLEMENTED
    bool is_closed() const
    {
      assert(false);
      return true;
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

/// Functions for accessing attributes of state formulas.
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
  inline
  state_formula exists(data_variable_list l, state_formula p)
  {
    return state_formula(gsMakeStateExists(l, p));
  }
  
  /// \brief Returns the universal quantification of the formula p over the variables in l
  inline
  state_formula forall(data_variable_list l, state_formula p)
  {
    return state_formula(gsMakeStateExists(l, p));
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
  
  /// \brief Returns the action formula of a must or may expression
  inline
  action_formula mod_act(state_formula t)
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

/// INTERNAL ONLY
namespace atermpp
{
using lps::state_formula;
using lps::action_formula;
using lps::regular_formula;

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

template<>
struct aterm_traits<action_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(action_formula t)   { t.protect(); }
  static void unprotect(action_formula t) { t.unprotect(); }
  static void mark(action_formula t)      { t.mark(); }
  static ATerm term(action_formula t)     { return t.term(); }
  static ATerm* ptr(action_formula& t)    { return &t.term(); }
};

template<>
struct aterm_traits<regular_formula>
{
  typedef ATermAppl aterm_type;
  static void protect(regular_formula t)   { t.protect(); }
  static void unprotect(regular_formula t) { t.unprotect(); }
  static void mark(regular_formula t)      { t.mark(); }
  static ATerm term(regular_formula t)     { return t.term(); }
  static ATerm* ptr(regular_formula& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_CALCULUS_H
