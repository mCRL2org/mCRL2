///////////////////////////////////////////////////////////////////////////////
/// \file pbes_init.h
//
/// Convenience functions for creating PBES formulas.

#ifndef LPE_PBES_INIT_H
#define LPE_PBES_INIT_H

#include "lpe/pbes.h"

namespace lpe {

namespace pbes_init {

// // PBES equation system
// <PBES>         ::= PBES(<DataSpec>, <PBEqn>*, <PropVarInst>)
// 
// // propositional variable instantiation
// <PropVarInst>  ::= PropVarInst(<String>, <DataExpr>*)
// 
// // parameterized boolean equation
// <PBEqn>        ::= PBEqn(<FixPoint>, <PropVarDecl>, <PBExpr>)
// 
// // fixpoint symbol
// <FixPoint>     ::= Mu
//                  | Nu
// 
// // propositional variable declaration
// <PropVarDecl>  ::= PropVarDecl(<String>, <DataExpr>*)
// 
// // parameterized boolean expression
// <PBExpr>       ::= <DataExpr>
//                  | PBESTrue
//                  | PBESFalse
//                  | PBESAnd(<PBExpr>, <PBExpr>)
//                  | PBESOr(<PBExpr>, <PBExpr>)
//                  | PBESForall(<DataVarId>+, <PBExpr>)
//                  | PBESExists(<DataVarId>+, <PBExpr>)
//                  | <PropVarInst>
// 
// //data specification
// <DataSpec>     ::= DataSpec(SortSpec(<SortDecl>*), ConsSpec(<OpId>*),
//                      MapSpec(<OpId>*), DataEqnSpec(<DataEqn>*))

  inline bool is_data                                (pbes_expression t) { return gsIsDataExpr      (t); }
  inline bool is_true                                (pbes_expression t) { return gsIsPBESTrue      (t); }
  inline bool is_false                               (pbes_expression t) { return gsIsPBESFalse     (t); }
  inline bool is_and                                 (pbes_expression t) { return gsIsPBESAnd       (t); }
  inline bool is_or                                  (pbes_expression t) { return gsIsPBESOr        (t); }
  inline bool is_forall                              (pbes_expression t) { return gsIsPBESForall    (t); }
  inline bool is_exists                              (pbes_expression t) { return gsIsPBESExists    (t); }
  inline bool is_propositional_variable_instantiation(pbes_expression t) { return gsIsPropVarInst   (t); }

  /// Conversion of a data expression to a pbes expression.
  inline
  pbes_expression val(data_expression d)
  {
    return pbes_expression(aterm_appl(d));
  }
  
  inline
  pbes_expression true_()
  {
    return pbes_expression(gsMakePBESTrue());
  }
  
  inline
  pbes_expression false_()
  {
    return pbes_expression(gsMakePBESFalse());
  }
  
  inline
  pbes_expression and_(pbes_expression p, pbes_expression q)
  {
    if (is_true(p))
      return q;
    else if (is_false(p))
      return false_();
    if (is_true(q))
      return p;
    else if (is_false(q))
      return false_();
    else
      return pbes_expression(gsMakePBESAnd(p,q));
  }
  
  inline
  pbes_expression or_(pbes_expression p, pbes_expression q)
  {
    if (is_true(p))
      return true_();
    else if (is_false(p))
      return q;
    if (is_true(q))
      return true_();
    else if (is_false(q))
      return p;
    else
      return pbes_expression(gsMakePBESOr(p,q));
  }
  
  inline
  pbes_expression forall(data_variable_list l, pbes_expression p)
  {
    return pbes_expression(gsMakePBESForall(l, p));
  }
  
  inline
  pbes_expression exists(data_variable_list l, pbes_expression p)
  {
    return pbes_expression(gsMakePBESExists(l, p));
  }

  inline
  pbes_fixpoint_symbol mu()
  {
    return pbes_equation::mu();
  }
  
  inline
  pbes_fixpoint_symbol nu()
  {
    return pbes_equation::nu();
  }

  /// Returns or_ applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  inline pbes_expression multi_or(FwdIt first, FwdIt last)
  {
    using namespace pbes_init;
  
    if (first == last)
      return pbes_init::false_();
    pbes_expression result = *first++;
    while (first != last)
    {
      result = or_(result, *first++);
    }
    return result;
  }
  
  /// Returns and_ applied to the sequence of pbes expressions [first, last[
  template <typename FwdIt>
  inline pbes_expression multi_and(FwdIt first, FwdIt last)
  {
    using namespace pbes_init;
  
    if (first == last)
      return pbes_init::true_();
    pbes_expression result = *first++;
    while (first != last)
    {
      result = and_(result, *first++);
    }
    return result;
  }

} // namespace pbes_init

} // namespace lpe

#endif // LPE_PBES_INIT_H
