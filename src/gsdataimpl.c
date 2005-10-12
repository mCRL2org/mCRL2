#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "gsdataimpl.h"
#include "gsfunc.h"
#include "gslowlevel.h"
#include "libprint_c.h"

//local declarations

static ATermAppl gsFoldSortRefs(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     type checking
//Ret: Spec in which all sort references are maximally folded, i.e.:
//     - sort references to SortId's and SortArrow's are removed from the
//       rest of Spec (including the other sort references) by means of
//       forward substitition
//     - other sort references are removed from the rest of Spec by means of
//       backward substitution
//     - self references are removed, i.e. sort references of the form A = A

static ATermList gsFoldSortRefsInSortRefs(ATermList SortRefs);
//Pre: SortRefs is a list of sort references
//Ret: SortRefs in which all sort references are maximally folded

static ATermAppl gsImplExprs(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     type checking in which sort references are maximally folded
//Ret: Spec in which all expressions are implemented

static ATermAppl gsImplSortRefs(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     data implementation, with the exception that sort references may occur
//Ret: Spec in which all sort references are implemented, i.e.:
//     - all sort references are removed from Spec
//     - if a sort reference is of the form SortRef(n, e), where e is the
//       implementation of a type constructor and SortRef(n, e) is the first
//       sort reference with e as a rhs, e is replaced by n in Spec;
//       otherwise, n is replaced by e in Spec

static ATermAppl gsImplFunctionSorts(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     data implementation
//Ret: Spec in which an implementation is added for each function sort

typedef struct {
  ATermList Sorts;
  ATermList ConsOps;
  ATermList Ops;
  ATermList DataEqns;
} TDataDecls;
//The type TDataDecls represents data declarations, i.e. sort, constructor,
//operation and data equation declarations

static bool gsDataDeclsIsInitialised(TDataDecls DataDecls);
//Ret: indicates whether the elements of DataDecls are initialised

static ATermAppl gsAddDataDecls(ATermAppl Spec, TDataDecls DataDecls);
//Pre: Spec is a specification that adheres to the internal syntax of an
//     arbitary phase
//Ret: Spec in which the data declarations from DataDecls are added

static ATermAppl gsImplExprsPart(ATermAppl Part, ATermList *PSubsts,
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

static ATermList gsImplExprsParts(ATermList Parts, ATermList *PSubsts,
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

static void gsImplFunctionSort(ATermAppl SortArrow, TDataDecls *PDataDecls);
//Pre: SortArrow is an arrow sort that adheres to the internal syntax after
//     data implementation
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of Part
//Ret: Spec in which an implementation is added for each function sort
//     - each substituted element is implemented, where the new data
//       declarations are stored in *PDataDecls

static ATermAppl gsImplSetBagEnum(ATermList Elts, ATermAppl SortExpr);
//Pre: Elts is a list containing 1 or more data expressions, all of the same
//     sort
//     SortExpr is a set or a bag sort
//Ret: Implementation of an enumeration of elements from Elts of sort SortExpr

static ATermList gsGetFreeVars(ATermAppl DataExpr);
//Pre: DataExpr is a data expression that adheres to the internal syntax after
//     type checking
//Ret: The free variables in DataExpr

static void gsGetFreeVars_Appl(ATermAppl DataExpr, ATermList BoundVars,
  ATermList* PFreeVars);
//Pre: DataExpr is a data expression or a bag enumeration element that adheres
//     to the internal format after type checking
//     BoundVars and *PFreeVars are lists of data variables, and represent the
//     bound/free variables of the context of DataExpr
//Post:*PFreeVars is extended with the free variables in DataExpr that did not
//     already occur in *PFreeVars or BoundVars

static void gsGetFreeVars_List(ATermList DataExprs, ATermList BoundVars,
  ATermList* PFreeVars);
//Pre: DataExprs is a list of data expressions or bag enumeration elements that
//     adhere to the internal format after type checking
//     BoundVars and *PFreeVars are lists of data variables, and represent the
//     bound/free variables of the context of DataExprs
//Post:*PFreeVars is extended with the free variables in DataExprs that did not
//     already occur in *PFreeVars or BoundVars

static ATermList gsGetFunctionSorts(ATermAppl Spec);
//Pre: Spec is a specification
//Ret: a list of all function sorts occurring in Spec, where each element is
//     unique

static void gsGetFunctionSorts_Appl(ATermAppl Part, ATermList *PFuncSorts);
//Pre: Part is a part of a specification
//     *PFuncSorts represents the function sorts that are already found
//Post:FuncSorts is extended with the function sorts in Part that did not
//     already occur in *PFuncSorts

static void gsGetFunctionSorts_List(ATermList Parts, ATermList *PFuncSorts);
//Pre: Parts is a list of parts of a specification
//     *PFuncSorts represents the function sorts that are already found
//Post:FuncSorts is extended with the function sorts in Parts that did not
//     already occur in *PFuncSorts

static ATermAppl gsApplyOpIdToVars(ATermAppl OpId, ATermList *PArgs,
                                   ATermList *PVars, ATerm Context);
//Pre: OpId is an operation identifier
//     PArgs points to a list
//     PVars points to a list, possibly containing DataVarIds
//     Context is some term
//Post:*PArgs contains a subset of *PVars
//     *PVars is extended with newly introduced variables (which do not occur
//     in Context
//Ret: OpId applied to as much variables as possible, which are listed (in
//     order) in *PArgs

static ATermList gsMergeList(ATermList L, ATermList M);
//Pre: L and M are two lists without duplicates
//Ret: a list with all elements of L and M precisely once

static ATermList gsSubstractList(ATermList L, ATermList M);
//Pre: L and M are two lists
//Ret: a copy of L without elements that occur in M

static ATermAppl gsImplSortStruct(ATermAppl SortStruct, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: SortStruct is a structured sort
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of SortStruct
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of SortStruct
//Post:an implementation of SortStruct is added to *PDataDecls and new induced
//     substitutions are added *PSubsts
//Ret: a sort identifier which is the implementation of SortStruct

static ATermAppl gsImplSortList(ATermAppl SortList, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: SortList is a structured sort
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of SortList
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of SortList
//Post:an implementation of SortList is added to *PDataDecls and new induced
//     substitutions are added *PSubsts
//Ret: a sort identifier which is the implementation of SortList

static ATermAppl gsImplSortSet(ATermAppl SortSet, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: SortSet is a set sort
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of SortSet
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of SortSet
//Post:an implementation of SortSet is added to *PDataDecls and new induced
//     substitutions are added *PSubsts
//Ret: a sort identifier which is the implementation of SortSet

static ATermAppl gsImplSortBag(ATermAppl SortBag, ATermList *PSubsts,
  TDataDecls *PDataDecls);
//Pre: SortBag is a bag sort
//     PSubsts is a pointer to a list of substitutions induced by the context
//     of SortBag
//     PDataDecls represents a pointer to new data declarations, induced by
//     the context of SortBag
//Post:an implementation of SortBag is added to *PDataDecls and new induced
//     substitutions are added *PSubsts
//Ret: a sort identifier which is the implementation of SortBag

static ATermAppl gsImplSortArrowProd(ATermAppl SortArrowProd);
//Pre: SortArrowProd represents a function sort that adheres to the syntax
//     after data implementation
//Ret: an implementation of SortArrowProd
  
static void gsImplSortBool(TDataDecls *PDataDecls);
//Pre: PDataDecls represents a pointer to new data declarations
//Post:an implementation of sort Bool is added to *PDataDecls

static void gsImplSortPos(TDataDecls *PDataDecls);
//Pre: PDataDecls represents a pointer to new data declarations
//Post:an implementation of sort Pos is added to *PDataDecls

static void gsImplSortNat(TDataDecls *PDataDecls);
//Pre: PDataDecls represents a pointer to new data declarations
//Post:an implementation of sort Nat is added to *PDataDecls

static void gsImplSortInt(TDataDecls *PDataDecls);
//Pre: PDataDecls represents a pointer to new data declarations
//Post:an implementation of sort Int is added to *PDataDecls

static void gsImplSortReal(TDataDecls *PDataDecls);
//Pre: PDataDecls represents a pointer to new data declarations
//Post:an implementation of sort Real is added to *PDataDecls

static void gsSplitSortDecls(ATermList SortDecls, ATermList *PSortIds,
  ATermList *PSortRefs);
//Pre: SortDecls is a list of SortId's and SortRef's
//Post:*PSortIds and *PSortRefs contain the SortId's and SortRef's from
//     SortDecls, in the same order

static const char *gsStructPrefix = "Struct@";
static const char *gsListPrefix   = "List@";
static const char *gsSetPrefix    = "Set@";
static const char *gsBagPrefix    = "Bag@";
static const char *gsLambdaPrefix = "lambda@";

static ATermAppl gsMakeFreshStructSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: sort identifier for the implementation of a structured sort with prefix
//     gsStructPrefix, that does not occur in Term

static ATermAppl gsMakeFreshListSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: fresh sort identifier for the implementation of a list sort with prefix
//     gsListPrefix, that does not occur in Term

static ATermAppl gsMakeFreshSetSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: fresh sort identifier for the implementation of a set sort with prefix
//     gsSetPrefix, that does not occur in Term

static ATermAppl gsMakeFreshBagSortId(ATerm Term);
//Pre: Term is not NULL
//Ret: fresh sort identifier for the implementation of a bag sort with prefix
//     gsBagPrefix, that does not occur in Term

static ATermAppl gsMakeFreshLambdaOpId(ATermAppl SortExpr, ATerm Term);
//Pre: SortExpr is a sort expression
//     Term is not NULL
//Ret: operation identifier OpId(n, s) for the implementation of a lambda
//     abstraction, where s is SortExpr and n is a name with prefix
//     gsLambdaPrefix, that does not occur in Term

static bool gsIsStructSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a structured sort

static bool gsIsListSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a list sort

static bool gsIsSetSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a set sort

static bool gsIsBagSortId(ATermAppl SortExpr);
//Pre: SortExpr is sort expression
//Ret: SortExpr is the implementation of a bag sort

//static bool gsIsLambdaOpId(ATermAppl DataExpr);
////Pre: DataExpr is a data expression
////Ret: DataExpr is an operation identifier for the implementation of a lambda
////     abstraction

//implementation

ATermAppl gsImplExprs(ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  //implement system sort and data expressions occurring in Spec
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
  //add implementation of sort Pos and Bool
  gsImplSortPos(&DataDecls);
  gsImplSortBool(&DataDecls);
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
    Part = gsImplSortArrowProd(Part);
  } else if (gsIsSortStruct(Part)) {
    //Part is a structured sort; replace by a new sort and add data
    //declarations for this sort
    Part = gsImplSortStruct(Part, PSubsts, PDataDecls);
  } else if (gsIsSortList(Part)) {
    //Part is a list sort; replace by a new sort and add data declarations for
    //this sort
    Part = gsImplSortList(Part, PSubsts, PDataDecls);
  } else if (gsIsSortSet(Part)) {
    //Part is a set sort; replace by a new sort and add data declarations for
    //this sort
    Part = gsImplSortSet(Part, PSubsts, PDataDecls);
  } else if (gsIsSortBag(Part)) {
    //Part is a bag sort; replace by a new sort and add data declarations for
    //this sort
    Part = gsImplSortBag(Part, PSubsts, PDataDecls);
  } else if (gsIsSortId(Part)) {
    //Part is a sort identifier; add data declarations for this sort, if needed
    if (ATisEqual(Part,gsMakeSortIdNat()))
    {
      //add implementation of sort Nat, if necessary
      if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
        gsImplSortNat(PDataDecls);
      }
    } else if (ATisEqual(Part, gsMakeSortIdInt())) {
      //add implementation of sort Int, if necessary
      if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
        gsImplSortInt(PDataDecls);
      }
    } else if (ATisEqual(Part, gsMakeSortIdReal())) {
      //add implementation of sort Int, if necessary
      if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdReal(), 0) == -1) {
        gsImplSortReal(PDataDecls);
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
      gsWarningMsg("%P can not be implemented because its sort differs from "
        "Pos, Nat or Int\n", Part);
  } else if (gsIsListEnum(Part)) {
    //Part is a list enumeration; replace by its internal representation
    ATermList Elts = ATLgetArgument(Part, 0);
    ATermAppl Sort = ATAgetArgument(Part, 1);
    if (ATgetLength(Elts) == 0) {
      //enumeration consists of 0 elements
      gsWarningMsg(
        "%P can not be implemented because it has 0 elements\n", Part);
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
        "%P can not be implemented because it has 0 elements\n", Part);
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
      gsWarningMsg("%P can not be implemented because the body is of sort %P "
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
    *PSubsts = gsAddSubstToSubsts(gsMakeSubst_Appl(Part, NewPart), *PSubsts);
    Part = NewPart;
    Recursive = false;
  } else if (gsIsWhr(Part)) {
    //Part is a where clause; replace by its corresponding lambda expression
    ATermAppl Body = ATAgetArgument(Part, 0);
    ATermList WhrDecls = ATLgetArgument(Part, 1);
    if (ATgetLength(WhrDecls) == 0) {
      //where clause consists of 0 where clause declarations
      gsWarningMsg("%P can not be implemented because it has 0 where clause "
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
      DECL_A(Args,ATerm,NrArgs);
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
      FREE_A(Args);
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
  ATermAppl Elt = ATAgetFirst(Elts);
  if (gsIsSortSet(SortExpr))
    Result = gsMakeDataExprEq(Var, Elt);
  else //gsIsSortBag(SortExpr)
    Result = gsMakeDataExprIf(gsMakeDataExprEq(Var, ATAgetArgument(Elt, 0)),
      ATAgetArgument(Elt, 1), gsMakeDataExpr0());
  Elts = ATgetNext(Elts);
  while (!ATisEmpty(Elts))
  {
    Elt = ATAgetFirst(Elts);
    if (gsIsSortSet(SortExpr))
      Result = gsMakeDataExprOr(gsMakeDataExprEq(Var, Elt), Result);
    else //gsIsSortBag(SortExpr)
      Result = gsMakeDataExprAdd(
        gsMakeDataExprIf(gsMakeDataExprEq(Var, ATAgetArgument(Elt, 0)),
        ATAgetArgument(Elt, 1), gsMakeDataExpr0()), Result);
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
    gsGetFreeVars_List(ATLgetArgument(DataExpr, 0), BoundVars, PFreeVars);
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
    gsErrorMsg("%P is not a data expression or a bag enumeration element\n",\
      DataExpr);
  }
}

void gsGetFreeVars_List(ATermList DataExprs, ATermList BoundVars,
  ATermList *PFreeVars)
{
  while (!ATisEmpty(DataExprs))
  {
    gsGetFreeVars_Appl(ATAgetFirst(DataExprs), BoundVars, PFreeVars);
    DataExprs = ATgetNext(DataExprs);
  }
}
 
ATermList gsGetFunctionSorts(ATermAppl Spec)
{
  ATermList FuncSorts = ATmakeList0();
  gsGetFunctionSorts_Appl(Spec, &FuncSorts);
  return FuncSorts;
}

void gsGetFunctionSorts_Appl(ATermAppl Part, ATermList *PFuncSorts)
{
  if (gsIsSortArrow(Part) || gsIsSortArrowProd(Part)) {
    if (ATindexOf(*PFuncSorts, (ATerm) Part, 0) == -1) {
      *PFuncSorts = ATinsert(*PFuncSorts, (ATerm) Part);
    }    
  }
  int NrArgs = ATgetArity(ATgetAFun(Part));      
  for (int i = 0; i < NrArgs; i++) {
    ATerm Arg = ATgetArgument(Part, i);
    if (ATgetType(Arg) == AT_APPL)
      gsGetFunctionSorts_Appl((ATermAppl) Arg, PFuncSorts);
    else //ATgetType(Arg) == AT_LIST
      gsGetFunctionSorts_List((ATermList) Arg, PFuncSorts);
  }
}
 
void gsGetFunctionSorts_List(ATermList Parts, ATermList *PFuncSorts)
{
  while (!ATisEmpty(Parts))
  {
    gsGetFunctionSorts_Appl(ATAgetFirst(Parts), PFuncSorts);
    Parts = ATgetNext(Parts);
  }
}
 
ATermAppl gsApplyOpIdToVars(ATermAppl OpId, ATermList *PArgs,
  ATermList *PVars, ATerm Context)
{
  ATermAppl t = OpId;
  ATermAppl sort = ATAgetArgument(t,1);
  *PArgs = ATmakeList0();
  ATermList tmpvars = *PVars; // We only use variables once in a term
  while ( gsIsSortArrow(sort) )
  {
    // Find a variabele with the right sort...
    ATermAppl v = NULL;
    for (ATermList n=tmpvars; !ATisEmpty(n); n=ATgetNext(n))
    {
      if ( ATisEqual(ATgetArgument(ATAgetFirst(n),1),ATgetArgument(sort,0)) )
      {
        v = ATAgetFirst(n);
        tmpvars = ATremoveElement(tmpvars,(ATerm) v);
        break;
      }
    }
    // ...or create a new one
    if ( v == NULL )
    {
      v = gsMakeDataVarId(gsFreshString2ATermAppl("v",
            (ATerm) ATmakeList2((ATerm) (*PVars),Context),false),
                      ATAgetArgument(sort,0));
      // Add it to *PVars
      *PVars = ATinsert(*PVars,(ATerm) v);
    }
    // Apply t to v and add v to *PArgs
    t = gsMakeDataAppl(t,v);
    *PArgs = ATinsert(*PArgs,(ATerm) v);
    // Next
    sort = ATAgetArgument(sort,1);
  }
  *PArgs = ATreverse(*PArgs);
  *PVars = ATreverse(*PVars);
  return t;
}

ATermList gsMergeList(ATermList L, ATermList M)
{
  for (; !ATisEmpty(M); M=ATgetNext(M))
  {
    if ( ATindexOf(L,ATgetFirst(M),0) == -1 )
    {
      ATinsert(L,ATgetFirst(M));
    }
  }

  return L;
}

ATermList gsSubstractList(ATermList L, ATermList M)
{
  for (; !ATisEmpty(M); M=ATgetNext(M))
  {
    L = ATremoveAll(L,ATgetFirst(M));
  }

  return L;
}

ATermAppl gsImplSortArrowProd(ATermAppl SortArrowProd)
{
  ATermList l = ATreverse(ATLgetArgument(SortArrowProd, 0));
  SortArrowProd = ATAgetArgument(SortArrowProd, 1);
  while (!ATisEmpty(l))
  {
    SortArrowProd = gsMakeSortArrow(ATAgetFirst(l), SortArrowProd);
    l = ATgetNext(l);
  }
  return SortArrowProd;
}

ATermAppl gsImplSortStruct(ATermAppl SortStruct, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  assert(gsIsSortStruct(SortStruct));
  //declare fresh sort identifier for SortStruct
  ATermAppl SortId = gsMakeFreshStructSortId((ATerm) PDataDecls->Sorts);
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
  //add substitution for this identifier
  ATermAppl Subst = gsMakeSubst_Appl(SortStruct, SortId);
  *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
  //store constructor, projection and recogniser operations for this identifier
  ATermList ConsOps = ATmakeList0();
  ATermList ProjOps = ATmakeList0();
  ATermList Projs = ATmakeList0();
  ATermList RecOps = ATmakeList0();
  ATermList Recs = ATmakeList0();
  ATermList StructConss = ATLgetArgument(SortStruct, 0);
  while (!ATisEmpty(StructConss))
  {
    ATermAppl StructCons = ATAgetFirst(StructConss);
    ATermAppl ConsName = ATAgetArgument(StructCons, 0);
    ATermList StructProjs = ATLgetArgument(StructCons, 1);
    ATermAppl RecName = ATAgetArgument(StructCons, 2);
    ATermList StructConsSorts = ATmakeList0();
    //store projection operations in ProjOps and store the implementations of
    //the sorts in StructConsSorts
    int i = 0;
    while (!ATisEmpty(StructProjs))
    {
      ATermAppl StructProj = ATAgetFirst(StructProjs);
      ATermAppl ProjName = ATAgetArgument(StructProj, 0);
      ATermAppl ProjSort = gsImplExprsPart(ATAgetArgument(StructProj, 1),
        PSubsts, PDataDecls);
      StructConsSorts = ATinsert(StructConsSorts, (ATerm) ProjSort);
      //store projection operation in ProjOps and Projs
      if (!gsIsNil(ProjName)) {
        ProjOps = ATinsert(ProjOps, (ATerm)
          gsMakeOpId(ProjName, gsMakeSortArrow(SortId, ProjSort)));
        Projs = ATinsert(Projs, (ATerm) ATmakeList2(
          ATgetFirst(ProjOps),(ATerm) ATmakeInt(i)));
      }
      StructProjs = ATgetNext(StructProjs);
      i++;
    }
    StructConsSorts = ATreverse(StructConsSorts);
    //store constructor operation in ConsOps
    ConsOps = ATinsert(ConsOps, (ATerm)
      gsMakeOpId(ConsName, gsMakeSortArrowList(StructConsSorts, SortId)));
    //store recogniser in RecOps and Recs
    if (!gsIsNil(RecName)) {
      RecOps = ATinsert(RecOps, (ATerm)
        gsMakeOpId(RecName, gsMakeSortArrow(SortId, gsMakeSortExprBool())));
      Recs = ATinsert(Recs,
        (ATerm) ATmakeList2(ATgetFirst(RecOps), ATgetFirst(ConsOps)));
    }
    //add constructor to Projs
    ATermList tmpl = ATmakeList0();
    for (; !ATisEmpty(Projs); Projs=ATgetNext(Projs))
    {
      tmpl = ATinsert(tmpl, (ATerm) ATappend(ATLgetFirst(Projs),ATgetFirst(ConsOps)));
    }
    Projs = ATreverse(tmpl);
    StructConss = ATgetNext(StructConss);
  }
  //add declarations for the constructo, projection and recogniser operations
  PDataDecls->ConsOps = ATconcat(ATreverse(ConsOps), PDataDecls->ConsOps);
  PDataDecls->Ops = ATconcat(ATconcat(ATreverse(ProjOps), ATreverse(RecOps)),
    PDataDecls->Ops);
  //Declare standard structured sorts operations
  PDataDecls->Ops = ATconcat(ATmakeList(3,
      (ATerm) gsMakeOpIdEq(SortId),
      (ATerm) gsMakeOpIdNeq(SortId),
      (ATerm) gsMakeOpIdIf(SortId)
    ), PDataDecls->Ops);
  //Declare data equations for structured sort
  ATermList OpEqns = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  // XXX more intelligent variable names would be nice
  ATermAppl s1 = gsMakeDataVarId(gsString2ATermAppl("s"), SortId);
  ATermAppl s2 = gsMakeDataVarId(gsString2ATermAppl("t"), SortId);
  ATermList ssl = ATmakeList2((ATerm) s1, (ATerm) s2);
  ATermList bssl = ATmakeList3((ATerm) b, (ATerm) s1, (ATerm) s2);
  ATermList vars = bssl;
  ATermList rhsv;
  ATermList lhsv;
  ATermList IdCtx = ATconcat(PDataDecls->Sorts,
                      ATconcat(PDataDecls->Ops,PDataDecls->ConsOps));
  //store equations for projections in OpEqns
  for (; !ATisEmpty(Projs); Projs=ATgetNext(Projs))
  {
    ATermList l = ATLgetFirst(Projs);
    // Name of constructor
    ATermAppl s = ATAgetFirst(ATgetNext(ATgetNext(l)));
    // Number of projected argument
    int i = ATgetInt((ATermInt) ATgetFirst(ATgetNext(l)));
    // Start with the constructor operation
    ATermAppl t = s;
    // Apply constructor t to (fresh) variables and store its
    // arguments in lhsv
    t = gsApplyOpIdToVars(t,&lhsv,&vars,(ATerm) IdCtx);
    // Apply projection function to t
    t = gsMakeDataAppl(ATAgetFirst(l),t);
    // Add equation
    OpEqns = ATinsert(OpEqns,
      (ATerm) gsMakeDataEqn(lhsv, nil, t, ATAelementAt(lhsv,i)));
  }
  //store equations for recognition in OpEqns
  for (; !ATisEmpty(Recs); Recs=ATgetNext(Recs))
  {
    ATermList l = ATLgetFirst(Recs);
    // Name of constructor
    ATermAppl s = ATAgetFirst(ATgetNext(l));
    ATermAppl t;
    // Add equation for every constructor
    for (ATermList m=ConsOps; !ATisEmpty(m); m=ATgetNext(m))
    {
      t = ATAgetFirst(m);
      // Apply constructor t to (fresh) variables and store its
      // arguments in lhsv
      t = gsApplyOpIdToVars(t,&lhsv,&vars,(ATerm) IdCtx);
      // Apply recognition function to t
      t = gsMakeDataAppl(ATAgetFirst(l),t);
      // Add right equation to OpEqns
      if ( ATisEqual(ATAgetFirst(m),s) )
      {
        OpEqns = ATinsert(OpEqns, (ATerm) gsMakeDataEqn(lhsv, nil, t, gsMakeDataExprTrue()));
      } else {
        OpEqns = ATinsert(OpEqns, (ATerm) gsMakeDataEqn(lhsv, nil, t, gsMakeDataExprFalse()));
      }
    }
  }
  //store equations for equalities in OpEqns
  //one equation for every pair of constructors
  OpEqns = ATinsert(OpEqns,
    (ATerm) gsMakeDataEqn(ATmakeList1((ATerm) s1), nil, gsMakeDataExprEq(s1, s1), t));
  for (ATermList l=ConsOps; !ATisEmpty(l); l=ATgetNext(l))
  {
    for (ATermList m=ConsOps; !ATisEmpty(m); m=ATgetNext(m))
    {
      ATermAppl t,u,r;
      ATermList vs,tmpvars;
      // Save vars list
      // Apply constructor in l to (fresh) variables and store its
      // arguments in lhsv
      t = gsApplyOpIdToVars(ATAgetFirst(l),&lhsv,&vars,(ATerm) IdCtx);
      // Apply constructor in m to (fresh) variables and store its
      // arguments in rhsv (making sure we don't use the vars that occur in t)
      tmpvars = gsSubstractList(vars,lhsv);
      u = gsApplyOpIdToVars(ATAgetFirst(m),&rhsv,&tmpvars,(ATerm) ATconcat(lhsv,IdCtx));
      // Update vars
      vars = gsMergeList(vars,rhsv);
      // Combine variable lists of lhs and rhs
      vs = ATconcat(lhsv,rhsv);
      // Create right result
      if ( ATisEqual(ATgetFirst(l),ATgetFirst(m)) )
      {
        // Constructors are the same, so match all variables
        r = NULL;
        for (; !ATisEmpty(lhsv); lhsv=ATgetNext(lhsv),rhsv=ATgetNext(rhsv))
        {
          if ( r == NULL )
          {
            r = gsMakeDataExprEq(ATAgetFirst(lhsv),ATAgetFirst(rhsv));
          } else {
            r = gsMakeDataExprAnd(r,gsMakeDataExprEq(ATAgetFirst(lhsv),ATAgetFirst(rhsv)));
          }
        }
        if ( r == NULL )
        {
          r = gsMakeDataExprTrue();
        }
      } else {
        // Different constructor, so not equal
        r = gsMakeDataExprFalse();
      }
      // Add equation to OpIds
      OpEqns = ATinsert(OpEqns, (ATerm) gsMakeDataEqn(vs,nil,gsMakeDataExprEq(t,u),r));
    }
  }
  //store equation for inequality in OpEqns
  OpEqns = ATinsert(OpEqns, (ATerm) gsMakeDataEqn(ssl, nil,
    gsMakeDataExprNeq(s1,s2), gsMakeDataExprNot(gsMakeDataExprEq(s1,s2))));
  //store equations for 'if' in OpEqns
  OpEqns = ATconcat(ATmakeList(3,
      (ATerm) gsMakeDataEqn(ssl, nil, gsMakeDataExprIf(t,s1,s2),s1),
      (ATerm) gsMakeDataEqn(ssl, nil, gsMakeDataExprIf(f,s1,s2),s2),
      (ATerm) gsMakeDataEqn(bssl, nil, gsMakeDataExprIf(b,s1,s1),s1)
    ), OpEqns);
  //Add OpEqns to DataEqns
  PDataDecls->DataEqns = ATconcat(PDataDecls->DataEqns,OpEqns);

  return SortId;
}

ATermAppl gsImplSortList(ATermAppl SortList, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  assert(gsIsSortList(SortList));
  //declare fresh sort identifier for SortList
  ATermAppl SortId = gsMakeFreshListSortId((ATerm) PDataDecls->Sorts);
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
  //add substitution for this identifier
  ATermAppl Subst = gsMakeSubst_Appl(SortList, SortId);
  *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
  //declare constructors for sort SortId
  ATermAppl SortElt = ATAgetArgument(SortList, 0);
  PDataDecls->ConsOps = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdEmptyList(SortId),
      (ATerm) gsMakeOpIdCons(SortElt, SortId)
    ), PDataDecls->ConsOps);
  //Declare operations for sort SortId
  PDataDecls->Ops = ATconcat(ATmakeList(11,
      (ATerm) gsMakeOpIdEq(SortId),
      (ATerm) gsMakeOpIdNeq(SortId),
      (ATerm) gsMakeOpIdIf(SortId),
      (ATerm) gsMakeOpIdListSize(SortId),
      (ATerm) gsMakeOpIdSnoc(SortId, SortElt),
      (ATerm) gsMakeOpIdConcat(SortId),
      (ATerm) gsMakeOpIdEltAt(SortId, SortElt),
      (ATerm) gsMakeOpIdLHead(SortId, SortElt),
      (ATerm) gsMakeOpIdLTail(SortId),
      (ATerm) gsMakeOpIdRHead(SortId, SortElt),
      (ATerm) gsMakeOpIdRTail(SortId)
    ), PDataDecls->Ops);
  //Declare data equations for sort SortId
  ATermList el = ATmakeList0();
  ATermAppl elSortId = gsMakeDataExprEmptyList(SortId);
  ATermAppl sSortId = gsMakeDataVarId(gsString2ATermAppl("s"), SortId);
  ATermAppl tSortId = gsMakeDataVarId(gsString2ATermAppl("t"), SortId);
  ATermAppl dSortElt = gsMakeDataVarId(gsString2ATermAppl("d"), SortElt);
  ATermAppl eSortElt = gsMakeDataVarId(gsString2ATermAppl("e"), SortElt);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl ds = gsMakeDataExprCons(dSortElt, sSortId);
  ATermAppl et = gsMakeDataExprCons(eSortElt, tSortId);
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExpr0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList dl = ATmakeList1((ATerm) dSortElt);
  ATermList sl = ATmakeList1((ATerm) sSortId);
  ATermList stl = ATmakeList2((ATerm) sSortId, (ATerm) tSortId);
  ATermList dsl = ATmakeList2((ATerm) dSortElt, (ATerm) sSortId);
  ATermList desl = ATmakeList3((ATerm) dSortElt, (ATerm) eSortElt,
    (ATerm) sSortId);
  ATermList dstl = ATmakeList3((ATerm) dSortElt, (ATerm) sSortId,
    (ATerm) tSortId);
  ATermList destl = ATmakeList4((ATerm) dSortElt, (ATerm) eSortElt,
    (ATerm) sSortId, (ATerm) tSortId);
  ATermList dspl = ATmakeList3((ATerm) dSortElt, (ATerm) sSortId, (ATerm) p);
  ATermList bsl = ATmakeList2((ATerm) b, (ATerm) sSortId);
  PDataDecls->DataEqns = ATconcat(ATmakeList(24,
      //equality (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(elSortId, elSortId), t),
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(elSortId, ds), f),
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(ds, elSortId), f),
      (ATerm) gsMakeDataEqn(destl, nil,
        gsMakeDataExprEq(ds, et),
        gsMakeDataExprAnd(
          gsMakeDataExprEq(dSortElt, eSortElt),
          gsMakeDataExprEq(sSortId, tSortId))),
      (ATerm) gsMakeDataEqn(sl, nil, gsMakeDataExprEq(sSortId, sSortId), t),
      //inequality (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprNeq(sSortId, tSortId), 
        gsMakeDataExprNot(gsMakeDataExprEq(sSortId, tSortId))),
      //conditional (Bool -> SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(t, sSortId, tSortId),
        sSortId),
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(f, sSortId, tSortId),
        tSortId),
      (ATerm) gsMakeDataEqn(bsl,nil,
        gsMakeDataExprIf(b, sSortId, sSortId),
        sSortId),
      //list size (SortId -> Nat)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprListSize(elSortId),
        gsMakeDataExpr0()),
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprListSize(ds),
        gsMakeDataExprCNat(
          gsMakeDataExprSucc(gsMakeDataExprListSize(sSortId)))),
      //list snoc (SortId -> SortElt -> SortId)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprSnoc(elSortId, dSortElt),
        gsMakeDataExprCons(dSortElt, elSortId)),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprSnoc(ds, eSortElt),
        gsMakeDataExprCons(dSortElt, gsMakeDataExprSnoc(sSortId, eSortElt))),
      //list concatenation (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprConcat(elSortId, sSortId),
        sSortId),
      (ATerm) gsMakeDataEqn(dstl, nil,
        gsMakeDataExprConcat(ds, tSortId),
        gsMakeDataExprCons(dSortElt, gsMakeDataExprConcat(sSortId, tSortId))),
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprConcat(sSortId, elSortId),
        sSortId),
      //list element at (SortId -> Nat -> SortElt)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprEltAt(ds, zero, SortElt),
        dSortElt),
      (ATerm) gsMakeDataEqn(dspl, nil,
        gsMakeDataExprEltAt(ds, gsMakeDataExprCNat(p), SortElt),
        gsMakeDataExprEltAt(sSortId, gsMakeDataExprPred(p), SortElt)),
      //left head (SortId -> SortElt)
      (ATerm) gsMakeDataEqn(dsl, nil,
         gsMakeDataExprLHead(ds, SortElt),
         dSortElt),
      //left tail (SortId -> SortId)
      (ATerm) gsMakeDataEqn(dsl, nil,
         gsMakeDataExprLTail(ds),
         sSortId),
      //right head (SortId -> SortElt)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprRHead(gsMakeDataExprCons(dSortElt, elSortId), SortElt),
        dSortElt),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprRHead(gsMakeDataExprCons(dSortElt,
          gsMakeDataExprCons(eSortElt, sSortId)), SortElt),
        gsMakeDataExprRHead(gsMakeDataExprCons(eSortElt, sSortId), SortElt)),
      //right tail (SortId -> SortId)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprRTail(gsMakeDataExprCons(dSortElt, elSortId)),
        elSortId),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprRTail(gsMakeDataExprCons(dSortElt,
          gsMakeDataExprCons(eSortElt, sSortId))),
        gsMakeDataExprCons(dSortElt,
          gsMakeDataExprRTail(gsMakeDataExprCons(eSortElt, sSortId))))
    ), PDataDecls->DataEqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    gsImplSortNat(PDataDecls);
  }
  return SortId;
}

ATermAppl gsImplSortSet(ATermAppl SortSet, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  assert(gsIsSortSet(SortSet));
  //declare fresh sort identifier for SortSet
  ATermAppl SortId = gsMakeFreshSetSortId((ATerm) PDataDecls->Sorts);
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
  //add substitution for this identifier
  ATermAppl Subst = gsMakeSubst_Appl(SortSet, SortId);
  *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
  //declare operations for sort SortId
  ATermAppl SortElt = ATAgetArgument(SortSet, 0);
  PDataDecls->Ops = ATconcat(ATmakeList(12,
      (ATerm) gsMakeOpIdEq(SortId),
      (ATerm) gsMakeOpIdNeq(SortId),
      (ATerm) gsMakeOpIdIf(SortId),
      (ATerm) gsMakeOpIdSetComp(SortElt, SortId),
      (ATerm) gsMakeOpIdEmptySet(SortId),
      (ATerm) gsMakeOpIdSetIn(SortElt, SortId),
      (ATerm) gsMakeOpIdSubSetEq(SortId),
      (ATerm) gsMakeOpIdSubSet(SortId),
      (ATerm) gsMakeOpIdSetUnion(SortId),
      (ATerm) gsMakeOpIdSetDiff(SortId),
      (ATerm) gsMakeOpIdSetIntersect(SortId),
      (ATerm) gsMakeOpIdSetCompl(SortId)
    ), PDataDecls->Ops);
  //declare data equations for sort SortId
  ATermAppl SortFunc = gsMakeSortArrow(SortElt, gsMakeSortExprBool());
  ATermList el = ATmakeList0();
  ATermAppl sSortId = gsMakeDataVarId(gsString2ATermAppl("s"), SortId);
  ATermAppl tSortId = gsMakeDataVarId(gsString2ATermAppl("t"), SortId);
  ATermAppl fSortFunc = gsMakeDataVarId(gsString2ATermAppl("f"), SortFunc);
  ATermAppl gSortFunc = gsMakeDataVarId(gsString2ATermAppl("g"), SortFunc);
  ATermAppl dSortElt = gsMakeDataVarId(gsString2ATermAppl("d"), SortElt);
  ATermAppl xSortElt = gsMakeDataVarId(gsString2ATermAppl("x"), SortElt);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList stl = ATmakeList2((ATerm) sSortId, (ATerm) tSortId);
  ATermList bsl = ATmakeList2((ATerm) b, (ATerm) sSortId);
  ATermList fl = ATmakeList1((ATerm) fSortFunc);
  ATermList dfl = ATmakeList2((ATerm) dSortElt, (ATerm) fSortFunc);
  ATermList fgl = ATmakeList2((ATerm) fSortFunc, (ATerm) gSortFunc);
  ATermAppl FalseFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt), f), PSubsts, PDataDecls);
  ATermAppl ImpFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprImp(
        gsMakeDataAppl(fSortFunc, xSortElt),
        gsMakeDataAppl(gSortFunc, xSortElt)
      )
    ), PSubsts, PDataDecls);
  ATermAppl OrFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprOr(
        gsMakeDataAppl(fSortFunc, xSortElt),
        gsMakeDataAppl(gSortFunc, xSortElt)
      )
    ), PSubsts, PDataDecls);
  ATermAppl AndFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprAnd(
        gsMakeDataAppl(fSortFunc, xSortElt),
        gsMakeDataAppl(gSortFunc, xSortElt)
      )
    ), PSubsts, PDataDecls);
  ATermAppl NotFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprNot(gsMakeDataAppl(fSortFunc, xSortElt))
    ), PSubsts, PDataDecls);
  PDataDecls->DataEqns = ATconcat(ATmakeList(13,
      //equality (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprSetComp(fSortFunc, SortId),
          gsMakeDataExprSetComp(gSortFunc, SortId)), 
        gsMakeDataExprEq(fSortFunc, gSortFunc)),
      //inequality (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprNeq(sSortId, tSortId), 
        gsMakeDataExprNot(gsMakeDataExprEq(sSortId, tSortId))),
      //conditional (Bool -> SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(t, sSortId, tSortId),
        sSortId),
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(f, sSortId, tSortId),
        tSortId),
      (ATerm) gsMakeDataEqn(bsl,nil,
        gsMakeDataExprIf(b, sSortId, sSortId),
        sSortId),
      //empty set (SortId)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptySet(SortId),
        gsMakeDataExprSetComp(FalseFunc, SortId)),
      //element test (SortElt -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprSetIn(dSortElt, gsMakeDataExprSetComp(fSortFunc, SortId)),
        gsMakeDataAppl(fSortFunc, dSortElt)),
      //subset or equal (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubSetEq(
          gsMakeDataExprSetComp(fSortFunc, SortId),
          gsMakeDataExprSetComp(gSortFunc, SortId)), 
        gsMakeDataExprForall(ImpFunc)),
      //proper subset (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSubSet(sSortId, tSortId),
        gsMakeDataExprAnd(
          gsMakeDataExprSubSetEq(sSortId, tSortId), 
          gsMakeDataExprNeq(sSortId, tSortId)
        )),
      //union (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetUnion(
          gsMakeDataExprSetComp(fSortFunc, SortId),
          gsMakeDataExprSetComp(gSortFunc, SortId)), 
        gsMakeDataExprSetComp(OrFunc, SortId)),
      //difference (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSetDiff(sSortId, tSortId),
        gsMakeDataExprSetInterSect(sSortId, gsMakeDataExprSetCompl(tSortId))),
      //intersection (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetInterSect(
          gsMakeDataExprSetComp(fSortFunc, SortId),
          gsMakeDataExprSetComp(gSortFunc, SortId)), 
        gsMakeDataExprSetComp(AndFunc, SortId)),
      //complement (SortId -> SortId)
      (ATerm) gsMakeDataEqn(fl, nil,
        gsMakeDataExprSetCompl(gsMakeDataExprSetComp(fSortFunc, SortId)),
        gsMakeDataExprSetComp(NotFunc, SortId))
      //simplification of combinations of functions false, not, imp, and, or
      ////left unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), FalseFunc, fSortFunc),
      //  fSortFunc),
      ////right unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), fSortFunc, FalseFunc),
      //  fSortFunc)
    ), PDataDecls->DataEqns);
  return SortId;
}

ATermAppl gsImplSortBag(ATermAppl SortBag, ATermList *PSubsts,
  TDataDecls *PDataDecls)
{
  assert(gsIsSortBag(SortBag));
  ATermAppl SortElt = ATAgetArgument(SortBag, 0);  
  //add implementation of sort Set(SortElt), if necessary
  // XXX This piece of code should somehow be moved to the end of the function
  ATermAppl SortSet = gsMakeSortSet(SortElt);
  ATermAppl SortSetImpl =
    (ATermAppl) gsSubstValues(*PSubsts, (ATerm) SortSet, false);
  if (ATisEqual(SortSetImpl, SortSet)) {
    //Set(SortElt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *PSubsts
    SortSetImpl = gsImplSortSet(SortSet, PSubsts, PDataDecls);
  }
  //declare fresh sort identifier for SortBag
  ATermAppl SortId = gsMakeFreshBagSortId((ATerm) PDataDecls->Sorts);
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) SortId);
  //add substitution for this identifier
  ATermAppl Subst = gsMakeSubst_Appl(SortBag, SortId);
  *PSubsts = gsAddSubstToSubsts(Subst, *PSubsts);
  //declare operations for sort SortId
  PDataDecls->Ops = ATconcat(ATmakeList(14,
      (ATerm) gsMakeOpIdEq(SortId),
      (ATerm) gsMakeOpIdNeq(SortId),
      (ATerm) gsMakeOpIdIf(SortId),
      (ATerm) gsMakeOpIdBagComp(SortElt, SortId),
      (ATerm) gsMakeOpIdEmptyBag(SortId),
      (ATerm) gsMakeOpIdCount(SortElt, SortId),
      (ATerm) gsMakeOpIdBagIn(SortElt, SortId),
      (ATerm) gsMakeOpIdSubBagEq(SortId),
      (ATerm) gsMakeOpIdSubBag(SortId),
      (ATerm) gsMakeOpIdBagUnion(SortId),
      (ATerm) gsMakeOpIdBagDiff(SortId),
      (ATerm) gsMakeOpIdBagIntersect(SortId),
      (ATerm) gsMakeOpIdBag2Set(SortId, SortSetImpl),
      (ATerm) gsMakeOpIdSet2Bag(SortSetImpl, SortId)
    ), PDataDecls->Ops);
  //declare data equations for sort SortId
  ATermAppl SortFunc = gsMakeSortArrow(SortElt, gsMakeSortExprNat());
  ATermList el = ATmakeList0();
  ATermAppl sSortId = gsMakeDataVarId(gsString2ATermAppl("s"), SortId);
  ATermAppl tSortId = gsMakeDataVarId(gsString2ATermAppl("t"), SortId);
  ATermAppl fSortFunc = gsMakeDataVarId(gsString2ATermAppl("f"), SortFunc);
  ATermAppl gSortFunc = gsMakeDataVarId(gsString2ATermAppl("g"), SortFunc);
  ATermAppl dSortElt = gsMakeDataVarId(gsString2ATermAppl("d"), SortElt);
  ATermAppl xSortElt = gsMakeDataVarId(gsString2ATermAppl("x"), SortElt);
  ATermAppl uSortSetImpl = gsMakeDataVarId(gsString2ATermAppl("u"), SortSetImpl);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl mName = gsString2ATermAppl("m");
  ATermAppl nName = gsString2ATermAppl("n");
  ATermAppl m = gsMakeDataVarId(mName, gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(nName, gsMakeSortExprNat());
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExpr0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList sl = ATmakeList1((ATerm) sSortId);
  ATermList stl = ATmakeList2((ATerm) sSortId, (ATerm) tSortId);
  ATermList dsl = ATmakeList2((ATerm) dSortElt, (ATerm) sSortId);
  ATermList ul = ATmakeList1((ATerm) uSortSetImpl);
  ATermList bsl = ATmakeList2((ATerm) b, (ATerm) sSortId);
  ATermList dfl = ATmakeList2((ATerm) dSortElt, (ATerm) fSortFunc);
  ATermList fgl = ATmakeList2((ATerm) fSortFunc, (ATerm) gSortFunc);
  ATermAppl ZeroFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt), zero), PSubsts, PDataDecls);
  ATermAppl LTEFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprLTE(
        gsMakeDataAppl(fSortFunc, xSortElt),
        gsMakeDataAppl(gSortFunc, xSortElt)
      )
    ), PSubsts, PDataDecls);
  ATermAppl AddFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprAdd(
        gsMakeDataAppl(fSortFunc, xSortElt),
        gsMakeDataAppl(gSortFunc, xSortElt)
      )
    ), PSubsts, PDataDecls);
  ATermAppl SubtMax0Func = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeWhr(
        gsMakeDataExprIf(
          gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
        ), ATmakeList2(
          (ATerm) gsMakeWhrDecl(mName, gsMakeDataAppl(fSortFunc, xSortElt)),
          (ATerm) gsMakeWhrDecl(nName, gsMakeDataAppl(gSortFunc, xSortElt))
        )
      )
    ), PSubsts, PDataDecls);
  ATermAppl MinFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprMin(
        gsMakeDataAppl(fSortFunc, xSortElt),
        gsMakeDataAppl(gSortFunc, xSortElt)
      )
    ), PSubsts, PDataDecls);
  ATermAppl Bag2SetFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprBagIn(xSortElt, sSortId)
    ), PSubsts, PDataDecls);
  ATermAppl Set2BagFunc = gsImplExprsPart(
    gsMakeLambda(ATmakeList1((ATerm) xSortElt),
      gsMakeDataExprIf(
        gsMakeDataExprSetIn(xSortElt, uSortSetImpl),
        gsMakeDataExprNat_int(1),
        gsMakeDataExprNat_int(0)
      )
    ), PSubsts, PDataDecls);
  PDataDecls->DataEqns = ATconcat(ATmakeList(15,
      //equality (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprBagComp(fSortFunc, SortId),
          gsMakeDataExprBagComp(gSortFunc, SortId)), 
        gsMakeDataExprEq(fSortFunc, gSortFunc)),
      //inequality (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprNeq(sSortId, tSortId), 
        gsMakeDataExprNot(gsMakeDataExprEq(sSortId, tSortId))),
      //conditional (Bool -> SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(t, sSortId, tSortId),
        sSortId),
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(f, sSortId, tSortId),
        tSortId),
      (ATerm) gsMakeDataEqn(bsl,nil,
        gsMakeDataExprIf(b, sSortId, sSortId),
        sSortId),
      //empty bag (SortId)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptyBag(SortId),
        gsMakeDataExprBagComp(ZeroFunc, SortId)),
      //count (SortElt -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprCount(dSortElt, gsMakeDataExprBagComp(fSortFunc, SortId)),
        gsMakeDataAppl(fSortFunc, dSortElt)),
      //element test (SortElt -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprBagIn(dSortElt, sSortId),
        gsMakeDataExprGT(gsMakeDataExprCount(dSortElt, sSortId), zero)),
      //subbag or equal (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubBagEq(
          gsMakeDataExprBagComp(fSortFunc, SortId),
          gsMakeDataExprBagComp(gSortFunc, SortId)), 
        gsMakeDataExprForall(LTEFunc)),
      //proper subbag (SortId -> SortId -> Bool)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSubBag(sSortId, tSortId),
        gsMakeDataExprAnd(
          gsMakeDataExprSubBagEq(sSortId, tSortId), 
          gsMakeDataExprNeq(sSortId, tSortId)
        )),
      //union (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagUnion(
          gsMakeDataExprBagComp(fSortFunc, SortId),
          gsMakeDataExprBagComp(gSortFunc, SortId)), 
        gsMakeDataExprBagComp(AddFunc, SortId)),
      //difference (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagDiff(
          gsMakeDataExprBagComp(fSortFunc, SortId),
          gsMakeDataExprBagComp(gSortFunc, SortId)), 
        gsMakeDataExprBagComp(SubtMax0Func, SortId)),
      //intersection (SortId -> SortId -> SortId)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagInterSect(
          gsMakeDataExprBagComp(fSortFunc, SortId),
          gsMakeDataExprBagComp(gSortFunc, SortId)), 
        gsMakeDataExprBagComp(MinFunc, SortId)),
      //Bag2Set (SortId -> SortSetImpl)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprBag2Set(sSortId, SortSetImpl),
        gsMakeDataExprSetComp(Bag2SetFunc, SortSetImpl)),
      //Set2Bag (SortSetImpl -> SortId)
      (ATerm) gsMakeDataEqn(ul, nil,
        gsMakeDataExprSet2Bag(uSortSetImpl, SortId),
        gsMakeDataExprBagComp(Set2BagFunc, SortId))
    ), PDataDecls->DataEqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    gsImplSortNat(PDataDecls);
  }
  return SortId;
}

void gsImplSortBool(TDataDecls *PDataDecls)
{
  //Declare sort Bool
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdBool());
  //Declare constructors for sort Bool
  PDataDecls->ConsOps = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdTrue(),
      (ATerm) gsMakeOpIdFalse()
    ), PDataDecls->ConsOps);
  //Declare operations for sort Bool
  PDataDecls->Ops = ATconcat(ATmakeList(7,
      (ATerm) gsMakeOpIdNot(),
      (ATerm) gsMakeOpIdAnd(),
      (ATerm) gsMakeOpIdOr(),
      (ATerm) gsMakeOpIdImp(),
      (ATerm) gsMakeOpIdEq(gsMakeSortExprBool()),
      (ATerm) gsMakeOpIdNeq(gsMakeSortExprBool()),
      (ATerm) gsMakeOpIdIf(gsMakeSortExprBool())
    ), PDataDecls->Ops);
  //Declare data equations for sort Bool
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), gsMakeSortExprBool());
  ATermList bl = ATmakeList1((ATerm) b);
  ATermList bcl = ATmakeList2((ATerm) b, (ATerm) c);
  PDataDecls->DataEqns = ATconcat(ATmakeList(24,
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
    ), PDataDecls->DataEqns);
}

void gsImplSortPos(TDataDecls *PDataDecls)
{
  //Declare sort Pos
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdPos());
  //Declare constructors for sort Pos
  PDataDecls->ConsOps = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpId1(),
      (ATerm) gsMakeOpIdCDub()
    ), PDataDecls->ConsOps);
  //Declare operations for sort Pos
  ATermAppl sePos = gsMakeSortExprPos();
  PDataDecls->Ops = ATconcat(ATmakeList(16,
      (ATerm) gsMakeOpIdEq(sePos),
      (ATerm) gsMakeOpIdNeq(sePos),
      (ATerm) gsMakeOpIdIf(sePos),
      (ATerm) gsMakeOpIdLTE(sePos),
      (ATerm) gsMakeOpIdLT(sePos),
      (ATerm) gsMakeOpIdGTE(sePos),
      (ATerm) gsMakeOpIdGT(sePos),
      (ATerm) gsMakeOpIdMax(sePos, sePos),
      (ATerm) gsMakeOpIdMin(sePos),
      (ATerm) gsMakeOpIdAbs(sePos),
      (ATerm) gsMakeOpIdSucc(sePos),
      (ATerm) gsMakeOpIdDub(sePos),
      (ATerm) gsMakeOpIdAdd(sePos, sePos),
      (ATerm) gsMakeOpIdAddC(),
      (ATerm) gsMakeOpIdMult(sePos),
      (ATerm) gsMakeOpIdMultIR()
    ), PDataDecls->Ops);
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
  PDataDecls->DataEqns = ATconcat(ATmakeList(45,
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
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprEq(p, p), t),
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
         gsMakeDataExprGTE(p, q), gsMakeDataExprLTE(q, p)),
      //greater than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGT(p, q), gsMakeDataExprLT(q, p)),
      //maximum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMax(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), q, p)),
      //minimum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMin(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), p, q)),
      //absolute value (Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprAbs(p), p),
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
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdAdd(sePos, sePos),
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
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprMult(p, q),
         gsMakeDataExprMultIR(f, one, p, q)),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprGT(p, q),
         gsMakeDataExprMult(p, q),
         gsMakeDataExprMultIR(f, one, q, p)),
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
    ), PDataDecls->DataEqns);
}

void gsImplSortNat(TDataDecls *PDataDecls)
{
  //Declare sort Nat
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat());
  //Declare constructors for sort Nat
  PDataDecls->ConsOps = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpId0(),
      (ATerm) gsMakeOpIdCNat()
    ), PDataDecls->ConsOps);
  //Declare operations for sort Nat
  ATermAppl seNat = gsMakeSortExprNat();
  ATermAppl sePos = gsMakeSortExprPos();
  PDataDecls->Ops = ATconcat(ATmakeList(31,
      (ATerm) gsMakeOpIdEq(seNat),
      (ATerm) gsMakeOpIdNeq(seNat),
      (ATerm) gsMakeOpIdIf(seNat),
      (ATerm) gsMakeOpIdPos2Nat(),
      (ATerm) gsMakeOpIdNat2Pos(),
      (ATerm) gsMakeOpIdLTE(seNat),
      (ATerm) gsMakeOpIdLT(seNat),
      (ATerm) gsMakeOpIdGTE(seNat),
      (ATerm) gsMakeOpIdGT(seNat),
      (ATerm) gsMakeOpIdMax(sePos, seNat),
      (ATerm) gsMakeOpIdMax(seNat, sePos),
      (ATerm) gsMakeOpIdMax(seNat, seNat),
      (ATerm) gsMakeOpIdMin(seNat),
      (ATerm) gsMakeOpIdAbs(seNat),
      (ATerm) gsMakeOpIdSucc(seNat),
      (ATerm) gsMakeOpIdPred(sePos),
      (ATerm) gsMakeOpIdDub(seNat),
      (ATerm) gsMakeOpIdAdd(sePos, seNat),
      (ATerm) gsMakeOpIdAdd(seNat, sePos),
      (ATerm) gsMakeOpIdAdd(seNat, seNat),
      (ATerm) gsMakeOpIdGTESubt(sePos),
      (ATerm) gsMakeOpIdGTESubt(seNat),
      (ATerm) gsMakeOpIdGTESubtB(),
      (ATerm) gsMakeOpIdMult(seNat),
      (ATerm) gsMakeOpIdDiv(sePos),
      (ATerm) gsMakeOpIdDiv(seNat),
      (ATerm) gsMakeOpIdMod(sePos),
      (ATerm) gsMakeOpIdMod(seNat),
      (ATerm) gsMakeOpIdExp(sePos),
      (ATerm) gsMakeOpIdExp(seNat),
      (ATerm) gsMakeOpIdEven()
    ), PDataDecls->Ops);
  //Declare data equations for sort Nat
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
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) b, (ATerm) c, (ATerm) p, (ATerm) q);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  PDataDecls->DataEqns = ATconcat(ATmakeList(70,
      //equality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(zero, zero), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(zero, gsMakeDataExprCNat(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprEq(n, n), t),
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
         gsMakeDataExprGTE(m, n), gsMakeDataExprLTE(n, m)),
      //greater than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGT(m, n), gsMakeDataExprLT(n, m)),
      //maximum (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprMax(p, zero), p),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCNat(q)),
         gsMakeDataExprMax(p, q)),
      //maximum (Nat -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprMax(zero, p), p),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(gsMakeDataExprCNat(p), q),
         gsMakeDataExprMax(p, q)),
      //maximum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMax(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), n, m)),
      //minimum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMin(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), m, n)),
      //absolute value (Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprAbs(n), n),
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
      //addition (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprAdd(p, zero), p),
      (ATerm) gsMakeDataEqn(pql, nil,
        gsMakeDataExprAdd(p, gsMakeDataExprCNat(q)),
        gsMakeDataExprAdd(p, q)),
      //addition (Nat -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprAdd(zero, p), p),
      (ATerm) gsMakeDataEqn(pql, nil,
        gsMakeDataExprAdd(gsMakeDataExprCNat(p), q),
        gsMakeDataExprAdd(p, q)),
      //addition (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(zero, n), n),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(n, zero), n),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprAddC(f, p, q))),
      //GTE subtraction (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprGTESubt(p, q),
         gsMakeDataExprGTESubtB(f, p, q)),
      //GTE subtraction (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprGTESubt(n, zero), n),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprGTESubt(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprGTESubt(p, q)),
      //GTE subtraction with borrow (Bool -> Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprGTESubtB(f, p, one),
         gsMakeDataExprPred(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprGTESubtB(t, p, one),
         gsMakeDataExprPred(gsMakeDataExprNat2Pos(gsMakeDataExprPred(p)))),
      (ATerm) gsMakeDataEqn(bcpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(c, p),
           gsMakeDataExprCDub(c, q)),
         gsMakeDataExprDub(b, gsMakeDataExprGTESubtB(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(f, p),
           gsMakeDataExprCDub(t, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b),
           gsMakeDataExprGTESubtB(t, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(t, p),
           gsMakeDataExprCDub(f, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b),
           gsMakeDataExprGTESubtB(f, p, q))),
      //multiplication (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(zero, n), zero),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(n, zero), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMult(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprMult(p, q))),
      //quotient after division (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprGTE(p, q),
         gsMakeDataExprDiv(p, q),
         gsMakeDataExprCNat(gsMakeDataExprSucc(
           gsMakeDataExprDiv(gsMakeDataExprGTESubt(p, q), q)))),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprDiv(p, q),
         gsMakeDataExpr0()),
      //quotient after division (Nat -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprDiv(zero, p), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprDiv(gsMakeDataExprCNat(p), q),
         gsMakeDataExprDiv(p, q)),
      //remainder after division (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprGTE(p, q),
         gsMakeDataExprMod(p, q),
         gsMakeDataExprMod(gsMakeDataExprGTESubt(p, q), q)),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprMod(p, q),
         gsMakeDataExprCNat(p)),
      //remainder after division (Nat -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprMod(zero, p), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMod(gsMakeDataExprCNat(p), q),
         gsMakeDataExprMod(p, q)),
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
        gsMakeDataExprEven(gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))),
        gsMakeDataExprNot(b))
    ), PDataDecls->DataEqns);
}

void gsImplSortInt(TDataDecls *PDataDecls)
{
  //Declare sort Int
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdInt());
  //Declare constructors for sort Int
  PDataDecls->ConsOps = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdCInt(),
      (ATerm) gsMakeOpIdCNeg()
    ), PDataDecls->ConsOps);
  //Declare operations for sort Int
  ATermAppl sePos = gsMakeSortExprPos();
  ATermAppl seNat = gsMakeSortExprNat();
  ATermAppl seInt = gsMakeSortExprInt();
  PDataDecls->Ops = ATconcat(ATmakeList(33,
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
      (ATerm) gsMakeOpIdMax(sePos, seInt),
      (ATerm) gsMakeOpIdMax(seInt, sePos),
      (ATerm) gsMakeOpIdMax(seNat, seInt),
      (ATerm) gsMakeOpIdMax(seInt, seNat),
      (ATerm) gsMakeOpIdMax(seInt, seInt),
      (ATerm) gsMakeOpIdMin(seInt),
      (ATerm) gsMakeOpIdAbs(seInt),
      (ATerm) gsMakeOpIdNeg(sePos),
      (ATerm) gsMakeOpIdNeg(seNat),
      (ATerm) gsMakeOpIdNeg(seInt),
      (ATerm) gsMakeOpIdSucc(seInt),
      (ATerm) gsMakeOpIdPred(seNat),
      (ATerm) gsMakeOpIdPred(seInt),
      (ATerm) gsMakeOpIdDub(seInt),
      (ATerm) gsMakeOpIdAdd(seInt, seInt),
      (ATerm) gsMakeOpIdSubt(sePos),
      (ATerm) gsMakeOpIdSubt(seNat),
      (ATerm) gsMakeOpIdSubt(seInt),
      (ATerm) gsMakeOpIdMult(seInt),
      (ATerm) gsMakeOpIdDiv(seInt),
      (ATerm) gsMakeOpIdMod(seInt),
      (ATerm) gsMakeOpIdExp(seInt)
    ), PDataDecls->Ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExpr1();
  ATermAppl zero = gsMakeDataExpr0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bxl = ATmakeList2((ATerm) b, (ATerm) x);
  ATermList xl  = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  PDataDecls->DataEqns = ATconcat(ATmakeList(69,
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
      (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprEq(x, x), t),
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
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprInt2Pos(gsMakeDataExprCInt(n)),
         gsMakeDataExprNat2Pos(n)),
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
         gsMakeDataExprGTE(x, y), gsMakeDataExprLTE(y, x)),
      //greater than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGT(x, y), gsMakeDataExprLT(y, x)),
      //maximum (Pos -> Int -> Pos)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCInt(n)),
         gsMakeDataExprMax(p, n)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCNeg(q)), p),
      //maximum (Int -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCInt(n), p),
         gsMakeDataExprMax(n, p)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(gsMakeDataExprCNeg(q), p), p),
      //maximum (Nat -> Int -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprMax(m, gsMakeDataExprCInt(n)),
         gsMakeDataExprMax(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(n, gsMakeDataExprCNeg(p)), n),
      //maximum (Int -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCInt(m), n),
         gsMakeDataExprMax(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCNeg(p), n), n),
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
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprGTE(p, q),
         gsMakeDataExprSubt(p, q),
         gsMakeDataExprCInt(gsMakeDataExprGTESubt(p, q))),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprSubt(p, q),
         gsMakeDataExprNeg(gsMakeDataExprGTESubt(q, p))),      
      //subtraction (Nat -> Nat -> Int)
      (ATerm) gsMakeDataEqn(mnl,
         gsMakeDataExprGTE(m, n),
         gsMakeDataExprSubt(m, n),
         gsMakeDataExprCInt(gsMakeDataExprGTESubt(m, n))),
      (ATerm) gsMakeDataEqn(mnl,
         gsMakeDataExprLT(m, n),
         gsMakeDataExprSubt(m, n),
         gsMakeDataExprNeg(gsMakeDataExprGTESubt(n, m))),      
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
        gsMakeDataExprMult(p, q)),
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
      (ATerm) gsMakeDataEqn(pnl,
         gsMakeDataExprEven(n),
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p, n)))),
      (ATerm) gsMakeDataEqn(pnl,
         gsMakeDataExprNot(gsMakeDataExprEven(n)),
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprCNeg(gsMakeDataExprExp(p, n)))
    ), PDataDecls->DataEqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    gsImplSortNat(PDataDecls);
  }
}

void gsImplSortReal(TDataDecls *PDataDecls)
{
  //Declare sort Real
  PDataDecls->Sorts = ATinsert(PDataDecls->Sorts, (ATerm) gsMakeSortIdReal());
  //Declare constructors for sort Real
  PDataDecls->ConsOps =
    ATconcat(ATmakeList1((ATerm) gsMakeOpIdCReal()), PDataDecls->ConsOps);
  //Declare operations for sort Real
  ATermAppl seReal = gsMakeSortExprReal();
  PDataDecls->Ops = ATconcat(ATmakeList(23,
      (ATerm) gsMakeOpIdEq(seReal),
      (ATerm) gsMakeOpIdNeq(seReal),
      (ATerm) gsMakeOpIdIf(seReal),
      (ATerm) gsMakeOpIdPos2Real(),
      (ATerm) gsMakeOpIdNat2Real(),
      (ATerm) gsMakeOpIdInt2Real(),
      (ATerm) gsMakeOpIdReal2Pos(),
      (ATerm) gsMakeOpIdReal2Nat(),
      (ATerm) gsMakeOpIdReal2Int(),
      (ATerm) gsMakeOpIdLTE(seReal),
      (ATerm) gsMakeOpIdLT(seReal),
      (ATerm) gsMakeOpIdGTE(seReal),
      (ATerm) gsMakeOpIdGT(seReal),
      (ATerm) gsMakeOpIdMax(seReal, seReal),
      (ATerm) gsMakeOpIdMin(seReal),
      (ATerm) gsMakeOpIdAbs(seReal),
      (ATerm) gsMakeOpIdNeg(seReal),
      (ATerm) gsMakeOpIdSucc(seReal),
      (ATerm) gsMakeOpIdPred(seReal),
      (ATerm) gsMakeOpIdAdd(seReal, seReal),
      (ATerm) gsMakeOpIdSubt(seReal),
      (ATerm) gsMakeOpIdMult(seReal),
      (ATerm) gsMakeOpIdExp(seReal)
    ), PDataDecls->Ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), gsMakeSortExprReal());
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), gsMakeSortExprReal());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList brl = ATmakeList2((ATerm) b, (ATerm) r);
  ATermList xl  = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList nxl = ATmakeList2((ATerm) n, (ATerm) x);
  ATermList rl  = ATmakeList1((ATerm) r);
  ATermList rsl = ATmakeList2((ATerm) r, (ATerm) s);
  PDataDecls->DataEqns = ATconcat(ATmakeList(26,
      //equality (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprEq(x, y)),
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprEq(r, r), t),
      //inequality (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprNeq(r, s), gsMakeDataExprNot(gsMakeDataExprEq(r, s))),
      //conditional (Bool -> Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprIf(t, r, s), r),
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprIf(f, r, s), s),
      (ATerm) gsMakeDataEqn(brl,nil, gsMakeDataExprIf(b, r, r), r),
      //convert Int to Real (Int -> Real)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdInt2Real(), gsMakeOpIdCReal()),
      //convert Nat to Real (Nat -> Real)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprNat2Real(n),
         gsMakeDataExprCReal(gsMakeDataExprCInt(n))),
      //convert Pos to Real (Pos -> Real)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprPos2Real(p),
         gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(p)))),
      //convert Real to Int (Real -> Int)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Int(gsMakeDataExprCReal(x)), x),
      //convert Real to Nat (Real -> Nat)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Nat(gsMakeDataExprCReal(x)),
         gsMakeDataExprInt2Nat(x)),
      //convert Real to Pos (Real -> Pos)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Pos(gsMakeDataExprCReal(x)),
         gsMakeDataExprInt2Pos(x)),
      //less than or equal (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprLTE(x, y)),
      //less than (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprLT(x, y)),
      //greater than or equal (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprGTE(r, s), gsMakeDataExprLTE(s, r)),
      //greater than (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprGT(r, s), gsMakeDataExprLT(s, r)),
      //maximum (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprMax(r, s),
         gsMakeDataExprIf(gsMakeDataExprLTE(r, s), s, r)),
      //minimum (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprMin(r, s),
         gsMakeDataExprIf(gsMakeDataExprLTE(r, s), r, s)),
      //absolute value (Real -> Real) 
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprAbs(x)))),
      //negation (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprNeg(x))),
      //successor (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprSucc(x))),
      //predecessor (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprPred(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprPred(x))),
      //addition (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprAdd(x, y))),
      //subtraction (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprSubt(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprSubt(x, y))),
      //multiplication (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprMult(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprMult(x, y))),
      //exponentiation (Real -> Nat -> Real)
      (ATerm) gsMakeDataEqn(nxl,nil,
         gsMakeDataExprExp(gsMakeDataExprCReal(x), n),
         gsMakeDataExprCReal(gsMakeDataExprExp(x, n)))
    ), PDataDecls->DataEqns);
  //add implementation of sort Int, if necessary
  if (ATindexOf(PDataDecls->Sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
    gsImplSortInt(PDataDecls);
  }
}

void gsImplFunctionSort(ATermAppl SortArrow, TDataDecls *PDataDecls)
{
  assert(gsIsSortArrow(SortArrow));
  //Declare operations for sort SortArrow
  PDataDecls->Ops = ATconcat(ATmakeList(3,
      (ATerm) gsMakeOpIdEq(SortArrow),
      (ATerm) gsMakeOpIdNeq(SortArrow),
      (ATerm) gsMakeOpIdIf(SortArrow)
    ), PDataDecls->Ops);
  //Declare data equations for sort SortArrow
  ATermAppl fSortArrow = gsMakeDataVarId(gsString2ATermAppl("f"), SortArrow);
  ATermAppl gSortArrow = gsMakeDataVarId(gsString2ATermAppl("g"), SortArrow);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList fl = ATmakeList1((ATerm) fSortArrow);
  ATermList fgl = ATmakeList2((ATerm) fSortArrow, (ATerm) gSortArrow);
  ATermList bfl = ATmakeList2((ATerm) b, (ATerm) fSortArrow);
  PDataDecls->DataEqns = ATconcat(ATmakeList(5,
      //equality (SortArrow -> SortArrow -> Bool)
      (ATerm) gsMakeDataEqn(fl, nil,
        gsMakeDataExprEq(fSortArrow, fSortArrow), t),
      //inequality (SortArrow -> SortArrow -> Bool)
      (ATerm) gsMakeDataEqn(fl,nil,
        gsMakeDataExprNeq(fSortArrow, fSortArrow), 
        gsMakeDataExprNot(gsMakeDataExprEq(fSortArrow, fSortArrow))),
      //conditional (Bool -> SortArrow -> SortArrow -> SortArrow)
      (ATerm) gsMakeDataEqn(fgl,nil,
        gsMakeDataExprIf(t, fSortArrow, gSortArrow),
        fSortArrow),
      (ATerm) gsMakeDataEqn(fgl,nil,
        gsMakeDataExprIf(f, fSortArrow, gSortArrow),
        gSortArrow),
      (ATerm) gsMakeDataEqn(bfl,nil,
        gsMakeDataExprIf(b, fSortArrow, fSortArrow),
        fSortArrow)
    ), PDataDecls->DataEqns);
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

//bool gsIsLambdaOpId(ATermAppl DataExpr)
//{
//  if (gsIsOpId(DataExpr)) {
//    return strncmp(
//      gsLambdaPrefix,
//      ATgetName(ATgetAFun(ATAgetArgument(DataExpr, 0))),
//      strlen(gsLambdaPrefix)) == 0;
//  } else {
//    return false;
//  }
//}

ATermAppl gsFoldSortRefs(ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  //get sort declarations
  ATermAppl SortSpec = ATAgetArgument(Spec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  //split sort declarations in sort id's and sort references
  ATermList SortIds = NULL;
  ATermList SortRefs = NULL;
  gsSplitSortDecls(SortDecls, &SortIds, &SortRefs);
  //fold sort references in the sort references themselves 
  SortRefs = gsFoldSortRefsInSortRefs(SortRefs);
  //substitute sort references in the rest of Spec, i.e.
  //(a) remove sort references from Spec
  SortDecls = SortIds;
  SortSpec = ATsetArgument(SortSpec, (ATerm) SortDecls, 0);
  Spec = ATsetArgument(Spec, (ATerm) SortSpec, 0);
  //(b) build substitution table
  ATermTable Substs = ATtableCreate(2*ATgetLength(SortRefs),50);
  ATermList l = SortRefs;
  while (!ATisEmpty(l)) {
    ATermAppl SortRef = ATAgetFirst(l);
    //add substitution for SortRef
    ATermAppl LHS = gsMakeSortId(ATAgetArgument(SortRef, 0));
    ATermAppl RHS = ATAgetArgument(SortRef, 1);
    if (gsIsSortId(RHS) || gsIsSortArrowProd(RHS) || gsIsSortArrow(RHS)) {
      //add forward substitution
      ATtablePut(Substs, (ATerm) LHS, (ATerm) RHS);
    } else {
      //add backward substitution
      ATtablePut(Substs, (ATerm) RHS, (ATerm) LHS);
    }
    l = ATgetNext(l);
  }
  //(c) perform substitutions until the specification becomes stable
  ATermAppl NewSpec = Spec;
  do {
    gsDebugMsg("substituting sort references in specification\n");
    Spec = NewSpec;
    NewSpec = (ATermAppl) gsSubstValuesTable(Substs, (ATerm) Spec, true);
  } while (!ATisEqual(NewSpec, Spec));
  ATtableDestroy(Substs);
  //add the removed sort references back to Spec
  SortDecls = ATconcat(SortIds, SortRefs);
  SortSpec = ATsetArgument(SortSpec, (ATerm) SortDecls, 0);
  Spec = ATsetArgument(Spec, (ATerm) SortSpec, 0);
  return Spec;
}

ATermList gsFoldSortRefsInSortRefs(ATermList SortRefs)
{
  //fold sort references in SortRefs by means of repeated forward and backward
  //substitution
  ATermList NewSortRefs = SortRefs;
  int n = ATgetLength(SortRefs);
  //perform substitutions until the list of sort references becomes stable
  do {
    SortRefs = NewSortRefs;
    gsDebugMsg("SortRefs contains the following sort references:\n%P",
      gsMakeSortSpec(SortRefs));
    //perform substitutions implied by sort references in NewSortRefs to the
    //other elements in NewSortRefs
    for (int i = 0; i < n; i++) {
      ATermAppl SortRef = ATAelementAt(NewSortRefs, i);
      //turn SortRef into a substitution
      ATermAppl LHS = gsMakeSortId(ATAgetArgument(SortRef, 0));
      ATermAppl RHS = ATAgetArgument(SortRef, 1);
      ATermAppl Subst;
      if (gsIsSortId(RHS) || gsIsSortArrowProd(RHS) || gsIsSortArrow(RHS)) {
        //make forward substitution
        Subst = gsMakeSubst_Appl(LHS, RHS);
      } else {
        //make backward substitution
        Subst = gsMakeSubst_Appl(RHS, LHS);
      }
      gsDebugMsg("performing substition %P  :=  %P\n",
        ATgetArgument(Subst, 0), ATgetArgument(Subst, 1));
      //perform Subst on all elements of NewSortRefs except for the i'th
      ATermList Substs = ATmakeList1((ATerm) Subst);
      for (int j = 0; j < n; j++) {
        if (i != j) {
          ATermAppl OldSortRef = ATAelementAt(NewSortRefs, j);
          ATermAppl NewSortRef = gsSubstValues_Appl(Substs, OldSortRef, true);
          if (!ATisEqual(NewSortRef, OldSortRef)) {
            NewSortRefs = ATreplace(NewSortRefs, (ATerm) NewSortRef, j);
          }
        }
      }
    }
    gsDebugMsg("\n");
  } while (!ATisEqual(NewSortRefs, SortRefs));
  //remove self references
  ATermList l = ATmakeList0();
  while (!ATisEmpty(SortRefs)) {
    ATermAppl SortRef = ATAgetFirst(SortRefs);
    if (!ATisEqual(gsMakeSortId(ATAgetArgument(SortRef, 0)),
      ATAgetArgument(SortRef, 1)))
    {
      l = ATinsert(l, (ATerm) SortRef);
    }
    SortRefs = ATgetNext(SortRefs);
  }
  SortRefs = ATreverse(l);
  gsDebugMsg("SortRefs, after removing self references:\n%P",
    gsMakeSortSpec(SortRefs));
  return SortRefs;
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
    //if RHS is the first occurrence of an implementation of a type constructor
    //at the rhs of a sort reference, add RHS := LHS; otherwise add LHS := RHS
    ATermAppl Subst;
    if (gsIsStructSortId(RHS) || gsIsListSortId(RHS) || gsIsSetSortId(RHS) ||
      gsIsBagSortId(RHS))
    {
      Subst = gsMakeSubst_Appl(RHS, LHS);
    } else {
      Subst = gsMakeSubst_Appl(LHS, RHS);
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

ATermAppl gsImplFunctionSorts(ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  //implement function sorts occurring in Spec
  ATermList FuncSorts = gsGetFunctionSorts(Spec);
  TDataDecls DataDecls;
  DataDecls.Sorts = ATmakeList0();
  DataDecls.ConsOps = ATmakeList0();
  DataDecls.Ops = ATmakeList0();
  DataDecls.DataEqns = ATmakeList0();
  while (!ATisEmpty(FuncSorts))
  {
    gsImplFunctionSort(ATAgetFirst(FuncSorts), &DataDecls);
    FuncSorts = ATgetNext(FuncSorts);
  }
  Spec = gsAddDataDecls(Spec, DataDecls);
  return Spec;
}

ATermAppl gsImplementData(ATermAppl Spec)
{
  int occ = gsCount((ATerm) gsMakeUnknown(), (ATerm) Spec);
  if (occ > 0) {
    gsErrorMsg("specification contains %d unknown type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }
  Spec = gsFoldSortRefs(Spec);
  Spec = gsImplExprs(Spec);
  Spec = gsImplSortRefs(Spec);
  Spec = gsImplFunctionSorts(Spec);
  return Spec;
}

#ifdef __cplusplus
}
#endif
