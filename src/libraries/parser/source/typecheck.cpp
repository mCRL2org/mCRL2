#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "typecheck.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"


// Static data 
// system constants and functions 
typedef struct { 
  ATermTable constants;		//name -> Set(sort expression)
  ATermTable functions;		//name -> Set(sort expression)
} gsSystem;
static gsSystem gssystem;

// the static context of the spec will be checked and used, not transformed
typedef struct { 
  ATermIndexedSet basic_sorts;	
  ATermTable defined_sorts;	//name -> sort expression
  ATermTable constants;		//name -> Set(sort expression)
  ATermTable functions;		//name -> Set(sort expression)
  ATermTable actions;	        //name -> Set(List(sort expression)) because of action polymorphism
  //ATermIndexedSet typedactions;	//name#type
  ATermTable processes;	        //name -> Set(List(sort expression)) bacause of process polymorphism
  //ATermIndexedSet typedprocesses: use keys of body.proc_pars
} Context;
static Context context;

// the body may be transformed
typedef struct { 
  ATermList equations;	
  ATermTable proc_pars;	        //name#type -> List(Vars)
  ATermTable proc_bodies;	//name#type -> rhs 
  //ATermAppl init;             //in the hash tables proc_pars and proc_bodies with key "init#[]" (beware when writing)
} Body;
static Body body;

// Static function declarations
static void gstcDataInit(void);
static void gstcDataDestroy(void);
static ATbool gstcReadInSorts (ATermList);
static ATbool gstcReadInConstructors();
static ATbool gstcReadInFuncs(ATermList);
static ATbool gstcReadInActs (ATermList);
static ATbool gstcReadInProcsAndInit (ATermList, ATermAppl);

static ATbool gstcTransformVarConsTypeData(void);
static ATbool gstcTransformActProcVarConst(void);

static ATermList gstcWriteProcs(ATermList);

static ATbool gstcInTypesA(ATermAppl, ATermList);
static ATbool gstcEqTypesA(ATermAppl, ATermAppl);
static ATbool gstcInTypesL(ATermList, ATermList);
static ATbool gstcEqTypesL(ATermList, ATermList);

static ATbool gstcIsSortDeclared(ATermAppl SortName);
static ATbool gstcIsSortExprDeclared(ATermAppl SortExpr);
static ATbool gstcIsSortExprListDeclared(ATermList SortExprList);
static ATbool gstcReadInSortStruct(ATermAppl);
static ATbool gstcAddConstant(ATermAppl, ATermAppl, const char*);
static ATbool gstcAddFunction(ATermAppl, ATermAppl, const char*);
static void gstcAddSystemConstant(ATermAppl, ATermAppl);
static void gstcAddSystemFunctionProd(ATermAppl, ATermAppl);

static void gstcATermTableCopy(ATermTable Vars, ATermTable CopyVars);

static ATermTable gstcAddVars2Table(ATermTable,ATermList);
static ATermTable gstcRemoveVars(ATermTable Vars, ATermList VarDecls);
static ATbool gstcVarsUnique(ATermList VarDecls);
static ATermAppl gstcRewrActProc(ATermTable, ATermAppl);
static inline ATermAppl gstcMakeActionOrProc(ATbool, ATermAppl, ATermList, ATermList);
static ATermAppl gstcTraverseActProcVarConstP(ATermTable, ATermAppl);
static ATermAppl gstcTraverseVarConsTypeD(ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl *, ATermAppl, ATermTable FreeVars=NULL);
static ATermAppl gstcTraverseVarConsTypeDN(int, ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl* , ATermAppl, ATermTable FreeVars=NULL);

static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type);

static inline ATbool gstcIsPos(ATermAppl Number) {char c=ATgetName(ATgetAFun(Number))[0]; return (ATbool) (isdigit(c) && c>'0');}
static inline ATbool gstcIsNat(ATermAppl Number) {return (ATbool) isdigit(ATgetName(ATgetAFun(Number))[0]);}

static inline ATermAppl gstcMakeSortArrowProd1(ATermAppl Source, ATermAppl Target){
  return gsMakeSortArrowProd(ATmakeList1((ATerm)Source),Target);
}

static inline ATermAppl gstcMakeSortArrowProd2(ATermAppl Source1, ATermAppl Source2, ATermAppl Target){
  return gsMakeSortArrowProd(ATmakeList2((ATerm)Source1,(ATerm)Source2),Target);
}

static inline ATermAppl gstcMakeSortArrowProd3(ATermAppl Source1, ATermAppl Source2,ATermAppl Source3, ATermAppl Target){
  return gsMakeSortArrowProd(ATmakeList3((ATerm)Source1,(ATerm)Source2,(ATerm)Source3),Target);
}

static inline ATermAppl gstcMakeOpIdPos2Nat(void){
  return gsMakeOpId(gsMakeOpIdNamePos2Nat(),gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdNat()));
}

static inline ATermAppl gstcMakeOpIdPos2Int(void){
  return gsMakeOpId(gsMakeOpIdNamePos2Int(),gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdInt()));
}

static inline ATermAppl gstcMakeOpIdPos2Real(void){
  return gsMakeOpId(gsMakeOpIdNamePos2Real(),gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdReal()));
}

static inline ATermAppl gstcMakeOpIdNat2Int(void){
  return gsMakeOpId(gsMakeOpIdNameNat2Int(),gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
}

static inline ATermAppl gstcMakeOpIdNat2Real(void){
  return gsMakeOpId(gsMakeOpIdNameNat2Real(),gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdReal()));
}

static inline ATermAppl gstcMakeOpIdInt2Real(void){
  return gsMakeOpId(gsMakeOpIdNameInt2Real(),gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdReal()));
}

static inline ATermAppl gstcMakeOpIdSet2Bag(ATermAppl Type){
    return gsMakeOpId(gsMakeOpIdNameSet2Bag(),gstcMakeSortArrowProd1(gsMakeSortSet(Type),gsMakeSortBag(Type)));
}

static inline ATermAppl INIT_KEY(void){return gsMakeProcVarId(gsString2ATermAppl("init"),ATmakeList0());}

static inline ATermAppl gstcMakeNotInferred(ATermList PossibleTypes){
  assert(PossibleTypes);
  return ATmakeAppl1(ATmakeAFun("notInferred",1,ATtrue),(ATerm)PossibleTypes);
}

static inline bool gstcIsNotInferred(ATermAppl SortTerm){
  return (ATgetAFun(SortTerm)==ATmakeAFun("notInferred",1,ATtrue));
}

static ATermAppl gstcUpCastNumericType(ATermAppl NeededType, ATermAppl Type, ATermAppl *Par);
static ATermList gstcGetNotInferredList(ATermList TypeListList);
static ATermList gstcAdjustNotInferredList(ATermList TypeList, ATermList TypeListList);
static ATbool gstcIsNotInferredL(ATermList TypeListList);
static ATbool gstcIsTypeAllowedA(ATermAppl Type, ATermAppl PosType);
static ATbool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList);
static ATermAppl gstcUnwindType(ATermAppl Type);
static ATermAppl gstcUnSet(ATermAppl PosType);
static ATermAppl gstcUnBag(ATermAppl PosType);
static ATermAppl gstcUnList(ATermAppl PosType);
static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType);
static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2);
static ATermList gstcGetVarTypes(ATermList VarDecls);
static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType);
static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList);
static ATbool gstcHasUnknown(ATermAppl Type);
static ATbool gstcIsNumericType(ATermAppl Type);
static ATermAppl gstcExpandNumTypesUp(ATermAppl Type);
static ATermAppl gstcExpandNumTypesDown(ATermAppl Type);
static ATermAppl gstcMinType(ATermList TypeList);
static ATbool gstcMActIn(ATermList MAct, ATermList MActs);
static ATbool gstcMActEq(ATermList MAct1, ATermList MAct2);
static ATbool gstcMActSubEq(ATermList MAct1, ATermList MAct2);
static ATermAppl gstcUnifyMinType(ATermAppl Type1, ATermAppl Type2);
static ATermAppl gstcMatchIf(ATermAppl Type);
static ATermAppl gstcMatchEqNeq(ATermAppl Type);
static ATermAppl gstcMatchListOpCons(ATermAppl Type);
static ATermAppl gstcMatchListOpSnoc(ATermAppl Type);
static ATermAppl gstcMatchListOpConcat(ATermAppl Type);
static ATermAppl gstcMatchListOpEltAt(ATermAppl Type);
static ATermAppl gstcMatchListOpHead(ATermAppl Type);
static ATermAppl gstcMatchListOpTail(ATermAppl Type);
static ATermAppl gstcMatchSetOpSet2Bag(ATermAppl Type);
static ATermAppl gstcMatchListSetBagOpIn(ATermAppl Type);
static ATermAppl gstcMatchSetBagOpSubEq(ATermAppl Type);
static ATermAppl gstcMatchSetBagOpUnionDiffIntersect(ATermAppl Type);
static ATermAppl gstcMatchSetOpSetCompl(ATermAppl Type);
static ATermAppl gstcMatchBagOpBag2Set(ATermAppl Type);
static ATermAppl gstcMatchBagOpBagCount(ATermAppl Type);
// end prototypes


static ATermAppl gstcFoldSortRefs(ATermAppl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     type checking
//Ret: Spec in which all sort references are maximally folded, i.e.:
//     - sort references to SortId's and SortArrow's are removed from the
//       rest of Spec (including the other sort references) by means of
//       forward substitition
//     - other sort references are removed from the rest of Spec by means of
//       backward substitution
//     - self references are removed, i.e. sort references of the form A = A

static ATermList gstcFoldSortRefsInSortRefs(ATermList SortRefs);
//Pre: SortRefs is a list of sort references
//Ret: SortRefs in which all sort references are maximally folded

static void gstcSplitSortDecls(ATermList SortDecls, ATermList *PSortIds,
  ATermList *PSortRefs);
//Pre: SortDecls is a list of SortId's and SortRef's
//Post:*PSortIds and *PSortRefs contain the SortId's and SortRef's from
//     SortDecls, in the same order

void gstcSplitSortDecls(ATermList SortDecls, ATermList *PSortIds,
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

ATermAppl gstcFoldSortRefs(ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  //get sort declarations
  ATermAppl SortSpec = ATAgetArgument(Spec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  //split sort declarations in sort id's and sort references
  ATermList SortIds = NULL;
  ATermList SortRefs = NULL;
  gstcSplitSortDecls(SortDecls, &SortIds, &SortRefs);
  //fold sort references in the sort references themselves 
  SortRefs = gstcFoldSortRefsInSortRefs(SortRefs);
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

ATermList gstcFoldSortRefsInSortRefs(ATermList SortRefs)
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

// Main function
ATermAppl gsTypeCheck (ATermAppl input){	
  ATermAppl Result=NULL;
  gsDebugMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking read-in phase started\n");
  
  if(gstcReadInSorts(ATLgetArgument(ATAgetArgument(input,0),0))) {
  // Check soorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if(gstcReadInConstructors()) {
  if(gstcReadInFuncs(ATconcat(ATLgetArgument(ATAgetArgument(input,1),0),
			       ATLgetArgument(ATAgetArgument(input,2),0)))) {
  body.equations=ATLgetArgument(ATAgetArgument(input,3),0);
  if(gstcReadInActs(ATLgetArgument(ATAgetArgument(input,4),0))) {
  if(gstcReadInProcsAndInit(ATLgetArgument(ATAgetArgument(input,5),0),
			     ATAgetArgument(ATAgetArgument(input,6),1))) {
  gsDebugMsg ("type checking read-in phase finished\n");
  
  gsDebugMsg ("type checking transform ActProc+VarConst phase started\n");
  if(gstcTransformVarConsTypeData()){
  if(gstcTransformActProcVarConst()){
  gsDebugMsg ("type checking transform ActProc+VarConst phase finished\n");

  Result=ATsetArgument(input,(ATerm)gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(Result,(ATerm)gsMakeProcEqnSpec(gstcWriteProcs(ATLgetArgument(ATAgetArgument(input,5),0))),5);
  Result=ATsetArgument(Result,(ATerm)gsMakeInit(ATmakeList0(),
    ATAtableGet(body.proc_bodies,(ATerm)INIT_KEY())),6);

  Result=gstcFoldSortRefs(Result);

  gsDebugMsg ("type checking phase finished\n");
  }}}}}}}

  if (Result != NULL) {
    gsDebugMsg("return %T\n", Result);
  } 
  else {
    gsDebugMsg("return NULL\n");
  }
  gstcDataDestroy();
  return Result;
}

// ============ Static functions
// ========= main processing functions
void gstcDataInit(void){
  gssystem.constants=ATtableCreate(63,50);
  gssystem.functions=ATtableCreate(63,50);
  context.basic_sorts=ATindexedSetCreate(63,50);
  context.defined_sorts=ATtableCreate(63,50);
  context.constants=ATtableCreate(63,50);
  context.functions=ATtableCreate(63,50);
  context.actions=ATtableCreate(63,50);
  context.processes=ATtableCreate(63,50);
  body.proc_pars=ATtableCreate(63,50);
  body.proc_bodies=ATtableCreate(63,50);

  //Creation of operation identifiers for system defined operations.
  //Bool
  gstcAddSystemConstant(gsMakeOpIdNameTrue(),gsMakeSortIdBool());
  gstcAddSystemConstant(gsMakeOpIdNameFalse(),gsMakeSortIdBool());
  gstcAddSystemFunctionProd(gsMakeOpIdNameNot(),
			    gstcMakeSortArrowProd1(gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAnd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdBool(),gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameOr(),
			    gstcMakeSortArrowProd2(gsMakeSortIdBool(),gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameImp(),
			    gstcMakeSortArrowProd2(gsMakeSortIdBool(),gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameEq(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeUnknown(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNeq(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeUnknown(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameIf(),
			    gstcMakeSortArrowProd3(gsMakeSortIdBool(),gsMakeUnknown(),gsMakeUnknown(),gsMakeUnknown()));
  //Numbers
  gstcAddSystemFunctionProd(gsMakeOpIdNamePos2Nat(),
			gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNamePos2Int(),
			gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNamePos2Real(),
			gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNat2Pos(),
			gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNat2Int(),
			gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNat2Real(),
			gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameInt2Pos(),
			gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameInt2Nat(),
			gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameInt2Real(),
			gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameReal2Pos(),
			gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameReal2Nat(),
			gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameReal2Int(),
			gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameLT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameGTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameGTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameGTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameGTE(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameGT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameGT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameGT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameGT(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdInt(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdInt(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMax(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameMin(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMin(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMin(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMin(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameAbs(),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAbs(),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAbs(),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAbs(),
			    gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameNeg(),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNeg(),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNeg(),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameNeg(),
			    gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSucc(),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSucc(),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSucc(),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSucc(),
			    gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNamePred(),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNamePred(),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNamePred(),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNamePred(),
			    gstcMakeSortArrowProd1(gsMakeSortIdReal(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAdd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAdd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAdd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAdd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAdd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameAdd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubt(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubt(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubt(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubt(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdReal()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMult(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMult(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMult(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMult(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNameDiv(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameDiv(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameDiv(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMod(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMod(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameMod(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameExp(),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameExp(),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameExp(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameExp(),
			    gstcMakeSortArrowProd2(gsMakeSortIdReal(),gsMakeSortIdNat(),gsMakeSortIdReal()));
//Lists
  gstcAddSystemConstant(gsMakeOpIdNameEmptyList(),gsMakeSortList(gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameEltIn(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortList(gsMakeUnknown()),gsMakeSortExprBool()));  gstcAddSystemFunctionProd(gsMakeOpIdNameListSize(),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameCons(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSnoc(),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeUnknown(),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameConcat(),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameEltAt(),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeSortExprNat(),gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameHead(),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameTail(),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameRHead(),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameRTail(),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
	    
//Sets
  gstcAddSystemFunctionProd(gsMakeOpIdNameSet2Bag(),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemConstant(gsMakeOpIdNameEmptySet(),gsMakeSortSet(gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameEltIn(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubSetEq(),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubSet(),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetUnion(),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetDiff(),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetIntersect(),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetCompl(),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));


//Bags
  gstcAddSystemFunctionProd(gsMakeOpIdNameBag2Set(),
			    gstcMakeSortArrowProd1(gsMakeSortBag(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemConstant(gsMakeOpIdNameEmptyBag(),gsMakeSortBag(gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameEltIn(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameCount(),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubBagEq(),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubBag(),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagUnion(),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagDiff(),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagIntersect(),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
}

void gstcDataDestroy(void){
  ATtableDestroy(gssystem.constants);
  ATtableDestroy(gssystem.functions);
  ATindexedSetDestroy(context.basic_sorts);
  ATtableDestroy(context.defined_sorts);
  ATtableDestroy(context.constants);
  ATtableDestroy(context.functions);
  ATtableDestroy(context.actions);
  ATtableDestroy(context.processes);
  ATtableDestroy(body.proc_pars);
  ATtableDestroy(body.proc_bodies);
}
 
static ATbool gstcReadInSorts (ATermList Sorts){
  ATbool nnew;
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
    ATermAppl Sort=ATAgetFirst(Sorts);
    ATermAppl SortName=ATAgetArgument(Sort,0);
    if(ATisEqual(gsMakeSortIdBool(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Bool\n");
      return ATfalse;
    }				
    if(ATisEqual(gsMakeSortIdPos(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Pos\n");
      return ATfalse;
    }				
    if(ATisEqual(gsMakeSortIdNat(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Nat\n");
      return ATfalse;
    }				
    if(ATisEqual(gsMakeSortIdInt(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Int\n");
      return ATfalse;
    }				
    if(ATisEqual(gsMakeSortIdReal(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Real\n");
      return ATfalse;
    }				
    if(ATindexedSetGetIndex(context.basic_sorts, (ATerm)SortName)>=0 
       || ATAtableGet(context.defined_sorts, (ATerm)SortName)){

      gsErrorMsg("double declaration of sort %P\n",SortName);
      return ATfalse;
    }				
    if(gsIsSortId(Sort)) ATindexedSetPut(context.basic_sorts, (ATerm)SortName, &nnew);
    else
      if(gsIsSortRef(Sort)){
	ATtablePut(context.defined_sorts, (ATerm)SortName, (ATerm)ATAgetArgument(Sort,1));
	gsDebugMsg("recognized %T %T\n",SortName,(ATerm)ATAgetArgument(Sort,1));    	
      }
      else assert(0);
  }
 
  return Result;
}  

static ATbool gstcReadInConstructors(){
  for(ATermList Sorts=ATtableKeys(context.defined_sorts);!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
    ATermAppl SortExpr=ATAtableGet(context.defined_sorts,ATgetFirst(Sorts));
    if(!gstcIsSortExprDeclared(SortExpr)) return ATfalse;
    if(!gstcReadInSortStruct(SortExpr)) return ATfalse;
  }
  return ATtrue;
} 

static ATbool gstcReadInFuncs(ATermList Funcs){
  gsDebugMsg("Start Read-in Func\n");    
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Funcs);Funcs=ATgetNext(Funcs)){
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=ATAgetArgument(Func,0);
    ATermAppl FuncType=ATAgetArgument(Func,1);
    
    if(!gstcIsSortExprDeclared(FuncType)) { return ATfalse; }
    //if FuncType is a defined function sort, unwind it
    { ATermAppl NewFuncType;
      if(gsIsSortId(FuncType) 
	 && (NewFuncType=ATAtableGet(context.defined_sorts,(ATerm)ATAgetArgument(FuncType,0))) 
	 && gsIsSortArrowProd(NewFuncType))
	FuncType=NewFuncType;
    }
    
    if(gsIsSortArrowProd(FuncType)){
      if(!gstcAddFunction(FuncName,FuncType,"function")) { return ATfalse; }
    }
    else{
      if(!gstcAddConstant(FuncName,FuncType,"constant")) { gsErrorMsg("could not add constant\n"); return ATfalse; }
    }
    gsDebugMsg("Read-in Func %T, Types %T\n",FuncName,FuncType);    
  }
  
  return Result;
} 

static ATbool gstcReadInActs (ATermList Acts){
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Acts);Acts=ATgetNext(Acts)){
    ATermAppl Act=ATAgetFirst(Acts);
    ATermAppl ActName=ATAgetArgument(Act,0);
    ATermList ActType=ATLgetArgument(Act,1);
    
    if(!gstcIsSortExprListDeclared(ActType)) { return ATfalse; }

    ATermList Types=ATLtableGet(context.actions, (ATerm)ActName);
    if(!Types){
      Types=ATmakeList1((ATerm)ActType);
    }
    else{
      // the table context.actions contains a list of types for each
      // action name. We need to check if there is already such a type 
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ActType, Types)){
	gsErrorMsg("double declaration of action %P\n", ActName);
	return ATfalse;
      }
      else{
	Types=ATappend(Types,(ATerm)ActType);
      }
    }
    ATtablePut(context.actions,(ATerm)ActName,(ATerm)Types);
  }
 
  return Result;
}

static ATbool gstcReadInProcsAndInit (ATermList Procs, ATermAppl Init){
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Procs);Procs=ATgetNext(Procs)){
    ATermAppl Proc=ATAgetFirst(Procs);
    ATermAppl ProcName=ATAgetArgument(ATAgetArgument(Proc,1),0);
    
    if(ATLtableGet(context.actions, (ATerm)ProcName)){
      gsErrorMsg("declaration of both process and action %P\n", ProcName);
      return ATfalse;
    }	

    ATermList ProcType=ATLgetArgument(ATAgetArgument(Proc,1),1);

    if(!gstcIsSortExprListDeclared(ProcType)) { return ATfalse; }

    ATermList Types=ATLtableGet(context.processes,(ATerm)ProcName);
    if(!Types){
      Types=ATmakeList1((ATerm)ProcType);
    }
    else{
      // the table context.processes contains a list of types for each
      // process name. We need to check if there is already such a type 
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ProcType, Types)){
	gsErrorMsg("double declaration of process %P\n", ProcName);
	return ATfalse;
      }
      else{
	Types=ATappend(Types,(ATerm)ProcType);
      }
    }
    ATtablePut(context.processes,(ATerm)ProcName,(ATerm)Types);

    //check that all formal parameters of the process are unique.
    ATermList ProcVars=ATLgetArgument(Proc,2);
    if(!gstcVarsUnique(ProcVars)){ gsErrorMsg("the formal variables in process %P are not unique\n",Proc); return ATfalse;}

    ATtablePut(body.proc_pars,(ATerm)ATAgetArgument(Proc,1),(ATerm)ATLgetArgument(Proc,2));
    ATtablePut(body.proc_bodies,(ATerm)ATAgetArgument(Proc,1),(ATerm)ATAgetArgument(Proc,3));
    gsDebugMsg("Read-in Proc Name %T, Types %T\n",ProcName,Types);    
  }
  ATtablePut(body.proc_pars,(ATerm)INIT_KEY(),(ATerm)ATmakeList0());
  ATtablePut(body.proc_bodies,(ATerm)INIT_KEY(),(ATerm)Init);
 
  return Result;
} 

static ATermList gstcWriteProcs(ATermList oldprocs){
  ATermList Result=ATmakeList0();
  for(ATermList ProcVars=oldprocs;!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetArgument(ATAgetFirst(ProcVars),1);
    if(ProcVar==INIT_KEY()) continue;
    Result=ATinsert(Result,(ATerm)gsMakeProcEqn(ATmakeList0(),
                                                ProcVar,
						ATLtableGet(body.proc_pars,(ATerm)ProcVar),
						ATAtableGet(body.proc_bodies,(ATerm)ProcVar)
						)
		    );
  }
  return ATreverse(Result);
}

static ATbool gstcTransformVarConsTypeData(void){
  ATbool Result=ATtrue;
  ATermTable DeclaredVars=ATtableCreate(63,50);
  ATermTable FreeVars=ATtableCreate(63,50);
  
  //data terms in equations
  ATermList NewEqns=ATmakeList0();
  bool b = true;
  for(ATermList Eqns=body.equations;!ATisEmpty(Eqns);Eqns=ATgetNext(Eqns)){
    ATermAppl Eqn=ATAgetFirst(Eqns);
    ATermList VarList=ATLgetArgument(Eqn,0);

    if(!gstcVarsUnique(VarList)){ b = false; gsErrorMsg("the variables in equation declaration %P are not unique\n",VarList,Eqn); break;}

    DeclaredVars=gstcAddVars2Table(DeclaredVars,VarList);
    if(!DeclaredVars){ b = false; break; }
    ATermAppl Left=ATAgetArgument(Eqn,2);
    ATermAppl LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,gsMakeUnknown(),FreeVars);
    if(!LeftType){ b = false; gsErrorMsg("the previous error occurred while typechecking %P as left hand side of equation %P\n",Left,Eqn); break;}
    
    ATermAppl Cond=ATAgetArgument(Eqn,1);
    if(!gsIsNil(Cond) && !(gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Cond,gsMakeSortIdBool()))){ b = false; break; }
    ATermAppl Right=ATAgetArgument(Eqn,3);
    ATermAppl RightType=gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Right,LeftType);
    if(!RightType){ b = false; gsErrorMsg("the previous error occurred while typechecking %P as right hand side of equation %P\n",Right,Eqn); break; }

    //If the types are not uniquly the same now: do once more:
    if(!gstcEqTypesA(LeftType,RightType)){
      gsDebugMsg("Doing again for the equation %P, LeftType: %P, RightType: %P\n",Eqn,LeftType,RightType);
      ATermAppl Type=gstcTypeMatchA(LeftType,RightType);
      if(!Type){gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P do not match\n",LeftType,RightType,Eqn); b = false; break; }
      
      Left=ATAgetArgument(Eqn,2);
      ATtableReset(FreeVars);
      LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,Type,FreeVars);
      if(!LeftType){ b = false; gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P do not match\n",LeftType,RightType,Eqn); break; }
    
      Right=ATAgetArgument(Eqn,3);
      RightType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Right,LeftType,FreeVars);
      if(!RightType){ b = false; gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P do not match\n",LeftType,RightType,Eqn); break; }
      
      Type=gstcTypeMatchA(LeftType,RightType);
      if(!Type){gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P do not match\n",LeftType,RightType,Eqn); b = false; break; }
      if(gstcHasUnknown(Type)){gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P cannot be uniquily determined\n",LeftType,RightType,Eqn); b = false; break; }
    }
    ATtableReset(DeclaredVars);
    NewEqns=ATinsert(NewEqns,(ATerm)gsMakeDataEqn(VarList,Cond,Left,Right));
  }
  if ( b ) { body.equations=ATreverse(NewEqns); }
  
  ATtableDestroy(FreeVars);
  ATtableDestroy(DeclaredVars);
  return b?Result:ATfalse;
} 
 
static ATbool gstcTransformActProcVarConst(void){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);

  //process and data terms in processes and init
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    ATtableReset(Vars);
    Vars=gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,(ATerm)ProcVar));
    if(!Vars){ Result = ATfalse; break; }
    ATermAppl NewProcTerm=gstcTraverseActProcVarConstP(Vars,ATAtableGet(body.proc_bodies,(ATerm)ProcVar));
    if(!NewProcTerm){ Result = ATfalse; break; }
    ATtablePut(body.proc_bodies,(ATerm)ProcVar,(ATerm)NewProcTerm);
  } 

  ATtableDestroy(Vars);
  return Result;
}

// ======== Auxiliary functions
static ATbool gstcInTypesA(ATermAppl Type, ATermList Types){
  for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    if(gstcEqTypesA(Type,ATAgetFirst(Types))) return ATtrue;
  return ATfalse;
}

static ATbool gstcEqTypesA(ATermAppl Type1, ATermAppl Type2){
  if(ATisEqual(Type1, Type2)) return ATtrue;

  if(!Type1 || !Type2) return ATfalse;

  return ATisEqual(gstcUnwindType(Type1),gstcUnwindType(Type2));
}

static ATbool gstcInTypesL(ATermList Type, ATermList Types){
  for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    if(gstcEqTypesL(Type,ATLgetFirst(Types))) return ATtrue;
  return ATfalse;
}

static ATbool gstcEqTypesL(ATermList Type1, ATermList Type2){
  if(ATisEqual(Type1, Type2)) return ATtrue;
  if(!Type1 || !Type2) return ATfalse;
  if(ATgetLength(Type1)!=ATgetLength(Type2)) return ATfalse;
  for(;!ATisEmpty(Type1);Type1=ATgetNext(Type1),Type2=ATgetNext(Type2))
    if(!gstcEqTypesA(ATAgetFirst(Type1),ATAgetFirst(Type2))) return ATfalse;
  return ATtrue;
}

static ATbool gstcIsSortDeclared(ATermAppl SortName){

  gsDebugMsg("gstcIsSortDeclared: SortName %P\n",SortName);    

  if(ATisEqual(gsMakeSortIdBool(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdPos(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdNat(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdInt(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdReal(),gsMakeSortId(SortName))) return ATtrue;
  if(ATindexedSetGetIndex(context.basic_sorts, (ATerm)SortName)>=0) return ATtrue;
  if(ATAtableGet(context.defined_sorts,(ATerm)SortName)) return ATtrue;
  return ATfalse;
}

static ATbool gstcIsSortExprDeclared(ATermAppl SortExpr){
  ATbool Result=ATtrue;

  if(gsIsSortId(SortExpr)){ 
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if(!gstcIsSortDeclared(SortName))
      {gsErrorMsg("basic or defined sort %P is not declared\n",SortName); return ATfalse; }
    return ATtrue;
  }

  if(gsIsSortList(SortExpr) || gsIsSortSet(SortExpr) || gsIsSortBag(SortExpr))
    return gstcIsSortExprDeclared(ATAgetArgument(SortExpr,0));
  
  if(gsIsSortArrowProd(SortExpr)){
    if(!gstcIsSortExprDeclared(ATAgetArgument(SortExpr,1))) return ATfalse;
    if(!gstcIsSortExprListDeclared(ATLgetArgument(SortExpr,0))) return ATfalse;
    return ATtrue;
  }
  
  if(gsIsSortStruct(SortExpr)){
    for(ATermList Constrs=ATLgetArgument(SortExpr,0);!ATisEmpty(Constrs);Constrs=ATgetNext(Constrs)){
      ATermAppl Constr=ATAgetFirst(Constrs);
    
      ATermList Projs=ATLgetArgument(Constr,1);
      for(;!ATisEmpty(Projs);Projs=ATgetNext(Projs)){
	ATermAppl Proj=ATAgetFirst(Projs);
	ATermAppl ProjSort=ATAgetArgument(Proj,1);
	
	// not to forget, recursive call for ProjSort ;-)
	if(!gstcIsSortExprDeclared(ProjSort)) {return ATfalse;}
      }
    }
    return ATtrue;
  }
  
  assert(0);
  return Result;
}

static ATbool gstcIsSortExprListDeclared(ATermList SortExprList){
  for(;!ATisEmpty(SortExprList);SortExprList=ATgetNext(SortExprList))
    if(!gstcIsSortExprDeclared(ATAgetFirst(SortExprList))) return ATfalse; 
  return ATtrue;
}


static ATbool gstcReadInSortStruct(ATermAppl SortExpr){
  ATbool Result=ATtrue;

  if(gsIsSortId(SortExpr)){ 
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if(!gstcIsSortDeclared(SortName))
      {gsErrorMsg("basic or defined sort %P is not declared\n",SortName);return ATfalse;}
    return ATtrue;
  }

  if(gsIsSortList(SortExpr) || gsIsSortSet(SortExpr) || gsIsSortBag(SortExpr))
    return gstcReadInSortStruct(ATAgetArgument(SortExpr,0));

  if(gsIsSortArrowProd(SortExpr)){
    if(!gstcReadInSortStruct(ATAgetArgument(SortExpr,1))) return ATfalse;
    for(ATermList Sorts=ATLgetArgument(SortExpr,0);!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
      if(!gstcReadInSortStruct(ATAgetFirst(Sorts))) return ATfalse;
    }
    return ATtrue;
  }
  
  if(gsIsSortStruct(SortExpr)){
    for(ATermList Constrs=ATLgetArgument(SortExpr,0);!ATisEmpty(Constrs);Constrs=ATgetNext(Constrs)){
      ATermAppl Constr=ATAgetFirst(Constrs);
    
      // recognizer -- if present -- a function from SortExpr to Bool
      ATermAppl Name=ATAgetArgument(Constr,2);
      if(!gsIsNil(Name) && 
	 !gstcAddFunction(Name,gsMakeSortArrowProd(ATmakeList1((ATerm)SortExpr),gsMakeSortExprBool()),"recognizer")) {return ATfalse;}
      
      // constructor type and projections
      ATermList Projs=ATLgetArgument(Constr,1);
      Name=ATAgetArgument(Constr,0);
      if(ATisEmpty(Projs)){
	if(!gstcAddConstant(Name,SortExpr,"constructor constant")){return ATfalse;}
	else continue;
      }
      
      ATermList ConstructorType=ATmakeList0();
      for(;!ATisEmpty(Projs);Projs=ATgetNext(Projs)){
	ATermAppl Proj=ATAgetFirst(Projs);
	ATermAppl ProjSort=ATAgetArgument(Proj,1);
	
	// not to forget, recursive call for ProjSort ;-)
	if(!gstcReadInSortStruct(ProjSort)) {return ATfalse;}

	ATermAppl ProjName=ATAgetArgument(Proj,0);
	if(!gsIsNil(ProjName) &&
	   !gstcAddFunction(ProjName,gsMakeSortArrowProd(ATmakeList1((ATerm)SortExpr),ProjSort),"projection")) {return ATfalse;}
	ConstructorType=ATinsert(ConstructorType,(ATerm)ProjSort);
      }
      if(!gstcAddFunction(Name,gsMakeSortArrowProd(ATreverse(ConstructorType),SortExpr),"constructor")) {return ATfalse;}
    }
    return ATtrue;
  }
  
  assert(0);
  return Result;
}

static ATbool gstcAddConstant(ATermAppl Name, ATermAppl Sort, const char* msg){
  ATbool Result=ATtrue;

  if(ATAtableGet(context.constants, (ATerm)Name) || ATLtableGet(context.functions, (ATerm)Name)){
    gsErrorMsg("double declaration of %s %P\n", msg, Name);
    return ATfalse;
  }

  if(ATAtableGet(gssystem.constants, (ATerm)Name) || ATLtableGet(gssystem.functions, (ATerm)Name)){
    gsErrorMsg("attempt to redeclare the system identifier with %s %P\n", msg, Name);
    return ATfalse;
  }
  
  ATtablePut(context.constants, (ATerm)Name, (ATerm)Sort);
  return Result;
}

static ATbool gstcAddFunction(ATermAppl Name, ATermAppl Sort, const char *msg){
  ATbool Result=ATtrue;

  //constants and functions can have the same names
  //  if(ATAtableGet(context.constants, (ATerm)Name)){
  //    ThrowMF("Double declaration of constant and %s %T\n", msg, Name);
  //  }

  if(ATAtableGet(gssystem.constants, (ATerm)Name) || ATLtableGet(gssystem.functions, (ATerm)Name)){
    gsErrorMsg("attempt to redeclare the system identifier with %s %P\n", msg, Name);
    return ATfalse;
  }

  ATermList Types=ATLtableGet(context.functions, (ATerm)Name);
  // the table context.functions contains a list of types for each
  // function name. We need to check if there is already such a type 
  // in the list. If so -- error, otherwise -- add
  if (Types && gstcInTypesA(Sort, Types)){
    gsErrorMsg("double declaration of %s %P\n", msg, Name);
    return ATfalse;
  }
  else{
    if (!Types) Types=ATmakeList0();
    Types=ATappend(Types,(ATerm)Sort);
    ATtablePut(context.functions,(ATerm)Name,(ATerm)Types);
  }
  gsDebugMsg("Read-in %s %T Type %T\n",msg,Name,Types);    
  return Result;
}

static void gstcAddSystemConstant(ATermAppl OpId, ATermAppl Type){
  ATermList Types=ATLtableGet(gssystem.constants, (ATerm)OpId);

  if (!Types) Types=ATmakeList0();
  Types=ATappend(Types,(ATerm)Type);
  ATtablePut(gssystem.constants,(ATerm)OpId,(ATerm)Types);
}

static void gstcAddSystemFunctionProd(ATermAppl OpId, ATermAppl Type){
  // Replace type in OpId with Type and add
  ATermList Types=ATLtableGet(gssystem.functions, (ATerm)OpId);

  if (!Types) Types=ATmakeList0();
  Types=ATappend(Types,(ATerm)Type);
  ATtablePut(gssystem.functions,(ATerm)OpId,(ATerm)Types);
}

static void gstcATermTableCopy(ATermTable Orig, ATermTable Copy){
  for(ATermList Keys=ATtableKeys(Orig);!ATisEmpty(Keys);Keys=ATgetNext(Keys)){
    ATerm Key=ATgetFirst(Keys);
    ATtablePut(Copy,Key,ATtableGet(Orig,Key));
  }
}


static ATbool gstcVarsUnique(ATermList VarDecls){
  ATbool Result=ATtrue;
  ATermIndexedSet Temp=ATindexedSetCreate(63,50);

  for(;!ATisEmpty(VarDecls);VarDecls=ATgetNext(VarDecls)){
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    ATbool nnew;
    ATindexedSetPut(Temp, (ATerm)VarName, &nnew);
    if(!nnew) {Result=ATfalse; goto final;}
  } 

 final:
  ATindexedSetDestroy(Temp);
  return Result;
}

static ATermTable gstcAddVars2Table(ATermTable Vars, ATermList VarDecls){
  for(;!ATisEmpty(VarDecls);VarDecls=ATgetNext(VarDecls)){
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    ATermAppl VarType=ATAgetArgument(VarDecl,1);
    //test the type
    if(!gstcIsSortExprDeclared(VarType)) return NULL;

    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    ATtablePut(Vars, (ATerm)VarName, (ATerm)VarType);
  } 

  return Vars;
}

static ATermTable gstcRemoveVars(ATermTable Vars, ATermList VarDecls){
  for(;!ATisEmpty(VarDecls);VarDecls=ATgetNext(VarDecls)){
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    //ATermAppl VarType=ATAgetArgument(VarDecl,1);
    
    ATtableRemove(Vars, (ATerm)VarName);
  } 

  return Vars;
}

static ATermAppl gstcRewrActProc(ATermTable Vars, ATermAppl ProcTerm){
  ATermAppl Result=NULL;
  ATermAppl Name=ATAgetArgument(ProcTerm,0);
  ATermList ParList;

  ATbool action;

  if((ParList=ATLtableGet(context.actions,(ATerm)Name))){
    action=ATtrue;
  }
  else{
    if((ParList=ATLtableGet(context.processes,(ATerm)Name))){
      action=ATfalse;
    }
    else{
      gsErrorMsg("action or process %P not declared\n", Name);
      return NULL;
    }
  }
  assert(!ATisEmpty(ParList));
  
  int nFactPars=ATgetLength(ATLgetArgument(ProcTerm,1));
  const char *msg=(action)?"action":"process";
  
  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    ATermList NewParList=ATmakeList0();
    for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
      ATermList Par=ATLgetFirst(ParList);
      if(ATgetLength(Par)==nFactPars) NewParList=ATinsert(NewParList,(ATerm)Par);
    }
    ParList=ATreverse(NewParList);
  }
 
  if(ATisEmpty(ParList)) {
    gsErrorMsg("no %s %P with %d parameters is declared (while typechecking %P)\n", msg, Name, nFactPars, ProcTerm);     
    return NULL;
  }

  if(ATgetLength(ParList)==1){
    Result=gstcMakeActionOrProc(action,Name,ATLgetFirst(ParList),ATLgetArgument(ProcTerm,1));
  }
  else{
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=gstcMakeActionOrProc(action,Name,gstcGetNotInferredList(ParList),ATLgetArgument(ProcTerm,1));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  ATermList PosTypeList=ATLgetArgument(ATAgetArgument(Result,0),1);
  
  ATermList NewPars=ATmakeList0();
  ATermList NewPosTypeList=ATmakeList0();
  for(ATermList Pars=ATLgetArgument(ProcTerm,1);!ATisEmpty(Pars);Pars=ATgetNext(Pars),PosTypeList=ATgetNext(PosTypeList)){
    ATermAppl Par=ATAgetFirst(Pars);
    ATermAppl PosType=ATAgetFirst(PosTypeList);

    ATermAppl NewPosType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,gstcExpandNumTypesDown(PosType));

    if(!NewPosType) {return NULL;}
    NewPars=ATinsert(NewPars,(ATerm)Par);
    NewPosTypeList=ATinsert(NewPosTypeList,(ATerm)NewPosType);
  }
  NewPars=ATreverse(NewPars);
  NewPosTypeList=ATreverse(NewPosTypeList);

  PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);

  if(!PosTypeList){
    PosTypeList=ATLgetArgument(ATAgetArgument(Result,0),1);
    ATermList Pars=NewPars;
    NewPars=ATmakeList0();
    ATermList CastedPosTypeList=ATmakeList0();
    for(;!ATisEmpty(Pars);Pars=ATgetNext(Pars),PosTypeList=ATgetNext(PosTypeList),NewPosTypeList=ATgetNext(NewPosTypeList)){
      ATermAppl Par=ATAgetFirst(Pars);
      ATermAppl PosType=ATAgetFirst(PosTypeList);
      ATermAppl NewPosType=ATAgetFirst(NewPosTypeList);

      ATermAppl CastedNewPosType=gstcUpCastNumericType(PosType,NewPosType,&Par);
      if(!CastedNewPosType)
	{gsErrorMsg("Cannot cast %P to %P (while typechecking %P)\n",NewPosType,PosType,Par);return NULL;}
      
      NewPars=ATinsert(NewPars,(ATerm)Par);
      CastedPosTypeList=ATinsert(CastedPosTypeList,(ATerm)CastedNewPosType);
    }
    NewPars=ATreverse(NewPars);
    NewPosTypeList=ATreverse(CastedPosTypeList);

    PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);
  } 

  if(!PosTypeList) {gsErrorMsg("no %s %P with type %P is declared (while typechecking %P)\n",msg,Name,NewPosTypeList,ProcTerm);return NULL;}
  
  if(gstcIsNotInferredL(PosTypeList)){
    gsWarningMsg("ambiguous %s %P\n",msg,Name);
  }
    
  Result=gstcMakeActionOrProc(action,Name,PosTypeList,NewPars);

  gsDebugMsg("recognized %s %T\n",msg,Result);    
  return Result;
}

static inline ATermAppl gstcMakeActionOrProc(ATbool action, ATermAppl Name, 
					     ATermList FormParList, ATermList FactParList){
  return (action)?gsMakeAction(gsMakeActId(Name,FormParList),FactParList)
    :gsMakeProcess(gsMakeProcVarId(Name,FormParList),FactParList);
}

static ATermAppl gstcTraverseActProcVarConstP(ATermTable Vars, ATermAppl ProcTerm){
  ATermAppl Result=NULL;
  AFun ProcSymbol=ATgetAFun(ProcTerm);
  int n = ATgetArity(ProcSymbol);
  if(n==0) return ProcTerm;

  if(gsIsParamId(ProcTerm)){
    return gstcRewrActProc(Vars,ProcTerm);
  }

  if(gsIsBlock(ProcTerm) || gsIsHide(ProcTerm) || 
     gsIsRename(ProcTerm) || gsIsComm(ProcTerm) || gsIsAllow(ProcTerm)){

    //block & hide
    if(gsIsBlock(ProcTerm) || gsIsHide(ProcTerm)){
      const char *msg=gsIsBlock(ProcTerm)?"Blocking":"Hiding";
      ATermList ActList=ATLgetArgument(ProcTerm,0);
      if(ATisEmpty(ActList)) gsWarningMsg("%s empty set of actions (typechecking %P)\n",msg,ProcTerm);

      ATermIndexedSet Acts=ATindexedSetCreate(63,50);
      for(;!ATisEmpty(ActList);ActList=ATgetNext(ActList)){
	ATermAppl Act=ATAgetFirst(ActList);
	
	//Actions must be declared
	if(!ATtableGet(context.actions,(ATerm)Act)) {gsErrorMsg("%s an undefined action %P (typechecking %P)\n",msg,Act,ProcTerm); return NULL;}
	ATbool nnew;
	ATindexedSetPut(Acts,(ATerm)Act,&nnew);
	if(!nnew) gsWarningMsg("%s action %P twice (typechecking %P)\n",msg,Act,ProcTerm);
      }
      ATindexedSetDestroy(Acts);
    }

    //rename
    if(gsIsRename(ProcTerm)){
      ATermList RenList=ATLgetArgument(ProcTerm,0);

      if(ATisEmpty(RenList)) gsWarningMsg("renaming empty set of actions (typechecking %P)\n",ProcTerm);

      ATermIndexedSet ActsFrom=ATindexedSetCreate(63,50);

      for(;!ATisEmpty(RenList);RenList=ATgetNext(RenList)){
	ATermAppl Ren=ATAgetFirst(RenList);
	ATermAppl ActFrom=ATAgetArgument(Ren,0);
	ATermAppl ActTo=ATAgetArgument(Ren,1);
	
	if(ATisEqual(ActFrom,ActTo)) gsWarningMsg("renaming action %P into itself (typechecking %P)\n",ActFrom,ProcTerm);
	
	//Actions must be declared and of the same types
	ATermList TypesFrom,TypesTo;
	if(!(TypesFrom=ATLtableGet(context.actions,(ATerm)ActFrom)))
	  {gsErrorMsg("renaming an undefined action %P (typechecking %P)\n",ActFrom,ProcTerm);return NULL;}
	if(!(TypesTo=ATLtableGet(context.actions,(ATerm)ActTo)))
	  {gsErrorMsg("renaming into an undefined action %P (typechecking %P)\n",ActTo,ProcTerm);return NULL;}

	TypesTo=gstcTypeListsIntersect(TypesFrom,TypesTo);
	if(!TypesTo || ATisEmpty(TypesTo))
	  {gsErrorMsg("renaming action %P into action %P: these two have no common type (typechecking %P)\n",ActTo,ActFrom,ProcTerm);return NULL;}
 
	ATbool nnew;
	ATindexedSetPut(ActsFrom,(ATerm)ActFrom,&nnew);
	if(!nnew) {gsErrorMsg("renaming action %P twice (typechecking %P)\n",ActFrom,ProcTerm);return NULL;}
     }
      ATindexedSetDestroy(ActsFrom);
    }

    //comm: like renaming multiactions (with the same parameters) to action/tau
    if(gsIsComm(ProcTerm)){
      ATermList CommList=ATLgetArgument(ProcTerm,0);

      if(ATisEmpty(CommList)) gsWarningMsg("synchronizing empty set of (multi)actions (typechecking %P)\n",ProcTerm);
      else{
	ATermList ActsFrom=ATmakeList0();

	for(;!ATisEmpty(CommList);CommList=ATgetNext(CommList)){
	  ATermAppl Comm=ATAgetFirst(CommList);
	  ATermList MActFrom=ATLgetArgument(ATAgetArgument(Comm,0),0);
	  ATermList BackupMActFrom=MActFrom;
	  assert(!ATisEmpty(MActFrom));
	  ATermAppl ActTo=ATAgetArgument(Comm,1);
	  
	  if(ATgetLength(MActFrom)==1)
	    gsWarningMsg("using Syncronization as Renaming (hiding) of action %P into %P (typechecking %P)\n",
			 ATgetFirst(MActFrom),ActTo,ProcTerm);
	  
	  //Actions must be declared
	  ATermList ResTypes=NULL;

	  if(!gsIsNil(ActTo)){
	    ResTypes=ATLtableGet(context.actions,(ATerm)ActTo);
	    if(!ResTypes) 
	      {gsErrorMsg("synchronizing to an undefined action %P (typechecking %P)\n",ActTo,ProcTerm);return NULL;}
	  }

	  for(;!ATisEmpty(MActFrom);MActFrom=ATgetNext(MActFrom)){
	    ATermAppl Act=ATAgetFirst(MActFrom);
	    ATermList Types=ATLtableGet(context.actions,(ATerm)Act);
	    if(!Types)
	      {gsErrorMsg("synchronizing an undefined action %P in (multi)action %P (typechecking %P)\n",Act,MActFrom,ProcTerm);return NULL;}
	    ResTypes=(ResTypes)?gstcTypeListsIntersect(ResTypes,Types):Types;
	    if(!ResTypes || ATisEmpty(ResTypes))
	      {gsErrorMsg("synchronizing action %P from (multi)action %P into action %P: these have no common type (typechecking %P), ResTypes: %T\n",
			  Act,BackupMActFrom,ActTo,ProcTerm,ResTypes);return NULL;}
	  }
	  MActFrom=BackupMActFrom;
	  
	  //the multiactions in the lhss of comm should not intersect.
	  //make the list of unique actions
	  ATermList Acts=ATmakeList0();
	  for(;!ATisEmpty(MActFrom);MActFrom=ATgetNext(MActFrom)){
	    ATermAppl Act=ATAgetFirst(MActFrom);
	    if(ATindexOf(Acts,(ATerm)Act,0)<0)
	      Acts=ATinsert(Acts,(ATerm)Act);
	  }
	  for(;!ATisEmpty(Acts);Acts=ATgetNext(Acts)){
	    ATermAppl Act=ATAgetFirst(Acts);
	    if(ATindexOf(ActsFrom,(ATerm)Act,0)>=0)
	      {gsErrorMsg("synchronizing action %P in different ways (typechecking %P)\n",Act,ProcTerm);return NULL;}
	    else ActsFrom=ATinsert(ActsFrom,(ATerm)Act);
	  }
	}
      }
    }
    
    //allow
    if(gsIsAllow(ProcTerm)){
      ATermList MActList=ATLgetArgument(ProcTerm,0);

      if(ATisEmpty(MActList)) gsWarningMsg("allowing empty set of (multi) actions (typechecking %P)\n",ProcTerm);
      else{
	ATermList MActs=ATmakeList0();
	
	for(;!ATisEmpty(MActList);MActList=ATgetNext(MActList)){
	  ATermList MAct=ATLgetArgument(ATAgetFirst(MActList),0);

	  //Actions must be declared
	  for(;!ATisEmpty(MAct);MAct=ATgetNext(MAct)){
	    ATermAppl Act=ATAgetFirst(MAct);
	    if(!ATLtableGet(context.actions,(ATerm)Act))
	      {gsErrorMsg("allowing an undefined action %P in (multi)action %P (typechecking %P)\n",Act,MAct,ProcTerm);return NULL;}
	  }	

	  MAct=ATLgetArgument(ATAgetFirst(MActList),0);
	  if(gstcMActIn(MAct,MActs))
	    gsWarningMsg("allowing (multi)action %P twice (typechecking %P)\n",MAct,ProcTerm);
	  else MActs=ATinsert(MActs,(ATerm)MAct);
	}
      }
    }

    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewProc) {return NULL;}
    return ATsetArgument(ProcTerm,(ATerm)NewProc,1);
  }

  if(gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
     gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm)){
    ATermAppl NewLeft=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,0));
 
    if(!NewLeft) {return NULL;}
    ATermAppl NewRight=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewRight) {return NULL;}
    return ATsetArgument(ATsetArgument(ProcTerm,(ATerm)NewLeft,0),(ATerm)NewRight,1);
  }

  if(gsIsAtTime(ProcTerm)){
    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,0));
    if(!NewProc) {return NULL;}
    ATermAppl Time=ATAgetArgument(ProcTerm,1);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(gsMakeSortIdReal()));
    if(!NewType) {return NULL;}

    if(!gstcTypeMatchA(gsMakeSortIdReal(),NewType)){
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(gsMakeSortIdReal(),NewType,&Time);
      if(!CastedNewType)
	{gsErrorMsg("Cannot (up)cast time value %P to type Real\n",Time);return NULL;}
    }
    
    return gsMakeAtTime(NewProc,Time);
  }

  if(gsIsCond(ProcTerm)){
    ATermAppl Cond=ATAgetArgument(ProcTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,gsMakeSortIdBool());
    if(!NewType) {return NULL;}
    ATermAppl NewLeft=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewLeft) {return NULL;}
    ATermAppl NewRight=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,2));
    if(!NewRight) {return NULL;}
    return gsMakeCond(Cond,NewLeft,NewRight);
  }

  if(gsIsSum(ProcTerm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars=gstcAddVars2Table(CopyVars,ATLgetArgument(ProcTerm,0));
    if(!NewVars) {
      ATtableDestroy(CopyVars); 
      gsErrorMsg("type error while typechecking %P\n",ProcTerm);    
      return NULL;
    }
    ATermAppl NewProc=gstcTraverseActProcVarConstP(NewVars,ATAgetArgument(ProcTerm,1));
    ATtableDestroy(CopyVars);
    if(!NewProc) {gsErrorMsg("while typechecking %P\n",ProcTerm);return NULL;}
    return ATsetArgument(ProcTerm,(ATerm)NewProc,1);
  }
  
  assert(0);
  return Result;
}

static ATermAppl gstcTraverseVarConsTypeD(ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl *DataTerm, ATermAppl PosType, ATermTable FreeVars){
  //Type checks and transforms *DataTerm replacing Unknown datatype with other ones.
  //Returns the type of the term
  //which should match the PosType
  //all the variables should be in AllowedVars
  //is a variable is in DeclaredVars and not in AllowedVars, 
  //a different error message is generated.
  //all free variables (if any) are added to FreeVars 

  ATermAppl Result=NULL;
 
  gsDebugMsg("gstcTraverseVarConsTypeD: DataTerm %T with PosType %T\n",*DataTerm,PosType);    

  if(gsIsNumber(*DataTerm)){
    ATermAppl Number=ATAgetArgument(*DataTerm,0);
    ATermAppl Sort=gsMakeSortIdInt();
    if(gstcIsPos(Number)) Sort=gsMakeSortIdPos();
    else if(gstcIsNat(Number)) Sort=gsMakeSortIdNat(); 
    
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Sort,1);
    
    if(gstcTypeMatchA(Sort,PosType)) return Sort;

    //upcasting
    ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Sort,DataTerm);
    if(!CastedNewType)
      {gsErrorMsg("Cannot (up)cast number %P to type %P\n",*DataTerm, PosType);return NULL;}
    return CastedNewType;
  }

  if(gsIsSetBagComp(*DataTerm)){
    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);
   
    ATermAppl VarDecl=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=ATAgetArgument(VarDecl,1);
    ATermList VarList=ATmakeList1((ATerm)VarDecl);
    ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
    if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
    ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
    if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    
    ATermAppl ResType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,gsMakeUnknown(),FreeVars);
    ATtableDestroy(CopyAllowedVars); 
    ATtableDestroy(CopyDeclaredVars); 

    if(!ResType) return NULL;
    if(gstcTypeMatchA(gsMakeSortIdBool(),ResType)) {
      NewType=gsMakeSortSet(NewType);
    } else if(gstcTypeMatchA(gsMakeSortIdNat(),ResType)) {
             NewType=gsMakeSortBag(NewType);
           } else return NULL;

    if(!(NewType=gstcTypeMatchA(NewType,PosType))){
      gsErrorMsg("a set or bag comprehansion of type %P does not match possible type %P (while typechecking %P)\n",ATAgetArgument(VarDecl,1),PosType,*DataTerm);
      return NULL;
    }

    if(FreeVars) 
      gstcRemoveVars(FreeVars,VarList);
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return NewType;
  }

  if(gsIsForall(*DataTerm) || gsIsExists(*DataTerm)){
    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    ATermList VarList=ATLgetArgument(*DataTerm,0);
    ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
    if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
    ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
    if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}

    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    if(!gstcTypeMatchA(gsMakeSortIdBool(),PosType)) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,gsMakeSortIdBool(),FreeVars);
    ATtableDestroy(CopyAllowedVars); 
    ATtableDestroy(CopyDeclaredVars); 

    if(!NewType) {return NULL;}
    if(!gstcTypeMatchA(gsMakeSortIdBool(),NewType)) {return NULL;}

    if(FreeVars) 
      gstcRemoveVars(FreeVars,VarList);
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return gsMakeSortIdBool();
  }

  if(gsIsLambda(*DataTerm)){
    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    ATermList VarList=ATLgetArgument(*DataTerm,0);
    ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
    if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
    ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
    if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}

    ATermList ArgTypes=gstcGetVarTypes(VarList);
    ATermAppl NewType=gstcUnArrowProd(ArgTypes,PosType);
    if(!NewType) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); gsErrorMsg("no functions with arguments %P among %P (while typechecking %P)\n", ArgTypes,PosType,*DataTerm);return NULL;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,NewType,FreeVars);
    ATtableDestroy(CopyAllowedVars); 
    ATtableDestroy(CopyDeclaredVars); 

    if(FreeVars) 
      gstcRemoveVars(FreeVars,VarList);
    if(!NewType) {return NULL;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return gsMakeSortArrowProd(ArgTypes,NewType);
  }
  
  if(gsIsWhr(*DataTerm)){
    ATermList WhereVarList=ATmakeList0();
    ATermList NewWhereList=ATmakeList0();
    for(ATermList WhereList=ATLgetArgument(*DataTerm,1);!ATisEmpty(WhereList);WhereList=ATgetNext(WhereList)){
      ATermAppl WhereElem=ATAgetFirst(WhereList);
      ATermAppl WhereTerm=ATAgetArgument(WhereElem,1);
      ATermAppl WhereType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&WhereTerm,gsMakeUnknown(),FreeVars);
      if(!WhereType) {return NULL;}
      WhereVarList=ATinsert(WhereVarList,(ATerm)gsMakeDataVarId(ATAgetArgument(WhereElem,0),WhereType));
      NewWhereList=ATinsert(NewWhereList,(ATerm)ATsetArgument(WhereElem,(ATerm)WhereTerm,1));
    }
    NewWhereList=ATreverse(NewWhereList);

    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    ATermList VarList=ATreverse(WhereVarList);
    ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
    if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
    ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
    if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}

    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,PosType,FreeVars);
    ATtableDestroy(CopyAllowedVars); 
    ATtableDestroy(CopyDeclaredVars); 

    if(!NewType) return NULL;
    if(FreeVars) 
      gstcRemoveVars(FreeVars,VarList);
    *DataTerm=gsMakeWhr(Data,NewWhereList);
    return NewType;
  }

  if(gsIsListEnum(*DataTerm) || gsIsSetEnum(*DataTerm)){
    ATermList DataTermList=ATLgetArgument(*DataTerm,0);
    ATermAppl Type=(gsIsListEnum(*DataTerm))?gstcUnList(PosType):gstcUnSet(PosType);
    if(!Type) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", (gsIsListEnum(*DataTerm))?"list":"set", PosType,*DataTerm);  return NULL;}
    ATermList NewDataTermList=ATmakeList0();
    for(;!ATisEmpty(DataTermList);DataTermList=ATgetNext(DataTermList)){
      ATermAppl DataTerm=ATAgetFirst(DataTermList);
      ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&DataTerm,Type,FreeVars);
      if(!Type0) return NULL;
      NewDataTermList=ATinsert(NewDataTermList,(ATerm)DataTerm);
      Type=Type0;
    }
    DataTermList=ATreverse(NewDataTermList);

    Type=(gsIsListEnum(*DataTerm))?gsMakeSortList(Type):gsMakeSortSet(Type);
    
    *DataTerm=(gsIsListEnum(*DataTerm))?gsMakeListEnum(DataTermList,Type):gsMakeSetEnum(DataTermList,Type);
    return Type;
  }  

  if(gsIsBagEnum(*DataTerm)){
    ATermList DataTermList2=ATLgetArgument(*DataTerm,0);
    ATermAppl Type=gstcUnBag(PosType);
    ATermList NewDataTermList2=ATmakeList0();
    for(;!ATisEmpty(DataTermList2);DataTermList2=ATgetNext(DataTermList2)){
      ATermAppl DataTerm2=ATAgetFirst(DataTermList2);
      ATermAppl DataTerm0=ATAgetArgument(DataTerm2,0);
      ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&DataTerm0,Type,FreeVars);
      if(!Type0) {return NULL;}
      ATermAppl DataTerm1=ATAgetArgument(DataTerm2,1);
      ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&DataTerm1,gsMakeSortIdNat(),FreeVars);
      if(!Type1) {return NULL;}
      NewDataTermList2=ATinsert(NewDataTermList2,(ATerm)gsMakeBagEnumElt(DataTerm0,DataTerm1));
      Type=Type0;
    }
    DataTermList2=ATreverse(NewDataTermList2);

    Type=gsMakeSortBag(Type);

    *DataTerm=gsMakeBagEnum(DataTermList2,Type);
    return Type;
  }

  if(gsIsDataApplProd(*DataTerm)){
    //arguments
    ATermList Arguments=ATLgetArgument(*DataTerm,1);
    int nArguments=ATgetLength(Arguments);

    ATermList NewArgumentTypes=ATmakeList0();
    ATermList NewArguments=ATmakeList0();
    for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
      ATermAppl Arg=ATAgetFirst(Arguments);
      ATermAppl Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,gsMakeUnknown(),FreeVars);
      if(!Type) {return NULL;}
      NewArguments=ATinsert(NewArguments,(ATerm)Arg);
      NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
    }
    Arguments=ATreverse(NewArguments);
    ATermList ArgumentTypes=ATreverse(NewArgumentTypes);

    //function
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeDN(nArguments,DeclaredVars,AllowedVars,
						&Data,gsMakeSortArrowProd(ArgumentTypes,PosType),FreeVars);
    if(!NewType) {gsErrorMsg("type error while trying to cast %P to type %P\n",gsMakeDataApplProd(Data,Arguments),PosType);return NULL;}
    
    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.

    if(gsIsSortArrowProd(gstcUnwindType(NewType))){
      ATermList NeededArgumentTypes=ATLgetArgument(gstcUnwindType(NewType),0);
     
      //arguments again
      ATermList NewArgumentTypes=ATmakeList0();
      ATermList NewArguments=ATmakeList0();
      for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments),
	    ArgumentTypes=ATgetNext(ArgumentTypes),NeededArgumentTypes=ATgetNext(NeededArgumentTypes)){
	ATermAppl Arg=ATAgetFirst(Arguments);
	ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
	ATermAppl Type=ATAgetFirst(ArgumentTypes);

	if(!gstcEqTypesA(NeededType,Type)){
	  //upcasting
	  ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg);
	  if(CastedNewType) Type=CastedNewType;
	  if(!gstcEqTypesA(NeededType,Type)){
	    gsDebugMsg("Doing again on %T, Type: %T, Needed type: %T\n",Arg,Type,NeededType);
	    ATermAppl NewType=gstcTypeMatchA(NeededType,Type);
	    if(!NewType) NewType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
	    if(!NewType) {gsErrorMsg("needed type %P does not match possible type %P (while typechecking %P in %P)\n",NeededType,Type,Arg,*DataTerm);return NULL;}
	    Type=NewType;
	    Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,Type,FreeVars);
	    if(!Type) {return NULL;}
	  }
	}
	NewArguments=ATinsert(NewArguments,(ATerm)Arg);
	NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
      }
      Arguments=ATreverse(NewArguments);
      ArgumentTypes=ATreverse(NewArgumentTypes);     
    }

    *DataTerm=gsMakeDataApplProd(Data,Arguments);
    
    if(gsIsSortArrowProd(gstcUnwindType(NewType))){
      return ATAgetArgument(gstcUnwindType(NewType),1);
    }
  
    return gstcUnArrowProd(ArgumentTypes,NewType);
  }  

  if(gsIsId(*DataTerm)||gsIsOpId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    ATermAppl Type=ATAtableGet(DeclaredVars,(ATerm)Name);
    if(Type){
      gsDebugMsg("Recognised declared variable %P, Type: %P\n",Name,Type);
      *DataTerm=gsMakeDataVarId(Name,Type);

      if(!ATAtableGet(AllowedVars,(ATerm)Name)) {
	gsWarningMsg("Variable %P freely occurs in the right-hand-side or in the condition of an equation, but not its left-hand-side\n",Name);
      }

      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if(NewType) Type=NewType;
      else{
	//upcasting
	ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Type,DataTerm);
	if(!CastedNewType)
	  {gsErrorMsg("Cannot (up)cast variable %P to type %P\n",*DataTerm,PosType);return NULL;}
      
	Type=CastedNewType;
      }

      //Add to free variables list
      if(FreeVars) 
	ATtablePut(FreeVars, (ATerm)Name, (ATerm)Type);

      return Type;
    }

    if((Type=ATAtableGet(context.constants,(ATerm)Name))){
      if(!(Type=gstcTypeMatchA(Type,PosType))) {gsErrorMsg("no constant %P with type %P\n",*DataTerm,PosType);return NULL;}
      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    
    ATermList ParList=ATLtableGet(gssystem.constants,(ATerm)Name);
    if(ParList){
      ATermList NewParList=ATmakeList0();
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if((Par=gstcTypeMatchA(Par,PosType)))
	  NewParList=ATinsert(NewParList,(ATerm)Par);
      }
      ParList=ATreverse(NewParList);
      if(ATisEmpty(ParList)) {gsErrorMsg("no system constant %P with type %P\n",*DataTerm,PosType);return NULL;}

      if(ATgetLength(ParList)==1){
	Type=ATAgetFirst(ParList);
	*DataTerm=gsMakeOpId(Name,Type);
	return Type;
      }
      else{
	//gsWarningMsg("ambiguous system constant %T\n",Name);    
	*DataTerm=gsMakeOpId(Name,gsMakeUnknown());
	return gsMakeUnknown();
      }
    }

    ATermList ParListS=ATLtableGet(gssystem.functions,(ATerm)Name);
    ParList=ATLtableGet(context.functions,(ATerm)Name);
    if(!ParList) ParList=ParListS;
    else if(ParListS) ParList=ATconcat(ParListS,ParList);

    if(!ParList){
      gsErrorMsg("unknown operation %P\n",Name);
      return NULL;
    }
    
    if(ATgetLength(ParList)==1){ 
      ATermAppl Type=ATAgetFirst(ParList);
      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    else{
      gsWarningMsg("unknown operation %P\n",Name);    
      *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
      return gsMakeUnknown();
    }
  }

  if(gsIsDataVarId(*DataTerm)){
    return ATAgetArgument(*DataTerm,1);
  }

  assert(0);
  return Result;
}
    
static ATermAppl gstcTraverseVarConsTypeDN(int nFactPars, ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl *DataTerm, ATermAppl PosType, ATermTable FreeVars){
  gsDebugMsg("gstcTraverseVarConsTypeDN: DataTerm %T with PosType %T, nFactPars %d\n",*DataTerm,PosType,nFactPars);    
  if(gsIsId(*DataTerm)||gsIsOpId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    ATermAppl Type=ATAtableGet(DeclaredVars,(ATerm)Name);
    if(Type){
      if(!gstcTypeMatchA(Type,PosType)){
	gsErrorMsg("The type %P of variable %P is incompatible with %P (typechecking %P)\n",Type,Name,PosType,*DataTerm); 
	return NULL;
      }

      if(!ATAtableGet(AllowedVars,(ATerm)Name)) {
	gsWarningMsg("The variable %P is not allowed in %P (in the context of an equation)\n",Name,*DataTerm);
      }

      //Add to free variables list
      if(FreeVars) 
	ATtablePut(FreeVars, (ATerm)Name, (ATerm)Type);

      *DataTerm=gsMakeDataVarId(Name,Type);
      return Type;
    }
    ATermList ParList;
    
    if(!nFactPars){
      if((Type=ATAtableGet(context.constants,(ATerm)Name))) return gsMakeOpId(Name,Type);
      else{
	if((ParList=ATLtableGet(gssystem.constants,(ATerm)Name))){
	  if(ATgetLength(ParList)==1){
	    ATermAppl Type=ATAgetFirst(ParList);
	    *DataTerm=gsMakeOpId(Name,Type);
	    return Type;
	  }
	  else{
	    gsWarningMsg("ambiguous system constant %P\n",Name);    
	    *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
	    return Type;
	  }
	}
	else{
	  gsErrorMsg("unknown constant %P\n",Name);
	  return NULL;
	}
      }
    }
    
    ATermList ParListS=ATLtableGet(gssystem.functions,(ATerm)Name);
    ParList=ATLtableGet(context.functions,(ATerm)Name);
    if(!ParList) ParList=ParListS;
    else if(ParListS) ParList=ATconcat(ParListS,ParList);

    if(!ParList) {
      gsErrorMsg("unknown operation %P with %d parameters\n",Name,nFactPars);
      return NULL;
    }
    gsDebugMsg("Possible types for Op %T with %d arguments are (ParList: %T; PosType: %T)\n",Name,nFactPars,ParList,PosType);

    { // filter ParList keeping only functions A_0#...#A_nFactPars->A
      ATermList NewParList=ATmakeList0(); 
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if(!gsIsSortArrowProd(Par)) continue;
	if(!(ATgetLength(ATLgetArgument(Par,0))==nFactPars)) continue; 
	NewParList=ATinsert(NewParList,(ATerm)Par);
      }
      ParList=ATreverse(NewParList);

      // filter ParList keeping only functions of the right type
      ATermList BackupParList=ParList;
      NewParList=ATmakeList0();
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if((Par=gstcTypeMatchA(Par,PosType)))
	  NewParList=ATinsert(NewParList,(ATerm)Par);
      }
      NewParList=ATreverse(NewParList);

      if(ATisEmpty(NewParList)){
	//Ok, this looks like a type error. We are not that strict. 
	//Pos can be Nat, or even Int...
	//So lets make PosType more liberal
	//We change every Pos to NotInferred(Pos,Nat,Int)...
	//and get the list. Then we take the min of the list.
	
	ParList=BackupParList;
	gsDebugMsg("Trying casting for Op %T with %d arguments (ParList: %T; PosType: %T)\n",Name,nFactPars,ParList,PosType);
	PosType=gstcExpandNumTypesUp(PosType);
	for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	  ATermAppl Par=ATAgetFirst(ParList);
	  if((Par=gstcTypeMatchA(Par,PosType)))
	    NewParList=ATinsert(NewParList,(ATerm)Par);
	}
	NewParList=ATreverse(NewParList);
	gsDebugMsg("The result of casting is %T\n",NewParList);
	if(ATgetLength(NewParList)>1) NewParList=ATmakeList1((ATerm)gstcMinType(NewParList));
      }

      if(ATisEmpty(NewParList)){
	//Ok, casting of the arguments did not help.
	//Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.
	
	ParList=BackupParList;
	gsDebugMsg("Trying result casting for Op %T with %d arguments (ParList: %T; PosType: %T)\n",Name,nFactPars,ParList,PosType);
	PosType=gstcExpandNumTypesDown(gstcExpandNumTypesUp(PosType));
	for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	  ATermAppl Par=ATAgetFirst(ParList);
	  if((Par=gstcTypeMatchA(Par,PosType)))
	    NewParList=ATinsert(NewParList,(ATerm)Par);
	}
	NewParList=ATreverse(NewParList);
	gsDebugMsg("The result of casting is %T\n",NewParList);
	if(ATgetLength(NewParList)>1) NewParList=ATmakeList1((ATerm)gstcMinType(NewParList));
      }
      ParList=NewParList;
    }
	
    if(ATisEmpty(ParList)) {
      gsErrorMsg("unknown operation %P with %d arguments that matches type %P\n",Name,nFactPars,PosType);    
      return NULL;
    }
    
    if(ATgetLength(ParList)==1){
      ATermAppl Type=ATAgetFirst(ParList);
      if(gstcHasUnknown(Type)){
	Type=gstcTypeMatchA(Type,PosType);
      }
      if(gstcHasUnknown(Type) && gsIsOpId(*DataTerm)){
	Type=gstcTypeMatchA(Type,ATAgetArgument(*DataTerm,1));
      }

      if(ATisEqual(gsMakeOpIdNameIf(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing if matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchIf(Type);
	if(!NewType){
	  gsErrorMsg("the function if has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameEq(),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(gsMakeOpIdNameNeq(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing == or != matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchEqNeq(Type);
	if(!NewType){
	  gsErrorMsg("the function == or != has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameCons(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing |> matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpCons(Type);
	if(!NewType){
	  gsErrorMsg("the function |> has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameSnoc(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing <| matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpSnoc(Type);
	if(!NewType){
	  gsErrorMsg("the function <| has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameConcat(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing |> matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpConcat(Type);
	if(!NewType){
	  gsErrorMsg("the function |> has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameEltAt(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing @ matching Type %T, PosType %T, DataTerm: %T\n",Type,PosType,*DataTerm);    
	ATermAppl NewType=gstcMatchListOpEltAt(Type);
	if(!NewType){
	  gsErrorMsg("the function @ has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }
      
      if(ATisEqual(gsMakeOpIdNameHead(),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(gsMakeOpIdNameRHead(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing {R,L}head matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpHead(Type);
	if(!NewType){
	  gsErrorMsg("the function {R,L}head has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameTail(),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(gsMakeOpIdNameRTail(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing {R,L}tail matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpTail(Type);
	if(!NewType){
	  gsErrorMsg("the function {R,L}tail has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameSet2Bag(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing Set2Bag matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchSetOpSet2Bag(Type);
	if(!NewType){
	  gsErrorMsg("the function Set2Bag has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameEltIn(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing {List,Set,Bag}In matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListSetBagOpIn(Type);
	if(!NewType){
	  gsErrorMsg("the function {List,Set,Bag}In has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameSubSet(),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(gsMakeOpIdNameSubSetEq(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing SubSet[Eq] or SubBag[Eq] matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchSetBagOpSubEq(Type);
	if(!NewType){
	  gsErrorMsg("the function SubSet[Eq] or SubBag[Eq]  has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameSetUnion(),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(gsMakeOpIdNameSetDiff(),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(gsMakeOpIdNameSetIntersect(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing {Set,Bag}{Union,Difference,Intersect} matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchSetBagOpUnionDiffIntersect(Type);
	if(!NewType){
	  gsErrorMsg("the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameSetCompl(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing SetCompl matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchSetOpSetCompl(Type);
	if(!NewType){
	  gsErrorMsg("the function SetCompl has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameBag2Set(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing Bag2Set matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchBagOpBag2Set(Type);
	if(!NewType){
	  gsErrorMsg("the function Bag2Set has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(gsMakeOpIdNameCount(),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing BagCount matching Type %T, PosType %T\n",Type,PosType);    
	ATermAppl NewType=gstcMatchBagOpBagCount(Type);
	if(!NewType){
	  gsErrorMsg("the function BagCount has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      *DataTerm=gsMakeOpId(Name,Type);
      assert(Type);
      return Type;
    }
    else{
      gsWarningMsg("ambiguous operation %P with %d parameters\n",Name,nFactPars);    
      *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
      return gsMakeUnknown();
    }
  }
  else {
    return gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,DataTerm,PosType,FreeVars);
  }
}

// ================================================================================
// Phase 2 -- type inference
// ================================================================================
static ATermList gstcGetNotInferredList(ATermList TypeListList){
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1
  
  ATermList Result=ATmakeList0();
  int nFormPars=ATgetLength(ATgetFirst(TypeListList));
  ATermList *Pars = NULL;
  if ( nFormPars > 0 )
    Pars = (ATermList *) malloc(nFormPars*sizeof(ATermList));
  //DECLA(ATermList,Pars,nFormPars);
  for(int i=0;i<nFormPars;i++){
    Pars[i]=ATmakeList0();
  }

  for(;!ATisEmpty(TypeListList);TypeListList=ATgetNext(TypeListList)){
    ATermList TypeList=ATLgetFirst(TypeListList);
    for(int i=0;i<nFormPars;TypeList=ATgetNext(TypeList),i++){
      Pars[i]=gstcInsertType(Pars[i],ATAgetFirst(TypeList));
    }
  }
  
  for(int i=nFormPars-1;i>=0;i--) Result=ATinsert(Result,(ATerm)gstcMakeNotInferred(ATreverse(Pars[i])));
  free(Pars);
  return Result;
}

static ATermAppl gstcUpCastNumericType(ATermAppl NeededType, ATermAppl Type, ATermAppl *Par){
  // Makes upcasting from Type to Needed Type for Par. Returns the resulting type      

  if(gsIsUnknown(NeededType)) return Type;
  if(gstcEqTypesA(NeededType,Type)) return Type;

  // Try Upcasting to Pos
  if(gstcTypeMatchA(NeededType,gsMakeSortIdPos())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())) return gsMakeSortIdPos();
  }

  // Try Upcasting to Nat
  if(gstcTypeMatchA(NeededType,gsMakeSortIdNat())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
      *Par=gsMakeDataApplProd(gstcMakeOpIdPos2Nat(),ATmakeList1((ATerm)*Par));
      return gsMakeSortIdNat();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdNat())) return gsMakeSortIdNat();
  }

  // Try Upcasting to Int
  if(gstcTypeMatchA(NeededType,gsMakeSortIdInt())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
      *Par=gsMakeDataApplProd(gstcMakeOpIdPos2Int(),ATmakeList1((ATerm)*Par));
      return gsMakeSortIdInt();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdNat())){
      *Par=gsMakeDataApplProd(gstcMakeOpIdNat2Int(),ATmakeList1((ATerm)*Par));
      return gsMakeSortIdInt();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdInt())) return gsMakeSortIdInt();
  }

  // Try Upcasting to Real
  if(gstcTypeMatchA(NeededType,gsMakeSortIdReal())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
      *Par=gsMakeDataApplProd(gstcMakeOpIdPos2Real(),ATmakeList1((ATerm)*Par));
      return gsMakeSortIdReal();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdNat())){
      *Par=gsMakeDataApplProd(gstcMakeOpIdNat2Real(),ATmakeList1((ATerm)*Par));
      return gsMakeSortIdReal();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdInt())){
      *Par=gsMakeDataApplProd(gstcMakeOpIdInt2Real(),ATmakeList1((ATerm)*Par));
      return gsMakeSortIdReal();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdReal())) return gsMakeSortIdReal();
  }

  return NULL;
}

static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type){
  for(ATermList OldTypeList=TypeList;!ATisEmpty(OldTypeList);OldTypeList=ATgetNext(OldTypeList)){
    if(gstcEqTypesA(ATAgetFirst(OldTypeList),Type)) return TypeList;
  }
  return ATinsert(TypeList,(ATerm)Type);
}

static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2){
  // returns the intersection of the 2 type list lists
  
  gsDebugMsg("gstcTypesIntersect:  TypeListList1 %T;    TypeListList2: %T\n",TypeListList1,TypeListList2);

  ATermList Result=ATmakeList0();

  for(;!ATisEmpty(TypeListList2);TypeListList2=ATgetNext(TypeListList2)){
    ATermList TypeList2=ATLgetFirst(TypeListList2);
    if(gstcInTypesL(TypeList2,TypeListList1)) Result=ATinsert(Result,(ATerm)TypeList2);
  }
  return ATreverse(Result);
}

static ATermList gstcAdjustNotInferredList(ATermList PosTypeList, ATermList TypeListList){
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  gsDebugMsg("gstcAdjustNotInferredList: PosTypeList %T;    TypeListList:%T \n",PosTypeList,TypeListList);

  //if PosTypeList has only normal types -- check if it is in TypeListList, 
  //if so return PosTypeList, otherwise return NULL
  if(!gstcIsNotInferredL(PosTypeList)){
    if(gstcInTypesL(PosTypeList,TypeListList)) return PosTypeList;
    else return NULL;
  }
 
  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  ATermList NewTypeListList=ATmakeList0();
  for(;!ATisEmpty(TypeListList);TypeListList=ATgetNext(TypeListList)){
    ATermList TypeList=ATLgetFirst(TypeListList);
    if(gstcIsTypeAllowedL(TypeList,PosTypeList))
      NewTypeListList=ATinsert(NewTypeListList,(ATerm)TypeList);
  }
  if(ATisEmpty(NewTypeListList)) return NULL;
  if(ATgetLength(NewTypeListList)==1) return ATLgetFirst(NewTypeListList);

  // otherwise return not inferred.
  return gstcGetNotInferredList(ATreverse(NewTypeListList));
}
  
static ATbool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList){
  //Checks if TypeList is allowed by PosTypeList (each respective element)
  assert(ATgetLength(TypeList)==ATgetLength(PosTypeList));
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList))
    if(!gstcIsTypeAllowedA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList))) return ATfalse;
  return ATtrue;
}

static ATbool gstcIsTypeAllowedA(ATermAppl Type, ATermAppl PosType){
  //Checks if Type is alowed by PosType
  if(gsIsUnknown(PosType)) return ATtrue;
  if(gstcIsNotInferred(PosType))
    return gstcInTypesA(Type,ATLgetArgument(PosType,0));

  //PosType is a normal type
  return gstcEqTypesA(Type,PosType);
}

static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType){
  //Checks if Type is allowed by PosType and returns the matching subtype of Type
  //we assume that Type has no NotInferred
  
  gsDebugMsg("gstcTypeMatchA Type: %T;    PosType: %T \n",Type,PosType);

  if(gsIsUnknown(Type)) return PosType;
  if(gsIsUnknown(PosType) || gstcEqTypesA(Type,PosType)) return Type;
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypeList=ATLgetArgument(PosType,0);!ATisEmpty(PosTypeList);PosTypeList=ATgetNext(PosTypeList)){
      ATermAppl NewPosType=ATAgetFirst(PosTypeList);
      gsDebugMsg("Matching candidate gstcTypeMatchA Type: %T;    PosType: %T New Type: %T\n",Type,PosType,NewPosType);

      if((NewPosType=gstcTypeMatchA(Type,NewPosType))){
	gsDebugMsg("Match gstcTypeMatchA Type: %T;    PosType: %T New Type: %T\n",Type,PosType,NewPosType);
	return NewPosType;
      }
    }
    gsDebugMsg("No match gstcTypeMatchA Type: %T;    PosType: %T \n",Type,PosType);
    return NULL;
  }
    
  //PosType is a normal type
  //if(!gstcHasUnknown(Type)) return NULL;

  if(gsIsSortId(Type)) Type=gstcUnwindType(Type);
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);

  if(gsIsSortList(Type)){
    if(!gsIsSortList(PosType)) return NULL;
    ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0));
    if(!Res) return NULL;
    return gsMakeSortList(Res);
  }

  if(gsIsSortSet(Type)){
    if(!gsIsSortSet(PosType)) return NULL;
    else {
      ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0));
      if(!Res) return NULL;
      return gsMakeSortSet(Res);
    }	
  }

  if(gsIsSortBag(Type)){
    if(!gsIsSortBag(PosType)) return NULL;
    else {
      ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0));
      if(!Res) return NULL;
      return gsMakeSortBag(Res);
    }
  }

  if(gsIsSortArrowProd(Type)){
    if(!gsIsSortArrowProd(PosType)) return NULL;
    else{
      ATermList ArgTypes=gstcTypeMatchL(ATLgetArgument(Type,0),ATLgetArgument(PosType,0));
      if(!ArgTypes) return NULL;
      ATermAppl ResType=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
      if(!ResType) return NULL;
      Type=gsMakeSortArrowProd(ArgTypes,ResType);
      gsDebugMsg("gstcTypeMatchA Done: Type: %T;    PosType: %T \n",Type,PosType);
      return Type;
    }
  }

  return NULL;
}

static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList){
  gsDebugMsg("gstcTypeMatchL TypeList: %T;    PosTypeList: %T \n",TypeList,PosTypeList);

  if(ATgetLength(TypeList)!=ATgetLength(PosTypeList)) return NULL;

  ATermList Result=ATmakeList0();
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList)){
    ATermAppl Type=gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList));
    if(!Type) return NULL;
    Result=ATinsert(Result,(ATerm)gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList)));
  }
  return ATreverse(Result);
}

static ATbool gstcIsNotInferredL(ATermList TypeList){
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList)){
    ATermAppl Type=ATAgetFirst(TypeList);
    if(gstcIsNotInferred(Type)||gsIsUnknown(Type)) return ATtrue;
  }
  return ATfalse;
}

static ATermAppl gstcUnwindType(ATermAppl Type){
  //gsDebugMsg("gstcUnwindType Type: %T\n",Type);

  if(gsIsSortList(Type)) return gsMakeSortList(gstcUnwindType(ATAgetArgument(Type,0)));
  if(gsIsSortSet(Type)) return gsMakeSortSet(gstcUnwindType(ATAgetArgument(Type,0)));
  if(gsIsSortBag(Type)) return gsMakeSortBag(gstcUnwindType(ATAgetArgument(Type,0)));
  
  if(gsIsSortId(Type)){
    ATermAppl Value=ATAtableGet(context.defined_sorts,(ATerm)ATAgetArgument(Type,0));
    if(!Value) return Type;
    return gstcUnwindType(Value);
  } 
  
  return Type;
}

static ATermAppl gstcUnSet(ATermAppl PosType){
  //select Set(Type), elements, return their list of arguments.
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortSet(PosType)) return ATAgetArgument(PosType,0);
  if(gsIsUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,1);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortSet(NewPosType)) NewPosType=ATAgetArgument(NewPosType,0);
      else if(!gsIsUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gstcMakeNotInferred(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcUnBag(ATermAppl PosType){
  //select Bag(Type), elements, return their list of arguments.
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortBag(PosType)) return ATAgetArgument(PosType,0);
  if(gsIsUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,1);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortBag(NewPosType)) NewPosType=ATAgetArgument(NewPosType,0);
      else if(!gsIsUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gstcMakeNotInferred(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcUnList(ATermAppl PosType){
  //select List(Type), elements, return their list of arguments.
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortList(PosType)) return ATAgetArgument(PosType,0);
  if(gsIsUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,1);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortList(NewPosType)) NewPosType=ATAgetArgument(NewPosType,0);
      else if(!gsIsUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gstcMakeNotInferred(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType){
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //return TypeX if unique, the set of TypeX as NotInferred if many, NULL otherwise

  gsDebugMsg("gstcUnArrowProd: ArgTypes %T with PosType %T\n",ArgTypes,PosType);    

  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortArrowProd(PosType)){
    ATermList PosArgTypes=ATLgetArgument(PosType,0);
    if(ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes)) return NULL;
    if(gstcEqTypesL(PosArgTypes,ArgTypes)) return ATAgetArgument(PosType,1);
  }
  if(gsIsUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,1);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortArrowProd(PosType)){
	ATermList PosArgTypes=ATLgetArgument(PosType,0);
	if(ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes)) continue;
	if(gstcEqTypesL(PosArgTypes,ArgTypes)) NewPosType=ATAgetArgument(NewPosType,1);
      }
      else if(!gsIsUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gstcMakeNotInferred(NewPosTypes);
  }
  return NULL;
}

static ATermList gstcGetVarTypes(ATermList VarDecls){
  ATermList Result=ATmakeList0();
  for(;!ATisEmpty(VarDecls);VarDecls=ATgetNext(VarDecls))
    Result=ATinsert(Result,(ATerm)ATAgetArgument(ATAgetFirst(VarDecls),1));
  return ATreverse(Result);
}

static ATbool gstcHasUnknown(ATermAppl Type){
  if(gsIsUnknown(Type)) return ATtrue;
  if(gsIsSortId(Type)) return ATfalse;
  if(gsIsSortList(Type)) return gstcHasUnknown(ATAgetArgument(Type,0));
  if(gsIsSortSet(Type)) return gstcHasUnknown(ATAgetArgument(Type,0));
  if(gsIsSortBag(Type)) return gstcHasUnknown(ATAgetArgument(Type,0));
  if(gsIsSortStruct(Type)) return ATfalse;

  if(gsIsSortArrowProd(Type)){
    for(ATermList TypeList=ATLgetArgument(Type,0);!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList))
      if(gstcHasUnknown(ATAgetFirst(TypeList))) return ATtrue;
    return gstcHasUnknown(ATAgetArgument(Type,1));
  } 
  
  return ATtrue;
}

static ATbool gstcIsNumericType(ATermAppl Type){
  //returns true if Type is Bool,Pos,Nat,Int or Real
  //otherwise return fase
  if(gsIsUnknown(Type)) return ATfalse;
  return (ATbool)(ATisEqual(gsMakeSortIdBool(),Type)||ATisEqual(gsMakeSortIdPos(),Type)||ATisEqual(gsMakeSortIdNat(),Type)||ATisEqual(gsMakeSortIdInt(),Type)||ATisEqual(gsMakeSortIdReal(),Type));
}

static ATermAppl gstcExpandNumTypesUp(ATermAppl Type){
  //Expand Pos.. to possible bigger types.
  if(gsIsUnknown(Type)) return Type;
  if(gstcEqTypesA(gsMakeSortIdPos(),Type)) return gstcMakeNotInferred(ATmakeList4((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gstcEqTypesA(gsMakeSortIdNat(),Type)) return gstcMakeNotInferred(ATmakeList3((ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gstcEqTypesA(gsMakeSortIdInt(),Type)) return gstcMakeNotInferred(ATmakeList2((ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gsIsSortId(Type)) return Type;
  if(gsIsSortList(Type) || gsIsSortSet(Type) || gsIsSortBag(Type)) return ATsetArgument(Type,(ATerm)gstcExpandNumTypesUp(ATAgetArgument(Type,0)),0);
  if(gsIsSortStruct(Type)) return Type;

  if(gsIsSortArrowProd(Type)){
    //the argument types, and if the resulting type is SortArrowProd -- recursively
    ATermList NewTypeList=ATmakeList0();
    for(ATermList TypeList=ATLgetArgument(Type,0);!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList))
      NewTypeList=ATinsert(NewTypeList,(ATerm)gstcExpandNumTypesUp(gstcUnwindType(ATAgetFirst(TypeList))));
    ATermAppl ResultType=ATAgetArgument(Type,1);
    if(!gsIsSortArrowProd(ResultType))
      return ATsetArgument(Type,(ATerm)ATreverse(NewTypeList),0);
    else 
      return gsMakeSortArrowProd(ATreverse(NewTypeList),gstcExpandNumTypesUp(gstcUnwindType(ResultType)));
  } 
  
  return Type;
}

static ATermAppl gstcExpandNumTypesDown(ATermAppl Type){
  // Expand Numeric types down
  if(gsIsUnknown(Type)) return Type;
  if(gsIsSortId(Type)) Type=gstcUnwindType(Type);
  
  ATbool function=ATfalse;
  ATermList Args=NULL;
  if(gsIsSortArrowProd(Type)){
    function=ATtrue;
    Args=ATLgetArgument(Type,0);
    Type=ATAgetArgument(Type,1);
  }
  
  if(gstcEqTypesA(gsMakeSortIdReal(),Type)) Type=gstcMakeNotInferred(ATmakeList4((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gstcEqTypesA(gsMakeSortIdInt(),Type)) Type=gstcMakeNotInferred(ATmakeList3((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt()));
  if(gstcEqTypesA(gsMakeSortIdNat(),Type)) Type=gstcMakeNotInferred(ATmakeList2((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat()));
  
  return (function)?gsMakeSortArrowProd(Args,Type):Type;
}

static ATermAppl gstcMinType(ATermList TypeList){
  return ATAgetFirst(TypeList);
}


// =========================== MultiActions
static ATbool gstcMActIn(ATermList MAct, ATermList MActs){
  //returns true if MAct is in MActs
  for(;!ATisEmpty(MActs);MActs=ATgetNext(MActs))
    if(gstcMActEq(MAct,ATLgetFirst(MActs))) return ATtrue;
       
  return ATfalse;
}

static ATbool gstcMActEq(ATermList MAct1, ATermList MAct2){
  //returns true if the two multiactions are equal.
  if(ATgetLength(MAct1)!=ATgetLength(MAct2)) return ATfalse;
  if(ATisEmpty(MAct1)) return ATtrue;
  ATermAppl Act1=ATAgetFirst(MAct1);
  MAct1=ATgetNext(MAct1);

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  ATermList NewMAct2=ATmakeList0();
  for(;!ATisEmpty(MAct2);MAct2=ATgetNext(MAct2)){
    ATermAppl Act2=ATAgetFirst(MAct2);
    if(ATisEqual(Act1,Act2)) {
      MAct2=ATconcat(ATreverse(NewMAct2),ATgetNext(MAct2)); goto gstcMActEq_found;
    }
    else{
      NewMAct2=ATinsert(NewMAct2,(ATerm)Act2);
    }
  }
  return ATfalse;
 gstcMActEq_found:
  return gstcMActEq(MAct1,MAct2);
}

static ATbool gstcMActSubEq(ATermList MAct1, ATermList MAct2){
  //returns true if MAct1 is a submultiaction of MAct2.
  if(ATgetLength(MAct1)>ATgetLength(MAct2)) return ATfalse;
  if(ATisEmpty(MAct1)) return ATtrue;
  ATermAppl Act1=ATAgetFirst(MAct1);
  MAct1=ATgetNext(MAct1);

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  ATermList NewMAct2=ATmakeList0();
  for(;!ATisEmpty(MAct2);MAct2=ATgetNext(MAct2)){
    ATermAppl Act2=ATAgetFirst(MAct2);
    if(ATisEqual(Act1,Act2)) {
      MAct2=ATconcat(ATreverse(NewMAct2),ATgetNext(MAct2)); goto gstcMActSubEqMA_found;
    }
    else{
      NewMAct2=ATinsert(NewMAct2,(ATerm)Act2);
    }
  }
  return ATfalse;
 gstcMActSubEqMA_found:
  return gstcMActSubEq(MAct1,MAct2);
}

static ATermAppl gstcUnifyMinType(ATermAppl Type1, ATermAppl Type2){
  //Find the minimal type that Unifies the 2. If not possible, return NULL.
  ATermAppl Res=gstcTypeMatchA(Type1,gstcExpandNumTypesUp(Type2));
  if(!Res) Res=gstcTypeMatchA(Type2,gstcExpandNumTypesUp(Type1));
  if(!Res) return NULL;
  if(gstcIsNotInferred(Res)) Res=ATAgetFirst(ATLgetArgument(Res,0));
  gsDebugMsg("gstcUnifyMinType: Type1 %T; Type2 %T; Res: %T\n",Type1,Type2,Res);    
  return Res;
}

static ATermAppl gstcMatchIf(ATermAppl Type){
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting 

  assert(gsIsSortArrowProd(Type));
  ATermList Args=ATLgetArgument(Type,0);
  ATermAppl Res=ATAgetArgument(Type,1);
  assert((ATgetLength(Args)==3));
  //assert(gsIsBool(ATAgetFirst(Args)));
  Args=ATgetNext(Args);

  if(!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args)))) return NULL;
  Args=ATgetNext(Args);
  if(!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args)))) return NULL;

  return gsMakeSortArrowProd(ATmakeList3((ATerm)gsMakeSortIdBool(),(ATerm)Res,(ATerm)Res),Res);
}

static ATermAppl gstcMatchEqNeq(ATermAppl Type){
  //tries to sort out the types for == or !=.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  //assert(gsIsBool(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  
  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Arg,(ATerm)Arg),gsMakeSortIdBool());
}

static ATermAppl gstcMatchListOpCons(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  gsDebugMsg("gstcMatchListOpCons: Type %T \n",Type);    

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(gstcUnwindType(Res)));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortList(gstcUnwindType(Arg2)));
  Arg2=ATAgetArgument(Arg2,0);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Res,(ATerm)gsMakeSortList(Res)),gsMakeSortList(Res));
}

static ATermAppl gstcMatchListOpSnoc(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortList(Arg1));
  Arg1=ATAgetArgument(Arg1,0);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)gsMakeSortList(Res),(ATerm)Res),gsMakeSortList(Res));
}

static ATermAppl gstcMatchListOpConcat(ATermAppl Type){
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortList(Arg1));
  Arg1=ATAgetArgument(Arg1,0);

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortList(Arg2));
  Arg2=ATAgetArgument(Arg2,0);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)gsMakeSortList(Res),(ATerm)gsMakeSortList(Res)),gsMakeSortList(Res));
}

static ATermAppl gstcMatchListOpEltAt(ATermAppl Type){
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortList(Arg1));
  Arg1=ATAgetArgument(Arg1,0);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  //assert((gsIsSortNat(ATAgetFirst(ATgetNext(Args))));

  return gsMakeSortArrowProd(ATmakeList2((ATerm)gsMakeSortList(Res),(ATerm)gsMakeSortIdNat()),Res);
}

static ATermAppl gstcMatchListOpHead(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortList(Arg));
  Arg=ATAgetArgument(Arg,0);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortList(Res)),Res);
}

static ATermAppl gstcMatchListOpTail(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortList(Arg));
  Arg=ATAgetArgument(Arg,0);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortList(Res)),gsMakeSortList(Res));
}

//Sets
static ATermAppl gstcMatchSetOpSet2Bag(ATermAppl Type){
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));

  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortBag(Res));
  Res=ATAgetArgument(Res,0);
 
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortSet(Arg));
  Arg=ATAgetArgument(Arg,0);

  Arg=gstcUnifyMinType(Arg,Res);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortSet(Arg)),gsMakeSortBag(Arg));
}

static ATermAppl gstcMatchListSetBagOpIn(ATermAppl Type){
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  //assert(gsIsBool(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortList(Arg2)||gsIsSortSet(Arg2)||gsIsSortBag(Arg2));
  ATermAppl Arg2s=ATAgetArgument(Arg2,0);
  
  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2s);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Arg,(ATerm)Arg2),gsMakeSortIdBool());
}

static ATermAppl gstcMatchSetBagOpSubEq(ATermAppl Type){
  //tries to sort out the types of SubSet, SubSetEq (Set(S)xSet(S)->Bool)
  //or SubBag, SubBagEq (Bag(S)xBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  //assert(gsIsBool(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  if(gstcIsNumericType(Arg1)) return Type;
  assert(gsIsSortSet(Arg1)||gsIsSortBag(Arg1));

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  if(gstcIsNumericType(Arg2)) return Type;
  assert(gsIsSortSet(Arg2)||gsIsSortBag(Arg2));
  
  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Arg,(ATerm)Arg),gsMakeSortIdBool());
}

static ATermAppl gstcMatchSetBagOpUnionDiffIntersect(ATermAppl Type){
  //tries to sort out the types of Set or Bag Union, Diff or Intersect 
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is 
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  if(gstcIsNumericType(Res)) return Type;
  assert(gsIsSortSet(Res)||gsIsSortBag(Res));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  if(gstcIsNumericType(Arg1)) return Type;
  assert(gsIsSortSet(Arg1)||gsIsSortBag(Arg1));

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  if(gstcIsNumericType(Arg2)) return Type;
  assert(gsIsSortSet(Arg2)||gsIsSortBag(Arg2));
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Res,(ATerm)Res),Res);
}

static ATermAppl gstcMatchSetOpSetCompl(ATermAppl Type){
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  if(gstcIsNumericType(Res)) return Type;
  assert(gsIsSortSet(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  if(gstcIsNumericType(Arg)) return Type;
  assert(gsIsSortSet(Arg));
  Arg=ATAgetArgument(Arg,0);
  
  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortSet(Res)),gsMakeSortSet(Res));
}

//Bags
static ATermAppl gstcMatchBagOpBag2Set(ATermAppl Type){
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));

  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortSet(Res));
  Res=ATAgetArgument(Res,0);
 
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortBag(Arg));
  Arg=ATAgetArgument(Arg,0);

  Arg=gstcUnifyMinType(Arg,Res);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortBag(Arg)),gsMakeSortSet(Arg));
}

static ATermAppl gstcMatchBagOpBagCount(ATermAppl Type){
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  //assert(gsIsNat(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortBag(Arg2));
  Arg2=ATAgetArgument(Arg2,0);
  
  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Arg,(ATerm)gsMakeSortBag(Arg)),gsMakeSortIdNat());
}



