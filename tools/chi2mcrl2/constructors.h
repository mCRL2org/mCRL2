// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/detail/constructors.h
/// \brief Add your file description here.

#ifndef LPS_DETAIL_CONSTRUCTORS
#define LPS_DETAIL_CONSTRUCTORS

#include "libstruct_core.h" // gsString2ATermAppl

namespace lps
{

namespace detail
{

// String
inline
ATermAppl initConstructString(ATermAppl& t)
{
  t = gsString2ATermAppl("@NoValue");
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructString()
{
  static ATermAppl t = initConstructString(t);
  return t;
}

// List
inline
ATermList constructList()
{
  return ATmakeList0();
}

//--- begin generated code
ATermAppl constructInstantiation();
ATermAppl constructChiSpec();
ATermAppl constructChanSpec();
ATermAppl constructChannelID();
ATermAppl constructDeltaStat();
ATermAppl constructSend();
ATermAppl constructDataVarExprID();
ATermAppl constructBinarySetExpression();
ATermAppl constructType();
ATermAppl constructSepStat();
ATermAppl constructOptGuard();
ATermAppl constructFunction();
ATermAppl constructModelDef();
ATermAppl constructNil();
ATermAppl constructTupleDot();
ATermAppl constructAltStat();
ATermAppl constructDataVarID();
ATermAppl constructAssignmentGGStat();
ATermAppl constructParStat();
ATermAppl constructListLiteral();
ATermAppl constructAssignmentStat();
ATermAppl constructRecv();
ATermAppl constructSetType();
ATermAppl constructFunction2();
ATermAppl constructProcDecl();
ATermAppl constructRecvStat();
ATermAppl constructDelta();
ATermAppl constructParenthesisedStat();
ATermAppl constructChannelTypedID();
ATermAppl constructTupleLiteral();
ATermAppl constructChanDecl();
ATermAppl constructStarStat();
ATermAppl constructSkipStat();
ATermAppl constructListType();
ATermAppl constructBinaryExpression();
ATermAppl constructOptChannel();
ATermAppl constructTupleType();
ATermAppl constructExpression();
ATermAppl constructProcDef();
ATermAppl constructUnaryExpression();
ATermAppl constructBinaryListExpression();
ATermAppl constructModelSpec();
ATermAppl constructVarDecl();
ATermAppl constructVarSpec();
ATermAppl constructSendStat();
ATermAppl constructSkip();
ATermAppl constructProcSpec();
ATermAppl constructSetLiteral();
ATermAppl constructGuardedStarStat();
ATermAppl constructSpecChi();
ATermAppl constructModelBody();
ATermAppl constructDecl();
ATermAppl constructProcBody();
ATermAppl constructVarExpID();
ATermAppl constructVarID();
ATermAppl constructTypeExp();
ATermAppl constructTypeID();
ATermAppl constructComTyp();
ATermAppl constructExpr();
ATermAppl constructStatement();

// Instantiation
inline
ATermAppl initConstructInstantiation(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunInstantiation(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructInstantiation()
{
  static ATermAppl t = initConstructInstantiation(t);
  return t;
}

// ChiSpec
inline
ATermAppl initConstructChiSpec(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunChiSpec(), reinterpret_cast<ATerm>(constructModelDef()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChiSpec()
{
  static ATermAppl t = initConstructChiSpec(t);
  return t;
}

// ChanSpec
inline
ATermAppl initConstructChanSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunChanSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChanSpec()
{
  static ATermAppl t = initConstructChanSpec(t);
  return t;
}

// ChannelID
inline
ATermAppl initConstructChannelID(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunChannelID(), reinterpret_cast<ATerm>(constructVarID()), reinterpret_cast<ATerm>(constructComType()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChannelID()
{
  static ATermAppl t = initConstructChannelID(t);
  return t;
}

// DeltaStat
inline
ATermAppl initConstructDeltaStat(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDeltaStat(), reinterpret_cast<ATerm>(constructOptGuard()), reinterpret_cast<ATerm>(constructDelta()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDeltaStat()
{
  static ATermAppl t = initConstructDeltaStat(t);
  return t;
}

// Send
inline
ATermAppl initConstructSend(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSend());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSend()
{
  static ATermAppl t = initConstructSend(t);
  return t;
}

// DataVarExprID
inline
ATermAppl initConstructDataVarExprID(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarExprID(), reinterpret_cast<ATerm>(constructVarID()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarExprID()
{
  static ATermAppl t = initConstructDataVarExprID(t);
  return t;
}

// BinarySetExpression
inline
ATermAppl initConstructBinarySetExpression(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunBinarySetExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBinarySetExpression()
{
  static ATermAppl t = initConstructBinarySetExpression(t);
  return t;
}

// Type
inline
ATermAppl initConstructType(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunType(), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructType()
{
  static ATermAppl t = initConstructType(t);
  return t;
}

// SepStat
inline
ATermAppl initConstructSepStat(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSepStat(), reinterpret_cast<ATerm>(constructStatement()), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSepStat()
{
  static ATermAppl t = initConstructSepStat(t);
  return t;
}

// OptGuard
inline
ATermAppl initConstructOptGuard(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunOptGuard());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructOptGuard()
{
  static ATermAppl t = initConstructOptGuard(t);
  return t;
}

// Function
inline
ATermAppl initConstructFunction(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunFunction(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructFunction()
{
  static ATermAppl t = initConstructFunction(t);
  return t;
}

// ModelDef
inline
ATermAppl initConstructModelDef(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunModelDef(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructModelBody()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructModelDef()
{
  static ATermAppl t = initConstructModelDef(t);
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

// TupleDot
inline
ATermAppl initConstructTupleDot(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunTupleDot(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTupleDot()
{
  static ATermAppl t = initConstructTupleDot(t);
  return t;
}

// AltStat
inline
ATermAppl initConstructAltStat(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAltStat(), reinterpret_cast<ATerm>(constructStatement()), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAltStat()
{
  static ATermAppl t = initConstructAltStat(t);
  return t;
}

// DataVarID
inline
ATermAppl initConstructDataVarID(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarID(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeExp()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarID()
{
  static ATermAppl t = initConstructDataVarID(t);
  return t;
}

// AssignmentGGStat
inline
ATermAppl initConstructAssignmentGGStat(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunAssignmentGGStat(), reinterpret_cast<ATerm>(constructOptGuard()), reinterpret_cast<ATerm>(constructOptChannel()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAssignmentGGStat()
{
  static ATermAppl t = initConstructAssignmentGGStat(t);
  return t;
}

// ParStat
inline
ATermAppl initConstructParStat(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunParStat(), reinterpret_cast<ATerm>(constructStatement()), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructParStat()
{
  static ATermAppl t = initConstructParStat(t);
  return t;
}

// ListLiteral
inline
ATermAppl initConstructListLiteral(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunListLiteral(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructListLiteral()
{
  static ATermAppl t = initConstructListLiteral(t);
  return t;
}

// AssignmentStat
inline
ATermAppl initConstructAssignmentStat(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunAssignmentStat(), reinterpret_cast<ATerm>(constructOptGuard()), reinterpret_cast<ATerm>(constructOptChannel()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAssignmentStat()
{
  static ATermAppl t = initConstructAssignmentStat(t);
  return t;
}

// Recv
inline
ATermAppl initConstructRecv(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunRecv());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRecv()
{
  static ATermAppl t = initConstructRecv(t);
  return t;
}

// SetType
inline
ATermAppl initConstructSetType(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSetType(), reinterpret_cast<ATerm>(constructTypeExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSetType()
{
  static ATermAppl t = initConstructSetType(t);
  return t;
}

// Function2
inline
ATermAppl initConstructFunction2(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunFunction2(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructFunction2()
{
  static ATermAppl t = initConstructFunction2(t);
  return t;
}

// ProcDecl
inline
ATermAppl initConstructProcDecl(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunProcDecl(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcDecl()
{
  static ATermAppl t = initConstructProcDecl(t);
  return t;
}

// RecvStat
inline
ATermAppl initConstructRecvStat(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunRecvStat(), reinterpret_cast<ATerm>(constructOptGuard()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRecvStat()
{
  static ATermAppl t = initConstructRecvStat(t);
  return t;
}

// Delta
inline
ATermAppl initConstructDelta(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunDelta());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDelta()
{
  static ATermAppl t = initConstructDelta(t);
  return t;
}

// ParenthesisedStat
inline
ATermAppl initConstructParenthesisedStat(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunParenthesisedStat(), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructParenthesisedStat()
{
  static ATermAppl t = initConstructParenthesisedStat(t);
  return t;
}

// ChannelTypedID
inline
ATermAppl initConstructChannelTypedID(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunChannelTypedID(), reinterpret_cast<ATerm>(constructChannelID()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChannelTypedID()
{
  static ATermAppl t = initConstructChannelTypedID(t);
  return t;
}

// TupleLiteral
inline
ATermAppl initConstructTupleLiteral(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunTupleLiteral(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTupleLiteral()
{
  static ATermAppl t = initConstructTupleLiteral(t);
  return t;
}

// ChanDecl
inline
ATermAppl initConstructChanDecl(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunChanDecl(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChanDecl()
{
  static ATermAppl t = initConstructChanDecl(t);
  return t;
}

// StarStat
inline
ATermAppl initConstructStarStat(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunStarStat(), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStarStat()
{
  static ATermAppl t = initConstructStarStat(t);
  return t;
}

// SkipStat
inline
ATermAppl initConstructSkipStat(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunSkipStat(), reinterpret_cast<ATerm>(constructOptGuard()), reinterpret_cast<ATerm>(constructOptChannel()), reinterpret_cast<ATerm>(constructSkip()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSkipStat()
{
  static ATermAppl t = initConstructSkipStat(t);
  return t;
}

// ListType
inline
ATermAppl initConstructListType(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunListType(), reinterpret_cast<ATerm>(constructTypeExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructListType()
{
  static ATermAppl t = initConstructListType(t);
  return t;
}

// BinaryExpression
inline
ATermAppl initConstructBinaryExpression(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunBinaryExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBinaryExpression()
{
  static ATermAppl t = initConstructBinaryExpression(t);
  return t;
}

// OptChannel
inline
ATermAppl initConstructOptChannel(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunOptChannel());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructOptChannel()
{
  static ATermAppl t = initConstructOptChannel(t);
  return t;
}

// TupleType
inline
ATermAppl initConstructTupleType(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunTupleType(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTupleType()
{
  static ATermAppl t = initConstructTupleType(t);
  return t;
}

// Expression
inline
ATermAppl initConstructExpression(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExpression()
{
  static ATermAppl t = initConstructExpression(t);
  return t;
}

// ProcDef
inline
ATermAppl initConstructProcDef(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunProcDef(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructProcDecl()), reinterpret_cast<ATerm>(constructProcBody()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcDef()
{
  static ATermAppl t = initConstructProcDef(t);
  return t;
}

// UnaryExpression
inline
ATermAppl initConstructUnaryExpression(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunUnaryExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructUnaryExpression()
{
  static ATermAppl t = initConstructUnaryExpression(t);
  return t;
}

// BinaryListExpression
inline
ATermAppl initConstructBinaryListExpression(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunBinaryListExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBinaryListExpression()
{
  static ATermAppl t = initConstructBinaryListExpression(t);
  return t;
}

// ModelSpec
inline
ATermAppl initConstructModelSpec(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunModelSpec(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructModelSpec()
{
  static ATermAppl t = initConstructModelSpec(t);
  return t;
}

// VarDecl
inline
ATermAppl initConstructVarDecl(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunVarDecl(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructVarDecl()
{
  static ATermAppl t = initConstructVarDecl(t);
  return t;
}

// VarSpec
inline
ATermAppl initConstructVarSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunVarSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructVarSpec()
{
  static ATermAppl t = initConstructVarSpec(t);
  return t;
}

// SendStat
inline
ATermAppl initConstructSendStat(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunSendStat(), reinterpret_cast<ATerm>(constructOptGuard()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSendStat()
{
  static ATermAppl t = initConstructSendStat(t);
  return t;
}

// Skip
inline
ATermAppl initConstructSkip(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSkip());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSkip()
{
  static ATermAppl t = initConstructSkip(t);
  return t;
}

// ProcSpec
inline
ATermAppl initConstructProcSpec(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcSpec(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcSpec()
{
  static ATermAppl t = initConstructProcSpec(t);
  return t;
}

// SetLiteral
inline
ATermAppl initConstructSetLiteral(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSetLiteral(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSetLiteral()
{
  static ATermAppl t = initConstructSetLiteral(t);
  return t;
}

// GuardedStarStat
inline
ATermAppl initConstructGuardedStarStat(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunGuardedStarStat(), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructStatement()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructGuardedStarStat()
{
  static ATermAppl t = initConstructGuardedStarStat(t);
  return t;
}

// SpecChi
inline
ATermAppl constructSpecChi()
{
  return constructChiSpec();
}

// ModelBody
inline
ATermAppl constructModelBody()
{
  return constructModelSpec();
}

// Decl
inline
ATermAppl constructDecl()
{
  return constructVarDecl();
}

// ProcBody
inline
ATermAppl constructProcBody()
{
  return constructProcSpec();
}

// VarExpID
inline
ATermAppl constructVarExpID()
{
  return constructDataVarExprID();
}

// VarID
inline
ATermAppl constructVarID()
{
  return constructDataVarID();
}

// TypeExp
inline
ATermAppl constructTypeExp()
{
  return constructTypeID();
}

// TypeID
inline
ATermAppl constructTypeID()
{
  return constructType();
}

// ComTyp
inline
ATermAppl constructComTyp()
{
  return constructNil();
}

// Expr
inline
ATermAppl constructExpr()
{
  return constructExpression();
}

// Statement
inline
ATermAppl constructStatement()
{
  return constructSkipStat();
}
//--- end generated code

} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_CONSTRUCTORS
