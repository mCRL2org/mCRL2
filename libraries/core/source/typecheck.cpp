// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/map.h"

using namespace mcrl2::core::detail;

namespace mcrl2 {
  namespace core {

// Static data

static bool was_warning_upcasting=false;
static bool was_ambiguous=false;

// system constants and functions
typedef struct {
  ATermTable constants;		//name -> Set(sort expression)
  ATermTable functions;		//name -> Set(sort expression)
} gsSystem;
static gsSystem gssystem;

static ATermList list_minus(ATermList l, ATermList m);
static ATermList list_minus(ATermList l, ATermList m){
  ATermList n = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l))
    if ( ATindexOf(m,ATgetFirst(l),0) < 0 )
      n = ATinsert(n,ATgetFirst(l));
  return ATreverse(n);
}

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
  ATermTable PBs;
} Context;
static Context context;

// the body may be transformed
typedef struct {
  ATermList equations;
  ATermTable proc_freevars;     //name#type -> List(Vars)
  ATermTable proc_pars;	        //name#type -> List(Vars)
  ATermTable proc_bodies;	//name#type -> rhs
  //ATermAppl init;             //in the hash tables proc_pars and proc_bodies with key "init#[]" (beware when writing)
} Body;
static Body body;

// Static function declarations
static void gstcDataInit(void);
static void gstcDataDestroy(void);
static ATbool gstcReadInSorts (ATermList, bool high_level=true);
static ATbool gstcReadInConstructors(ATermList NewSorts=NULL);
static ATbool gstcReadInFuncs(ATermList, ATermList, bool high_level=true);
static ATbool gstcReadInActs (ATermList);
static ATbool gstcReadInProcsAndInit (ATermList, ATermAppl);
static ATbool gstcReadInPBESAndInit(ATermAppl, ATermAppl);

static ATbool gstcTransformVarConsTypeData(void);
static ATbool gstcTransformActProcVarConst(void);
static ATbool gstcTransformPBESVarConst(void);

static ATermList gstcWriteProcs(ATermList);
static ATermList gstcWritePBES(ATermList);

static ATbool gstcInTypesA(ATermAppl, ATermList);
static ATbool gstcEqTypesA(ATermAppl, ATermAppl);
static ATbool gstcInTypesL(ATermList, ATermList);
static ATbool gstcEqTypesL(ATermList, ATermList);

static ATbool gstcIsSortDeclared(ATermAppl SortName);
static ATbool gstcIsSortExprDeclared(ATermAppl SortExpr, bool high_level=true);
static ATbool gstcIsSortExprListDeclared(ATermList SortExprList);
static ATbool gstcReadInSortStruct(ATermAppl);
static ATbool gstcAddConstant(ATermAppl, const char*, bool high_level=true);
static ATbool gstcAddFunction(ATermAppl, const char*, bool high_level=true);

static void gstcAddSystemConstant(ATermAppl);
static void gstcAddSystemFunction(ATermAppl);

static void gstcATermTableCopy(ATermTable Vars, ATermTable CopyVars);

static ATermTable gstcAddVars2Table(ATermTable,ATermList);
static ATermTable gstcRemoveVars(ATermTable Vars, ATermList VarDecls);
static ATbool gstcVarsUnique(ATermList VarDecls);
static ATermAppl gstcRewrActProc(ATermTable, ATermAppl, bool is_pbes=false);
static inline ATermAppl gstcMakeActionOrProc(ATbool, ATermAppl, ATermList, ATermList);
static ATermAppl gstcTraverseActProcVarConstP(ATermTable, ATermAppl);
static ATermAppl gstcTraversePBESVarConstPB(ATermTable, ATermAppl);
static ATermAppl gstcTraverseVarConsTypeD(ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl *, ATermAppl, ATermTable FreeVars=NULL, bool strict_ambiguous=true, bool warn_upcasting=false);
static ATermAppl gstcTraverseVarConsTypeDN(ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl* , ATermAppl, ATermTable FreeVars=NULL, bool strict_ambiguous=true, int nPars = -1, bool warn_upcasting=false);

static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type);

static inline ATbool gstcIsPos(ATermAppl Number) {char c=gsATermAppl2String(Number)[0]; return (ATbool) (isdigit(c) && c>'0');}
static inline ATbool gstcIsNat(ATermAppl Number) {return (ATbool) isdigit(gsATermAppl2String(Number)[0]);}

static inline ATermAppl INIT_KEY(void){return gsMakeProcVarId(gsString2ATermAppl("init"),ATmakeList0());}

static ATermAppl gstcUpCastNumericType(ATermAppl NeededType, ATermAppl Type, ATermAppl *Par, bool warn_upcasting=false);
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
static ATermAppl gstcMatchEqNeqComparison(ATermAppl Type);
static ATermAppl gstcMatchListOpCons(ATermAppl Type);
static ATermAppl gstcMatchListOpSnoc(ATermAppl Type);
static ATermAppl gstcMatchListOpConcat(ATermAppl Type);
static ATermAppl gstcMatchListOpEltAt(ATermAppl Type);
static ATermAppl gstcMatchListOpHead(ATermAppl Type);
static ATermAppl gstcMatchListOpTail(ATermAppl Type);
static ATermAppl gstcMatchSetOpSet2Bag(ATermAppl Type);
static ATermAppl gstcMatchListSetBagOpIn(ATermAppl Type);
static ATermAppl gstcMatchSetBagOpUnionDiffIntersect(ATermAppl Type);
static ATermAppl gstcMatchSetOpSetCompl(ATermAppl Type);
static ATermAppl gstcMatchBagOpBag2Set(ATermAppl Type);
static ATermAppl gstcMatchBagOpBagCount(ATermAppl Type);

static void gstcErrorMsgCannotCast(ATermAppl CandidateType, ATermList Arguments, ATermList ArgumentTypes);

// Typechecking modal formulas
static ATermAppl gstcTraverseStateFrm(ATermTable Vars, ATermTable StateVars, ATermAppl StateFrm);
static ATermAppl gstcTraverseRegFrm(ATermTable Vars, ATermAppl RegFrm);
static ATermAppl gstcTraverseActFrm(ATermTable Vars, ATermAppl ActFrm);


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

static ATermAppl gstcUpdateSortSpec(ATermAppl Spec, ATermAppl SortSpec);
//Pre: Spec and SortSpec are resp. specifications and sort specifications that
//     adhere to the internal syntax after type checking
//Ret: Spec in which the sort specification is replaced by SortSpec

///\brief  Checks whether a state formula is monotonic.
///\param[in] state_frm An ATerm representation of a state formula.
///\return Whether the state formula is monotonic.
static bool check_monotonicity(ATermAppl state_frm);

///\brief  Checks whether a state formula is monotonic with respect to a
///        context of propositional variables and the number of
///        negations under which they occur.
///\param[in] state_frm An ATerm representation of a state formula.
///\param[in] prop_var_negs A mapping from ATerm representations of
///        propositional variable symbols to a boolean indicating
///        whether the symbols occurs under an even number of negations
///\return Whether state_frm is monotonic with respect to the
///        information in prop_var_negs
static bool check_monotonicity(ATermAppl state_frm, atermpp::map<atermpp::aterm,bool> prop_var_negs);

///\brief Increases the value of each key in map
///\param[in] m A mapping from an ATerm to a boolean
///\return m in which all values are negated
static inline atermpp::map<atermpp::aterm,bool> neg_values(atermpp::map<atermpp::aterm,bool> m) {
  for ( atermpp::map<atermpp::aterm,bool>::iterator i = m.begin() ; i != m.end(); i++ ) {
    m[i->first] = !i->second;
  }
  return m;
}


//type checking functions
//-----------------------

ATermAppl type_check_data_spec(ATermAppl data_spec)
{
  ATermAppl Result=NULL;
  gsDebugMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking read-in phase started\n");

  if(gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0))) {
  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if(gstcReadInConstructors()) {
  if(gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
		     ATLgetArgument(ATAgetArgument(data_spec,2),0))) {
  body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);
  gsDebugMsg ("type checking read-in phase finished\n");

  gsDebugMsg ("type checking transform VarConst phase started\n");
  if(gstcTransformVarConsTypeData()){
  gsDebugMsg ("type checking transform VarConst phase finished\n");

  Result = ATsetArgument(data_spec, (ATerm) gsMakeDataEqnSpec(body.equations),3);

  Result = gstcFoldSortRefs(Result);

  gsDebugMsg ("type checking phase finished\n");
  }}}}

  if (Result != NULL) {
    gsDebugMsg("return %T\n", Result);
  }
  else {
    gsDebugMsg("return NULL\n");
  }
  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_proc_spec(ATermAppl proc_spec)
{
  ATermAppl Result=NULL;
  gsDebugMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking read-in phase started\n");

  ATermAppl data_spec = ATAgetArgument(proc_spec, 0);
  if(gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0))) {
  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if(gstcReadInConstructors()) {
  if(gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
		     ATLgetArgument(ATAgetArgument(data_spec,2),0))) {
  body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);
  if(gstcReadInActs(ATLgetArgument(ATAgetArgument(proc_spec,1),0))) {
  if(gstcReadInProcsAndInit(ATLgetArgument(ATAgetArgument(proc_spec,2),0),
			     ATAgetArgument(ATAgetArgument(proc_spec,3),1))) {
  gsDebugMsg ("type checking read-in phase finished\n");

  gsDebugMsg ("type checking transform ActProc+VarConst phase started\n");
  if(gstcTransformVarConsTypeData()){
  if(gstcTransformActProcVarConst()){
  gsDebugMsg ("type checking transform ActProc+VarConst phase finished\n");

  data_spec=ATAgetArgument(proc_spec,0);
  data_spec=ATsetArgument(data_spec, (ATerm) gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(proc_spec,(ATerm)data_spec,0);
  Result=ATsetArgument(Result,(ATerm)gsMakeProcEqnSpec(gstcWriteProcs(ATLgetArgument(ATAgetArgument(proc_spec,2),0))),2);
  Result=ATsetArgument(Result,(ATerm)gsMakeProcessInit(ATmakeList0(),
    ATAtableGet(body.proc_bodies,(ATerm)INIT_KEY())),3);

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

ATermAppl type_check_sort_expr(ATermAppl sort_expr, ATermAppl spec)
{
  //check correctness of the sort expression in sort_expr
  //using the specification in spec
  assert(gsIsSortExpr(sort_expr));
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  //gsWarningMsg("type checking of sort expressions is partially implemented\n");

  ATermAppl Result=NULL;

  gsDebugMsg ("type checking phase started\n");

  gstcDataInit();

  gsDebugMsg ("type checking of sort expressions read-in phase started\n");

  ATermAppl data_spec = NULL;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);

  //XXX read-in from spec (not finished)
  if(gstcReadInSorts(sorts,false)){
    gsDebugMsg ("type checking of sort expressions read-in phase finished\n");

    if(!gsIsNotInferred(sort_expr)){
      if(gstcIsSortExprDeclared(sort_expr)) Result=sort_expr;
    }
    else {
      gsErrorMsg("type checking of sort expressions failed (%T is not a sort expression)\n",sort_expr);
    }
  }
  else {
      gsErrorMsg("reading Sorts from LPS failed\n");
  }

  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_data_expr(ATermAppl data_expr, ATermAppl sort_expr, ATermAppl spec, ATermTable Vars)
{
  //check correctness of the data expression in data_expr using
  //the specification in spec

  //check preconditions
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  assert(gsIsDataExpr(data_expr));
  assert((sort_expr == NULL) || gsIsSortExpr(sort_expr));

  ATermAppl Result=NULL;

  gsDebugMsg ("type checking phase started\n");

  gstcDataInit();

  gsDebugMsg ("type checking of data expression read-in phase started\n");

  ATermAppl data_spec = NULL;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts,false) &&
      gstcReadInConstructors() &&
      gstcReadInFuncs(constructors,mappings,false))
  {
    gsDebugMsg ("type checking of data expression read-in phase finished\n");

    if( (sort_expr != NULL) && gsIsNotInferred(sort_expr)){
      gsErrorMsg("type checking of data expression failed (%T is not a sort expression)\n",sort_expr);
    } else if( (sort_expr == NULL) || gstcIsSortExprDeclared(sort_expr)) {
      bool destroy_vars=(Vars == NULL);
      if(destroy_vars) Vars=ATtableCreate(63,50);
      ATermAppl data=data_expr;
      ATermAppl Type=gstcTraverseVarConsTypeD(Vars,Vars,&data,sort_expr==NULL?gsMakeSortUnknown():sort_expr);
      if(destroy_vars) ATtableDestroy(Vars);
      if(Type && !gsIsSortUnknown(Type)) Result=data;
      else gsErrorMsg("type checking of data expression failed\n");
    }
  } else {
    gsErrorMsg("reading from LPS failed\n");
  }
  gstcDataDestroy();

  return Result;
}

ATermAppl type_check_mult_act(ATermAppl mult_act, ATermAppl spec)
{
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  ATermAppl Result=NULL;

  gsDebugMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking of multiactions read-in phase started\n");

  ATermAppl data_spec = ATAgetArgument(spec, 0);
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);
  ATermList action_labels = ATLgetArgument(ATAgetArgument(spec, 1), 0);

  //XXX read-in from spec (not finished)
    if(gstcReadInSorts(sorts,false)
       && gstcReadInConstructors()
       && gstcReadInFuncs(constructors,mappings,false)
       && gstcReadInActs(action_labels)
      ){
    gsDebugMsg ("type checking of multiactions read-in phase finished\n");

    if(gsIsMultAct(mult_act)){
      ATermTable Vars=ATtableCreate(63,50);
      ATermList r=ATmakeList0();
      for(ATermList l=ATLgetArgument(mult_act,0);!ATisEmpty(l);l=ATgetNext(l)){
        ATermAppl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        if(!o) goto done;
        r=ATinsert(r,(ATerm)o);
      }
      Result=ATsetArgument(mult_act,(ATerm)ATreverse(r),0);

    done:
      ATtableDestroy(Vars);
    }
    else {
      gsErrorMsg("type checking of multiactions failed (%T is not a multiaction)\n",mult_act);
    }
  }
  else {
      gsErrorMsg("reading from LPS failed\n");
  }
  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_proc_expr(ATermAppl proc_expr, ATermAppl spec)
{
  //check correctness of the process expression in proc_expr using
  //the process specification or LPS in spec
  assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  gsWarningMsg("type checking of process expressions is not yet implemented\n");
  return proc_expr;
}

ATermAppl type_check_state_frm(ATermAppl state_frm, ATermAppl spec)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  //check correctness of the state formula in state_formula using
  //the process specification or LPS in spec as follows:
  //1) determine the types of actions according to the definitions
  //   in spec
  //2) determine the types of data expressions according to the
  //   definitions in spec
  //3) check for name conflicts of data variable declarations in
  //   forall, exists, mu and nu quantifiers
  //4) check for monotonicity of fixpoint variables

  ATermAppl Result=NULL;
  gsDebugMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking of state formulas read-in phase started\n");

  ATermAppl data_spec = ATAgetArgument(spec, 0);
  ATermList action_labels = ATLgetArgument(ATAgetArgument(spec, 1), 0);

  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);

  //XXX read-in from spec (not finished)
  if(gstcReadInSorts(sorts,false)){
    if(gstcReadInConstructors()){
       if(gstcReadInFuncs(constructors,mappings,false)){
         if (action_labels != NULL)
         {
           if(!gstcReadInActs(action_labels))
             gsWarningMsg("ignoring the previous error(s), the formula will be typechecked without action label information\n");
         }
         else
         {
           gsWarningMsg("ignoring the previous error(s), the formula will be typechecked without action label information\n");
         }
         gsDebugMsg ("type checking of state formulas read-in phase finished\n");

         ATermTable Vars=ATtableCreate(63,50);
         Result=gstcTraverseStateFrm(Vars,Vars,state_frm);
         ATtableDestroy(Vars);
       }
       else {
         gsErrorMsg("reading functions from LPS failed\n");
       }
    }
    else {
      gsErrorMsg("reading structure constructors from LPS failed.\n");
    }
  }
  else {
    gsErrorMsg("reading sorts from LPS failed\n");
  }
  gstcDataDestroy();
  if (Result != NULL) {
    if (!check_monotonicity(Result)) {
      gsErrorMsg("state formula is not monotonic\n");
      Result = NULL;
    }
  }
  return Result;
}

ATermAppl type_check_action_rename_spec(ATermAppl ar_spec, ATermAppl lps_spec){

  //check precondition
  assert(gsIsActionRenameSpec(ar_spec));

  ATermAppl Result=NULL;
  gsDebugMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking of action rename specification read-in phase started\n");

  ATermTable actions_from_lps=ATtableCreate(63,50);

  ATermAppl lps_data_spec = ATAgetArgument(lps_spec, 0);
  ATermList lps_sorts = ATLgetArgument(ATAgetArgument(lps_data_spec, 0), 0);
  ATermList lps_constructors = ATLgetArgument(ATAgetArgument(lps_data_spec, 1), 0);
  ATermList lps_mappings = ATLgetArgument(ATAgetArgument(lps_data_spec, 2), 0);
  ATermList lps_action_labels = ATLgetArgument(ATAgetArgument(lps_spec, 1), 0);

  //XXX read-in from LPS (not finished)
  if(gstcReadInSorts((ATermList) lps_sorts,false)){
    if(gstcReadInConstructors()){
       if(gstcReadInFuncs(lps_constructors, lps_mappings,false)){
         if(!gstcReadInActs(lps_action_labels))
           gsWarningMsg("ignoring the previous error(s), the formula will be typechecked without action label information\n");
         gsDebugMsg ("type checking of action rename specification read-in phase of LPS finished\n");
         gsDebugMsg ("type checking of action rename specification read-in phase of rename file started\n");

         ATermAppl data_spec = ATAgetArgument(ar_spec, 0);
         ATermList LPSSorts=ATtableKeys(context.defined_sorts); // remember the sorts from the LPS.
         if(!gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0))) {
	   goto finally;
	 }
         gsDebugMsg ("type checking of action rename specification read-in phase of rename file sorts finished\n");

         // Check sorts for loops
         // Unwind sorts to enable equiv and subtype relations
         if(!gstcReadInConstructors(list_minus(ATtableKeys(context.defined_sorts),LPSSorts))) {
           goto finally;
         }
         gsDebugMsg ("type checking of action rename specification read-in phase of rename file constructors finished\n");

         if(!gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
                             ATLgetArgument(ATAgetArgument(data_spec,2),0))) {
           goto finally;
         }
         gsDebugMsg ("type checking of action rename specification read-in phase of rename file functions finished\n");

         body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);

	 //Save the actions from LPS only for the latter use.
         gstcATermTableCopy(context.actions,actions_from_lps);
         if(!gstcReadInActs(ATLgetArgument(ATAgetArgument(ar_spec,1),0))) {
           goto finally;
         }
         gsDebugMsg ("type checking action rename specification read-in phase of the ActionRenameSpec finished\n");

         if(!gstcTransformVarConsTypeData()){
           goto finally;
         }
         gsDebugMsg ("type checking transform VarConstTypeData phase finished\n");

         data_spec=ATsetArgument(data_spec, (ATerm) gsMakeDataEqnSpec(body.equations),3);
         Result=ATsetArgument(ar_spec,(ATerm)data_spec,0);
         Result=gstcFoldSortRefs(Result);


         // now the action renaming rules themselves.
         ATermAppl ActionRenameRules=ATAgetArgument(ar_spec, 2);
	 ATermList NewRules=ATmakeList0();

         ATermTable DeclaredVars=ATtableCreate(63,50);
         ATermTable FreeVars=ATtableCreate(63,50);

         bool b = true;

         for(ATermList l=ATLgetArgument(ActionRenameRules,0);!ATisEmpty(l);l=ATgetNext(l)){
           ATermAppl Rule=ATAgetFirst(l);
           assert(gsIsActionRenameRule(Rule));

           ATermList VarList=ATLgetArgument(Rule,0);
           if(!gstcVarsUnique(VarList)){
             b = false;
             gsErrorMsg("the variables in action rename rule %P are not unique\n",VarList,Rule);
             break;
           }

           ATermTable NewDeclaredVars=gstcAddVars2Table(DeclaredVars,VarList);
           if(!NewDeclaredVars){ b = false; break; }
           else DeclaredVars=NewDeclaredVars;

           ATermAppl Left=ATAgetArgument(Rule,2);
	   assert(gsIsParamId(Left));
           {  //extra check requested by Tom: actions in the LHS can only come from the LPS
              ATermTable temp=context.actions;
              context.actions=actions_from_lps;
	      Left=gstcTraverseActProcVarConstP(DeclaredVars,Left);
              context.actions=temp;
	      if(!Left) { b = false; break; }
           }

           ATermAppl Cond=ATAgetArgument(Rule,1);
           if(!gsIsNil(Cond) && !(gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Cond,gsMakeSortIdBool()))){ b = false; break; }

           ATermAppl Right=ATAgetArgument(Rule,3);
           assert(gsIsParamId(Right) || gsIsTau(Right) || gsIsDelta(Right));
           Right=gstcTraverseActProcVarConstP(DeclaredVars,Right);
           if(!Right) { b = false; break; }

	   NewRules=ATinsert(NewRules,(ATerm)gsMakeActionRenameRule(VarList,Cond,Left,Right));
	 }
         ATtableDestroy(FreeVars);
         ATtableDestroy(DeclaredVars);
	 if(!b){
           Result = NULL;
           goto finally;
         }

	 ActionRenameRules=ATsetArgument(ActionRenameRules,(ATerm)ATreverse(NewRules),0);
         Result=ATsetArgument(Result,(ATerm)ActionRenameRules,2);
         gsDebugMsg ("type checking phase finished\n");
       }
       else {
         gsErrorMsg("reading functions from LPS failed\n");
       }
    }
    else {
      gsErrorMsg("reading structure constructors from LPS failed\n");
    }
  }
  else {
    gsErrorMsg("reading sorts from LPS failed\n");
  }

finally:
  ATtableDestroy(actions_from_lps);
  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_pbes_spec(ATermAppl pbes_spec)
{
  //check correctness of the PBES specification in pbes_spec
  //gsWarningMsg("type checking of PBES specifications is only partially implemented\n");

  assert(gsIsPBES(pbes_spec));

  ATermAppl Result=NULL;
  gsDebugMsg ("type checking phase of PBES specifications started\n");
  gstcDataInit();

  gsDebugMsg ("type checking of PBES specification read-in phase started\n");


  ATermAppl data_spec = ATAgetArgument(pbes_spec,0);
  ATermAppl pb_eqn_spec = ATAgetArgument(pbes_spec,1);
  ATermAppl pb_init = ATAgetArgument(pbes_spec,2);

  if(!gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0))) {
    goto finally;
  }
  gsDebugMsg ("type checking of PBES specification read-in phase of sorts finished\n");

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if(!gstcReadInConstructors()) {
    goto finally;
  }
  gsDebugMsg ("type checking of PBES specification read-in phase of constructors finished\n");

  if(!gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
                      ATLgetArgument(ATAgetArgument(data_spec,2),0))) {
    goto finally;
  }
  gsDebugMsg ("type checking of PBES specification read-in phase of functions finished\n");

  body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);

  if(!gstcReadInPBESAndInit(pb_eqn_spec,pb_init)) {
    goto finally;
  }
  gsDebugMsg ("type checking PBES read-in phase finished\n");

  gsDebugMsg ("type checking transform Data+PBES phase started\n");
  if(!gstcTransformVarConsTypeData()){
    goto finally;
  }
  if(!gstcTransformPBESVarConst()){
    goto finally;
  }
  gsDebugMsg ("type checking transform Data+PBES phase finished\n");

  data_spec=ATsetArgument(data_spec,(ATerm)gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(pbes_spec,(ATerm)data_spec,0);

  pb_eqn_spec=ATsetArgument(pb_eqn_spec,(ATerm)gstcWritePBES(ATLgetArgument(pb_eqn_spec,1)),1);
  Result=ATsetArgument(Result,(ATerm)pb_eqn_spec,1);

  pb_init=ATsetArgument(pb_init,(ATerm)ATAtableGet(body.proc_bodies,(ATerm)INIT_KEY()),1);
  Result=ATsetArgument(Result,(ATerm)pb_init,2);

  Result=gstcFoldSortRefs(Result);

finally:
  gstcDataDestroy();
  return Result;
}

ATermList type_check_data_vars(ATermList data_vars, ATermAppl spec)
{
  //check correctness of the data variable declaration in sort_expr
  //using the specification in spec

  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));

  gsDebugMsg ("type checking phase started\n");

  gstcDataInit();

  gsDebugMsg ("type checking of data variables read-in phase started\n");

  ATermAppl data_spec = NULL;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts,false)) {
    gsDebugMsg ("type checking of data variables read-in phase finished\n");

    ATermTable Vars=ATtableCreate(63,50);
    ATermTable NewVars=gstcAddVars2Table(Vars,data_vars);
    if(!NewVars) {
      ATtableDestroy(Vars);
      gsErrorMsg("type error while typechecking data variables\n");
      return NULL;
    }
    ATtableDestroy(Vars);
  } else {
    gsErrorMsg("reading from LPS failed\n");
  }
  gstcDataDestroy();

  return data_vars;
}

//local functions
//---------------

// fold functions

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

ATermAppl gstcUpdateSortSpec(ATermAppl Spec, ATermAppl SortSpec)
{
  assert(gsIsDataSpec(Spec) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) || gsIsPBES(Spec) || gsIsActionRenameSpec(Spec));
  assert(gsIsSortSpec(SortSpec));
  if (gsIsDataSpec(Spec)) {
    Spec = ATsetArgument(Spec, (ATerm) SortSpec, 0);
  } else {
    ATermAppl DataSpec = ATAgetArgument(Spec, 0);
    DataSpec = ATsetArgument(DataSpec, (ATerm) SortSpec, 0);
    Spec = ATsetArgument(Spec, (ATerm) DataSpec, 0);
  }
  return Spec;
}

ATermAppl gstcFoldSortRefs(ATermAppl Spec)
{
  assert(gsIsDataSpec(Spec) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) || gsIsPBES(Spec) || gsIsActionRenameSpec(Spec));
  gsDebugMsg("specification before folding:\n%T\n\n", Spec);
  //get sort declarations
  ATermAppl DataSpec;
  if (gsIsDataSpec(Spec)) {
    DataSpec = Spec;
  } else {
    DataSpec = ATAgetArgument(Spec, 0);
  }
  ATermAppl SortSpec = ATAgetArgument(DataSpec, 0);
  ATermList SortDecls = ATLgetArgument(SortSpec, 0);
  //split sort declarations in sort id's and sort references
  ATermList SortIds = NULL;
  ATermList SortRefs = NULL;
  gstcSplitSortDecls(SortDecls, &SortIds, &SortRefs);
  //fold sort references in the sort references themselves
  SortRefs = gstcFoldSortRefsInSortRefs(SortRefs);
  //substitute sort references in the rest of Spec, i.e.
  //(a) remove sort references from Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(SortIds));
  //(b) build substitution table
  ATermTable Substs = ATtableCreate(2*ATgetLength(SortRefs),50);
  ATermList l = SortRefs;
  while (!ATisEmpty(l)) {
    ATermAppl SortRef = ATAgetFirst(l);
    //add substitution for SortRef
    ATermAppl LHS = gsMakeSortId(ATAgetArgument(SortRef, 0));
    ATermAppl RHS = ATAgetArgument(SortRef, 1);
    if (gsIsSortId(RHS) || gsIsSortArrow(RHS)) {
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
    gsDebugMsg("substituting sort references in specification\n\n");
    Spec = NewSpec;
    NewSpec = (ATermAppl) gsSubstValuesTable(Substs, (ATerm) Spec, true);
  } while (!ATisEqual(NewSpec, Spec));
  ATtableDestroy(Substs);
  //add the removed sort references back to Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(ATconcat(SortIds, SortRefs)));
  gsDebugMsg("specification after folding:\n%T\n\n", Spec);
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
      if (gsIsSortId(RHS) || gsIsSortArrow(RHS)) {
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

bool check_monotonicity(ATermAppl state_frm)
{
  assert(gsIsStateFrm(state_frm));
  atermpp::map<atermpp::aterm,bool> prop_var_negs;
  return check_monotonicity(state_frm, prop_var_negs);
}

bool check_monotonicity(ATermAppl state_frm, atermpp::map<atermpp::aterm,bool> prop_var_negs)
{
  assert(gsIsStateFrm(state_frm));
  if (gsIsDataExpr(state_frm)) {
    //state_frm is a data expression; return true
    return true;
  } else if (gsIsStateTrue(state_frm) || gsIsStateFalse(state_frm)) {
    //state_frm is true or false; return true
    return true;
  } else if (gsIsStateNot(state_frm)) {
    //state_frm is a negation; check monotonicity of its argument, where the number
    //of negations in prop_var_negs is incremented
    return check_monotonicity(
      ATAgetArgument(state_frm,0), neg_values(prop_var_negs));
  } else if (gsIsStateAnd(state_frm) || gsIsStateOr(state_frm)) {
    //state_frm is a conjunction or a disjunction; check monotonicity of both
    //arguments
    return
      check_monotonicity(ATAgetArgument(state_frm,0), prop_var_negs) &&
      check_monotonicity(ATAgetArgument(state_frm,1), prop_var_negs);
  } else if (gsIsStateImp(state_frm)) {
    //state_frm is an implication; check monotonicity of both arguments, where the
    //number of negations in prop_var_negs is negated for the first argument
    return
      check_monotonicity(ATAgetArgument(state_frm,0), neg_values(prop_var_negs)) &&
      check_monotonicity(ATAgetArgument(state_frm,1), prop_var_negs);
  } else if (gsIsStateForall(state_frm) || gsIsStateExists(state_frm)) {
    //state_frm is a boolean quantification; check monotonicity of the body of the
    //quantification
    return check_monotonicity(ATAgetArgument(state_frm,1), prop_var_negs);
  } else if (gsIsStateMust(state_frm) || gsIsStateMay(state_frm)) {
    //state_frm is a modal operation; check monotonicity of its second argument
    return check_monotonicity(ATAgetArgument(state_frm,1), prop_var_negs);
  } else if (gsIsStateYaled(state_frm) || gsIsStateYaledTimed(state_frm) ||
             gsIsStateDelay(state_frm) || gsIsStateDelayTimed(state_frm))
  {
    //state_frm is a delay or yaled; return true
    return true;
  } else if (gsIsStateVar(state_frm)) {
    //state_frm is a fixed point variable occurrence indicator; return true if the
    //number of negations of the occurrence is even; return false otherwise
    ATermAppl fp_var_name = ATAgetArgument(state_frm, 0);
    ATermList args = ATLgetArgument(state_frm, 1);
    ATermList fp_var_type = ATmakeList0();
    while (!ATisEmpty(args)) {
      fp_var_type = ATinsert(fp_var_type,
        (ATerm) gsGetSort(ATAgetFirst(args))
      );
      args = ATgetNext(args);
    }
    ATerm fp_var = (ATerm) ATmakeList2((ATerm) fp_var_name, (ATerm) ATreverse(fp_var_type));
    return prop_var_negs[fp_var];
  } else if (gsIsStateNu(state_frm) || gsIsStateMu(state_frm)) {
    //state_frm is a fixed point quantification; check monotonicity of the body of
    //the quantification, where prop_var_negs is extended or updated with the fixed
    //point variable, depending on if the variable already occurs in it
    ATermAppl fp_var_name = ATAgetArgument(state_frm, 0);
    ATermList data_var_id_inits = ATLgetArgument(state_frm, 1);
    ATermList fp_var_type = ATmakeList0();
    while (!ATisEmpty(data_var_id_inits)) {
      fp_var_type = ATinsert(fp_var_type,
        (ATerm) gsGetSort(ATAgetArgument(ATAgetFirst(data_var_id_inits), 0))
      );
      data_var_id_inits = ATgetNext(data_var_id_inits);
    }
    ATerm fp_var = (ATerm) ATmakeList2((ATerm) fp_var_name, (ATerm) ATreverse(fp_var_type));
    prop_var_negs[fp_var] = true;
    return check_monotonicity(ATAgetArgument(state_frm,2), prop_var_negs);
  } else {
    assert(false);
    return false;
  }
}

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
  context.PBs=ATtableCreate(63,50);
  body.proc_freevars=ATtableCreate(63,50);
  body.proc_pars=ATtableCreate(63,50);
  body.proc_bodies=ATtableCreate(63,50);
  ATprotectList(&body.equations);

  //Creation of operation identifiers for system defined operations.
  //Bool
  gstcAddSystemConstant(gsMakeOpIdTrue());
  gstcAddSystemConstant(gsMakeOpIdFalse());
  gstcAddSystemFunction(gsMakeOpIdNot());
  gstcAddSystemFunction(gsMakeOpIdAnd());
  gstcAddSystemFunction(gsMakeOpIdOr());
  gstcAddSystemFunction(gsMakeOpIdImp());
  gstcAddSystemFunction(gsMakeOpIdEq(gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdNeq(gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdIf(gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdLT(gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdLTE(gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdGTE(gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdGT(gsMakeSortUnknown()));
  //Numbers
  gstcAddSystemFunction(gsMakeOpIdPos2Nat());
  gstcAddSystemFunction(gsMakeOpIdPos2Int());
  gstcAddSystemFunction(gsMakeOpIdPos2Real());
  gstcAddSystemFunction(gsMakeOpIdNat2Pos());
  gstcAddSystemFunction(gsMakeOpIdNat2Int());
  gstcAddSystemFunction(gsMakeOpIdNat2Real());
  gstcAddSystemFunction(gsMakeOpIdInt2Pos());
  gstcAddSystemFunction(gsMakeOpIdInt2Nat());
  gstcAddSystemFunction(gsMakeOpIdInt2Real());
  gstcAddSystemFunction(gsMakeOpIdReal2Pos());
  gstcAddSystemFunction(gsMakeOpIdReal2Nat());
  gstcAddSystemFunction(gsMakeOpIdReal2Int());
  //more
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdInt(),gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdInt(),gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdMax(gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunction(gsMakeOpIdMin(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdMin(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdMin(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdMin(gsMakeSortIdReal()));
  //more
  gstcAddSystemFunction(gsMakeOpIdAbs(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdAbs(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdAbs(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdAbs(gsMakeSortIdReal()));
  //more
  gstcAddSystemFunction(gsMakeOpIdNeg(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdNeg(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdNeg(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdNeg(gsMakeSortIdReal()));
  gstcAddSystemFunction(gsMakeOpIdSucc(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdSucc(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdSucc(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdSucc(gsMakeSortIdReal()));
  gstcAddSystemFunction(gsMakeOpIdPred(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdPred(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdPred(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdPred(gsMakeSortIdReal()));
  gstcAddSystemFunction(gsMakeOpIdAdd(gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdAdd(gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdAdd(gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdAdd(gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdAdd(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdAdd(gsMakeSortIdReal(),gsMakeSortIdReal()));
  //more
  gstcAddSystemFunction(gsMakeOpIdSubt(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdSubt(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdSubt(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdSubt(gsMakeSortIdReal()));
  gstcAddSystemFunction(gsMakeOpIdMult(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdMult(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdMult(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdMult(gsMakeSortIdReal()));
  //more
  gstcAddSystemFunction(gsMakeOpIdDiv(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdDiv(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdDiv(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdMod(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdMod(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdMod(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdDivide(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdDivide(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdDivide(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdDivide(gsMakeSortIdReal()));
  gstcAddSystemFunction(gsMakeOpIdExp(gsMakeSortIdPos()));
  gstcAddSystemFunction(gsMakeOpIdExp(gsMakeSortIdNat()));
  gstcAddSystemFunction(gsMakeOpIdExp(gsMakeSortIdInt()));
  gstcAddSystemFunction(gsMakeOpIdExp(gsMakeSortIdReal()));
  gstcAddSystemFunction(gsMakeOpIdFloor());
  gstcAddSystemFunction(gsMakeOpIdCeil());
  gstcAddSystemFunction(gsMakeOpIdRound());
//Lists
  gstcAddSystemConstant(gsMakeOpIdEmptyList(gsMakeSortExprList(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdCons(gsMakeSortUnknown(),gsMakeSortExprList(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdListSize(gsMakeSortExprList(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdSnoc(gsMakeSortExprList(gsMakeSortUnknown()),gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdConcat(gsMakeSortExprList(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdEltAt(gsMakeSortExprList(gsMakeSortUnknown()),gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdHead(gsMakeSortExprList(gsMakeSortUnknown()), gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdTail(gsMakeSortExprList(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdRHead(gsMakeSortExprList(gsMakeSortUnknown()),gsMakeSortUnknown()));
  gstcAddSystemFunction(gsMakeOpIdRTail(gsMakeSortExprList(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdEltIn(gsMakeSortUnknown(),gsMakeSortExprList(gsMakeSortUnknown())));
//Sets
  gstcAddSystemFunction(gsMakeOpIdSet2Bag(gsMakeSortExprSet(gsMakeSortUnknown()),gsMakeSortExprBag(gsMakeSortUnknown())));
  gstcAddSystemConstant(gsMakeOpIdEmptySet(gsMakeSortExprSet(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdEltIn(gsMakeSortUnknown(),gsMakeSortExprSet(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdSetUnion(gsMakeSortExprSet(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdSetDiff(gsMakeSortExprSet(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdSetIntersect(gsMakeSortExprSet(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdSetCompl(gsMakeSortExprSet(gsMakeSortUnknown())));


//Bags
  gstcAddSystemFunction(gsMakeOpIdBag2Set(gsMakeSortExprBag(gsMakeSortUnknown()),gsMakeSortExprSet(gsMakeSortUnknown())));
  gstcAddSystemConstant(gsMakeOpIdEmptyBag(gsMakeSortExprBag(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdEltIn(gsMakeSortUnknown(),gsMakeSortExprBag(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdCount(gsMakeSortUnknown(),gsMakeSortExprBag(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdBagJoin(gsMakeSortExprBag(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdBagDiff(gsMakeSortExprBag(gsMakeSortUnknown())));
  gstcAddSystemFunction(gsMakeOpIdBagIntersect(gsMakeSortExprBag(gsMakeSortUnknown())));
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
  ATtableDestroy(context.PBs);
  ATtableDestroy(body.proc_freevars);
  ATtableDestroy(body.proc_pars);
  ATtableDestroy(body.proc_bodies);
  ATunprotectList(&body.equations);
}

static ATbool gstcReadInSorts (ATermList Sorts, bool high_level){
  ATbool nnew;
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
    ATermAppl Sort=ATAgetFirst(Sorts);
    ATermAppl SortName=ATAgetArgument(Sort,0);
    if(high_level && ATisEqual(gsMakeSortIdBool(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Bool\n");
      return ATfalse;
    }
    if(high_level && ATisEqual(gsMakeSortIdPos(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Pos\n");
      return ATfalse;
    }
    if(high_level && ATisEqual(gsMakeSortIdNat(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Nat\n");
      return ATfalse;
    }
    if(high_level && ATisEqual(gsMakeSortIdInt(),gsMakeSortId(SortName))){
      gsErrorMsg("attempt to redeclare sort Int\n");
      return ATfalse;
    }
    if(high_level && ATisEqual(gsMakeSortIdReal(),gsMakeSortId(SortName))){
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

static ATbool gstcReadInConstructors(ATermList NewSorts){
  ATermList Sorts=NewSorts;
  if(!Sorts) Sorts=ATtableKeys(context.defined_sorts);
  for(;!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
    ATermAppl SortExpr=ATAtableGet(context.defined_sorts,ATgetFirst(Sorts));
    if(!gstcIsSortExprDeclared(SortExpr)) return ATfalse;
    if(!gstcReadInSortStruct(SortExpr)) return ATfalse;
  }
  return ATtrue;
}

static ATbool gstcReadInFuncs(ATermList Cons, ATermList Maps, bool high_level){
  gsDebugMsg("Start Read-in Func\n");
  ATbool Result=ATtrue;

  unsigned constr_number=ATgetLength(Cons);
  for(ATermList Funcs=ATconcat(Cons,Maps);!ATisEmpty(Funcs);Funcs=ATgetNext(Funcs)){
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=ATAgetArgument(Func,0);
    ATermAppl FuncType=ATAgetArgument(Func,1);

    if(!gstcIsSortExprDeclared(FuncType,high_level)) { return ATfalse; }

//if FuncType is a defined function sort, unwind it
//{ ATermAppl NewFuncType;
//  if(gsIsSortId(FuncType)
//	 && (NewFuncType=ATAtableGet(context.defined_sorts,(ATerm)ATAgetArgument(FuncType,0)))
//	 && gsIsSortArrow(NewFuncType))
//	FuncType=NewFuncType;
//  }

    //if FuncType is a defined function sort, unwind it
    if(gsIsSortId(FuncType)){
      ATermAppl NewFuncType=gstcUnwindType(FuncType);
      if(gsIsSortArrow(NewFuncType)) FuncType=NewFuncType;
    }

    if((gsIsSortArrow(FuncType))){
      if(!gstcAddFunction(gsMakeOpId(FuncName,FuncType),"function",high_level)) { return ATfalse; }
    }
    else{
      if(!gstcAddConstant(gsMakeOpId(FuncName,FuncType),"constant",high_level)) { gsErrorMsg("could not add constant\n"); return ATfalse; }
    }

    if(constr_number){
      constr_number--;

      if(high_level){
        //Here checks for the constructors
        ATermAppl ConstructorType=FuncType;
        if(gsIsSortArrow(ConstructorType)) ConstructorType=ATAgetArgument(ConstructorType,1);
	ConstructorType=gstcUnwindType(ConstructorType);
	if(!gsIsSortId(ConstructorType)) { gsErrorMsg("Could not add constructor %P of sort %P. Constructors of a built-in sorts are not allowed.\n",FuncName,FuncType); return ATfalse; }
        if(ATisEqual(gsMakeSortIdBool(),ConstructorType)) { gsErrorMsg("Could not add constructor %P of sort %P. Constructors of a built-in sorts are not allowed.\n",FuncName,FuncType); return ATfalse; }
        if(ATisEqual(gsMakeSortIdPos(),ConstructorType)) { gsErrorMsg("Could not add constructor %P of sort %P. Constructors of a built-in sorts are not allowed.\n",FuncName,FuncType); return ATfalse; }
        if(ATisEqual(gsMakeSortIdNat(),ConstructorType)) { gsErrorMsg("Could not add constructor %P of sort %P. Constructors of a built-in sorts are not allowed.\n",FuncName,FuncType); return ATfalse; }
        if(ATisEqual(gsMakeSortIdInt(),ConstructorType)) { gsErrorMsg("Could not add constructor %P of sort %P. Constructors of a built-in sorts are not allowed.\n",FuncName,FuncType); return ATfalse; }
        if(ATisEqual(gsMakeSortIdReal(),ConstructorType)) { gsErrorMsg("Could not add constructor %P of sort %P. Constructors of a built-in sorts are not allowed.\n",FuncName,FuncType); return ATfalse; }
      }
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
    gsDebugMsg("Read-in Act Name %T, Types %T\n",ActName,Types);
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

static ATbool gstcReadInPBESAndInit(ATermAppl PBEqnSpec, ATermAppl PBInit){
  ATbool Result=ATtrue;

  ATermList PBFreeVars=ATLgetArgument(PBEqnSpec,0);
  ATermList PBEqns=ATLgetArgument(PBEqnSpec,1);

  for(;!ATisEmpty(PBEqns);PBEqns=ATgetNext(PBEqns)){
    ATermAppl PBEqn=ATAgetFirst(PBEqns);
    ATermAppl PBName=ATAgetArgument(ATAgetArgument(PBEqn,1),0);

    ATermList PBVars=ATLgetArgument(ATAgetArgument(PBEqn,1),1);

    ATermList PBType=ATmakeList0();
    for(ATermList l=PBVars; !ATisEmpty(l); l=ATgetNext(l)){
      PBType=ATinsert(PBType,(ATerm)ATAgetArgument(ATAgetFirst(l),1));
    }
    PBType=ATreverse(PBType);

    if(!gstcIsSortExprListDeclared(PBType)) { return ATfalse; }

    ATermList Types=ATLtableGet(context.PBs,(ATerm)PBName);
    if(!Types){
      Types=ATmakeList1((ATerm)PBType);
    }
    else{
      // temporarily prohibit overloading here
      gsErrorMsg("attempt to overload propositional variable %P\n", PBName); return ATfalse;
      // the table context.PBs contains a list of types for each
      // PBES name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(PBType, Types)){
	gsErrorMsg("double declaration of propositional variable %P\n", PBName);
	return ATfalse;
      }
      else{
	Types=ATappend(Types,(ATerm)PBType);
      }
    }
    ATtablePut(context.PBs,(ATerm)PBName,(ATerm)Types);

    //check that all formal parameters of the PBES are unique.
    if(!gstcVarsUnique(ATconcat(PBVars,PBFreeVars))){ gsErrorMsg("the formal and/or free variables in PBES %P are not unique\n",PBEqn); return ATfalse;}

    //This is a fake ProcVarId (There is no PBVarId)
    ATermAppl Index=gsMakeProcVarId(PBName,PBType);
    ATtablePut(body.proc_freevars,(ATerm)Index,(ATerm)PBFreeVars);
    ATtablePut(body.proc_pars,(ATerm)Index,(ATerm)PBVars);
    ATtablePut(body.proc_bodies,(ATerm)Index,(ATerm)ATAgetArgument(PBEqn,2));
    gsDebugMsg("Read-in Proc Name %T, Types %T\n",PBName,Types);
  }
  ATtablePut(body.proc_freevars,(ATerm)INIT_KEY(),(ATerm)ATLgetArgument(PBInit,0));
  ATtablePut(body.proc_pars,(ATerm)INIT_KEY(),(ATerm)ATmakeList0());
  ATtablePut(body.proc_bodies,(ATerm)INIT_KEY(),(ATerm)ATAgetArgument(PBInit,1));

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

static ATermList gstcWritePBES(ATermList oldPBES){
  ATermList Result=ATmakeList0();
  for(ATermList PBEqns=oldPBES;!ATisEmpty(PBEqns);PBEqns=ATgetNext(PBEqns)){
    ATermAppl PBEqn=ATAgetFirst(PBEqns);
    ATermAppl PBESVar=ATAgetArgument(PBEqn,1);

    ATermList PBType=ATmakeList0();
    for(ATermList l=ATLgetArgument(PBESVar,1); !ATisEmpty(l); l=ATgetNext(l)){
      PBType=ATinsert(PBType,(ATerm)ATAgetArgument(ATAgetFirst(l),1));
    }
    PBType=ATreverse(PBType);

    ATermAppl Index=gsMakeProcVarId(ATAgetArgument(PBESVar,0),PBType);

    if(Index==INIT_KEY()) continue;
    Result=ATinsert(Result,(ATerm)ATsetArgument(PBEqn,(ATerm)ATAtableGet(body.proc_bodies,(ATerm)Index),2));
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

    ATermTable NewDeclaredVars=gstcAddVars2Table(DeclaredVars,VarList);
    if(!NewDeclaredVars){ b = false; break; }
    else DeclaredVars=NewDeclaredVars;

    ATermAppl Left=ATAgetArgument(Eqn,2);
    ATermAppl LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,gsMakeSortUnknown(),FreeVars,false,true);
    if(!LeftType){ b = false; gsErrorMsg("the previous error occurred while typechecking %P as left hand side of equation %P\n",Left,Eqn); break;}
    if(was_warning_upcasting){ was_warning_upcasting=false; gsWarningMsg("the previous warning occurred while typechecking %P as left hand side of equation %P\n",Left,Eqn);}

    ATermAppl Cond=ATAgetArgument(Eqn,1);
    if(!gsIsNil(Cond) && !(gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Cond,gsMakeSortIdBool()))){ b = false; break; }
    ATermAppl Right=ATAgetArgument(Eqn,3);
    ATermAppl RightType=gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Right,LeftType,false);
    if(!RightType){ b = false; gsErrorMsg("the previous error occurred while typechecking %P as right hand side of equation %P\n",Right,Eqn); break; }

    //If the types are not uniquly the same now: do once more:
    if(!gstcEqTypesA(LeftType,RightType)){
      gsDebugMsg("Doing again for the equation %P, LeftType: %P, RightType: %P\n",Eqn,LeftType,RightType);
      ATermAppl Type=gstcTypeMatchA(LeftType,RightType);
      if(!Type){gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P do not match\n",LeftType,RightType,Eqn); b = false; break; }
      Left=ATAgetArgument(Eqn,2);
      ATtableReset(FreeVars);
      LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,Type,FreeVars,true);
      if(!LeftType){ b = false; gsErrorMsg("types of the left- and right-hand-sides of the equation %P do not match\n",Eqn); break; }
      if(was_warning_upcasting){ was_warning_upcasting=false; gsWarningMsg("the previous warning occurred while typechecking %P as left hand side of equation %P\n",Left,Eqn);}
      Right=ATAgetArgument(Eqn,3);
      RightType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Right,LeftType,FreeVars);
      if(!RightType){ b = false; gsErrorMsg("types of the left- and right-hand-sides of the equation %P do not match\n",Eqn); break; }
      Type=gstcTypeMatchA(LeftType,RightType);
      if(!Type){gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P do not match\n",LeftType,RightType,Eqn); b = false; break; }
      if(gstcHasUnknown(Type)){gsErrorMsg("types of the left- (%P) and right- (%P) hand-sides of the equation %P cannot be uniquely determined\n",LeftType,RightType,Eqn); b = false; break; }
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
    ATermTable NewVars=gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,(ATerm)ProcVar));
    if(!NewVars){ Result = ATfalse; break; }
    else Vars=NewVars;

    ATermAppl NewProcTerm=gstcTraverseActProcVarConstP(Vars,ATAtableGet(body.proc_bodies,(ATerm)ProcVar));
    if(!NewProcTerm){ Result = ATfalse; break; }
    ATtablePut(body.proc_bodies,(ATerm)ProcVar,(ATerm)NewProcTerm);
  }

  ATtableDestroy(Vars);
  return Result;
}

static ATbool gstcTransformPBESVarConst(void){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);

  //PBEs and data terms in PBEqns and init
  for(ATermList PBVars=ATtableKeys(body.proc_pars);!ATisEmpty(PBVars);PBVars=ATgetNext(PBVars)){
    ATermAppl PBVar=ATAgetFirst(PBVars);
    ATtableReset(Vars);

    ATermTable NewVars=gstcAddVars2Table(Vars,ATLtableGet(body.proc_freevars,(ATerm)PBVar));
    if(!NewVars){ Result = ATfalse; break; }
    else Vars=NewVars;

    NewVars=gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,(ATerm)PBVar));
    if(!NewVars){ Result = ATfalse; break; }
    else Vars=NewVars;

    ATermAppl NewPBTerm=gstcTraversePBESVarConstPB(Vars,ATAtableGet(body.proc_bodies,(ATerm)PBVar));
    if(!NewPBTerm){ Result = ATfalse; break; }
    ATtablePut(body.proc_bodies,(ATerm)PBVar,(ATerm)NewPBTerm);
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

static ATbool gstcIsSortExprDeclared(ATermAppl SortExpr, bool high_level){
  if(gsIsSortId(SortExpr)){
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if(!gstcIsSortDeclared(SortName))
      {gsErrorMsg("basic or defined sort %P is not declared\n",SortName); return ATfalse; }
    return ATtrue;
  }

  if(gsIsSortCons(SortExpr))
    return gstcIsSortExprDeclared(ATAgetArgument(SortExpr, 1),high_level);

  if(gsIsSortArrow(SortExpr)){
    if(!gstcIsSortExprDeclared(ATAgetArgument(SortExpr,1),high_level)) return ATfalse;
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
	if(!gstcIsSortExprDeclared(ProjSort,high_level)) {return ATfalse;}
      }
    }
    return ATtrue;
  }

  gsErrorMsg("this is not a sort expression %T\n",SortExpr);
  return ATfalse;
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

  if(gsIsSortCons(SortExpr))
    return gstcReadInSortStruct(ATAgetArgument(SortExpr,1));

  if(gsIsSortArrow(SortExpr)){
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
	 !gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(ATmakeList1((ATerm)SortExpr),gsMakeSortExprBool())),"recognizer")) {return ATfalse;}

      // constructor type and projections
      ATermList Projs=ATLgetArgument(Constr,1);
      Name=ATAgetArgument(Constr,0);
      if(ATisEmpty(Projs)){
	if(!gstcAddConstant(gsMakeOpId(Name,SortExpr),"constructor constant")){return ATfalse;}
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
	   !gstcAddFunction(gsMakeOpId(ProjName,gsMakeSortArrow(ATmakeList1((ATerm)SortExpr),ProjSort)),"projection")) {return ATfalse;}
	ConstructorType=ATinsert(ConstructorType,(ATerm)ProjSort);
      }
      if(!gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(ATreverse(ConstructorType),SortExpr)),"constructor")) {return ATfalse;}
    }
    return ATtrue;
  }

  assert(0);
  return Result;
}

static ATbool gstcAddConstant(ATermAppl OpId, const char* msg, bool high_level){
  assert(gsIsOpId(OpId));
  ATbool Result=ATtrue;

  ATermAppl Name = gsGetName(OpId);
  ATermAppl Sort = gsGetSort(OpId);

  if(ATAtableGet(context.constants, (ATerm)Name) /*|| ATLtableGet(context.functions, (ATerm)Name)*/){
    gsErrorMsg("double declaration of %s %P\n", msg, Name);
    return ATfalse;
  }

  if(ATLtableGet(gssystem.constants, (ATerm)Name) || ATLtableGet(gssystem.functions, (ATerm)Name)){
    if(high_level){
      gsErrorMsg("attempt to declare a constant with the name that is a built-in identifier (%P)\n", Name);
      return ATfalse;
    } else {
      // In this case we ignore the constant, i.e. don't add it to the context.constants
      return Result;
    }
  }

  ATtablePut(context.constants, (ATerm)Name, (ATerm)Sort);
  return Result;
}

static ATbool gstcAddFunction(ATermAppl OpId, const char *msg, bool high_level){
  assert(gsIsOpId(OpId));
  ATbool Result=ATtrue;
  ATermAppl Name = gsGetName(OpId);
  ATermAppl Sort = gsGetSort(OpId);

  //constants and functions can have the same names
  //  if(ATAtableGet(context.constants, (ATerm)Name)){
  //    ThrowMF("Double declaration of constant and %s %T\n", msg, Name);
  //  }

  if(ATAtableGet(gssystem.constants, (ATerm)Name)){
    if(high_level){
      gsErrorMsg("attempt to redeclare the system constant with %s %P\n", msg, Name);
      return ATfalse;
    }
    else {
      //ignore the implemented buil-in function/constant
      return Result;
    }
  }

  if(ATLtableGet(gssystem.functions, (ATerm)Name) &&
     !ATisEqual(Name,gsMakeOpIdNameMax()) &&
     !ATisEqual(Name,gsMakeOpIdNameMin()) &&
     !ATisEqual(Name,gsMakeOpIdNameAbs()) &&
     !ATisEqual(Name,gsMakeOpIdNameSucc()) &&
     !ATisEqual(Name,gsMakeOpIdNamePred()) &&
     !ATisEqual(Name,gsMakeOpIdNameDiv()) &&
     !ATisEqual(Name,gsMakeOpIdNameMod()) &&
     !ATisEqual(Name,gsMakeOpIdNameExp()) &&
     !ATisEqual(Name,gsMakeOpIdNameFloor()) &&
     !ATisEqual(Name,gsMakeOpIdNameCeil()) &&
     !ATisEqual(Name,gsMakeOpIdNameRound()) &&
     !ATisEqual(Name,gsMakeOpIdNameHead()) &&
     !ATisEqual(Name,gsMakeOpIdNameTail()) &&
     !ATisEqual(Name,gsMakeOpIdNameRHead()) &&
     !ATisEqual(Name,gsMakeOpIdNameRTail()) &&
     !ATisEqual(Name,gsMakeOpIdNameEltIn()) &&
     !ATisEqual(Name,gsMakeOpIdNameCount())
    ){
    if(high_level){
      gsErrorMsg("attempt to redeclare the system function with %s %P\n", msg, Name);
      return ATfalse;
    }
    else {
      //ignore the implemented buil-in function/constant
      return Result;
    }
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

static void gstcAddSystemConstant(ATermAppl OpId){
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.constants table
  assert(gsIsOpId(OpId));
  ATermAppl OpIdName = gsGetName(OpId);
  ATermAppl Type = gsGetSort(OpId);

  ATermList Types=ATLtableGet(gssystem.constants, (ATerm)OpIdName);

  if (!Types) Types=ATmakeList0();
  Types=ATappend(Types,(ATerm)Type);
  ATtablePut(gssystem.constants,(ATerm)OpIdName,(ATerm)Types);
}

static void gstcAddSystemFunction(ATermAppl OpId){
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  assert(gsIsOpId(OpId));
  ATermAppl OpIdName = gsGetName(OpId);
  ATermAppl Type = gsGetSort(OpId);
  assert(gsIsSortArrow(Type));

  ATermList Types=ATLtableGet(gssystem.functions, (ATerm)OpIdName);

  if (!Types) Types=ATmakeList0();
  Types=ATappend(Types,(ATerm)Type);
  ATtablePut(gssystem.functions,(ATerm)OpIdName,(ATerm)Types);
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

static ATermAppl gstcRewrActProc(ATermTable Vars, ATermAppl ProcTerm, bool is_pbes){
  ATermAppl Result=NULL;
  ATermAppl Name=ATAgetArgument(ProcTerm,0);
  ATermList ParList;

  ATbool action=ATfalse;

  if(!is_pbes){
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
  }
  else {
    if(!(ParList=ATLtableGet(context.PBs,(ATerm)Name))){
      gsErrorMsg("propositional variable %P not declared\n", Name);
      return NULL;
    }
  }
  assert(!ATisEmpty(ParList));

  unsigned int nFactPars=ATgetLength(ATLgetArgument(ProcTerm,1));
  const char *msg=(is_pbes)?"propositional variable":((action)?"action":"process");


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
    gsErrorMsg("no %s %P with %d parameter%s is declared (while typechecking %P)\n",
      msg, Name, nFactPars, (nFactPars != 1)?"s":"", ProcTerm);
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

    ATermAppl NewPosType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,PosType); //gstcExpandNumTypesDown(PosType));

    if(!NewPosType) {gsErrorMsg("cannot typecheck %P as type %P (while typechecking %P)\n",Par,gstcExpandNumTypesDown(PosType),ProcTerm);return NULL;}
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
	{gsErrorMsg("cannot cast %P to %P (while typechecking %P in %P)\n",NewPosType,PosType,Par,ProcTerm);return NULL;}

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

  if(is_pbes) Result=ATsetArgument(ProcTerm,(ATerm)NewPars,1);

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
  int n = ATgetArity(ATgetAFun(ProcTerm));
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

	  if(ATgetLength(MActFrom)==1) {
	    gsErrorMsg("using synchronization as renaming/hiding of action %P into %P (typechecking %P)\n",
	      ATgetFirst(MActFrom),ActTo,ProcTerm);
            return NULL;
          }

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
	{gsErrorMsg("cannot (up)cast time value %P to type Real\n",Time);return NULL;}
    }

    return gsMakeAtTime(NewProc,Time);
  }

  if(gsIsIfThen(ProcTerm)){
    ATermAppl Cond=ATAgetArgument(ProcTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,gsMakeSortIdBool());
    if(!NewType) {return NULL;}
    ATermAppl NewThen=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewThen) {return NULL;}
    return gsMakeIfThen(Cond,NewThen);
  }

  if(gsIsIfThenElse(ProcTerm)){
    ATermAppl Cond=ATAgetArgument(ProcTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,gsMakeSortIdBool());
    if(!NewType) {return NULL;}
    ATermAppl NewThen=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewThen) {return NULL;}
    ATermAppl NewElse=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,2));
    if(!NewElse) {return NULL;}
    return gsMakeIfThenElse(Cond,NewThen,NewElse);
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

static ATermAppl gstcTraversePBESVarConstPB(ATermTable Vars, ATermAppl PBESTerm){
  ATermAppl Result=NULL;

  if(gsIsDataExpr(PBESTerm)){
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&PBESTerm,gsMakeSortIdBool());
    if(!NewType) {return NULL;}
    return PBESTerm;
  }

  if(gsIsPBESTrue(PBESTerm) || gsIsPBESFalse(PBESTerm)) return PBESTerm;

  if(gsIsPBESNot(PBESTerm)){
    ATermAppl NewArg=gstcTraversePBESVarConstPB(Vars,ATAgetArgument(PBESTerm,0));
    if(!NewArg) {return NULL;}
    return ATsetArgument(PBESTerm,(ATerm)NewArg,0);
  }

  if(gsIsPBESAnd(PBESTerm) || gsIsPBESOr(PBESTerm) || gsIsPBESImp(PBESTerm)){
    ATermAppl NewLeft=gstcTraversePBESVarConstPB(Vars,ATAgetArgument(PBESTerm,0));
    if(!NewLeft) {return NULL;}
    ATermAppl NewRight=gstcTraversePBESVarConstPB(Vars,ATAgetArgument(PBESTerm,1));
    if(!NewRight) {return NULL;}
    return ATsetArgument(ATsetArgument(PBESTerm,(ATerm)NewLeft,0),(ATerm)NewRight,1);
  }

  if(gsIsPBESForall(PBESTerm)||gsIsPBESExists(PBESTerm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars=gstcAddVars2Table(CopyVars,ATLgetArgument(PBESTerm,0));
    if(!NewVars) {
      ATtableDestroy(CopyVars);
      gsErrorMsg("type error while typechecking %P\n",PBESTerm);
      return NULL;
    }
    ATermAppl NewPBES=gstcTraversePBESVarConstPB(NewVars,ATAgetArgument(PBESTerm,1));
    ATtableDestroy(CopyVars);
    if(!NewPBES) {gsErrorMsg("while typechecking %P\n",PBESTerm);return NULL;}
    return ATsetArgument(PBESTerm,(ATerm)NewPBES,1);
  }

  if(gsIsPropVarInst(PBESTerm)){
    return gstcRewrActProc(Vars, PBESTerm, true);
  }

  assert(0);
  return Result;
}

static ATermAppl gstcTraverseVarConsTypeD(ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl *DataTerm, ATermAppl PosType, ATermTable FreeVars, bool strict_ambiguous, bool warn_upcasting){
  //Type checks and transforms *DataTerm replacing Unknown datatype with other ones.
  //Returns the type of the term
  //which should match the PosType
  //all the variables should be in AllowedVars
  //if a variable is in DeclaredVars and not in AllowedVars,
  //a different error message is generated.
  //all free variables (if any) are added to FreeVars

  ATermAppl Result=NULL;

  gsDebugMsg("gstcTraverseVarConsTypeD: DataTerm %T with PosType %T\n",*DataTerm,PosType);

  if(gsIsBinder(*DataTerm)){
    //The variable declaration of a binder should have at least 1 declaration
    if(ATAgetFirst(ATLgetArgument(*DataTerm, 1)) == NULL)
      {gsErrorMsg("binder %P should have at least one declared variable\n",*DataTerm); return NULL;}

    ATermAppl BindingOperator = ATAgetArgument(*DataTerm, 0);
    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    if(gsIsSetBagComp(BindingOperator) || gsIsSetComp(BindingOperator)
        || gsIsBagComp(BindingOperator)){
      ATermList VarDecls=ATLgetArgument(*DataTerm,1);
      ATermAppl VarDecl=ATAgetFirst(VarDecls);

      //A Set/bag comprehension should have exactly one variable declared
      VarDecls=ATgetNext(VarDecls);
      if(ATAgetFirst(VarDecls) != NULL)
        {gsErrorMsg("set/bag comprehension %P should have exactly one declared variable\n", *DataTerm); return NULL;}

      ATermAppl NewType=ATAgetArgument(VarDecl,1);
      ATermList VarList=ATmakeList1((ATerm)VarDecl);
      ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
      if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
      ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
      if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
      ATermAppl Data=ATAgetArgument(*DataTerm,2);

      ATermAppl ResType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,gsMakeSortUnknown(),FreeVars,strict_ambiguous,warn_upcasting);
      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);

      if(!ResType) return NULL;
      if(gstcTypeMatchA(gsMakeSortIdBool(),ResType)) {
        NewType=gsMakeSortExprSet(NewType);
        *DataTerm = ATsetArgument(*DataTerm, (ATerm)gsMakeSetComp(), 0);
      } else if(gstcTypeMatchA(gsMakeSortIdNat(),ResType)) {
        NewType=gsMakeSortExprBag(NewType);
        *DataTerm = ATsetArgument(*DataTerm, (ATerm)gsMakeBagComp(), 0);
      } else return NULL;

      if(!(NewType=gstcTypeMatchA(NewType,PosType))){
        gsErrorMsg("a set or bag comprehension of type %P does not match possible type %P (while typechecking %P)\n",ATAgetArgument(VarDecl,1),PosType,*DataTerm);
        return NULL;
      }

      if(FreeVars)
        gstcRemoveVars(FreeVars,VarList);
      *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,2);
      return NewType;
    }

    if(gsIsForall(BindingOperator) || gsIsExists(BindingOperator)){
      ATermList VarList=ATLgetArgument(*DataTerm,1);
      ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
      if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
      ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
      if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}

      ATermAppl Data=ATAgetArgument(*DataTerm,2);
      if(!gstcTypeMatchA(gsMakeSortIdBool(),PosType)) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
      ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,gsMakeSortIdBool(),FreeVars,strict_ambiguous,warn_upcasting);
      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);

      if(!NewType) {return NULL;}
      if(!gstcTypeMatchA(gsMakeSortIdBool(),NewType)) {return NULL;}

      if(FreeVars)
        gstcRemoveVars(FreeVars,VarList);
      *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,2);
      return gsMakeSortIdBool();
    }

    if(gsIsLambda(BindingOperator)){
      ATermList VarList=ATLgetArgument(*DataTerm,1);
      ATermTable NewAllowedVars=gstcAddVars2Table(CopyAllowedVars,VarList);
      if(!NewAllowedVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}
      ATermTable NewDeclaredVars=gstcAddVars2Table(CopyDeclaredVars,VarList);
      if(!NewDeclaredVars) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); return NULL;}

      ATermList ArgTypes=gstcGetVarTypes(VarList);
      ATermAppl NewType=gstcUnArrowProd(ArgTypes,PosType);
      if(!NewType) {ATtableDestroy(CopyAllowedVars); ATtableDestroy(CopyDeclaredVars); gsErrorMsg("no functions with arguments %P among %P (while typechecking %P)\n", ArgTypes,PosType,*DataTerm);return NULL;}
      ATermAppl Data=ATAgetArgument(*DataTerm,2);

      NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,NewType,FreeVars,strict_ambiguous,warn_upcasting);

      gsDebugMsg("Result of gstcTraverseVarConsTypeD: DataTerm %T\n",Data);

      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);

      if(FreeVars)
        gstcRemoveVars(FreeVars,VarList);
      if(!NewType) {return NULL;}
      *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,2);
      return gsMakeSortArrow(ArgTypes,NewType);
    }
  }

  if(gsIsWhr(*DataTerm)){
    ATermList WhereVarList=ATmakeList0();
    ATermList NewWhereList=ATmakeList0();
    for(ATermList WhereList=ATLgetArgument(*DataTerm,1);!ATisEmpty(WhereList);WhereList=ATgetNext(WhereList)){
      ATermAppl WhereElem=ATAgetFirst(WhereList);
      ATermAppl WhereTerm=ATAgetArgument(WhereElem,1);
      ATermAppl WhereType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&WhereTerm,gsMakeSortUnknown(),FreeVars,strict_ambiguous,warn_upcasting);
      if(!WhereType) {return NULL;}
      ATermAppl NewWhereVar=gsMakeDataVarId(ATAgetArgument(WhereElem,0),WhereType);
      WhereVarList=ATinsert(WhereVarList,(ATerm)NewWhereVar);
      NewWhereList=ATinsert(NewWhereList,(ATerm)gsMakeDataVarIdInit(NewWhereVar,WhereTerm));
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
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,PosType,FreeVars,strict_ambiguous,warn_upcasting);
    ATtableDestroy(CopyAllowedVars);
    ATtableDestroy(CopyDeclaredVars);

    if(!NewType) return NULL;
    if(FreeVars)
      gstcRemoveVars(FreeVars,VarList);
    *DataTerm=gsMakeWhr(Data,NewWhereList);
    return NewType;
  }

  if(gsIsDataAppl(*DataTerm)){
    //arguments
    ATermList Arguments=ATLgetArgument(*DataTerm,1);
    unsigned int nArguments=ATgetLength(Arguments);

    //The following is needed to check enumerations
    ATermAppl Arg0 = ATAgetArgument(*DataTerm,0);
    if(gsIsOpId(Arg0) || gsIsId(Arg0)) {
      ATermAppl Name = ATAgetArgument(Arg0,0);
      if(Name == gsMakeOpIdNameListEnum()) {
        ATermAppl Type=gstcUnList(PosType);
        if(!Type) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "list", PosType,Arguments);  return NULL;}

	ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermList NewArguments=ATmakeList0();
        for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,gstcExpandNumTypesUp(Type),FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type0) return NULL;
          NewArguments=ATinsert(NewArguments,(ATerm)Argument);
          Type=Type0;
        }
        Arguments=OldArguments;

        //Second time to do the real work.
        NewArguments=ATmakeList0();
        for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,gstcExpandNumTypesUp(Type),FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type0) return NULL;
          NewArguments=ATinsert(NewArguments,(ATerm)Argument);
          Type=Type0;
        }
        Arguments=ATreverse(NewArguments);

        Type=gsMakeSortExprList(Type);
        *DataTerm=gsMakeDataExprListEnum(Arguments,Type);
        return Type;
      }
      if(Name == gsMakeOpIdNameSetEnum()) {
        ATermAppl Type=gstcUnSet(PosType);
        if(!Type) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "set", PosType,Arguments);  return NULL;}

        ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermList NewArguments=ATmakeList0();
        for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,gstcExpandNumTypesUp(Type),FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type0) {gsErrorMsg("not possible to cast %s to %P (while typechecking %T)\n", "element", Type,Argument);  return NULL;}
          NewArguments=ATinsert(NewArguments,(ATerm)Argument);
          Type=Type0;
        }
        Arguments=OldArguments;

        //Second time to do the real work.
        NewArguments=ATmakeList0();
        for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,gstcExpandNumTypesUp(Type),FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type0) {gsErrorMsg("not possible to cast %s to %P (while typechecking %T)\n", "element", Type,Argument);  return NULL;}
          NewArguments=ATinsert(NewArguments,(ATerm)Argument);
          Type=Type0;
        }
        Arguments=ATreverse(NewArguments);
        Type=gsMakeSortExprSet(Type);
        *DataTerm=gsMakeDataExprSetEnum(Arguments,Type);
        return Type;
      }
      if(Name == gsMakeOpIdNameBagEnum()) {
        ATermAppl Type=gstcUnBag(PosType);
        if(!Type) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "bag", PosType,Arguments);  return NULL;}

        ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermList NewArguments=ATmakeList0();
        for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
          ATermAppl Argument0=ATAgetFirst(Arguments);
          Arguments=ATgetNext(Arguments);
          ATermAppl Argument1=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument0,Type,FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type0) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "element", Type,Argument0);  return NULL;}
          ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument1,gsMakeSortIdNat(),FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type1) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "number", gsMakeSortIdNat(),Argument1);  return NULL;}
          NewArguments=ATinsert(NewArguments,(ATerm)Argument0);
          NewArguments=ATinsert(NewArguments,(ATerm)Argument1);
          Type=Type0;
        }
        Arguments=OldArguments;

        //Second time to do the real work.
        NewArguments=ATmakeList0();
        for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
          ATermAppl Argument0=ATAgetFirst(Arguments);
          Arguments=ATgetNext(Arguments);
          ATermAppl Argument1=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument0,Type,FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type0) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "element", Type,Argument0);  return NULL;}
          ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument1,gsMakeSortIdNat(),FreeVars,strict_ambiguous,warn_upcasting);
          if(!Type1) {gsErrorMsg("not possible to cast %s to %P (while typechecking %P)\n", "number", gsMakeSortIdNat(),Argument1);  return NULL;}
          NewArguments=ATinsert(NewArguments,(ATerm)Argument0);
          NewArguments=ATinsert(NewArguments,(ATerm)Argument1);
          Type=Type0;
        }
        Arguments=ATreverse(NewArguments);
        Type=gsMakeSortExprBag(Type);
        *DataTerm=gsMakeDataExprBagEnum(Arguments,Type);
        return Type;
      }
    }

    ATermList NewArgumentTypes=ATmakeList0();
    ATermList NewArguments=ATmakeList0();

    for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
      ATermAppl Arg=ATAgetFirst(Arguments);
      ATermAppl Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,gsMakeSortUnknown(),FreeVars,strict_ambiguous,warn_upcasting);
      if(!Type) {return NULL;}
      NewArguments=ATinsert(NewArguments,(ATerm)Arg);
      NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
    }
    Arguments=ATreverse(NewArguments);
    ATermList ArgumentTypes=ATreverse(NewArgumentTypes);

    //function
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
						&Data,gsMakeSortUnknown()/*gsMakeSortArrow(ArgumentTypes,PosType)*/,FreeVars,false,nArguments,warn_upcasting);

    gsDebugMsg("Result of gstcTraverseVarConsTypeDN: DataTerm %T\n",Data);

    if(!NewType) {
      if(was_ambiguous) was_ambiguous=false;
      else if(gsIsOpId(Data)||gsIsDataVarId(Data)) gstcErrorMsgCannotCast(ATAgetArgument(Data,1),Arguments,ArgumentTypes);
      gsErrorMsg("type error while trying to cast %P to type %P\n",gsMakeDataAppl(Data,Arguments),PosType);
      return NULL;
    }

    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.

    if(gsIsSortArrow(gstcUnwindType(NewType))){
      ATermList NeededArgumentTypes=ATLgetArgument(gstcUnwindType(NewType),0);

      gsDebugMsg("Arguments again: NeededArgumentTypes: %P, ArgumentTypes: %P\n",NeededArgumentTypes,ArgumentTypes);

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
	  ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg,warn_upcasting);
	  if(CastedNewType) Type=CastedNewType;
        }
	if(!gstcEqTypesA(NeededType,Type)){
	  gsDebugMsg("Doing again on %T, Type: %T, Needed type: %T\n",Arg,Type,NeededType);
	  ATermAppl NewArgType=gstcTypeMatchA(NeededType,Type);
	  if(!NewArgType) NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
	  //if(!NewArgType) {gsErrorMsg("needed type %P does not match possible type %P (while typechecking %P in %P)\n",NeededType,Type,Arg,*DataTerm);return NULL;}
	  if(!NewArgType) NewArgType=NeededType;
	  NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,NewArgType,FreeVars,strict_ambiguous,warn_upcasting);
	  gsDebugMsg("Result of Doing again gstcTraverseVarConsTypeD: DataTerm %T\n",Arg);
          if(!NewArgType) {gsErrorMsg("needed type %P does not match possible type %P (while typechecking %P in %P)\n",NeededType,Type,Arg,*DataTerm);return NULL;}
          Type=NewArgType;
        }
	NewArguments=ATinsert(NewArguments,(ATerm)Arg);
	NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
      }
      Arguments=ATreverse(NewArguments);
      ArgumentTypes=ATreverse(NewArgumentTypes);
    }

    //the function again
    NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                                &Data,gsMakeSortArrow(ArgumentTypes,PosType),FreeVars,strict_ambiguous,nArguments,warn_upcasting);

    gsDebugMsg("Result of gstcTraverseVarConsTypeDN: DataTerm %T\n",Data);

    if(!NewType) {
      if(was_ambiguous) was_ambiguous=false;
      else gstcErrorMsgCannotCast(ATAgetArgument(Data,1),Arguments,ArgumentTypes);
      gsErrorMsg("type error while trying to cast %P to type %P\n",gsMakeDataAppl(Data,Arguments),PosType);
      return NULL;
    }

    gsDebugMsg("Arguments once more: Arguments %T, ArgumentTypes: %T, NewType: %T\n",Arguments,ArgumentTypes,NewType);

    //and the arguments once more
    if(gsIsSortArrow(gstcUnwindType(NewType))){
      ATermList NeededArgumentTypes=ATLgetArgument(gstcUnwindType(NewType),0);
      ATermList NewArgumentTypes=ATmakeList0();
      ATermList NewArguments=ATmakeList0();
      for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments),
            ArgumentTypes=ATgetNext(ArgumentTypes),NeededArgumentTypes=ATgetNext(NeededArgumentTypes)){
        ATermAppl Arg=ATAgetFirst(Arguments);
        ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
        ATermAppl Type=ATAgetFirst(ArgumentTypes);

        if(!gstcEqTypesA(NeededType,Type)){
          //upcasting
          ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg,warn_upcasting);
          if(CastedNewType) Type=CastedNewType;
        }
        if(!gstcEqTypesA(NeededType,Type)){
          gsDebugMsg("Doing again on %T, Type: %T, Needed type: %T\n",Arg,Type,NeededType);
          ATermAppl NewArgType=gstcTypeMatchA(NeededType,Type);
          if(!NewArgType) NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          //if(!NewArgType) {gsErrorMsg("needed type %P does not match possible type %P (while typechecking %P in %P)\n",NeededType,Type,Arg,*DataTerm);return NULL;}
          if(!NewArgType) NewArgType=NeededType;
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,NewArgType,FreeVars,strict_ambiguous,warn_upcasting);
          if(!NewArgType) {gsErrorMsg("needed type %P does not match possible type %P (while typechecking %P in %P)\n",NeededType,Type,Arg,*DataTerm);return NULL;}
          Type=NewArgType;
          }

        NewArguments=ATinsert(NewArguments,(ATerm)Arg);
        NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
      }
      Arguments=ATreverse(NewArguments);
      ArgumentTypes=ATreverse(NewArgumentTypes);
    }

    gsDebugMsg("Arguments after once more: Arguments %T, ArgumentTypes: %T\n",Arguments,ArgumentTypes);

    *DataTerm=gsMakeDataAppl(Data,Arguments);

    if(gsIsSortArrow(gstcUnwindType(NewType))){
      return ATAgetArgument(gstcUnwindType(NewType),1);
    }

    return gstcUnArrowProd(ArgumentTypes,NewType);
  }

  if(gsIsId(*DataTerm)||gsIsOpId(*DataTerm)||gsIsDataVarId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    if(gsIsNumericString(gsATermAppl2String(Name)))
    {
      ATermAppl Sort=gsMakeSortIdInt();
      if(gstcIsPos(Name)) Sort=gsMakeSortIdPos();
      else if(gstcIsNat(Name)) Sort=gsMakeSortIdNat();
      *DataTerm=gsMakeOpId(Name,Sort);

      if(gstcTypeMatchA(Sort,PosType)) return Sort;

      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Sort,DataTerm,warn_upcasting);
      if(!CastedNewType)
        {gsErrorMsg("cannot (up)cast number %P to type %P\n",*DataTerm, PosType);return NULL;}
      return CastedNewType;
    }

    ATermAppl Type=ATAtableGet(DeclaredVars,(ATerm)Name);
    if(Type){
      gsDebugMsg("Recognised declared variable %P, Type: %P\n",Name,Type);
      *DataTerm=gsMakeDataVarId(Name,Type);

      if(!ATAtableGet(AllowedVars,(ATerm)Name)) {
        gsErrorMsg("variable %P occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side\n", Name);
        return NULL;
      }

      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if(NewType) Type=NewType;
      else{
	//upcasting
	ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
	if(!CastedNewType)
	  {gsErrorMsg("cannot (up)cast variable %P to type %P\n",*DataTerm,PosType);return NULL;}

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
	*DataTerm=gsMakeOpId(Name,gsMakeSortUnknown());
	return gsMakeSortUnknown();
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
      return gstcTraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strict_ambiguous, -1, warn_upcasting);
    }
  }

  // if(gsIsDataVarId(*DataTerm)){
  //   return ATAgetArgument(*DataTerm,1);
  // }

  assert(0);
  return Result;
}

static ATermAppl gstcTraverseVarConsTypeDN(ATermTable DeclaredVars, ATermTable AllowedVars, ATermAppl *DataTerm, ATermAppl PosType, ATermTable FreeVars, bool strict_ambiguous, int nFactPars, bool warn_upcasting){
  // -1 means the number of arguments is not known.
  gsDebugMsg("gstcTraverseVarConsTypeDN: DataTerm %T with PosType %T, nFactPars %d\n",*DataTerm,PosType,nFactPars);
  if(gsIsId(*DataTerm)||gsIsOpId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    bool variable=false;
    ATermAppl Type=ATAtableGet(DeclaredVars,(ATerm)Name);
    if(Type){
      variable=true;
      if(!ATAtableGet(AllowedVars,(ATerm)Name)) {
        gsErrorMsg("variable %P occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side\n", Name);
        return NULL;
      }

      //Add to free variables list
      if(FreeVars)
	ATtablePut(FreeVars, (ATerm)Name, (ATerm)Type);
    }
    ATermList ParList;

    if(nFactPars==0){
      if((Type=ATAtableGet(DeclaredVars,(ATerm)Name))) {
        if(!gstcTypeMatchA(Type,PosType)){
          gsErrorMsg("the type %P of variable %P is incompatible with %P (typechecking %P)\n",Type,Name,PosType,*DataTerm);
          return NULL;
        }
        *DataTerm=gsMakeDataVarId(Name,Type);
        return Type;
      }
      else if((Type=ATAtableGet(context.constants,(ATerm)Name))) {
        if(!gstcTypeMatchA(Type,PosType)){
          gsErrorMsg("the type %P of constant %P is incompatible with %P (typechecking %P)\n",Type,Name,PosType,*DataTerm);
          return NULL;
        }
        *DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else{
	if((ParList=ATLtableGet(gssystem.constants,(ATerm)Name))){
	  if(ATgetLength(ParList)==1){
	    ATermAppl Type=ATAgetFirst(ParList);
	    *DataTerm=gsMakeOpId(Name,Type);
	    return Type;
	  }
	  else{
	    gsWarningMsg("ambiguous system constant %P\n",Name);
	    *DataTerm=gsMakeOpId(Name,gsMakeSortUnknown());
	    return Type;
	  }
	}
	else{
	  gsErrorMsg("unknown constant %P\n",Name);
	  return NULL;
	}
      }
    }

    if(Type) {
      ParList=ATmakeList1((ATerm)gstcUnwindType(Type));
    }
    else {
      ATermList ParListS=ATLtableGet(gssystem.functions,(ATerm)Name);
      ParList=ATLtableGet(context.functions,(ATerm)Name);
      if(!ParList) ParList=ParListS;
      else if(ParListS) ParList=ATconcat(ParListS,ParList);
    }

    if(!ParList) {
      if(nFactPars>=0) gsErrorMsg("unknown operation %P with %d parameter%s\n",Name, nFactPars, (nFactPars != 1)?"s":"");
      else gsErrorMsg("unknown operation %P\n",Name);
      return NULL;
    }
    gsDebugMsg("Possible types for Op/Var %T with %d argument%s are (ParList: %T; PosType: %T)\n",
      Name, nFactPars, (nFactPars != 1)?"s":"", ParList, PosType);

    ATermList CandidateParList=ParList;

    { // filter ParList keeping only functions A_0#...#A_nFactPars->A
      ATermList NewParList;
      if(nFactPars>=0){
        NewParList=ATmakeList0();
        for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	  ATermAppl Par=ATAgetFirst(ParList);
	  if(!gsIsSortArrow(Par)) continue;
	  if(!(ATgetLength(ATLgetArgument(Par,0))==(unsigned int)nFactPars)) continue;
	  NewParList=ATinsert(NewParList,(ATerm)Par);
        }
        ParList=ATreverse(NewParList);
      }

      if(!ATisEmpty(ParList)) CandidateParList=ParList;

      // filter ParList keeping only functions of the right type
      ATermList BackupParList=ParList;
      NewParList=ATmakeList0();
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if((Par=gstcTypeMatchA(Par,PosType))) {
	  NewParList=ATinsertUnique(NewParList,(ATerm)Par);
        }
      }
      NewParList=ATreverse(NewParList);

      gsDebugMsg("Possible matches w/o casting for Op/Var %T with %d argument%s are (ParList: %T; PosType: %T)\n",
        Name, nFactPars, (nFactPars != 1)?"s":"", NewParList, PosType);

      if(ATisEmpty(NewParList)){
	//Ok, this looks like a type error. We are not that strict.
	//Pos can be Nat, or even Int...
	//So lets make PosType more liberal
	//We change every Pos to NotInferred(Pos,Nat,Int)...
	//and get the list. Then we take the min of the list.

	ParList=BackupParList;
        gsDebugMsg("Trying casting for Op %T with %d argument%s (ParList: %T; PosType: %T)\n",
          Name, nFactPars, (nFactPars != 1)?"s":"", ParList, PosType);
	PosType=gstcExpandNumTypesUp(PosType);
	for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	  ATermAppl Par=ATAgetFirst(ParList);
	  if((Par=gstcTypeMatchA(Par,PosType)))
	    NewParList=ATinsertUnique(NewParList,(ATerm)Par);
	}
	NewParList=ATreverse(NewParList);
        gsDebugMsg("The result of casting is %T\n",NewParList);
	if(ATgetLength(NewParList)>1) NewParList=ATmakeList1((ATerm)gstcMinType(NewParList));
      }

      if(ATisEmpty(NewParList)){
	//Ok, casting of the arguments did not help.
	//Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

	ParList=BackupParList;
	gsDebugMsg("Trying result casting for Op %T with %d argument%s (ParList: %T; PosType: %T)\n",
          Name, nFactPars, (nFactPars != 1)?"s":"", ParList, PosType);
	PosType=gstcExpandNumTypesDown(gstcExpandNumTypesUp(PosType));
	for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	  ATermAppl Par=ATAgetFirst(ParList);
	  if((Par=gstcTypeMatchA(Par,PosType)))
	    NewParList=ATinsertUnique(NewParList,(ATerm)Par);
	}
	NewParList=ATreverse(NewParList);
	gsDebugMsg("The result of casting is %T\n",NewParList);
	if(ATgetLength(NewParList)>1) NewParList=ATmakeList1((ATerm)gstcMinType(NewParList));
      }

      ParList=NewParList;
    }

    if(ATisEmpty(ParList)) {
      //provide some information to the upper layer for a better error message
      ATermAppl Sort;
      if(ATgetLength(CandidateParList)==1) Sort=ATAgetFirst(CandidateParList); else Sort=gsMakeSortsPossible(CandidateParList);
      *DataTerm=gsMakeOpId(Name,Sort);
      if(nFactPars>=0) gsErrorMsg("unknown operation/variable %P with %d argument%s that matches type %P\n",
        Name, nFactPars, (nFactPars != 1)?"s":"", PosType);
      else
        gsErrorMsg("unknown operation/variable %P that matches type %P\n",Name,PosType);
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

      if ( ATisEqual(gsMakeOpIdNameEq(),ATAgetArgument(*DataTerm,0))
        || ATisEqual(gsMakeOpIdNameNeq(),ATAgetArgument(*DataTerm,0))
        || ATisEqual(gsMakeOpIdNameLT(),ATAgetArgument(*DataTerm,0))
        || ATisEqual(gsMakeOpIdNameLTE(),ATAgetArgument(*DataTerm,0))
        || ATisEqual(gsMakeOpIdNameGTE(),ATAgetArgument(*DataTerm,0))
        || ATisEqual(gsMakeOpIdNameGT(),ATAgetArgument(*DataTerm,0))
      ) {
	gsDebugMsg("Doing ==, !=, <, <=, >= or > matching Type %T, PosType %T\n",Type,PosType);
	ATermAppl NewType=gstcMatchEqNeqComparison(Type);
	if(!NewType){
	  gsErrorMsg("the function %P has incompatible argument types %P (while typechecking %P)\n",ATAgetArgument(*DataTerm,0),Type,*DataTerm);
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
	gsDebugMsg("Doing {List,Set,Bag} matching Type %T, PosType %T\n",Type,PosType);
	ATermAppl NewType=gstcMatchListSetBagOpIn(Type);
	if(!NewType){
	  gsErrorMsg("the function {List,Set,Bag}In has incompatible argument types %P (while typechecking %P)\n",Type,*DataTerm);
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
      if(variable) *DataTerm=gsMakeDataVarId(Name,Type);

      assert(Type);
      return Type;
    }
    else{
      was_ambiguous=true;
      if(strict_ambiguous){
        gsDebugMsg("ambiguous operation %P (ParList %T)\n",Name,ParList);
	if(nFactPars>=0) gsErrorMsg("ambiguous operation %P with %d parameter%s\n", Name, nFactPars, (nFactPars != 1)?"s":"");
        else gsErrorMsg("ambiguous operation %P\n", Name);
        return NULL;
      }
      else{
        //*DataTerm=gsMakeOpId(Name,gsMakeSortUnknown());
	//if(variable) *DataTerm=gsMakeDataVarId(Name,gsMakeSortUnknown());
        return gsMakeSortUnknown();
      }
    }
  }
  else {
    return gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,DataTerm,PosType,FreeVars,strict_ambiguous,warn_upcasting);
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
  int nFormPars=ATgetLength((ATermList)ATgetFirst(TypeListList));
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

  for(int i=nFormPars-1;i>=0;i--){
    ATermAppl Sort;
    if(ATgetLength(Pars[i])==1) Sort=ATAgetFirst(Pars[i]); else Sort=gsMakeSortsPossible(ATreverse(Pars[i]));
    Result=ATinsert(Result,(ATerm)Sort);
  }
  free(Pars);
  return Result;
}

static ATermAppl gstcUpCastNumericType(ATermAppl NeededType, ATermAppl Type, ATermAppl *Par, bool warn_upcasting){
  // Makes upcasting from Type to Needed Type for Par. Returns the resulting type

  if(gsIsSortUnknown(Type)) return Type;
  if(gsIsSortUnknown(NeededType)) return Type;
  if(gstcEqTypesA(NeededType,Type)) return Type;

  if(warn_upcasting && gsIsOpId(*Par) && gsIsNumericString(gsATermAppl2String(ATAgetArgument(*Par,0)))) {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  if(gstcTypeMatchA(NeededType,gsMakeSortIdPos())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())) return gsMakeSortIdPos();
  }

  // Try Upcasting to Nat
  if(gstcTypeMatchA(NeededType,gsMakeSortIdNat())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(gsMakeOpIdPos2Nat(),ATmakeList1((ATerm)*Par));
      if(warn_upcasting){ was_warning_upcasting=true; gsWarningMsg("Upcasting %P to sort Nat by applying Pos2Nat to it.\n",OldPar);}
      return gsMakeSortIdNat();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdNat())) return gsMakeSortIdNat();
  }

  // Try Upcasting to Int
  if(gstcTypeMatchA(NeededType,gsMakeSortIdInt())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
      ATermAppl OldPar=*Par;
       *Par=gsMakeDataAppl(gsMakeOpIdPos2Int(),ATmakeList1((ATerm)*Par));
      if(warn_upcasting) { was_warning_upcasting=true; gsWarningMsg("Upcasting %P to sort Int by applying Pos2Int to it.\n",OldPar);}
      return gsMakeSortIdInt();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdNat())){
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(gsMakeOpIdNat2Int(),ATmakeList1((ATerm)*Par));
      if(warn_upcasting) { was_warning_upcasting=true; gsWarningMsg("Upcasting %P to sort Int by applying Nat2Int to it.\n",OldPar);}
      return gsMakeSortIdInt();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdInt())) return gsMakeSortIdInt();
  }

  // Try Upcasting to Real
  if(gstcTypeMatchA(NeededType,gsMakeSortIdReal())){
    if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
      ATermAppl OldPar=*Par;
       *Par=gsMakeDataAppl(gsMakeOpIdPos2Real(),ATmakeList1((ATerm)*Par));
      if(warn_upcasting) { was_warning_upcasting=true; gsWarningMsg("Upcasting %P to sort Real by applying Pos2Real to it.\n",OldPar);}
      return gsMakeSortIdReal();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdNat())){
      ATermAppl OldPar=*Par;
       *Par=gsMakeDataAppl(gsMakeOpIdNat2Real(),ATmakeList1((ATerm)*Par));
      if(warn_upcasting) { was_warning_upcasting=true; gsWarningMsg("Upcasting %P to sort Real by applying Nat2Real to it.",OldPar);}
      return gsMakeSortIdReal();
    }
    if(gstcTypeMatchA(Type,gsMakeSortIdInt())){
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(gsMakeOpIdInt2Real(),ATmakeList1((ATerm)*Par));
      if(warn_upcasting) { was_warning_upcasting=true; gsWarningMsg("Upcasting %P to sort Real by applying Int2Real to it.\n",OldPar);}
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
    else {
      return NULL;
    }
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
  //Checks if Type is allowed by PosType
  if(gsIsSortUnknown(PosType)) return ATtrue;
  if(gsIsSortsPossible(PosType))
    return gstcInTypesA(Type,ATLgetArgument(PosType,0));

  //PosType is a normal type
  return gstcEqTypesA(Type,PosType);
}

static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType){
  //Checks if Type is allowed by PosType and returns the matching subtype of Type

  gsDebugMsg("gstcTypeMatchA Type: %T;    PosType: %T \n",Type,PosType);

  if(gsIsSortUnknown(Type)) return PosType;
  if(gsIsSortUnknown(PosType) || gstcEqTypesA(Type,PosType)) return Type;
  if(gsIsSortsPossible(Type) && !gsIsSortsPossible(PosType)) {ATermAppl TmpType=PosType; PosType=Type; Type=TmpType; }
  if(gsIsSortsPossible(PosType)){
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

  if(gsIsSortCons(Type))
  {
    ATermAppl ConsType = ATAgetArgument(Type, 0);
    if(gsIsSortList(ConsType))
    {
      if(!gsIsSortExprList(PosType)) return NULL;
      ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
      if(!Res) return NULL;
      return gsMakeSortExprList(Res);
    }

    if(gsIsSortSet(ConsType))
    {
      if(!gsIsSortExprSet(PosType)) return NULL;
      else {
        ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
        if(!Res) return NULL;
        return gsMakeSortExprSet(Res);
      }
    }

    if(gsIsSortBag(ConsType))
    {
      if(!gsIsSortExprBag(PosType)) return NULL;
      else {
        ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
        if(!Res) return NULL;
        return gsMakeSortExprBag(Res);
      }
    }
  }

  if(gsIsSortArrow(Type)){
    if(!gsIsSortArrow(PosType)) return NULL;
    else{
      ATermList ArgTypes=gstcTypeMatchL(ATLgetArgument(Type,0),ATLgetArgument(PosType,0));
      if(!ArgTypes) return NULL;
      ATermAppl ResType=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
      if(!ResType) return NULL;
      Type=gsMakeSortArrow(ArgTypes,ResType);
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
    if(gsIsNotInferred(Type)) return ATtrue;
  }
  return ATfalse;
}

static ATermAppl gstcUnwindType(ATermAppl Type){
  gsDebugMsg("gstcUnwindType Type: %T\n",Type);

  if(gsIsSortCons(Type)) return ATsetArgument(Type,(ATerm)gstcUnwindType(ATAgetArgument(Type,1)),1);
  if(gsIsSortArrow(Type)){
    Type=ATsetArgument(Type,(ATerm)gstcUnwindType(ATAgetArgument(Type,1)),1);
    ATermList Args=ATLgetArgument(Type,0);
    ATermList NewArgs=ATmakeList0();
    for(;!ATisEmpty(Args);Args=ATgetNext(Args)){
      NewArgs=ATinsert(NewArgs,(ATerm)gstcUnwindType(ATAgetFirst(Args)));
    }
    NewArgs=ATreverse(NewArgs);
    Type=ATsetArgument(Type,(ATerm)NewArgs,0);
    return Type;
  }

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
  if(gsIsSortExprSet(PosType)) return ATAgetArgument(PosType,1);
  if(gsIsSortUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gsIsSortsPossible(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,0);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortExprSet(NewPosType)) NewPosType=ATAgetArgument(NewPosType,1);
      else if(!gsIsSortUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gsMakeSortsPossible(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcUnBag(ATermAppl PosType){
  //select Bag(Type), elements, return their list of arguments.
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortExprBag(PosType)) return ATAgetArgument(PosType,1);
  if(gsIsSortUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gsIsSortsPossible(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,0);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortExprBag(NewPosType)) NewPosType=ATAgetArgument(NewPosType,1);
      else if(!gsIsSortUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gsMakeSortsPossible(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcUnList(ATermAppl PosType){
  //select List(Type), elements, return their list of arguments.
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortExprList(PosType)) return ATAgetArgument(PosType,1);
  if(gsIsSortUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gsIsSortsPossible(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,0);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortExprList(NewPosType)) NewPosType=ATAgetArgument(NewPosType,1);
      else if(!gsIsSortUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gsMakeSortsPossible(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType){
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //return TypeX if unique, the set of TypeX as NotInferred if many, NULL otherwise

  gsDebugMsg("gstcUnArrowProd: ArgTypes %T with PosType %T\n",ArgTypes,PosType);

  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortArrow(PosType)){
    ATermList PosArgTypes=ATLgetArgument(PosType,0);

   gsDebugMsg("gstcUnArrowProd: PosArgTypes %T \n",PosArgTypes);

    if(ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes)) return NULL;
    if(gstcTypeMatchL(PosArgTypes,ArgTypes)) return ATAgetArgument(PosType,1);
  }
  if(gsIsSortUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gsIsSortsPossible(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,0);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortArrow(PosType)){
	ATermList PosArgTypes=ATLgetArgument(PosType,0);
	if(ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes)) continue;
	if(gstcTypeMatchL(PosArgTypes,ArgTypes)) NewPosType=ATAgetArgument(NewPosType,1);
      }
      else if(!gsIsSortUnknown(NewPosType)) continue;
      NewPosTypes=ATinsertUnique(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gsMakeSortsPossible(NewPosTypes);
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
  if(gsIsSortUnknown(Type)) return ATtrue;
  if(gsIsSortId(Type)) return ATfalse;
  if(gsIsSortCons(Type)) return gstcHasUnknown(ATAgetArgument(Type,1));
  if(gsIsSortStruct(Type)) return ATfalse;

  if(gsIsSortArrow(Type)){
    for(ATermList TypeList=ATLgetArgument(Type,0);!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList))
      if(gstcHasUnknown(ATAgetFirst(TypeList))) return ATtrue;
    return gstcHasUnknown(ATAgetArgument(Type,1));
  }

  return ATtrue;
}

static ATbool gstcIsNumericType(ATermAppl Type){
  //returns true if Type is Bool,Pos,Nat,Int or Real
  //otherwise return fase
  if(gsIsSortUnknown(Type)) return ATfalse;
  return (ATbool)(ATisEqual(gsMakeSortIdBool(),Type)||ATisEqual(gsMakeSortIdPos(),Type)||ATisEqual(gsMakeSortIdNat(),Type)||ATisEqual(gsMakeSortIdInt(),Type)||ATisEqual(gsMakeSortIdReal(),Type));
}

static ATermAppl gstcExpandNumTypesUp(ATermAppl Type){
  //Expand Pos.. to possible bigger types.
  if(gsIsSortUnknown(Type)) return Type;
  if(gstcEqTypesA(gsMakeSortIdPos(),Type)) return gsMakeSortsPossible(ATmakeList4((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gstcEqTypesA(gsMakeSortIdNat(),Type)) return gsMakeSortsPossible(ATmakeList3((ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gstcEqTypesA(gsMakeSortIdInt(),Type)) return gsMakeSortsPossible(ATmakeList2((ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gsIsSortId(Type)) return Type;
  if(gsIsSortCons(Type)) return ATsetArgument(Type,(ATerm)gstcExpandNumTypesUp(ATAgetArgument(Type,1)),1);
  if(gsIsSortStruct(Type)) return Type;

  if(gsIsSortArrow(Type)){
    //the argument types, and if the resulting type is SortArrow -- recursively
    ATermList NewTypeList=ATmakeList0();
    for(ATermList TypeList=ATLgetArgument(Type,0);!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList))
      NewTypeList=ATinsert(NewTypeList,(ATerm)gstcExpandNumTypesUp(gstcUnwindType(ATAgetFirst(TypeList))));
    ATermAppl ResultType=ATAgetArgument(Type,1);
    if(!gsIsSortArrow(ResultType))
      return ATsetArgument(Type,(ATerm)ATreverse(NewTypeList),0);
    else
      return gsMakeSortArrow(ATreverse(NewTypeList),gstcExpandNumTypesUp(gstcUnwindType(ResultType)));
  }

  return Type;
}

static ATermAppl gstcExpandNumTypesDown(ATermAppl Type){
  // Expand Numeric types down
  if(gsIsSortUnknown(Type)) return Type;
  if(gsIsSortId(Type)) Type=gstcUnwindType(Type);

  ATbool function=ATfalse;
  ATermList Args=NULL;
  if(gsIsSortArrow(Type)){
    function=ATtrue;
    Args=ATLgetArgument(Type,0);
    Type=ATAgetArgument(Type,1);
  }

  if(gstcEqTypesA(gsMakeSortIdReal(),Type)) Type=gsMakeSortsPossible(ATmakeList4((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt(),(ATerm)gsMakeSortIdReal()));
  if(gstcEqTypesA(gsMakeSortIdInt(),Type)) Type=gsMakeSortsPossible(ATmakeList3((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt()));
  if(gstcEqTypesA(gsMakeSortIdNat(),Type)) Type=gsMakeSortsPossible(ATmakeList2((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat()));

  return (function)?gsMakeSortArrow(Args,Type):Type;
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
  ATermAppl Res=gstcTypeMatchA(Type1,Type2);
  if(!Res){
    Res=gstcTypeMatchA(Type1,gstcExpandNumTypesUp(Type2));
    if(!Res) Res=gstcTypeMatchA(Type2,gstcExpandNumTypesUp(Type1));
    if(!Res) {gsDebugMsg("gstcUnifyMinType: No match: Type1 %T; Type2 %T; \n",Type1,Type2); return NULL;}
  }

  if(gsIsSortsPossible(Res)) Res=ATAgetFirst(ATLgetArgument(Res,0));
  gsDebugMsg("gstcUnifyMinType: Type1 %T; Type2 %T; Res: %T\n",Type1,Type2,Res);
  return Res;
}

static ATermAppl gstcMatchIf(ATermAppl Type){
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  assert(gsIsSortArrow(Type));
  ATermList Args=ATLgetArgument(Type,0);
  ATermAppl Res=ATAgetArgument(Type,1);
  assert((ATgetLength(Args)==3));
  //assert(gsIsBool(ATAgetFirst(Args)));
  Args=ATgetNext(Args);

  if(!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args)))) return NULL;
  Args=ATgetNext(Args);
  if(!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args)))) return NULL;

  return gsMakeSortArrow(ATmakeList3((ATerm)gsMakeSortIdBool(),(ATerm)Res,(ATerm)Res),Res);
}

static ATermAppl gstcMatchEqNeqComparison(ATermAppl Type){
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if(!Arg) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)Arg,(ATerm)Arg),gsMakeSortIdBool());
}

static ATermAppl gstcMatchListOpCons(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  gsDebugMsg("gstcMatchListOpCons: Type %T \n",Type);

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortExprList(gstcUnwindType(Res)));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortExprList(Arg2));
  Arg2=ATAgetArgument(Arg2,1);

  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)Res,(ATerm)gsMakeSortExprList(Res)),gsMakeSortExprList(Res));
}

static ATermAppl gstcMatchListOpSnoc(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortExprList(Res));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortExprList(Arg1));
  Arg1=ATAgetArgument(Arg1,1);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);

  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)gsMakeSortExprList(Res),(ATerm)Res),gsMakeSortExprList(Res));
}

static ATermAppl gstcMatchListOpConcat(ATermAppl Type){
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortExprList(Res));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortExprList(Arg1));
  Arg1=ATAgetArgument(Arg1,1);

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortExprList(Arg2));
  Arg2=ATAgetArgument(Arg2,1);

  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)gsMakeSortExprList(Res),(ATerm)gsMakeSortExprList(Res)),gsMakeSortExprList(Res));
}

static ATermAppl gstcMatchListOpEltAt(ATermAppl Type){
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortExprList(Arg1));
  Arg1=ATAgetArgument(Arg1,1);

  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  //assert((gsIsSortNat(ATAgetFirst(ATgetNext(Args))));

  return gsMakeSortArrow(ATmakeList2((ATerm)gsMakeSortExprList(Res),(ATerm)gsMakeSortIdNat()),Res);
}

static ATermAppl gstcMatchListOpHead(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortExprList(Arg));
  Arg=ATAgetArgument(Arg,1);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList1((ATerm)gsMakeSortExprList(Res)),Res);
}

static ATermAppl gstcMatchListOpTail(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortExprList(Res));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortExprList(Arg));
  Arg=ATAgetArgument(Arg,1);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList1((ATerm)gsMakeSortExprList(Res)),gsMakeSortExprList(Res));
}

//Sets
static ATermAppl gstcMatchSetOpSet2Bag(ATermAppl Type){
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortExprBag(Res));
  Res=ATAgetArgument(Res,1);

  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortExprSet(Arg));
  Arg=ATAgetArgument(Arg,1);

  Arg=gstcUnifyMinType(Arg,Res);
  if(!Arg) return NULL;

  return gsMakeSortArrow(ATmakeList1((ATerm)gsMakeSortExprSet(Arg)),gsMakeSortExprBag(Arg));
}

static ATermAppl gstcMatchListSetBagOpIn(ATermAppl Type){
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  //assert(gsIsBool(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortCons(Arg2));
  ATermAppl Arg2s=ATAgetArgument(Arg2,1);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2s);
  if(!Arg) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)Arg,(ATerm)ATsetArgument(Arg2,(ATerm)Arg,1)),gsMakeSortIdBool());
}

static ATermAppl gstcMatchSetBagOpUnionDiffIntersect(ATermAppl Type){
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  if(gstcIsNumericType(Res)) return Type;
  assert(gsIsSortExprSet(Res)||gsIsSortExprBag(Res));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  if(gstcIsNumericType(Arg1)) return Type;
  assert(gsIsSortExprSet(Arg1)||gsIsSortExprBag(Arg1));

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  if(gstcIsNumericType(Arg2)) return Type;
  assert(gsIsSortExprSet(Arg2)||gsIsSortExprBag(Arg2));

  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)Res,(ATerm)Res),Res);
}

static ATermAppl gstcMatchSetOpSetCompl(ATermAppl Type){
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  if(gstcIsNumericType(Res)) return Type;
  assert(gsIsSortExprSet(Res));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  if(gstcIsNumericType(Arg)) return Type;
  assert(gsIsSortExprSet(Arg));
  Arg=ATAgetArgument(Arg,1);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrow(ATmakeList1((ATerm)gsMakeSortExprSet(Res)),gsMakeSortExprSet(Res));
}

//Bags
static ATermAppl gstcMatchBagOpBag2Set(ATermAppl Type){
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortExprSet(Res));
  Res=ATAgetArgument(Res,1);

  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortExprBag(Arg));
  Arg=ATAgetArgument(Arg,1);

  Arg=gstcUnifyMinType(Arg,Res);
  if(!Arg) return NULL;

  return gsMakeSortArrow(ATmakeList1((ATerm)gsMakeSortExprBag(Arg)),gsMakeSortExprSet(Arg));
}

static ATermAppl gstcMatchBagOpBagCount(ATermAppl Type){
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  //If the second argument is not a Bag, don't match

  if(!gsIsSortArrow(Type)) return Type;
  //assert(gsIsNat(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  if(!(ATgetLength(Args)==2)) return Type;

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  if(!gsIsSortExprBag(Arg2)) return Type;
  Arg2=ATAgetArgument(Arg2,1);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if(!Arg) return NULL;

  return gsMakeSortArrow(ATmakeList2((ATerm)Arg,(ATerm)gsMakeSortExprBag(Arg)),gsMakeSortIdNat());
}

static void gstcErrorMsgCannotCast(ATermAppl CandidateType, ATermList Arguments, ATermList ArgumentTypes){
  //prints more information about impossible cast.
  //at this point we know that Arguments cannot be cast to CandidateType. We need to find out why and print.
  assert (ATgetLength(Arguments)==ATgetLength(ArgumentTypes));

  gsVerboseMsg("CandidateType: %T, Arguments %T, ArgumentTypes %T\n",CandidateType,Arguments,ArgumentTypes);

  ATermList CandidateList;
  if(gsIsSortsPossible(CandidateType)) CandidateList=ATLgetArgument(CandidateType,0);
  else CandidateList=ATmakeList1((ATerm)CandidateType);

  ATermList NewCandidateList=ATmakeList0();
  for(ATermList l=CandidateList;!ATisEmpty(l);l=ATgetNext(l)){
    NewCandidateList=ATinsert(NewCandidateList,(ATerm)ATLgetArgument(ATAgetFirst(l),0));
  }
  CandidateList=ATreverse(NewCandidateList);

  //CandidateList=gstcTraverseListList(CandidateList);
  ATermList CurrentCandidateList=CandidateList;
  CandidateList=ATmakeList0();
  while(true){
    ATermList NewCurrentCandidateList=ATmakeList0();
    ATermList NewList=ATmakeList0();
    for(ATermList l=CurrentCandidateList;!ATisEmpty(l);l=ATgetNext(l)){
      ATermList List=ATLgetFirst(l);
      if(!ATisEmpty(List)) {
        NewList=ATinsert(NewList,(ATerm)ATAgetFirst(List));
        NewCurrentCandidateList=ATinsertUnique(NewCurrentCandidateList,(ATerm)ATgetNext(List));
      }
      else {
        NewCurrentCandidateList=ATinsert(NewCurrentCandidateList,(ATerm)ATmakeList0());
      }
    }
    if(ATisEmpty(NewList)) break;
    CurrentCandidateList=ATreverse(NewCurrentCandidateList);
    CandidateList=ATinsert(CandidateList,(ATerm)ATreverse(NewList));
  }
  CandidateList=ATreverse(CandidateList);

  //gsVerboseMsg("CandidateList: %T, Arguments %T, ArgumentTypes %T\n",CandidateList,Arguments,ArgumentTypes);

  for(ATermList l=Arguments, m=ArgumentTypes, n=CandidateList;!(ATisEmpty(l)||ATisEmpty(m)||ATisEmpty(n));l=ATgetNext(l), m=ATgetNext(m), n=ATgetNext(n)){
    ATermList PosTypes=ATLgetFirst(n);
    ATermAppl NeededType=ATAgetFirst(m);
    bool found=true;
    for(ATermList k=PosTypes;!ATisEmpty(k);k=ATgetNext(k)){
      if(gstcTypeMatchA(ATAgetFirst(k),NeededType)){
        found=false;
        break;
      }
    }
    if(found) {
      ATermAppl Sort;
      if(ATgetLength(PosTypes)==1) Sort=ATAgetFirst(PosTypes); else Sort=gsMakeSortsPossible(PosTypes);
      gsErrorMsg("this is, for instance, because cannot cast %P to type %P\n",ATAgetFirst(l),Sort);
      break;
    }
  }
}

//===================================
// Type checking modal formulas
//===================================

static ATermAppl gstcTraverseStateFrm(ATermTable Vars, ATermTable StateVars, ATermAppl StateFrm){
  gsDebugMsg("gstcTraverseStateFrm: %T\n",StateFrm);

  if(gsIsStateTrue(StateFrm) || gsIsStateFalse(StateFrm) || gsIsStateDelay(StateFrm) || gsIsStateYaled(StateFrm))
    return StateFrm;

  if(gsIsStateNot(StateFrm)){
    ATermAppl NewArg=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,0));
    if(!NewArg) return NULL;
    return ATsetArgument(StateFrm,(ATerm)NewArg,0);
  }

  if(gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm) || gsIsStateImp(StateFrm)){
    ATermAppl NewArg1=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,0));
    if(!NewArg1) return NULL;
    ATermAppl NewArg2=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,1));
    if(!NewArg2) return NULL;
    return ATsetArgument(ATsetArgument(StateFrm,(ATerm)NewArg1,0),(ATerm)NewArg2,1);
  }

  if(gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=ATLgetArgument(StateFrm,0);
    ATermTable NewVars=gstcAddVars2Table(CopyVars,VarList);
    if(!NewVars) {ATtableDestroy(CopyVars); return NULL;}

    ATermAppl NewArg2=gstcTraverseStateFrm(NewVars,StateVars,ATAgetArgument(StateFrm,1));
    if(!NewArg2) return NULL;
    ATtableDestroy(CopyVars);

    return ATsetArgument(StateFrm,(ATerm)NewArg2,1);
  }

  if(gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm)){
    ATermAppl RegFrm=gstcTraverseRegFrm(Vars,ATAgetArgument(StateFrm,0));
    if(!RegFrm) return NULL;
    ATermAppl NewArg2=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,1));
    if(!NewArg2) return NULL;
    return ATsetArgument(ATsetArgument(StateFrm,(ATerm)RegFrm,0),(ATerm)NewArg2,1);
  }

  if(gsIsStateDelayTimed(StateFrm) || gsIsStateYaledTimed(StateFrm)){
   ATermAppl Time=ATAgetArgument(StateFrm,0);
   ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(gsMakeSortIdReal()));
   if(!NewType) {return NULL;}

   if(!gstcTypeMatchA(gsMakeSortIdReal(),NewType)){
     //upcasting
     ATermAppl CastedNewType=gstcUpCastNumericType(gsMakeSortIdReal(),NewType,&Time);
     if(!CastedNewType)
       {gsErrorMsg("cannot (up)cast time value %P to type Real (typechecking state formula %P)\n",Time,StateFrm);return NULL;}
   }
   return ATsetArgument(StateFrm,(ATerm)Time,0);
  }

  if(gsIsStateVar(StateFrm)){
    ATermAppl StateVarName=ATAgetArgument(StateFrm,0);
    ATermList TypeList=ATLtableGet(StateVars,(ATerm)StateVarName);
    if(!TypeList){
      gsErrorMsg("undefined state variable %P (typechecking state formula %P)\n",StateVarName,StateFrm);
      return NULL;
    }

    ATermList Pars=ATLgetArgument(StateFrm,1);
    if(ATgetLength(TypeList)!=ATgetLength(Pars)){
      gsErrorMsg("incorrect number of parameters for state variable %P (typechecking state formula %P)\n",StateVarName,StateFrm);
      return NULL;
    }

    ATermList r=ATmakeList0();
    ATbool success=ATtrue;
    for(;!ATisEmpty(Pars);Pars=ATgetNext(Pars),TypeList=ATgetNext(TypeList)){
      ATermAppl Par=ATAgetFirst(Pars);
      ATermAppl ParType=ATAgetFirst(TypeList);
      ATermAppl NewParType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,gstcExpandNumTypesDown(ParType));
      if(!NewParType){
        gsErrorMsg("typechecking %P\n",StateFrm);
        success=ATfalse;
        break;
      }

      if(!gstcTypeMatchA(ParType,NewParType)){
        //upcasting
        NewParType=gstcUpCastNumericType(ParType,NewParType,&Par);
        if(!NewParType){
          gsErrorMsg("cannot (up)cast %P to type %P (typechecking state formula %P)\n",Par,ParType,StateFrm);
          success=ATfalse;
          break;
        }
      }

      r=ATinsert(r,(ATerm)Par);
    }

    if(!success) return NULL;

    return ATsetArgument(StateFrm,(ATerm)ATreverse(r),1);

  }

  if(gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm)){
    ATermTable CopyStateVars=ATtableCreate(63,50);
    gstcATermTableCopy(StateVars,CopyStateVars);

    // Make the new state variable:
    ATermTable FormPars=ATtableCreate(63,50);
    ATermList r=ATmakeList0();
    ATermList t=ATmakeList0();
    ATbool success=ATtrue;
    for(ATermList l=ATLgetArgument(StateFrm,1);!ATisEmpty(l);l=ATgetNext(l)){
      ATermAppl o=ATAgetFirst(l);

      ATermAppl VarName=ATAgetArgument(ATAgetArgument(o,0),0);
      if(ATAtableGet(FormPars,(ATerm)VarName)){
        gsErrorMsg("non-unique formal parameter %P (typechecking %P)\n",VarName,StateFrm);
        success=ATfalse;
        break;
      }

      ATermAppl VarType=ATAgetArgument(ATAgetArgument(o,0),1);
      if(!gstcIsSortExprDeclared(VarType)){
        gsErrorMsg("the previous type error occurred while typechecking %P\n",StateFrm);
        success=ATfalse;
        break;
      }

      ATtablePut(FormPars,(ATerm)VarName, (ATerm)VarType);

      ATermAppl VarInit=ATAgetArgument(o,1);
      ATermAppl VarInitType=gstcTraverseVarConsTypeD(Vars,Vars,&VarInit,gstcExpandNumTypesDown(VarType));
      if(!VarInitType){
        gsErrorMsg("typechecking %P\n",StateFrm);
        success=ATfalse;
        break;
      }

      if(!gstcTypeMatchA(VarType,VarInitType)){
        //upcasting
        VarInitType=gstcUpCastNumericType(VarType,VarInitType,&VarInit);
        if(!VarInitType){
          gsErrorMsg("cannot (up)cast %P to type %P (typechecking state formula %P)\n",VarInit,VarType,StateFrm);
          success=ATfalse;
          break;
        }
      }

      r=ATinsert(r,(ATerm)ATsetArgument(o,(ATerm)VarInit,1));
      t=ATinsert(t,(ATerm)VarType);
    }

    if(!success){
      ATtableDestroy(CopyStateVars);
      ATtableDestroy(FormPars);
      return NULL;
    }

    StateFrm=ATsetArgument(StateFrm,(ATerm)ATreverse(r),1);
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);
    gstcATermTableCopy(FormPars,CopyVars);
    ATtableDestroy(FormPars);

    ATtablePut(CopyStateVars,(ATerm)ATAgetArgument(StateFrm,0),(ATerm)ATreverse(t));
    ATermAppl NewArg=gstcTraverseStateFrm(CopyVars,CopyStateVars,ATAgetArgument(StateFrm,2));
    ATtableDestroy(CopyStateVars);
    ATtableDestroy(CopyVars);
    if(!NewArg) {gsErrorMsg("while typechecking %P\n",StateFrm);return NULL;}
    return ATsetArgument(StateFrm,(ATerm)NewArg,2);
  }

  if(gsIsDataExpr(StateFrm)){
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars, Vars, &StateFrm, gsMakeSortIdBool());
    if(!Type) return NULL;
    return StateFrm;
  }

  assert(0);
  return NULL;
}

static ATermAppl gstcTraverseRegFrm(ATermTable Vars, ATermAppl RegFrm){
  gsDebugMsg("gstcTraverseRegFrm: %T\n",RegFrm);
  if(gsIsRegNil(RegFrm)){
    return RegFrm;
  }

  if(gsIsRegSeq(RegFrm) || gsIsRegAlt(RegFrm)){
    ATermAppl NewArg1=gstcTraverseRegFrm(Vars,ATAgetArgument(RegFrm,0));
    if(!NewArg1) return NULL;
    ATermAppl NewArg2=gstcTraverseRegFrm(Vars,ATAgetArgument(RegFrm,1));
    if(!NewArg2) return NULL;
    return ATsetArgument(ATsetArgument(RegFrm,(ATerm)NewArg1,0),(ATerm)NewArg2,1);
  }

  if(gsIsRegTrans(RegFrm) || gsIsRegTransOrNil(RegFrm)){
    ATermAppl NewArg=gstcTraverseRegFrm(Vars,ATAgetArgument(RegFrm,0));
    if(!NewArg) return NULL;
    return ATsetArgument(RegFrm,(ATerm)NewArg,0);
  }

  if(gsIsActFrm(RegFrm))
    return gstcTraverseActFrm(Vars, RegFrm);

  assert(0);
  return NULL;
}

static ATermAppl gstcTraverseActFrm(ATermTable Vars, ATermAppl ActFrm){
  gsDebugMsg("gstcTraverseActFrm: %T\n",ActFrm);


  if(gsIsActTrue(ActFrm) || gsIsActFalse(ActFrm)){
    return ActFrm;
  }

  if(gsIsActNot(ActFrm)){
    ATermAppl NewArg=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,0));
    if(!NewArg) return NULL;
    return ATsetArgument(ActFrm,(ATerm)NewArg,0);
  }

  if(gsIsActAnd(ActFrm) || gsIsActOr(ActFrm) || gsIsActImp(ActFrm)){
    ATermAppl NewArg1=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,0));
    if(!NewArg1) return NULL;
    ATermAppl NewArg2=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,1));
    if(!NewArg2) return NULL;
    return ATsetArgument(ATsetArgument(ActFrm,(ATerm)NewArg1,0),(ATerm)NewArg2,1);
  }

  if(gsIsActForall(ActFrm) || gsIsActExists(ActFrm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=ATLgetArgument(ActFrm,0);
    ATermTable NewVars=gstcAddVars2Table(CopyVars,VarList);
    if(!NewVars) {ATtableDestroy(CopyVars); return NULL;}

    ATermAppl NewArg2=gstcTraverseActFrm(NewVars,ATAgetArgument(ActFrm,1));
    if(!NewArg2) return NULL;
    ATtableDestroy(CopyVars);

    return ATsetArgument(ActFrm,(ATerm)NewArg2,1);
  }

  if(gsIsActAt(ActFrm)){
   ATermAppl NewArg1=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,0));
   if(!NewArg1) return NULL;

   ATermAppl Time=ATAgetArgument(ActFrm,1);
   ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(gsMakeSortIdReal()));
   if(!NewType) {return NULL;}

   if(!gstcTypeMatchA(gsMakeSortIdReal(),NewType)){
     //upcasting
     ATermAppl CastedNewType=gstcUpCastNumericType(gsMakeSortIdReal(),NewType,&Time);
     if(!CastedNewType)
       {gsErrorMsg("cannot (up)cast time value %P to type Real (typechecking action formula %P)\n",Time,ActFrm);return NULL;}
   }
   return ATsetArgument(ATsetArgument(ActFrm,(ATerm)NewArg1,0),(ATerm)Time,1);
  }

  if(gsIsMultAct(ActFrm)){
    ATermList r=ATmakeList0();
    for(ATermList l=ATLgetArgument(ActFrm,0);!ATisEmpty(l);l=ATgetNext(l)){
      ATermAppl o=ATAgetFirst(l);
      assert(gsIsParamId(o));
      o=gstcTraverseActProcVarConstP(Vars,o);
      if(!o) return NULL;
      r=ATinsert(r,(ATerm)o);
    }
    return ATsetArgument(ActFrm,(ATerm)ATreverse(r),0);
  }

  if(gsIsDataExpr(ActFrm)){
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars, Vars, &ActFrm, gsMakeSortIdBool());
    if(!Type) return NULL;
    return ActFrm;
  }

  assert(0);
  return NULL;
}

  }
}
