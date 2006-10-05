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
    return pbes_expression(gsMakePBESAnd(p,q));
  }
  
  inline
  pbes_expression or_(pbes_expression p, pbes_expression q)
  {
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
  pbes_expression propvar(aterm_string name, data_expression_list l)
  {
    return pbes_expression(gsMakePropVarInst(name, l));
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
