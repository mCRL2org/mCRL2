// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libstruct_core.h

#ifndef MCRL2_LIBSTRUCT_CORE_H
#define MCRL2_LIBSTRUCT_CORE_H

//This file describes the functions that can be used for the internal ATerm
//structure.

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <aterm2.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

//Global precondition: the ATerm library has been initialised

// TODO: remove this function.
inline
void gsEnableCoreConstructorFunctions()
{}

//--- begin generated code
// AltStat
inline
AFun initAFunAltStat(AFun& f)
{
  f = ATmakeAFun("AltStat", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAltStat()
{
  static AFun AFunAltStat = initAFunAltStat(AFunAltStat);
  return AFunAltStat;
}

inline
bool gsIsAltStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAltStat();
}

// AssignmentGGStat
inline
AFun initAFunAssignmentGGStat(AFun& f)
{
  f = ATmakeAFun("AssignmentGGStat", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAssignmentGGStat()
{
  static AFun AFunAssignmentGGStat = initAFunAssignmentGGStat(AFunAssignmentGGStat);
  return AFunAssignmentGGStat;
}

inline
bool gsIsAssignmentGGStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAssignmentGGStat();
}

// AssignmentStat
inline
AFun initAFunAssignmentStat(AFun& f)
{
  f = ATmakeAFun("AssignmentStat", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAssignmentStat()
{
  static AFun AFunAssignmentStat = initAFunAssignmentStat(AFunAssignmentStat);
  return AFunAssignmentStat;
}

inline
bool gsIsAssignmentStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAssignmentStat();
}

// BinaryExpression
inline
AFun initAFunBinaryExpression(AFun& f)
{
  f = ATmakeAFun("BinaryExpression", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBinaryExpression()
{
  static AFun AFunBinaryExpression = initAFunBinaryExpression(AFunBinaryExpression);
  return AFunBinaryExpression;
}

inline
bool gsIsBinaryExpression(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBinaryExpression();
}

// ChanDecl
inline
AFun initAFunChanDecl(AFun& f)
{
  f = ATmakeAFun("ChanDecl", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChanDecl()
{
  static AFun AFunChanDecl = initAFunChanDecl(AFunChanDecl);
  return AFunChanDecl;
}

inline
bool gsIsChanDecl(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChanDecl();
}

// ChanSpec
inline
AFun initAFunChanSpec(AFun& f)
{
  f = ATmakeAFun("ChanSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChanSpec()
{
  static AFun AFunChanSpec = initAFunChanSpec(AFunChanSpec);
  return AFunChanSpec;
}

inline
bool gsIsChanSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChanSpec();
}

// ChannelID
inline
AFun initAFunChannelID(AFun& f)
{
  f = ATmakeAFun("ChannelID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChannelID()
{
  static AFun AFunChannelID = initAFunChannelID(AFunChannelID);
  return AFunChannelID;
}

inline
bool gsIsChannelID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChannelID();
}

// ChannelTypedID
inline
AFun initAFunChannelTypedID(AFun& f)
{
  f = ATmakeAFun("ChannelTypedID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChannelTypedID()
{
  static AFun AFunChannelTypedID = initAFunChannelTypedID(AFunChannelTypedID);
  return AFunChannelTypedID;
}

inline
bool gsIsChannelTypedID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChannelTypedID();
}

// ChiSpec
inline
AFun initAFunChiSpec(AFun& f)
{
  f = ATmakeAFun("ChiSpec", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChiSpec()
{
  static AFun AFunChiSpec = initAFunChiSpec(AFunChiSpec);
  return AFunChiSpec;
}

inline
bool gsIsChiSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChiSpec();
}

// DataVarExprID
inline
AFun initAFunDataVarExprID(AFun& f)
{
  f = ATmakeAFun("DataVarExprID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarExprID()
{
  static AFun AFunDataVarExprID = initAFunDataVarExprID(AFunDataVarExprID);
  return AFunDataVarExprID;
}

inline
bool gsIsDataVarExprID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarExprID();
}

// DataVarID
inline
AFun initAFunDataVarID(AFun& f)
{
  f = ATmakeAFun("DataVarID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarID()
{
  static AFun AFunDataVarID = initAFunDataVarID(AFunDataVarID);
  return AFunDataVarID;
}

inline
bool gsIsDataVarID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarID();
}

// Delta
inline
AFun initAFunDelta(AFun& f)
{
  f = ATmakeAFun("Delta", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDelta()
{
  static AFun AFunDelta = initAFunDelta(AFunDelta);
  return AFunDelta;
}

inline
bool gsIsDelta(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDelta();
}

// DeltaStat
inline
AFun initAFunDeltaStat(AFun& f)
{
  f = ATmakeAFun("DeltaStat", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDeltaStat()
{
  static AFun AFunDeltaStat = initAFunDeltaStat(AFunDeltaStat);
  return AFunDeltaStat;
}

inline
bool gsIsDeltaStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDeltaStat();
}

// Expression
inline
AFun initAFunExpression(AFun& f)
{
  f = ATmakeAFun("Expression", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExpression()
{
  static AFun AFunExpression = initAFunExpression(AFunExpression);
  return AFunExpression;
}

inline
bool gsIsExpression(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExpression();
}

// GuardedStarStat
inline
AFun initAFunGuardedStarStat(AFun& f)
{
  f = ATmakeAFun("GuardedStarStat", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunGuardedStarStat()
{
  static AFun AFunGuardedStarStat = initAFunGuardedStarStat(AFunGuardedStarStat);
  return AFunGuardedStarStat;
}

inline
bool gsIsGuardedStarStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunGuardedStarStat();
}

// Instantiation
inline
AFun initAFunInstantiation(AFun& f)
{
  f = ATmakeAFun("Instantiation", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunInstantiation()
{
  static AFun AFunInstantiation = initAFunInstantiation(AFunInstantiation);
  return AFunInstantiation;
}

inline
bool gsIsInstantiation(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunInstantiation();
}

// ModelDef
inline
AFun initAFunModelDef(AFun& f)
{
  f = ATmakeAFun("ModelDef", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunModelDef()
{
  static AFun AFunModelDef = initAFunModelDef(AFunModelDef);
  return AFunModelDef;
}

inline
bool gsIsModelDef(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunModelDef();
}

// ModelSpec
inline
AFun initAFunModelSpec(AFun& f)
{
  f = ATmakeAFun("ModelSpec", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunModelSpec()
{
  static AFun AFunModelSpec = initAFunModelSpec(AFunModelSpec);
  return AFunModelSpec;
}

inline
bool gsIsModelSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunModelSpec();
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

// OptChannel
inline
AFun initAFunOptChannel(AFun& f)
{
  f = ATmakeAFun("OptChannel", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunOptChannel()
{
  static AFun AFunOptChannel = initAFunOptChannel(AFunOptChannel);
  return AFunOptChannel;
}

inline
bool gsIsOptChannel(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunOptChannel();
}

// OptGuard
inline
AFun initAFunOptGuard(AFun& f)
{
  f = ATmakeAFun("OptGuard", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunOptGuard()
{
  static AFun AFunOptGuard = initAFunOptGuard(AFunOptGuard);
  return AFunOptGuard;
}

inline
bool gsIsOptGuard(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunOptGuard();
}

// ParStat
inline
AFun initAFunParStat(AFun& f)
{
  f = ATmakeAFun("ParStat", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunParStat()
{
  static AFun AFunParStat = initAFunParStat(AFunParStat);
  return AFunParStat;
}

inline
bool gsIsParStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunParStat();
}

// ParenthesisedStat
inline
AFun initAFunParenthesisedStat(AFun& f)
{
  f = ATmakeAFun("ParenthesisedStat", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunParenthesisedStat()
{
  static AFun AFunParenthesisedStat = initAFunParenthesisedStat(AFunParenthesisedStat);
  return AFunParenthesisedStat;
}

inline
bool gsIsParenthesisedStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunParenthesisedStat();
}

// ProcDecl
inline
AFun initAFunProcDecl(AFun& f)
{
  f = ATmakeAFun("ProcDecl", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcDecl()
{
  static AFun AFunProcDecl = initAFunProcDecl(AFunProcDecl);
  return AFunProcDecl;
}

inline
bool gsIsProcDecl(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcDecl();
}

// ProcDef
inline
AFun initAFunProcDef(AFun& f)
{
  f = ATmakeAFun("ProcDef", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcDef()
{
  static AFun AFunProcDef = initAFunProcDef(AFunProcDef);
  return AFunProcDef;
}

inline
bool gsIsProcDef(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcDef();
}

// ProcSpec
inline
AFun initAFunProcSpec(AFun& f)
{
  f = ATmakeAFun("ProcSpec", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcSpec()
{
  static AFun AFunProcSpec = initAFunProcSpec(AFunProcSpec);
  return AFunProcSpec;
}

inline
bool gsIsProcSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcSpec();
}

// Recv
inline
AFun initAFunRecv(AFun& f)
{
  f = ATmakeAFun("Recv", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRecv()
{
  static AFun AFunRecv = initAFunRecv(AFunRecv);
  return AFunRecv;
}

inline
bool gsIsRecv(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRecv();
}

// RecvStat
inline
AFun initAFunRecvStat(AFun& f)
{
  f = ATmakeAFun("RecvStat", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRecvStat()
{
  static AFun AFunRecvStat = initAFunRecvStat(AFunRecvStat);
  return AFunRecvStat;
}

inline
bool gsIsRecvStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRecvStat();
}

// Send
inline
AFun initAFunSend(AFun& f)
{
  f = ATmakeAFun("Send", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSend()
{
  static AFun AFunSend = initAFunSend(AFunSend);
  return AFunSend;
}

inline
bool gsIsSend(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSend();
}

// SendStat
inline
AFun initAFunSendStat(AFun& f)
{
  f = ATmakeAFun("SendStat", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSendStat()
{
  static AFun AFunSendStat = initAFunSendStat(AFunSendStat);
  return AFunSendStat;
}

inline
bool gsIsSendStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSendStat();
}

// SepStat
inline
AFun initAFunSepStat(AFun& f)
{
  f = ATmakeAFun("SepStat", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSepStat()
{
  static AFun AFunSepStat = initAFunSepStat(AFunSepStat);
  return AFunSepStat;
}

inline
bool gsIsSepStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSepStat();
}

// Skip
inline
AFun initAFunSkip(AFun& f)
{
  f = ATmakeAFun("Skip", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSkip()
{
  static AFun AFunSkip = initAFunSkip(AFunSkip);
  return AFunSkip;
}

inline
bool gsIsSkip(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSkip();
}

// SkipStat
inline
AFun initAFunSkipStat(AFun& f)
{
  f = ATmakeAFun("SkipStat", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSkipStat()
{
  static AFun AFunSkipStat = initAFunSkipStat(AFunSkipStat);
  return AFunSkipStat;
}

inline
bool gsIsSkipStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSkipStat();
}

// StarStat
inline
AFun initAFunStarStat(AFun& f)
{
  f = ATmakeAFun("StarStat", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStarStat()
{
  static AFun AFunStarStat = initAFunStarStat(AFunStarStat);
  return AFunStarStat;
}

inline
bool gsIsStarStat(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStarStat();
}

// Type
inline
AFun initAFunType(AFun& f)
{
  f = ATmakeAFun("Type", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunType()
{
  static AFun AFunType = initAFunType(AFunType);
  return AFunType;
}

inline
bool gsIsType(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunType();
}

// UnaryExpression
inline
AFun initAFunUnaryExpression(AFun& f)
{
  f = ATmakeAFun("UnaryExpression", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunUnaryExpression()
{
  static AFun AFunUnaryExpression = initAFunUnaryExpression(AFunUnaryExpression);
  return AFunUnaryExpression;
}

inline
bool gsIsUnaryExpression(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunUnaryExpression();
}

// VarDecl
inline
AFun initAFunVarDecl(AFun& f)
{
  f = ATmakeAFun("VarDecl", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunVarDecl()
{
  static AFun AFunVarDecl = initAFunVarDecl(AFunVarDecl);
  return AFunVarDecl;
}

inline
bool gsIsVarDecl(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunVarDecl();
}

// VarSpec
inline
AFun initAFunVarSpec(AFun& f)
{
  f = ATmakeAFun("VarSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunVarSpec()
{
  static AFun AFunVarSpec = initAFunVarSpec(AFunVarSpec);
  return AFunVarSpec;
}

inline
bool gsIsVarSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunVarSpec();
}

inline
ATermAppl gsMakeAltStat(ATermAppl Statement_0, ATermAppl Statement_1)
{
  return ATmakeAppl2(gsAFunAltStat(), (ATerm) Statement_0, (ATerm) Statement_1);
}

inline
ATermAppl gsMakeAssignmentGGStat(ATermAppl OptGuard_0, ATermAppl OptChannel_1, ATermList Expr_2, ATermList Expr_3)
{
  return ATmakeAppl4(gsAFunAssignmentGGStat(), (ATerm) OptGuard_0, (ATerm) OptChannel_1, (ATerm) Expr_2, (ATerm) Expr_3);
}

inline
ATermAppl gsMakeAssignmentStat(ATermAppl OptGuard_0, ATermAppl OptChannel_1, ATermList Expr_2, ATermList Expr_3)
{
  return ATmakeAppl4(gsAFunAssignmentStat(), (ATerm) OptGuard_0, (ATerm) OptChannel_1, (ATerm) Expr_2, (ATerm) Expr_3);
}

inline
ATermAppl gsMakeBinaryExpression(ATermAppl String_0, ATermAppl TypeID_1, ATermAppl Expr_2, ATermAppl Expr_3)
{
  return ATmakeAppl4(gsAFunBinaryExpression(), (ATerm) String_0, (ATerm) TypeID_1, (ATerm) Expr_2, (ATerm) Expr_3);
}

inline
ATermAppl gsMakeChanDecl(ATermList TypedChannels_0)
{
  return ATmakeAppl1(gsAFunChanDecl(), (ATerm) TypedChannels_0);
}

inline
ATermAppl gsMakeChanSpec(ATermList ChanID_0)
{
  return ATmakeAppl1(gsAFunChanSpec(), (ATerm) ChanID_0);
}

inline
ATermAppl gsMakeChannelID(ATermAppl VarID_0, ATermAppl ComType_1)
{
  return ATmakeAppl2(gsAFunChannelID(), (ATerm) VarID_0, (ATerm) ComType_1);
}

inline
ATermAppl gsMakeChannelTypedID(ATermAppl ChannelID_0, ATermAppl TypeID_1)
{
  return ATmakeAppl2(gsAFunChannelTypedID(), (ATerm) ChannelID_0, (ATerm) TypeID_1);
}

inline
ATermAppl gsMakeChiSpec(ATermAppl ModelDef_0, ATermList ProcDef_1)
{
  return ATmakeAppl2(gsAFunChiSpec(), (ATerm) ModelDef_0, (ATerm) ProcDef_1);
}

inline
ATermAppl gsMakeDataVarExprID(ATermAppl VarID_0, ATermAppl Expr_1)
{
  return ATmakeAppl2(gsAFunDataVarExprID(), (ATerm) VarID_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeDataVarID(ATermAppl String_0, ATermAppl TypeExp_1)
{
  return ATmakeAppl2(gsAFunDataVarID(), (ATerm) String_0, (ATerm) TypeExp_1);
}

inline
ATermAppl gsMakeDelta()
{
  return ATmakeAppl0(gsAFunDelta());
}

inline
ATermAppl gsMakeDeltaStat(ATermAppl OptGuard_0, ATermAppl Delta_1)
{
  return ATmakeAppl2(gsAFunDeltaStat(), (ATerm) OptGuard_0, (ATerm) Delta_1);
}

inline
ATermAppl gsMakeExpression(ATermAppl String_0, ATermAppl TypeID_1)
{
  return ATmakeAppl2(gsAFunExpression(), (ATerm) String_0, (ATerm) TypeID_1);
}

inline
ATermAppl gsMakeGuardedStarStat(ATermAppl Expr_0, ATermAppl Statement_1)
{
  return ATmakeAppl2(gsAFunGuardedStarStat(), (ATerm) Expr_0, (ATerm) Statement_1);
}

inline
ATermAppl gsMakeInstantiation(ATermAppl String_0, ATermList Expr_1)
{
  return ATmakeAppl2(gsAFunInstantiation(), (ATerm) String_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeModelDef(ATermAppl String_0, ATermAppl ModelBody_1)
{
  return ATmakeAppl2(gsAFunModelDef(), (ATerm) String_0, (ATerm) ModelBody_1);
}

inline
ATermAppl gsMakeModelSpec(ATermList VarSpec_0, ATermAppl Statement_1)
{
  return ATmakeAppl2(gsAFunModelSpec(), (ATerm) VarSpec_0, (ATerm) Statement_1);
}

inline
ATermAppl gsMakeNil()
{
  return ATmakeAppl0(gsAFunNil());
}

inline
ATermAppl gsMakeOptChannel()
{
  return ATmakeAppl0(gsAFunOptChannel());
}

inline
ATermAppl gsMakeOptGuard()
{
  return ATmakeAppl0(gsAFunOptGuard());
}

inline
ATermAppl gsMakeParStat(ATermAppl Statement_0, ATermAppl Statement_1)
{
  return ATmakeAppl2(gsAFunParStat(), (ATerm) Statement_0, (ATerm) Statement_1);
}

inline
ATermAppl gsMakeParenthesisedStat(ATermAppl Statement_0)
{
  return ATmakeAppl1(gsAFunParenthesisedStat(), (ATerm) Statement_0);
}

inline
ATermAppl gsMakeProcDecl(ATermList Decl_0)
{
  return ATmakeAppl1(gsAFunProcDecl(), (ATerm) Decl_0);
}

inline
ATermAppl gsMakeProcDef(ATermAppl String_0, ATermAppl ProcDecl_1, ATermAppl ProcBody_2)
{
  return ATmakeAppl3(gsAFunProcDef(), (ATerm) String_0, (ATerm) ProcDecl_1, (ATerm) ProcBody_2);
}

inline
ATermAppl gsMakeProcSpec(ATermList VarSpec_0, ATermAppl Statement_1)
{
  return ATmakeAppl2(gsAFunProcSpec(), (ATerm) VarSpec_0, (ATerm) Statement_1);
}

inline
ATermAppl gsMakeRecv()
{
  return ATmakeAppl0(gsAFunRecv());
}

inline
ATermAppl gsMakeRecvStat(ATermAppl OptGuard_0, ATermAppl Expr_1, ATermList Expr_2)
{
  return ATmakeAppl3(gsAFunRecvStat(), (ATerm) OptGuard_0, (ATerm) Expr_1, (ATerm) Expr_2);
}

inline
ATermAppl gsMakeSend()
{
  return ATmakeAppl0(gsAFunSend());
}

inline
ATermAppl gsMakeSendStat(ATermAppl OptGuard_0, ATermAppl Expr_1, ATermList Expr_2)
{
  return ATmakeAppl3(gsAFunSendStat(), (ATerm) OptGuard_0, (ATerm) Expr_1, (ATerm) Expr_2);
}

inline
ATermAppl gsMakeSepStat(ATermAppl Statement_0, ATermAppl Statement_1)
{
  return ATmakeAppl2(gsAFunSepStat(), (ATerm) Statement_0, (ATerm) Statement_1);
}

inline
ATermAppl gsMakeSkip()
{
  return ATmakeAppl0(gsAFunSkip());
}

inline
ATermAppl gsMakeSkipStat(ATermAppl OptGuard_0, ATermAppl OptChannel_1, ATermAppl Skip_2)
{
  return ATmakeAppl3(gsAFunSkipStat(), (ATerm) OptGuard_0, (ATerm) OptChannel_1, (ATerm) Skip_2);
}

inline
ATermAppl gsMakeStarStat(ATermAppl Statement_0)
{
  return ATmakeAppl1(gsAFunStarStat(), (ATerm) Statement_0);
}

inline
ATermAppl gsMakeType(ATermAppl String_0)
{
  return ATmakeAppl1(gsAFunType(), (ATerm) String_0);
}

inline
ATermAppl gsMakeUnaryExpression(ATermAppl String_0, ATermAppl TypeID_1, ATermAppl Expr_2)
{
  return ATmakeAppl3(gsAFunUnaryExpression(), (ATerm) String_0, (ATerm) TypeID_1, (ATerm) Expr_2);
}

inline
ATermAppl gsMakeVarDecl(ATermList VarID_0)
{
  return ATmakeAppl1(gsAFunVarDecl(), (ATerm) VarID_0);
}

inline
ATermAppl gsMakeVarSpec(ATermList VarExpID_0)
{
  return ATmakeAppl1(gsAFunVarSpec(), (ATerm) VarExpID_0);
}
//--- end generated code

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

inline
ATermAppl gsString2ATermAppl(const char *s)
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL
{
  if (s != NULL) {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  } else {
    return gsMakeNil();   
  }
}

inline
bool gsIsString(ATermAppl term)
//Ret: term is a quoted constant
{
  AFun head = ATgetAFun(term);
  return ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATtrue));
}

inline
char *gsATermAppl2String(ATermAppl term)
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise
{
  if (gsIsString(term)) {
    return ATgetName(ATgetAFun(term));
  } else {
    return NULL;
  }
}

inline
bool gsIsNumericString(const char* s)
//Ret: true if s is of form "0 | -? [1-9][0-9]*", false otherwise
{
  if (s == NULL) return false;
  if (s[0] == '\0') return false;
  if (s[0] == '-') ++s;
  if (s[0] == '\0') return false;
  if (s[0] == '0') {
    ++s;
    if (s[0] == '\0') return true;
    else return false;
  }
  for (; s[0] != '\0'; ++s)
    if(!isdigit(s[0])) return false;
  return true;
}

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//     s is not NULL
//Ret: "s", if it does not occur in Term, and TryNoSuffix holds
//     "sk" as a quoted ATermAppl constant, where k is the smallest natural
//     number such that "sk" does not occur in Term, otherwise

#ifdef __cplusplus
}
#endif

#endif // MCRL2_LIBSTRUCT_CORE_H
