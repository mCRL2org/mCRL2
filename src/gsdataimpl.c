#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#ifdef __cplusplus
}
#endif

#include "gsdataimpl.h"
#include "gsfunc.h"
#include "gslowlevel.h"

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

ATermAppl gsImplExprs(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     type checking
//Ret: Spec in which all expressions are implemented

ATermAppl gsImplSortRefs(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     data implementation, with the exception that sort references may occur
//Ret: Spec in which all sort references are implemented, i.e.:
//     - all sort references are removed from Spec
//     - if a sort reference is of the form SortRef(n, e), where e is the
//       implementation of a structured sort and SortRef(n, e) is the first
//       sort reference with e as a rhs, e is replaced by n in Spec;
//       otherwise, n is replaced by e in Spec

typedef struct {
  ATermList Sorts;
  ATermList ConsOps;
  ATermList Ops;
  ATermList DataEqns;
} TDataDecls;
//The type TDataDecls represents data declarations, i.e. sort, constructor,
//operation and data equation declarations

bool gsDataDeclsIsInitialised(TDataDecls DataDecls);
//Ret: indicates whether the elements of DataDecls are initialised

ATermAppl gsAddDataDecls(ATermAppl Spec, TDataDecls DataDecls);
//Pre: Spec is a specification that adheres to the internal syntax of an
//     arbitary phase
//Ret: Spec in which the data declarations from DataDecls are added

ATermAppl gsImplExprsPart(ATermAppl Part, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: Part is a part of a specification that adheres to the internal syntax
//     after type checking
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of Part
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of Part
//Ret: Part in which:
//     - all substitutions of *PSubsts are performed on the elements of Part
//     - each substituted element is implemented, where the new data
//       declarations are stored in *PDataDecls

ATermList gsImplExprsParts(ATermList Parts, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: Parts consists of parts of a specification that adheres to the internal
//     syntax after type checking
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of Parts
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of Part
//Ret: Parts in which:
//     - all substitutions of *PSubsts are performed on the elements of Parts
//     - each substituted element is implemented, where the new data
//       declarations are stored in *PDataDecls

ATermList gsGetFreeVars(ATermAppl DataExpr);
//Pre: DataExpr is a data expression that adheres to the internal syntax after
//     type checking
//Ret: The free variables in DataExpr

void gsGetFreeVars_Appl(ATermAppl DataExpr, ATermList BoundVars,
  ATermList* PFreeVars);
//Pre: DataExpr is a data expression or a bag enumeration element that adheres
//     to the internal format after type checking
//     BoundVars and *PFreeVars are lists of data variables, and represent the
//     bound/free variables of the context of DataExpr
//Post:*PFreeVars is extended with the free variables in DataExpr that did not
//     already occur in *PFreeVars or BoundVars

void gsGetFreeVars_List(ATermList DataExprs, ATermList BoundVars,
  ATermList* PFreeVars);
//Pre: DataExprs is a list of data expressions or bag enumeration elements that
//     adhere to the internal format after type checking
//     BoundVars and *PFreeVars are lists of data variables, and represent the
//     bound/free variables of the context of DataExprs
//Post:*PFreeVars is extended with the free variables in DataExprs that did not
//     already occur in *PFreeVars or BoundVars

ATermAppl gsImplSortStruct(ATermAppl SortStruct, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: SortStruct is a structured sort
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of SortStruct
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of SortStruct
//Post:an implementation of SortStruct is added to *PDataDecls and new induced
//     substitutions are added *PSubsts
//Ret: a sort identifier which is the implementation of SortStruct

ATermAppl gsImplSetBagEnum(ATermList Elts, ATermAppl SortExpr);
//Pre: Elts is a list containing 1 or more data expressions, all of the same
//     sort
//     SortExpr is a set or a bag sort
//Ret: Implementation of an enumeration of elements from Elts of sort SortExpr

void gsSplitSortDecls(ATermList SortDecls, ATermList *PSortIds,
  ATermList *PSortRefs);
//Pre: SortDecls is a list of SortId's and SortRef's
//Post:*PSortIds and *PSortRefs contain the SortId's and SortRef's from
//     SortDecls, in the same order

static const char *gsStructPrefix = "Struct@";
static const char *gsListPrefix   = "List@";
static const char *gsSetPrefix    = "Set@";
static const char *gsBagPrefix    = "Bag@";
static const char *gsLambdaPrefix = "lambda@";

ATermAppl gsMakeFreshStructSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: sort identifier for the implementation of a structured sort with prefix
//     gsStructPrefix, that does not occur in Term

ATermAppl gsMakeFreshListSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: fresh sort identifier for the implementation of a list sort with prefix
//     gsListPrefix, that does not occur in Term

ATermAppl gsMakeFreshSetSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: fresh sort identifier for the implementation of a set sort with prefix
//     gsSetPrefix, that does not occur in Term

ATermAppl gsMakeFreshBagSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: fresh sort identifier for the implementation of a bag sort with prefix
//     gsBagPrefix, that does not occur in Term

ATermAppl gsMakeFreshLambdaOpId(ATermAppl SortExpr, ATerm Term);
//Pre: SortExpr is a sort expression
//     Term is not NULL
//Ret: operation identifier OpId(n, s) for the implementation of a lambda
//     abstraction, where s is SortExpr and n is a name with prefix
//     gsLambdaPrefix, that does not occur in Term

bool gsIsStructSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a structured sort

bool gsIsListSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a list sort

bool gsIsSetSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a set sort

bool gsIsBagSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a bag sort

bool gsIsLambdaOpId(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: DataExpr is an operation identifier for the implementation of a lambda
//     abstraction

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

ATermAppl gsImplExprs(ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  //implement special sort and data expressions occurring in Spec
  ATermList Substs   = ATmakeList0();
  TDataDecls DataDecls;
  DataDecls.Sorts    = ATmakeList0();
  DataDecls.ConsOps  = ATmakeList0();
  DataDecls.Ops      = ATmakeList0();
  DataDecls.DataEqns = ATmakeList0();
  Spec = gsImplExprsPart(Spec, &Substs, &DataDecls);
  //perform substitutions on data declarations
  DataDecls.Sorts    = gsSubstValues_List(Substs, DataDecls.Sorts, true);
  DataDecls.ConsOps  = gsSubstValues_List(Substs, DataDecls.ConsOps, true);
  DataDecls.Ops      = gsSubstValues_List(Substs, DataDecls.Ops, true);
  DataDecls.DataEqns = gsSubstValues_List(Substs, DataDecls.DataEqns, true);
  //add declarations for sort Pos
  DataDecls.Sorts = ATinsert(DataDecls.Sorts, (ATerm) gsMakeSortIdPos());
  //add declarations for sort Bool
  DataDecls.Sorts = ATinsert(DataDecls.Sorts, (ATerm) gsMakeSortIdBool());
  //add new data declarations to Spec
  Spec = gsAddDataDecls(Spec, DataDecls);
  return Spec;
}

bool gsDataDeclsIsInitialised(TDataDecls DataDecls)
{
  return
    DataDecls.Sorts != NULL && DataDecls.ConsOps  != NULL &&
    DataDecls.Ops   != NULL && DataDecls.DataEqns != NULL;
}

ATermAppl gsAddDataDecls(ATermAppl Spec, TDataDecls DataDecls)
{
  assert(gsIsSpecV1(Spec));
  assert(gsDataDeclsIsInitialised(DataDecls));
  //add sort declarations
  ATermAppl SortSpec  = ATAgetArgument(Spec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  SortDecls = ATconcat(DataDecls.Sorts, SortDecls);
  SortSpec = ATsetArgument(SortSpec, (ATerm) SortDecls, 0);  
  Spec = ATsetArgument(Spec, (ATerm) SortSpec, 0);
  //add constructor operation declarations
  ATermAppl ConsSpec  = ATAgetArgument(Spec, 1);
  ATermList ConsDecls = ATLgetArgument(ConsSpec, 0);
  ConsDecls = ATconcat(DataDecls.ConsOps, ConsDecls);
  ConsSpec = ATsetArgument(ConsSpec, (ATerm) ConsDecls, 0);  
  Spec = ATsetArgument(Spec, (ATerm) ConsSpec, 1);
  //add operation declarations
  ATermAppl MapSpec  = ATAgetArgument(Spec, 2);
  ATermList MapDecls = ATLgetArgument(MapSpec, 0);
  MapDecls = ATconcat(DataDecls.Ops, MapDecls);
  MapSpec = ATsetArgument(MapSpec, (ATerm) MapDecls, 0);  
  Spec = ATsetArgument(Spec, (ATerm) MapSpec, 2);
  //add data equation declarations
  ATermAppl DataEqnSpec  = ATAgetArgument(Spec, 3);
  ATermList DataEqnDecls = ATLgetArgument(DataEqnSpec, 0);
  DataEqnDecls = ATconcat(DataDecls.DataEqns, DataEqnDecls);
  DataEqnSpec = ATsetArgument(DataEqnSpec, (ATerm) DataEqnDecls, 0);  
  Spec = ATsetArgument(Spec, (ATerm) DataEqnSpec, 3);
  //return the new specification
  return Spec;
}

ATermAppl gsImplExprsPart(ATermAppl Part, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  bool Recursive = true;
  //perform substitutions from *PSubsts on Part
  Part = gsSubstValues_Appl(*PSubsts, Part, false);
  //replace Part by an implementation if the head of Part is a special
  //expression
  if (gsIsSortArrowProd(Part)) {
    //Part is a product arrow sort; replace by arrow sorts
    ATermList l = ATreverse(ATLgetArgument(Part, 0));
    Part = ATAgetArgument(Part, 1);
    while (!ATisEmpty(l))
    {
      Part = gsMakeSortArrow(ATAgetFirst(l), Part);
      l = ATgetNext(l);
    }
  } else if (gsIsSortStruct(Part)) {
    //Part is a structured sort; replace by a new sort and add data
    //declarations for this sort
    Part = gsImplSortStruct(Part, PSubsts, PDataDecls);
  } else if (gsIsSortList(Part)) {
    //Part is a list sort; replace by a new sort and add data declarations for
    //this sort
    ATermAppl SortId = gsMakeFreshListSortId((ATerm) PDataDecls->Sorts);
    PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
    ATermAppl Subst = gsMakeSubst(Part, SortId);
    *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
    Part = SortId;
  } else if (gsIsSortSet(Part)) {
    //Part is a set sort; replace by a new sort and add data declarations for
    //this sort
    ATermAppl SortId = gsMakeFreshSetSortId((ATerm) PDataDecls->Sorts);
    PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
    ATermAppl Subst = gsMakeSubst(Part, SortId);
    *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
    Part = SortId;
  } else if (gsIsSortBag(Part)) {
    //Part is a bag sort; replace by a new sort and add data declarations for
    //this sort
    ATermAppl SortId = gsMakeFreshBagSortId((ATerm) PDataDecls->Sorts);
    PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
    ATermAppl Subst = gsMakeSubst(Part, SortId);
    *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
    Part = SortId;
  } else if (gsIsSortId(Part)) {
    //Part is a sort identifier; add data declarations for this sort, if needed
    if (ATisEqual(Part, gsMakeSortIdInt())) {
      //add sort Int, if necessary
      if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
        PDataDecls->Sorts =
          ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdInt());
      }
    }
    if (ATisEqual(Part,gsMakeSortIdNat()) || ATisEqual(Part,gsMakeSortIdInt()))
    {
      //add sort Nat, if necessary
      if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
        PDataDecls->Sorts =
          ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat());
      }
    }
  } else if (gsIsDataApplProd(Part)) {
    //Part is a product data application; replace by data applications
    ATermList l = ATLgetArgument(Part, 1);
    Part = ATAgetArgument(Part, 0);
    while (!ATisEmpty(l))
    {
      Part = gsMakeDataAppl(Part, ATAgetFirst(l));
      l = ATgetNext(l);
    }
  } else if (gsIsNumber(Part)) {
    //Part is a number; replace by its internal representation
    ATermAppl Number = ATAgetArgument(Part, 0);
    ATermAppl Sort = ATAgetArgument(Part, 1);
    if (ATisEqual(Sort, gsMakeSortExprPos()))
      Part = gsMakeDataExprPos(gsATermAppl2String(Number));
    else if (ATisEqual(Sort, gsMakeSortExprNat()))
      Part = gsMakeDataExprNat(gsATermAppl2String(Number));
    else if (ATisEqual(Sort, gsMakeSortExprInt()))
      Part = gsMakeDataExprInt(gsATermAppl2String(Number));
    else //sort of Part is wrong
      gsWarningMsg("%t can not be implemented because its sort differs from "
        "Pos, Nat or Int\n", Part);
  } else if (gsIsListEnum(Part)) {
    //Part is a list enumeration; replace by its internal representation
    ATermList Elts = ATLgetArgument(Part, 0);
    ATermAppl Sort = ATAgetArgument(Part, 1);
    if (ATgetLength(Elts) == 0) {
      //enumeration consists of 0 elements
      gsWarningMsg(
        "%t can not be implemented because it has 0 elements\n", Part);
    } else if (!gsIsSortList(Sort)) {
      //sort of the enumeration is wrong
      gsWarningMsg(
        "%t can not be implemented because its sort is not a list sort\n",
        Part);
    } else {
      //make cons list
      Elts = ATreverse(Elts);
      Part = gsMakeDataExprEmptyList(Sort);
      while (!ATisEmpty(Elts))
      {
        Part = gsMakeDataExprCons(ATAgetFirst(Elts), Part);
        Elts = ATgetNext(Elts);
      }
    }
  } else if (gsIsSetEnum(Part) || gsIsBagEnum(Part)) {
    //Part is a set/bag enumeration; replace by a set/bag comprehension
    ATermList Elts = ATLgetArgument(Part, 0);
    ATermAppl Sort = ATAgetArgument(Part, 1);
    if (ATgetLength(Elts) == 0) {
      //enumeration consists of 0 elements
      gsWarningMsg(
        "%t can not be implemented because it has 0 elements\n", Part);
    } else if ((gsIsSetEnum(Part) && !gsIsSortSet(Sort)) ||
               (gsIsBagEnum(Part) && !gsIsSortBag(Sort))) {
      //sort of the enumeration is wrong
      gsWarningMsg(
        "%t can not be implemented because its sort is not a %s sort\n", Part,
        gsIsSetEnum(Part)?"set":"bag");
    } else {
      Part = gsImplSetBagEnum(Elts, Sort);
    }
  } else if (gsIsSetBagComp(Part)) {
    //Part is a set/bag comprehension; replace by its implementation
    ATermAppl Var = ATAgetArgument(Part, 0);
    ATermAppl Body = ATAgetArgument(Part, 1);
    ATermAppl BodySort = gsGetSort(Body);
    ATermAppl VarSort = gsGetSort(Var);
    if (!(ATisEqual(BodySort, gsMakeSortIdBool()) ||
        ATisEqual(BodySort, gsMakeSortIdNat()))) {
      //sort of the comprehension is wrong
      gsWarningMsg("%t can not be implemented because the body is of sort %t "
        "instead of Bool or Nat\n", Part, BodySort);
    } else {
      if (ATisEqual(BodySort, gsMakeSortIdBool())) {
        //Part is a set comprehension
        Part = gsMakeDataExprSetComp(gsMakeLambda(ATmakeList1((ATerm) Var),
          Body), gsMakeSortSet(VarSort));
      } else {
        //Part is a bag comprehension
        Part = gsMakeDataExprBagComp(gsMakeLambda(ATmakeList1((ATerm) Var),
          Body), gsMakeSortBag(VarSort));
      }
    }
  } else if (gsIsForall(Part) || gsIsExists(Part)) {
    //Part is a quantification; replace by its implementation
    ATermList Vars = ATreverse(ATLgetArgument(Part, 0));
    bool IsForall = gsIsForall(Part);
    Part = gsMakeLambda(ATmakeList1(ATgetFirst(Vars)), ATAgetArgument(Part, 1));
    Part = IsForall?gsMakeDataExprForall(Part):gsMakeDataExprExists(Part);
    Vars = ATgetNext(Vars);
    while (!ATisEmpty(Vars))
    {
      Part = gsMakeLambda(ATmakeList1(ATgetFirst(Vars)), Part);
      Part = IsForall?gsMakeDataExprForall(Part):gsMakeDataExprExists(Part);
      Vars = ATgetNext(Vars);      
    }
  } else if (gsIsLambda(Part)) {
    //Part is a lambda abstraction; replace by a named function
    //implement the body, the bound variables and the free variables
    ATermList BoundVars = gsImplExprsParts(ATLgetArgument(Part, 0),
      PSubsts, PDataDecls);
    ATermAppl Body = gsImplExprsPart(ATAgetArgument(Part, 1),
      PSubsts, PDataDecls);
    ATermList FreeVars = gsImplExprsParts(gsGetFreeVars(Part),
      PSubsts, PDataDecls);
    ATermList Vars = ATconcat(FreeVars, BoundVars);
    //create sort for the new operation identifier
    ATermAppl OpIdSort = gsGetSort(Body);
    ATermList l = ATreverse(Vars);
    while (!ATisEmpty(l))
    {
      OpIdSort = gsMakeSortArrow(ATAgetArgument(ATAgetFirst(l), 1), OpIdSort);
      l = ATgetNext(l);
    }
    //create new operation identifier
    ATermAppl OpId = gsMakeFreshLambdaOpId(OpIdSort, (ATerm) PDataDecls->Ops);
    //add operation identifier to the data declarations
    PDataDecls->Ops = ATinsert(PDataDecls->Ops, (ATerm) OpId);
    //add data equation for the operation to the data declarations
    PDataDecls->DataEqns = ATinsert(PDataDecls->DataEqns, (ATerm)
      gsMakeDataEqn(Vars, gsMakeNil(), gsMakeDataApplList(OpId, Vars), Body));
    //replace Part
    ATermAppl NewPart = gsMakeDataApplList(OpId, FreeVars);
    *PSubsts = gsAddSubstToSubsts(gsMakeSubst(Part, NewPart), *PSubsts);
    Part = NewPart;
    Recursive = false;
  } else if (gsIsWhr(Part)) {
    //Part is a where clause; replace by its corresponding lambda expression
    ATermAppl Body = ATAgetArgument(Part, 0);
    ATermList WhrDecls = ATLgetArgument(Part, 1);
    if (ATgetLength(WhrDecls) == 0) {
      //where clause consists of 0 where clause declarations
      gsWarningMsg("%t can not be implemented because it has 0 where clause "
         "declarations\n", Part);
    } else {
      //make list of variables and where expressions
      WhrDecls = ATreverse(WhrDecls);
      ATermList Vars = ATmakeList0();
      ATermList Exprs = ATmakeList0();
      while (!ATisEmpty(WhrDecls))
      {
        ATermAppl WhrDecl = ATAgetFirst(WhrDecls);
        ATermAppl VarName = ATAgetArgument(WhrDecl, 0);
        ATermAppl Expr = ATAgetArgument(WhrDecl, 1);
        Vars = ATinsert(Vars,
          (ATerm) gsMakeDataVarId(VarName, gsGetSort(Expr)));
        Exprs = ATinsert(Exprs, (ATerm) Expr);
        WhrDecls = ATgetNext(WhrDecls);
      }
      //replace Part
      Part = gsMakeDataApplList(gsMakeLambda(Vars, Body), Exprs);
    }
  }
  //implement expressions in the arguments of Part
  if (Recursive) {
    AFun Head = ATgetAFun(Part);
    int NrArgs = ATgetArity(Head);      
    if (NrArgs > 0) {
      ATerm Args[NrArgs];
      for (int i = 0; i < NrArgs; i++) {
        ATerm Arg = ATgetArgument(Part, i);
        if (ATgetType(Arg) == AT_APPL)
          Args[i] = (ATerm) gsImplExprsPart((ATermAppl) Arg, PSubsts,
            PDataDecls);
        else //ATgetType(Arg) == AT_LIST
          Args[i] = (ATerm) gsImplExprsParts((ATermList) Arg, PSubsts,
            PDataDecls);
      }
      Part = ATmakeApplArray(Head, Args);
    }
  }
  return Part;
}

ATermList gsImplExprsParts(ATermList Parts, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  ATermList Result = ATmakeList0();
  while (!ATisEmpty(Parts))
  {
    Result = ATinsert(Result, (ATerm)
      gsImplExprsPart(ATAgetFirst(Parts), PSubsts, PDataDecls));
    Parts = ATgetNext(Parts);
  }
  return ATreverse(Result);
}

ATermList gsGetFreeVars(ATermAppl DataExpr)
{
  ATermList Result = ATmakeList0();
  gsGetFreeVars_Appl(DataExpr, ATmakeList0(), &Result);
  return ATreverse(Result);
}

void gsGetFreeVars_Appl(ATermAppl DataExpr, ATermList BoundVars,
  ATermList* PFreeVars)
{
  if (gsIsDataVarId(DataExpr)) {
    //DataExpr is a data variable; add it to *PFreeVars if it does not occur in
    //BoundVars or *PFreeVars
    if ((ATindexOf(BoundVars, (ATerm) DataExpr, 0) == -1) &&
        (ATindexOf(*PFreeVars, (ATerm) DataExpr, 0) == -1)) {
      *PFreeVars = ATinsert(*PFreeVars, (ATerm) DataExpr);
    }
  } else if (gsIsOpId(DataExpr) || gsIsNumber(DataExpr)) {
    //DataExpr is an operation identifier or a number; do nothing
  } else if (gsIsDataAppl(DataExpr) || gsIsBagEnumElt(DataExpr)) {
    //DataExpr is a data application or a bag enumeration element; get free
    //variables from the arguments
    gsGetFreeVars_Appl(ATAgetArgument(DataExpr, 0), BoundVars, PFreeVars);
    gsGetFreeVars_Appl(ATAgetArgument(DataExpr, 1), BoundVars, PFreeVars);
  } else if (gsIsDataApplProd(DataExpr)) {
    //DataExpr is a product data application; get free variables from the
    //arguments
    gsGetFreeVars_Appl(ATAgetArgument(DataExpr, 0), BoundVars, PFreeVars);
    gsGetFreeVars_List(ATLgetArgument(DataExpr, 1), BoundVars, PFreeVars);
  } else if (gsIsListEnum(DataExpr) || gsIsSetEnum(DataExpr) ||
      gsIsBagEnum(DataExpr)) {
    //DataExpr is an enumeration; get free variables from the elements
    gsGetFreeVars_List(ATLgetArgument(DataExpr, 1), BoundVars, PFreeVars);
  } else if (gsIsSetBagComp(DataExpr)) {
    //DataExpr is a set of bag comprehension; get free variables from the body
    //where BoundVars is extended with the variable declaration
    ATermAppl Var = ATAgetArgument(DataExpr, 0);
    if (ATindexOf(BoundVars, (ATerm) Var, 0) == -1) {
      BoundVars = ATinsert(BoundVars, (ATerm) Var);
    }
    gsGetFreeVars_Appl(ATAgetArgument(DataExpr, 1), BoundVars, PFreeVars);
  } else if (gsIsLambda(DataExpr) || gsIsForall(DataExpr) ||
      gsIsExists(DataExpr)) {
    //DataExpr is a lambda abstraction or a quantification; get free variables
    //from the body where BoundVars is extended with the variable declaration
    ATermList Vars = ATLgetArgument(DataExpr, 0);
    while (!ATisEmpty(Vars)) {
      ATermAppl Var = ATAgetFirst(Vars);
      if (ATindexOf(BoundVars, (ATerm) Var, 0) == -1) {
        BoundVars = ATinsert(BoundVars, (ATerm) Var);
      }
      Vars = ATgetNext(Vars);
    }
    gsGetFreeVars_Appl(ATAgetArgument(DataExpr, 1), BoundVars, PFreeVars);
  } else if (gsIsWhr(DataExpr)) {
    //DataExpr is a where clause; get free variables from the rhs's of the
    //where clause declarations and from the body where BoundVars is extended
    //with the lhs's of the where clause declarations
    ATermList WhrDecls = ATLgetArgument(DataExpr, 1);
    //get free variables from the rhs's of the where clause declarations
    while (!ATisEmpty(WhrDecls)) {
      gsGetFreeVars_Appl(ATAgetArgument(ATAgetFirst(WhrDecls), 1),
        BoundVars, PFreeVars);
      WhrDecls = ATgetNext(WhrDecls);
    }
    //get free variables from the body
    WhrDecls = ATLgetArgument(DataExpr, 1);
    while (!ATisEmpty(WhrDecls)) {
      ATermAppl WhrDecl = ATAgetFirst(WhrDecls);
      ATermAppl Var = gsMakeDataVarId(ATAgetArgument(WhrDecl, 0),
        gsGetSort(ATAgetArgument(WhrDecl, 1)));
      if (ATindexOf(BoundVars, (ATerm) Var, 0) == -1) {
        BoundVars = ATinsert(BoundVars, (ATerm) Var);
      }
      WhrDecls = ATgetNext(WhrDecls);
    }
    gsGetFreeVars_Appl(ATAgetArgument(DataExpr, 0), BoundVars, PFreeVars);
  } else {
    gsErrorMsg("%t is not a data expression or a bag enumeration element\n",\
      DataExpr);
  }
}

void gsGetFreeVars_List(ATermList DataExprs, ATermList BoundVars,
  ATermList* PFreeVars)
{
  while (!ATisEmpty(DataExprs))
  {
    gsGetFreeVars_Appl(ATAgetFirst(DataExprs), BoundVars, PFreeVars);
    DataExprs = ATgetNext(DataExprs);
  }
}

ATermAppl gsImplSortStruct(ATermAppl SortStruct, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  assert(gsIsSortStruct(SortStruct));
  //declare fresh sort identifier for SortStruct
  ATermAppl SortId = gsMakeFreshStructSortId((ATerm) PDataDecls->Sorts);
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
  //add substitution for this identifier
  ATermAppl Subst = gsMakeSubst(SortStruct, SortId);
  *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
  return SortId;
}

ATermAppl gsImplSetBagEnum(ATermList Elts, ATermAppl SortExpr)
{
  assert(ATgetLength(Elts) > 0);
  assert(gsIsSortSet(SortExpr) || gsIsSortBag(SortExpr));
  ATermAppl Result;
  //introduce a fresh variable
  ATermAppl Var =
    gsMakeDataVarId(gsFreshString2ATermAppl("x", (ATerm) Elts, true),
      ATAgetArgument(SortExpr, 0));
  //make body for the lambda abstraction
  Elts = ATreverse(Elts);
  if (gsIsSortSet(SortExpr)) {
    Result = gsMakeDataExprEq(Var, ATAgetFirst(Elts));
    Elts = ATgetNext(Elts);
  } else { //gsIsSortBag(SortExpr)
    Result = gsMakeDataExpr0();
  }
  while (!ATisEmpty(Elts))
  {
    ATermAppl Elt = ATAgetFirst(Elts);
    if (gsIsSortSet(SortExpr))
      Result = gsMakeDataExprOr(gsMakeDataExprEq(Var, Elt), Result);
    else //gsIsSortBag(SortExpr)
      Result = gsMakeDataExprIf(gsMakeDataExprEq(Var, ATAgetArgument(Elt, 0)),
        ATAgetArgument(Elt, 1), Result);
    Elts = ATgetNext(Elts);
  }
  //make lambda abstraction
  Result = gsMakeLambda(ATmakeList1((ATerm) Var), Result);
  //make set/bag enumeration
  if (gsIsSortSet(SortExpr))
    Result = gsMakeDataExprSetComp(Result, SortExpr);
  else //gsIsSortBag(SortExpr)
    Result = gsMakeDataExprBagComp(Result, SortExpr);
  //return Result
  return Result;
}

ATermAppl gsImplSortRefs(ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  //get sort declarations
  ATermAppl SortSpec = ATAgetArgument(Spec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  //split sort declarations in sort id's and sort references
  ATermList SortIds = NULL;
  ATermList SortRefs = NULL;
  gsSplitSortDecls(SortDecls, &SortIds, &SortRefs);
  //replace the sort declarations in Spec by the SortIds, the list of
  //identifiers
  SortSpec = ATsetArgument(SortSpec, (ATerm) SortIds, 0);  
  Spec = ATsetArgument(Spec, (ATerm) SortSpec, 0);
  //make list of substitutions from SortRefs, the list of sort references
  ATermList Substs = ATmakeList0();
  while (!ATisEmpty(SortRefs))
  {
    ATermAppl SortRef = ATAgetFirst(SortRefs);
    ATermAppl LHS = gsMakeSortId(ATAgetArgument(SortRef, 0));
    ATermAppl RHS = ATAgetArgument(SortRef, 1);
    //if RHS is the first occurrence of an implementation of a structured sort
    //at the rhs of a sort reference, add RHS := LHS; otherwise add LHS := RHS
    ATermAppl Subst;
    if (gsIsStructSortId(RHS) || gsIsListSortId(RHS) || gsIsSetSortId(RHS) ||
      gsIsBagSortId(RHS))
    {
      Subst = gsMakeSubst(RHS, LHS);
    } else {
      Subst = gsMakeSubst(LHS, RHS);
    }
    Substs = ATinsert(Substs, (ATerm) Subst);
    //perform substitution on the remaining elements of SortRefs
    SortRefs = ATgetNext(SortRefs);    
    SortRefs = gsSubstValues_List(ATmakeList1((ATerm) Subst), SortRefs, true);
  }
  //perform substitutions on Spec
  Spec = gsSubstValues_Appl(Substs, Spec, true);
  return Spec;
}

void gsSplitSortDecls(ATermList SortDecls, ATermList *PSortIds,
  ATermList *PSortRefs)
{
  ATermList SortIds = ATmakeList0();
  ATermList SortRefs = ATmakeList0();
  while (!ATisEmpty(SortDecls))
  {
    ATermAppl SortDecl = ATAgetFirst(SortDecls);
    if (gsIsSortRef(SortDecl)) {
      SortRefs = ATinsert(SortRefs, (ATerm) SortDecl);
    } else { //gsIsSortId(SortDecl)
      SortIds = ATinsert(SortIds, (ATerm) SortDecl);
    }
    SortDecls = ATgetNext(SortDecls);
  }
  *PSortIds = ATreverse(SortIds);  
  *PSortRefs = ATreverse(SortRefs);
}

ATermAppl gsMakeFreshStructSortId(ATerm Term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsStructPrefix, Term, false));
}
 
ATermAppl gsMakeFreshListSortId(ATerm Term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsListPrefix, Term, false));
}

ATermAppl gsMakeFreshSetSortId(ATerm Term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSetPrefix, Term, false));
}

ATermAppl gsMakeFreshBagSortId(ATerm Term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsBagPrefix, Term, false));
}

ATermAppl gsMakeFreshLambdaOpId(ATermAppl SortExpr, ATerm Term)
{
  return gsMakeOpId(gsFreshString2ATermAppl(gsLambdaPrefix, Term, false),
    SortExpr);
}

bool gsIsStructSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsStructPrefix,
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsStructPrefix)) == 0;
  } else {
    return false;
  }
}

bool gsIsListSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsListPrefix,
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsListPrefix)) == 0;
  } else {
    return false;
  }
}

bool gsIsSetSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSetPrefix,
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSetPrefix)) == 0;
  } else {
    return false;
  }
}

bool gsIsBagSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsBagPrefix,
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsBagPrefix)) == 0;
  } else {
    return false;
  }
}

bool gsIsLambdaOpId(ATermAppl DataExpr)
{
  if (gsIsOpId(DataExpr)) {
    return strncmp(
      gsLambdaPrefix,
      ATgetName(ATgetAFun(ATAgetArgument(DataExpr, 0))),
      strlen(gsLambdaPrefix)) == 0;
  } else {
    return false;
  }
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
  Spec = gsDeclareConsOps(Spec, ATmakeList2(
      (ATerm) gsMakeOpId1(),
      (ATerm) gsMakeOpIdCDub()
    ));
  //Declare operations for sort Pos
  ATermAppl sePos = gsMakeSortExprPos();
  Spec = gsDeclareOps(Spec, ATmakeList(15,
      (ATerm) gsMakeOpIdEq(sePos),
      (ATerm) gsMakeOpIdNeq(sePos),
      (ATerm) gsMakeOpIdIf(sePos),
      (ATerm) gsMakeOpIdLTE(sePos),
      (ATerm) gsMakeOpIdLT(sePos),
      (ATerm) gsMakeOpIdGTE(sePos),
      (ATerm) gsMakeOpIdGT(sePos),
      (ATerm) gsMakeOpIdMax(sePos),
      (ATerm) gsMakeOpIdMin(sePos),
      (ATerm) gsMakeOpIdSucc(sePos),
      (ATerm) gsMakeOpIdDub(sePos),
      (ATerm) gsMakeOpIdAdd(sePos),
      (ATerm) gsMakeOpIdAddC(),
      (ATerm) gsMakeOpIdMult(sePos),
      (ATerm) gsMakeOpIdMultIR()
    ));
  //Declare data equations for sort Pos
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExpr1();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), gsMakeSortExprPos());
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), gsMakeSortExprBool());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pqrl = ATmakeList3((ATerm) p, (ATerm) q, (ATerm) r);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) b, (ATerm) c, (ATerm) p, (ATerm) q);
  ATermList bpqrl = ATmakeList4((ATerm) b, (ATerm) p, (ATerm) q, (ATerm) r);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(42,
      //equality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(one, one), t),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprCDub(b, p)), f),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), one), f),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         f),
      //inequality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprNeq(p, q), gsMakeDataExprNot(gsMakeDataExprEq(p, q))),
      //conditional (Bool -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprIf(t, p, q), p),
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprIf(f, p, q), q),
      (ATerm) gsMakeDataEqn(bpl,nil, gsMakeDataExprIf(b, p, p), p),
      //less than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLTE(one, p), t),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), one), f),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprLT(p, q)),
      //less than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLT(p, one), f),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprCDub(b, p)), t),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprLT(p, q)),
      //greater than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGTE(p, q), gsMakeDataExprNot(gsMakeDataExprLT(q, p))),
      //greater than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGT(p, q), gsMakeDataExprNot(gsMakeDataExprLTE(q, p))),
      //maximum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMax(p, q),
         gsMakeDataExprIf(gsMakeDataExprGTE(p, q), q, p)),
      //minimum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMin(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), p, q)),
      //successor (Pos -> Pos)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprSucc(one), gsMakeDataExprCDub(f, one)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCDub(f, p)),
         gsMakeDataExprCDub(t, p)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCDub(t, p)),
         gsMakeDataExprCDub(f, gsMakeDataExprSucc(p))),
      //double (Pos -> Pos)
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDub(b, p), gsMakeDataExprCDub(b, p)),
      //addition (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdAdd(sePos),
         gsMakeDataAppl(gsMakeOpIdAddC(), gsMakeOpIdFalse())),
      //addition with carry (Bool -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(f, one, p), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(t, one, p),
         gsMakeDataExprSucc(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(f, p, one), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(t, p, one),
         gsMakeDataExprSucc(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(bcpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(c, p), gsMakeDataExprCDub(c, q)),
         gsMakeDataExprCDub(b, gsMakeDataExprAddC(c, p, q))),
      (ATerm) gsMakeDataEqn(bpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprCDub(gsMakeDataExprNot(b), gsMakeDataExprAddC(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprCDub(gsMakeDataExprNot(b), gsMakeDataExprAddC(b, p, q))),
      //multiplication (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql, nil, gsMakeDataExprMult(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q),
           gsMakeDataExprMultIR(f, one, p, q),
           gsMakeDataExprMultIR(f, one, q, p))),
      //multiplication with intermediate result
      //  (Bool -> Pos -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMultIR(f, p, one, q), q),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMultIR(t, p, one, q), gsMakeDataExprAddC(f, p, q)),
      (ATerm) gsMakeDataEqn(bpqrl,nil, 
         gsMakeDataExprMultIR(b, p, gsMakeDataExprCDub(f, q), r),
         gsMakeDataExprMultIR(b, p, q, gsMakeDataExprCDub(f, r))),
      (ATerm) gsMakeDataEqn(pqrl,nil, 
         gsMakeDataExprMultIR(f, p, gsMakeDataExprCDub(t, q), r),
         gsMakeDataExprMultIR(t, r, q, gsMakeDataExprCDub(f, r))),
      (ATerm) gsMakeDataEqn(pqrl,nil, 
         gsMakeDataExprMultIR(t, p, gsMakeDataExprCDub(t, q), r),
         gsMakeDataExprMultIR(t, gsMakeDataExprAddC(f, p, r), q,
           gsMakeDataExprCDub(f, r)))
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
  Spec = gsDeclareOps(Spec, ATmakeList(21,
      (ATerm) gsMakeOpIdEq(seNat),
      (ATerm) gsMakeOpIdNeq(seNat),
      (ATerm) gsMakeOpIdIf(seNat),
      (ATerm) gsMakeOpIdPos2Nat(),
      (ATerm) gsMakeOpIdNat2Pos(),
      (ATerm) gsMakeOpIdLTE(seNat),
      (ATerm) gsMakeOpIdLT(seNat),
      (ATerm) gsMakeOpIdGTE(seNat),
      (ATerm) gsMakeOpIdGT(seNat),
      (ATerm) gsMakeOpIdMax(seNat),
      (ATerm) gsMakeOpIdMin(seNat),
      (ATerm) gsMakeOpIdSucc(seNat),
      (ATerm) gsMakeOpIdPred(sePos),
      (ATerm) gsMakeOpIdDub(seNat),
      (ATerm) gsMakeOpIdAdd(seNat),
      (ATerm) gsMakeOpIdMult(seNat),
      (ATerm) gsMakeOpIdDiv(seNat),
      (ATerm) gsMakeOpIdMod(seNat),
      (ATerm) gsMakeOpIdExp(sePos),
      (ATerm) gsMakeOpIdExp(seNat),
      (ATerm) gsMakeOpIdEven()
    ));
  //Declare data equations for sort Nat
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExpr1();
  ATermAppl zero = gsMakeDataExpr0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(44,
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
      (ATerm) gsMakeDataEqn(el,nil, gsMakeOpIdPos2Nat(), gsMakeOpIdCNat()),
      //convert Nat to Pos (Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNat2Pos(gsMakeDataExprCNat(p)), p),
      //less than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLTE(zero, n), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLTE(p, q)),
      //less than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLT(n, zero), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(zero, gsMakeDataExprCNat(p)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLT(p, q)),
      //greater than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGTE(m, n), gsMakeDataExprNot(gsMakeDataExprLT(n, m))),
      //greater than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGT(m, n), gsMakeDataExprNot(gsMakeDataExprLTE(n, m))),
      //maximum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMax(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), n, m)),
      //minimum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMin(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), m, n)),
      //successor (Nat -> Pos)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprSucc(zero), one),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNat(p)), gsMakeDataExprSucc(p)),
      //predecessor (Pos -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprPred(one), zero),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCDub(t, p)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCDub(f, p)),
         gsMakeDataExprDub(t, gsMakeDataExprPred(p))),
      //double (Bool -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprDub(f, zero), zero),
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprDub(t, zero), gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDub(b, gsMakeDataExprCNat(p)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))),
      //addition (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(zero, n), n),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(n, zero), n),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprAddC(f, p, q))),
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
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprCDub(f, q))),
         gsMakeDataExprExp(gsMakeDataExprMultIR(f, one, p, p),
           gsMakeDataExprCNat(q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprCDub(t, q))),
         gsMakeDataExprMultIR(f, one, p,
           gsMakeDataExprExp(gsMakeDataExprMultIR(f, one, p, p),
             gsMakeDataExprCNat(q)))),
      //exponentiation (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprExp(n, zero),
        gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(zero, gsMakeDataExprCNat(p)), zero),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprExp(gsMakeDataExprCNat(p), n),
         gsMakeDataExprCNat(gsMakeDataExprExp(p, n))),
      //even (Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEven(zero), t),
      (ATerm) gsMakeDataEqn(pl, nil,
        gsMakeDataExprEven(gsMakeDataExprCNat(one)), f),
      (ATerm) gsMakeDataEqn(bpl, nil,
        gsMakeDataExprEven(gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))), b)
    ));
  return Spec;
}

ATermAppl gsImplementInt(ATermAppl Spec)
{
  //Declare sort Int
  Spec = gsDeclareSort(Spec, gsMakeSortIdInt());
  //Declare constructors for sort Int
  Spec = gsDeclareConsOps(Spec, ATmakeList2(
      (ATerm) gsMakeOpIdCInt(),
      (ATerm) gsMakeOpIdCNeg()
    ));
  //Declare operations for sort Int
  ATermAppl sePos = gsMakeSortExprPos();
  ATermAppl seNat = gsMakeSortExprNat();
  ATermAppl seInt = gsMakeSortExprInt();
  Spec = gsDeclareOps(Spec, ATmakeList(30,
      (ATerm) gsMakeOpIdEq(seInt),
      (ATerm) gsMakeOpIdNeq(seInt),
      (ATerm) gsMakeOpIdIf(seInt),
      (ATerm) gsMakeOpIdNat2Int(),
      (ATerm) gsMakeOpIdInt2Nat(),
      (ATerm) gsMakeOpIdPos2Int(),
      (ATerm) gsMakeOpIdInt2Pos(),
      (ATerm) gsMakeOpIdLTE(seInt),
      (ATerm) gsMakeOpIdLT(seInt),
      (ATerm) gsMakeOpIdGTE(seInt),
      (ATerm) gsMakeOpIdGT(seInt),
      (ATerm) gsMakeOpIdMax(seInt),
      (ATerm) gsMakeOpIdMin(seInt),
      (ATerm) gsMakeOpIdAbs(),
      (ATerm) gsMakeOpIdNeg(sePos),
      (ATerm) gsMakeOpIdNeg(seNat),
      (ATerm) gsMakeOpIdNeg(seInt),
      (ATerm) gsMakeOpIdSucc(seInt),
      (ATerm) gsMakeOpIdPred(seNat),
      (ATerm) gsMakeOpIdPred(seInt),
      (ATerm) gsMakeOpIdDub(seInt),
      (ATerm) gsMakeOpIdAdd(seInt),
      (ATerm) gsMakeOpIdSubt(sePos),
      (ATerm) gsMakeOpIdSubtB(),
      (ATerm) gsMakeOpIdSubt(seNat),
      (ATerm) gsMakeOpIdSubt(seInt),
      (ATerm) gsMakeOpIdMult(seInt),
      (ATerm) gsMakeOpIdDiv(seInt),
      (ATerm) gsMakeOpIdMod(seInt),
      (ATerm) gsMakeOpIdExp(seInt)
    ));
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExpr1();
  ATermAppl zero = gsMakeDataExpr0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) b, (ATerm) c, (ATerm) p, (ATerm) q);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bxl = ATmakeList2((ATerm) b, (ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  Spec = gsDeclareDataEqns(Spec, ATmakeList(66,
      //equality (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprEq(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprEq(p, q)),
      //inequality (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprNeq(x, y), gsMakeDataExprNot(gsMakeDataExprEq(x, y))),
      //conditional (Bool -> Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprIf(t, x, y), x),
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprIf(f, x, y), y),
      (ATerm) gsMakeDataEqn(bxl,nil, gsMakeDataExprIf(b, x, x), x),
      //convert Nat to Int (Nat -> Int)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdNat2Int(), gsMakeOpIdCInt()),
      //convert Int to Nat (Int -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprInt2Nat(gsMakeDataExprCInt(n)), n),
      //convert Pos to Int (Pos -> Int)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprPos2Int(p), gsMakeDataExprCInt(gsMakeDataExprCNat(p))),
      //convert Int to Pos (Int -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprInt2Pos(gsMakeDataExprCInt(gsMakeDataExprCNat(p))), p),
      //less than or equal (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprLTE(q, p)),
      //less than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprLT(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprLT(q, p)),
      //greater than or equal (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGTE(x, y), gsMakeDataExprNot(gsMakeDataExprLT(y, x))),
      //greater than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGT(x, y), gsMakeDataExprNot(gsMakeDataExprLTE(y, x))),
      //maximum (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprMax(x, y),
         gsMakeDataExprIf(gsMakeDataExprLTE(x, y), y, x)),
      //minimum (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprMin(x, y),
         gsMakeDataExprIf(gsMakeDataExprLTE(x, y), x, y)),
      //absolute value (Int -> Nat)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCInt(n)), n),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNat(p)),
      //negation (Pos -> Int)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(p), gsMakeDataExprCNeg(p)),
      //negation (Nat -> Int)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprNeg(zero), gsMakeDataExprCInt(zero)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCNat(p)),
         gsMakeDataExprCNeg(p)),
      //negation (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCInt(n)), gsMakeDataExprNeg(n)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(p))),
      //successor (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprSucc(n)))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNeg(p)),
           gsMakeDataExprNeg(gsMakeDataExprPred(p))),      
      //predecessor (Nat -> Int)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprPred(zero), gsMakeDataExprCNeg(one)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCNat(p)),
         gsMakeDataExprCInt(gsMakeDataExprPred(p))),
      //predecessor (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPred(gsMakeDataExprCInt(n)), gsMakeDataExprPred(n)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNeg(gsMakeDataExprSucc(p))),
      //double (Bool -> Int -> Int)
      (ATerm) gsMakeDataEqn(bnl,nil,
         gsMakeDataExprDub(b, gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprDub(b, n))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprDub(f, gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNeg(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprDub(t, gsMakeDataExprCNeg(p)),
         gsMakeDataExprNeg(gsMakeDataExprPred(gsMakeDataExprCDub(f, p)))),
      //addition (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(mnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprAdd(m, n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)),
         gsMakeDataExprSubt(n, gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)),
         gsMakeDataExprSubt(n, gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprAdd(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprCNeg(gsMakeDataExprAddC(f, p, q))),
      //subtraction (Pos -> Pos -> Int)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdSubt(sePos),
         gsMakeDataAppl(gsMakeOpIdSubtB(), f)),
      //subtraction with borrow (Bool -> Pos -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprSubtB(f, one, p),
         gsMakeDataExprNeg(gsMakeDataExprPred(p))),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprSubtB(t, one, p),
         gsMakeDataExprCNeg(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprSubtB(f, p, one),
         gsMakeDataExprCInt(gsMakeDataExprPred(p))),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprSubtB(t, p, one),
         gsMakeDataExprPred(gsMakeDataExprPred(p))),
      (ATerm) gsMakeDataEqn(bcpql, nil,
         gsMakeDataExprSubtB(b, gsMakeDataExprCDub(c, p),
           gsMakeDataExprCDub(c, q)),
         gsMakeDataExprDub(b, gsMakeDataExprSubtB(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprSubtB(b, gsMakeDataExprCDub(f, p),
           gsMakeDataExprCDub(t, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b), gsMakeDataExprSubtB(t, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprSubtB(b, gsMakeDataExprCDub(t, p),
           gsMakeDataExprCDub(t, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b), gsMakeDataExprSubtB(f, p, q))),
      //subtraction (Nat -> Nat -> Int)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprSubt(zero, n), gsMakeDataExprNeg(n)),
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprSubt(n, zero), gsMakeDataExprCInt(n)),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprSubt(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprSubtB(f, p, q)),
      //subtraction (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprSubt(x, y), gsMakeDataExprAdd(x, gsMakeDataExprNeg(y))),
      //multiplication (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(mnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprMult(m, n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)),
         gsMakeDataExprNeg(gsMakeDataExprMult(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)),
         gsMakeDataExprNeg(gsMakeDataExprMult(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMult(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprCNeg(gsMakeDataExprMult(p, q))),
      //quotient after division (Int -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprDiv(gsMakeDataExprCInt(n), p),
         gsMakeDataExprDiv(n, p)),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprDiv(gsMakeDataExprCNeg(p), q),
         gsMakeDataExprCNeg(gsMakeDataExprSucc(
           gsMakeDataExprDiv(gsMakeDataExprPred(p), q)))),
      //remainder after division (Int -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMod(gsMakeDataExprCInt(n), p),
         gsMakeDataExprMod(n, p)),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMod(gsMakeDataExprCNeg(p), q),
         gsMakeDataExprInt2Nat(gsMakeDataExprSubt(q, gsMakeDataExprSucc(
           gsMakeDataExprMod(gsMakeDataExprPred(p), q))))),
      //exponentiation (Int -> Nat -> Int)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprExp(gsMakeDataExprCInt(m), n),
         gsMakeDataExprCInt(gsMakeDataExprExp(m, n))),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprIf(gsMakeDataExprEven(n),
           gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p, n))),
           gsMakeDataExprCNeg(gsMakeDataExprExp(p, n))))
    ));
  return Spec;
}

ATermAppl gsImplementData(ATermAppl Spec)
{
  Spec = gsImplExprs(Spec);
  Spec = gsImplSortRefs(Spec);
  //Spec = gsImplementInt(Spec);
  //Spec = gsImplementNat(Spec);
  //Spec = gsImplementPos(Spec);
  //Spec = gsImplementBool(Spec);
  gsVerboseMsg("data implementation is not yet fully implemented\n");
  return Spec;
}
