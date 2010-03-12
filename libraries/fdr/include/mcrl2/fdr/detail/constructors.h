// Author(s): Generated code
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/detail/constructors.h
/// \brief Functions for constructing terms.

#ifndef MCRL2_FDR_DETAIL_CONSTRUCTORS_H
#define MCRL2_FDR_DETAIL_CONSTRUCTORS_H

#include "mcrl2/fdr/detail/utility.h"

namespace mcrl2 {

namespace fdr {

namespace detail {

//--- start generated code ---//
ATermAppl constructRepInternalChoice();
ATermAppl constructRCheck();
ATermAppl constructUnion();
ATermAppl constructTarg();
ATermAppl constructGreaterOrEqual();
ATermAppl constructBranch();
ATermAppl constructTest();
ATermAppl constructInclude();
ATermAppl constructSharing();
ATermAppl constructdeadlock_free();
ATermAppl constructTargGens();
ATermAppl constructdeterministic();
ATermAppl constructTarg0();
ATermAppl constructFDRSpec();
ATermAppl constructOr();
ATermAppl constructTypeName();
ATermAppl constructNumb();
ATermAppl constructTypeTyple();
ATermAppl constructNot();
ATermAppl constructMod();
ATermAppl constructLessOrEqual();
ATermAppl constructdivergence_free();
ATermAppl constructnormalise();
ATermAppl constructElem();
ATermAppl constructCat();
ATermAppl constructRepExternalChoice();
ATermAppl constructLength();
ATermAppl constructPlus();
ATermAppl constructT();
ATermAppl constructDiv();
ATermAppl constructMinus();
ATermAppl constructRepInterleave();
ATermAppl constructLinks();
ATermAppl constructLess();
ATermAppl constructInternalChoice();
ATermAppl constructLambdaExpr();
ATermAppl constructConditional();
ATermAppl constructFileName();
ATermAppl constructBool();
ATermAppl constructinter();
ATermAppl constructTypeExpr();
ATermAppl constructNotEqual();
ATermAppl constructset();
ATermAppl constructLinkedParallel();
ATermAppl constructproductions();
ATermAppl constructInput();
ATermAppl constructPrint();
ATermAppl constructTargGens0();
ATermAppl constructChannel();
ATermAppl constructOpenRange();
ATermAppl constructExternalChoice();
ATermAppl constructBoolGuard();
ATermAppl constructSubType();
ATermAppl constructLink();
ATermAppl constructExternal();
ATermAppl constructSKIP();
ATermAppl constructlivelock_free();
ATermAppl constructSimpleBranch();
ATermAppl constructTimes();
ATermAppl constructExprs();
ATermAppl constructSimpleChannel();
ATermAppl constructGreater();
ATermAppl constructRename();
ATermAppl constructmodel_compress();
ATermAppl constructInterleave();
ATermAppl constructClosedRange();
ATermAppl constructRepSharing();
ATermAppl constructextensions();
ATermAppl constructexplicate();
ATermAppl constructNull();
ATermAppl constructDotted();
ATermAppl constructSTOP();
ATermAppl constructCHAOS();
ATermAppl constructMember();
ATermAppl constructMaps();
ATermAppl constructLinksGens();
ATermAppl constructBCheck();
ATermAppl constructRepAlphaParallel();
ATermAppl constructdiff();
ATermAppl constructRepSequentialComposition();
ATermAppl constructProc();
ATermAppl constructEmpty();
ATermAppl constructNameType();
ATermAppl constructFD();
ATermAppl constructTuple();
ATermAppl constructunion();
ATermAppl constructTransparent();
ATermAppl constructCard();
ATermAppl constructdiamond();
ATermAppl constructF();
ATermAppl constructLambdaAppl();
ATermAppl constructAlphaParallel();
ATermAppl constructInterrupt();
ATermAppl constructInter();
ATermAppl constructtrue();
ATermAppl constructAssign();
ATermAppl constructLambda();
ATermAppl constructAnd();
ATermAppl constructSet();
ATermAppl constructSeq();
ATermAppl constructNumber();
ATermAppl constructGen();
ATermAppl constructPrefix();
ATermAppl constructConcat();
ATermAppl constructHiding();
ATermAppl constructSequentialComposition();
ATermAppl constructfalse();
ATermAppl constructMin();
ATermAppl constructSimpleInput();
ATermAppl constructSimpleTypeName();
ATermAppl constructTypeProduct();
ATermAppl constructnormalize();
ATermAppl constructMap();
ATermAppl constructsbsim();
ATermAppl constructNil();
ATermAppl constructTCheck();
ATermAppl constructRepLinkedParallel();
ATermAppl constructAssert();
ATermAppl constructLocalDef();
ATermAppl constructtau_loop_factor();
ATermAppl constructName();
ATermAppl constructnormal();
ATermAppl constructExpr();
ATermAppl constructEqual();
ATermAppl constructCommon();
ATermAppl constructOutput();
ATermAppl constructModel();
ATermAppl constructDot();
ATermAppl constructMapsGens();
ATermAppl constructDataType();
ATermAppl constructUntimedTimeOut();
ATermAppl constructDefn();
ATermAppl constructVarType();
ATermAppl constructType();
ATermAppl constructCheck();
ATermAppl constructRefined();
ATermAppl constructFailureModel();
ATermAppl constructTestType();
ATermAppl constructTrName();
ATermAppl constructAny();
ATermAppl constructField();
ATermAppl constructRenaming();
ATermAppl constructLinkPar();

// RepInternalChoice
inline
ATermAppl initConstructRepInternalChoice(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRepInternalChoice(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepInternalChoice()
{
  static ATermAppl t = initConstructRepInternalChoice(t);
  return t;
}

// RCheck
inline
ATermAppl initConstructRCheck(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunRCheck(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructRefined()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRCheck()
{
  static ATermAppl t = initConstructRCheck(t);
  return t;
}

// Union
inline
ATermAppl initConstructUnion(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunUnion(), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructUnion()
{
  static ATermAppl t = initConstructUnion(t);
  return t;
}

// Targ
inline
ATermAppl initConstructTarg(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTarg(), reinterpret_cast<ATerm>(constructTarg()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTarg()
{
  static ATermAppl t = initConstructTarg(t);
  return t;
}

// GreaterOrEqual
inline
ATermAppl initConstructGreaterOrEqual(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunGreaterOrEqual(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructGreaterOrEqual()
{
  static ATermAppl t = initConstructGreaterOrEqual(t);
  return t;
}

// Branch
inline
ATermAppl initConstructBranch(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBranch(), reinterpret_cast<ATerm>(constructName()), reinterpret_cast<ATerm>(constructType()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBranch()
{
  static ATermAppl t = initConstructBranch(t);
  return t;
}

// Test
inline
ATermAppl initConstructTest(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTest(), reinterpret_cast<ATerm>(constructTestType()), reinterpret_cast<ATerm>(constructFailureModel()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTest()
{
  static ATermAppl t = initConstructTest(t);
  return t;
}

// Include
inline
ATermAppl initConstructInclude(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunInclude(), reinterpret_cast<ATerm>(constructFileName()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInclude()
{
  static ATermAppl t = initConstructInclude(t);
  return t;
}

// Sharing
inline
ATermAppl initConstructSharing(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunSharing(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSharing()
{
  static ATermAppl t = initConstructSharing(t);
  return t;
}

// deadlock_free
inline
ATermAppl initConstructdeadlock_free(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFundeadlock_free());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructdeadlock_free()
{
  static ATermAppl t = initConstructdeadlock_free(t);
  return t;
}

// TargGens
inline
ATermAppl initConstructTargGens(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTargGens(), reinterpret_cast<ATerm>(constructTarg()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTargGens()
{
  static ATermAppl t = initConstructTargGens(t);
  return t;
}

// deterministic
inline
ATermAppl initConstructdeterministic(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFundeterministic());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructdeterministic()
{
  static ATermAppl t = initConstructdeterministic(t);
  return t;
}

// Targ0
inline
ATermAppl initConstructTarg0(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTarg0(), reinterpret_cast<ATerm>(constructTarg()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTarg0()
{
  static ATermAppl t = initConstructTarg0(t);
  return t;
}

// FDRSpec
inline
ATermAppl initConstructFDRSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunFDRSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructFDRSpec()
{
  static ATermAppl t = initConstructFDRSpec(t);
  return t;
}

// Or
inline
ATermAppl initConstructOr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunOr(), reinterpret_cast<ATerm>(constructBool()), reinterpret_cast<ATerm>(constructBool()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructOr()
{
  static ATermAppl t = initConstructOr(t);
  return t;
}

// TypeName
inline
ATermAppl initConstructTypeName(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTypeName(), reinterpret_cast<ATerm>(constructName()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTypeName()
{
  static ATermAppl t = initConstructTypeName(t);
  return t;
}

// Numb
inline
ATermAppl initConstructNumb(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunNumb(), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNumb()
{
  static ATermAppl t = initConstructNumb(t);
  return t;
}

// TypeTyple
inline
ATermAppl initConstructTypeTyple(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTypeTyple(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTypeTyple()
{
  static ATermAppl t = initConstructTypeTyple(t);
  return t;
}

// Not
inline
ATermAppl initConstructNot(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunNot(), reinterpret_cast<ATerm>(constructBool()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNot()
{
  static ATermAppl t = initConstructNot(t);
  return t;
}

// Mod
inline
ATermAppl initConstructMod(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunMod(), reinterpret_cast<ATerm>(constructNumb()), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMod()
{
  static ATermAppl t = initConstructMod(t);
  return t;
}

// LessOrEqual
inline
ATermAppl initConstructLessOrEqual(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLessOrEqual(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLessOrEqual()
{
  static ATermAppl t = initConstructLessOrEqual(t);
  return t;
}

// divergence_free
inline
ATermAppl initConstructdivergence_free(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFundivergence_free());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructdivergence_free()
{
  static ATermAppl t = initConstructdivergence_free(t);
  return t;
}

// normalise
inline
ATermAppl initConstructnormalise(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunnormalise());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructnormalise()
{
  static ATermAppl t = initConstructnormalise(t);
  return t;
}

// Elem
inline
ATermAppl initConstructElem(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunElem(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructElem()
{
  static ATermAppl t = initConstructElem(t);
  return t;
}

// Cat
inline
ATermAppl initConstructCat(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunCat(), reinterpret_cast<ATerm>(constructSeq()), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructCat()
{
  static ATermAppl t = initConstructCat(t);
  return t;
}

// RepExternalChoice
inline
ATermAppl initConstructRepExternalChoice(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRepExternalChoice(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepExternalChoice()
{
  static ATermAppl t = initConstructRepExternalChoice(t);
  return t;
}

// Length
inline
ATermAppl initConstructLength(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunLength(), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLength()
{
  static ATermAppl t = initConstructLength(t);
  return t;
}

// Plus
inline
ATermAppl initConstructPlus(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPlus(), reinterpret_cast<ATerm>(constructNumb()), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPlus()
{
  static ATermAppl t = initConstructPlus(t);
  return t;
}

// T
inline
ATermAppl initConstructT(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunT());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructT()
{
  static ATermAppl t = initConstructT(t);
  return t;
}

// Div
inline
ATermAppl initConstructDiv(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDiv(), reinterpret_cast<ATerm>(constructNumb()), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDiv()
{
  static ATermAppl t = initConstructDiv(t);
  return t;
}

// Minus
inline
ATermAppl initConstructMinus(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunMinus(), reinterpret_cast<ATerm>(constructNumb()), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMinus()
{
  static ATermAppl t = initConstructMinus(t);
  return t;
}

// RepInterleave
inline
ATermAppl initConstructRepInterleave(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRepInterleave(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepInterleave()
{
  static ATermAppl t = initConstructRepInterleave(t);
  return t;
}

// Links
inline
ATermAppl initConstructLinks(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunLinks(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinks()
{
  static ATermAppl t = initConstructLinks(t);
  return t;
}

// Less
inline
ATermAppl initConstructLess(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLess(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLess()
{
  static ATermAppl t = initConstructLess(t);
  return t;
}

// InternalChoice
inline
ATermAppl initConstructInternalChoice(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunInternalChoice(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInternalChoice()
{
  static ATermAppl t = initConstructInternalChoice(t);
  return t;
}

// LambdaExpr
inline
ATermAppl initConstructLambdaExpr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLambdaExpr(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructAny()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLambdaExpr()
{
  static ATermAppl t = initConstructLambdaExpr(t);
  return t;
}

// Conditional
inline
ATermAppl initConstructConditional(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunConditional(), reinterpret_cast<ATerm>(constructBool()), reinterpret_cast<ATerm>(constructAny()), reinterpret_cast<ATerm>(constructAny()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructConditional()
{
  static ATermAppl t = initConstructConditional(t);
  return t;
}

// FileName
inline
ATermAppl initConstructFileName(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunFileName(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructFileName()
{
  static ATermAppl t = initConstructFileName(t);
  return t;
}

// Bool
inline
ATermAppl initConstructBool(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunBool(), reinterpret_cast<ATerm>(constructBool()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBool()
{
  static ATermAppl t = initConstructBool(t);
  return t;
}

// inter
inline
ATermAppl initConstructinter(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFuninter(), reinterpret_cast<ATerm>(constructSet()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructinter()
{
  static ATermAppl t = initConstructinter(t);
  return t;
}

// TypeExpr
inline
ATermAppl initConstructTypeExpr(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTypeExpr(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTypeExpr()
{
  static ATermAppl t = initConstructTypeExpr(t);
  return t;
}

// NotEqual
inline
ATermAppl initConstructNotEqual(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunNotEqual(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNotEqual()
{
  static ATermAppl t = initConstructNotEqual(t);
  return t;
}

// set
inline
ATermAppl initConstructset(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunset(), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructset()
{
  static ATermAppl t = initConstructset(t);
  return t;
}

// LinkedParallel
inline
ATermAppl initConstructLinkedParallel(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunLinkedParallel(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructLinkPar()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinkedParallel()
{
  static ATermAppl t = initConstructLinkedParallel(t);
  return t;
}

// productions
inline
ATermAppl initConstructproductions(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunproductions(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructproductions()
{
  static ATermAppl t = initConstructproductions(t);
  return t;
}

// Input
inline
ATermAppl initConstructInput(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunInput(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInput()
{
  static ATermAppl t = initConstructInput(t);
  return t;
}

// Print
inline
ATermAppl initConstructPrint(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunPrint(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPrint()
{
  static ATermAppl t = initConstructPrint(t);
  return t;
}

// TargGens0
inline
ATermAppl initConstructTargGens0(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTargGens0(), reinterpret_cast<ATerm>(constructTarg()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTargGens0()
{
  static ATermAppl t = initConstructTargGens0(t);
  return t;
}

// Channel
inline
ATermAppl initConstructChannel(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunChannel(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructType()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChannel()
{
  static ATermAppl t = initConstructChannel(t);
  return t;
}

// OpenRange
inline
ATermAppl initConstructOpenRange(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunOpenRange(), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructOpenRange()
{
  static ATermAppl t = initConstructOpenRange(t);
  return t;
}

// ExternalChoice
inline
ATermAppl initConstructExternalChoice(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunExternalChoice(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExternalChoice()
{
  static ATermAppl t = initConstructExternalChoice(t);
  return t;
}

// BoolGuard
inline
ATermAppl initConstructBoolGuard(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBoolGuard(), reinterpret_cast<ATerm>(constructBool()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBoolGuard()
{
  static ATermAppl t = initConstructBoolGuard(t);
  return t;
}

// SubType
inline
ATermAppl initConstructSubType(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSubType(), reinterpret_cast<ATerm>(constructName()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSubType()
{
  static ATermAppl t = initConstructSubType(t);
  return t;
}

// Link
inline
ATermAppl initConstructLink(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLink(), reinterpret_cast<ATerm>(constructDotted()), reinterpret_cast<ATerm>(constructDotted()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLink()
{
  static ATermAppl t = initConstructLink(t);
  return t;
}

// External
inline
ATermAppl initConstructExternal(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunExternal(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExternal()
{
  static ATermAppl t = initConstructExternal(t);
  return t;
}

// SKIP
inline
ATermAppl initConstructSKIP(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSKIP());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSKIP()
{
  static ATermAppl t = initConstructSKIP(t);
  return t;
}

// livelock_free
inline
ATermAppl initConstructlivelock_free(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunlivelock_free());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructlivelock_free()
{
  static ATermAppl t = initConstructlivelock_free(t);
  return t;
}

// SimpleBranch
inline
ATermAppl initConstructSimpleBranch(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSimpleBranch(), reinterpret_cast<ATerm>(constructName()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSimpleBranch()
{
  static ATermAppl t = initConstructSimpleBranch(t);
  return t;
}

// Times
inline
ATermAppl initConstructTimes(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTimes(), reinterpret_cast<ATerm>(constructNumb()), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTimes()
{
  static ATermAppl t = initConstructTimes(t);
  return t;
}

// Exprs
inline
ATermAppl initConstructExprs(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunExprs(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExprs()
{
  static ATermAppl t = initConstructExprs(t);
  return t;
}

// SimpleChannel
inline
ATermAppl initConstructSimpleChannel(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSimpleChannel(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSimpleChannel()
{
  static ATermAppl t = initConstructSimpleChannel(t);
  return t;
}

// Greater
inline
ATermAppl initConstructGreater(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunGreater(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructGreater()
{
  static ATermAppl t = initConstructGreater(t);
  return t;
}

// Rename
inline
ATermAppl initConstructRename(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRename(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructRenaming()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRename()
{
  static ATermAppl t = initConstructRename(t);
  return t;
}

// model_compress
inline
ATermAppl initConstructmodel_compress(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunmodel_compress());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructmodel_compress()
{
  static ATermAppl t = initConstructmodel_compress(t);
  return t;
}

// Interleave
inline
ATermAppl initConstructInterleave(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunInterleave(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInterleave()
{
  static ATermAppl t = initConstructInterleave(t);
  return t;
}

// ClosedRange
inline
ATermAppl initConstructClosedRange(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunClosedRange(), reinterpret_cast<ATerm>(constructNumb()), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructClosedRange()
{
  static ATermAppl t = initConstructClosedRange(t);
  return t;
}

// RepSharing
inline
ATermAppl initConstructRepSharing(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunRepSharing(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepSharing()
{
  static ATermAppl t = initConstructRepSharing(t);
  return t;
}

// extensions
inline
ATermAppl initConstructextensions(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunextensions(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructextensions()
{
  static ATermAppl t = initConstructextensions(t);
  return t;
}

// explicate
inline
ATermAppl initConstructexplicate(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunexplicate());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructexplicate()
{
  static ATermAppl t = initConstructexplicate(t);
  return t;
}

// Null
inline
ATermAppl initConstructNull(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunNull(), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNull()
{
  static ATermAppl t = initConstructNull(t);
  return t;
}

// Dotted
inline
ATermAppl initConstructDotted(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunDotted(), reinterpret_cast<ATerm>(constructDotted()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDotted()
{
  static ATermAppl t = initConstructDotted(t);
  return t;
}

// STOP
inline
ATermAppl initConstructSTOP(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSTOP());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSTOP()
{
  static ATermAppl t = initConstructSTOP(t);
  return t;
}

// CHAOS
inline
ATermAppl initConstructCHAOS(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunCHAOS(), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructCHAOS()
{
  static ATermAppl t = initConstructCHAOS(t);
  return t;
}

// Member
inline
ATermAppl initConstructMember(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunMember(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMember()
{
  static ATermAppl t = initConstructMember(t);
  return t;
}

// Maps
inline
ATermAppl initConstructMaps(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunMaps(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMaps()
{
  static ATermAppl t = initConstructMaps(t);
  return t;
}

// LinksGens
inline
ATermAppl initConstructLinksGens(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLinksGens(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinksGens()
{
  static ATermAppl t = initConstructLinksGens(t);
  return t;
}

// BCheck
inline
ATermAppl initConstructBCheck(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunBCheck(), reinterpret_cast<ATerm>(constructBool()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBCheck()
{
  static ATermAppl t = initConstructBCheck(t);
  return t;
}

// RepAlphaParallel
inline
ATermAppl initConstructRepAlphaParallel(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunRepAlphaParallel(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepAlphaParallel()
{
  static ATermAppl t = initConstructRepAlphaParallel(t);
  return t;
}

// diff
inline
ATermAppl initConstructdiff(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFundiff(), reinterpret_cast<ATerm>(constructSet()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructdiff()
{
  static ATermAppl t = initConstructdiff(t);
  return t;
}

// RepSequentialComposition
inline
ATermAppl initConstructRepSequentialComposition(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRepSequentialComposition(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepSequentialComposition()
{
  static ATermAppl t = initConstructRepSequentialComposition(t);
  return t;
}

// Proc
inline
ATermAppl initConstructProc(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunProc(), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProc()
{
  static ATermAppl t = initConstructProc(t);
  return t;
}

// Empty
inline
ATermAppl initConstructEmpty(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunEmpty(), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructEmpty()
{
  static ATermAppl t = initConstructEmpty(t);
  return t;
}

// NameType
inline
ATermAppl initConstructNameType(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunNameType(), reinterpret_cast<ATerm>(constructName()), reinterpret_cast<ATerm>(constructType()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNameType()
{
  static ATermAppl t = initConstructNameType(t);
  return t;
}

// FD
inline
ATermAppl initConstructFD(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunFD());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructFD()
{
  static ATermAppl t = initConstructFD(t);
  return t;
}

// Tuple
inline
ATermAppl initConstructTuple(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTuple(), reinterpret_cast<ATerm>(constructTuple()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTuple()
{
  static ATermAppl t = initConstructTuple(t);
  return t;
}

// union
inline
ATermAppl initConstructunion(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFununion(), reinterpret_cast<ATerm>(constructSet()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructunion()
{
  static ATermAppl t = initConstructunion(t);
  return t;
}

// Transparent
inline
ATermAppl initConstructTransparent(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTransparent(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTransparent()
{
  static ATermAppl t = initConstructTransparent(t);
  return t;
}

// Card
inline
ATermAppl initConstructCard(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunCard(), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructCard()
{
  static ATermAppl t = initConstructCard(t);
  return t;
}

// diamond
inline
ATermAppl initConstructdiamond(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFundiamond());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructdiamond()
{
  static ATermAppl t = initConstructdiamond(t);
  return t;
}

// F
inline
ATermAppl initConstructF(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunF());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructF()
{
  static ATermAppl t = initConstructF(t);
  return t;
}

// LambdaAppl
inline
ATermAppl initConstructLambdaAppl(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLambdaAppl(), reinterpret_cast<ATerm>(constructLambda()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLambdaAppl()
{
  static ATermAppl t = initConstructLambdaAppl(t);
  return t;
}

// AlphaParallel
inline
ATermAppl initConstructAlphaParallel(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunAlphaParallel(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructSet()), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAlphaParallel()
{
  static ATermAppl t = initConstructAlphaParallel(t);
  return t;
}

// Interrupt
inline
ATermAppl initConstructInterrupt(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunInterrupt(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInterrupt()
{
  static ATermAppl t = initConstructInterrupt(t);
  return t;
}

// Inter
inline
ATermAppl initConstructInter(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunInter(), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInter()
{
  static ATermAppl t = initConstructInter(t);
  return t;
}

// true
inline
ATermAppl initConstructtrue(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFuntrue());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructtrue()
{
  static ATermAppl t = initConstructtrue(t);
  return t;
}

// Assign
inline
ATermAppl initConstructAssign(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAssign(), reinterpret_cast<ATerm>(constructAny()), reinterpret_cast<ATerm>(constructAny()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAssign()
{
  static ATermAppl t = initConstructAssign(t);
  return t;
}

// Lambda
inline
ATermAppl initConstructLambda(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunLambda(), reinterpret_cast<ATerm>(constructLambda()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLambda()
{
  static ATermAppl t = initConstructLambda(t);
  return t;
}

// And
inline
ATermAppl initConstructAnd(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAnd(), reinterpret_cast<ATerm>(constructBool()), reinterpret_cast<ATerm>(constructBool()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAnd()
{
  static ATermAppl t = initConstructAnd(t);
  return t;
}

// Set
inline
ATermAppl initConstructSet(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSet(), reinterpret_cast<ATerm>(constructSet()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSet()
{
  static ATermAppl t = initConstructSet(t);
  return t;
}

// Seq
inline
ATermAppl initConstructSeq(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSeq(), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSeq()
{
  static ATermAppl t = initConstructSeq(t);
  return t;
}

// Number
inline
ATermAppl initConstructNumber(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunNumber(), reinterpret_cast<ATerm>(constructNumber()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNumber()
{
  static ATermAppl t = initConstructNumber(t);
  return t;
}

// Gen
inline
ATermAppl initConstructGen(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunGen(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructGen()
{
  static ATermAppl t = initConstructGen(t);
  return t;
}

// Prefix
inline
ATermAppl initConstructPrefix(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunPrefix(), reinterpret_cast<ATerm>(constructDotted()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPrefix()
{
  static ATermAppl t = initConstructPrefix(t);
  return t;
}

// Concat
inline
ATermAppl initConstructConcat(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunConcat(), reinterpret_cast<ATerm>(constructSeq()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructConcat()
{
  static ATermAppl t = initConstructConcat(t);
  return t;
}

// Hiding
inline
ATermAppl initConstructHiding(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunHiding(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructHiding()
{
  static ATermAppl t = initConstructHiding(t);
  return t;
}

// SequentialComposition
inline
ATermAppl initConstructSequentialComposition(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSequentialComposition(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSequentialComposition()
{
  static ATermAppl t = initConstructSequentialComposition(t);
  return t;
}

// false
inline
ATermAppl initConstructfalse(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunfalse());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructfalse()
{
  static ATermAppl t = initConstructfalse(t);
  return t;
}

// Min
inline
ATermAppl initConstructMin(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunMin(), reinterpret_cast<ATerm>(constructNumb()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMin()
{
  static ATermAppl t = initConstructMin(t);
  return t;
}

// SimpleInput
inline
ATermAppl initConstructSimpleInput(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSimpleInput(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSimpleInput()
{
  static ATermAppl t = initConstructSimpleInput(t);
  return t;
}

// SimpleTypeName
inline
ATermAppl initConstructSimpleTypeName(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSimpleTypeName(), reinterpret_cast<ATerm>(constructName()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSimpleTypeName()
{
  static ATermAppl t = initConstructSimpleTypeName(t);
  return t;
}

// TypeProduct
inline
ATermAppl initConstructTypeProduct(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTypeProduct(), reinterpret_cast<ATerm>(constructType()), reinterpret_cast<ATerm>(constructType()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTypeProduct()
{
  static ATermAppl t = initConstructTypeProduct(t);
  return t;
}

// normalize
inline
ATermAppl initConstructnormalize(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunnormalize());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructnormalize()
{
  static ATermAppl t = initConstructnormalize(t);
  return t;
}

// Map
inline
ATermAppl initConstructMap(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunMap(), reinterpret_cast<ATerm>(constructDotted()), reinterpret_cast<ATerm>(constructDotted()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMap()
{
  static ATermAppl t = initConstructMap(t);
  return t;
}

// sbsim
inline
ATermAppl initConstructsbsim(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunsbsim());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructsbsim()
{
  static ATermAppl t = initConstructsbsim(t);
  return t;
}

// Nil
inline
ATermAppl initConstructNil(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunNil());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNil()
{
  static ATermAppl t = initConstructNil(t);
  return t;
}

// TCheck
inline
ATermAppl initConstructTCheck(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTCheck(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructTest()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTCheck()
{
  static ATermAppl t = initConstructTCheck(t);
  return t;
}

// RepLinkedParallel
inline
ATermAppl initConstructRepLinkedParallel(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunRepLinkedParallel(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructLinkPar()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRepLinkedParallel()
{
  static ATermAppl t = initConstructRepLinkedParallel(t);
  return t;
}

// Assert
inline
ATermAppl initConstructAssert(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunAssert(), reinterpret_cast<ATerm>(constructCheck()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAssert()
{
  static ATermAppl t = initConstructAssert(t);
  return t;
}

// LocalDef
inline
ATermAppl initConstructLocalDef(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLocalDef(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructAny()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLocalDef()
{
  static ATermAppl t = initConstructLocalDef(t);
  return t;
}

// tau_loop_factor
inline
ATermAppl initConstructtau_loop_factor(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFuntau_loop_factor());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructtau_loop_factor()
{
  static ATermAppl t = initConstructtau_loop_factor(t);
  return t;
}

// Name
inline
ATermAppl initConstructName(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunName(), reinterpret_cast<ATerm>(constructName()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructName()
{
  static ATermAppl t = initConstructName(t);
  return t;
}

// normal
inline
ATermAppl initConstructnormal(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunnormal());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructnormal()
{
  static ATermAppl t = initConstructnormal(t);
  return t;
}

// Expr
inline
ATermAppl initConstructExpr(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunExpr(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExpr()
{
  static ATermAppl t = initConstructExpr(t);
  return t;
}

// Equal
inline
ATermAppl initConstructEqual(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunEqual(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructEqual()
{
  static ATermAppl t = initConstructEqual(t);
  return t;
}

// Common
inline
ATermAppl initConstructCommon(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunCommon(), reinterpret_cast<ATerm>(constructCommon()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructCommon()
{
  static ATermAppl t = initConstructCommon(t);
  return t;
}

// Output
inline
ATermAppl initConstructOutput(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunOutput(), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructOutput()
{
  static ATermAppl t = initConstructOutput(t);
  return t;
}

// Model
inline
ATermAppl initConstructModel(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunModel(), reinterpret_cast<ATerm>(constructModel()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructModel()
{
  static ATermAppl t = initConstructModel(t);
  return t;
}

// Dot
inline
ATermAppl initConstructDot(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDot(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDot()
{
  static ATermAppl t = initConstructDot(t);
  return t;
}

// MapsGens
inline
ATermAppl initConstructMapsGens(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunMapsGens(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMapsGens()
{
  static ATermAppl t = initConstructMapsGens(t);
  return t;
}

// DataType
inline
ATermAppl initConstructDataType(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataType(), reinterpret_cast<ATerm>(constructName()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataType()
{
  static ATermAppl t = initConstructDataType(t);
  return t;
}

// UntimedTimeOut
inline
ATermAppl initConstructUntimedTimeOut(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunUntimedTimeOut(), reinterpret_cast<ATerm>(constructProc()), reinterpret_cast<ATerm>(constructProc()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructUntimedTimeOut()
{
  static ATermAppl t = initConstructUntimedTimeOut(t);
  return t;
}

// Defn
inline
ATermAppl constructDefn()
{
  return constructAssign();
}

// VarType
inline
ATermAppl constructVarType()
{
  return constructSimpleBranch();
}

// Type
inline
ATermAppl constructType()
{
  return constructTypeProduct();
}

// Check
inline
ATermAppl constructCheck()
{
  return constructBCheck();
}

// Refined
inline
ATermAppl constructRefined()
{
  return constructModel();
}

// FailureModel
inline
ATermAppl constructFailureModel()
{
  return constructF();
}

// TestType
inline
ATermAppl constructTestType()
{
  return constructdeterministic();
}

// TrName
inline
ATermAppl constructTrName()
{
  return constructnormal();
}

// Any
inline
ATermAppl constructAny()
{
  return constructExpr();
}

// Field
inline
ATermAppl constructField()
{
  return constructSimpleInput();
}

// Renaming
inline
ATermAppl constructRenaming()
{
  return constructMaps();
}

// LinkPar
inline
ATermAppl constructLinkPar()
{
  return constructLinks();
}

//--- end generated code ---//

} // namespace detail

} // namespace fdr

} // mcrl2

#endif // MCRL2_FDR_DETAIL_CONSTRUCTORS_H
