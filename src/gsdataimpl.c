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
      //logical negation
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(f), t),
      (ATerm) gsMakeDataEqn(bl, nil,
                     gsMakeDataExprNot(gsMakeDataExprNot(b)), b),
      //conjunction
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, t), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, f), f),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(f, b), f),
      //disjunction
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, f), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(t, b), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(f, b), b),
      //implication
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, f),
                                            gsMakeDataExprNot(b)),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(f, b), t),
      //equality
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, t), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, f), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, f), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(b, b), t),
      //inequality
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprNeq(b, c), 
                      gsMakeDataExprNot(gsMakeDataExprEq(b, c))),
      //conditional
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
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprPos());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprPos());
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(51,
      //equality
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(one, one), t),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprDot0(n)), f),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprDot1(n)), f),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprEq(gsMakeDataExprDot0(n), one), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot0(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprEq(m, n)),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot0(m), gsMakeDataExprDot1(n)), f),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprEq(gsMakeDataExprDot1(n), one), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot1(m), gsMakeDataExprDot0(n)), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprDot1(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprEq(m, n)),
      //inequality
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprNeq(m, n), gsMakeDataExprNot(gsMakeDataExprEq(m, n))),
      //conditional
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprIf(t, m, n), m),
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprIf(f, m, n), n),
      (ATerm) gsMakeDataEqn(bnl,nil, gsMakeDataExprIf(b, n, n), n),
      //less than
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprLT(one, one), f),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprDot0(n)), t),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprDot1(n)), t),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprLT(gsMakeDataExprDot0(n), one), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot0(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprLT(m, n)),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot0(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprLT(gsMakeDataExprDot1(n), one), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot1(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprLT(m, n)),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprDot1(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprLT(m, n)),
      //greater than
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGT(m, n), gsMakeDataExprNot(gsMakeDataExprLTE(n, m))),
      //less than or equal
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLTE(one, n), t),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot0(n), one), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot0(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot0(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot1(n), one), f),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot1(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprLT(m, n)),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprDot1(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprLTE(m, n)),
      //greater than or equal
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGTE(m, n), gsMakeDataExprNot(gsMakeDataExprLT(n, m))),
      //maximum
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMax(m, n),
         gsMakeDataExprIf(gsMakeDataExprGTE(m, n), m, n)),
      //minimum
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMin(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), m, n)),
      //successor
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprSucc(one), gsMakeDataExprDot0(one)),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprSucc(gsMakeDataExprDot0(n)), gsMakeDataExprDot1(n)),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprSucc(gsMakeDataExprDot1(n)),
         gsMakeDataExprDot0(gsMakeDataExprSucc(n))),
      //positive predecessor
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprPredPos(one), one),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot0(one)), one),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot0(gsMakeDataExprDot0(n))),
         gsMakeDataExprDot1(gsMakeDataExprPredPos(gsMakeDataExprDot0(n)))),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot0(gsMakeDataExprDot1(n))),
         gsMakeDataExprDot1(gsMakeDataExprPredPos(gsMakeDataExprDot1(n)))),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPredPos(gsMakeDataExprDot1(n)), gsMakeDataExprDot0(n)),
      //addition
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprAdd(one, n), gsMakeDataExprSucc(n)),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot0(n), one),
         gsMakeDataExprSucc(gsMakeDataExprDot0(n))),
      (ATerm) gsMakeDataEqn(nl, nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot1(n), one),
         gsMakeDataExprDot0(gsMakeDataExprSucc(n))),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot0(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprDot0(gsMakeDataExprAdd(m, n))),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot0(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprDot1(gsMakeDataExprAdd(m, n))),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot1(m), gsMakeDataExprDot0(n)),
         gsMakeDataExprDot1(gsMakeDataExprAdd(m, n))),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprAdd(gsMakeDataExprDot1(m), gsMakeDataExprDot1(n)),
         gsMakeDataExprDot0(gsMakeDataExprSucc(gsMakeDataExprAdd(m, n)))),
      //multiplication
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(one, n), n),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprMult(gsMakeDataExprDot0(m), n),
         gsMakeDataExprDot0(gsMakeDataExprMult(m, n))),
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprMult(gsMakeDataExprDot1(m), n),
         gsMakeDataExprAdd(gsMakeDataExprDot0(gsMakeDataExprMult(m, n)), n))
    ));
  return Spec;
}

ATermAppl gsImplementNat(ATermAppl Spec)
{
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
