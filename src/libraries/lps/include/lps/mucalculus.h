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
    {}

    action_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(check_rule_ActFrm(m_term));
    }

    action_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_ActFrm(m_term));
    }

    /// Returns true if the action formula equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      return gsIsActTrue(*this);
    }     

    /// Returns true if the action formula equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return gsIsActFalse(*this);
    }

    /// Applies a substitution to this action_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
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

/// Returns the parameters of a multi action.
inline
action_list mult_params(action_formula t)
{
  assert(gsIsMultAct(t));
  return list_arg1(t);
}

/// Returns the argument of a unary operator of type not.
inline
action_formula not_arg(action_formula t)
{
  assert(gsIsActNot(t));
  return arg1(t);
}

/// Returns the left hand side of a binary operator of type and/or/imp.
inline
action_formula lhs(action_formula t)
{
  assert(gsIsActAnd(t) || gsIsActOr(t) || gsIsActImp(t));
  return arg1(t);
}

/// Returns the right hand side of a binary operator of type and/or/imp.
inline
action_formula rhs(action_formula t)
{
  assert(gsIsActAnd(t) || gsIsActOr(t) || gsIsActImp(t));
  return arg2(t);
}

/// Returns the quantifier variables of a state formula of type exists/forall.
inline
data_variable_list quant_vars(action_formula t)
{
  assert(gsIsActExists(t) || gsIsActForall(t));
  return list_arg1(t);
}

/// Returns the action formula argument of a state formula of type exists/forall.
inline
action_formula quant_form(action_formula t)
{
  assert(gsIsActExists(t) || gsIsActForall(t));
  return arg2(t);
}

/// Returns the action formula argument of a state formula of type at.
inline
action_formula at_form(action_formula t)
{
  assert(gsIsActAt(t));
  return arg1(t);
}

/// Returns the time of a state formula of type at.
inline
data_expression at_time(action_formula t)
{
  assert(gsIsActAt(t));
  return arg2(t);
}

} // namespace act_frm

///////////////////////////////////////////////////////////////////////////////
// state_formula
/// \brief state formula expression.
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
    {}

    state_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(check_rule_StateFrm(m_term));
    }

    state_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_StateFrm(m_term));
    }

    // example: "X(d:D,e:E)"
    state_formula(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_term = reinterpret_cast<ATerm>(gsMakeStateVar(identifier_string(p.first), p.second));
    }

    /// Returns true if every propositional variable occurring in the formula
    /// is bound by a 'nu' and every data variable occurring in the formula is
    /// bound by a 'forall'.
    /// TODO: NOT YET IMPLEMENTED
    ///
    bool is_closed() const
    {
      assert(false);
      return true;
    }     

    /// Returns true if the state formula equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      return gsIsStateTrue(*this);
    }     

    /// Returns true if the state formula equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return gsIsStateFalse(*this);
    }

    /// Applies a substitution to this state_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    state_formula substitute(Substitution f) const
    {
      return state_formula(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// state_formula_list
/// \brief singly linked list of data expressions
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

/// Returns the argument of a unary operator of type not.
inline
state_formula not_arg(state_formula t)
{
  assert(gsIsStateNot(t));
  return arg1(t);
}

/// Returns the left hand side of a binary operator of type and/or/imp.
inline
state_formula lhs(state_formula t)
{
  assert(gsIsStateAnd(t) || gsIsStateOr(t) || gsIsStateImp(t));
  return arg1(t);
}

/// Returns the right hand side of a binary operator of type and/or/imp.
inline
state_formula rhs(state_formula t)
{
  assert(gsIsStateAnd(t) || gsIsStateOr(t) || gsIsStateImp(t));
  return arg2(t);
}

/// Returns the quantifier variables of a state formula of type exists/forall.
inline
data_variable_list quant_vars(state_formula t)
{
  assert(gsIsStateExists(t) || gsIsStateForall(t));
  return list_arg1(t);
}

/// Returns the state formula argument of a state formula of type exists/forall.
inline
state_formula quant_form(state_formula t)
{
  assert(gsIsStateExists(t) || gsIsStateForall(t));
  return arg2(t);
}

/// Returns the time of a state formula of type delay/yaled.
inline
data_expression time(state_formula t)
{
  assert(gsIsStateDelayTimed(t) || gsIsStateYaledTimed(t));
  return arg1(t);
}

/// Returns the name of a variable.
inline
identifier_string var_name(state_formula t)
{
  assert(gsIsStateVar(t) || gsIsStateMu(t) || gsIsStateNu(t));
  return arg1(t);
}

/// Returns the value of a variable.
inline
data_expression_list var_val(state_formula t)
{
  assert(gsIsStateVar(t));
  return list_arg2(t);
}

/// Returns the name of a mu-operator expression.
inline
identifier_string mu_name(state_formula t)
{
  assert(gsIsStateMu(t) || gsIsStateNu(t));
  return arg1(t);
}

/// Returns the parameters of a mu-operator expression.
inline
data_assignment_list mu_params(state_formula t)
{
  assert(gsIsStateMu(t) || gsIsStateNu(t));
  return list_arg2(t);
}

/// Returns the variables corresponding to mu_params(f).
inline
data_variable_list mu_variables(state_formula f)
{
  assert(gsIsStateMu(f) || gsIsStateNu(f));
  data_assignment_list l = mu_params(f);
  data_variable_list result;
  for (data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->lhs());
  }
  return atermpp::reverse(result);
}

/// Returns the data expressions corresponding to mu_params(f).
inline
data_expression_list mu_expressions(state_formula f)
{
  assert(gsIsStateMu(f) || gsIsStateNu(f));
  data_assignment_list l = mu_params(f);
  data_expression_list result;
  for (data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

/// Returns the state formula argument of a mu-operator expression.
inline
state_formula mu_form(state_formula t)
{
  assert(gsIsStateMu(t) || gsIsStateNu(t));
  return arg3(t);
}

/// Returns the action formula argument of a modal operator of type must/may.
inline
action_formula mod_act(state_formula t)
{
  assert(gsIsStateMust(t) || gsIsStateMay(t));
  return arg1(t);
}

/// Returns the state formula argument of a modal operator of type must/may.
inline
state_formula mod_form(state_formula t)
{
  assert(gsIsStateMust(t) || gsIsStateMay(t));
  return arg2(t);
}

} // namespace state_frm

///////////////////////////////////////////////////////////////////////////////
// regular_formula
/// \brief regular formula expression.
///
class regular_formula: public aterm_appl
{
  public:
    regular_formula()
    {}

    regular_formula(ATermAppl t)
      : aterm_appl(aterm_appl(t))
    {
      assert(check_rule_RegFrm(m_term));
    }

    regular_formula(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_RegFrm(m_term));
    }

    /// Applies a substitution to this regular_formula and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    regular_formula substitute(Substitution f) const
    {
      return regular_formula(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// regular_formula_list
/// \brief singly linked list of data expressions
///
typedef term_list<regular_formula> regular_formula_list;

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

