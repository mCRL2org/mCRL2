// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/mucalculus.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_ACTION_FORMULA_H
#define MCRL2_BASIC_ACTION_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/aterm_traits.h"
#include "atermpp/atermpp.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/detail/action_utility.h"

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

    /// \brief Applies a substitution to this action_formula and returns the result
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

/// Accessor functions and predicates for action formulas.
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
  /// \pre l may not be empty
  inline
  action_formula forall(data_variable_list l, action_formula p)
  {
    assert(!l.empty());
    return action_formula(gsMakeActForall(l, p));
  }

  /// \brief Returns the existential quantification of the formula p over the variables in l
  /// \pre l may not be empty
  inline
  action_formula exists(data_variable_list l, action_formula p)
  {
    assert(!l.empty());
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

} // namespace lps

/// \internal
namespace atermpp
{
using lps::action_formula;

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

} // namespace atermpp

#endif // MCRL2_BASIC_ACTION_FORMULA_H
