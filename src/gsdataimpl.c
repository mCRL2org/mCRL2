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
  Spec = gsDeclareDataEqns(Spec, ATmakeList(23,
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(f), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, t), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, f), f),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(f, b), f),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, f), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(t, b), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(f, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, f),
                                            gsMakeDataExprNot(b)),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(f, b), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, t), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, f), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, f), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(b, b), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNeq(b, c), 
                      gsMakeDataExprNot(gsMakeDataExprNeq(b, c))),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprIf(t, b, c), b),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprIf(f, b, c), c),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprIf(b, c, c), c)
    ));
}

ATermAppl gsImplementData(ATermAppl Spec)
{
  Spec = gsImplementBool(Spec);
  gsVerboseMsg("data implementation is not yet fully implemented\n");
  return Spec;
}
