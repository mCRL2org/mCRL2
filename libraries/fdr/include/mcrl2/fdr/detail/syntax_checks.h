// Author(s): Generated code
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/detail/syntax_checks.h
/// \brief Functions for checking the syntax of terms.

#ifndef MCRL2_FDR_DETAIL_SYNTAX_CHECKS_H
#define MCRL2_FDR_DETAIL_SYNTAX_CHECKS_H

#include <iostream>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/fdr/detail/utility.h"

namespace mcrl2 {

namespace fdr {

namespace detail {

template <typename Term>
bool check_rule_Name(Term t)
{
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (a.size() > 0)
    return false;
  return true;
}

template <typename Term>
bool check_rule_Number(Term t)
{
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (a.size() > 0)
    return false;
  return true;
}

//--- start generated code ---//
template <typename Term> bool check_rule_FDRSpec(Term t);
template <typename Term> bool check_rule_Defn(Term t);
template <typename Term> bool check_rule_Channel(Term t);
template <typename Term> bool check_rule_VarType(Term t);
template <typename Term> bool check_rule_Type(Term t);
template <typename Term> bool check_rule_Check(Term t);
template <typename Term> bool check_rule_Refined(Term t);
template <typename Term> bool check_rule_Model(Term t);
template <typename Term> bool check_rule_FailureModel(Term t);
template <typename Term> bool check_rule_Test(Term t);
template <typename Term> bool check_rule_TestType(Term t);
template <typename Term> bool check_rule_TrName(Term t);
template <typename Term> bool check_rule_FileName(Term t);
template <typename Term> bool check_rule_Any(Term t);
template <typename Term> bool check_rule_Expr(Term t);
template <typename Term> bool check_rule_Numb(Term t);
template <typename Term> bool check_rule_Bool(Term t);
template <typename Term> bool check_rule_Set(Term t);
template <typename Term> bool check_rule_Seq(Term t);
template <typename Term> bool check_rule_TargGens(Term t);
template <typename Term> bool check_rule_Targ(Term t);
template <typename Term> bool check_rule_Gen(Term t);
template <typename Term> bool check_rule_Tuple(Term t);
template <typename Term> bool check_rule_Dotted(Term t);
template <typename Term> bool check_rule_Lambda(Term t);
template <typename Term> bool check_rule_Common(Term t);
template <typename Term> bool check_rule_Proc(Term t);
template <typename Term> bool check_rule_Field(Term t);
template <typename Term> bool check_rule_Renaming(Term t);
template <typename Term> bool check_rule_Map(Term t);
template <typename Term> bool check_rule_LinkPar(Term t);
template <typename Term> bool check_rule_Link(Term t);
template <typename Term> bool check_term_RepInternalChoice(Term t);
template <typename Term> bool check_term_RCheck(Term t);
template <typename Term> bool check_term_Union(Term t);
template <typename Term> bool check_term_GreaterOrEqual(Term t);
template <typename Term> bool check_term_Branch(Term t);
template <typename Term> bool check_term_Test(Term t);
template <typename Term> bool check_term_Include(Term t);
template <typename Term> bool check_term_Sharing(Term t);
template <typename Term> bool check_term_deadlock_free(Term t);
template <typename Term> bool check_term_TargGens(Term t);
template <typename Term> bool check_term_deterministic(Term t);
template <typename Term> bool check_term_FDRSpec(Term t);
template <typename Term> bool check_term_Minus(Term t);
template <typename Term> bool check_term_TypeName(Term t);
template <typename Term> bool check_term_Numb(Term t);
template <typename Term> bool check_term_Not(Term t);
template <typename Term> bool check_term_Mod(Term t);
template <typename Term> bool check_term_LessOrEqual(Term t);
template <typename Term> bool check_term_divergence_free(Term t);
template <typename Term> bool check_term_normalise(Term t);
template <typename Term> bool check_term_Elem(Term t);
template <typename Term> bool check_term_Cat(Term t);
template <typename Term> bool check_term_RepExternalChoice(Term t);
template <typename Term> bool check_term_Length(Term t);
template <typename Term> bool check_term_Plus(Term t);
template <typename Term> bool check_term_T(Term t);
template <typename Term> bool check_term_Div(Term t);
template <typename Term> bool check_term_Or(Term t);
template <typename Term> bool check_term_RepInterleave(Term t);
template <typename Term> bool check_term_Links(Term t);
template <typename Term> bool check_term_Less(Term t);
template <typename Term> bool check_term_InternalChoice(Term t);
template <typename Term> bool check_term_LambdaExpr(Term t);
template <typename Term> bool check_term_Conditional(Term t);
template <typename Term> bool check_term_FileName(Term t);
template <typename Term> bool check_term_Bool(Term t);
template <typename Term> bool check_term_inter(Term t);
template <typename Term> bool check_term_RepSharing(Term t);
template <typename Term> bool check_term_NotEqual(Term t);
template <typename Term> bool check_term_set(Term t);
template <typename Term> bool check_term_Pattern(Term t);
template <typename Term> bool check_term_LinkedParallel(Term t);
template <typename Term> bool check_term_Tail(Term t);
template <typename Term> bool check_term_productions(Term t);
template <typename Term> bool check_term_NotCheck(Term t);
template <typename Term> bool check_term_Print(Term t);
template <typename Term> bool check_term_Channel(Term t);
template <typename Term> bool check_term_Head(Term t);
template <typename Term> bool check_term_OpenRange(Term t);
template <typename Term> bool check_term_ExternalChoice(Term t);
template <typename Term> bool check_term_BoolGuard(Term t);
template <typename Term> bool check_term_TypeSet(Term t);
template <typename Term> bool check_term_SubType(Term t);
template <typename Term> bool check_term_Link(Term t);
template <typename Term> bool check_term_External(Term t);
template <typename Term> bool check_term_SKIP(Term t);
template <typename Term> bool check_term_Input(Term t);
template <typename Term> bool check_term_livelock_free(Term t);
template <typename Term> bool check_term_SimpleBranch(Term t);
template <typename Term> bool check_term_Times(Term t);
template <typename Term> bool check_term_Exprs(Term t);
template <typename Term> bool check_term_SimpleChannel(Term t);
template <typename Term> bool check_term_ChanSet(Term t);
template <typename Term> bool check_term_Rename(Term t);
template <typename Term> bool check_term_model_compress(Term t);
template <typename Term> bool check_term_Bracketed(Term t);
template <typename Term> bool check_term_Interleave(Term t);
template <typename Term> bool check_term_ClosedRange(Term t);
template <typename Term> bool check_term_extensions(Term t);
template <typename Term> bool check_term_explicate(Term t);
template <typename Term> bool check_term_Null(Term t);
template <typename Term> bool check_term_Dotted(Term t);
template <typename Term> bool check_term_STOP(Term t);
template <typename Term> bool check_term_CHAOS(Term t);
template <typename Term> bool check_term_Member(Term t);
template <typename Term> bool check_term_Maps(Term t);
template <typename Term> bool check_term_LinksGens(Term t);
template <typename Term> bool check_term_BCheck(Term t);
template <typename Term> bool check_term_RepAlphaParallel(Term t);
template <typename Term> bool check_term_diff(Term t);
template <typename Term> bool check_term_RepSequentialComposition(Term t);
template <typename Term> bool check_term_Proc(Term t);
template <typename Term> bool check_term_Empty(Term t);
template <typename Term> bool check_term_NameType(Term t);
template <typename Term> bool check_term_FD(Term t);
template <typename Term> bool check_term_Tuple(Term t);
template <typename Term> bool check_term_union(Term t);
template <typename Term> bool check_term_Transparent(Term t);
template <typename Term> bool check_term_Card(Term t);
template <typename Term> bool check_term_diamond(Term t);
template <typename Term> bool check_term_F(Term t);
template <typename Term> bool check_term_LambdaAppl(Term t);
template <typename Term> bool check_term_AlphaParallel(Term t);
template <typename Term> bool check_term_Interrupt(Term t);
template <typename Term> bool check_term_Inter(Term t);
template <typename Term> bool check_term_true(Term t);
template <typename Term> bool check_term_Assign(Term t);
template <typename Term> bool check_term_Lambda(Term t);
template <typename Term> bool check_term_And(Term t);
template <typename Term> bool check_term_Set(Term t);
template <typename Term> bool check_term_Seq(Term t);
template <typename Term> bool check_term_Number(Term t);
template <typename Term> bool check_term_Gen(Term t);
template <typename Term> bool check_term_Prefix(Term t);
template <typename Term> bool check_term_Concat(Term t);
template <typename Term> bool check_term_Hiding(Term t);
template <typename Term> bool check_term_SequentialComposition(Term t);
template <typename Term> bool check_term_false(Term t);
template <typename Term> bool check_term_Min(Term t);
template <typename Term> bool check_term_SimpleInput(Term t);
template <typename Term> bool check_term_SimpleTypeName(Term t);
template <typename Term> bool check_term_TypeProduct(Term t);
template <typename Term> bool check_term_BGen(Term t);
template <typename Term> bool check_term_normalize(Term t);
template <typename Term> bool check_term_Map(Term t);
template <typename Term> bool check_term_sbsim(Term t);
template <typename Term> bool check_term_Nil(Term t);
template <typename Term> bool check_term_TCheck(Term t);
template <typename Term> bool check_term_Greater(Term t);
template <typename Term> bool check_term_Assert(Term t);
template <typename Term> bool check_term_LocalDef(Term t);
template <typename Term> bool check_term_tau_loop_factor(Term t);
template <typename Term> bool check_term_Name(Term t);
template <typename Term> bool check_term_normal(Term t);
template <typename Term> bool check_term_Expr(Term t);
template <typename Term> bool check_term_Equal(Term t);
template <typename Term> bool check_term_TypeTuple(Term t);
template <typename Term> bool check_term_Output(Term t);
template <typename Term> bool check_term_Model(Term t);
template <typename Term> bool check_term_Dot(Term t);
template <typename Term> bool check_term_MapsGens(Term t);
template <typename Term> bool check_term_RepLinkedParallel(Term t);
template <typename Term> bool check_term_DataType(Term t);
template <typename Term> bool check_term_UntimedTimeOut(Term t);

template <typename Term>
bool check_rule_FDRSpec(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_FDRSpec(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Defn(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Assign(t)
         || check_rule_Channel(t)
         || check_term_NameType(t)
         || check_term_DataType(t)
         || check_term_SubType(t)
         || check_term_External(t)
         || check_term_Transparent(t)
         || check_term_Assert(t)
         || check_term_Print(t)
         || check_term_Include(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Channel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Channel(t)
         || check_term_SimpleChannel(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_VarType(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_SimpleBranch(t)
         || check_term_Branch(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Type(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_TypeProduct(t)
         || check_term_TypeTuple(t)
         || check_term_TypeSet(t)
         || check_term_SimpleTypeName(t)
         || check_term_TypeName(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Check(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_BCheck(t)
         || check_term_RCheck(t)
         || check_term_TCheck(t)
         || check_term_NotCheck(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Refined(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Model(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Model(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Nil(t)
         || check_term_T(t)
         || check_rule_FailureModel(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_FailureModel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_F(t)
         || check_term_FD(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Test(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_TestType(t)
         || check_term_Test(t)
         || check_term_divergence_free(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_TestType(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_deterministic(t)
         || check_term_deadlock_free(t)
         || check_term_livelock_free(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_TrName(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_normal(t)
         || check_term_normalise(t)
         || check_term_normalize(t)
         || check_term_sbsim(t)
         || check_term_tau_loop_factor(t)
         || check_term_diamond(t)
         || check_term_model_compress(t)
         || check_term_explicate(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_FileName(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_FileName(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Any(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Expr(t)
         || check_term_Proc(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Expr(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Numb(t)
         || check_term_Bool(t)
         || check_term_Set(t)
         || check_term_Seq(t)
         || check_term_Tuple(t)
         || check_term_Dotted(t)
         || check_term_Lambda(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Numb(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_term_Number(t)
         || check_term_Card(t)
         || check_term_Length(t)
         || check_term_Plus(t)
         || check_term_Minus(t)
         || check_term_Times(t)
         || check_term_Div(t)
         || check_term_Mod(t)
         || check_term_Min(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Bool(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_term_true(t)
         || check_term_false(t)
         || check_term_And(t)
         || check_term_Or(t)
         || check_term_Not(t)
         || check_term_Null(t)
         || check_term_Elem(t)
         || check_term_Member(t)
         || check_term_Empty(t)
         || check_term_Equal(t)
         || check_term_NotEqual(t)
         || check_term_Less(t)
         || check_term_LessOrEqual(t)
         || check_term_Greater(t)
         || check_term_GreaterOrEqual(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Set(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_rule_TargGens(t)
         || check_term_ChanSet(t)
         || check_term_union(t)
         || check_term_inter(t)
         || check_term_diff(t)
         || check_term_Union(t)
         || check_term_Inter(t)
         || check_term_set(t)
         || check_term_Set(t)
         || check_term_Seq(t)
         || check_term_extensions(t)
         || check_term_productions(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Seq(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_rule_TargGens(t)
         || check_term_Cat(t)
         || check_term_Concat(t)
         || check_term_Head(t)
         || check_term_Tail(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_TargGens(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Targ(t)
         || check_term_TargGens(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Targ(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Nil(t)
         || check_term_Exprs(t)
         || check_term_ClosedRange(t)
         || check_term_OpenRange(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Gen(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_BGen(t)
         || check_term_Gen(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Tuple(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_term_Exprs(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Dotted(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_term_Dot(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Lambda(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_term_LambdaExpr(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Common(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Conditional(t)
         || check_term_Name(t)
         || check_term_LambdaAppl(t)
         || check_term_LocalDef(t)
         || check_term_Bracketed(t)
         || check_term_Pattern(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Proc(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_rule_Common(t)
         || check_term_STOP(t)
         || check_term_SKIP(t)
         || check_term_CHAOS(t)
         || check_term_Prefix(t)
         || check_term_ExternalChoice(t)
         || check_term_InternalChoice(t)
         || check_term_SequentialComposition(t)
         || check_term_Interrupt(t)
         || check_term_Hiding(t)
         || check_term_Rename(t)
         || check_term_Interleave(t)
         || check_term_Sharing(t)
         || check_term_AlphaParallel(t)
         || check_term_RepExternalChoice(t)
         || check_term_RepInternalChoice(t)
         || check_term_RepSequentialComposition(t)
         || check_term_RepInterleave(t)
         || check_term_RepSharing(t)
         || check_term_RepAlphaParallel(t)
         || check_term_UntimedTimeOut(t)
         || check_term_BoolGuard(t)
         || check_term_LinkedParallel(t)
         || check_term_RepLinkedParallel(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Field(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_SimpleInput(t)
         || check_term_Input(t)
         || check_term_Output(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Renaming(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Maps(t)
         || check_term_MapsGens(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Map(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Map(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_LinkPar(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Links(t)
         || check_term_LinksGens(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

template <typename Term>
bool check_rule_Link(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  return    check_term_Link(t);
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

// RepInternalChoice(Gen+, Proc)
template <typename Term>
bool check_term_RepInternalChoice(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepInternalChoice(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RCheck(Proc, Proc, Refined)
template <typename Term>
bool check_term_RCheck(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRCheck(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Refined<atermpp::aterm>))
    {
      std::cerr << "check_rule_Refined" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Union(Set)
template <typename Term>
bool check_term_Union(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsUnion(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// GreaterOrEqual(Expr, Expr)
template <typename Term>
bool check_term_GreaterOrEqual(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsGreaterOrEqual(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Branch(Name, Type)
template <typename Term>
bool check_term_Branch(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsBranch(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Type<atermpp::aterm>))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Test(TestType, FailureModel)
template <typename Term>
bool check_term_Test(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTest(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_TestType<atermpp::aterm>))
    {
      std::cerr << "check_rule_TestType" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_FailureModel<atermpp::aterm>))
    {
      std::cerr << "check_rule_FailureModel" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Include(FileName)
template <typename Term>
bool check_term_Include(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsInclude(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_FileName<atermpp::aterm>))
    {
      std::cerr << "check_rule_FileName" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Sharing(Proc, Proc, Set)
template <typename Term>
bool check_term_Sharing(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSharing(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// deadlock_free()
template <typename Term>
bool check_term_deadlock_free(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsdeadlock_free(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// TargGens(Targ, Gen+)
template <typename Term>
bool check_term_TargGens(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTargGens(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Targ<atermpp::aterm>))
    {
      std::cerr << "check_rule_Targ" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// deterministic()
template <typename Term>
bool check_term_deterministic(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsdeterministic(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// FDRSpec(Defn*)
template <typename Term>
bool check_term_FDRSpec(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsFDRSpec(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Defn<atermpp::aterm>, 0))
    {
      std::cerr << "check_rule_Defn" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Minus(Numb, Numb)
template <typename Term>
bool check_term_Minus(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMinus(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// TypeName(Name, Type+)
template <typename Term>
bool check_term_TypeName(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTypeName(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_Type<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Numb(Numb)
template <typename Term>
bool check_term_Numb(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNumb(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Not(Bool)
template <typename Term>
bool check_term_Not(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNot(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Mod(Numb, Numb)
template <typename Term>
bool check_term_Mod(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMod(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// LessOrEqual(Expr, Expr)
template <typename Term>
bool check_term_LessOrEqual(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLessOrEqual(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// divergence_free()
template <typename Term>
bool check_term_divergence_free(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsdivergence_free(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// normalise()
template <typename Term>
bool check_term_normalise(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsnormalise(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Elem(Expr, Seq)
template <typename Term>
bool check_term_Elem(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsElem(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Cat(Seq, Seq)
template <typename Term>
bool check_term_Cat(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsCat(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RepExternalChoice(Gen+, Proc)
template <typename Term>
bool check_term_RepExternalChoice(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepExternalChoice(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Length(Seq)
template <typename Term>
bool check_term_Length(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLength(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Plus(Numb, Numb)
template <typename Term>
bool check_term_Plus(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsPlus(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// T()
template <typename Term>
bool check_term_T(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsT(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Div(Numb, Numb)
template <typename Term>
bool check_term_Div(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsDiv(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Or(Bool, Bool)
template <typename Term>
bool check_term_Or(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsOr(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RepInterleave(Gen+, Proc)
template <typename Term>
bool check_term_RepInterleave(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepInterleave(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Links(Link+)
template <typename Term>
bool check_term_Links(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLinks(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Link<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Link" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Less(Expr, Expr)
template <typename Term>
bool check_term_Less(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLess(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// InternalChoice(Proc, Proc)
template <typename Term>
bool check_term_InternalChoice(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsInternalChoice(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// LambdaExpr(Expr+, Any)
template <typename Term>
bool check_term_LambdaExpr(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLambdaExpr(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Expr<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Conditional(Bool, Any, Any)
template <typename Term>
bool check_term_Conditional(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsConditional(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// FileName(Name+)
template <typename Term>
bool check_term_FileName(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsFileName(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Name<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Bool(Bool)
template <typename Term>
bool check_term_Bool(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsBool(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// inter(Set, Set)
template <typename Term>
bool check_term_inter(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsinter(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RepSharing(Gen+, Proc, Set)
template <typename Term>
bool check_term_RepSharing(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepSharing(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// NotEqual(Expr, Expr)
template <typename Term>
bool check_term_NotEqual(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNotEqual(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// set(Seq)
template <typename Term>
bool check_term_set(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsset(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Pattern(Any, Any)
template <typename Term>
bool check_term_Pattern(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsPattern(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// LinkedParallel(Proc, Proc, LinkPar)
template <typename Term>
bool check_term_LinkedParallel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLinkedParallel(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_LinkPar<atermpp::aterm>))
    {
      std::cerr << "check_rule_LinkPar" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Tail(Seq)
template <typename Term>
bool check_term_Tail(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTail(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// productions(Expr)
template <typename Term>
bool check_term_productions(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsproductions(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// NotCheck(Check)
template <typename Term>
bool check_term_NotCheck(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNotCheck(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Check<atermpp::aterm>))
    {
      std::cerr << "check_rule_Check" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Print(Expr)
template <typename Term>
bool check_term_Print(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsPrint(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Channel(Name+, Type)
template <typename Term>
bool check_term_Channel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsChannel(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Name<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Type<atermpp::aterm>))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Head(Seq)
template <typename Term>
bool check_term_Head(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsHead(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// OpenRange(Numb)
template <typename Term>
bool check_term_OpenRange(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsOpenRange(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// ExternalChoice(Proc, Proc)
template <typename Term>
bool check_term_ExternalChoice(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsExternalChoice(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// BoolGuard(Bool, Proc)
template <typename Term>
bool check_term_BoolGuard(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsBoolGuard(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// TypeSet(Set)
template <typename Term>
bool check_term_TypeSet(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTypeSet(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SubType(Name, VarType+)
template <typename Term>
bool check_term_SubType(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSubType(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_VarType<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_VarType" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Link(Dotted, Dotted)
template <typename Term>
bool check_term_Link(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLink(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Dotted<atermpp::aterm>))
    {
      std::cerr << "check_rule_Dotted" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Dotted<atermpp::aterm>))
    {
      std::cerr << "check_rule_Dotted" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// External(Name+)
template <typename Term>
bool check_term_External(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsExternal(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Name<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SKIP()
template <typename Term>
bool check_term_SKIP(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSKIP(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Input(Expr, Set)
template <typename Term>
bool check_term_Input(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsInput(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// livelock_free()
template <typename Term>
bool check_term_livelock_free(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIslivelock_free(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SimpleBranch(Name)
template <typename Term>
bool check_term_SimpleBranch(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSimpleBranch(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Times(Numb, Numb)
template <typename Term>
bool check_term_Times(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTimes(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Exprs(Expr+)
template <typename Term>
bool check_term_Exprs(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsExprs(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Expr<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SimpleChannel(Name+)
template <typename Term>
bool check_term_SimpleChannel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSimpleChannel(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Name<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// ChanSet(TargGens)
template <typename Term>
bool check_term_ChanSet(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsChanSet(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_TargGens<atermpp::aterm>))
    {
      std::cerr << "check_rule_TargGens" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Rename(Proc, Renaming)
template <typename Term>
bool check_term_Rename(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRename(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Renaming<atermpp::aterm>))
    {
      std::cerr << "check_rule_Renaming" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// model_compress()
template <typename Term>
bool check_term_model_compress(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsmodel_compress(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Bracketed(Any)
template <typename Term>
bool check_term_Bracketed(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsBracketed(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Interleave(Proc, Proc)
template <typename Term>
bool check_term_Interleave(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsInterleave(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// ClosedRange(Numb, Numb)
template <typename Term>
bool check_term_ClosedRange(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsClosedRange(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// extensions(Expr)
template <typename Term>
bool check_term_extensions(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsextensions(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// explicate()
template <typename Term>
bool check_term_explicate(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsexplicate(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Null(Seq)
template <typename Term>
bool check_term_Null(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNull(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Dotted(Dotted)
template <typename Term>
bool check_term_Dotted(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsDotted(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Dotted<atermpp::aterm>))
    {
      std::cerr << "check_rule_Dotted" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// STOP()
template <typename Term>
bool check_term_STOP(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSTOP(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// CHAOS(Set)
template <typename Term>
bool check_term_CHAOS(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsCHAOS(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Member(Expr, Set)
template <typename Term>
bool check_term_Member(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMember(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Maps(Map+)
template <typename Term>
bool check_term_Maps(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMaps(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Map<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Map" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// LinksGens(Link+, Gen+)
template <typename Term>
bool check_term_LinksGens(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLinksGens(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Link<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Link" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// BCheck(Bool)
template <typename Term>
bool check_term_BCheck(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsBCheck(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RepAlphaParallel(Gen+, Proc, Set)
template <typename Term>
bool check_term_RepAlphaParallel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepAlphaParallel(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// diff(Set, Set)
template <typename Term>
bool check_term_diff(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsdiff(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RepSequentialComposition(Gen+, Proc)
template <typename Term>
bool check_term_RepSequentialComposition(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepSequentialComposition(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Proc(Proc)
template <typename Term>
bool check_term_Proc(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsProc(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Empty(Set)
template <typename Term>
bool check_term_Empty(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsEmpty(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// NameType(Name, Type)
template <typename Term>
bool check_term_NameType(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNameType(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Type<atermpp::aterm>))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// FD()
template <typename Term>
bool check_term_FD(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsFD(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Tuple(Tuple)
template <typename Term>
bool check_term_Tuple(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTuple(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Tuple<atermpp::aterm>))
    {
      std::cerr << "check_rule_Tuple" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// union(Set, Set)
template <typename Term>
bool check_term_union(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsunion(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Transparent(TrName+)
template <typename Term>
bool check_term_Transparent(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTransparent(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_TrName<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_TrName" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Card(Set)
template <typename Term>
bool check_term_Card(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsCard(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// diamond()
template <typename Term>
bool check_term_diamond(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsdiamond(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// F()
template <typename Term>
bool check_term_F(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsF(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// LambdaAppl(Lambda, Expr+)
template <typename Term>
bool check_term_LambdaAppl(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLambdaAppl(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Lambda<atermpp::aterm>))
    {
      std::cerr << "check_rule_Lambda" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_Expr<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// AlphaParallel(Proc, Proc, Set, Set)
template <typename Term>
bool check_term_AlphaParallel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsAlphaParallel(a))
    return false;

  // check the children
  if (a.size() != 4)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
  if (!check_term_argument(a(3), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Interrupt(Proc, Proc)
template <typename Term>
bool check_term_Interrupt(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsInterrupt(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Inter(Set)
template <typename Term>
bool check_term_Inter(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsInter(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// true()
template <typename Term>
bool check_term_true(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIstrue(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Assign(Any, Any)
template <typename Term>
bool check_term_Assign(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsAssign(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Lambda(Lambda)
template <typename Term>
bool check_term_Lambda(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLambda(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Lambda<atermpp::aterm>))
    {
      std::cerr << "check_rule_Lambda" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// And(Bool, Bool)
template <typename Term>
bool check_term_And(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsAnd(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Set(Set)
template <typename Term>
bool check_term_Set(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSet(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Seq(Seq)
template <typename Term>
bool check_term_Seq(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSeq(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Number(Number)
template <typename Term>
bool check_term_Number(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNumber(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Number<atermpp::aterm>))
    {
      std::cerr << "check_rule_Number" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Gen(Expr, Expr)
template <typename Term>
bool check_term_Gen(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsGen(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Prefix(Dotted, Field*, Proc)
template <typename Term>
bool check_term_Prefix(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsPrefix(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Dotted<atermpp::aterm>))
    {
      std::cerr << "check_rule_Dotted" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_Field<atermpp::aterm>, 0))
    {
      std::cerr << "check_rule_Field" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Concat(Seq)
template <typename Term>
bool check_term_Concat(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsConcat(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Seq<atermpp::aterm>))
    {
      std::cerr << "check_rule_Seq" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Hiding(Proc, Set)
template <typename Term>
bool check_term_Hiding(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsHiding(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Set<atermpp::aterm>))
    {
      std::cerr << "check_rule_Set" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SequentialComposition(Proc, Proc)
template <typename Term>
bool check_term_SequentialComposition(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSequentialComposition(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// false()
template <typename Term>
bool check_term_false(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsfalse(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Min(Numb)
template <typename Term>
bool check_term_Min(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMin(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Numb<atermpp::aterm>))
    {
      std::cerr << "check_rule_Numb" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SimpleInput(Expr)
template <typename Term>
bool check_term_SimpleInput(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSimpleInput(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// SimpleTypeName(Name)
template <typename Term>
bool check_term_SimpleTypeName(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsSimpleTypeName(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// TypeProduct(Type, Type)
template <typename Term>
bool check_term_TypeProduct(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTypeProduct(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Type<atermpp::aterm>))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Type<atermpp::aterm>))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// BGen(Bool)
template <typename Term>
bool check_term_BGen(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsBGen(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Bool<atermpp::aterm>))
    {
      std::cerr << "check_rule_Bool" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// normalize()
template <typename Term>
bool check_term_normalize(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsnormalize(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Map(Dotted, Dotted)
template <typename Term>
bool check_term_Map(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMap(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Dotted<atermpp::aterm>))
    {
      std::cerr << "check_rule_Dotted" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Dotted<atermpp::aterm>))
    {
      std::cerr << "check_rule_Dotted" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// sbsim()
template <typename Term>
bool check_term_sbsim(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIssbsim(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Nil()
template <typename Term>
bool check_term_Nil(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsNil(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// TCheck(Proc, Test)
template <typename Term>
bool check_term_TCheck(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTCheck(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Test<atermpp::aterm>))
    {
      std::cerr << "check_rule_Test" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Greater(Expr, Expr)
template <typename Term>
bool check_term_Greater(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsGreater(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Assert(Check)
template <typename Term>
bool check_term_Assert(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsAssert(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Check<atermpp::aterm>))
    {
      std::cerr << "check_rule_Check" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// LocalDef(Defn+, Any)
template <typename Term>
bool check_term_LocalDef(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsLocalDef(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Defn<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Defn" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Any<atermpp::aterm>))
    {
      std::cerr << "check_rule_Any" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// tau_loop_factor()
template <typename Term>
bool check_term_tau_loop_factor(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIstau_loop_factor(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Name(Name)
template <typename Term>
bool check_term_Name(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsName(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// normal()
template <typename Term>
bool check_term_normal(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsnormal(a))
    return false;

  // check the children
  if (a.size() != 0)
    return false;

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Expr(Expr)
template <typename Term>
bool check_term_Expr(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsExpr(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Equal(Expr, Expr)
template <typename Term>
bool check_term_Equal(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsEqual(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// TypeTuple(Type+)
template <typename Term>
bool check_term_TypeTuple(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsTypeTuple(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Type<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Type" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Output(Expr)
template <typename Term>
bool check_term_Output(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsOutput(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Model(Model)
template <typename Term>
bool check_term_Model(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsModel(a))
    return false;

  // check the children
  if (a.size() != 1)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Model<atermpp::aterm>))
    {
      std::cerr << "check_rule_Model" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// Dot(Expr, Expr)
template <typename Term>
bool check_term_Dot(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsDot(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Expr<atermpp::aterm>))
    {
      std::cerr << "check_rule_Expr" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// MapsGens(Map+, Gen+)
template <typename Term>
bool check_term_MapsGens(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsMapsGens(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Map<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Map" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// RepLinkedParallel(Gen+, Proc, LinkPar)
template <typename Term>
bool check_term_RepLinkedParallel(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsRepLinkedParallel(a))
    return false;

  // check the children
  if (a.size() != 3)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_list_argument(a(0), check_rule_Gen<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_Gen" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(2), check_rule_LinkPar<atermpp::aterm>))
    {
      std::cerr << "check_rule_LinkPar" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// DataType(Name, VarType+)
template <typename Term>
bool check_term_DataType(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsDataType(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Name<atermpp::aterm>))
    {
      std::cerr << "check_rule_Name" << std::endl;
      return false;
    }
  if (!check_list_argument(a(1), check_rule_VarType<atermpp::aterm>, 1))
    {
      std::cerr << "check_rule_VarType" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

// UntimedTimeOut(Proc, Proc)
template <typename Term>
bool check_term_UntimedTimeOut(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
  // check the type of the term
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (!gsIsUntimedTimeOut(a))
    return false;

  // check the children
  if (a.size() != 2)
    return false;
#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS
  if (!check_term_argument(a(0), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
  if (!check_term_argument(a(1), check_rule_Proc<atermpp::aterm>))
    {
      std::cerr << "check_rule_Proc" << std::endl;
      return false;
    }
#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS

#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}//--- end generated code ---//

} // namespace detail

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DETAIL_SYNTAX_CHECKS_H
