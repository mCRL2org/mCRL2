#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include "gsdataimpl.h"
#include "gsfunc.h"

//local declarations that might be moved to gsfunc.h

ATermAppl gsDeclareSorts(ATermAppl Spec, ATermList SortIds);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     SortIds is a list of sort identifier
//Ret: Spec in which each element of SortId is added as a sort declaration

ATermAppl gsDeclareSort(ATermAppl Spec, ATermAppl SortId);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     SortId is a sort identifier
//Ret: Spec in which SortId is added as a sort declaration

ATermAppl gsDeclareConsOps(ATermAppl Spec, ATermList OpIds);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     OpIds is a list of operation identifiers
//Ret: Spec in which each element of OpIds is added as a constructor operation
//     declaration

ATermAppl gsDeclareConsOp(ATermAppl Spec, ATermAppl OpId);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     OpId is an of operation identifier
//Ret: Spec in which OpId is added as a constructor operation declaration

ATermAppl gsDeclareOps(ATermAppl Spec, ATermList OpIds);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     OpIds is a list of operation identifiers
//Ret: Spec in which each element of OpIds is added as an operation declaration

ATermAppl gsDeclareOp(ATermAppl Spec, ATermAppl OpId);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     OpId is an of operation identifier
//Ret: Spec in which OpId is added as an operation declaration

ATermAppl gsDeclareDataEqns(ATermAppl Spec, ATermList DataEqns);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     DataEqns is a list of data equations
//Ret: Spec in which each element of DataEqns is added to the list of data
//     equations

ATermAppl gsDeclareDataEqn(ATermAppl Spec, ATermAppl DataEqn);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//     DataEqn is a data equation
//Ret: Spec in which DataEqn is added to the list of data equations

//local declarations

ATermAppl gsImplementBool(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking
//Ret: Spec in which an implementation of sort Bool is added, i.e. sort,
//     operation and equation declarations are added.

ATermAppl gsImplementPos(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking.
//Ret: Spec in which an implementation of sort Pos is added, i.e. sort,
//     operation and equation declarations are added.

ATermAppl gsImplementNat(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking.
//Ret: Spec in which an implementation of sort Nat is added, i.e. sort,
//     operation and equation declarations are added.

ATermAppl gsImplementInt(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after type
//     checking.
//Ret: Spec in which an implementation of sort Int is added, i.e. sort,
//     operation and equation declarations are added.

//implementation

ATermAppl gsDeclareSorts(ATermAppl Spec, ATermList SortIds)
{
  ATermAppl SortSpec = ATAgetArgument(Spec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  SortDecls = ATconcat(SortIds, SortDecls);
  SortSpec = ATsetArgument(SortSpec, (ATerm) SortDecls, 0);  
  return ATsetArgument(Spec, (ATerm) SortSpec, 0);
}

ATermAppl gsDeclareSort(ATermAppl Spec, ATermAppl SortId)
{
  ATermAppl SortSpec = ATAgetArgument(Spec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  SortDecls = ATinsert(SortDecls, (ATerm) SortId);
  SortSpec = ATsetArgument(SortSpec, (ATerm) SortDecls, 0);  
  return ATsetArgument(Spec, (ATerm) SortSpec, 0);
}

ATermAppl gsDeclareConsOps(ATermAppl Spec, ATermList OpIds)
{
  ATermAppl ConsSpec = ATAgetArgument(Spec, 1);
  ATermList ConsDecls = ATLgetArgument(ConsSpec, 0);
  ConsDecls = ATconcat(OpIds, ConsDecls);
  ConsSpec = ATsetArgument(ConsSpec, (ATerm) ConsDecls, 0);  
  return ATsetArgument(Spec, (ATerm) ConsSpec, 1);
}

ATermAppl gsDeclareConsOp(ATermAppl Spec, ATermAppl OpId)
{
  ATermAppl ConsSpec = ATAgetArgument(Spec, 1);
  ATermList ConsDecls = ATLgetArgument(ConsSpec, 0);
  ConsDecls = ATinsert(ConsDecls, (ATerm) OpId);
  ConsSpec = ATsetArgument(ConsSpec, (ATerm) ConsDecls, 0);  
  return ATsetArgument(Spec, (ATerm) ConsSpec, 1);
}

ATermAppl gsDeclareOps(ATermAppl Spec, ATermList OpIds)
{
  ATermAppl MapSpec = ATAgetArgument(Spec, 2);
  ATermList MapDecls = ATLgetArgument(MapSpec, 0);
  MapDecls = ATconcat(OpIds, MapDecls);
  MapSpec = ATsetArgument(MapSpec, (ATerm) MapDecls, 0);  
  return ATsetArgument(Spec, (ATerm) MapSpec, 2);
}

ATermAppl gsDeclareOp(ATermAppl Spec, ATermAppl OpId)
{
  ATermAppl MapSpec = ATAgetArgument(Spec, 2);
  ATermList MapDecls = ATLgetArgument(MapSpec, 0);
  MapDecls = ATinsert(MapDecls, (ATerm) OpId);
  MapSpec = ATsetArgument(MapSpec, (ATerm) MapDecls, 0);  
  return ATsetArgument(Spec, (ATerm) MapSpec, 2);
}

ATermAppl gsDeclareDataEqns(ATermAppl Spec, ATermList DataEqns)
{
  ATermAppl DataEqnSpec = ATAgetArgument(Spec, 3);
  ATermList DataEqnDecls = ATLgetArgument(DataEqnSpec, 0);
  DataEqnDecls = ATconcat(DataEqns, DataEqnDecls);
  DataEqnSpec = ATsetArgument(DataEqnSpec, (ATerm) DataEqnDecls, 0);  
  return ATsetArgument(Spec, (ATerm) DataEqnSpec, 3);
}

ATermAppl gsDeclareDataEqn(ATermAppl Spec, ATermAppl DataEqn)
{
  ATermAppl DataEqnSpec = ATAgetArgument(Spec, 3);
  ATermList DataEqnDecls = ATLgetArgument(DataEqnSpec, 0);
  DataEqnDecls = ATinsert(DataEqnDecls, (ATerm) DataEqn);
  DataEqnSpec = ATsetArgument(DataEqnSpec, (ATerm) DataEqnDecls, 0);  
  return ATsetArgument(Spec, (ATerm) DataEqnSpec, 3);
}

ATermAppl gsImplementBool(ATermAppl Spec)
{
  //Declare sort Bool
  Spec = gsDeclareSort(Spec, gsMakeSortIdBool());
  //Declare constructors for sort Bool
  Spec = gsDeclareConsOps(Spec, ATmakeList2(
      (ATerm) gsMakeOpIdTrue(),
      (ATerm) gsMakeOpIdFalse()
    ));
  //Declare operations for sort Bool
  Spec = gsDeclareOps(Spec, ATmakeList(7,
      (ATerm) gsMakeOpIdNot(),
      (ATerm) gsMakeOpIdAnd(),
      (ATerm) gsMakeOpIdOr(),
      (ATerm) gsMakeOpIdImp(),
      (ATerm) gsMakeOpIdEq(gsMakeSortExprBool()),
      (ATerm) gsMakeOpIdNeq(gsMakeSortExprBool()),
      (ATerm) gsMakeOpIdIf(gsMakeSortExprBool())
    ));
  //Declare data equations for sort Bool
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), gsMakeSortExprBool());
  ATermList bl = ATmakeList1((ATerm) b);
  ATermList bcl = ATmakeList2((ATerm) b, (ATerm) c);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(24,
      //logical negation (Bool -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(f), t),
      (ATerm) gsMakeDataEqn(bl, nil,
                     gsMakeDataExprNot(gsMakeDataExprNot(b)), b),
      //conjunction (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, t), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, f), f),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(f, b), f),
      //disjunction (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, f), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(t, b), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(f, b), b),
      //implication (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, f),
                                            gsMakeDataExprNot(b)),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(f, b), t),
      //equality (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, t), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, f), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, f), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(b, b), t),
      //inequality (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprNeq(b, c), 
                      gsMakeDataExprNot(gsMakeDataExprEq(b, c))),
      //conditional (Bool -> Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprIf(t, b, c), b),
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprIf(f, b, c), c),
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprIf(b, c, c), c)
    ));
  return Spec;
}

ATermAppl gsImplementPos(ATermAppl Spec)
{
  //Declare sort Pos
  Spec = gsDeclareSort(Spec, gsMakeSortIdPos());
  //Declare constructors for sort Pos
  Spec = gsDeclareConsOps(Spec, ATmakeList3(
      (ATerm) gsMakeOpId1(),
      (ATerm) gsMakeOpIdDot0(),
      (ATerm) gsMakeOpIdDot1()
    ));
  //Declare operations for sort Pos
  ATermAppl sePos = gsMakeSortExprPos();
  Spec = gsDeclareOps(Spec, ATmakeList(13,
      (ATerm) gsMakeOpIdEq(sePos),
      (ATerm) gsMakeOpIdNeq(sePos),
      (ATerm) gsMakeOpIdIf(sePos),
      (ATerm) gsMakeOpIdLT(sePos),
      (ATerm) gsMakeOpIdGT(sePos),
      (ATerm) gsMakeOpIdLTE(sePos),
      (ATerm) gsMakeOpIdGTE(sePos),
      (ATerm) gsMakeOpIdMax(sePos),
      (ATerm) gsMakeOpIdMin(sePos),
      (ATerm) gsMakeOpIdSucc(sePos),
      (ATerm) gsMakeOpIdPredPos(),
      (ATerm) gsMakeOpIdAdd(sePos),
      (ATerm) gsMakeOpIdMult(sePos)
    ));
  //Declare data equations for sort Pos
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExpr1();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(50,
      //equality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(one, one), t),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprDot0(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprDot1(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprEq(gsMakeDataExprDot0(p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot0(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot0(p), gsMakeDataExprDot1(q)), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprEq(gsMakeDataExprDot1(p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot1(p), gsMakeDataExprDot0(q)), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot1(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprEq(p, q)),
      //inequality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprNeq(p, q), gsMakeDataExprNot(gsMakeDataExprEq(p, q))),
      //conditional (Bool -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprIf(t, p, q), p),
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprIf(f, p, q), q),
      (ATerm) gsMakeDataEqn(bpl,nil, gsMakeDataExprIf(b, p, p), p),
      //less than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprLT(one, one), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprDot0(p)), t),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprDot1(p)), t),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(gsMakeDataExprDot0(p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot0(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot0(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(gsMakeDataExprDot1(p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot1(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot1(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprLT(p, q)),
      //greater than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGT(p, q), gsMakeDataExprNot(gsMakeDataExprLTE(q, p))),
      //less than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLTE(one, p), t),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot0(p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot0(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot0(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot1(p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot1(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot1(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprLTE(p, q)),
      //greater than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGTE(p, q), gsMakeDataExprNot(gsMakeDataExprLT(q, p))),
      //maximum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMax(p, q),
         gsMakeDataExprIf(gsMakeDataExprGTE(p, q), q, p)),
      //minimum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMin(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), p, q)),
      //successor (Pos -> Pos)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprSucc(one), gsMakeDataExprDot0(one)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprDot0(p)), gsMakeDataExprDot1(p)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprDot1(p)),
         gsMakeDataExprDot0(gsMakeDataExprSucc(p))),
      //positive predecessor (Pos -> Pos)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot0(one)), one),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot0(gsMakeDataExprDot0(p))),
         gsMakeDataExprDot1(gsMakeDataExprPredPos(gsMakeDataExprDot0(p)))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot0(gsMakeDataExprDot1(p))),
         gsMakeDataExprDot1(gsMakeDataExprPredPos(gsMakeDataExprDot1(p)))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot1(p)), gsMakeDataExprDot0(p)),
      //addition (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAdd(one, p), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot0(p), one),
         gsMakeDataExprSucc(gsMakeDataExprDot0(p))),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot1(p), one),
         gsMakeDataExprDot0(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot0(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprDot0(gsMakeDataExprAdd(p, q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot0(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprDot1(gsMakeDataExprAdd(p, q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot1(p), gsMakeDataExprDot0(q)),
         gsMakeDataExprDot1(gsMakeDataExprAdd(p, q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot1(p), gsMakeDataExprDot1(q)),
         gsMakeDataExprDot0(gsMakeDataExprSucc(gsMakeDataExprAdd(p, q)))),
      //multiplication (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprMult(one, p), p),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMult(gsMakeDataExprDot0(p), q),
         gsMakeDataExprDot0(gsMakeDataExprMult(p, q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMult(gsMakeDataExprDot1(p), q),
         gsMakeDataExprAdd(gsMakeDataExprDot0(gsMakeDataExprMult(p, q)), q))
    ));
  return Spec;
}

ATermAppl gsImplementNat(ATermAppl Spec)
{
  //Declare sort Nat
  Spec = gsDeclareSort(Spec, gsMakeSortIdNat());
  //Declare constructors for sort Nat
  Spec = gsDeclareConsOps(Spec, ATmakeList2(
      (ATerm) gsMakeOpId0(),
      (ATerm) gsMakeOpIdCNat()
    ));
  //Declare operations for sort Nat
  ATermAppl seNat = gsMakeSortExprNat();
  ATermAppl sePos = gsMakeSortExprPos();
  Spec = gsDeclareOps(Spec, ATmakeList(19,
      (ATerm) gsMakeOpIdEq(seNat),
      (ATerm) gsMakeOpIdNeq(seNat),
      (ATerm) gsMakeOpIdIf(seNat),
      (ATerm) gsMakeOpIdPos2Nat(),
      (ATerm) gsMakeOpIdNat2Pos(),
      (ATerm) gsMakeOpIdLT(seNat),
      (ATerm) gsMakeOpIdGT(seNat),
      (ATerm) gsMakeOpIdLTE(seNat),
      (ATerm) gsMakeOpIdGTE(seNat),
      (ATerm) gsMakeOpIdMax(seNat),
      (ATerm) gsMakeOpIdMin(seNat),
      (ATerm) gsMakeOpIdSucc(seNat),
      (ATerm) gsMakeOpIdPred(sePos),
      (ATerm) gsMakeOpIdAdd(seNat),
      (ATerm) gsMakeOpIdMult(seNat),
      (ATerm) gsMakeOpIdDiv(seNat),
      (ATerm) gsMakeOpIdMod(seNat),
      (ATerm) gsMakeOpIdExp(sePos),
      (ATerm) gsMakeOpIdExp(seNat)
    ));
  //Declare data equations for sort Nat
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExpr1();
  ATermAppl zero = gsMakeDataExpr0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(37,
      //equality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(zero, zero), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(zero, gsMakeDataExprCNat(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprEq(p, q)),
      //inequality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprNeq(m, n), gsMakeDataExprNot(gsMakeDataExprEq(m, n))),
      //conditional (Bool -> Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprIf(t, m, n), m),
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprIf(f, m, n), n),
      (ATerm) gsMakeDataEqn(bnl,nil, gsMakeDataExprIf(b, n, n), n),
      //convert Pos to Nat (Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPos2Nat(p), gsMakeDataExprCNat(p)),
      //convert Nat to Pos (Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNat2Pos(gsMakeDataExprCNat(p)), p),
      //less than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprLT(zero, zero), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(zero, gsMakeDataExprCNat(p)), t),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLT(p, q)),
      //greater than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGT(m, n), gsMakeDataExprNot(gsMakeDataExprLTE(n, m))),
      //less than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLTE(zero, n), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLTE(p, q)),
      //greater than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGTE(m, n), gsMakeDataExprNot(gsMakeDataExprLT(n, m))),
      //maximum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMax(m, n),
         gsMakeDataExprIf(gsMakeDataExprGTE(m, n), m, n)),
      //minimum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMin(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), m, n)),
      //successor (Nat -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprSucc(zero), one),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNat(p)), gsMakeDataExprSucc(p)),
      //predecessor (Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprPred(p),
         gsMakeDataExprIf(gsMakeDataExprEq(p, one), zero,
           gsMakeDataExprCNat(gsMakeDataExprPredPos(p)))),
      //addition (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(zero, n), n),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(n, zero), n),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprAdd(p, q))),
      //multiplication (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(zero, n), n),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(n, zero), n),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMult(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprMult(p, q))),
      //quotient after division (Nat -> Pos -> Nat)
      //TODO
      //remainder after division (Nat -> Pos -> Nat)
      //TODO
      //exponentiation (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprExp(p, zero), one),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(p, gsMakeDataExprCNat(one)), p),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprDot0(q))),
         gsMakeDataExprExp(gsMakeDataExprMult(p, p), gsMakeDataExprCNat(q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprDot1(q))),
         gsMakeDataExprMult(p,
           gsMakeDataExprExp(gsMakeDataExprMult(p, p),
             gsMakeDataExprCNat(q)))),
      //exponentiation (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprExp(zero, zero),
        gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(zero, gsMakeDataExprCNat(p)), zero),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprExp(gsMakeDataExprCNat(p), n),
         gsMakeDataExprCNat(gsMakeDataExprExp(p, n)))
    ));
  return Spec;
}

ATermAppl gsImplementInt(ATermAppl Spec)
{
  return Spec;
}

ATermAppl gsImplementData(ATermAppl Spec)
{
  Spec = gsImplementInt(Spec);
  Spec = gsImplementNat(Spec);
  Spec = gsImplementPos(Spec);
  Spec = gsImplementBool(Spec);
  gsVerboseMsg("data implementation is not yet fully implemented\n");
  return Spec;
}
