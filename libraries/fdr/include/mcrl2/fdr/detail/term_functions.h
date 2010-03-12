// Author(s): Generated code
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/detail/term_functions.h
/// \brief Functions for term classes.

#ifndef MCRL2_FDR_DETAIL_TERM_FUNCTIONS_H
#define MCRL2_FDR_DETAIL_TERM_FUNCTIONS_H

#include "mcrl2/fdr/detail/utility.h"

namespace mcrl2 {

namespace fdr {

namespace detail {

//--- start generated code ---//
// AlphaParallel
inline
AFun initAFunAlphaParallel(AFun& f)
{
  f = ATmakeAFun("AlphaParallel", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAlphaParallel()
{
  static AFun AFunAlphaParallel = initAFunAlphaParallel(AFunAlphaParallel);
  return AFunAlphaParallel;
}

inline
bool gsIsAlphaParallel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAlphaParallel();
}

// And
inline
AFun initAFunAnd(AFun& f)
{
  f = ATmakeAFun("And", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAnd()
{
  static AFun AFunAnd = initAFunAnd(AFunAnd);
  return AFunAnd;
}

inline
bool gsIsAnd(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAnd();
}

// Assert
inline
AFun initAFunAssert(AFun& f)
{
  f = ATmakeAFun("Assert", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAssert()
{
  static AFun AFunAssert = initAFunAssert(AFunAssert);
  return AFunAssert;
}

inline
bool gsIsAssert(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAssert();
}

// Assign
inline
AFun initAFunAssign(AFun& f)
{
  f = ATmakeAFun("Assign", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAssign()
{
  static AFun AFunAssign = initAFunAssign(AFunAssign);
  return AFunAssign;
}

inline
bool gsIsAssign(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAssign();
}

// BCheck
inline
AFun initAFunBCheck(AFun& f)
{
  f = ATmakeAFun("BCheck", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBCheck()
{
  static AFun AFunBCheck = initAFunBCheck(AFunBCheck);
  return AFunBCheck;
}

inline
bool gsIsBCheck(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBCheck();
}

// Bool
inline
AFun initAFunBool(AFun& f)
{
  f = ATmakeAFun("Bool", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBool()
{
  static AFun AFunBool = initAFunBool(AFunBool);
  return AFunBool;
}

inline
bool gsIsBool(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBool();
}

// BoolGuard
inline
AFun initAFunBoolGuard(AFun& f)
{
  f = ATmakeAFun("BoolGuard", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBoolGuard()
{
  static AFun AFunBoolGuard = initAFunBoolGuard(AFunBoolGuard);
  return AFunBoolGuard;
}

inline
bool gsIsBoolGuard(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBoolGuard();
}

// Branch
inline
AFun initAFunBranch(AFun& f)
{
  f = ATmakeAFun("Branch", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBranch()
{
  static AFun AFunBranch = initAFunBranch(AFunBranch);
  return AFunBranch;
}

inline
bool gsIsBranch(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBranch();
}

// CHAOS
inline
AFun initAFunCHAOS(AFun& f)
{
  f = ATmakeAFun("CHAOS", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunCHAOS()
{
  static AFun AFunCHAOS = initAFunCHAOS(AFunCHAOS);
  return AFunCHAOS;
}

inline
bool gsIsCHAOS(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunCHAOS();
}

// Card
inline
AFun initAFunCard(AFun& f)
{
  f = ATmakeAFun("Card", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunCard()
{
  static AFun AFunCard = initAFunCard(AFunCard);
  return AFunCard;
}

inline
bool gsIsCard(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunCard();
}

// Cat
inline
AFun initAFunCat(AFun& f)
{
  f = ATmakeAFun("Cat", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunCat()
{
  static AFun AFunCat = initAFunCat(AFunCat);
  return AFunCat;
}

inline
bool gsIsCat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunCat();
}

// Channel
inline
AFun initAFunChannel(AFun& f)
{
  f = ATmakeAFun("Channel", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChannel()
{
  static AFun AFunChannel = initAFunChannel(AFunChannel);
  return AFunChannel;
}

inline
bool gsIsChannel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChannel();
}

// ClosedRange
inline
AFun initAFunClosedRange(AFun& f)
{
  f = ATmakeAFun("ClosedRange", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunClosedRange()
{
  static AFun AFunClosedRange = initAFunClosedRange(AFunClosedRange);
  return AFunClosedRange;
}

inline
bool gsIsClosedRange(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunClosedRange();
}

// Common
inline
AFun initAFunCommon(AFun& f)
{
  f = ATmakeAFun("Common", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunCommon()
{
  static AFun AFunCommon = initAFunCommon(AFunCommon);
  return AFunCommon;
}

inline
bool gsIsCommon(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunCommon();
}

// Concat
inline
AFun initAFunConcat(AFun& f)
{
  f = ATmakeAFun("Concat", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunConcat()
{
  static AFun AFunConcat = initAFunConcat(AFunConcat);
  return AFunConcat;
}

inline
bool gsIsConcat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunConcat();
}

// Conditional
inline
AFun initAFunConditional(AFun& f)
{
  f = ATmakeAFun("Conditional", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunConditional()
{
  static AFun AFunConditional = initAFunConditional(AFunConditional);
  return AFunConditional;
}

inline
bool gsIsConditional(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunConditional();
}

// DataType
inline
AFun initAFunDataType(AFun& f)
{
  f = ATmakeAFun("DataType", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataType()
{
  static AFun AFunDataType = initAFunDataType(AFunDataType);
  return AFunDataType;
}

inline
bool gsIsDataType(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataType();
}

// Div
inline
AFun initAFunDiv(AFun& f)
{
  f = ATmakeAFun("Div", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDiv()
{
  static AFun AFunDiv = initAFunDiv(AFunDiv);
  return AFunDiv;
}

inline
bool gsIsDiv(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDiv();
}

// Dot
inline
AFun initAFunDot(AFun& f)
{
  f = ATmakeAFun("Dot", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDot()
{
  static AFun AFunDot = initAFunDot(AFunDot);
  return AFunDot;
}

inline
bool gsIsDot(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDot();
}

// Dotted
inline
AFun initAFunDotted(AFun& f)
{
  f = ATmakeAFun("Dotted", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDotted()
{
  static AFun AFunDotted = initAFunDotted(AFunDotted);
  return AFunDotted;
}

inline
bool gsIsDotted(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDotted();
}

// Elem
inline
AFun initAFunElem(AFun& f)
{
  f = ATmakeAFun("Elem", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunElem()
{
  static AFun AFunElem = initAFunElem(AFunElem);
  return AFunElem;
}

inline
bool gsIsElem(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunElem();
}

// Empty
inline
AFun initAFunEmpty(AFun& f)
{
  f = ATmakeAFun("Empty", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunEmpty()
{
  static AFun AFunEmpty = initAFunEmpty(AFunEmpty);
  return AFunEmpty;
}

inline
bool gsIsEmpty(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunEmpty();
}

// Equal
inline
AFun initAFunEqual(AFun& f)
{
  f = ATmakeAFun("Equal", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunEqual()
{
  static AFun AFunEqual = initAFunEqual(AFunEqual);
  return AFunEqual;
}

inline
bool gsIsEqual(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunEqual();
}

// Expr
inline
AFun initAFunExpr(AFun& f)
{
  f = ATmakeAFun("Expr", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExpr()
{
  static AFun AFunExpr = initAFunExpr(AFunExpr);
  return AFunExpr;
}

inline
bool gsIsExpr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExpr();
}

// Exprs
inline
AFun initAFunExprs(AFun& f)
{
  f = ATmakeAFun("Exprs", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExprs()
{
  static AFun AFunExprs = initAFunExprs(AFunExprs);
  return AFunExprs;
}

inline
bool gsIsExprs(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExprs();
}

// External
inline
AFun initAFunExternal(AFun& f)
{
  f = ATmakeAFun("External", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExternal()
{
  static AFun AFunExternal = initAFunExternal(AFunExternal);
  return AFunExternal;
}

inline
bool gsIsExternal(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExternal();
}

// ExternalChoice
inline
AFun initAFunExternalChoice(AFun& f)
{
  f = ATmakeAFun("ExternalChoice", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExternalChoice()
{
  static AFun AFunExternalChoice = initAFunExternalChoice(AFunExternalChoice);
  return AFunExternalChoice;
}

inline
bool gsIsExternalChoice(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExternalChoice();
}

// F
inline
AFun initAFunF(AFun& f)
{
  f = ATmakeAFun("F", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunF()
{
  static AFun AFunF = initAFunF(AFunF);
  return AFunF;
}

inline
bool gsIsF(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunF();
}

// FD
inline
AFun initAFunFD(AFun& f)
{
  f = ATmakeAFun("FD", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunFD()
{
  static AFun AFunFD = initAFunFD(AFunFD);
  return AFunFD;
}

inline
bool gsIsFD(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunFD();
}

// FDRSpec
inline
AFun initAFunFDRSpec(AFun& f)
{
  f = ATmakeAFun("FDRSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunFDRSpec()
{
  static AFun AFunFDRSpec = initAFunFDRSpec(AFunFDRSpec);
  return AFunFDRSpec;
}

inline
bool gsIsFDRSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunFDRSpec();
}

// FileName
inline
AFun initAFunFileName(AFun& f)
{
  f = ATmakeAFun("FileName", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunFileName()
{
  static AFun AFunFileName = initAFunFileName(AFunFileName);
  return AFunFileName;
}

inline
bool gsIsFileName(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunFileName();
}

// Gen
inline
AFun initAFunGen(AFun& f)
{
  f = ATmakeAFun("Gen", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunGen()
{
  static AFun AFunGen = initAFunGen(AFunGen);
  return AFunGen;
}

inline
bool gsIsGen(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunGen();
}

// Greater
inline
AFun initAFunGreater(AFun& f)
{
  f = ATmakeAFun("Greater", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunGreater()
{
  static AFun AFunGreater = initAFunGreater(AFunGreater);
  return AFunGreater;
}

inline
bool gsIsGreater(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunGreater();
}

// GreaterOrEqual
inline
AFun initAFunGreaterOrEqual(AFun& f)
{
  f = ATmakeAFun("GreaterOrEqual", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunGreaterOrEqual()
{
  static AFun AFunGreaterOrEqual = initAFunGreaterOrEqual(AFunGreaterOrEqual);
  return AFunGreaterOrEqual;
}

inline
bool gsIsGreaterOrEqual(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunGreaterOrEqual();
}

// Hiding
inline
AFun initAFunHiding(AFun& f)
{
  f = ATmakeAFun("Hiding", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunHiding()
{
  static AFun AFunHiding = initAFunHiding(AFunHiding);
  return AFunHiding;
}

inline
bool gsIsHiding(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunHiding();
}

// Include
inline
AFun initAFunInclude(AFun& f)
{
  f = ATmakeAFun("Include", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInclude()
{
  static AFun AFunInclude = initAFunInclude(AFunInclude);
  return AFunInclude;
}

inline
bool gsIsInclude(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInclude();
}

// Input
inline
AFun initAFunInput(AFun& f)
{
  f = ATmakeAFun("Input", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInput()
{
  static AFun AFunInput = initAFunInput(AFunInput);
  return AFunInput;
}

inline
bool gsIsInput(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInput();
}

// Inter
inline
AFun initAFunInter(AFun& f)
{
  f = ATmakeAFun("Inter", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInter()
{
  static AFun AFunInter = initAFunInter(AFunInter);
  return AFunInter;
}

inline
bool gsIsInter(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInter();
}

// Interleave
inline
AFun initAFunInterleave(AFun& f)
{
  f = ATmakeAFun("Interleave", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInterleave()
{
  static AFun AFunInterleave = initAFunInterleave(AFunInterleave);
  return AFunInterleave;
}

inline
bool gsIsInterleave(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInterleave();
}

// InternalChoice
inline
AFun initAFunInternalChoice(AFun& f)
{
  f = ATmakeAFun("InternalChoice", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInternalChoice()
{
  static AFun AFunInternalChoice = initAFunInternalChoice(AFunInternalChoice);
  return AFunInternalChoice;
}

inline
bool gsIsInternalChoice(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInternalChoice();
}

// Interrupt
inline
AFun initAFunInterrupt(AFun& f)
{
  f = ATmakeAFun("Interrupt", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInterrupt()
{
  static AFun AFunInterrupt = initAFunInterrupt(AFunInterrupt);
  return AFunInterrupt;
}

inline
bool gsIsInterrupt(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInterrupt();
}

// Lambda
inline
AFun initAFunLambda(AFun& f)
{
  f = ATmakeAFun("Lambda", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLambda()
{
  static AFun AFunLambda = initAFunLambda(AFunLambda);
  return AFunLambda;
}

inline
bool gsIsLambda(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLambda();
}

// LambdaAppl
inline
AFun initAFunLambdaAppl(AFun& f)
{
  f = ATmakeAFun("LambdaAppl", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLambdaAppl()
{
  static AFun AFunLambdaAppl = initAFunLambdaAppl(AFunLambdaAppl);
  return AFunLambdaAppl;
}

inline
bool gsIsLambdaAppl(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLambdaAppl();
}

// LambdaExpr
inline
AFun initAFunLambdaExpr(AFun& f)
{
  f = ATmakeAFun("LambdaExpr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLambdaExpr()
{
  static AFun AFunLambdaExpr = initAFunLambdaExpr(AFunLambdaExpr);
  return AFunLambdaExpr;
}

inline
bool gsIsLambdaExpr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLambdaExpr();
}

// Length
inline
AFun initAFunLength(AFun& f)
{
  f = ATmakeAFun("Length", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLength()
{
  static AFun AFunLength = initAFunLength(AFunLength);
  return AFunLength;
}

inline
bool gsIsLength(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLength();
}

// Less
inline
AFun initAFunLess(AFun& f)
{
  f = ATmakeAFun("Less", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLess()
{
  static AFun AFunLess = initAFunLess(AFunLess);
  return AFunLess;
}

inline
bool gsIsLess(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLess();
}

// LessOrEqual
inline
AFun initAFunLessOrEqual(AFun& f)
{
  f = ATmakeAFun("LessOrEqual", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLessOrEqual()
{
  static AFun AFunLessOrEqual = initAFunLessOrEqual(AFunLessOrEqual);
  return AFunLessOrEqual;
}

inline
bool gsIsLessOrEqual(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLessOrEqual();
}

// Link
inline
AFun initAFunLink(AFun& f)
{
  f = ATmakeAFun("Link", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLink()
{
  static AFun AFunLink = initAFunLink(AFunLink);
  return AFunLink;
}

inline
bool gsIsLink(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLink();
}

// LinkedParallel
inline
AFun initAFunLinkedParallel(AFun& f)
{
  f = ATmakeAFun("LinkedParallel", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinkedParallel()
{
  static AFun AFunLinkedParallel = initAFunLinkedParallel(AFunLinkedParallel);
  return AFunLinkedParallel;
}

inline
bool gsIsLinkedParallel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinkedParallel();
}

// Links
inline
AFun initAFunLinks(AFun& f)
{
  f = ATmakeAFun("Links", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinks()
{
  static AFun AFunLinks = initAFunLinks(AFunLinks);
  return AFunLinks;
}

inline
bool gsIsLinks(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinks();
}

// LinksGens
inline
AFun initAFunLinksGens(AFun& f)
{
  f = ATmakeAFun("LinksGens", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinksGens()
{
  static AFun AFunLinksGens = initAFunLinksGens(AFunLinksGens);
  return AFunLinksGens;
}

inline
bool gsIsLinksGens(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinksGens();
}

// LocalDef
inline
AFun initAFunLocalDef(AFun& f)
{
  f = ATmakeAFun("LocalDef", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLocalDef()
{
  static AFun AFunLocalDef = initAFunLocalDef(AFunLocalDef);
  return AFunLocalDef;
}

inline
bool gsIsLocalDef(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLocalDef();
}

// Map
inline
AFun initAFunMap(AFun& f)
{
  f = ATmakeAFun("Map", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMap()
{
  static AFun AFunMap = initAFunMap(AFunMap);
  return AFunMap;
}

inline
bool gsIsMap(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMap();
}

// Maps
inline
AFun initAFunMaps(AFun& f)
{
  f = ATmakeAFun("Maps", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMaps()
{
  static AFun AFunMaps = initAFunMaps(AFunMaps);
  return AFunMaps;
}

inline
bool gsIsMaps(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMaps();
}

// MapsGens
inline
AFun initAFunMapsGens(AFun& f)
{
  f = ATmakeAFun("MapsGens", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMapsGens()
{
  static AFun AFunMapsGens = initAFunMapsGens(AFunMapsGens);
  return AFunMapsGens;
}

inline
bool gsIsMapsGens(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMapsGens();
}

// Member
inline
AFun initAFunMember(AFun& f)
{
  f = ATmakeAFun("Member", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMember()
{
  static AFun AFunMember = initAFunMember(AFunMember);
  return AFunMember;
}

inline
bool gsIsMember(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMember();
}

// Min
inline
AFun initAFunMin(AFun& f)
{
  f = ATmakeAFun("Min", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMin()
{
  static AFun AFunMin = initAFunMin(AFunMin);
  return AFunMin;
}

inline
bool gsIsMin(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMin();
}

// Minus
inline
AFun initAFunMinus(AFun& f)
{
  f = ATmakeAFun("Minus", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMinus()
{
  static AFun AFunMinus = initAFunMinus(AFunMinus);
  return AFunMinus;
}

inline
bool gsIsMinus(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMinus();
}

// Mod
inline
AFun initAFunMod(AFun& f)
{
  f = ATmakeAFun("Mod", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMod()
{
  static AFun AFunMod = initAFunMod(AFunMod);
  return AFunMod;
}

inline
bool gsIsMod(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMod();
}

// Model
inline
AFun initAFunModel(AFun& f)
{
  f = ATmakeAFun("Model", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunModel()
{
  static AFun AFunModel = initAFunModel(AFunModel);
  return AFunModel;
}

inline
bool gsIsModel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunModel();
}

// Name
inline
AFun initAFunName(AFun& f)
{
  f = ATmakeAFun("Name", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunName()
{
  static AFun AFunName = initAFunName(AFunName);
  return AFunName;
}

inline
bool gsIsName(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunName();
}

// NameType
inline
AFun initAFunNameType(AFun& f)
{
  f = ATmakeAFun("NameType", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNameType()
{
  static AFun AFunNameType = initAFunNameType(AFunNameType);
  return AFunNameType;
}

inline
bool gsIsNameType(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNameType();
}

// Nil
inline
AFun initAFunNil(AFun& f)
{
  f = ATmakeAFun("Nil", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNil()
{
  static AFun AFunNil = initAFunNil(AFunNil);
  return AFunNil;
}

inline
bool gsIsNil(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNil();
}

// Not
inline
AFun initAFunNot(AFun& f)
{
  f = ATmakeAFun("Not", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNot()
{
  static AFun AFunNot = initAFunNot(AFunNot);
  return AFunNot;
}

inline
bool gsIsNot(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNot();
}

// NotEqual
inline
AFun initAFunNotEqual(AFun& f)
{
  f = ATmakeAFun("NotEqual", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNotEqual()
{
  static AFun AFunNotEqual = initAFunNotEqual(AFunNotEqual);
  return AFunNotEqual;
}

inline
bool gsIsNotEqual(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNotEqual();
}

// Null
inline
AFun initAFunNull(AFun& f)
{
  f = ATmakeAFun("Null", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNull()
{
  static AFun AFunNull = initAFunNull(AFunNull);
  return AFunNull;
}

inline
bool gsIsNull(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNull();
}

// Numb
inline
AFun initAFunNumb(AFun& f)
{
  f = ATmakeAFun("Numb", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNumb()
{
  static AFun AFunNumb = initAFunNumb(AFunNumb);
  return AFunNumb;
}

inline
bool gsIsNumb(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNumb();
}

// Number
inline
AFun initAFunNumber(AFun& f)
{
  f = ATmakeAFun("Number", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNumber()
{
  static AFun AFunNumber = initAFunNumber(AFunNumber);
  return AFunNumber;
}

inline
bool gsIsNumber(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNumber();
}

// OpenRange
inline
AFun initAFunOpenRange(AFun& f)
{
  f = ATmakeAFun("OpenRange", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunOpenRange()
{
  static AFun AFunOpenRange = initAFunOpenRange(AFunOpenRange);
  return AFunOpenRange;
}

inline
bool gsIsOpenRange(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunOpenRange();
}

// Or
inline
AFun initAFunOr(AFun& f)
{
  f = ATmakeAFun("Or", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunOr()
{
  static AFun AFunOr = initAFunOr(AFunOr);
  return AFunOr;
}

inline
bool gsIsOr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunOr();
}

// Output
inline
AFun initAFunOutput(AFun& f)
{
  f = ATmakeAFun("Output", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunOutput()
{
  static AFun AFunOutput = initAFunOutput(AFunOutput);
  return AFunOutput;
}

inline
bool gsIsOutput(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunOutput();
}

// Plus
inline
AFun initAFunPlus(AFun& f)
{
  f = ATmakeAFun("Plus", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPlus()
{
  static AFun AFunPlus = initAFunPlus(AFunPlus);
  return AFunPlus;
}

inline
bool gsIsPlus(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPlus();
}

// Prefix
inline
AFun initAFunPrefix(AFun& f)
{
  f = ATmakeAFun("Prefix", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPrefix()
{
  static AFun AFunPrefix = initAFunPrefix(AFunPrefix);
  return AFunPrefix;
}

inline
bool gsIsPrefix(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPrefix();
}

// Print
inline
AFun initAFunPrint(AFun& f)
{
  f = ATmakeAFun("Print", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPrint()
{
  static AFun AFunPrint = initAFunPrint(AFunPrint);
  return AFunPrint;
}

inline
bool gsIsPrint(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPrint();
}

// Proc
inline
AFun initAFunProc(AFun& f)
{
  f = ATmakeAFun("Proc", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProc()
{
  static AFun AFunProc = initAFunProc(AFunProc);
  return AFunProc;
}

inline
bool gsIsProc(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProc();
}

// RCheck
inline
AFun initAFunRCheck(AFun& f)
{
  f = ATmakeAFun("RCheck", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRCheck()
{
  static AFun AFunRCheck = initAFunRCheck(AFunRCheck);
  return AFunRCheck;
}

inline
bool gsIsRCheck(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRCheck();
}

// Rename
inline
AFun initAFunRename(AFun& f)
{
  f = ATmakeAFun("Rename", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRename()
{
  static AFun AFunRename = initAFunRename(AFunRename);
  return AFunRename;
}

inline
bool gsIsRename(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRename();
}

// RepAlphaParallel
inline
AFun initAFunRepAlphaParallel(AFun& f)
{
  f = ATmakeAFun("RepAlphaParallel", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepAlphaParallel()
{
  static AFun AFunRepAlphaParallel = initAFunRepAlphaParallel(AFunRepAlphaParallel);
  return AFunRepAlphaParallel;
}

inline
bool gsIsRepAlphaParallel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepAlphaParallel();
}

// RepExternalChoice
inline
AFun initAFunRepExternalChoice(AFun& f)
{
  f = ATmakeAFun("RepExternalChoice", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepExternalChoice()
{
  static AFun AFunRepExternalChoice = initAFunRepExternalChoice(AFunRepExternalChoice);
  return AFunRepExternalChoice;
}

inline
bool gsIsRepExternalChoice(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepExternalChoice();
}

// RepInterleave
inline
AFun initAFunRepInterleave(AFun& f)
{
  f = ATmakeAFun("RepInterleave", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepInterleave()
{
  static AFun AFunRepInterleave = initAFunRepInterleave(AFunRepInterleave);
  return AFunRepInterleave;
}

inline
bool gsIsRepInterleave(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepInterleave();
}

// RepInternalChoice
inline
AFun initAFunRepInternalChoice(AFun& f)
{
  f = ATmakeAFun("RepInternalChoice", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepInternalChoice()
{
  static AFun AFunRepInternalChoice = initAFunRepInternalChoice(AFunRepInternalChoice);
  return AFunRepInternalChoice;
}

inline
bool gsIsRepInternalChoice(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepInternalChoice();
}

// RepLinkedParallel
inline
AFun initAFunRepLinkedParallel(AFun& f)
{
  f = ATmakeAFun("RepLinkedParallel", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepLinkedParallel()
{
  static AFun AFunRepLinkedParallel = initAFunRepLinkedParallel(AFunRepLinkedParallel);
  return AFunRepLinkedParallel;
}

inline
bool gsIsRepLinkedParallel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepLinkedParallel();
}

// RepSequentialComposition
inline
AFun initAFunRepSequentialComposition(AFun& f)
{
  f = ATmakeAFun("RepSequentialComposition", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepSequentialComposition()
{
  static AFun AFunRepSequentialComposition = initAFunRepSequentialComposition(AFunRepSequentialComposition);
  return AFunRepSequentialComposition;
}

inline
bool gsIsRepSequentialComposition(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepSequentialComposition();
}

// RepSharing
inline
AFun initAFunRepSharing(AFun& f)
{
  f = ATmakeAFun("RepSharing", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRepSharing()
{
  static AFun AFunRepSharing = initAFunRepSharing(AFunRepSharing);
  return AFunRepSharing;
}

inline
bool gsIsRepSharing(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRepSharing();
}

// SKIP
inline
AFun initAFunSKIP(AFun& f)
{
  f = ATmakeAFun("SKIP", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSKIP()
{
  static AFun AFunSKIP = initAFunSKIP(AFunSKIP);
  return AFunSKIP;
}

inline
bool gsIsSKIP(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSKIP();
}

// STOP
inline
AFun initAFunSTOP(AFun& f)
{
  f = ATmakeAFun("STOP", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSTOP()
{
  static AFun AFunSTOP = initAFunSTOP(AFunSTOP);
  return AFunSTOP;
}

inline
bool gsIsSTOP(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSTOP();
}

// Seq
inline
AFun initAFunSeq(AFun& f)
{
  f = ATmakeAFun("Seq", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSeq()
{
  static AFun AFunSeq = initAFunSeq(AFunSeq);
  return AFunSeq;
}

inline
bool gsIsSeq(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSeq();
}

// SequentialComposition
inline
AFun initAFunSequentialComposition(AFun& f)
{
  f = ATmakeAFun("SequentialComposition", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSequentialComposition()
{
  static AFun AFunSequentialComposition = initAFunSequentialComposition(AFunSequentialComposition);
  return AFunSequentialComposition;
}

inline
bool gsIsSequentialComposition(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSequentialComposition();
}

// Set
inline
AFun initAFunSet(AFun& f)
{
  f = ATmakeAFun("Set", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSet()
{
  static AFun AFunSet = initAFunSet(AFunSet);
  return AFunSet;
}

inline
bool gsIsSet(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSet();
}

// Sharing
inline
AFun initAFunSharing(AFun& f)
{
  f = ATmakeAFun("Sharing", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSharing()
{
  static AFun AFunSharing = initAFunSharing(AFunSharing);
  return AFunSharing;
}

inline
bool gsIsSharing(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSharing();
}

// SimpleBranch
inline
AFun initAFunSimpleBranch(AFun& f)
{
  f = ATmakeAFun("SimpleBranch", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSimpleBranch()
{
  static AFun AFunSimpleBranch = initAFunSimpleBranch(AFunSimpleBranch);
  return AFunSimpleBranch;
}

inline
bool gsIsSimpleBranch(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSimpleBranch();
}

// SimpleChannel
inline
AFun initAFunSimpleChannel(AFun& f)
{
  f = ATmakeAFun("SimpleChannel", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSimpleChannel()
{
  static AFun AFunSimpleChannel = initAFunSimpleChannel(AFunSimpleChannel);
  return AFunSimpleChannel;
}

inline
bool gsIsSimpleChannel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSimpleChannel();
}

// SimpleInput
inline
AFun initAFunSimpleInput(AFun& f)
{
  f = ATmakeAFun("SimpleInput", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSimpleInput()
{
  static AFun AFunSimpleInput = initAFunSimpleInput(AFunSimpleInput);
  return AFunSimpleInput;
}

inline
bool gsIsSimpleInput(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSimpleInput();
}

// SimpleTypeName
inline
AFun initAFunSimpleTypeName(AFun& f)
{
  f = ATmakeAFun("SimpleTypeName", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSimpleTypeName()
{
  static AFun AFunSimpleTypeName = initAFunSimpleTypeName(AFunSimpleTypeName);
  return AFunSimpleTypeName;
}

inline
bool gsIsSimpleTypeName(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSimpleTypeName();
}

// SubType
inline
AFun initAFunSubType(AFun& f)
{
  f = ATmakeAFun("SubType", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSubType()
{
  static AFun AFunSubType = initAFunSubType(AFunSubType);
  return AFunSubType;
}

inline
bool gsIsSubType(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSubType();
}

// T
inline
AFun initAFunT(AFun& f)
{
  f = ATmakeAFun("T", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunT()
{
  static AFun AFunT = initAFunT(AFunT);
  return AFunT;
}

inline
bool gsIsT(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunT();
}

// TCheck
inline
AFun initAFunTCheck(AFun& f)
{
  f = ATmakeAFun("TCheck", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTCheck()
{
  static AFun AFunTCheck = initAFunTCheck(AFunTCheck);
  return AFunTCheck;
}

inline
bool gsIsTCheck(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTCheck();
}

// Targ
inline
AFun initAFunTarg(AFun& f)
{
  f = ATmakeAFun("Targ", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTarg()
{
  static AFun AFunTarg = initAFunTarg(AFunTarg);
  return AFunTarg;
}

inline
bool gsIsTarg(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTarg();
}

// Targ0
inline
AFun initAFunTarg0(AFun& f)
{
  f = ATmakeAFun("Targ0", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTarg0()
{
  static AFun AFunTarg0 = initAFunTarg0(AFunTarg0);
  return AFunTarg0;
}

inline
bool gsIsTarg0(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTarg0();
}

// TargGens
inline
AFun initAFunTargGens(AFun& f)
{
  f = ATmakeAFun("TargGens", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTargGens()
{
  static AFun AFunTargGens = initAFunTargGens(AFunTargGens);
  return AFunTargGens;
}

inline
bool gsIsTargGens(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTargGens();
}

// TargGens0
inline
AFun initAFunTargGens0(AFun& f)
{
  f = ATmakeAFun("TargGens0", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTargGens0()
{
  static AFun AFunTargGens0 = initAFunTargGens0(AFunTargGens0);
  return AFunTargGens0;
}

inline
bool gsIsTargGens0(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTargGens0();
}

// Test
inline
AFun initAFunTest(AFun& f)
{
  f = ATmakeAFun("Test", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTest()
{
  static AFun AFunTest = initAFunTest(AFunTest);
  return AFunTest;
}

inline
bool gsIsTest(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTest();
}

// Times
inline
AFun initAFunTimes(AFun& f)
{
  f = ATmakeAFun("Times", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTimes()
{
  static AFun AFunTimes = initAFunTimes(AFunTimes);
  return AFunTimes;
}

inline
bool gsIsTimes(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTimes();
}

// Transparent
inline
AFun initAFunTransparent(AFun& f)
{
  f = ATmakeAFun("Transparent", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTransparent()
{
  static AFun AFunTransparent = initAFunTransparent(AFunTransparent);
  return AFunTransparent;
}

inline
bool gsIsTransparent(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTransparent();
}

// Tuple
inline
AFun initAFunTuple(AFun& f)
{
  f = ATmakeAFun("Tuple", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTuple()
{
  static AFun AFunTuple = initAFunTuple(AFunTuple);
  return AFunTuple;
}

inline
bool gsIsTuple(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTuple();
}

// TypeExpr
inline
AFun initAFunTypeExpr(AFun& f)
{
  f = ATmakeAFun("TypeExpr", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTypeExpr()
{
  static AFun AFunTypeExpr = initAFunTypeExpr(AFunTypeExpr);
  return AFunTypeExpr;
}

inline
bool gsIsTypeExpr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTypeExpr();
}

// TypeName
inline
AFun initAFunTypeName(AFun& f)
{
  f = ATmakeAFun("TypeName", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTypeName()
{
  static AFun AFunTypeName = initAFunTypeName(AFunTypeName);
  return AFunTypeName;
}

inline
bool gsIsTypeName(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTypeName();
}

// TypeProduct
inline
AFun initAFunTypeProduct(AFun& f)
{
  f = ATmakeAFun("TypeProduct", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTypeProduct()
{
  static AFun AFunTypeProduct = initAFunTypeProduct(AFunTypeProduct);
  return AFunTypeProduct;
}

inline
bool gsIsTypeProduct(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTypeProduct();
}

// TypeTyple
inline
AFun initAFunTypeTyple(AFun& f)
{
  f = ATmakeAFun("TypeTyple", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTypeTyple()
{
  static AFun AFunTypeTyple = initAFunTypeTyple(AFunTypeTyple);
  return AFunTypeTyple;
}

inline
bool gsIsTypeTyple(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTypeTyple();
}

// Union
inline
AFun initAFunUnion(AFun& f)
{
  f = ATmakeAFun("Union", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunUnion()
{
  static AFun AFunUnion = initAFunUnion(AFunUnion);
  return AFunUnion;
}

inline
bool gsIsUnion(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunUnion();
}

// UntimedTimeOut
inline
AFun initAFunUntimedTimeOut(AFun& f)
{
  f = ATmakeAFun("UntimedTimeOut", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunUntimedTimeOut()
{
  static AFun AFunUntimedTimeOut = initAFunUntimedTimeOut(AFunUntimedTimeOut);
  return AFunUntimedTimeOut;
}

inline
bool gsIsUntimedTimeOut(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunUntimedTimeOut();
}

// deadlock_free
inline
AFun initAFundeadlock_free(AFun& f)
{
  f = ATmakeAFun("deadlock_free", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFundeadlock_free()
{
  static AFun AFundeadlock_free = initAFundeadlock_free(AFundeadlock_free);
  return AFundeadlock_free;
}

inline
bool gsIsdeadlock_free(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFundeadlock_free();
}

// deterministic
inline
AFun initAFundeterministic(AFun& f)
{
  f = ATmakeAFun("deterministic", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFundeterministic()
{
  static AFun AFundeterministic = initAFundeterministic(AFundeterministic);
  return AFundeterministic;
}

inline
bool gsIsdeterministic(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFundeterministic();
}

// diamond
inline
AFun initAFundiamond(AFun& f)
{
  f = ATmakeAFun("diamond", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFundiamond()
{
  static AFun AFundiamond = initAFundiamond(AFundiamond);
  return AFundiamond;
}

inline
bool gsIsdiamond(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFundiamond();
}

// diff
inline
AFun initAFundiff(AFun& f)
{
  f = ATmakeAFun("diff", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFundiff()
{
  static AFun AFundiff = initAFundiff(AFundiff);
  return AFundiff;
}

inline
bool gsIsdiff(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFundiff();
}

// divergence_free
inline
AFun initAFundivergence_free(AFun& f)
{
  f = ATmakeAFun("divergence_free", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFundivergence_free()
{
  static AFun AFundivergence_free = initAFundivergence_free(AFundivergence_free);
  return AFundivergence_free;
}

inline
bool gsIsdivergence_free(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFundivergence_free();
}

// explicate
inline
AFun initAFunexplicate(AFun& f)
{
  f = ATmakeAFun("explicate", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunexplicate()
{
  static AFun AFunexplicate = initAFunexplicate(AFunexplicate);
  return AFunexplicate;
}

inline
bool gsIsexplicate(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunexplicate();
}

// extensions
inline
AFun initAFunextensions(AFun& f)
{
  f = ATmakeAFun("extensions", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunextensions()
{
  static AFun AFunextensions = initAFunextensions(AFunextensions);
  return AFunextensions;
}

inline
bool gsIsextensions(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunextensions();
}

// false
inline
AFun initAFunfalse(AFun& f)
{
  f = ATmakeAFun("false", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunfalse()
{
  static AFun AFunfalse = initAFunfalse(AFunfalse);
  return AFunfalse;
}

inline
bool gsIsfalse(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunfalse();
}

// inter
inline
AFun initAFuninter(AFun& f)
{
  f = ATmakeAFun("inter", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFuninter()
{
  static AFun AFuninter = initAFuninter(AFuninter);
  return AFuninter;
}

inline
bool gsIsinter(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFuninter();
}

// livelock_free
inline
AFun initAFunlivelock_free(AFun& f)
{
  f = ATmakeAFun("livelock_free", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunlivelock_free()
{
  static AFun AFunlivelock_free = initAFunlivelock_free(AFunlivelock_free);
  return AFunlivelock_free;
}

inline
bool gsIslivelock_free(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunlivelock_free();
}

// model_compress
inline
AFun initAFunmodel_compress(AFun& f)
{
  f = ATmakeAFun("model_compress", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunmodel_compress()
{
  static AFun AFunmodel_compress = initAFunmodel_compress(AFunmodel_compress);
  return AFunmodel_compress;
}

inline
bool gsIsmodel_compress(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunmodel_compress();
}

// normal
inline
AFun initAFunnormal(AFun& f)
{
  f = ATmakeAFun("normal", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunnormal()
{
  static AFun AFunnormal = initAFunnormal(AFunnormal);
  return AFunnormal;
}

inline
bool gsIsnormal(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunnormal();
}

// normalise
inline
AFun initAFunnormalise(AFun& f)
{
  f = ATmakeAFun("normalise", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunnormalise()
{
  static AFun AFunnormalise = initAFunnormalise(AFunnormalise);
  return AFunnormalise;
}

inline
bool gsIsnormalise(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunnormalise();
}

// normalize
inline
AFun initAFunnormalize(AFun& f)
{
  f = ATmakeAFun("normalize", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunnormalize()
{
  static AFun AFunnormalize = initAFunnormalize(AFunnormalize);
  return AFunnormalize;
}

inline
bool gsIsnormalize(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunnormalize();
}

// productions
inline
AFun initAFunproductions(AFun& f)
{
  f = ATmakeAFun("productions", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunproductions()
{
  static AFun AFunproductions = initAFunproductions(AFunproductions);
  return AFunproductions;
}

inline
bool gsIsproductions(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunproductions();
}

// sbsim
inline
AFun initAFunsbsim(AFun& f)
{
  f = ATmakeAFun("sbsim", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunsbsim()
{
  static AFun AFunsbsim = initAFunsbsim(AFunsbsim);
  return AFunsbsim;
}

inline
bool gsIssbsim(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunsbsim();
}

// set
inline
AFun initAFunset(AFun& f)
{
  f = ATmakeAFun("set", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunset()
{
  static AFun AFunset = initAFunset(AFunset);
  return AFunset;
}

inline
bool gsIsset(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunset();
}

// tau_loop_factor
inline
AFun initAFuntau_loop_factor(AFun& f)
{
  f = ATmakeAFun("tau_loop_factor", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFuntau_loop_factor()
{
  static AFun AFuntau_loop_factor = initAFuntau_loop_factor(AFuntau_loop_factor);
  return AFuntau_loop_factor;
}

inline
bool gsIstau_loop_factor(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFuntau_loop_factor();
}

// true
inline
AFun initAFuntrue(AFun& f)
{
  f = ATmakeAFun("true", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFuntrue()
{
  static AFun AFuntrue = initAFuntrue(AFuntrue);
  return AFuntrue;
}

inline
bool gsIstrue(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFuntrue();
}

// union
inline
AFun initAFununion(AFun& f)
{
  f = ATmakeAFun("union", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFununion()
{
  static AFun AFununion = initAFununion(AFununion);
  return AFununion;
}

inline
bool gsIsunion(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFununion();
}

inline
ATermAppl gsMakeAlphaParallel(ATermAppl Proc_0, ATermAppl Proc_1, ATermAppl Set_2, ATermAppl Set_3)
{
  return ATmakeAppl4(gsAFunAlphaParallel(), (ATerm) Proc_0, (ATerm) Proc_1, (ATerm) Set_2, (ATerm) Set_3);
}

inline
ATermAppl gsMakeAnd(ATermAppl Bool_0, ATermAppl Bool_1)
{
  return ATmakeAppl2(gsAFunAnd(), (ATerm) Bool_0, (ATerm) Bool_1);
}

inline
ATermAppl gsMakeAssert(ATermAppl Check_0)
{
  return ATmakeAppl1(gsAFunAssert(), (ATerm) Check_0);
}

inline
ATermAppl gsMakeAssign(ATermAppl Any_0, ATermAppl Any_1)
{
  return ATmakeAppl2(gsAFunAssign(), (ATerm) Any_0, (ATerm) Any_1);
}

inline
ATermAppl gsMakeBCheck(ATermAppl Bool_0)
{
  return ATmakeAppl1(gsAFunBCheck(), (ATerm) Bool_0);
}

inline
ATermAppl gsMakeBool(ATermAppl Bool_0)
{
  return ATmakeAppl1(gsAFunBool(), (ATerm) Bool_0);
}

inline
ATermAppl gsMakeBoolGuard(ATermAppl Bool_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunBoolGuard(), (ATerm) Bool_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeBranch(ATermAppl Name_0, ATermAppl Type_1)
{
  return ATmakeAppl2(gsAFunBranch(), (ATerm) Name_0, (ATerm) Type_1);
}

inline
ATermAppl gsMakeCHAOS(ATermAppl Set_0)
{
  return ATmakeAppl1(gsAFunCHAOS(), (ATerm) Set_0);
}

inline
ATermAppl gsMakeCard(ATermAppl Set_0)
{
  return ATmakeAppl1(gsAFunCard(), (ATerm) Set_0);
}

inline
ATermAppl gsMakeCat(ATermAppl Seq_0, ATermAppl Seq_1)
{
  return ATmakeAppl2(gsAFunCat(), (ATerm) Seq_0, (ATerm) Seq_1);
}

inline
ATermAppl gsMakeChannel(ATermList Name_0, ATermAppl Type_1)
{
  return ATmakeAppl2(gsAFunChannel(), (ATerm) Name_0, (ATerm) Type_1);
}

inline
ATermAppl gsMakeClosedRange(ATermAppl Numb_0, ATermAppl Numb_1)
{
  return ATmakeAppl2(gsAFunClosedRange(), (ATerm) Numb_0, (ATerm) Numb_1);
}

inline
ATermAppl gsMakeCommon(ATermAppl Common_0)
{
  return ATmakeAppl1(gsAFunCommon(), (ATerm) Common_0);
}

inline
ATermAppl gsMakeConcat(ATermAppl Seq_0)
{
  return ATmakeAppl1(gsAFunConcat(), (ATerm) Seq_0);
}

inline
ATermAppl gsMakeConditional(ATermAppl Bool_0, ATermAppl Any_1, ATermAppl Any_2)
{
  return ATmakeAppl3(gsAFunConditional(), (ATerm) Bool_0, (ATerm) Any_1, (ATerm) Any_2);
}

inline
ATermAppl gsMakeDataType(ATermAppl Name_0, ATermList VarType_1)
{
  return ATmakeAppl2(gsAFunDataType(), (ATerm) Name_0, (ATerm) VarType_1);
}

inline
ATermAppl gsMakeDiv(ATermAppl Numb_0, ATermAppl Numb_1)
{
  return ATmakeAppl2(gsAFunDiv(), (ATerm) Numb_0, (ATerm) Numb_1);
}

inline
ATermAppl gsMakeDot(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunDot(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeDotted(ATermAppl Dotted_0)
{
  return ATmakeAppl1(gsAFunDotted(), (ATerm) Dotted_0);
}

inline
ATermAppl gsMakeElem(ATermAppl Expr_0, ATermAppl Seq_1)
{
  return ATmakeAppl2(gsAFunElem(), (ATerm) Expr_0, (ATerm) Seq_1);
}

inline
ATermAppl gsMakeEmpty(ATermAppl Set_0)
{
  return ATmakeAppl1(gsAFunEmpty(), (ATerm) Set_0);
}

inline
ATermAppl gsMakeEqual(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunEqual(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeExpr(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunExpr(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakeExprs(ATermList Expr_0)
{
  return ATmakeAppl1(gsAFunExprs(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakeExternal(ATermList Name_0)
{
  return ATmakeAppl1(gsAFunExternal(), (ATerm) Name_0);
}

inline
ATermAppl gsMakeExternalChoice(ATermAppl Proc_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunExternalChoice(), (ATerm) Proc_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeF()
{
  return ATmakeAppl0(gsAFunF());
}

inline
ATermAppl gsMakeFD()
{
  return ATmakeAppl0(gsAFunFD());
}

inline
ATermAppl gsMakeFDRSpec(ATermList Defn_0)
{
  return ATmakeAppl1(gsAFunFDRSpec(), (ATerm) Defn_0);
}

inline
ATermAppl gsMakeFileName(ATermList Name_0)
{
  return ATmakeAppl1(gsAFunFileName(), (ATerm) Name_0);
}

inline
ATermAppl gsMakeGen(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunGen(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeGreater(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunGreater(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeGreaterOrEqual(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunGreaterOrEqual(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeHiding(ATermAppl Proc_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunHiding(), (ATerm) Proc_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeInclude(ATermAppl FileName_0)
{
  return ATmakeAppl1(gsAFunInclude(), (ATerm) FileName_0);
}

inline
ATermAppl gsMakeInput(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunInput(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeInter(ATermAppl Set_0)
{
  return ATmakeAppl1(gsAFunInter(), (ATerm) Set_0);
}

inline
ATermAppl gsMakeInterleave(ATermAppl Proc_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunInterleave(), (ATerm) Proc_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeInternalChoice(ATermAppl Proc_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunInternalChoice(), (ATerm) Proc_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeInterrupt(ATermAppl Proc_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunInterrupt(), (ATerm) Proc_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeLambda(ATermAppl Lambda_0)
{
  return ATmakeAppl1(gsAFunLambda(), (ATerm) Lambda_0);
}

inline
ATermAppl gsMakeLambdaAppl(ATermAppl Lambda_0, ATermList Expr_1)
{
  return ATmakeAppl2(gsAFunLambdaAppl(), (ATerm) Lambda_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeLambdaExpr(ATermList Expr_0, ATermAppl Any_1)
{
  return ATmakeAppl2(gsAFunLambdaExpr(), (ATerm) Expr_0, (ATerm) Any_1);
}

inline
ATermAppl gsMakeLength(ATermAppl Seq_0)
{
  return ATmakeAppl1(gsAFunLength(), (ATerm) Seq_0);
}

inline
ATermAppl gsMakeLess(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunLess(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeLessOrEqual(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunLessOrEqual(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeLink(ATermAppl Dotted_0, ATermAppl Dotted_1)
{
  return ATmakeAppl2(gsAFunLink(), (ATerm) Dotted_0, (ATerm) Dotted_1);
}

inline
ATermAppl gsMakeLinkedParallel(ATermAppl Proc_0, ATermAppl Proc_1, ATermAppl LinkPar_2)
{
  return ATmakeAppl3(gsAFunLinkedParallel(), (ATerm) Proc_0, (ATerm) Proc_1, (ATerm) LinkPar_2);
}

inline
ATermAppl gsMakeLinks(ATermList Link_0)
{
  return ATmakeAppl1(gsAFunLinks(), (ATerm) Link_0);
}

inline
ATermAppl gsMakeLinksGens(ATermList Link_0, ATermList Gen_1)
{
  return ATmakeAppl2(gsAFunLinksGens(), (ATerm) Link_0, (ATerm) Gen_1);
}

inline
ATermAppl gsMakeLocalDef(ATermList Defn_0, ATermAppl Any_1)
{
  return ATmakeAppl2(gsAFunLocalDef(), (ATerm) Defn_0, (ATerm) Any_1);
}

inline
ATermAppl gsMakeMap(ATermAppl Dotted_0, ATermAppl Dotted_1)
{
  return ATmakeAppl2(gsAFunMap(), (ATerm) Dotted_0, (ATerm) Dotted_1);
}

inline
ATermAppl gsMakeMaps(ATermList Map_0)
{
  return ATmakeAppl1(gsAFunMaps(), (ATerm) Map_0);
}

inline
ATermAppl gsMakeMapsGens(ATermList Map_0, ATermList Gen_1)
{
  return ATmakeAppl2(gsAFunMapsGens(), (ATerm) Map_0, (ATerm) Gen_1);
}

inline
ATermAppl gsMakeMember(ATermAppl Expr_0, ATermAppl Set_1)
{
  return ATmakeAppl2(gsAFunMember(), (ATerm) Expr_0, (ATerm) Set_1);
}

inline
ATermAppl gsMakeMin(ATermAppl Numb_0)
{
  return ATmakeAppl1(gsAFunMin(), (ATerm) Numb_0);
}

inline
ATermAppl gsMakeMinus(ATermAppl Numb_0, ATermAppl Numb_1)
{
  return ATmakeAppl2(gsAFunMinus(), (ATerm) Numb_0, (ATerm) Numb_1);
}

inline
ATermAppl gsMakeMod(ATermAppl Numb_0, ATermAppl Numb_1)
{
  return ATmakeAppl2(gsAFunMod(), (ATerm) Numb_0, (ATerm) Numb_1);
}

inline
ATermAppl gsMakeModel(ATermAppl Model_0)
{
  return ATmakeAppl1(gsAFunModel(), (ATerm) Model_0);
}

inline
ATermAppl gsMakeName(ATermAppl Name_0)
{
  return ATmakeAppl1(gsAFunName(), (ATerm) Name_0);
}

inline
ATermAppl gsMakeNameType(ATermAppl Name_0, ATermAppl Type_1)
{
  return ATmakeAppl2(gsAFunNameType(), (ATerm) Name_0, (ATerm) Type_1);
}

inline
ATermAppl gsMakeNil()
{
  return ATmakeAppl0(gsAFunNil());
}

inline
ATermAppl gsMakeNot(ATermAppl Bool_0)
{
  return ATmakeAppl1(gsAFunNot(), (ATerm) Bool_0);
}

inline
ATermAppl gsMakeNotEqual(ATermAppl Expr_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunNotEqual(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeNull(ATermAppl Seq_0)
{
  return ATmakeAppl1(gsAFunNull(), (ATerm) Seq_0);
}

inline
ATermAppl gsMakeNumb(ATermAppl Numb_0)
{
  return ATmakeAppl1(gsAFunNumb(), (ATerm) Numb_0);
}

inline
ATermAppl gsMakeNumber(ATermAppl Number_0)
{
  return ATmakeAppl1(gsAFunNumber(), (ATerm) Number_0);
}

inline
ATermAppl gsMakeOpenRange(ATermAppl Numb_0)
{
  return ATmakeAppl1(gsAFunOpenRange(), (ATerm) Numb_0);
}

inline
ATermAppl gsMakeOr(ATermAppl Bool_0, ATermAppl Bool_1)
{
  return ATmakeAppl2(gsAFunOr(), (ATerm) Bool_0, (ATerm) Bool_1);
}

inline
ATermAppl gsMakeOutput(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunOutput(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakePlus(ATermAppl Numb_0, ATermAppl Numb_1)
{
  return ATmakeAppl2(gsAFunPlus(), (ATerm) Numb_0, (ATerm) Numb_1);
}

inline
ATermAppl gsMakePrefix(ATermAppl Dotted_0, ATermList Field_1, ATermAppl Proc_2)
{
  return ATmakeAppl3(gsAFunPrefix(), (ATerm) Dotted_0, (ATerm) Field_1, (ATerm) Proc_2);
}

inline
ATermAppl gsMakePrint(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunPrint(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakeProc(ATermAppl Proc_0)
{
  return ATmakeAppl1(gsAFunProc(), (ATerm) Proc_0);
}

inline
ATermAppl gsMakeRCheck(ATermAppl Proc_0, ATermAppl Proc_1, ATermAppl Refined_2)
{
  return ATmakeAppl3(gsAFunRCheck(), (ATerm) Proc_0, (ATerm) Proc_1, (ATerm) Refined_2);
}

inline
ATermAppl gsMakeRename(ATermAppl Proc_0, ATermAppl Renaming_1)
{
  return ATmakeAppl2(gsAFunRename(), (ATerm) Proc_0, (ATerm) Renaming_1);
}

inline
ATermAppl gsMakeRepAlphaParallel(ATermList Gen_0, ATermAppl Proc_1, ATermAppl Set_2)
{
  return ATmakeAppl3(gsAFunRepAlphaParallel(), (ATerm) Gen_0, (ATerm) Proc_1, (ATerm) Set_2);
}

inline
ATermAppl gsMakeRepExternalChoice(ATermList Gen_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunRepExternalChoice(), (ATerm) Gen_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeRepInterleave(ATermList Gen_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunRepInterleave(), (ATerm) Gen_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeRepInternalChoice(ATermList Gen_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunRepInternalChoice(), (ATerm) Gen_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeRepLinkedParallel(ATermList Gen_0, ATermAppl Proc_1, ATermAppl LinkPar_2)
{
  return ATmakeAppl3(gsAFunRepLinkedParallel(), (ATerm) Gen_0, (ATerm) Proc_1, (ATerm) LinkPar_2);
}

inline
ATermAppl gsMakeRepSequentialComposition(ATermList Gen_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunRepSequentialComposition(), (ATerm) Gen_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeRepSharing(ATermList Gen_0, ATermAppl Proc_1, ATermAppl Set_2)
{
  return ATmakeAppl3(gsAFunRepSharing(), (ATerm) Gen_0, (ATerm) Proc_1, (ATerm) Set_2);
}

inline
ATermAppl gsMakeSKIP()
{
  return ATmakeAppl0(gsAFunSKIP());
}

inline
ATermAppl gsMakeSTOP()
{
  return ATmakeAppl0(gsAFunSTOP());
}

inline
ATermAppl gsMakeSeq(ATermAppl Seq_0)
{
  return ATmakeAppl1(gsAFunSeq(), (ATerm) Seq_0);
}

inline
ATermAppl gsMakeSequentialComposition(ATermAppl Proc_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunSequentialComposition(), (ATerm) Proc_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakeSet(ATermAppl Set_0)
{
  return ATmakeAppl1(gsAFunSet(), (ATerm) Set_0);
}

inline
ATermAppl gsMakeSharing(ATermAppl Proc_0, ATermAppl Proc_1, ATermAppl Set_2)
{
  return ATmakeAppl3(gsAFunSharing(), (ATerm) Proc_0, (ATerm) Proc_1, (ATerm) Set_2);
}

inline
ATermAppl gsMakeSimpleBranch(ATermAppl Name_0)
{
  return ATmakeAppl1(gsAFunSimpleBranch(), (ATerm) Name_0);
}

inline
ATermAppl gsMakeSimpleChannel(ATermList Name_0)
{
  return ATmakeAppl1(gsAFunSimpleChannel(), (ATerm) Name_0);
}

inline
ATermAppl gsMakeSimpleInput(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunSimpleInput(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakeSimpleTypeName(ATermAppl Name_0)
{
  return ATmakeAppl1(gsAFunSimpleTypeName(), (ATerm) Name_0);
}

inline
ATermAppl gsMakeSubType(ATermAppl Name_0, ATermList VarType_1)
{
  return ATmakeAppl2(gsAFunSubType(), (ATerm) Name_0, (ATerm) VarType_1);
}

inline
ATermAppl gsMakeT()
{
  return ATmakeAppl0(gsAFunT());
}

inline
ATermAppl gsMakeTCheck(ATermAppl Proc_0, ATermAppl Test_1)
{
  return ATmakeAppl2(gsAFunTCheck(), (ATerm) Proc_0, (ATerm) Test_1);
}

inline
ATermAppl gsMakeTarg(ATermAppl Targ_0)
{
  return ATmakeAppl1(gsAFunTarg(), (ATerm) Targ_0);
}

inline
ATermAppl gsMakeTarg0(ATermAppl Targ_0)
{
  return ATmakeAppl1(gsAFunTarg0(), (ATerm) Targ_0);
}

inline
ATermAppl gsMakeTargGens(ATermAppl Targ_0, ATermList Gen_1)
{
  return ATmakeAppl2(gsAFunTargGens(), (ATerm) Targ_0, (ATerm) Gen_1);
}

inline
ATermAppl gsMakeTargGens0(ATermAppl Targ_0, ATermList Gen_1)
{
  return ATmakeAppl2(gsAFunTargGens0(), (ATerm) Targ_0, (ATerm) Gen_1);
}

inline
ATermAppl gsMakeTest(ATermAppl TestType_0, ATermAppl FailureModel_1)
{
  return ATmakeAppl2(gsAFunTest(), (ATerm) TestType_0, (ATerm) FailureModel_1);
}

inline
ATermAppl gsMakeTimes(ATermAppl Numb_0, ATermAppl Numb_1)
{
  return ATmakeAppl2(gsAFunTimes(), (ATerm) Numb_0, (ATerm) Numb_1);
}

inline
ATermAppl gsMakeTransparent(ATermList TrName_0)
{
  return ATmakeAppl1(gsAFunTransparent(), (ATerm) TrName_0);
}

inline
ATermAppl gsMakeTuple(ATermAppl Tuple_0)
{
  return ATmakeAppl1(gsAFunTuple(), (ATerm) Tuple_0);
}

inline
ATermAppl gsMakeTypeExpr(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunTypeExpr(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakeTypeName(ATermAppl Name_0, ATermList Type_1)
{
  return ATmakeAppl2(gsAFunTypeName(), (ATerm) Name_0, (ATerm) Type_1);
}

inline
ATermAppl gsMakeTypeProduct(ATermAppl Type_0, ATermAppl Type_1)
{
  return ATmakeAppl2(gsAFunTypeProduct(), (ATerm) Type_0, (ATerm) Type_1);
}

inline
ATermAppl gsMakeTypeTyple(ATermList Type_0)
{
  return ATmakeAppl1(gsAFunTypeTyple(), (ATerm) Type_0);
}

inline
ATermAppl gsMakeUnion(ATermAppl Set_0)
{
  return ATmakeAppl1(gsAFunUnion(), (ATerm) Set_0);
}

inline
ATermAppl gsMakeUntimedTimeOut(ATermAppl Proc_0, ATermAppl Proc_1)
{
  return ATmakeAppl2(gsAFunUntimedTimeOut(), (ATerm) Proc_0, (ATerm) Proc_1);
}

inline
ATermAppl gsMakedeadlock_free()
{
  return ATmakeAppl0(gsAFundeadlock_free());
}

inline
ATermAppl gsMakedeterministic()
{
  return ATmakeAppl0(gsAFundeterministic());
}

inline
ATermAppl gsMakediamond()
{
  return ATmakeAppl0(gsAFundiamond());
}

inline
ATermAppl gsMakediff(ATermAppl Set_0, ATermAppl Set_1)
{
  return ATmakeAppl2(gsAFundiff(), (ATerm) Set_0, (ATerm) Set_1);
}

inline
ATermAppl gsMakedivergence_free()
{
  return ATmakeAppl0(gsAFundivergence_free());
}

inline
ATermAppl gsMakeexplicate()
{
  return ATmakeAppl0(gsAFunexplicate());
}

inline
ATermAppl gsMakeextensions(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunextensions(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakefalse()
{
  return ATmakeAppl0(gsAFunfalse());
}

inline
ATermAppl gsMakeinter(ATermAppl Set_0, ATermAppl Set_1)
{
  return ATmakeAppl2(gsAFuninter(), (ATerm) Set_0, (ATerm) Set_1);
}

inline
ATermAppl gsMakelivelock_free()
{
  return ATmakeAppl0(gsAFunlivelock_free());
}

inline
ATermAppl gsMakemodel_compress()
{
  return ATmakeAppl0(gsAFunmodel_compress());
}

inline
ATermAppl gsMakenormal()
{
  return ATmakeAppl0(gsAFunnormal());
}

inline
ATermAppl gsMakenormalise()
{
  return ATmakeAppl0(gsAFunnormalise());
}

inline
ATermAppl gsMakenormalize()
{
  return ATmakeAppl0(gsAFunnormalize());
}

inline
ATermAppl gsMakeproductions(ATermAppl Expr_0)
{
  return ATmakeAppl1(gsAFunproductions(), (ATerm) Expr_0);
}

inline
ATermAppl gsMakesbsim()
{
  return ATmakeAppl0(gsAFunsbsim());
}

inline
ATermAppl gsMakeset(ATermAppl Seq_0)
{
  return ATmakeAppl1(gsAFunset(), (ATerm) Seq_0);
}

inline
ATermAppl gsMaketau_loop_factor()
{
  return ATmakeAppl0(gsAFuntau_loop_factor());
}

inline
ATermAppl gsMaketrue()
{
  return ATmakeAppl0(gsAFuntrue());
}

inline
ATermAppl gsMakeunion(ATermAppl Set_0, ATermAppl Set_1)
{
  return ATmakeAppl2(gsAFununion(), (ATerm) Set_0, (ATerm) Set_1);
}//--- end generated code ---//

} // namespace detail

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DETAIL_TERM_FUNCTIONS_H
