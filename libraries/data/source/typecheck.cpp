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
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/print.h"
#include "mcrl2/aterm/aterm.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/unknown_sort.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/function_update.h"

using namespace mcrl2::log;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;

namespace mcrl2
{
namespace core
{

// Static data

static bool was_warning_upcasting=false;
static bool was_ambiguous=false;

// system constants and functions
typedef struct
{
  ATermTable constants;   //name -> Set(sort expression)
  ATermTable functions;   //name -> Set(sort expression)
} gsSystem;

static gsSystem gssystem;

static ATermList list_minus(ATermList l, ATermList m)
{
  ATermList n = ATmakeList0();
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    if (ATindexOf(m,ATgetFirst(l)) == ATERM_NON_EXISTING_POSITION)
    {
      n = ATinsert(n,ATgetFirst(l));
    }
  }
  return ATreverse(n);
}

// the static context of the spec will be checked and used, not transformed
typedef struct
{
  ATermIndexedSet basic_sorts;
  ATermTable defined_sorts; //name -> sort expression
  ATermTable constants;   //name -> Set(sort expression)
  ATermTable functions;   //name -> Set(sort expression)
  ATermTable actions;         //name -> Set(List(sort expression)) because of action polymorphism
  ATermTable processes;         //name -> Set(List(sort expression)) because of process polymorphism
  ATermTable glob_vars;   //name -> Type: global variables (for proc, pbes and init)
  ATermTable PBs;
} Context;

static Context context;

// the body may be transformed
typedef struct
{
  ATermList equations;
  ATermTable proc_pars;         //name#type -> List(Vars)
  ATermTable proc_bodies; //name#type -> rhs
} Body;
static Body body;

// Static function declarations
static void gstcDataInit(void);
static void gstcDataDestroy(void);
static bool gstcReadInSorts(ATermList);
static bool gstcReadInConstructors(ATermList NewSorts=NULL);
static bool gstcReadInFuncs(ATermList, ATermList);
static bool gstcReadInActs(ATermList);
static bool gstcReadInProcsAndInit(ATermList, ATermAppl);
static bool gstcReadInPBESAndInit(ATermAppl, ATermAppl);

static bool gstcTransformVarConsTypeData(void);
static bool gstcTransformActProcVarConst(void);
static bool gstcTransformPBESVarConst(void);

static ATermList gstcWriteProcs(ATermList);
static ATermList gstcWritePBES(ATermList);

static bool gstcInTypesA(ATermAppl, ATermList);
static bool gstcEqTypesA(ATermAppl, ATermAppl);
static bool gstcInTypesL(ATermList, ATermList);
static bool gstcEqTypesL(ATermList, ATermList);

static bool gstcIsSortDeclared(ATermAppl SortName);
static bool gstcIsSortExprDeclared(ATermAppl SortExpr);
static bool gstcIsSortExprListDeclared(ATermList SortExprList);
static bool gstcReadInSortStruct(ATermAppl);
static bool gstcAddConstant(ATermAppl, const char*);
static bool gstcAddFunction(ATermAppl, const char*, bool allow_double_decls=false);

static void gstcAddSystemConstant(ATermAppl);
static void gstcAddSystemFunction(ATermAppl);

static void gstcATermTableCopy(const ATermTable &Vars, ATermTable &CopyVars);

static bool gstcAddVars2Table(ATermTable &,ATermList, ATermTable &);
static ATermTable gstcRemoveVars(ATermTable &Vars, ATermList VarDecls);
static bool gstcVarsUnique(ATermList VarDecls);
static ATermAppl gstcRewrActProc(const ATermTable &, ATermAppl, bool is_pbes=false);
static inline ATermAppl gstcMakeActionOrProc(bool, ATermAppl, ATermList, ATermList);
static ATermAppl gstcTraverseActProcVarConstP(const ATermTable &, ATermAppl);
static ATermAppl gstcTraversePBESVarConstPB(const ATermTable &, ATermAppl);

static ATermAppl gstcTraverseVarConsTypeD(const ATermTable &DeclaredVars, const ATermTable &AllowedVars, ATermAppl*, ATermAppl, ATermTable &FreeVars, bool strict_ambiguous=true, const bool warn_upcasting=false, const bool print_cast_error=true);
static ATermAppl gstcTraverseVarConsTypeD(const ATermTable &DeclaredVars, const ATermTable &AllowedVars, ATermAppl* t1, ATermAppl t2)
{
  ATermTable dummy_table;
  return gstcTraverseVarConsTypeD(DeclaredVars, AllowedVars, t1, t2, 
        dummy_table, true, false, true);
}
static ATermAppl gstcTraverseVarConsTypeDN(const ATermTable &DeclaredVars, const ATermTable &AllowedVars, ATermAppl* , ATermAppl, 
               ATermTable &FreeVars, bool strict_ambiguous=true, size_t nPars = ATERM_NON_EXISTING_POSITION, const bool warn_upcasting=false, const bool print_cast_error=true);

/* static ATermAppl gstcTraverseVarConsTypeDN(const ATermTable &DeclaredVars, const ATermTable &AllowedVars, ATermAppl* t1, ATermAppl t2)
{
  ATermTable dummy_empty_table;
  return gstcTraverseVarConsTypeDN(DeclaredVars, AllowedVars, t1, t2, dummy_empty_table, true, ATERM_NON_EXISTING_POSITION, false, true);
} */


static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type);

static inline bool gstcIsPos(ATermAppl Number)
{
  char c=gsATermAppl2String(Number)[0];
  return (bool)(isdigit(c) && c>'0');
}
static inline bool gstcIsNat(ATermAppl Number)
{
  return isdigit(gsATermAppl2String(Number)[0]) != 0;
}

static inline ATermAppl INIT_KEY(void)
{
  return gsMakeProcVarId(gsString2ATermAppl("init"),ATmakeList0());
}

static ATermAppl gstcUpCastNumericType(ATermAppl NeededType, ATermAppl Type, ATermAppl* Par, bool warn_upcasting=false);
static ATermAppl gstcMaximumType(ATermAppl Type1, ATermAppl Type2);

static ATermList gstcGetNotInferredList(ATermList TypeListList);
static ATermList gstcAdjustNotInferredList(ATermList TypeList, ATermList TypeListList);
static bool gstcIsNotInferredL(ATermList TypeListList);
static bool gstcIsTypeAllowedA(ATermAppl Type, ATermAppl PosType);
static bool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList);
static ATermAppl gstcUnwindType(ATermAppl Type);
static ATermAppl gstcUnSet(ATermAppl PosType);
static ATermAppl gstcUnBag(ATermAppl PosType);
static ATermAppl gstcUnList(ATermAppl PosType);
static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType);
static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2);
static ATermList gstcGetVarTypes(ATermList VarDecls);
static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType);
static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList);
static bool gstcHasUnknown(ATermAppl Type);
static bool gstcIsNumericType(ATermAppl Type);
static ATermAppl gstcExpandNumTypesUp(ATermAppl Type);
static ATermAppl gstcExpandNumTypesDown(ATermAppl Type);
static ATermAppl gstcMinType(ATermList TypeList);
static bool gstcMActIn(ATermList MAct, ATermList MActs);
static bool gstcMActEq(ATermList MAct1, ATermList MAct2);
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
static ATermAppl gstcMatchFuncUpdate(ATermAppl Type);
static ATermAppl replace_possible_sorts(ATermAppl Type);


static void gstcErrorMsgCannotCast(ATermAppl CandidateType, ATermList Arguments, ATermList ArgumentTypes);

// Typechecking modal formulas
static ATermAppl gstcTraverseStateFrm(const ATermTable &Vars, const ATermTable &StateVars, ATermAppl StateFrm);
static ATermAppl gstcTraverseRegFrm(const ATermTable &Vars, ATermAppl RegFrm);
static ATermAppl gstcTraverseActFrm(const ATermTable &Vars, ATermAppl ActFrm);


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

static void gstcSplitSortDecls(ATermList SortDecls, ATermList* PSortIds,
                               ATermList* PSortRefs);
//Pre: SortDecls is a list of SortId's and SortRef's
//Post:*PSortIds and *PSortRefs contain the SortId's and SortRef's from
//     SortDecls, in the same order

static ATermAppl gstcUpdateSortSpec(ATermAppl Spec, ATermAppl SortSpec);
//Pre: Spec and SortSpec are resp. specifications and sort specifications that
//     adhere to the internal syntax after type checking
//Ret: Spec in which the sort specification is replaced by SortSpec

///\brief Increases the value of each key in map
///\param[in] m A mapping from an ATerm to a boolean
///\return m in which all values are negated
static inline std::map<atermpp::aterm,bool> neg_values(std::map<atermpp::aterm,bool> m)
{
  for (std::map<atermpp::aterm,bool>::iterator i = m.begin() ; i != m.end(); i++)
  {
    m[i->first] = !i->second;
  }
  return m;
}


//type checking functions
//-----------------------

ATermAppl type_check_data_spec(ATermAppl data_spec)
{
  mCRL2log(verbose) << "type checking data specification..." << std::endl;

  ATermAppl Result=NULL;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking read-in phase started" << std::endl;

  if (gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0)))
  {
    // Check sorts for loops
    // Unwind sorts to enable equiv and subtype relations
    if (gstcReadInConstructors())
    {
      if (gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
                          ATLgetArgument(ATAgetArgument(data_spec,2),0)))
      {
        body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);
        mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

        mCRL2log(debug) << "type checking transform VarConst phase started" << std::endl;

        if (gstcTransformVarConsTypeData())
        {
          mCRL2log(debug) << "type checking transform VarConst phase finished" << std::endl;

          Result = ATsetArgument(data_spec, gsMakeDataEqnSpec(body.equations),3);

          Result = gstcFoldSortRefs(Result);

          mCRL2log(debug) << "type checking phase finished" << std::endl;
        }
      }
    }
  }

  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_proc_spec(ATermAppl proc_spec)
{
  mCRL2log(verbose) << "type checking process specification..." << std::endl;

  ATermAppl Result=NULL;

  mCRL2log(debug) << "type checking phase started: " << core::pp_deprecated(proc_spec) << "" << std::endl;
  gstcDataInit();

  ATermAppl data_spec = ATAgetArgument(proc_spec, 0);
  if (gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0)))
  {
    // Check sorts for loops
    // Unwind sorts to enable equiv and subtype relations
    if (gstcReadInConstructors())
    {
      if (gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
                          ATLgetArgument(ATAgetArgument(data_spec,2),0)))
      {
        body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);
        if (gstcReadInActs(ATLgetArgument(ATAgetArgument(proc_spec,1),0)))
        {
          ATermAppl glob_var_spec = ATAgetArgument(proc_spec,2);
          ATermList glob_vars = ATLgetArgument(glob_var_spec,0);
          ATermTable dummy;
          if (gstcAddVars2Table(context.glob_vars, glob_vars,dummy))
          {
            if (gstcReadInProcsAndInit(ATLgetArgument(ATAgetArgument(proc_spec,3),0),
                                       ATAgetArgument(ATAgetArgument(proc_spec,4),0)))
            {
              mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

              mCRL2log(debug) << "type checking transform ActProc+VarConst phase started" << std::endl;
              if (gstcTransformVarConsTypeData())
              {
                if (gstcTransformActProcVarConst())
                {
                  mCRL2log(debug) << "type checking transform ActProc+VarConst phase finished" << std::endl;

                  data_spec=ATAgetArgument(proc_spec,0);
                  data_spec=ATsetArgument(data_spec, gsMakeDataEqnSpec(body.equations),3);
                  Result=ATsetArgument(proc_spec,data_spec,0);
                  Result=ATsetArgument(Result,gsMakeProcEqnSpec(gstcWriteProcs(ATLgetArgument(ATAgetArgument(proc_spec,3),0))),3);
                  Result=ATsetArgument(Result,gsMakeProcessInit(ATAtableGet(body.proc_bodies,INIT_KEY())),4);

                  Result=gstcFoldSortRefs(Result);

                  mCRL2log(debug) << "type checking phase finished" << std::endl;
                }
              }
            }
          }
        }
      }
    }
  }

  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_sort_expr(ATermAppl sort_expr, ATermAppl spec)
{
  mCRL2log(verbose) << "type checking sort expression..." << std::endl;
  //check correctness of the sort expression in sort_expr
  //using the specification in spec
  assert(gsIsSortExpr(sort_expr));
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));

  ATermAppl Result=NULL;

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of sort expressions read-in phase started" << std::endl;

  ATermAppl data_spec = NULL;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts))
  {
    mCRL2log(debug) << "type checking of sort expressions read-in phase finished" << std::endl;

    if (!is_unknown_sort(sort_expr) && !is_multiple_possible_sorts(sort_expr))
    {
      if (gstcIsSortExprDeclared(sort_expr))
      {
        Result=sort_expr;
      }
    }
    else
    {
      mCRL2log(error) << "type checking of sort expressions failed (" << atermpp::aterm(sort_expr) << ") is not a sort expression)" << std::endl;
    }
  }
  else
  {
    mCRL2log(error) << "reading Sorts from LPS failed" << std::endl;
  }

  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_data_expr(ATermAppl data_expr, ATermAppl sort_expr, ATermAppl spec, const ATermTable &Vars)
{
  mCRL2log(verbose) << "type checking data expression..." << std::endl;
  //check correctness of the data expression in data_expr using
  //the specification in spec

  //check preconditions
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  assert(gsIsDataExpr(data_expr));
  assert((sort_expr == ATerm()) || gsIsSortExpr(sort_expr));

  ATermAppl Result=NULL;

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of data expression read-in phase started" << std::endl;

  ATermAppl data_spec = NULL;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts) &&
      gstcReadInConstructors() &&
      gstcReadInFuncs(constructors,mappings))
  {
    mCRL2log(debug) << "type checking of data expression read-in phase finished" << std::endl;

    if ((sort_expr != ATerm()) && (is_unknown_sort(sort_expr) || is_multiple_possible_sorts(sort_expr)))
    {
      mCRL2log(error) << "type checking of data expression failed (" << atermpp::aterm(sort_expr) << " is not a sort expression)" << std::endl;
    }
    else if ((sort_expr == ATerm()) || gstcIsSortExprDeclared(sort_expr))
    {
      /* bool destroy_vars=(Vars == NULL);
      if (destroy_vars)
      {
        Vars=ATtableCreate(63,50);
      } */
      ATermAppl data=data_expr;
      ATermAppl Type=gstcTraverseVarConsTypeD(Vars,Vars,&data,(sort_expr==ATermAppl())?(ATermAppl)data::unknown_sort():sort_expr);
      /* if (destroy_vars)
      {
        ATtableDestroy(Vars);
      } */
      if (&*Type && !data::is_unknown_sort(data::sort_expression(Type)))
      {
        Result=data;
      }
      else
      {
        mCRL2log(error) << "type checking of data expression failed" << std::endl;
      }
    }
  }
  else
  {
    mCRL2log(error) << "reading from LPS failed" << std::endl;
  }
  gstcDataDestroy();

  return Result;
}

ATermAppl type_check_mult_act(
  ATermAppl mult_act,
  ATermAppl data_spec,
  ATermList action_labels)
{
  mCRL2log(debug) << "type checking multiaction..." << std::endl;
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  // assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  ATermAppl Result=NULL;

  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of multiactions read-in phase started" << std::endl;

  // ATermAppl data_spec = ATAgetArgument(spec, 0);
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);
  // ATermList action_labels = ATLgetArgument(ATAgetArgument(spec, 1), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts)
      && gstcReadInConstructors()
      && gstcReadInFuncs(constructors,mappings)
      && gstcReadInActs(action_labels)
     )
  {
    mCRL2log(debug) << "type checking of multiactions read-in phase finished" << std::endl;

    if (gsIsMultAct(mult_act))
    {
      ATermTable Vars=ATtableCreate(63,50);
      ATermList r=ATmakeList0();
      for (ATermList l=ATLgetArgument(mult_act,0); !ATisEmpty(l); l=ATgetNext(l))
      {
        ATermAppl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        if (!&*o)
        {
          goto done;
        }
        r=ATinsert(r,o);
      }
      Result=ATsetArgument(mult_act,ATreverse(r),0);

done:
      ATtableDestroy(Vars);
    }
    else
    {
      mCRL2log(error) << "type checking of multiactions failed (" << atermpp::aterm(mult_act) << " is not a multiaction)" << std::endl;
    }
  }
  else
  {
    mCRL2log(error) << "reading from LPS failed" << std::endl;
  }
  gstcDataDestroy();
  return Result;
}

ATermList type_check_mult_actions(
  ATermList mult_actions,
  ATermAppl data_spec,
  ATermList action_labels)
{
  mCRL2log(debug) << "type checking multiactions..." << std::endl;
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  // assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));

  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of multiactions read-in phase started" << std::endl;

  // ATermAppl data_spec = ATAgetArgument(spec, 0);
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);
  // ATermList action_labels = ATLgetArgument(ATAgetArgument(spec, 1), 0);

  //XXX read-in from spec (not finished)
  ATermList result=ATempty;
  if (gstcReadInSorts(sorts)
      && gstcReadInConstructors()
      && gstcReadInFuncs(constructors,mappings)
      && gstcReadInActs(action_labels))
  {
    mCRL2log(debug) << "type checking of multiactions read-in phase finished" << std::endl;

    for (; !ATisEmpty(mult_actions); mult_actions=ATgetNext(mult_actions))
    {
      ATermTable Vars=ATtableCreate(63,50);
      ATermList r=ATmakeList0();

      for (ATermList l=(ATermList)ATgetFirst(mult_actions) ; !ATisEmpty(l); l=ATgetNext(l))
      {
        ATermAppl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        if (!&*o)
        {
          ATtableDestroy(Vars);
          throw mcrl2::runtime_error("Typechecking action failed: "+ core::pp_deprecated(ATAgetFirst(l)));
        }
        r=ATinsert(r,o);
      }
      result = ATinsert(result,ATreverse(r));
    }
  }
  else
  {
    throw mcrl2::runtime_error("reading data/action specification failed");
  }
  gstcDataDestroy();
  return ATreverse(result);
}

ATermAppl type_check_proc_expr(ATermAppl proc_expr, ATermAppl spec)
{
  mCRL2log(verbose) << "type checking process expression..." << std::endl;

  //check correctness of the process expression in proc_expr using
  //the process specification or LPS in spec
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  mCRL2log(warning) << "type checking of process expressions is not yet implemented" << std::endl;
  return proc_expr;
}

ATermAppl type_check_state_frm(ATermAppl state_frm, ATermAppl spec)
{
  mCRL2log(verbose) << "type checking state formula..." << std::endl;
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
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of state formulas read-in phase started" << std::endl;

  ATermAppl data_spec = ATAgetArgument(spec, 0);
  ATermList action_labels = ATLgetArgument(ATAgetArgument(spec, 1), 0);

  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  ATermList constructors = ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  ATermList mappings = ATLgetArgument(ATAgetArgument(data_spec, 2), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts))
  {
    if (gstcReadInConstructors())
    {
      if (gstcReadInFuncs(constructors,mappings))
      {
        if (action_labels != ATerm())
        {
          if (!gstcReadInActs(action_labels))
          {
            mCRL2log(warning) << "ignoring the previous error(s), the formula will be typechecked without action label information" << std::endl;
          }
        }
        else
        {
          mCRL2log(warning) << "ignoring the previous error(s), the formula will be typechecked without action label information" << std::endl;
        }
        mCRL2log(debug) << "type checking of state formulas read-in phase finished" << std::endl;

        ATermTable Vars=ATtableCreate(63,50);
        Result=gstcTraverseStateFrm(Vars,Vars,state_frm);
        ATtableDestroy(Vars);
      }
      else
      {
        mCRL2log(error) << "reading functions from LPS failed" << std::endl;
      }
    }
    else
    {
      mCRL2log(error) << "reading structure constructors from LPS failed." << std::endl;
    }
  }
  else
  {
    mCRL2log(error) << "reading sorts from LPS failed" << std::endl;
  }
  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_action_rename_spec(ATermAppl ar_spec, ATermAppl lps_spec)
{

  mCRL2log(verbose) << "type checking action rename specification..." << std::endl;

  //check precondition
  assert(gsIsActionRenameSpec(ar_spec));

  ATermAppl Result=NULL;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of action rename specification read-in phase started" << std::endl;

  ATermTable actions_from_lps=ATtableCreate(63,50);

  ATermAppl lps_data_spec = ATAgetArgument(lps_spec, 0);
  ATermList lps_sorts = ATLgetArgument(ATAgetArgument(lps_data_spec, 0), 0);
  ATermList lps_constructors = ATLgetArgument(ATAgetArgument(lps_data_spec, 1), 0);
  ATermList lps_mappings = ATLgetArgument(ATAgetArgument(lps_data_spec, 2), 0);
  ATermList lps_action_labels = ATLgetArgument(ATAgetArgument(lps_spec, 1), 0);

  //XXX read-in from LPS (not finished)
  if (gstcReadInSorts((ATermList) lps_sorts))
  {
    if (gstcReadInConstructors())
    {
      if (gstcReadInFuncs(lps_constructors, lps_mappings))
      {
        if (!gstcReadInActs(lps_action_labels))
        {
          mCRL2log(warning) << "ignoring the previous error(s), the formula will be typechecked without action label information" << std::endl;
        }
        mCRL2log(debug) << "type checking of action rename specification read-in phase of LPS finished" << std::endl;
        mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file started" << std::endl;

        ATermAppl data_spec = ATAgetArgument(ar_spec, 0);
        ATermList LPSSorts=ATtableKeys(context.defined_sorts); // remember the sorts from the LPS.
        if (!gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0)))
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file sorts finished" << std::endl;

        // Check sorts for loops
        // Unwind sorts to enable equiv and subtype relations
        if (!gstcReadInConstructors(list_minus(ATtableKeys(context.defined_sorts),LPSSorts)))
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file constructors finished" << std::endl;

        if (!gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
                             ATLgetArgument(ATAgetArgument(data_spec,2),0)))
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file functions finished" << std::endl;

        body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);

        //Save the actions from LPS only for the latter use.
        gstcATermTableCopy(context.actions,actions_from_lps);
        if (!gstcReadInActs(ATLgetArgument(ATAgetArgument(ar_spec,1),0)))
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking action rename specification read-in phase of the ActionRenameSpec finished" << std::endl;

        if (!gstcTransformVarConsTypeData())
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;

        data_spec=ATsetArgument(data_spec, gsMakeDataEqnSpec(body.equations),3);
        Result=ATsetArgument(ar_spec,data_spec,0);
        Result=gstcFoldSortRefs(Result);


        // now the action renaming rules themselves.
        ATermAppl ActionRenameRules=ATAgetArgument(ar_spec, 2);
        ATermList NewRules=ATmakeList0();

        ATermTable DeclaredVars=ATtableCreate(63,50);
        ATermTable FreeVars=ATtableCreate(63,50);

        bool b = true;

        for (ATermList l=ATLgetArgument(ActionRenameRules,0); !ATisEmpty(l); l=ATgetNext(l))
        {
          ATermAppl Rule=ATAgetFirst(l);
          assert(gsIsActionRenameRule(Rule));

          ATermList VarList=ATLgetArgument(Rule,0);
          if (!gstcVarsUnique(VarList))
          {
            b = false;
            mCRL2log(error) << "the variables " << core::pp_deprecated(VarList) << " in action rename rule " << core::pp_deprecated(Rule) << " are not unique" << std::endl;
            break;
          }

          ATermTable NewDeclaredVars;
          if (!gstcAddVars2Table(DeclaredVars,VarList,NewDeclaredVars))
          {
            b = false;
            break;
          }
          else
          {
            DeclaredVars=NewDeclaredVars;
          }

          ATermAppl Left=ATAgetArgument(Rule,2);
          assert(gsIsParamId(Left));
          {
            //extra check requested by Tom: actions in the LHS can only come from the LPS
            ATermTable temp=context.actions;
            context.actions=actions_from_lps;
            Left=gstcTraverseActProcVarConstP(DeclaredVars,Left);
            context.actions=temp;
            if (!&*Left)
            {
              b = false;
              break;
            }
          }

          ATermAppl Cond=ATAgetArgument(Rule,1);
          if (!&*(gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Cond,sort_bool::bool_())))
          {
            b = false;  // JK 15/10/2009 remove gsIsNil check
            break;
          }

          ATermAppl Right=ATAgetArgument(Rule,3);
          assert(gsIsParamId(Right) || gsIsTau(Right) || gsIsDelta(Right));
          Right=gstcTraverseActProcVarConstP(DeclaredVars,Right);
          if (!&*Right)
          {
            b = false;
            break;
          }

          NewRules=ATinsert(NewRules,gsMakeActionRenameRule(VarList,Cond,Left,Right));
        }
        ATtableDestroy(FreeVars);
        ATtableDestroy(DeclaredVars);
        if (!b)
        {
          Result = NULL;
          goto finally;
        }

        ActionRenameRules=ATsetArgument(ActionRenameRules,ATreverse(NewRules),0);
        Result=ATsetArgument(Result,ActionRenameRules,2);
        mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;
      }
      else
      {
        mCRL2log(error) << "reading functions from LPS failed" << std::endl;
      }
    }
    else
    {
      mCRL2log(error) << "reading structure constructors from LPS failed" << std::endl;
    }
  }
  else
  {
    mCRL2log(error) << "reading sorts from LPS failed" << std::endl;
  }

finally:
  ATtableDestroy(actions_from_lps);
  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_pbes_spec(ATermAppl pbes_spec)
{
  //check correctness of the PBES specification in pbes_spec

  mCRL2log(verbose) << "type checking PBES specification..." << std::endl;

  assert(gsIsPBES(pbes_spec));

  ATermAppl Result=NULL;
  mCRL2log(debug) << "type checking phase of PBES specifications started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of PBES specification read-in phase started" << std::endl;


  ATermAppl data_spec = ATAgetArgument(pbes_spec,0);
  ATermAppl pb_eqn_spec = ATAgetArgument(pbes_spec,2);
  ATermAppl pb_init = ATAgetArgument(pbes_spec,3);
  ATermAppl glob_var_spec = ATAgetArgument(pbes_spec,1);
  ATermTable dummy;

  if (!gstcReadInSorts(ATLgetArgument(ATAgetArgument(data_spec,0),0)))
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking of PBES specification read-in phase of sorts finished" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if (!gstcReadInConstructors())
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking of PBES specification read-in phase of constructors finished" << std::endl;

  if (!gstcReadInFuncs(ATLgetArgument(ATAgetArgument(data_spec,1),0),
                       ATLgetArgument(ATAgetArgument(data_spec,2),0)))
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking of PBES specification read-in phase of functions finished" << std::endl;

  body.equations=ATLgetArgument(ATAgetArgument(data_spec,3),0);

  if (!gstcAddVars2Table(context.glob_vars, ATLgetArgument(glob_var_spec,0),dummy))
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking of PBES specification read-in phase of global variables finished" << std::endl;

  if (!gstcReadInPBESAndInit(pb_eqn_spec,pb_init))
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking PBES read-in phase finished" << std::endl;

  mCRL2log(debug) << "type checking transform Data+PBES phase started" << std::endl;
  if (!gstcTransformVarConsTypeData())
  {
    goto finally;
  }
  if (!gstcTransformPBESVarConst())
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking transform Data+PBES phase finished" << std::endl;

  data_spec=ATsetArgument(data_spec,gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(pbes_spec,data_spec,0);

  pb_eqn_spec=ATsetArgument(pb_eqn_spec,gstcWritePBES(ATLgetArgument(pb_eqn_spec,0)),0);
  Result=ATsetArgument(Result,pb_eqn_spec,2);

  pb_init=ATsetArgument(pb_init,ATAtableGet(body.proc_bodies,INIT_KEY()),0);
  Result=ATsetArgument(Result,pb_init,3);

  Result=gstcFoldSortRefs(Result);

finally:
  gstcDataDestroy();
  return Result;
}

ATermList type_check_data_vars(ATermList data_vars, ATermAppl spec)
{
  mCRL2log(verbose) << "type checking data variables..." << std::endl;
  //check correctness of the data variable declaration in sort_expr
  //using the specification in spec

  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of data variables read-in phase started" << std::endl;

  ATermAppl data_spec = NULL;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermList sorts = ATLgetArgument(ATAgetArgument(data_spec, 0), 0);

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts))
  {
    mCRL2log(debug) << "type checking of data variables read-in phase finished" << std::endl;

    ATermTable Vars=ATtableCreate(63,50);
    ATermTable NewVars;
    if (!gstcAddVars2Table(Vars,data_vars,NewVars))
    {
      ATtableDestroy(Vars);
      mCRL2log(error) << "type error while typechecking data variables" << std::endl;
      return NULL;
    }
    ATtableDestroy(Vars);
  }
  else
  {
    mCRL2log(error) << "reading from LPS failed" << std::endl;
  }
  gstcDataDestroy();

  return data_vars;
}

//local functions
//---------------

// fold functions

void gstcSplitSortDecls(ATermList SortDecls, ATermList* PSortIds,
                        ATermList* PSortRefs)
{
  ATermList SortIds = ATmakeList0();
  ATermList SortRefs = ATmakeList0();
  while (!ATisEmpty(SortDecls))
  {
    ATermAppl SortDecl = ATAgetFirst(SortDecls);
    if (gsIsSortRef(SortDecl))
    {
      SortRefs = ATinsert(SortRefs, SortDecl);
    }
    else     //gsIsSortId(SortDecl)
    {
      SortIds = ATinsert(SortIds, SortDecl);
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
  if (gsIsDataSpec(Spec))
  {
    Spec = ATsetArgument(Spec, SortSpec, 0);
  }
  else
  {
    ATermAppl DataSpec = ATAgetArgument(Spec, 0);
    DataSpec = ATsetArgument(DataSpec, SortSpec, 0);
    Spec = ATsetArgument(Spec, DataSpec, 0);
  }
  return Spec;
}

ATermAppl gstcFoldSortRefs(ATermAppl Spec)
{
  assert(gsIsDataSpec(Spec) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) || gsIsPBES(Spec) || gsIsActionRenameSpec(Spec));
  mCRL2log(debug) << "specification before folding:" << core::pp_deprecated(Spec) << "" << std::endl;
  //get sort declarations
  ATermAppl DataSpec;
  if (gsIsDataSpec(Spec))
  {
    DataSpec = Spec;
  }
  else
  {
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
  while (!ATisEmpty(l))
  {
    ATermAppl SortRef = ATAgetFirst(l);
    //add substitution for SortRef
    ATermAppl LHS = gsMakeSortId(ATAgetArgument(SortRef, 0));
    ATermAppl RHS = ATAgetArgument(SortRef, 1);
    if (gsIsSortId(RHS) || gsIsSortArrow(RHS))
    {
      //add forward substitution
      ATtablePut(Substs, LHS, RHS);
    }
    else
    {
      //add backward substitution
      ATtablePut(Substs, RHS, LHS);
    }
    l = ATgetNext(l);
  }
  //(c) perform substitutions until the specification becomes stable
  ATermAppl NewSpec = Spec;
  do
  {
    mCRL2log(debug) << "substituting sort references in specification" << std::endl;
    Spec = NewSpec;
    NewSpec = (ATermAppl) gsSubstValuesTable(Substs, Spec, true);
  }
  while (!ATisEqual(NewSpec, Spec));
  ATtableDestroy(Substs);

  //add the removed sort references back to Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(ATconcat(SortIds, SortRefs)));
  mCRL2log(debug) << "specification after folding:\n" << core::pp_deprecated(Spec) << "\n" ;
  return Spec;
}

ATermList gstcFoldSortRefsInSortRefs(ATermList SortRefs)
{
  //fold sort references in SortRefs by means of repeated forward and backward
  //substitution
  ATermList NewSortRefs = SortRefs;
  size_t n = ATgetLength(SortRefs);
  //perform substitutions until the list of sort references becomes stable
  do
  {
    SortRefs = NewSortRefs;
    mCRL2log(debug) << "SortRefs contains the following sort references:\n" << core::pp_deprecated(gsMakeSortSpec(SortRefs)) << "" << std::endl;
    //perform substitutions implied by sort references in NewSortRefs to the
    //other elements in NewSortRefs
    for (size_t i = 0; i < n; i++)
    {
      ATermAppl SortRef = ATAelementAt(NewSortRefs, i);
      //turn SortRef into a substitution
      ATermAppl LHS = gsMakeSortId(ATAgetArgument(SortRef, 0));
      ATermAppl RHS = ATAgetArgument(SortRef, 1);
      ATermAppl Subst;
      if (gsIsSortId(RHS) || gsIsSortArrow(RHS))
      {
        //make forward substitution
        Subst = gsMakeSubst_Appl(LHS, RHS);
      }
      else
      {
        //make backward substitution
        Subst = gsMakeSubst_Appl(RHS, LHS);
      }
      mCRL2log(debug) << "performing substition " << core::pp_deprecated(ATgetArgument(Subst, 0)) << " := " << core::pp_deprecated(ATgetArgument(Subst, 1)) << "" << std::endl;
      //perform Subst on all elements of NewSortRefs except for the i'th
      ATermList Substs = ATmakeList1(Subst);
      for (size_t j = 0; j < n; j++)
      {
        if (i != j)
        {
          ATermAppl OldSortRef = ATAelementAt(NewSortRefs, j);
          ATermAppl NewSortRef = gsSubstValues_Appl(Substs, OldSortRef, true);
          if (!ATisEqual(NewSortRef, OldSortRef))
          {
            NewSortRefs = ATreplace(NewSortRefs, NewSortRef, j);
          }
        }
      }
    }
    mCRL2log(debug) << std::endl;
  }
  while (!ATisEqual(NewSortRefs, SortRefs));
  //remove self references
  ATermList l = ATmakeList0();
  while (!ATisEmpty(SortRefs))
  {
    ATermAppl SortRef = ATAgetFirst(SortRefs);
    if (!ATisEqual(gsMakeSortId(ATAgetArgument(SortRef, 0)),
                   ATAgetArgument(SortRef, 1)))
    {
      l = ATinsert(l, SortRef);
    }
    SortRefs = ATgetNext(SortRefs);
  }
  SortRefs = ATreverse(l);
  mCRL2log(debug) << "SortRefs, after removing self references:\n" << core::pp_deprecated(gsMakeSortSpec(SortRefs)) << "" << std::endl;
  return SortRefs;
}

// ========= main processing functions
void gstcDataInit(void)
{
  gssystem.constants=ATtableCreate(63,50);
  gssystem.functions=ATtableCreate(63,50);
  context.basic_sorts=ATindexedSetCreate(63,50);
  context.defined_sorts=ATtableCreate(63,50);
  context.constants=ATtableCreate(63,50);
  context.functions=ATtableCreate(63,50);
  context.actions=ATtableCreate(63,50);
  context.processes=ATtableCreate(63,50);
  context.PBs=ATtableCreate(63,50);
  context.glob_vars=ATtableCreate(63,50);
  body.proc_pars=ATtableCreate(63,50);
  body.proc_bodies=ATtableCreate(63,50);
  body.equations = ATmakeList0();

  //Creation of operation identifiers for system defined operations.
  //Bool
  gstcAddSystemConstant(sort_bool::true_());
  gstcAddSystemConstant(sort_bool::false_());
  gstcAddSystemFunction(sort_bool::not_());
  gstcAddSystemFunction(sort_bool::and_());
  gstcAddSystemFunction(sort_bool::or_());
  gstcAddSystemFunction(sort_bool::implies());
  gstcAddSystemFunction(equal_to(data::unknown_sort()));
  gstcAddSystemFunction(not_equal_to(data::unknown_sort()));
  gstcAddSystemFunction(if_(data::unknown_sort()));
  gstcAddSystemFunction(less(data::unknown_sort()));
  gstcAddSystemFunction(less_equal(data::unknown_sort()));
  gstcAddSystemFunction(greater_equal(data::unknown_sort()));
  gstcAddSystemFunction(greater(data::unknown_sort()));
  //Numbers
  gstcAddSystemFunction(sort_nat::pos2nat());
  gstcAddSystemFunction(sort_nat::cnat());
  gstcAddSystemFunction(sort_int::pos2int());
  gstcAddSystemFunction(sort_real::pos2real());
  gstcAddSystemFunction(sort_nat::nat2pos());
  gstcAddSystemFunction(sort_int::nat2int());
  gstcAddSystemFunction(sort_int::cint());
  gstcAddSystemFunction(sort_real::nat2real());
  gstcAddSystemFunction(sort_int::int2pos());
  gstcAddSystemFunction(sort_int::int2nat());
  gstcAddSystemFunction(sort_real::int2real());
  gstcAddSystemFunction(sort_real::creal());
  gstcAddSystemFunction(sort_real::real2pos());
  gstcAddSystemFunction(sort_real::real2nat());
  gstcAddSystemFunction(sort_real::real2int());
  gstcAddSystemConstant(sort_pos::c1());
  //more
  gstcAddSystemFunction(sort_real::maximum(sort_pos::pos(),sort_pos::pos()));
  gstcAddSystemFunction(sort_real::maximum(sort_pos::pos(),sort_nat::nat()));
  gstcAddSystemFunction(sort_real::maximum(sort_nat::nat(),sort_pos::pos()));
  gstcAddSystemFunction(sort_real::maximum(sort_nat::nat(),sort_nat::nat()));
  gstcAddSystemFunction(sort_real::maximum(sort_pos::pos(),sort_int::int_()));
  gstcAddSystemFunction(sort_real::maximum(sort_int::int_(),sort_pos::pos()));
  gstcAddSystemFunction(sort_real::maximum(sort_nat::nat(),sort_int::int_()));
  gstcAddSystemFunction(sort_real::maximum(sort_int::int_(),sort_nat::nat()));
  gstcAddSystemFunction(sort_real::maximum(sort_int::int_(),sort_int::int_()));
  gstcAddSystemFunction(sort_real::maximum(sort_real::real_(),sort_real::real_()));
  //more
  gstcAddSystemFunction(sort_real::minimum(sort_pos::pos(), sort_pos::pos()));
  gstcAddSystemFunction(sort_real::minimum(sort_nat::nat(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::minimum(sort_int::int_(), sort_int::int_()));
  gstcAddSystemFunction(sort_real::minimum(sort_real::real_(), sort_real::real_()));
  //more
  // gstcAddSystemFunction(sort_real::abs(sort_pos::pos()));
  // gstcAddSystemFunction(sort_real::abs(sort_nat::nat()));
  gstcAddSystemFunction(sort_real::abs(sort_int::int_()));
  gstcAddSystemFunction(sort_real::abs(sort_real::real_()));
  //more
  gstcAddSystemFunction(sort_real::negate(sort_pos::pos()));
  gstcAddSystemFunction(sort_real::negate(sort_nat::nat()));
  gstcAddSystemFunction(sort_real::negate(sort_int::int_()));
  gstcAddSystemFunction(sort_real::negate(sort_real::real_()));
  gstcAddSystemFunction(sort_real::succ(sort_pos::pos()));
  gstcAddSystemFunction(sort_real::succ(sort_nat::nat()));
  gstcAddSystemFunction(sort_real::succ(sort_int::int_()));
  gstcAddSystemFunction(sort_real::succ(sort_real::real_()));
  gstcAddSystemFunction(sort_real::pred(sort_pos::pos()));
  gstcAddSystemFunction(sort_real::pred(sort_nat::nat()));
  gstcAddSystemFunction(sort_real::pred(sort_int::int_()));
  gstcAddSystemFunction(sort_real::pred(sort_real::real_()));
  gstcAddSystemFunction(sort_real::plus(sort_pos::pos(),sort_pos::pos()));
  gstcAddSystemFunction(sort_real::plus(sort_pos::pos(),sort_nat::nat()));
  gstcAddSystemFunction(sort_real::plus(sort_nat::nat(),sort_pos::pos()));
  gstcAddSystemFunction(sort_real::plus(sort_nat::nat(),sort_nat::nat()));
  gstcAddSystemFunction(sort_real::plus(sort_int::int_(),sort_int::int_()));
  gstcAddSystemFunction(sort_real::plus(sort_real::real_(),sort_real::real_()));
  //more
  gstcAddSystemFunction(sort_real::minus(sort_pos::pos(), sort_pos::pos()));
  gstcAddSystemFunction(sort_real::minus(sort_nat::nat(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::minus(sort_int::int_(), sort_int::int_()));
  gstcAddSystemFunction(sort_real::minus(sort_real::real_(), sort_real::real_()));
  gstcAddSystemFunction(sort_real::times(sort_pos::pos(), sort_pos::pos()));
  gstcAddSystemFunction(sort_real::times(sort_nat::nat(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::times(sort_int::int_(), sort_int::int_()));
  gstcAddSystemFunction(sort_real::times(sort_real::real_(), sort_real::real_()));
  //more
  // gstcAddSystemFunction(sort_int::div(sort_pos::pos(), sort_pos::pos()));
  gstcAddSystemFunction(sort_int::div(sort_nat::nat(), sort_pos::pos()));
  gstcAddSystemFunction(sort_int::div(sort_int::int_(), sort_pos::pos()));
  // gstcAddSystemFunction(sort_int::mod(sort_pos::pos(), sort_pos::pos()));
  gstcAddSystemFunction(sort_int::mod(sort_nat::nat(), sort_pos::pos()));
  gstcAddSystemFunction(sort_int::mod(sort_int::int_(), sort_pos::pos()));
  gstcAddSystemFunction(sort_real::divides(sort_pos::pos(), sort_pos::pos()));
  gstcAddSystemFunction(sort_real::divides(sort_nat::nat(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::divides(sort_int::int_(), sort_int::int_()));
  gstcAddSystemFunction(sort_real::divides(sort_real::real_(), sort_real::real_()));
  gstcAddSystemFunction(sort_real::exp(sort_pos::pos(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::exp(sort_nat::nat(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::exp(sort_int::int_(), sort_nat::nat()));
  gstcAddSystemFunction(sort_real::exp(sort_real::real_(), sort_int::int_()));
  gstcAddSystemFunction(sort_real::floor());
  gstcAddSystemFunction(sort_real::ceil());
  gstcAddSystemFunction(sort_real::round());
  //Lists
  gstcAddSystemConstant(sort_list::nil(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::cons_(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::count(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::snoc(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::concat(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::element_at(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::head(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::tail(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::rhead(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::rtail(data::unknown_sort()));
  gstcAddSystemFunction(sort_list::in(data::unknown_sort()));
  //Sets
  gstcAddSystemFunction(sort_bag::set2bag(data::unknown_sort()));
  gstcAddSystemConstant(sort_set::empty(data::unknown_sort()));
  gstcAddSystemFunction(sort_set::in(data::unknown_sort()));
  gstcAddSystemFunction(sort_set::union_(data::unknown_sort()));
  gstcAddSystemFunction(sort_set::difference(data::unknown_sort()));
  gstcAddSystemFunction(sort_set::intersection(data::unknown_sort()));
  gstcAddSystemFunction(sort_set::complement(data::unknown_sort()));

  //Bags
  gstcAddSystemFunction(sort_bag::bag2set(data::unknown_sort()));
  gstcAddSystemConstant(sort_bag::empty(data::unknown_sort()));
  gstcAddSystemFunction(sort_bag::in(data::unknown_sort()));
  gstcAddSystemFunction(sort_bag::count(data::unknown_sort()));
  gstcAddSystemFunction(sort_bag::join(data::unknown_sort()));
  gstcAddSystemFunction(sort_bag::difference(data::unknown_sort()));
  gstcAddSystemFunction(sort_bag::intersection(data::unknown_sort()));

  // function update
  gstcAddSystemFunction(data::function_update(data::unknown_sort(),data::unknown_sort()));
}

void gstcDataDestroy(void)
{
  ATtableDestroy(gssystem.constants);
  ATtableDestroy(gssystem.functions);
  ATindexedSetDestroy(context.basic_sorts);
  ATtableDestroy(context.defined_sorts);
  ATtableDestroy(context.constants);
  ATtableDestroy(context.functions);
  ATtableDestroy(context.actions);
  ATtableDestroy(context.processes);
  ATtableDestroy(context.PBs);
  ATtableDestroy(context.glob_vars);
  ATtableDestroy(body.proc_pars);
  ATtableDestroy(body.proc_bodies);
  // ATunprotectList(&body.equations);
}

static bool gstc_check_for_sort_alias_loop_through_function_sort_via_expression(
  const sort_expression& sort_expression_start_search,
  const basic_sort& end_search,
  std::set < basic_sort > &visited,
  const bool observed_a_sort_constructor);

// This function checks whether there is a path of sort aliases
// from start_search to end_search.
// The boolean observed_a_sort_constructor indicates whether on the
// current path a sort constainer or a function sort has been
// observed. The set visited indicates which basic sorts have been
// encountered on the current path. This is only used for loop  checking.
// The current algorithm is exponential, which is considered not too
// much of a problem, given that sort expressions are generally small.
static bool gstc_check_for_sort_alias_loop_through_function_sort(
  const basic_sort& start_search,
  const basic_sort& end_search,
  std::set < basic_sort > &visited,
  const bool observed_a_sort_constructor)
{
  ATermAppl aterm_reference=(ATermAppl)ATtableGet(context.defined_sorts,
                            static_cast<ATermAppl>(start_search.name()));

  if (aterm_reference==ATermAppl())
  {
    // start_search is not a sort alias, and hence not a recursive sort.
    return false;
  }

  if (start_search==end_search)
  {
    // We found a loop.
    return observed_a_sort_constructor;
  }
  if (visited.find(start_search)!=visited.end())
  {
    // start_search has already been encountered. end_search will not be found via this path.
    return false;
  }

  visited.insert(start_search);
  const sort_expression reference(aterm_reference);
  return gstc_check_for_sort_alias_loop_through_function_sort_via_expression(reference,end_search,visited,observed_a_sort_constructor);
}

static bool gstc_check_for_sort_alias_loop_through_function_sort_via_expression(
  const sort_expression& sort_expression_start_search,
  const basic_sort& end_search,
  std::set < basic_sort > &visited,
  const bool observed_a_sort_constructor)
{
  if (is_basic_sort(sort_expression_start_search))
  {
    const basic_sort start_search(sort_expression_start_search);
    if (end_search==start_search)
    {
      return observed_a_sort_constructor;
    }
    else
    {
      return gstc_check_for_sort_alias_loop_through_function_sort(start_search,end_search,visited,observed_a_sort_constructor);
    }
  }

  if (is_container_sort(sort_expression_start_search))
  {
    // A loop through a list container is allowed, but a loop through a set or bag container
    // is problematic.
    const container_sort start_search_container(sort_expression_start_search);
    return gstc_check_for_sort_alias_loop_through_function_sort_via_expression(
             start_search_container.element_sort(),end_search,visited,
             start_search_container.container_name()!=list_container());
  }

  if (is_function_sort(sort_expression_start_search))
  {
    const function_sort f_start_search(sort_expression_start_search);
    if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(
          f_start_search.codomain(),end_search,visited,true))
    {
      return true;
    }
    for (sort_expression_list::const_iterator i=f_start_search.domain().begin();
         i!=f_start_search.domain().end(); ++i)
    {
      if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(
            *i,end_search,visited,true))
      {
        return true;
      }
    }
    // end_search has not been found, so:
    return false;
  }

  if (is_structured_sort(sort_expression_start_search))
  {
    const structured_sort struct_start_search(sort_expression_start_search);
    const function_symbol_vector constructor_functions=struct_start_search.constructor_functions();
    for (function_symbol_vector::const_iterator i=constructor_functions.begin();
         i!=constructor_functions.end(); ++i)
    {
      if (is_function_sort(i->sort()))
      {
        const sort_expression_list domain_sorts=function_sort(i->sort()).domain();
        for (sort_expression_list::const_iterator j=domain_sorts.begin();
             j!=domain_sorts.end(); ++j)
        {
          if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(
                *j,end_search,visited,observed_a_sort_constructor))
          {
            return true;
          }
        }
      }
    }
    return false;

  }

  assert(0); // start_search cannot be a multiple_possible_sorts, or an unknown sort.
  return false;
}

static bool gstcReadInSorts(ATermList Sorts)
{
  bool nnew;
  bool Result=true;
  for (; !ATisEmpty(Sorts); Sorts=ATgetNext(Sorts))
  {
    ATermAppl Sort=ATAgetFirst(Sorts);
    ATermAppl SortName=ATAgetArgument(Sort,0);
    if (sort_bool::is_bool(basic_sort(core::identifier_string(SortName))))
    {
      mCRL2log(error) << "attempt to redeclare sort Bool" << std::endl;
      return false;
    }
    if (sort_pos::is_pos(basic_sort(core::identifier_string(SortName))))
    {
      mCRL2log(error) << "attempt to redeclare sort Pos" << std::endl;
      return false;
    }
    if (sort_nat::is_nat(basic_sort(core::identifier_string(SortName))))
    {
      mCRL2log(error) << "attempt to redeclare sort Nat" << std::endl;
      return false;
    }
    if (sort_int::is_int(basic_sort(core::identifier_string(SortName))))
    {
      mCRL2log(error) << "attempt to redeclare sort Int" << std::endl;
      return false;
    }
    if (sort_real::is_real(basic_sort(core::identifier_string(SortName))))
    {
      mCRL2log(error) << "attempt to redeclare sort Real" << std::endl;
      return false;
    }
    if (ATindexedSetGetIndex(context.basic_sorts, SortName)>=0
        || &*ATAtableGet(context.defined_sorts, SortName))
    {

      mCRL2log(error) << "double declaration of sort " << core::pp_deprecated(SortName) << std::endl;
      return false;
    }
    if (gsIsSortId(Sort))
    {
      ATindexedSetPut(context.basic_sorts, SortName, &nnew);
    }
    else if (gsIsSortRef(Sort))
    {
      ATtablePut(context.defined_sorts, SortName, ATAgetArgument(Sort,1));
      mCRL2log(debug) << "Add sort alias " << core::pp_deprecated(SortName) << "  " << core::pp_deprecated(ATAgetArgument(Sort,1)) << "" << std::endl;
    }
    else
    {
      assert(0);
    }
  }

  // Check for sorts that are recursive through container sorts.
  // E.g. sort L=List(L);
  // This is forbidden.

  ATermList sort_aliases=ATtableKeys(context.defined_sorts);
  for (; sort_aliases!=ATempty ; sort_aliases=ATgetNext(sort_aliases))
  {
    std::set < basic_sort > visited;
    const basic_sort s(core::identifier_string((ATermAppl)ATgetFirst(sort_aliases)));
    ATermAppl aterm_reference=(ATermAppl)ATtableGet(context.defined_sorts,
                              static_cast<ATermAppl>(s.name()));
    assert(aterm_reference!=ATermAppl());
    const sort_expression ar(aterm_reference);
    if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(ar,s,visited,false))
    {
      mCRL2log(error) << "sort " << core::pp_deprecated(ATgetFirst(sort_aliases)) << " is recursively defined via a function sort, or a set or a bag type container" << std::endl;
      return false;
    }
  }

  return Result;
}

static bool gstcReadInConstructors(ATermList NewSorts)
{
  ATermList Sorts=NewSorts;
  if (!&*Sorts)
  {
    Sorts=ATtableKeys(context.defined_sorts);
  }
  for (; !ATisEmpty(Sorts); Sorts=ATgetNext(Sorts))
  {
    ATermAppl SortExpr=ATAtableGet(context.defined_sorts,ATgetFirst(Sorts));
    if (!gstcIsSortExprDeclared(SortExpr))
    {
      return false;
    }
    if (!gstcReadInSortStruct(SortExpr))
    {
      return false;
    }
  }
  return true;
}

static
std::map < data::sort_expression, data::basic_sort > construct_normalised_aliases()
{
  // This function does the same as data_specification::reconstruct_m_normalised_aliases().
  // Therefore, it should be replaced by that function, after restructuring the type checker.
  // First reset the normalised aliases and the mappings and constructors that have been
  // inherited to basic sort aliases during a previous round of sort normalisation.
  std::map < data::sort_expression, data::basic_sort > normalised_aliases;

  // Fill normalised_aliases. Simple aliases are stored from left to
  // right. If the right hand side is non trivial (struct, list, set or bag)
  // the alias is stored from right to left.
  for (ATermList sort_walker=ATtableKeys(context.defined_sorts);  sort_walker!=ATempty; sort_walker=ATgetNext(sort_walker))
  {
    const core::identifier_string sort_name(ATAgetFirst(sort_walker));
    const data::basic_sort first(sort_name);
    const data::sort_expression second(atermpp::aterm_appl((ATermAppl)ATtableGet(context.defined_sorts,static_cast<ATermAppl>(sort_name))));
    if (is_structured_sort(second) ||
        is_function_sort(second) ||
        is_container_sort(second))
    {
      // We deal here with a declaration of the shape sort A=ComplexType.
      // Rewrite every occurrence of ComplexType to A. Suppose that there are
      // two declarations of the shape sort A=ComplexType; B=ComplexType then
      // ComplexType is rewritten to A and B is also rewritten to A.
      const std::map< sort_expression, basic_sort >::const_iterator j=normalised_aliases.find(second);
      if (j!=normalised_aliases.end())
      {
        normalised_aliases[first]=j->second;
      }
      else
      {
        normalised_aliases[second]=first;
      }
    }
    else
    {
      // We are dealing with a sort declaration of the shape sort A=B.
      // Every occurrence of sort A is normalised to sort B.
      normalised_aliases[first]=second;
    }
  }

  // Close the mapping normalised_aliases under itself. If a rewriting
  // loop is detected, throw a runtime error.

  for (std::map< sort_expression, basic_sort >::iterator i=normalised_aliases.begin();
       i!=normalised_aliases.end(); i++)
  {
    std::set < sort_expression > sort_already_seen;
    sort_expression result_sort=i->second;

    std::set< sort_expression > all_sorts;
    if (is_container_sort(i->first) || is_function_sort(i->first))
    {
      find_sort_expressions<sort_expression>(i->first, std::inserter(all_sorts, all_sorts.end()));
    }
    while (normalised_aliases.count(result_sort)>0)
    {
      sort_already_seen.insert(result_sort);
      result_sort= normalised_aliases.find(result_sort)->second;
      if (sort_already_seen.count(result_sort))
      {
        throw mcrl2::runtime_error("Sort alias " + core::pp_deprecated(result_sort) + " is defined in terms of itself.");
      }

      for (std::set< sort_expression >::const_iterator j = all_sorts.begin(); j != all_sorts.end(); ++j)
      {
        if (*j==result_sort)
        {
          throw mcrl2::runtime_error("Sort alias " + core::pp_deprecated(i->first) + " depends on sort" +
                                     core::pp_deprecated(result_sort) + ", which is circularly defined.\n");
        }
      }
    }
    // So the normalised sort of i->first is result_sort.
    i->second=result_sort;
  }
  return normalised_aliases;
}

static sort_expression mapping(sort_expression s,std::map < sort_expression, basic_sort > &m)
{
  if (m.find(s)==m.end())
  {
    return s;
  }
  return m[s];
}

// Under the assumption that constructor_list contains all the constructors, this
// function checks whether there are sorts that must be necessarily empty because they
// have only constructors referring to the domain itself, e.g. sort D; cons f:D->D;
// If such a domain exists a message is printed to stderr and true is returned.
//
// The algorithm works by putting all target sorts of constructors in a set possibly_empty_constructor_sorts.
// Subsequently, those sorts for which there is a constructor function with
// argument sorts not in this set, are removed, until no more sorts can be removed.
// All sorts remaining in possibly_empty_constructor_sorts are empty constructor sorts
// and are reported. If this set is empty, true is reported, i.e. showing no problem.

static bool gstc_check_for_empty_constructor_domains(ATermList constructor_list)
{
  // First add the constructors for structured sorts to the constructor list;
  try
  {
    ATermList defined_sorts=ATtableKeys(context.defined_sorts);
    std::map < sort_expression, basic_sort > normalised_aliases=construct_normalised_aliases();
    std::set< sort_expression > all_sorts;
    for (; defined_sorts!=ATempty; defined_sorts=ATgetNext(defined_sorts))
    {
      const basic_sort s(core::identifier_string(gstcUnwindType(ATAgetFirst(defined_sorts))));
      ATermAppl reference=ATAtableGet(context.defined_sorts,static_cast<ATermAppl>(s.name()));
      // if (is_container_sort(i->first) || is_function_sort(i->first))
      find_sort_expressions<sort_expression>(sort_expression(reference), std::inserter(all_sorts, all_sorts.end()));
    }

    for (std::set< sort_expression > ::const_iterator i=all_sorts.begin(); i!=all_sorts.end(); ++i)
    {
      if (is_structured_sort(*i))
      {
        const function_symbol_vector r=structured_sort(*i).constructor_functions();
        for (function_symbol_vector::const_iterator j=r.begin();
             j!=r.end(); ++j)
        {
          constructor_list=ATinsert(constructor_list,static_cast<ATermAppl>(*j));
        }
      }

      if (is_structured_sort(*i))
      {
        const function_symbol_vector r=structured_sort(*i).constructor_functions();
        for (function_symbol_vector::const_iterator i=r.begin();
             i!=r.end(); ++i)
        {
          constructor_list=ATinsert(constructor_list,static_cast<ATermAppl>(*i));
        }
      }

    }

    std::set < sort_expression > possibly_empty_constructor_sorts;
    for (ATermList constructor_list_walker=constructor_list;
         constructor_list_walker!=ATempty; constructor_list_walker=ATgetNext(constructor_list_walker))
    {
      const sort_expression s=data::function_symbol(ATgetFirst(constructor_list_walker)).sort();
      if (is_function_sort(s))
      {
        // if s is a constant sort, nothing needs to be added.
        possibly_empty_constructor_sorts.insert(mapping(function_sort(s).codomain(),normalised_aliases));
      }
    }

    // Walk through the constructors removing constructor sorts that are not empty,
    // until no more constructors sorts can be removed.
    for (bool stable=false ; !stable ;)
    {
      stable=true;
      for (ATermList constructor_list_walker=constructor_list;
           constructor_list_walker!=ATempty; constructor_list_walker=ATgetNext(constructor_list_walker))
      {
        const sort_expression s=data::function_symbol(ATgetFirst(constructor_list_walker)).sort();
        if (!is_function_sort(s))
        {
          if (possibly_empty_constructor_sorts.erase(mapping(s,normalised_aliases))==1) // True if one element has been removed.
          {
            stable=false;
          }
        }
        else
        {
          sort_expression_list r=function_sort(s).domain();
          bool has_a_domain_sort_possibly_empty_sorts=false;
          for (sort_expression_list::const_iterator i=r.begin();
               i!=r.end(); ++i)
          {
            if (possibly_empty_constructor_sorts.find(mapping(*i,normalised_aliases))!=possibly_empty_constructor_sorts.end())
            {
              //
              has_a_domain_sort_possibly_empty_sorts=true;
              continue;
            }
          }
          if (!has_a_domain_sort_possibly_empty_sorts)
          {
            // Condition below is true if one element has been removed.
            if (possibly_empty_constructor_sorts.erase(mapping(function_sort(s).codomain(),normalised_aliases))==1)
            {
              stable=false;
            }
          }
        }
      }
    }
    // Print the sorts remaining in possibly_empty_constructor_sorts, as they must be empty
    if (possibly_empty_constructor_sorts.empty())
    {
      return true; // There are no empty sorts
    }
    else
    {
      mCRL2log(error) << "the following domains are empty due to recursive constructors:" << std::endl;
      for (std::set < sort_expression >:: const_iterator i=possibly_empty_constructor_sorts.begin();
           i!=possibly_empty_constructor_sorts.end(); ++i)
      {
        mCRL2log(error) << core::pp_deprecated(*i) << std::endl;
      }
      return false;
    }
  }
  catch (mcrl2::runtime_error& e)
  {
    mCRL2log(error) << e.what() << std::endl;
    return false;
  }
  return false; // compiler warning
}

static bool gstcReadInFuncs(ATermList Cons, ATermList Maps)
{
  mCRL2log(debug) << "Start Read-in Func" << std::endl;
  bool Result=true;

  size_t constr_number=ATgetLength(Cons);
  for (ATermList Funcs=ATconcat(Cons,Maps); !ATisEmpty(Funcs); Funcs=ATgetNext(Funcs))
  {
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=ATAgetArgument(Func,0);
    ATermAppl FuncType=ATAgetArgument(Func,1);

    if (!gstcIsSortExprDeclared(FuncType))
    {
      return false;
    }

    //if FuncType is a defined function sort, unwind it
    //{ ATermAppl NewFuncType;
    //  if(gsIsSortId(FuncType)
    //   && (NewFuncType=ATAtableGet(context.defined_sorts,ATAgetArgument(FuncType,0)))
    //   && gsIsSortArrow(NewFuncType))
    //  FuncType=NewFuncType;
    //  }

    //if FuncType is a defined function sort, unwind it
    if (gsIsSortId(FuncType))
    {
      ATermAppl NewFuncType=gstcUnwindType(FuncType);
      if (gsIsSortArrow(NewFuncType))
      {
        FuncType=NewFuncType;
      }
    }

    if ((gsIsSortArrow(FuncType)))
    {
      if (!gstcAddFunction(gsMakeOpId(FuncName,FuncType),"function"))
      {
        return false;
      }
    }
    else
    {
      if (!gstcAddConstant(gsMakeOpId(FuncName,FuncType),"constant"))
      {
        mCRL2log(error) << "could not add constant" << std::endl;
        return false;
      }
    }

    if (constr_number)
    {
      constr_number--;

      //Here checks for the constructors
      ATermAppl ConstructorType=FuncType;
      if (gsIsSortArrow(ConstructorType))
      {
        ConstructorType=ATAgetArgument(ConstructorType,1);
      }
      ConstructorType=gstcUnwindType(ConstructorType);
      if (!gsIsSortId(ConstructorType) ||
          sort_bool::is_bool(sort_expression(ConstructorType)) ||
          sort_pos::is_pos(sort_expression(ConstructorType)) ||
          sort_nat::is_nat(sort_expression(ConstructorType)) ||
          sort_int::is_int(sort_expression(ConstructorType)) ||
          sort_real::is_real(sort_expression(ConstructorType))
          )
      {
        mCRL2log(error) << "Could not add constructor " << core::pp_deprecated(FuncName) << " of sort " << core::pp_deprecated(FuncType) << ". Constructors of built-in sorts are not allowed." << std::endl;
        return false;
      }
    }

    mCRL2log(debug) << "Read-in Func " << core::pp_deprecated(FuncName) << ", Types " << core::pp_deprecated(FuncType) << "" << std::endl;
  }

  // Check that the constructors are defined such that they cannot generate an empty sort.
  // E.g. in the specification sort D; cons f:D->D; the sort D must be necessarily empty, which is
  // forbidden. The function below checks whether such malicious specifications occur.

  if (!gstc_check_for_empty_constructor_domains(Cons))
  {
    return false;
  }

  return Result;
}

static bool gstcReadInActs(ATermList Acts)
{
  bool Result=true;
  for (; !ATisEmpty(Acts); Acts=ATgetNext(Acts))
  {
    ATermAppl Act=ATAgetFirst(Acts);
    ATermAppl ActName=ATAgetArgument(Act,0);
    ATermList ActType=ATLgetArgument(Act,1);

    if (!gstcIsSortExprListDeclared(ActType))
    {
      return false;
    }

    ATermList Types=ATLtableGet(context.actions, ActName);
    if (!&*Types)
    {
      Types=ATmakeList1(ActType);
    }
    else
    {
      // the table context.actions contains a list of types for each
      // action name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ActType, Types))
      {
        mCRL2log(error) << "double declaration of action " << core::pp_deprecated(ActName) << std::endl;
        return false;
      }
      else
      {
        Types=ATappend(Types,ActType);
      }
    }
    ATtablePut(context.actions,ActName,Types);
  }

  return Result;
}

static bool gstcReadInProcsAndInit(ATermList Procs, ATermAppl Init)
{
  bool Result=true;
  for (; !ATisEmpty(Procs); Procs=ATgetNext(Procs))
  {
    ATermAppl Proc=ATAgetFirst(Procs);
    ATermAppl ProcName=ATAgetArgument(ATAgetArgument(Proc,0),0);

    if (&*ATLtableGet(context.actions, ProcName))
    {
      mCRL2log(error) << "declaration of both process and action " << core::pp_deprecated(ProcName) << std::endl;
      return false;
    }

    ATermList ProcType=ATLgetArgument(ATAgetArgument(Proc,0),1);

    if (!gstcIsSortExprListDeclared(ProcType))
    {
      return false;
    }

    ATermList Types=ATLtableGet(context.processes,ProcName);
    if (!&*Types)
    {
      Types=ATmakeList1(ProcType);
    }
    else
    {
      // the table context.processes contains a list of types for each
      // process name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ProcType, Types))
      {
        mCRL2log(error) << "double declaration of process " << core::pp_deprecated(ProcName) << std::endl;
        return false;
      }
      else
      {
        Types=ATappend(Types,ProcType);
      }
    }
    ATtablePut(context.processes,ProcName,Types);

    //check that all formal parameters of the process are unique.
    ATermList ProcVars=ATLgetArgument(Proc,1);
    if (!gstcVarsUnique(ProcVars))
    {
      mCRL2log(error) << "the formal variables in process " << core::pp_deprecated(Proc) << " are not unique" << std::endl;
      return false;
    }

    ATtablePut(body.proc_pars,ATAgetArgument(Proc,0),ATLgetArgument(Proc,1));
    ATtablePut(body.proc_bodies,ATAgetArgument(Proc,0),ATAgetArgument(Proc,2));
  }
  ATtablePut(body.proc_pars,INIT_KEY(),ATmakeList0());
  ATtablePut(body.proc_bodies,INIT_KEY(),Init);

  return Result;
}

static bool gstcReadInPBESAndInit(ATermAppl PBEqnSpec, ATermAppl PBInit)
{
  bool Result=true;

  //ATermList PBFreeVars=ATLgetArgument(GlobVarSpec,0);
  ATermList PBEqns=ATLgetArgument(PBEqnSpec,0);

  for (; !ATisEmpty(PBEqns); PBEqns=ATgetNext(PBEqns))
  {
    ATermAppl PBEqn=ATAgetFirst(PBEqns);
    ATermAppl PBName=ATAgetArgument(ATAgetArgument(PBEqn,1),0);

    ATermList PBVars=ATLgetArgument(ATAgetArgument(PBEqn,1),1);

    ATermList PBType=ATmakeList0();
    for (ATermList l=PBVars; !ATisEmpty(l); l=ATgetNext(l))
    {
      PBType=ATinsert(PBType,ATAgetArgument(ATAgetFirst(l),1));
    }
    PBType=ATreverse(PBType);

    if (!gstcIsSortExprListDeclared(PBType))
    {
      return false;
    }

    ATermList Types=ATLtableGet(context.PBs,PBName);
    if (!&*Types)
    {
      Types=ATmakeList1(PBType);
    }
    else
    {
      // temporarily prohibit overloading here
      mCRL2log(error) << "attempt to overload propositional variable " << core::pp_deprecated(PBName) << std::endl;
      return false;
      // the table context.PBs contains a list of types for each
      // PBES name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(PBType, Types))
      {
        mCRL2log(error) << "double declaration of propositional variable " << core::pp_deprecated(PBName) << std::endl;
        return false;
      }
      else
      {
        Types=ATappend(Types,PBType);
      }
    }
    ATtablePut(context.PBs,PBName,Types);

    //check that all formal parameters of the PBES are unique.
    //if(!gstcVarsUnique(ATconcat(PBVars,PBFreeVars))){ gsErrorMsg("the formal and/or global variables in PBES %P are not unique\n",PBEqn); return ATfalse;}

    //This is a fake ProcVarId (There is no PBVarId)
    ATermAppl Index=gsMakeProcVarId(PBName,PBType);
    //ATtablePut(body.proc_freevars,Index,PBFreeVars);
    ATtablePut(body.proc_pars,Index,PBVars);
    ATtablePut(body.proc_bodies,Index,ATAgetArgument(PBEqn,2));
  }
  //ATtablePut(body.proc_freevars,(ATerm)INIT_KEY(),(ATerm)PBFreeVars);
  ATtablePut(body.proc_pars,INIT_KEY(),ATmakeList0());
  ATtablePut(body.proc_bodies,INIT_KEY(),ATAgetArgument(PBInit,0));

  return Result;
}

static ATermList gstcWriteProcs(ATermList oldprocs)
{
  ATermList Result=ATmakeList0();
  for (ATermList l=oldprocs; !ATisEmpty(l); l=ATgetNext(l))
  {
    ATermAppl ProcVar=ATAgetArgument(ATAgetFirst(l),0);
    if (ProcVar==INIT_KEY())
    {
      continue;
    }
    Result=ATinsert(Result,gsMakeProcEqn(ProcVar,
                    ATLtableGet(body.proc_pars,ProcVar),
                    ATAtableGet(body.proc_bodies,ProcVar)
                                               )
                   );
  }
  Result=ATreverse(Result);
  return Result;
}

static ATermList gstcWritePBES(ATermList oldPBES)
{
  ATermList Result=ATmakeList0();
  for (ATermList PBEqns=oldPBES; !ATisEmpty(PBEqns); PBEqns=ATgetNext(PBEqns))
  {
    ATermAppl PBEqn=ATAgetFirst(PBEqns);
    ATermAppl PBESVar=ATAgetArgument(PBEqn,1);

    ATermList PBType=ATmakeList0();
    for (ATermList l=ATLgetArgument(PBESVar,1); !ATisEmpty(l); l=ATgetNext(l))
    {
      PBType=ATinsert(PBType,ATAgetArgument(ATAgetFirst(l),1));
    }
    PBType=ATreverse(PBType);

    ATermAppl Index=gsMakeProcVarId(ATAgetArgument(PBESVar,0),PBType);

    if (Index==INIT_KEY())
    {
      continue;
    }
    Result=ATinsert(Result,ATsetArgument(PBEqn,ATAtableGet(body.proc_bodies,Index),2));
  }
  return ATreverse(Result);
}


static bool gstcTransformVarConsTypeData(void)
{
  bool Result=true;
  ATermTable DeclaredVars=ATtableCreate(63,50);
  ATermTable FreeVars=ATtableCreate(63,50);

  //data terms in equations
  ATermList NewEqns=ATmakeList0();
  bool b = true;
  for (ATermList Eqns=body.equations; !ATisEmpty(Eqns); Eqns=ATgetNext(Eqns))
  {
    ATermAppl Eqn=ATAgetFirst(Eqns);
    ATermList VarList=ATLgetArgument(Eqn,0);

    if (!gstcVarsUnique(VarList))
    {
      b = false;
      mCRL2log(error) << "the variables " << core::pp_deprecated(VarList) << " in equation declaration " << core::pp_deprecated(Eqn) << " are not unique" << std::endl;
      break;
    }

    ATermTable NewDeclaredVars;
    if (!gstcAddVars2Table(DeclaredVars,VarList,NewDeclaredVars))
    {
      b = false;
      break;
    }
    else
    {
      DeclaredVars=NewDeclaredVars;
    }

    ATermAppl Left=ATAgetArgument(Eqn,2);
    ATermAppl LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,data::unknown_sort(),FreeVars,false,true);
    if (!&*LeftType)
    {
      b = false;
      mCRL2log(error) << "error occurred while typechecking " << core::pp_deprecated(Left) << " as left hand side of equation " << core::pp_deprecated(Eqn) << std::endl;
      break;
    }
    if (was_warning_upcasting)
    {
      was_warning_upcasting=false;
      mCRL2log(warning) << "warning occurred while typechecking " << core::pp_deprecated(Left) << " as left hand side of equation " << core::pp_deprecated(Eqn) << std::endl;
    }

    ATermAppl Cond=ATAgetArgument(Eqn,1);
    if (!&*gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Cond,sort_bool::bool_()))
    {
      b = false;
      break;
    }
    ATermAppl Right=ATAgetArgument(Eqn,3);
    ATermTable dummy_empty_table;
    ATermAppl RightType=gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Right,LeftType,dummy_empty_table,false);
    if (!&*RightType)
    {
      b = false;
      mCRL2log(error) << "error occurred while typechecking " << core::pp_deprecated(Right) << " as right hand side of equation " << core::pp_deprecated(Eqn) << std::endl;
      break;
    }

    //If the types are not uniquely the same now: do once more:
    if (!gstcEqTypesA(LeftType,RightType))
    {
      ATermAppl Type=gstcTypeMatchA(LeftType,RightType);
      if (!&*Type)
      {
        mCRL2log(error) << "types of the left- (" << core::pp_deprecated(LeftType) << ") and right- (" << core::pp_deprecated(RightType) << ") hand-sides of the equation " << core::pp_deprecated(Eqn) << " do not match" << std::endl;
        b = false;
        break;
      }
      Left=ATAgetArgument(Eqn,2);
      ATtableReset(FreeVars);
      LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,Type,FreeVars,true);
      if (!&*LeftType)
      {
        b = false;
        mCRL2log(error) << "types of the left- and right-hand-sides of the equation " << core::pp_deprecated(Eqn) << " do not match" << std::endl;
        break;
      }
      if (was_warning_upcasting)
      {
        was_warning_upcasting=false;
        mCRL2log(warning) << "warning occurred while typechecking " << core::pp_deprecated(Left) << " as left hand side of equation " << core::pp_deprecated(Eqn) << std::endl;
      }
      Right=ATAgetArgument(Eqn,3);
      RightType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Right,LeftType,FreeVars);
      if (!&*RightType)
      {
        b = false;
        mCRL2log(error) << "types of the left- and right-hand-sides of the equation " << core::pp_deprecated(Eqn) << " do not match" << std::endl;
        break;
      }
      Type=gstcTypeMatchA(LeftType,RightType);
      if (!&*Type)
      {
        mCRL2log(error) << "types of the left- (" << core::pp_deprecated(LeftType) << ") and right- (" << core::pp_deprecated(RightType) << ") hand-sides of the equation " << core::pp_deprecated(Eqn) << " do not match" << std::endl;
        b = false;
        break;
      }
      if (gstcHasUnknown(Type))
      {
        mCRL2log(error) << "types of the left- (" << core::pp_deprecated(LeftType) << ") and right- (" << core::pp_deprecated(RightType) << ") hand-sides of the equation " << core::pp_deprecated(Eqn) << " cannot be uniquely determined" << std::endl;
        b = false;
        break;
      }
    }
    ATtableReset(DeclaredVars);
    NewEqns=ATinsert(NewEqns,gsMakeDataEqn(VarList,Cond,Left,Right));
  }
  if (b)
  {
    body.equations=ATreverse(NewEqns);
  }

  ATtableDestroy(FreeVars);
  ATtableDestroy(DeclaredVars);
  return b?Result:false;
}

static bool gstcTransformActProcVarConst(void)
{
  bool Result=true;
  ATermTable Vars=ATtableCreate(63,50);

  //process and data terms in processes and init
  for (ATermList ProcVars=ATtableKeys(body.proc_pars); !ATisEmpty(ProcVars); ProcVars=ATgetNext(ProcVars))
  {
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    ATtableReset(Vars);
    gstcATermTableCopy(context.glob_vars,Vars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,ProcVar),NewVars))
    {
      Result = false;
      break;
    }
    else
    {
      Vars=NewVars;
    }

    ATermAppl NewProcTerm=gstcTraverseActProcVarConstP(Vars,ATAtableGet(body.proc_bodies,ProcVar));
    if (!&*NewProcTerm)
    {
      Result = false;
      break;
    }
    ATtablePut(body.proc_bodies,ProcVar,NewProcTerm);
  }

  ATtableDestroy(Vars);
  return Result;
}

static bool gstcTransformPBESVarConst(void)
{
  bool Result=true;
  ATermTable Vars=ATtableCreate(63,50);

  //PBEs and data terms in PBEqns and init
  for (ATermList PBVars=ATtableKeys(body.proc_pars); !ATisEmpty(PBVars); PBVars=ATgetNext(PBVars))
  {
    ATermAppl PBVar=ATAgetFirst(PBVars);
    ATtableReset(Vars);
    gstcATermTableCopy(context.glob_vars,Vars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,PBVar),NewVars))
    {
      Result = false;
      break;
    }
    else
    {
      Vars=NewVars;
    }

    ATermAppl NewPBTerm=gstcTraversePBESVarConstPB(Vars,ATAtableGet(body.proc_bodies,PBVar));
    if (!&*NewPBTerm)
    {
      Result = false;
      break;
    }
    ATtablePut(body.proc_bodies,PBVar,NewPBTerm);
  }

  ATtableDestroy(Vars);
  return Result;
}


// ======== Auxiliary functions
static bool gstcInTypesA(ATermAppl Type, ATermList Types)
{
  for (; !ATisEmpty(Types); Types=ATgetNext(Types))
    if (gstcEqTypesA(Type,ATAgetFirst(Types)))
    {
      return true;
    }
  return false;
}

static bool gstcEqTypesA(ATermAppl Type1, ATermAppl Type2)
{
  if (ATisEqual(Type1, Type2))
  {
    return true;
  }

  if (!&*Type1 || !&*Type2)
  {
    return false;
  }

  return ATisEqual(gstcUnwindType(Type1),gstcUnwindType(Type2));
}

static bool gstcInTypesL(ATermList Type, ATermList Types)
{
  for (; !ATisEmpty(Types); Types=ATgetNext(Types))
    if (gstcEqTypesL(Type,ATLgetFirst(Types)))
    {
      return true;
    }
  return false;
}

static bool gstcEqTypesL(ATermList Type1, ATermList Type2)
{
  if (ATisEqual(Type1, Type2))
  {
    return true;
  }
  if (!&*Type1 || !&*Type2)
  {
    return false;
  }
  if (ATgetLength(Type1)!=ATgetLength(Type2))
  {
    return false;
  }
  for (; !ATisEmpty(Type1); Type1=ATgetNext(Type1),Type2=ATgetNext(Type2))
    if (!gstcEqTypesA(ATAgetFirst(Type1),ATAgetFirst(Type2)))
    {
      return false;
    }
  return true;
}

static bool gstcIsSortDeclared(ATermAppl SortName)
{

  if (sort_bool::is_bool(basic_sort(core::identifier_string(SortName))) ||
      sort_pos::is_pos(basic_sort(core::identifier_string(SortName))) ||
      sort_nat::is_nat(basic_sort(core::identifier_string(SortName))) ||
      sort_int::is_int(basic_sort(core::identifier_string(SortName))) ||
      sort_real::is_real(basic_sort(core::identifier_string(SortName))))
  {
    return true;
  }
  if (ATindexedSetGetIndex(context.basic_sorts, SortName)>=0)
  {
    return true;
  }
  if (&*ATAtableGet(context.defined_sorts,SortName))
  {
    return true;
  }
  return false;
}

static bool gstcIsSortExprDeclared(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr))
  {
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if (!gstcIsSortDeclared(SortName))
    {
      mCRL2log(error) << "basic or defined sort " << core::pp_deprecated(SortName) << " is not declared" << std::endl;
      return false;
    }
    return true;
  }

  if (gsIsSortCons(SortExpr))
  {
    return gstcIsSortExprDeclared(ATAgetArgument(SortExpr, 1));
  }

  if (gsIsSortArrow(SortExpr))
  {
    if (!gstcIsSortExprDeclared(ATAgetArgument(SortExpr,1)))
    {
      return false;
    }
    if (!gstcIsSortExprListDeclared(ATLgetArgument(SortExpr,0)))
    {
      return false;
    }
    return true;
  }

  if (gsIsSortStruct(SortExpr))
  {
    for (ATermList Constrs=ATLgetArgument(SortExpr,0); !ATisEmpty(Constrs); Constrs=ATgetNext(Constrs))
    {
      ATermAppl Constr=ATAgetFirst(Constrs);

      ATermList Projs=ATLgetArgument(Constr,1);
      for (; !ATisEmpty(Projs); Projs=ATgetNext(Projs))
      {
        ATermAppl Proj=ATAgetFirst(Projs);
        ATermAppl ProjSort=ATAgetArgument(Proj,1);

        // not to forget, recursive call for ProjSort ;-)
        if (!gstcIsSortExprDeclared(ProjSort))
        {
          return false;
        }
      }
    }
    return true;
  }

  assert(0);
  mCRL2log(error) << "this is not a sort expression " << atermpp::aterm(SortExpr) << std::endl;
  return false;
}

static bool gstcIsSortExprListDeclared(ATermList SortExprList)
{
  for (; !ATisEmpty(SortExprList); SortExprList=ATgetNext(SortExprList))
    if (!gstcIsSortExprDeclared(ATAgetFirst(SortExprList)))
    {
      return false;
    }
  return true;
}


static bool gstcReadInSortStruct(ATermAppl SortExpr)
{
  bool Result=true;

  if (gsIsSortId(SortExpr))
  {
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if (!gstcIsSortDeclared(SortName))
    {
      mCRL2log(error) << "basic or defined sort " << core::pp_deprecated(SortName) << " is not declared" << std::endl;
      return false;
    }
    return true;
  }

  if (gsIsSortCons(SortExpr))
  {
    return gstcReadInSortStruct(ATAgetArgument(SortExpr,1));
  }

  if (gsIsSortArrow(SortExpr))
  {
    if (!gstcReadInSortStruct(ATAgetArgument(SortExpr,1)))
    {
      return false;
    }
    for (ATermList Sorts=ATLgetArgument(SortExpr,0); !ATisEmpty(Sorts); Sorts=ATgetNext(Sorts))
    {
      if (!gstcReadInSortStruct(ATAgetFirst(Sorts)))
      {
        return false;
      }
    }
    return true;
  }

  if (gsIsSortStruct(SortExpr))
  {
    for (ATermList Constrs=ATLgetArgument(SortExpr,0); !ATisEmpty(Constrs); Constrs=ATgetNext(Constrs))
    {
      ATermAppl Constr=ATAgetFirst(Constrs);

      // recognizer -- if present -- a function from SortExpr to Bool
      ATermAppl Name=ATAgetArgument(Constr,2);
      if (!gsIsNil(Name) &&
          !gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(ATmakeList1(SortExpr),sort_bool::bool_())),"recognizer"))
      {
        return false;
      }

      // constructor type and projections
      ATermList Projs=ATLgetArgument(Constr,1);
      Name=ATAgetArgument(Constr,0);
      if (ATisEmpty(Projs))
      {
        if (!gstcAddConstant(gsMakeOpId(Name,SortExpr),"constructor constant"))
        {
          return false;
        }
        else
        {
          continue;
        }
      }

      ATermList ConstructorType=ATmakeList0();
      for (; !ATisEmpty(Projs); Projs=ATgetNext(Projs))
      {
        ATermAppl Proj=ATAgetFirst(Projs);
        ATermAppl ProjSort=ATAgetArgument(Proj,1);

        // not to forget, recursive call for ProjSort ;-)
        if (!gstcReadInSortStruct(ProjSort))
        {
          return false;
        }

        ATermAppl ProjName=ATAgetArgument(Proj,0);
        if (!gsIsNil(ProjName) &&
            !gstcAddFunction(gsMakeOpId(ProjName,gsMakeSortArrow(ATmakeList1(SortExpr),ProjSort)),"projection",true))
        {
          return false;
        }
        ConstructorType=ATinsert(ConstructorType,ProjSort);
      }
      if (!gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(ATreverse(ConstructorType),SortExpr)),"constructor"))
      {
        return false;
      }
    }
    return true;
  }

  assert(0);
  return Result;
}

static bool gstcAddConstant(ATermAppl OpId, const char* msg)
{
  assert(gsIsOpId(OpId));
  bool Result=true;

  ATermAppl Name = data::function_symbol(OpId).name();
  ATermAppl Sort = data::function_symbol(OpId).sort();

  if (&*ATAtableGet(context.constants, Name) /*|| ATLtableGet(context.functions, (ATerm)Name)*/)
  {
    mCRL2log(error) << "double declaration of " << msg << " " << core::pp_deprecated(Name) << std::endl;
    return false;
  }

  if (&*ATLtableGet(gssystem.constants, Name) || &*ATLtableGet(gssystem.functions, Name))
  {
    mCRL2log(error) << "attempt to declare a constant with the name that is a built-in identifier (" << core::pp_deprecated(Name) << ")" << std::endl;
    return false;
  }

  ATtablePut(context.constants, Name, Sort);
  return Result;
}

static bool gstcAddFunction(ATermAppl OpId, const char* msg, bool allow_double_decls)
{
  assert(gsIsOpId(OpId));
  bool Result=true;
  const data::function_symbol f(OpId);
  const sort_expression_list domain=function_sort(f.sort()).domain();
  ATermAppl Name = f.name();
  ATermAppl Sort = f.sort();

  //constants and functions can have the same names
  //  if(ATAtableGet(context.constants, (ATerm)Name)){
  //    ThrowMF("Double declaration of constant and %s %T\n", msg, Name);
  //  }

  if (domain.size()==0)
  {
    if (&*ATAtableGet(gssystem.constants, Name))
    {
      mCRL2log(error) << "attempt to redeclare the system constant with " << msg << " " << core::pp_deprecated(OpId) << std::endl;
      return false;
    }
  }
  else // domain.size()>0
  {
    ATermList L=ATLtableGet(gssystem.functions, Name);
    for (; L!=ATerm() && L!=ATempty ; L=ATgetNext(L))
    {
      if (gstcTypeMatchA(Sort,(ATermAppl)ATgetFirst(L))!=ATerm())
      {
        // f matches a predefined function
        mCRL2log(error) << "attempt to redeclare a system function with " << msg << " " << core::pp_deprecated(OpId) << ":" << core::pp_deprecated(Sort) << std::endl;
        return false;
      }
    }
  }

  ATermList Types=ATLtableGet(context.functions, Name);
  // the table context.functions contains a list of types for each
  // function name. We need to check if there is already such a type
  // in the list. If so -- error, otherwise -- add
  if ((&*Types) && gstcInTypesA(Sort, Types))
  {
    if (!allow_double_decls)
    {
      mCRL2log(error) << "double declaration of " << msg << " " << core::pp_deprecated(Name) << std::endl;
      return false;
    }
  }
  else
  {
    if (!&*Types)
    {
      Types=ATmakeList0();
    }
    Types=ATappend(Types,Sort);
    ATtablePut(context.functions,Name,Types);
  }
  mCRL2log(debug) << "Read-in " << msg << " " << core::pp_deprecated(Name) << ". Type " << core::pp_deprecated(Types) << "" << std::endl;
  return Result;
}

static void gstcAddSystemConstant(ATermAppl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.constants table
  assert(gsIsOpId(OpId));
  ATermAppl OpIdName = data::function_symbol(OpId).name();
  ATermAppl Type = data::function_symbol(OpId).sort();

  ATermList Types=ATLtableGet(gssystem.constants, OpIdName);

  if (!&*Types)
  {
    Types=ATmakeList0();
  }
  Types=ATappend(Types,Type);
  ATtablePut(gssystem.constants,OpIdName,Types);
}

static void gstcAddSystemFunction(ATermAppl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  assert(gsIsOpId(OpId));
  ATermAppl OpIdName = data::function_symbol(OpId).name();
  ATermAppl Type = data::function_symbol(OpId).sort();
  assert(gsIsSortArrow(Type));

  ATermList Types=ATLtableGet(gssystem.functions, OpIdName);

  if (!&*Types)
  {
    Types=ATmakeList0();
  }
  Types=ATappend(Types,Type);  // TODO: Avoid ATappend!!!! But the order is essential.
  ATtablePut(gssystem.functions,OpIdName,Types);
}

static void gstcATermTableCopy(const ATermTable &Orig, ATermTable &Copy)
{
  for (ATermList Keys=ATtableKeys(Orig); !ATisEmpty(Keys); Keys=ATgetNext(Keys))
  {
    ATerm Key=ATgetFirst(Keys);
    ATtablePut(Copy,Key,ATtableGet(Orig,Key));
  }
}


static bool gstcVarsUnique(ATermList VarDecls)
{
  bool Result=true;
  ATermIndexedSet Temp=ATindexedSetCreate(63,50);

  for (; !ATisEmpty(VarDecls); VarDecls=ATgetNext(VarDecls))
  {
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    bool nnew;
    ATindexedSetPut(Temp, VarName, &nnew);
    if (!nnew)
    {
      Result=false;
      goto final;
    }
  }

final:
  ATindexedSetDestroy(Temp);
  return Result;
}

static bool gstcAddVars2Table(ATermTable &Vars, ATermList VarDecls, ATermTable &result)
{
  for (; !ATisEmpty(VarDecls); VarDecls=ATgetNext(VarDecls))
  {
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    ATermAppl VarType=ATAgetArgument(VarDecl,1);
    //test the type
    if (!gstcIsSortExprDeclared(VarType))
    {
      return false;
    }

    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    ATtablePut(Vars, VarName, VarType);
  }
  result=Vars;
  return true;
}

static ATermTable gstcRemoveVars(ATermTable &Vars, ATermList VarDecls)
{
  for (; !ATisEmpty(VarDecls); VarDecls=ATgetNext(VarDecls))
  {
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    //ATermAppl VarType=ATAgetArgument(VarDecl,1);

    ATtableRemove(Vars, VarName);
  }

  return Vars;
}

static ATermAppl gstcRewrActProc(const ATermTable &Vars, ATermAppl ProcTerm, bool is_pbes)
{
  ATermAppl Result=NULL;
  ATermAppl Name=ATAgetArgument(ProcTerm,0);
  ATermList ParList;

  bool action=false;

  if (!is_pbes)
  {
    if (&*(ParList=ATLtableGet(context.actions,Name)))
    {
      action=true;
    }
    else
    {
      if (&*(ParList=ATLtableGet(context.processes,Name)))
      {
        action=false;
      }
      else
      {
        mCRL2log(error) << "action or process " << core::pp_deprecated(Name) << " not declared" << std::endl;
        return NULL;
      }
    }
  }
  else
  {
    if (!&*(ParList=ATLtableGet(context.PBs,Name)))
    {
      mCRL2log(error) << "propositional variable " << core::pp_deprecated(Name) << " not declared" << std::endl;
      return NULL;
    }
  }
  assert(!ATisEmpty(ParList));

  size_t nFactPars=ATgetLength(ATLgetArgument(ProcTerm,1));
  const char* msg=(is_pbes)?"propositional variable":((action)?"action":"process");

  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    ATermList NewParList=ATmakeList0();
    for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
    {
      ATermList Par=ATLgetFirst(ParList);
      if (ATgetLength(Par)==nFactPars)
      {
        NewParList=ATinsert(NewParList,Par);
      }
    }
    ParList=ATreverse(NewParList);
  }

  if (ATisEmpty(ParList))
  {
    mCRL2log(error) << "no " << msg << " " << core::pp_deprecated(Name)
                    << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"")
                    << " is declared (while typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
    return NULL;
  }

  if (ATgetLength(ParList)==1)
  {
    Result=gstcMakeActionOrProc(action,Name,ATLgetFirst(ParList),ATLgetArgument(ProcTerm,1));
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=gstcMakeActionOrProc(action,Name,gstcGetNotInferredList(ParList),ATLgetArgument(ProcTerm,1));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  ATermList PosTypeList=ATLgetArgument(ATAgetArgument(Result,0),1);

  ATermList NewPars=ATmakeList0();
  ATermList NewPosTypeList=ATmakeList0();
  for (ATermList Pars=ATLgetArgument(ProcTerm,1); !ATisEmpty(Pars); Pars=ATgetNext(Pars),PosTypeList=ATgetNext(PosTypeList))
  {
    ATermAppl Par=ATAgetFirst(Pars);
    ATermAppl PosType=ATAgetFirst(PosTypeList);

    ATermAppl NewPosType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,PosType); //gstcExpandNumTypesDown(PosType));

    if (!&*NewPosType)
    {
      mCRL2log(error) << "cannot typecheck " << core::pp_deprecated(Par) << " as type " << core::pp_deprecated(gstcExpandNumTypesDown(PosType)) << " (while typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      return NULL;
    }
    NewPars=ATinsert(NewPars,Par);
    NewPosTypeList=ATinsert(NewPosTypeList,NewPosType);
  }
  NewPars=ATreverse(NewPars);
  NewPosTypeList=ATreverse(NewPosTypeList);

  PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);

  if (!&*PosTypeList)
  {
    PosTypeList=ATLgetArgument(ATAgetArgument(Result,0),1);
    ATermList Pars=NewPars;
    NewPars=ATmakeList0();
    ATermList CastedPosTypeList=ATmakeList0();
    for (; !ATisEmpty(Pars); Pars=ATgetNext(Pars),PosTypeList=ATgetNext(PosTypeList),NewPosTypeList=ATgetNext(NewPosTypeList))
    {
      ATermAppl Par=ATAgetFirst(Pars);
      ATermAppl PosType=ATAgetFirst(PosTypeList);
      ATermAppl NewPosType=ATAgetFirst(NewPosTypeList);

      ATermAppl CastedNewPosType=gstcUpCastNumericType(PosType,NewPosType,&Par);
      if (!&*CastedNewPosType)
      {
        mCRL2log(error) << "cannot cast " << core::pp_deprecated(NewPosType) << " to "
                        << core::pp_deprecated(PosType) << "(while typechecking " << core::pp_deprecated(Par)
                        << " in " << core::pp_deprecated(ProcTerm) << std::endl;
        return NULL;
      }

      NewPars=ATinsert(NewPars,Par);
      CastedPosTypeList=ATinsert(CastedPosTypeList,CastedNewPosType);
    }
    NewPars=ATreverse(NewPars);
    NewPosTypeList=ATreverse(CastedPosTypeList);

    PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);
  }

  if (!&*PosTypeList)
  {
    mCRL2log(error) << "no " << msg << " " << core::pp_deprecated(Name) << "with type "
                    << core::pp_deprecated(NewPosTypeList) << " is declared (while typechecking "
                    << core::pp_deprecated(ProcTerm) << ")" << std::endl;
    return NULL;
  }

  if (gstcIsNotInferredL(PosTypeList))
  {
    mCRL2log(error) << "ambiguous " << msg << " " << core::pp_deprecated(Name) << std::endl;
    return NULL;
  }

  Result=gstcMakeActionOrProc(action,Name,PosTypeList,NewPars);

  if (is_pbes)
  {
    Result=ATsetArgument(ProcTerm,NewPars,1);
  }

  return Result;
}

static inline ATermAppl gstcMakeActionOrProc(bool action, ATermAppl Name,
    ATermList FormParList, ATermList FactParList)
{
  return (action)?gsMakeAction(gsMakeActId(Name,FormParList),FactParList)
         :gsMakeProcess(gsMakeProcVarId(Name,FormParList),FactParList);
}

static ATermAppl gstcTraverseActProcVarConstP(const ATermTable &Vars, ATermAppl ProcTerm)
{
  ATermAppl Result=NULL;
  size_t n = ATgetArity(ATgetAFun(ProcTerm));
  if (n==0)
  {
    return ProcTerm;
  }

  //Here the code for short-hand assignments begins.
  if (gsIsIdAssignment(ProcTerm))
  {
    mCRL2log(debug) << "typechecking a process call with short-hand assignments " << &*ProcTerm << "" << std::endl;
    ATermAppl Name=ATAgetArgument(ProcTerm,0);
    ATermList ParList=ATLtableGet(context.processes,Name);
    if (!&*ParList)
    {
      mCRL2log(error) << "process " << core::pp_deprecated(Name) << " not declared" << std::endl;
      return NULL;
    }

    // Put the assignments into a table
    ATermTable As=ATtableCreate(63,50);
    for (ATermList l=ATLgetArgument(ProcTerm,1); !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermAppl a=ATAgetFirst(l);
      ATerm existing_rhs = ATtableGet(As,ATAgetArgument(a,0));
      if (&*existing_rhs == NULL) // An assignment of the shape x:=t does not yet exist, this is OK.
      {
        ATtablePut(As,ATAgetArgument(a,0),ATAgetArgument(a,1));
      }
      else
      {
        mCRL2log(error) << "Double assignment to variable " << core::pp_deprecated(ATAgetArgument(a,0))
                        << " (detected assigned values are " << core::pp_deprecated(existing_rhs)
                        << " and " << core::pp_deprecated(ATAgetArgument(a,1)) << ")" << std::endl;
        return NULL;
      }
    }

    {
      // Now filter the ParList to contain only the processes with parameters in this process call with assignments
      ATermList NewParList=ATmakeList0();
      assert(!ATisEmpty(ParList));
      ATermAppl Culprit=NULL; // Variable used for more intelligible error messages.
      for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
      {
        ATermList Par=ATLgetFirst(ParList);

        // get the formal parameter names
        ATermList FormalPars=ATLtableGet(body.proc_pars,gsMakeProcVarId(Name,Par));
        // we only need the names of the parameters, not the types
        ATermList FormalParNames=ATmakeList0();
        for (; !ATisEmpty(FormalPars); FormalPars=ATgetNext(FormalPars))
        {
          FormalParNames=ATinsert(FormalParNames,ATAgetArgument(ATAgetFirst(FormalPars),0));
        }

        ATermList l=list_minus(ATtableKeys(As),FormalParNames);
        if (ATisEmpty(l))
        {
          NewParList=ATinsert(NewParList,Par);
        }
        else
        {
          Culprit=ATAgetFirst(l);
        }
      }
      ParList=ATreverse(NewParList);

      if (ATisEmpty(ParList))
      {
        ATtableDestroy(As);
        mCRL2log(error) << "no process " << core::pp_deprecated(Name) << " containing all assignments in "
                        << core::pp_deprecated(ProcTerm) << "." << std::endl
                        << "Problematic variable is " << core::pp_deprecated(Culprit) << "." << std::endl;
        return NULL;
      }
      if (!ATisEmpty(ATgetNext(ParList)))
      {
        ATtableDestroy(As);
        mCRL2log(error) << "ambiguous process " << core::pp_deprecated(Name) << " containing all assignments in " << core::pp_deprecated(ProcTerm) << "." << std::endl;
        return NULL;
      }
    }

    // get the formal parameter names
    ATermList ActualPars=ATmakeList0();
    ATermList FormalPars=ATLtableGet(body.proc_pars,gsMakeProcVarId(Name,ATLgetFirst(ParList)));
    {
      // we only need the names of the parameters, not the types
      for (ATermList l=FormalPars; !ATisEmpty(l); l=ATgetNext(l))
      {
        ATermAppl FormalParName=ATAgetArgument(ATAgetFirst(l),0);
        ATermAppl ActualPar=ATAtableGet(As,FormalParName);
        if (!&*ActualPar)
        {
          ActualPar=gsMakeId(FormalParName);
        }
        ActualPars=ATinsert(ActualPars,ActualPar);
      }
      ActualPars=ATreverse(ActualPars);
    }

    ATermAppl TypeCheckedProcTerm=gstcRewrActProc(Vars, gsMakeParamId(Name,ActualPars));
    if (!&*TypeCheckedProcTerm)
    {
      ATtableDestroy(As);
      mCRL2log(error) << "type error occurred while typechecking the process call with short-hand assignments " << core::pp_deprecated(ProcTerm) << std::endl;
      return NULL;
    }

    //reverse the assignments
    ATtableReset(As);
    for (ATermList l=ATLgetArgument(TypeCheckedProcTerm,1),m=FormalPars; !ATisEmpty(l); l=ATgetNext(l),m=ATgetNext(m))
    {
      ATermAppl act_par=ATAgetFirst(l);
      ATermAppl form_par=ATAgetFirst(m);
      if (ATisEqual(form_par,act_par))
      {
        continue;  //parameter does not change
      }
      ATtablePut(As,ATAgetArgument(form_par,0),gsMakeDataVarIdInit(form_par,act_par));
    }

    ATermList TypedAssignments=ATmakeList0();
    for (ATermList l=ATLgetArgument(ProcTerm,1); !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermAppl a=ATAgetFirst(l);
      a=ATAtableGet(As,ATAgetArgument(a,0));
      if (!&*a)
      {
        continue;
      }
      TypedAssignments=ATinsert(TypedAssignments,a);
    }
    TypedAssignments=ATreverse(TypedAssignments);

    ATtableDestroy(As);

    TypeCheckedProcTerm=gsMakeProcessAssignment(ATAgetArgument(TypeCheckedProcTerm,0),TypedAssignments);

    return TypeCheckedProcTerm;
  }
  //Here it ends.

  if (gsIsParamId(ProcTerm))
  {
    ATermAppl a= gstcRewrActProc(Vars,ProcTerm);
    return a;
  }

  if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm) ||
      gsIsRename(ProcTerm) || gsIsComm(ProcTerm) || gsIsAllow(ProcTerm))
  {

    //block & hide
    if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm))
    {
      const char* msg=gsIsBlock(ProcTerm)?"Blocking":"Hiding";
      ATermList ActList=ATLgetArgument(ProcTerm,0);
      if (ATisEmpty(ActList))
      {
        mCRL2log(warning) << msg << " empty set of actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }

      ATermIndexedSet Acts=ATindexedSetCreate(63,50);
      for (; !ATisEmpty(ActList); ActList=ATgetNext(ActList))
      {
        ATermAppl Act=ATAgetFirst(ActList);

        //Actions must be declared
        if (!&*ATtableGet(context.actions,Act))
        {
          mCRL2log(error) << msg << " an undefined action " << core::pp_deprecated(Act) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return NULL;
        }
        bool nnew;
        ATindexedSetPut(Acts,Act,&nnew);
        if (!nnew)
        {
          mCRL2log(warning) << msg << " action " << core::pp_deprecated(Act) << " twice (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
        }
      }
      ATindexedSetDestroy(Acts);
    }

    //rename
    if (gsIsRename(ProcTerm))
    {
      ATermList RenList=ATLgetArgument(ProcTerm,0);

      if (ATisEmpty(RenList))
      {
        mCRL2log(warning) << "renaming empty set of actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }

      ATermIndexedSet ActsFrom=ATindexedSetCreate(63,50);

      for (; !ATisEmpty(RenList); RenList=ATgetNext(RenList))
      {
        ATermAppl Ren=ATAgetFirst(RenList);
        ATermAppl ActFrom=ATAgetArgument(Ren,0);
        ATermAppl ActTo=ATAgetArgument(Ren,1);

        if (ATisEqual(ActFrom,ActTo))
        {
          mCRL2log(warning) << "renaming action " << core::pp_deprecated(ActFrom) << " into itself (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
        }

        //Actions must be declared and of the same types
        ATermList TypesFrom,TypesTo;
        if (!&*(TypesFrom=ATLtableGet(context.actions,ActFrom)))
        {
          mCRL2log(error) << "renaming an undefined action " << core::pp_deprecated(ActFrom) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return NULL;
        }
        if (!&*(TypesTo=ATLtableGet(context.actions,ActTo)))
        {
          mCRL2log(error) << "renaming into an undefined action " << core::pp_deprecated(ActTo) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return NULL;
        }

        TypesTo=gstcTypeListsIntersect(TypesFrom,TypesTo);
        if (!&*TypesTo || ATisEmpty(TypesTo))
        {
          mCRL2log(error) << "renaming action " << core::pp_deprecated(ActFrom) << " into action " << core::pp_deprecated(ActTo) << ": these two have no common type (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return NULL;
        }

        bool nnew;
        ATindexedSetPut(ActsFrom,ActFrom,&nnew);
        if (!nnew)
        {
          mCRL2log(error) << "renaming action " << core::pp_deprecated(ActFrom) << " twice (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return NULL;
        }
      }
      ATindexedSetDestroy(ActsFrom);
    }

    //comm: like renaming multiactions (with the same parameters) to action/tau
    if (gsIsComm(ProcTerm))
    {
      ATermList CommList=ATLgetArgument(ProcTerm,0);

      if (ATisEmpty(CommList))
      {
        mCRL2log(warning) << "synchronizing empty set of (multi)actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }
      else
      {
        ATermList ActsFrom=ATmakeList0();

        for (; !ATisEmpty(CommList); CommList=ATgetNext(CommList))
        {
          ATermAppl Comm=ATAgetFirst(CommList);
          ATermList MActFrom=ATLgetArgument(ATAgetArgument(Comm,0),0);
          ATermList BackupMActFrom=MActFrom;
          assert(!ATisEmpty(MActFrom));
          ATermAppl ActTo=ATAgetArgument(Comm,1);

          if (ATgetLength(MActFrom)==1)
          {
            mCRL2log(error) << "using synchronization as renaming/hiding of action " << core::pp_deprecated(ATgetFirst(MActFrom))
                            << " into " << core::pp_deprecated(ActTo) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
            return NULL;
          }

          //Actions must be declared
          ATermList ResTypes=NULL;

          if (!gsIsNil(ActTo))
          {
            ResTypes=ATLtableGet(context.actions,ActTo);
            if (!&*ResTypes)
            {
              mCRL2log(error) << "synchronizing to an undefined action " << core::pp_deprecated(ActTo) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return NULL;
            }
          }

          for (; !ATisEmpty(MActFrom); MActFrom=ATgetNext(MActFrom))
          {
            ATermAppl Act=ATAgetFirst(MActFrom);
            ATermList Types=ATLtableGet(context.actions,Act);
            if (!&*Types)
            {
              mCRL2log(error) << "synchronizing an undefined action " << core::pp_deprecated(Act) << " in (multi)action " << core::pp_deprecated(MActFrom) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return NULL;
            }
            ResTypes=(&*ResTypes)?gstcTypeListsIntersect(ResTypes,Types):Types;
            if (!&*ResTypes || ATisEmpty(ResTypes))
            {
              mCRL2log(error) << "synchronizing action " << core::pp_deprecated(Act) << " from (multi)action " << core::pp_deprecated(BackupMActFrom)
                              << " into action " << core::pp_deprecated(ActTo) << ": these have no common type (typechecking " << core::pp_deprecated(ProcTerm) << "), ResTypes: " << atermpp::aterm(ResTypes) << std::endl;
              return NULL;
            }
          }
          MActFrom=BackupMActFrom;

          //the multiactions in the lhss of comm should not intersect.
          //make the list of unique actions
          ATermList Acts=ATmakeList0();
          for (; !ATisEmpty(MActFrom); MActFrom=ATgetNext(MActFrom))
          {
            ATermAppl Act=ATAgetFirst(MActFrom);
            if (ATindexOf(Acts,Act)==ATERM_NON_EXISTING_POSITION)
            {
              Acts=ATinsert(Acts,Act);
            }
          }
          for (; !ATisEmpty(Acts); Acts=ATgetNext(Acts))
          {
            ATermAppl Act=ATAgetFirst(Acts);
            if (ATindexOf(ActsFrom,Act)!=ATERM_NON_EXISTING_POSITION)
            {
              mCRL2log(error) << "synchronizing action " << core::pp_deprecated(Act) << " in different ways (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return NULL;
            }
            else
            {
              ActsFrom=ATinsert(ActsFrom,Act);
            }
          }
        }
      }
    }

    //allow
    if (gsIsAllow(ProcTerm))
    {
      ATermList MActList=ATLgetArgument(ProcTerm,0);

      if (ATisEmpty(MActList))
      {
        mCRL2log(warning) << "allowing empty set of (multi) actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }
      else
      {
        ATermList MActs=ATmakeList0();

        for (; !ATisEmpty(MActList); MActList=ATgetNext(MActList))
        {
          ATermList MAct=ATLgetArgument(ATAgetFirst(MActList),0);

          //Actions must be declared
          for (; !ATisEmpty(MAct); MAct=ATgetNext(MAct))
          {
            ATermAppl Act=ATAgetFirst(MAct);
            if (!&*ATLtableGet(context.actions,Act))
            {
              mCRL2log(error) << "allowing an undefined action " << core::pp_deprecated(Act) << " in (multi)action " << core::pp_deprecated(MAct) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return NULL;
            }
          }

          MAct=ATLgetArgument(ATAgetFirst(MActList),0);
          if (gstcMActIn(MAct,MActs))
          {
            mCRL2log(warning) << "allowing (multi)action " << core::pp_deprecated(MAct) << " twice (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          }
          else
          {
            MActs=ATinsert(MActs,MAct);
          }
        }
      }
    }

    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if (!&*NewProc)
    {
      return NULL;
    }
    return ATsetArgument(ProcTerm,NewProc,1);
  }

  if (gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
      gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm))
  {
    ATermAppl NewLeft=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,0));

    if (!&*NewLeft)
    {
      return NULL;
    }
    ATermAppl NewRight=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if (!&*NewRight)
    {
      return NULL;
    }
    ATermAppl a=ATsetArgument(ATsetArgument(ProcTerm,NewLeft,0),NewRight,1);
    return a;
  }

  if (gsIsAtTime(ProcTerm))
  {
    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,0));
    if (!&*NewProc)
    {
      return NULL;
    }
    ATermAppl Time=ATAgetArgument(ProcTerm,1);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (!&*NewType)
    {
      return NULL;
    }

    if (!&*gstcTypeMatchA(sort_real::real_(),NewType))
    {
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,&Time);
      if (!&*CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast time value " << core::pp_deprecated(Time) << " to type Real" << std::endl;
        return NULL;
      }
    }

    return gsMakeAtTime(NewProc,Time);
  }

  if (gsIsIfThen(ProcTerm))
  {
    ATermAppl Cond=ATAgetArgument(ProcTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,sort_bool::bool_());
    if (!&*NewType)
    {
      return NULL;
    }
    ATermAppl NewThen=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if (!&*NewThen)
    {
      return NULL;
    }
    return gsMakeIfThen(Cond,NewThen);
  }

  if (gsIsIfThenElse(ProcTerm))
  {
    ATermAppl Cond=ATAgetArgument(ProcTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,sort_bool::bool_());
    if (!&*NewType)
    {
      return NULL;
    }
    ATermAppl NewThen=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if (!&*NewThen)
    {
      return NULL;
    }
    ATermAppl NewElse=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,2));
    if (!&*NewElse)
    {
      return NULL;
    }
    return gsMakeIfThenElse(Cond,NewThen,NewElse);
  }

  if (gsIsSum(ProcTerm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,ATLgetArgument(ProcTerm,0),NewVars))
    {
      ATtableDestroy(CopyVars);
      mCRL2log(error) << "type error while typechecking " << core::pp_deprecated(ProcTerm) << std::endl;
      return NULL;
    }
    ATermAppl NewProc=gstcTraverseActProcVarConstP(NewVars,ATAgetArgument(ProcTerm,1));
    ATtableDestroy(CopyVars);
    if (!&*NewProc)
    {
      mCRL2log(error) << "while typechecking " << core::pp_deprecated(ProcTerm) << std::endl;
      return NULL;
    }
    return ATsetArgument(ProcTerm,NewProc,1);
  }

  assert(0);
  return Result;
}

static ATermAppl gstcTraversePBESVarConstPB(const ATermTable &Vars, ATermAppl PBESTerm)
{
  ATermAppl Result=NULL;

  if (gsIsDataExpr(PBESTerm))
  {
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&PBESTerm,sort_bool::bool_());
    if (!&*NewType)
    {
      return NULL;
    }
    return PBESTerm;
  }

  if (gsIsPBESTrue(PBESTerm) || gsIsPBESFalse(PBESTerm))
  {
    return PBESTerm;
  }

  if (gsIsPBESNot(PBESTerm))
  {
    ATermAppl NewArg=gstcTraversePBESVarConstPB(Vars,ATAgetArgument(PBESTerm,0));
    if (!&*NewArg)
    {
      return NULL;
    }
    return ATsetArgument(PBESTerm,NewArg,0);
  }

  if (gsIsPBESAnd(PBESTerm) || gsIsPBESOr(PBESTerm) || gsIsPBESImp(PBESTerm))
  {
    ATermAppl NewLeft=gstcTraversePBESVarConstPB(Vars,ATAgetArgument(PBESTerm,0));
    if (!&*NewLeft)
    {
      return NULL;
    }
    ATermAppl NewRight=gstcTraversePBESVarConstPB(Vars,ATAgetArgument(PBESTerm,1));
    if (!&*NewRight)
    {
      return NULL;
    }
    return ATsetArgument(ATsetArgument(PBESTerm,NewLeft,0),NewRight,1);
  }

  if (gsIsPBESForall(PBESTerm)||gsIsPBESExists(PBESTerm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,ATLgetArgument(PBESTerm,0),NewVars))
    {
      ATtableDestroy(CopyVars);
      mCRL2log(error) << "type error while typechecking " << core::pp_deprecated(PBESTerm) << std::endl;
      return NULL;
    }
    ATermAppl NewPBES=gstcTraversePBESVarConstPB(NewVars,ATAgetArgument(PBESTerm,1));
    ATtableDestroy(CopyVars);
    if (!&*NewPBES)
    {
      mCRL2log(error) << "while typechecking " << core::pp_deprecated(PBESTerm) << std::endl;
      return NULL;
    }
    return ATsetArgument(PBESTerm,NewPBES,1);
  }

  if (gsIsPropVarInst(PBESTerm))
  {
    return gstcRewrActProc(Vars, PBESTerm, true);
  }

  assert(0);
  return Result;
}

static ATermAppl gstcTraverseVarConsTypeD(
  const ATermTable &DeclaredVars,
  const ATermTable &AllowedVars,
  ATermAppl* DataTerm,
  ATermAppl PosType,
  ATermTable &FreeVars,
  const bool strict_ambiguous,
  const bool warn_upcasting,
  const bool print_cast_error)
{
  //Type checks and transforms *DataTerm replacing Unknown datatype with other ones.
  //Returns the type of the term
  //which should match the PosType
  //all the variables should be in AllowedVars
  //if a variable is in DeclaredVars and not in AllowedVars,
  //a different error message is generated.
  //all free variables (if any) are added to FreeVars

  ATermAppl Result=NULL;

  mCRL2log(debug) << "gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(*DataTerm) <<
              " with PosType " << core::pp_deprecated(PosType) << "" << std::endl;

  if (is_abstraction(*DataTerm))
  {
    //The variable declaration of a binder should have at least 1 declaration
    if (ATAgetFirst(ATLgetArgument(*DataTerm, 1)) == ATerm())
    {
      mCRL2log(error) << "binder " << core::pp_deprecated(*DataTerm) << " should have at least one declared variable" << std::endl;
      return NULL;
    }

    ATermAppl BindingOperator = ATAgetArgument(*DataTerm, 0);
    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    if (gsIsSetBagComp(BindingOperator) ||
        gsIsSetComp(BindingOperator) ||
        gsIsBagComp(BindingOperator))
    {
      ATermList VarDecls=ATLgetArgument(*DataTerm,1);
      ATermAppl VarDecl=ATAgetFirst(VarDecls);

      //A Set/bag comprehension should have exactly one variable declared
      VarDecls=ATgetNext(VarDecls);
      if (VarDecls != ATempty)
      {
        mCRL2log(error) << "set/bag comprehension " << core::pp_deprecated(*DataTerm) << " should have exactly one declared variable" << std::endl;
        return NULL;
      }

      ATermAppl NewType=ATAgetArgument(VarDecl,1);
      ATermList VarList=ATmakeList1(VarDecl);
      ATermTable NewAllowedVars;
      if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }
      ATermTable NewDeclaredVars;
      if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }
      ATermAppl Data=ATAgetArgument(*DataTerm,2);

      ATermAppl ResType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,data::unknown_sort(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);

      if (!&*ResType)
      {
        mCRL2log(error) << "the condition or count of a set/bag comprehension " << core::pp_deprecated(*DataTerm) << " cannot be determined\n";
        return NULL;
      }
      if (&*gstcTypeMatchA(sort_bool::bool_(),ResType))
      {
        NewType=sort_set::set_(sort_expression(NewType));
        *DataTerm = ATsetArgument(*DataTerm, gsMakeSetComp(), 0);
      }
      else if (&*gstcTypeMatchA(sort_nat::nat(),ResType))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        *DataTerm = ATsetArgument(*DataTerm, gsMakeBagComp(), 0);
      }
      else if (&*gstcTypeMatchA(sort_pos::pos(),ResType))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        *DataTerm = ATsetArgument(*DataTerm,gsMakeBagComp(), 0);
        Data=gsMakeDataAppl(sort_nat::cnat(),ATmakeList1(Data));
      }
      else
      {
        mCRL2log(error) << "the condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " << core::pp_deprecated(ResType) << std::endl;
        return NULL;
      }

      if (!&*(NewType=gstcTypeMatchA(NewType,PosType)))
      {
        mCRL2log(error) << "a set or bag comprehension of type " << core::pp_deprecated(ATAgetArgument(VarDecl, 1)) << " does not match possible type " <<
                            core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
        return NULL;
      }

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      *DataTerm=ATsetArgument(*DataTerm,Data,2);
      return NewType;
    }

    if (gsIsForall(BindingOperator) || gsIsExists(BindingOperator))
    {
      ATermList VarList=ATLgetArgument(*DataTerm,1);
      ATermTable NewAllowedVars;
      if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }
      ATermTable NewDeclaredVars;
      if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }

      ATermAppl Data=ATAgetArgument(*DataTerm,2);
      if (!&*gstcTypeMatchA(sort_bool::bool_(),PosType))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }
      ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,sort_bool::bool_(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);

      if (!&*NewType)
      {
        return NULL;
      }
      if (!&*gstcTypeMatchA(sort_bool::bool_(),NewType))
      {
        return NULL;
      }

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      *DataTerm=ATsetArgument(*DataTerm,Data,2);
      return sort_bool::bool_();
    }

    if (gsIsLambda(BindingOperator))
    {
      ATermList VarList=ATLgetArgument(*DataTerm,1);
      ATermTable NewAllowedVars;
      if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }
      ATermTable NewDeclaredVars;
      if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        return NULL;
      }

      ATermList ArgTypes=gstcGetVarTypes(VarList);
      ATermAppl NewType=gstcUnArrowProd(ArgTypes,PosType);
      if (!&*NewType)
      {
        ATtableDestroy(CopyAllowedVars);
        ATtableDestroy(CopyDeclaredVars);
        mCRL2log(error) << "no functions with arguments " << core::pp_deprecated(ArgTypes) << " among " << core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
        return NULL;
      }
      ATermAppl Data=ATAgetArgument(*DataTerm,2);

      NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,NewType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);

      mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(Data) << "" << std::endl;

      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      if (!&*NewType)
      {
        return NULL;
      }
      *DataTerm=ATsetArgument(*DataTerm,Data,2);
      return gsMakeSortArrow(ArgTypes,NewType);
    }
  }

  if (is_where_clause(*DataTerm))
  {
    ATermList WhereVarList=ATmakeList0();
    ATermList NewWhereList=ATmakeList0();
    for (ATermList WhereList=ATLgetArgument(*DataTerm,1); !ATisEmpty(WhereList); WhereList=ATgetNext(WhereList))
    {
      ATermAppl WhereElem=ATAgetFirst(WhereList);
      ATermAppl WhereTerm=ATAgetArgument(WhereElem,1);
      ATermAppl WhereType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&WhereTerm,data::unknown_sort(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
      if (!&*WhereType)
      {
        return NULL;
      }

      ATermAppl NewWhereVar=NULL;
      if (gsIsDataVarId(ATAgetArgument(WhereElem,0)))
      {
        // The variable in WhereElem is type checked, and a proper variable.
        NewWhereVar=ATAgetArgument(WhereElem,0);
      }
      else
      {
        // The variable in WhereElem is just a string and needs to be transformed to a DataVarId.
        NewWhereVar=gsMakeDataVarId(ATAgetArgument(WhereElem,0),WhereType);
      }
      WhereVarList=ATinsert(WhereVarList,NewWhereVar);
      NewWhereList=ATinsert(NewWhereList,gsMakeDataVarIdInit(NewWhereVar,WhereTerm));
    }
    NewWhereList=ATreverse(NewWhereList);

    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    ATermList VarList=ATreverse(WhereVarList);
    ATermTable NewAllowedVars;
    if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
    {
      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);
      return NULL;
    }
    ATermTable NewDeclaredVars;
    if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
    {
      ATtableDestroy(CopyAllowedVars);
      ATtableDestroy(CopyDeclaredVars);
      return NULL;
    }

    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,PosType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
    ATtableDestroy(CopyAllowedVars);
    ATtableDestroy(CopyDeclaredVars);

    if (!&*NewType)
    {
      return NULL;
    }
    // if (FreeVars)
    {
      gstcRemoveVars(FreeVars,VarList);
    }
    *DataTerm=gsMakeWhr(Data,NewWhereList);
    return NewType;
  }

  if (gsIsDataAppl(*DataTerm))
  {
    //arguments
    ATermList Arguments=ATLgetArgument(*DataTerm,1);
    size_t nArguments=ATgetLength(Arguments);

    //The following is needed to check enumerations
    ATermAppl Arg0 = ATAgetArgument(*DataTerm,0);
    if (gsIsOpId(Arg0) || gsIsId(Arg0))
    {
      ATermAppl Name = ATAgetArgument(Arg0,0);
      if (Name == sort_list::list_enumeration_name())
      {
        ATermAppl Type=gstcUnList(PosType);
        if (!&*Type)
        {
          mCRL2log(error) << "not possible to cast list to " << core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(Arguments) << ")" << std::endl;
          return NULL;
        }

        ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermList NewArguments=ATmakeList0();
        bool Type_is_stable=true;
        for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
        {
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,false);
          if (!&*Type0)
          {
            // Try again, but now without Type as the suggestion. 
            // If this does not work, it will be caught in the second pass below.
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,data::unknown_sort(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
            if (!&*Type0)
            { 
              return NULL;
            }
          }
          NewArguments=ATinsert(NewArguments,Argument);
          Type_is_stable=Type_is_stable && (Type==Type0);
          Type=Type0;
        }
        Arguments=OldArguments;

        //Second time to do the real work, but only if the elements in the list have different types.
        if (!Type_is_stable)
        {
          NewArguments=ATmakeList0();
          for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
          {
            ATermAppl Argument=ATAgetFirst(Arguments);
            ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
            if (!&*Type0)
            {
              return NULL;
            }
            NewArguments=ATinsert(NewArguments,Argument);
            Type=Type0;
          }
        }
        Arguments=ATreverse(NewArguments);

        Type=sort_list::list(sort_expression(Type));
        *DataTerm=sort_list::list_enumeration(sort_expression(Type), data_expression_list(Arguments));
        return Type;
      }
      if (Name == sort_set::set_enumeration_name())
      {
        ATermAppl Type=gstcUnSet(PosType);
        if (!&*Type)
        {
          mCRL2log(error) << "not possible to cast set to " << core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(Arguments) << ")" << std::endl;
          return NULL;
        }

        ATermList OldArguments=Arguments;

        //First time to determine the common type only (which will be NewType)!
        ATermAppl NewType=NULL;
        for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
        {
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (!&*Type0)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument) << ")" << std::endl;
            return NULL;
          }

          ATermAppl OldNewType=NewType;
          if (NewType==ATermAppl())
          {
            NewType=Type0;
          }
          else
          {
            NewType=gstcMaximumType(NewType,Type0);
          }

          if (NewType==ATermAppl())
          {
            mCRL2log(error) << "Set contains incompatible elements of sorts " << core::pp_deprecated(OldNewType) << " and " << core::pp_deprecated(Type0) << " (while typechecking " << core::pp_deprecated(Argument) << std::endl;
            return NULL;
          }
        }

        // Type is now the most generic type to be used.
        assert(Type!=ATermAppl());
        Type=NewType;
        Arguments=OldArguments;

        //Second time to do the real work.
        ATermList NewArguments=ATmakeList0();
        for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
        {
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (!&*Type0)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument) << ")" << std::endl;
            return NULL;
          }
          NewArguments=ATinsert(NewArguments,Argument);
          Type=Type0;
        }
        Arguments=ATreverse(NewArguments);
        Type=sort_set::set_(sort_expression(Type));
        *DataTerm=sort_set::set_enumeration(sort_expression(Type),data_expression_list(Arguments));
        return Type;
      }
      if (Name == sort_bag::bag_enumeration_name())
      {
        ATermAppl Type=gstcUnBag(PosType);
        if (!&*Type)
        {
          mCRL2log(error) << "not possible to cast bag to " << core::pp_deprecated(PosType) << "(while typechecking " << core::pp_deprecated(Arguments) << ")" << std::endl;
          return NULL;
        }

        ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermAppl NewType=NULL;
        for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
        {
          ATermAppl Argument0=ATAgetFirst(Arguments);
          Arguments=ATgetNext(Arguments);
          ATermAppl Argument1=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument0,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (!&*Type0)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument0) << ")" << std::endl;
            return NULL;
          }
          ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument1,sort_nat::nat(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if ((!&*Type1) && print_cast_error)
          {
            mCRL2log(error) << "not possible to cast number to " << core::pp_deprecated(sort_nat::nat()) << " (while typechecking " << core::pp_deprecated(Argument1) << ")" << std::endl;
            return NULL;
          }
          ATermAppl OldNewType=NewType;
          if (NewType==ATermAppl())
          {
            NewType=Type0;
          }
          else
          {
            NewType=gstcMaximumType(NewType,Type0);
          }
          if (NewType==ATermAppl())
          {
            mCRL2log(error) << "Bag contains incompatible elements of sorts " << core::pp_deprecated(OldNewType) << " and " << core::pp_deprecated(Type0) << " (while typechecking " << core::pp_deprecated(Argument0) << ")" << std::endl;
            return NULL;
          }
        }
        assert(Type!=ATermAppl());
        Type=NewType;
        Arguments=OldArguments;

        //Second time to do the real work.
        ATermList NewArguments=ATmakeList0();
        for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
        {
          ATermAppl Argument0=ATAgetFirst(Arguments);
          Arguments=ATgetNext(Arguments);
          ATermAppl Argument1=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument0,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if ((!&*Type0) && print_cast_error)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument0) << ")" << std::endl;
            return NULL;
          }
          ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument1,sort_nat::nat(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if ((!&*Type1) && print_cast_error)
          {
            mCRL2log(error) << "not possible to cast number to " << core::pp_deprecated(sort_nat::nat()) << " (while typechecking " << core::pp_deprecated(Argument1) << ")" << std::endl;
            return NULL;
          }
          NewArguments=ATinsert(NewArguments,Argument0);
          NewArguments=ATinsert(NewArguments,Argument1);
          Type=Type0;
        }
        Arguments=ATreverse(NewArguments);
        Type=sort_bag::bag(sort_expression(Type));
        *DataTerm=sort_bag::bag_enumeration(sort_expression(Type), data_expression_list(Arguments));
        return Type;
      }
    }
    ATermList NewArgumentTypes=ATmakeList0();
    ATermList NewArguments=ATmakeList0();

    for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments))
    {
      ATermAppl Arg=ATAgetFirst(Arguments);
      ATermAppl Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,data::unknown_sort(),FreeVars,false,warn_upcasting,print_cast_error);
      if (!&*Type)
      {
        return NULL;
      }
      NewArguments=ATinsert(NewArguments,Arg);
      NewArgumentTypes=ATinsert(NewArgumentTypes,Type);
    }
    Arguments=ATreverse(NewArguments);
    ATermList ArgumentTypes=ATreverse(NewArgumentTypes);

    //function
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                      &Data,
                      data::unknown_sort() /* gsMakeSortArrow(ArgumentTypes,PosType) */,
                      FreeVars,false,nArguments,warn_upcasting,print_cast_error);
    mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(Data) << "" << std::endl;

    if (!&*NewType)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else if (gsIsOpId(Data)||gsIsDataVarId(Data))
      {
        gstcErrorMsgCannotCast(ATAgetArgument(Data,1),Arguments,ArgumentTypes);
      }
      mCRL2log(error) << "type error while trying to cast " << core::pp_deprecated(gsMakeDataAppl(Data,Arguments)) << " to type " << core::pp_deprecated(PosType) << std::endl;
      return NULL;
    }

    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.


    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      ATermList NeededArgumentTypes=ATLgetArgument(gstcUnwindType(NewType),0);

      if (ATgetLength(NeededArgumentTypes)!=ATgetLength(Arguments))
      {
         mCRL2log(error) << "need argumens of sorts " << core::pp_deprecated(NeededArgumentTypes) << 
                         " which does not match the number of provided arguments "
                            << core::pp_deprecated(Arguments) << " (while typechecking " 
                            << core::pp_deprecated(*DataTerm) << ")" << std::endl;
         return NULL;

      }
      //arguments again
      ATermList NewArgumentTypes=ATmakeList0();
      ATermList NewArguments=ATmakeList0();
      for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments),
           ArgumentTypes=ATgetNext(ArgumentTypes),NeededArgumentTypes=ATgetNext(NeededArgumentTypes))
      {
        assert(!ATisEmpty(Arguments));
        assert(!ATisEmpty(NeededArgumentTypes));
        ATermAppl Arg=ATAgetFirst(Arguments);
        ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
        ATermAppl Type=ATAgetFirst(ArgumentTypes);
        if (!gstcEqTypesA(NeededType,Type))
        {
          //upcasting
          ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg,warn_upcasting);
          if (&*CastedNewType)
          {
            Type=CastedNewType;
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on (1) " << core::pp_deprecated(Arg) << ", Type: " << core::pp_deprecated(Type) << ", Needed type: " << core::pp_deprecated(NeededType) << "" << std::endl;
          ATermAppl NewArgType=gstcTypeMatchA(NeededType,Type);
          if (!&*NewArgType)
          {
            NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          }
          if (!&*NewArgType)
          {
            NewArgType=NeededType;
          }
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,NewArgType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          mCRL2log(debug) << "Result of Doing again gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(Arg) << "" << std::endl;
          if (!&*NewArgType)
          {
            mCRL2log(error) << "needed type " << core::pp_deprecated(NeededType) << " does not match possible type "
                            << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Arg) << " in " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
            return NULL;
          }
          Type=NewArgType;
        }
        NewArguments=ATinsert(NewArguments,Arg);
        NewArgumentTypes=ATinsert(NewArgumentTypes,Type);
      }
      Arguments=ATreverse(NewArguments);
      ArgumentTypes=ATreverse(NewArgumentTypes);
    }

    //the function again
    NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                      &Data,gsMakeSortArrow(ArgumentTypes,PosType),FreeVars,strict_ambiguous,nArguments,warn_upcasting,print_cast_error);

    mCRL2log(debug) << "Result of gstcTraverseVarConsTypeDN: DataTerm " << core::pp_deprecated(Data) << "" << std::endl;

    if (!&*NewType)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else
      {
        gstcErrorMsgCannotCast(ATAgetArgument(Data,1),Arguments,ArgumentTypes);
      }
      mCRL2log(error) << "type error while trying to cast " << core::pp_deprecated(gsMakeDataAppl(Data,Arguments)) << " to type " << core::pp_deprecated(PosType) << std::endl;
      return NULL;
    }

    mCRL2log(debug) << "Arguments once more: Arguments " << core::pp_deprecated(Arguments) << ", ArgumentTypes: " <<
                core::pp_deprecated(ArgumentTypes) << ", NewType: " << core::pp_deprecated(NewType) << "" << std::endl;

    //and the arguments once more
    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      ATermList NeededArgumentTypes=ATLgetArgument(gstcUnwindType(NewType),0);
      ATermList NewArgumentTypes=ATmakeList0();
      ATermList NewArguments=ATmakeList0();
      for (; !ATisEmpty(Arguments); Arguments=ATgetNext(Arguments),
           ArgumentTypes=ATgetNext(ArgumentTypes),NeededArgumentTypes=ATgetNext(NeededArgumentTypes))
      {
        ATermAppl Arg=ATAgetFirst(Arguments);
        ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
        ATermAppl Type=ATAgetFirst(ArgumentTypes);

        if (!gstcEqTypesA(NeededType,Type))
        {
          //upcasting
          ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg,warn_upcasting);
          if (&*CastedNewType)
          {
            Type=CastedNewType;
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on (2) " << core::pp_deprecated(Arg) << ", Type: " << core::pp_deprecated(Type) << ", Needed type: " << core::pp_deprecated(NeededType) << "" << std::endl;
          ATermAppl NewArgType=gstcTypeMatchA(NeededType,Type);
          if (!&*NewArgType)
          {
            NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          }
          if (!&*NewArgType)
          {
            NewArgType=NeededType;
          }
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,NewArgType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (!&*NewArgType)
          {
            mCRL2log(error) << "needed type " << core::pp_deprecated(NeededType) << " does not match possible type "
                            << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Arg) << " in " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
            return NULL;
          }
          Type=NewArgType;
        }

        NewArguments=ATinsert(NewArguments,Arg);
        NewArgumentTypes=ATinsert(NewArgumentTypes,Type);
      }
      Arguments=ATreverse(NewArguments);
      ArgumentTypes=ATreverse(NewArgumentTypes);
    }

    mCRL2log(debug) << "Arguments after once more: Arguments " << core::pp_deprecated(Arguments) << ", ArgumentTypes: " << core::pp_deprecated(ArgumentTypes) << "" << std::endl;

    *DataTerm=gsMakeDataAppl(Data,Arguments);

    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      return ATAgetArgument(gstcUnwindType(NewType),1);
    }

    if (gstcHasUnknown(gstcUnArrowProd(ArgumentTypes,NewType)))
    {
      mCRL2log(error) << "Fail to properly type " << core::pp_deprecated(*DataTerm) << std::endl;
      return NULL;
    }
    return gstcUnArrowProd(ArgumentTypes,NewType);
  }

  if (gsIsId(*DataTerm)||gsIsOpId(*DataTerm)||gsIsDataVarId(*DataTerm))
  {
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    if (gsIsNumericString(gsATermAppl2String(Name)))
    {
      ATermAppl Sort=sort_int::int_();
      if (gstcIsPos(Name))
      {
        Sort=sort_pos::pos();
      }
      else if (gstcIsNat(Name))
      {
        Sort=sort_nat::nat();
      }
      *DataTerm=gsMakeOpId(Name,Sort);

      if (&*gstcTypeMatchA(Sort,PosType))
      {
        return Sort;
      }

      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Sort,DataTerm,warn_upcasting);
      if (!&*CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast number " << core::pp_deprecated(*DataTerm) << " to type " << core::pp_deprecated(PosType) << std::endl;
        return NULL;
      }
      return CastedNewType;
    }

    ATermAppl Type=ATAtableGet(DeclaredVars,Name);
    if (&*Type)
    {
      mCRL2log(debug) << "Recognised declared variable " << core::pp_deprecated(Name) << ", Type: " << core::pp_deprecated(Type) << "" << std::endl;
      *DataTerm=gsMakeDataVarId(Name,Type);

      if (!&*ATAtableGet(AllowedVars,Name))
      {
        mCRL2log(error) << "variable " << core::pp_deprecated(Name) << " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side" << std::endl;
        return NULL;
      }

      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if (&*NewType)
      {
        Type=NewType;
      }
      else
      {
        //upcasting
        ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        if ((!&*CastedNewType) && print_cast_error)
        {
          mCRL2log(error) << "cannot (up)cast variable " << core::pp_deprecated(*DataTerm) << " to type " << core::pp_deprecated(PosType) << std::endl;
          return NULL;
        }

        Type=CastedNewType;
      }

      //Add to free variables list
      // if (FreeVars)
      {
        ATtablePut(FreeVars, Name, Type);
      }

      return Type;
    }

    if (&*(Type=ATAtableGet(context.constants,Name)))
    {
      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if (&*NewType)
      {
        Type=NewType;
        *DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        // The type cannot be unified. Try upcasting the type.
        *DataTerm=gsMakeOpId(Name,Type);
        ATermAppl NewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        if (NewType==ATermAppl())
        {
          mCRL2log(error) << "no constant " << core::pp_deprecated(*DataTerm) << " with type " << core::pp_deprecated(PosType) << std::endl;
          return NULL;
        }
        else
        {
          return NewType;
        }
      }
    }

    ATermList ParList=ATLtableGet(gssystem.constants,Name);
    if (&*ParList)
    {
      ATermList NewParList=ATmakeList0();
      for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
      {
        ATermAppl Par=ATAgetFirst(ParList);
        if (&*(Par=gstcTypeMatchA(Par,PosType)))
        {
          NewParList=ATinsert(NewParList,Par);
        }
      }
      ParList=ATreverse(NewParList);
      if (ATisEmpty(ParList))
      {
        mCRL2log(error) << "no system constant " << core::pp_deprecated(*DataTerm) << " with type " << core::pp_deprecated(PosType) << std::endl;
        return NULL;
      }

      if (ATgetLength(ParList)==1)
      {
        Type=ATAgetFirst(ParList);
        *DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        *DataTerm=gsMakeOpId(Name,data::unknown_sort());
        return data::unknown_sort();
      }
    }

    ATermList ParListS=ATLtableGet(gssystem.functions,Name);
    ParList=ATLtableGet(context.functions,Name);
    if (!&*ParList)
    {
      ParList=ParListS;
    }
    else if (&*ParListS)
    {
      ParList=ATconcat(ParListS,ParList);
    }

    if (!&*ParList)
    {
      mCRL2log(error) << "unknown operation " << core::pp_deprecated(Name) << std::endl;
      return NULL;
    }

    if (ATgetLength(ParList)==1)
    {
      ATermAppl Type=ATAgetFirst(ParList);
      *DataTerm=gsMakeOpId(Name,Type);
      ATermAppl NewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
      if (NewType==ATermAppl())
      {
        mCRL2log(error) << "no constant " << core::pp_deprecated(*DataTerm) << " with type " << core::pp_deprecated(PosType) << std::endl;
        return NULL;
      }
      return NewType;
    }
    else
    {
      return gstcTraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strict_ambiguous, ATERM_NON_EXISTING_POSITION, warn_upcasting,print_cast_error);
    }
  }

  // if(gsIsDataVarId(*DataTerm)){
  //   return ATAgetArgument(*DataTerm,1);
  // }

  return Result;
}

static ATermAppl gstcTraverseVarConsTypeDN(
  const ATermTable &DeclaredVars,
  const ATermTable &AllowedVars,
  ATermAppl* DataTerm,
  ATermAppl PosType,
  ATermTable &FreeVars,
  const bool strict_ambiguous,
  const size_t nFactPars,
  const bool warn_upcasting,
  const bool print_cast_error)
{
  // ATERM_NON_EXISTING_POSITION for nFactPars means the number of arguments is not known.
  mCRL2log(debug) << "gstcTraverseVarConsTypeDN: DataTerm " << core::pp_deprecated(*DataTerm)
                  << " with PosType " << core::pp_deprecated(PosType) << ", nFactPars " << nFactPars << "" << std::endl;
  if (gsIsId(*DataTerm)||gsIsOpId(*DataTerm))
  {
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    bool variable=false;
    ATermAppl Type=ATAtableGet(DeclaredVars,Name);
    if (&*Type)
    {
      const sort_expression Type1(gstcUnwindType(Type));
      if (is_function_sort(Type1)?(function_sort(Type1).domain().size()==nFactPars):(nFactPars==0))
      {
        variable=true;
        if (!&*ATAtableGet(AllowedVars,Name))
        {
          mCRL2log(error) << "variable " << core::pp_deprecated(Name) << " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side" << std::endl;
          return NULL;
        }

        //Add to free variables list
        // if (FreeVars)
        {
          ATtablePut(FreeVars, Name, Type);
        }
      }
      else
      {
        Type=NULL;
      }
    }
    ATermList ParList;

    if (nFactPars==0)
    {
      if (&*(Type=ATAtableGet(DeclaredVars,Name)))
      {
        if (!&*gstcTypeMatchA(Type,PosType))
        {
          mCRL2log(error) << "the type " << core::pp_deprecated(Type) << " of variable " << core::pp_deprecated(Name)
                          << " is incompatible with " << core::pp_deprecated(PosType) << " (typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        *DataTerm=gsMakeDataVarId(Name,Type);
        return Type;
      }
      else if (&*(Type=ATAtableGet(context.constants,Name)))
      {
        if (!&*gstcTypeMatchA(Type,PosType))
        {
          mCRL2log(error) << "the type " << core::pp_deprecated(Type) << " of constant " << core::pp_deprecated(Name)
                          << " is incompatible with " << core::pp_deprecated(PosType) << " (typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        *DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        if (&*(ParList=ATLtableGet(gssystem.constants,Name)))
        {
          if (ATgetLength(ParList)==1)
          {
            ATermAppl Type=ATAgetFirst(ParList);
            *DataTerm=gsMakeOpId(Name,Type);
            return Type;
          }
          else
          {
            mCRL2log(warning) << "ambiguous system constant " << core::pp_deprecated(Name) << std::endl;
            *DataTerm=gsMakeOpId(Name,data::unknown_sort());
            return Type;
          }
        }
        else
        {
          mCRL2log(error) << "unknown constant " << core::pp_deprecated(Name) << std::endl;
          return NULL;
        }
      }
    }

    if (&*Type)
    {
      ParList=ATmakeList1(gstcUnwindType(Type));
    }
    else
    {
      ATermList ParListS=ATLtableGet(gssystem.functions,Name);
      ParList=ATLtableGet(context.functions,Name);
      if (!&*ParList)
      {
        ParList=ParListS;
      }
      else if (&*ParListS)
      {
        ParList=ATconcat(ParListS,ParList);
      }
    }

    if (!&*ParList)
    {
      if (nFactPars!=ATERM_NON_EXISTING_POSITION)
      {
        mCRL2log(error) << "unknown operation " << core::pp_deprecated(Name) << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"") << std::endl;
      }
      else
      {
        mCRL2log(error) << "unknown operation " << core::pp_deprecated(Name) << std::endl;;
      }
      return NULL;
    }
    mCRL2log(debug) << "Possible types for Op/Var " << core::pp_deprecated(Name) << " with " << nFactPars <<
                " argument are (ParList: " << core::pp_deprecated(ParList) << "; PosType: " << core::pp_deprecated(PosType) << ")" << std::endl;

    ATermList CandidateParList=ParList;

    {
      // filter ParList keeping only functions A_0#...#A_nFactPars->A
      ATermList NewParList;
      if (nFactPars!=ATERM_NON_EXISTING_POSITION)
      {
        NewParList=ATmakeList0();
        for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
        {
          ATermAppl Par=ATAgetFirst(ParList);
          if (!gsIsSortArrow(Par))
          {
            continue;
          }
          if ((ATgetLength(ATLgetArgument(Par,0))!=nFactPars))
          {
            continue;
          }
          NewParList=ATinsert(NewParList,Par);
        }
        ParList=ATreverse(NewParList);
      }

      if (!ATisEmpty(ParList))
      {
        CandidateParList=ParList;
      }

      // filter ParList keeping only functions of the right type
      ATermList BackupParList=ParList;
      NewParList=ATmakeList0();
      for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
      {
        ATermAppl Par=ATAgetFirst(ParList);
        if (&*(Par=gstcTypeMatchA(Par,PosType)))
        {
          NewParList=ATinsertUnique(NewParList,Par);
        }
      }
      NewParList=ATreverse(NewParList);

      mCRL2log(debug) << "Possible matches w/o casting for Op/Var " << core::pp_deprecated(Name) << " with "<< nFactPars <<               " argument are (ParList: " << core::pp_deprecated(NewParList) << "; PosType: " << core::pp_deprecated(PosType) << "" << std::endl;

      if (ATisEmpty(NewParList))
      {
        //Ok, this looks like a type error. We are not that strict.
        //Pos can be Nat, or even Int...
        //So lets make PosType more liberal
        //We change every Pos to NotInferred(Pos,Nat,Int)...
        //and get the list. Then we take the min of the list.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying casting for Op " << core::pp_deprecated(Name) << " with " << nFactPars << " argument (ParList: " <<                             core::pp_deprecated(ParList) << "; PosType: " << core::pp_deprecated(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesUp(PosType);
        for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
        {
          ATermAppl Par=ATAgetFirst(ParList);
          if (&*(Par=gstcTypeMatchA(Par,PosType)))
          {
            NewParList=ATinsertUnique(NewParList,Par);
          }
        }
        NewParList=ATreverse(NewParList);
        mCRL2log(debug) << "The result of casting is (1) " << core::pp_deprecated(NewParList) << "" << std::endl;
        if (ATgetLength(NewParList)>1)
        {
          NewParList=ATmakeList1(gstcMinType(NewParList));
        }
      }

      if (ATisEmpty(NewParList))
      {
        //Ok, casting of the arguments did not help.
        //Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying result casting for Op " << core::pp_deprecated(Name) << " with " << nFactPars << " argument (ParList: "                       << "; PosType: " << core::pp_deprecated(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesDown(gstcExpandNumTypesUp(PosType));
        for (; !ATisEmpty(ParList); ParList=ATgetNext(ParList))
        {
          ATermAppl Par=ATAgetFirst(ParList);
          if (&*(Par=gstcTypeMatchA(Par,PosType)))
          {
            NewParList=ATinsertUnique(NewParList,Par);
          }
        }
        NewParList=ATreverse(NewParList);
        mCRL2log(debug) << "The result of casting is (2)" << core::pp_deprecated(NewParList) << "" << std::endl;
        if (ATgetLength(NewParList)>1)
        {
          NewParList=ATmakeList1(gstcMinType(NewParList));
        }
      }

      ParList=NewParList;
    }
    if (ATisEmpty(ParList))
    {
      //provide some information to the upper layer for a better error message
      ATermAppl Sort;
      if (ATgetLength(CandidateParList)==1)
      {
        Sort=ATAgetFirst(CandidateParList);
      }
      else
      {
        // Sort=multiple_possible_sorts(atermpp::aterm_list(CandidateParList));
        Sort=multiple_possible_sorts(sort_expression_list(CandidateParList));
      }
      *DataTerm=gsMakeOpId(Name,Sort);
      if (nFactPars!=ATERM_NON_EXISTING_POSITION)
      {
        mCRL2log(error) << "unknown operation/variable " << core::pp_deprecated(Name)
                        << " with " << nFactPars << " argument" << ((nFactPars != 1)?"s":"")
                        << " that matches type " << core::pp_deprecated(PosType) << std::endl;
      }
      else
      {
        mCRL2log(error) << "unknown operation/variable " << core::pp_deprecated(Name) << " that matches type " << core::pp_deprecated(PosType) << std::endl;
      }
      return NULL;
    }

    if (ATgetLength(ParList)==1)
    {
      // replace PossibleSorts by a single possibility.
      ATermAppl Type=ATAgetFirst(ParList);

      ATermAppl OldType=Type;
      if (gstcHasUnknown(Type))
      {
        Type=gstcTypeMatchA(Type,PosType);
      }

      if (gstcHasUnknown(Type) && gsIsOpId(*DataTerm))
      {
        Type=gstcTypeMatchA(Type,ATAgetArgument(*DataTerm,1));
      }
      if (Type==ATermAppl())
      {
        mCRL2log(error) << "fail to match sort " << core::pp_deprecated(OldType) << " with " << core::pp_deprecated(PosType) << std::endl;
        return NULL;
      }

      if (ATisEqual(static_cast<ATermAppl>(data::detail::if_symbol()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing if matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchIf(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function if has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(data::detail::equal_symbol()),ATAgetArgument(*DataTerm,0))
          || ATisEqual(static_cast<ATermAppl>(data::detail::not_equal_symbol()),ATAgetArgument(*DataTerm,0))
          || ATisEqual(static_cast<ATermAppl>(data::detail::less_symbol()),ATAgetArgument(*DataTerm,0))
          || ATisEqual(static_cast<ATermAppl>(data::detail::less_equal_symbol()),ATAgetArgument(*DataTerm,0))
          || ATisEqual(static_cast<ATermAppl>(data::detail::greater_symbol()),ATAgetArgument(*DataTerm,0))
          || ATisEqual(static_cast<ATermAppl>(data::detail::greater_equal_symbol()),ATAgetArgument(*DataTerm,0))
         )
      {
        mCRL2log(debug) << "Doing ==, !=, <, <=, >= or > matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchEqNeqComparison(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function " << core::pp_deprecated(ATAgetArgument(*DataTerm, 0)) << " has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::cons_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing |> matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpCons(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function |> has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::snoc_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing <| matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpSnoc(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function <| has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::concat_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing ++ matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpConcat(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function ++ has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::element_at_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing @ matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << ", DataTerm: " << core::pp_deprecated(*DataTerm) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpEltAt(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function @ has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::head_name()),ATAgetArgument(*DataTerm,0))||
          ATisEqual(static_cast<ATermAppl>(sort_list::rhead_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing {R,L}head matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
// Type==NULL
        ATermAppl NewType=gstcMatchListOpHead(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function {R,L}head has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::tail_name()),ATAgetArgument(*DataTerm,0))||
          ATisEqual(static_cast<ATermAppl>(sort_list::rtail_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing {R,L}tail matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpTail(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function {R,L}tail has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_bag::set2bag_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing Set2Bag matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchSetOpSet2Bag(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function Set2Bag has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_list::in_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing {List,Set,Bag} matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListSetBagOpIn(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function {List,Set,Bag}In has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_set::union_name()),ATAgetArgument(*DataTerm,0))||
          ATisEqual(static_cast<ATermAppl>(sort_set::difference_name()),ATAgetArgument(*DataTerm,0))||
          ATisEqual(static_cast<ATermAppl>(sort_set::intersection_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing {Set,Bag}{Union,Difference,Intersect} matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchSetBagOpUnionDiffIntersect(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_set::complement_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing SetCompl matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchSetOpSetCompl(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function SetCompl has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_bag::bag2set_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing Bag2Set matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchBagOpBag2Set(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function Bag2Set has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }

      if (ATisEqual(static_cast<ATermAppl>(sort_bag::count_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing BagCount matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchBagOpBagCount(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "the function BagCount has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }


      if (ATisEqual(static_cast<ATermAppl>(data::function_update_name()),ATAgetArgument(*DataTerm,0)))
      {
        mCRL2log(debug) << "Doing FuncUpdate matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchFuncUpdate(Type);
        if (!&*NewType)
        {
          mCRL2log(error) << "function update has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return NULL;
        }
        Type=NewType;
      }


      Type=replace_possible_sorts(Type); // Set the type to one option in possible sorts, if there are more options.
      *DataTerm=gsMakeOpId(Name,Type);
      if (variable)
      {
        *DataTerm=gsMakeDataVarId(Name,Type);
      }

      assert(&*Type);
      return Type;
    }
    else
    {
      was_ambiguous=true;
      if (strict_ambiguous)
      {
        mCRL2log(debug) << "ambiguous operation " << core::pp_deprecated(Name) << " (ParList " << core::pp_deprecated(ParList) << ")" << std::endl;
        if (nFactPars!=ATERM_NON_EXISTING_POSITION)
        {
          mCRL2log(error) << "ambiguous operation " << core::pp_deprecated(Name) << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"") << std::endl;
        }
        else
        {
          mCRL2log(error) << "ambiguous operation " << core::pp_deprecated(Name) << std::endl;
        }
        return NULL;
      }
      else
      {
        //*DataTerm=gsMakeOpId(Name,data::unknown_sort());
        //if(variable) *DataTerm=gsMakeDataVarId(Name,data::unknown_sort());
        return data::unknown_sort();
      }
    }
  }
  else
  {
    return gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,DataTerm,PosType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
  }
}

// ================================================================================
// Phase 2 -- type inference
// ================================================================================
static ATermList gstcGetNotInferredList(ATermList TypeListList)
{
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1

  ATermList Result=ATmakeList0();
  size_t nFormPars=ATgetLength((ATermList)ATgetFirst(TypeListList));
  std::vector<ATermList> Pars(nFormPars);
  /* if (nFormPars > 0)
  {
    Pars = (ATermList*) malloc(nFormPars*sizeof(ATermList));
  } */
  //DECLA(ATermList,Pars,nFormPars);
  for (size_t i=0; i<nFormPars; i++)
  {
    Pars[i]=ATmakeList0();
  }

  for (; !ATisEmpty(TypeListList); TypeListList=ATgetNext(TypeListList))
  {
    ATermList TypeList=ATLgetFirst(TypeListList);
    for (size_t i=0; i<nFormPars; TypeList=ATgetNext(TypeList),i++)
    {
      Pars[i]=gstcInsertType(Pars[i],ATAgetFirst(TypeList));
    }
  }

  for (size_t i=nFormPars; i>0; i--)
  {
    ATermAppl Sort;
    if (ATgetLength(Pars[i-1])==1)
    {
      Sort=ATAgetFirst(Pars[i-1]);
    }
    else
    {
      // Sort=multiple_possible_sorts(atermpp::aterm_list(ATreverse(Pars[i-1])));
      Sort=multiple_possible_sorts(sort_expression_list(ATreverse(Pars[i-1])));
    }
    Result=ATinsert(Result,Sort);
  }
  // free(Pars);
  return Result;
}

static ATermAppl gstcUpCastNumericType(ATermAppl NeededType, ATermAppl Type, ATermAppl* Par, bool warn_upcasting)
{
  // Makes upcasting from Type to Needed Type for Par. Returns the resulting type.
  // Moreover, *Par is extended with the required type transformations.
  mCRL2log(debug) << "gstcUpCastNumericType " << core::pp_deprecated(NeededType) << " -- " << core::pp_deprecated(Type) << "" << std::endl;

  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (data::is_unknown_sort(data::sort_expression(NeededType)))
  {
    return Type;
  }
  if (gstcEqTypesA(NeededType,Type))
  {
    return Type;
  }

  if (data::is_multiple_possible_sorts(data::sort_expression(NeededType)))
  {
    sort_expression_list l=sort_expression_list(ATLgetArgument(NeededType,0));
    for(sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      ATermAppl r=gstcUpCastNumericType(*i,Type,Par,warn_upcasting);
      if (r!=ATermAppl())
      {
        return r;
      }
    }
    return NULL;
  }

  if (warn_upcasting && gsIsOpId(*Par) && gsIsNumericString(gsATermAppl2String(ATAgetArgument(*Par,0))))
  {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  if (&*gstcTypeMatchA(NeededType,sort_pos::pos()))
  {
    if (&*gstcTypeMatchA(Type,sort_pos::pos()))
    {
      return sort_pos::pos();
    }
  }

  // Try Upcasting to Nat
  if (&*gstcTypeMatchA(NeededType,sort_nat::nat()))
  {
    if (&*gstcTypeMatchA(Type,sort_pos::pos()))
    {
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(sort_nat::cnat(),ATmakeList1(*Par));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << core::pp_deprecated(OldPar) << " to sort Nat by applying Pos2Nat to it." << std::endl;
      }
      return sort_nat::nat();
    }
    if (&*gstcTypeMatchA(Type,sort_nat::nat()))
    {
      return sort_nat::nat();
    }
  }

  // Try Upcasting to Int
  if (&*gstcTypeMatchA(NeededType,sort_int::int_()))
  {
    if (&*gstcTypeMatchA(Type,sort_pos::pos()))
    {
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(sort_int::cint(),ATmakeList1(gsMakeDataAppl(sort_nat::cnat(),ATmakeList1(*Par))));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << core::pp_deprecated(OldPar) << " to sort Int by applying Pos2Int to it." << std::endl;
      }
      return sort_int::int_();
    }
    if (&*gstcTypeMatchA(Type,sort_nat::nat()))
    {
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(sort_int::cint(),ATmakeList1(*Par));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << core::pp_deprecated(OldPar) << " to sort Int by applying Nat2Int to it." << std::endl;
      }
      return sort_int::int_();
    }
    if (&*gstcTypeMatchA(Type,sort_int::int_()))
    {
      return sort_int::int_();
    }
  }

  // Try Upcasting to Real
  if (&*gstcTypeMatchA(NeededType,sort_real::real_()))
  {
    if (&*gstcTypeMatchA(Type,sort_pos::pos()))
    {
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(sort_real::creal(),ATmakeList2(gsMakeDataAppl(sort_int::cint(),
                          ATmakeList1(gsMakeDataAppl(sort_nat::cnat(),ATmakeList1(*Par)))),
                          // (ATerm)gsMakeOpId(ATmakeAppl0(AFun("1",0,ATtrue)),(ATermAppl)sort_pos::pos())));
                          (ATermAppl)sort_pos::c1()));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << core::pp_deprecated(OldPar) << " to sort Real by applying Pos2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (&*gstcTypeMatchA(Type,sort_nat::nat()))
    {
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(sort_real::creal(),ATmakeList2(gsMakeDataAppl(sort_int::cint(),ATmakeList1(*Par)),
                          // (ATerm)gsMakeOpId(ATmakeAppl0(AFun("1",0,ATtrue)),(ATermAppl)sort_pos::pos())));
                          (ATermAppl)(sort_pos::c1())));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << core::pp_deprecated(OldPar) << " to sort Real by applying Nat2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (&*gstcTypeMatchA(Type,sort_int::int_()))
    {
      ATermAppl OldPar=*Par;
      *Par=gsMakeDataAppl(sort_real::creal(),ATmakeList2(*Par,
                          // (ATerm)gsMakeOpId(ATmakeAppl0(AFun("1",0,ATtrue)),(ATermAppl)sort_pos::pos())));
                          (ATermAppl)data_expression(sort_pos::c1())));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << core::pp_deprecated(OldPar) << " to sort Real by applying Int2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (&*gstcTypeMatchA(Type,sort_real::real_()))
    {
      return sort_real::real_();
    }
  }

  return NULL;
}

static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type)
{
  for (ATermList OldTypeList=TypeList; !ATisEmpty(OldTypeList); OldTypeList=ATgetNext(OldTypeList))
  {
    if (gstcEqTypesA(ATAgetFirst(OldTypeList),Type))
    {
      return TypeList;
    }
  }
  return ATinsert(TypeList,Type);
}

static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2)
{
  // returns the intersection of the 2 type list lists

  ATermList Result=ATmakeList0();

  for (; !ATisEmpty(TypeListList2); TypeListList2=ATgetNext(TypeListList2))
  {
    ATermList TypeList2=ATLgetFirst(TypeListList2);
    if (gstcInTypesL(TypeList2,TypeListList1))
    {
      Result=ATinsert(Result,TypeList2);
    }
  }
  return ATreverse(Result);
}

static ATermList gstcAdjustNotInferredList(ATermList PosTypeList, ATermList TypeListList)
{
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  //if PosTypeList has only normal types -- check if it is in TypeListList,
  //if so return PosTypeList, otherwise return NULL
  if (!gstcIsNotInferredL(PosTypeList))
  {
    if (gstcInTypesL(PosTypeList,TypeListList))
    {
      return PosTypeList;
    }
    else
    {
      return NULL;
    }
  }

  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  ATermList NewTypeListList=ATmakeList0();
  for (; !ATisEmpty(TypeListList); TypeListList=ATgetNext(TypeListList))
  {
    ATermList TypeList=ATLgetFirst(TypeListList);
    if (gstcIsTypeAllowedL(TypeList,PosTypeList))
    {
      NewTypeListList=ATinsert(NewTypeListList,TypeList);
    }
  }
  if (ATisEmpty(NewTypeListList))
  {
    return NULL;
  }
  if (ATgetLength(NewTypeListList)==1)
  {
    return ATLgetFirst(NewTypeListList);
  }

  // otherwise return not inferred.
  return gstcGetNotInferredList(ATreverse(NewTypeListList));
}

static bool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList)
{
  //Checks if TypeList is allowed by PosTypeList (each respective element)
  assert(ATgetLength(TypeList)==ATgetLength(PosTypeList));
  for (; !ATisEmpty(TypeList); TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList))
    if (!gstcIsTypeAllowedA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList)))
    {
      return false;
    }
  return true;
}

static bool gstcIsTypeAllowedA(ATermAppl Type, ATermAppl PosType)
{
  //Checks if Type is allowed by PosType
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return true;
  }
  if (gsIsSortsPossible(PosType))
  {
    return gstcInTypesA(Type,ATLgetArgument(PosType,0));
  }

  //PosType is a normal type
  return gstcEqTypesA(Type,PosType);
}

static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType)
{
  // Checks if Type and PosType match by instantiating unknown sorts.
  // It returns the matching instantiation of Type. If matching fails,
  // it returns NULL.

  mCRL2log(debug) << "gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) << " " << std::endl;

  if (data::is_unknown_sort(Type))
  {
    return PosType;
  }
  if (data::is_unknown_sort(PosType) || gstcEqTypesA(Type,PosType))
  {
    return Type;
  }
  if (gsIsSortsPossible(Type) && !gsIsSortsPossible(PosType))
  {
    ATermAppl TmpType=PosType;
    PosType=Type;
    Type=TmpType;
  }
  if (gsIsSortsPossible(PosType))
  {
    ATermList NewTypeList=ATmakeList0();
    for (ATermList PosTypeList=ATLgetArgument(PosType,0); !ATisEmpty(PosTypeList); PosTypeList=ATgetNext(PosTypeList))
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypeList);
      mCRL2log(debug) << "Matching candidate gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: "
                  << core::pp_deprecated(PosType) << " New Type: " << core::pp_deprecated(NewPosType) << "" << std::endl;

      if (&*(NewPosType=gstcTypeMatchA(Type,NewPosType)))
      {
        mCRL2log(debug) << "Match gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) <<
                    " New Type: " << core::pp_deprecated(NewPosType) << "" << std::endl;
        NewTypeList=ATinsert(NewTypeList,NewPosType);
      }
    }
    if (ATisEmpty(NewTypeList))
    {
      mCRL2log(debug) << "No match gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) << " " << std::endl;
      return NULL;
    }

    if (ATisEmpty(ATgetNext(NewTypeList)))
    {
      return ATAgetFirst(NewTypeList);
    }

    return multiple_possible_sorts(sort_expression_list(ATreverse(NewTypeList)));
    // return multiple_possible_sorts(atermpp::aterm_list(ATreverse(NewTypeList)));
  }

  //PosType is a normal type
  //if(!gstcHasUnknown(Type)) return NULL;

  if (gsIsSortId(Type))
  {
    Type=gstcUnwindType(Type);
  }
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (gsIsSortCons(Type))
  {
    ATermAppl ConsType = ATAgetArgument(Type, 0);
    if (gsIsSortList(ConsType))
    {
      if (!sort_list::is_list(sort_expression(PosType)))
      {
        return NULL;
      }
      ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
      if (!&*Res)
      {
        return NULL;
      }
      return sort_list::list(sort_expression(Res));
    }

    if (gsIsSortSet(ConsType))
    {
      if (!sort_set::is_set(sort_expression(PosType)))
      {
        return NULL;
      }
      else
      {
        ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
        if (!&*Res)
        {
          return NULL;
        }
        return sort_set::set_(sort_expression(Res));
      }
    }

    if (gsIsSortBag(ConsType))
    {
      if (!sort_bag::is_bag(sort_expression(PosType)))
      {
        return NULL;
      }
      else
      {
        ATermAppl Res=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
        if (!&*Res)
        {
          return NULL;
        }
        return sort_bag::bag(sort_expression(Res));
      }
    }
  }

  if (gsIsSortArrow(Type))
  {
    if (!gsIsSortArrow(PosType))
    {
      return NULL;
    }
    else
    {
      ATermList ArgTypes=gstcTypeMatchL(ATLgetArgument(Type,0),ATLgetArgument(PosType,0));
      if (!&*ArgTypes)
      {
        return NULL;
      }
      ATermAppl ResType=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
      if (!&*ResType)
      {
        return NULL;
      }
      Type=gsMakeSortArrow(ArgTypes,ResType);
      // mCRL2log(debug) << "gstcTypeMatchA Done: Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) << "" << std::endl;
      return Type;
    }
  }

  return NULL;
}

static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList)
{
  mCRL2log(debug) << "gstcTypeMatchL TypeList: " << core::pp_deprecated(TypeList) << ";    PosTypeList: " <<
              core::pp_deprecated(PosTypeList) << "" << std::endl;

  if (ATgetLength(TypeList)!=ATgetLength(PosTypeList))
  {
    return NULL;
  }

  ATermList Result=ATmakeList0();
  for (; !ATisEmpty(TypeList); TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList))
  {
    ATermAppl Type=gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList));
    if (!&*Type)
    {
      return NULL;
    }
    Result=ATinsert(Result,Type);
  }
  return ATreverse(Result);
}

static bool gstcIsNotInferredL(ATermList TypeList)
{
  for (; !ATisEmpty(TypeList); TypeList=ATgetNext(TypeList))
  {
    ATermAppl Type=ATAgetFirst(TypeList);
    if (is_unknown_sort(Type) || is_multiple_possible_sorts(Type))
    {
      return true;
    }
  }
  return false;
}

static ATermAppl gstcUnwindType(ATermAppl Type)
{
  if (gsIsSortCons(Type))
  {
    return ATsetArgument(Type,gstcUnwindType(ATAgetArgument(Type,1)),1);
  }
  if (gsIsSortArrow(Type))
  {
    Type=ATsetArgument(Type,gstcUnwindType(ATAgetArgument(Type,1)),1);
    ATermList Args=ATLgetArgument(Type,0);
    ATermList NewArgs=ATmakeList0();
    for (; !ATisEmpty(Args); Args=ATgetNext(Args))
    {
      NewArgs=ATinsert(NewArgs,gstcUnwindType(ATAgetFirst(Args)));
    }
    NewArgs=ATreverse(NewArgs);
    Type=ATsetArgument(Type,NewArgs,0);
    return Type;
  }

  if (gsIsSortId(Type))
  {
    ATermAppl Value=ATAtableGet(context.defined_sorts,ATAgetArgument(Type,0));
    if (!&*Value)
    {
      return Type;
    }
    return gstcUnwindType(Value);
  }

  return Type;
}

static ATermAppl gstcUnSet(ATermAppl PosType)
{
  //select Set(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_set::is_set(sort_expression(PosType)))
  {
    return ATAgetArgument(PosType,1);
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes=ATmakeList0();
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=ATLgetArgument(PosType,0); !ATisEmpty(PosTypes); PosTypes=ATgetNext(PosTypes))
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_set::is_set(sort_expression(NewPosType)))
      {
        NewPosType=ATAgetArgument(NewPosType,1);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=ATinsert(NewPosTypes,NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return NULL;
}

static ATermAppl gstcUnBag(ATermAppl PosType)
{
  //select Bag(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_bag::is_bag(sort_expression(PosType)))
  {
    return ATAgetArgument(PosType,1);
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes=ATmakeList0();
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=ATLgetArgument(PosType,0); !ATisEmpty(PosTypes); PosTypes=ATgetNext(PosTypes))
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_bag::is_bag(sort_expression(NewPosType)))
      {
        NewPosType=ATAgetArgument(NewPosType,1);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=ATinsert(NewPosTypes,NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return NULL;
}

static ATermAppl gstcUnList(ATermAppl PosType)
{
  //select List(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_list::is_list(sort_expression(PosType)))
  {
    return ATAgetArgument(PosType,1);
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes=ATmakeList0();
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=ATLgetArgument(PosType,0); !ATisEmpty(PosTypes); PosTypes=ATgetNext(PosTypes))
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_list::is_list(sort_expression(NewPosType)))
      {
        NewPosType=ATAgetArgument(NewPosType,1);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=ATinsert(NewPosTypes,NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return NULL;
}

static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType)
{
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //return TypeX if unique, the set of TypeX as NotInferred if many, NULL otherwise

  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (gsIsSortArrow(PosType))
  {
    ATermList PosArgTypes=ATLgetArgument(PosType,0);

    if (ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes))
    {
      return NULL;
    }
    if (&*gstcTypeMatchL(PosArgTypes,ArgTypes))
    {
      return ATAgetArgument(PosType,1);
    }
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes=ATmakeList0();
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=ATLgetArgument(PosType,0); !ATisEmpty(PosTypes); PosTypes=ATgetNext(PosTypes))
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (gsIsSortArrow(PosType))
      {
        ATermList PosArgTypes=ATLgetArgument(PosType,0);
        if (ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes))
        {
          continue;
        }
        if (&*gstcTypeMatchL(PosArgTypes,ArgTypes))
        {
          NewPosType=ATAgetArgument(NewPosType,1);
        }
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=ATinsertUnique(NewPosTypes,NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return NULL;
}

static ATermList gstcGetVarTypes(ATermList VarDecls)
{
  ATermList Result=ATmakeList0();
  for (; !ATisEmpty(VarDecls); VarDecls=ATgetNext(VarDecls))
  {
    Result=ATinsert(Result,ATAgetArgument(ATAgetFirst(VarDecls),1));
  }
  return ATreverse(Result);
}

// Replace occurrences of multiple_possible_sorts([s1,...,sn]) by selecting
// one of the possible sorts from s1,...,sn. Currently, the first is chosen.
static ATermAppl replace_possible_sorts(ATermAppl Type)
{
  if (gsIsSortsPossible(data::sort_expression(Type)))
  {
    return ATAgetFirst(ATLgetArgument(Type,0)); // get the first element of the possible sorts.
  }
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return data::unknown_sort();
  }
  if (gsIsSortId(Type))
  {
    return Type;
  }
  if (gsIsSortCons(Type)) 
  {
    return ATsetArgument(Type,replace_possible_sorts(ATAgetArgument(Type,1)),1);
  }

  if (gsIsSortStruct(Type))
  {
    return Type;  // I assume that there are no possible sorts in sort constructors. JFG.
  }

  if (gsIsSortArrow(Type))
  {
    ATermList NewTypeList=ATmakeList0();
    for (ATermList TypeList=ATLgetArgument(Type,0); !ATisEmpty(TypeList); TypeList=ATgetNext(TypeList))
    {
      NewTypeList=ATinsert(NewTypeList,replace_possible_sorts(ATAgetFirst(TypeList)));
    }
    ATermAppl ResultType=ATAgetArgument(Type,1);
    return gsMakeSortArrow(ATreverse(NewTypeList),replace_possible_sorts(ResultType));
  }
  assert(0); // All cases are dealt with above.
  return Type; // Avoid compiler warnings.
}


static bool gstcHasUnknown(ATermAppl Type)
{
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return true;
  }
  if (gsIsSortId(Type))
  {
    return false;
  }
  if (gsIsSortCons(Type))
  {
    return gstcHasUnknown(ATAgetArgument(Type,1));
  }
  if (gsIsSortStruct(Type))
  {
    return false;
  }

  if (gsIsSortArrow(Type))
  {
    for (ATermList TypeList=ATLgetArgument(Type,0); !ATisEmpty(TypeList); TypeList=ATgetNext(TypeList))
      if (gstcHasUnknown(ATAgetFirst(TypeList)))
      {
        return true;
      }
    return gstcHasUnknown(ATAgetArgument(Type,1));
  }

  return true;
}

static bool gstcIsNumericType(ATermAppl Type)
{
  //returns true if Type is Bool,Pos,Nat,Int or Real
  //otherwise return fase
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return false;
  }
  return (bool)(sort_bool::is_bool(sort_expression(Type))||
                  sort_pos::is_pos(sort_expression(Type))||
                  sort_nat::is_nat(sort_expression(Type))||
                  sort_int::is_int(sort_expression(Type))||
                  sort_real::is_real(sort_expression(Type)));
}

static ATermAppl gstcMaximumType(ATermAppl Type1, ATermAppl Type2)
{
  // if Type1 is convertible into Type2 or vice versa, the most general
  // of these types are returned. If not conversion is possible the result
  // is NULL. Conversions only apply between numerical types
  if (gstcEqTypesA(Type1,Type2))
  {
    return Type1;
  }
  if (data::is_unknown_sort(data::sort_expression(Type1)))
  {
    return Type2;
  }
  if (data::is_unknown_sort(data::sort_expression(Type2)))
  {
    return Type1;
  }
  if (gstcEqTypesA(Type1,sort_real::real_()))
  {
    if (gstcEqTypesA(Type2,sort_int::int_()))
    {
      return Type1;
    }
    if (gstcEqTypesA(Type2,sort_nat::nat()))
    {
      return Type1;
    }
    if (gstcEqTypesA(Type2,sort_pos::pos()))
    {
      return Type1;
    }
    return NULL;
  }
  if (gstcEqTypesA(Type1,sort_int::int_()))
  {
    if (gstcEqTypesA(Type2,sort_real::real_()))
    {
      return Type2;
    }
    if (gstcEqTypesA(Type2,sort_nat::nat()))
    {
      return Type1;
    }
    if (gstcEqTypesA(Type2,sort_pos::pos()))
    {
      return Type1;
    }
    return NULL;
  }
  if (gstcEqTypesA(Type1,sort_nat::nat()))
  {
    if (gstcEqTypesA(Type2,sort_real::real_()))
    {
      return Type2;
    }
    if (gstcEqTypesA(Type2,sort_int::int_()))
    {
      return Type2;
    }
    if (gstcEqTypesA(Type2,sort_pos::pos()))
    {
      return Type1;
    }
    return NULL;
  }
  if (gstcEqTypesA(Type1,sort_pos::pos()))
  {
    if (gstcEqTypesA(Type2,sort_real::real_()))
    {
      return Type2;
    }
    if (gstcEqTypesA(Type2,sort_int::int_()))
    {
      return Type2;
    }
    if (gstcEqTypesA(Type2,sort_nat::nat()))
    {
      return Type2;
    }
    return NULL;
  }
  return NULL;
}

static ATermAppl gstcExpandNumTypesUp(ATermAppl Type)
{
  //Expand Pos.. to possible bigger types.
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (gstcEqTypesA(sort_pos::pos(),Type))
  {
    return multiple_possible_sorts(atermpp::make_list(sort_pos::pos(), sort_nat::nat(), sort_int::int_(),sort_real::real_()));
  }
  if (gstcEqTypesA(sort_nat::nat(),Type))
  {
    return multiple_possible_sorts(atermpp::make_list(sort_nat::nat(), sort_int::int_(),sort_real::real_()));
  }
  if (gstcEqTypesA(sort_int::int_(),Type))
  {
    return multiple_possible_sorts(atermpp::make_list(sort_int::int_(), sort_real::real_()));
  }
  if (gsIsSortId(Type))
  {
    return Type;
  }
  if (gsIsSortCons(Type))
  {
    return ATsetArgument(Type,gstcExpandNumTypesUp(ATAgetArgument(Type,1)),1);
  }
  if (gsIsSortStruct(Type))
  {
    return Type;
  }

  if (gsIsSortArrow(Type))
  {
    //the argument types, and if the resulting type is SortArrow -- recursively
    ATermList NewTypeList=ATmakeList0();
    for (ATermList TypeList=ATLgetArgument(Type,0); !ATisEmpty(TypeList); TypeList=ATgetNext(TypeList))
    {
      NewTypeList=ATinsert(NewTypeList,gstcExpandNumTypesUp(gstcUnwindType(ATAgetFirst(TypeList))));
    }
    ATermAppl ResultType=ATAgetArgument(Type,1);
    if (!gsIsSortArrow(ResultType))
    {
      return ATsetArgument(Type,ATreverse(NewTypeList),0);
    }
    else
    {
      return gsMakeSortArrow(ATreverse(NewTypeList),gstcExpandNumTypesUp(gstcUnwindType(ResultType)));
    }
  }

  return Type;
}

static ATermAppl gstcExpandNumTypesDown(ATermAppl Type)
{
  // Expand Numeric types down
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (gsIsSortId(Type))
  {
    Type=gstcUnwindType(Type);
  }

  bool function=false;
  ATermList Args=NULL;
  if (gsIsSortArrow(Type))
  {
    function=true;
    Args=ATLgetArgument(Type,0);
    Type=ATAgetArgument(Type,1);
  }

  if (gstcEqTypesA(sort_real::real_(),Type))
  {
    Type=multiple_possible_sorts(atermpp::make_list(sort_pos::pos(),sort_nat::nat(),sort_int::int_(),sort_real::real_()));
  }
  if (gstcEqTypesA(sort_int::int_(),Type))
  {
    Type=multiple_possible_sorts(atermpp::make_list(sort_pos::pos(),sort_nat::nat(),sort_int::int_()));
  }
  if (gstcEqTypesA(sort_nat::nat(),Type))
  {
    Type=multiple_possible_sorts(atermpp::make_list(sort_pos::pos(),sort_nat::nat()));
  }

  return (function)?gsMakeSortArrow(Args,Type):Type;
}

static ATermAppl gstcMinType(ATermList TypeList)
{
  return ATAgetFirst(TypeList);
}


// =========================== MultiActions
static bool gstcMActIn(ATermList MAct, ATermList MActs)
{
  //returns true if MAct is in MActs
  for (; !ATisEmpty(MActs); MActs=ATgetNext(MActs))
    if (gstcMActEq(MAct,ATLgetFirst(MActs)))
    {
      return true;
    }

  return false;
}

static bool gstcMActEq(ATermList MAct1, ATermList MAct2)
{
  //returns true if the two multiactions are equal.
  if (ATgetLength(MAct1)!=ATgetLength(MAct2))
  {
    return false;
  }
  if (ATisEmpty(MAct1))
  {
    return true;
  }
  ATermAppl Act1=ATAgetFirst(MAct1);
  MAct1=ATgetNext(MAct1);

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  ATermList NewMAct2=ATmakeList0();
  for (; !ATisEmpty(MAct2); MAct2=ATgetNext(MAct2))
  {
    ATermAppl Act2=ATAgetFirst(MAct2);
    if (ATisEqual(Act1,Act2))
    {
      MAct2=ATconcat(ATreverse(NewMAct2),ATgetNext(MAct2));
      return gstcMActEq(MAct1,MAct2);
    }
    else
    {
      NewMAct2=ATinsert(NewMAct2,Act2);
    }
  }
  return false;
}

static ATermAppl gstcUnifyMinType(ATermAppl Type1, ATermAppl Type2)
{
  //Find the minimal type that Unifies the 2. If not possible, return NULL.
  ATermAppl Res=gstcTypeMatchA(Type1,Type2);
  if (!&*Res)
  {
    Res=gstcTypeMatchA(Type1,gstcExpandNumTypesUp(Type2));
    if (!&*Res)
    {
      Res=gstcTypeMatchA(Type2,gstcExpandNumTypesUp(Type1));
    }
    if (!&*Res)
    {
      mCRL2log(debug) << "gstcUnifyMinType: No match: Type1 " << core::pp_deprecated(Type1) << "; Type2 " << core::pp_deprecated(Type2) << "; " << std::endl;
      return NULL;
    }
  }

  if (gsIsSortsPossible(Res))
  {
    Res=ATAgetFirst(ATLgetArgument(Res,0));
  }
  mCRL2log(debug) << "gstcUnifyMinType: Type1 " << core::pp_deprecated(Type1) << "; Type2 " << core::pp_deprecated(Type2) << "; Res: " << core::pp_deprecated(Res) << "" << std::endl;
  return Res;
}

static ATermAppl gstcMatchIf(ATermAppl Type)
{
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  assert(gsIsSortArrow(Type));
  ATermList Args=ATLgetArgument(Type,0);
  ATermAppl Res=ATAgetArgument(Type,1);
  assert((ATgetLength(Args)==3));
  //assert(gsIsBool(ATAgetFirst(Args)));
  Args=ATgetNext(Args);

  if (!&*(Res=gstcUnifyMinType(Res,ATAgetFirst(Args))))
  {
    return NULL;
  }
  Args=ATgetNext(Args);
  if (!&*(Res=gstcUnifyMinType(Res,ATAgetFirst(Args))))
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList3(static_cast<ATermAppl>(sort_bool::bool_()),Res,Res),Res);
}

static ATermAppl gstcMatchEqNeqComparison(ATermAppl Type)
{
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if (!&*Arg)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(Arg,Arg),sort_bool::bool_());
}

static ATermAppl gstcMatchListOpCons(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(gstcUnwindType(Res))));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=ATAgetArgument(Arg2,1);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!&*Res)
  {
    return NULL;
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(Res,static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
}

static ATermAppl gstcMatchListOpSnoc(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=ATAgetArgument(Arg1,1);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!&*Res)
  {
    return NULL;
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(static_cast<ATermAppl>(sort_list::list(sort_expression(Res))),Res),sort_list::list(sort_expression(Res)));
}

static ATermAppl gstcMatchListOpConcat(ATermAppl Type)
{
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=ATAgetArgument(Arg1,1);

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=ATAgetArgument(Arg2,1);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!&*Res)
  {
    return NULL;
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(static_cast<ATermAppl>(sort_list::list(sort_expression(Res))),
       static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
}

static ATermAppl gstcMatchListOpEltAt(ATermAppl Type)
{
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=ATAgetArgument(Arg1,1);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!&*Res)
  {
    return NULL;
  }

  //assert((gsIsSortNat(ATAgetFirst(ATgetNext(Args))));

  return gsMakeSortArrow(ATmakeList2(static_cast<ATermAppl>(sort_list::list(sort_expression(Res))),
               static_cast<ATermAppl>(sort_nat::nat())),Res);
}

static ATermAppl gstcMatchListOpHead(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=ATAgetArgument(Arg,1);

  Res=gstcUnifyMinType(Res,Arg);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),Res);
}

static ATermAppl gstcMatchListOpTail(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=ATAgetArgument(Arg,1);

  Res=gstcUnifyMinType(Res,Arg);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),
                   sort_list::list(sort_expression(Res)));
}

//Sets
static ATermAppl gstcMatchSetOpSet2Bag(ATermAppl Type)
{
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_bag::is_bag(sort_expression(Res)));
  Res=ATAgetArgument(Res,1);

  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=ATAgetArgument(Arg,1);

  Arg=gstcUnifyMinType(Arg,Res);
  if (!&*Arg)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_set::set_(sort_expression(Arg)))),
                  sort_bag::bag(sort_expression(Arg)));
}

static ATermAppl gstcMatchListSetBagOpIn(ATermAppl Type)
{
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  //assert(gsIsBool(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(gsIsSortCons(Arg2));
  ATermAppl Arg2s=ATAgetArgument(Arg2,1);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2s);
  if (!&*Arg)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(Arg,ATsetArgument(Arg2,Arg,1)),sort_bool::bool_());
}

static ATermAppl gstcMatchSetBagOpUnionDiffIntersect(ATermAppl Type)
{
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Res))||sort_bag::is_bag(sort_expression(Res)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  if (gstcIsNumericType(Arg1))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Arg1))||sort_bag::is_bag(sort_expression(Arg1)));

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  if (gstcIsNumericType(Arg2))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Arg2))||sort_bag::is_bag(sort_expression(Arg2)));

  Res=gstcUnifyMinType(Res,Arg1);
  if (!&*Res)
  {
    return NULL;
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(Res,Res),Res);
}

static ATermAppl gstcMatchSetOpSetCompl(ATermAppl Type)
{
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=ATAgetArgument(Res,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  if (gstcIsNumericType(Arg))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=ATAgetArgument(Arg,1);

  Res=gstcUnifyMinType(Res,Arg);
  if (!&*Res)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_set::set_(sort_expression(Res)))),sort_set::set_(sort_expression(Res)));
}

//Bags
static ATermAppl gstcMatchBagOpBag2Set(ATermAppl Type)
{
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  ATermAppl Res=ATAgetArgument(Type,1);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=ATAgetArgument(Res,1);

  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_bag::is_bag(sort_expression(Arg)));
  Arg=ATAgetArgument(Arg,1);

  Arg=gstcUnifyMinType(Arg,Res);
  if (!&*Arg)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_bag::bag(sort_expression(Arg)))),sort_set::set_(sort_expression(Arg)));
}

static ATermAppl gstcMatchBagOpBagCount(ATermAppl Type)
{
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  //If the second argument is not a Bag, don't match

  if (!gsIsSortArrow(Type))
  {
    return Type;
  }
  //assert(gsIsNat(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  if (!(ATgetLength(Args)==2))
  {
    return Type;
  }

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  if (!sort_bag::is_bag(sort_expression(Arg2)))
  {
    return Type;
  }
  Arg2=ATAgetArgument(Arg2,1);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if (!&*Arg)
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList2(Arg,static_cast<ATermAppl>(sort_bag::bag(sort_expression(Arg)))),sort_nat::nat());
}


static ATermAppl gstcMatchFuncUpdate(ATermAppl Type)
{
  //tries to sort out the types of FuncUpdate ((A->B)xAxB->(A->B))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==3));
  ATermAppl Arg1=ATAgetFirst(Args);
  assert(gsIsSortArrow(Arg1));
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg3=ATAgetFirst(Args);
  ATermAppl Res=ATAgetArgument(Type,1);
  assert(gsIsSortArrow(Res));

  Arg1=gstcUnifyMinType(Arg1,Res);
  if (!&*Arg1)
  {
    return NULL;
  }

  // determine A and B from Arg1:
  ATermList LA=ATLgetArgument(Arg1,0);
  assert((ATgetLength(LA)==1));
  ATermAppl A=ATAgetFirst(LA);
  ATermAppl B=ATAgetArgument(Arg1,1);

  if (!&*gstcUnifyMinType(A,Arg2))
  {
    return NULL;
  }
  if (!&*gstcUnifyMinType(B,Arg3))
  {
    return NULL;
  }

  return gsMakeSortArrow(ATmakeList3(Arg1,A,B),Arg1);
}

static void gstcErrorMsgCannotCast(ATermAppl CandidateType, ATermList Arguments, ATermList ArgumentTypes)
{
  //prints more information about impossible cast.
  //at this point we know that Arguments cannot be cast to CandidateType. We need to find out why and print.
  assert(ATgetLength(Arguments)==ATgetLength(ArgumentTypes));

  ATermList CandidateList;
  if (gsIsSortsPossible(CandidateType))
  {
    CandidateList=ATLgetArgument(CandidateType,0);
  }
  else
  {
    CandidateList=ATmakeList1(CandidateType);
  }

  ATermList NewCandidateList=ATmakeList0();
  for (ATermList l=CandidateList; !ATisEmpty(l); l=ATgetNext(l))
  {
    ATermAppl Candidate=ATAgetFirst(l);
    if (!gsIsSortArrow(Candidate))
    {
      continue;
    }
    NewCandidateList=ATinsert(NewCandidateList,ATLgetArgument(Candidate,0));
  }
  CandidateList=ATreverse(NewCandidateList);

  //CandidateList=gstcTraverseListList(CandidateList);
  ATermList CurrentCandidateList=CandidateList;
  CandidateList=ATmakeList0();
  while (true)
  {
    ATermList NewCurrentCandidateList=ATmakeList0();
    ATermList NewList=ATmakeList0();
    for (ATermList l=CurrentCandidateList; !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermList List=ATLgetFirst(l);
      if (!ATisEmpty(List))
      {
        NewList=ATinsert(NewList,ATAgetFirst(List));
        NewCurrentCandidateList=ATinsertUnique(NewCurrentCandidateList,ATgetNext(List));
      }
      else
      {
        NewCurrentCandidateList=ATinsert(NewCurrentCandidateList,ATmakeList0());
      }
    }
    if (ATisEmpty(NewList))
    {
      break;
    }
    CurrentCandidateList=ATreverse(NewCurrentCandidateList);
    CandidateList=ATinsert(CandidateList,ATreverse(NewList));
  }
  CandidateList=ATreverse(CandidateList);

  for (ATermList l=Arguments, m=ArgumentTypes, n=CandidateList; !(ATisEmpty(l)||ATisEmpty(m)||ATisEmpty(n)); l=ATgetNext(l), m=ATgetNext(m), n=ATgetNext(n))
  {
    ATermList PosTypes=ATLgetFirst(n);
    ATermAppl NeededType=ATAgetFirst(m);
    bool found=true;
    for (ATermList k=PosTypes; !ATisEmpty(k); k=ATgetNext(k))
    {
      if (&*gstcTypeMatchA(ATAgetFirst(k),NeededType))
      {
        found=false;
        break;
      }
    }
    if (found)
    {
      ATermAppl Sort;
      if (ATgetLength(PosTypes)==1)
      {
        Sort=ATAgetFirst(PosTypes);
      }
      else
      {
        // Sort=multiple_possible_sorts(atermpp::aterm_list(PosTypes));
        Sort=multiple_possible_sorts(sort_expression_list(PosTypes));
      }
      mCRL2log(error) << "this is, for instance, because cannot cast " << core::pp_deprecated(ATAgetFirst(l)) << " to type " << core::pp_deprecated(Sort) << std::endl;
      break;
    }
  }
}

//===================================
// Type checking modal formulas
//===================================

static ATermAppl gstcTraverseStateFrm(const ATermTable &Vars, const ATermTable &StateVars, ATermAppl StateFrm)
{
  mCRL2log(debug) << "gstcTraverseStateFrm: " + core::pp_deprecated(StateFrm) + "" << std::endl;

  if (gsIsStateTrue(StateFrm) || gsIsStateFalse(StateFrm) || gsIsStateDelay(StateFrm) || gsIsStateYaled(StateFrm))
  {
    return StateFrm;
  }

  if (gsIsStateNot(StateFrm))
  {
    ATermAppl NewArg=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,0));
    if (!&*NewArg)
    {
      return NULL;
    }
    return ATsetArgument(StateFrm,NewArg,0);
  }

  if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm) || gsIsStateImp(StateFrm))
  {
    ATermAppl NewArg1=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,0));
    if (!&*NewArg1)
    {
      return NULL;
    }
    ATermAppl NewArg2=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,1));
    if (!&*NewArg2)
    {
      return NULL;
    }
    return ATsetArgument(ATsetArgument(StateFrm,NewArg1,0),NewArg2,1);
  }

  if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=ATLgetArgument(StateFrm,0);
    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,VarList,NewVars))
    {
      ATtableDestroy(CopyVars);
      return NULL;
    }

    ATermAppl NewArg2=gstcTraverseStateFrm(NewVars,StateVars,ATAgetArgument(StateFrm,1));
    if (!&*NewArg2)
    {
      return NULL;
    }
    ATtableDestroy(CopyVars);

    return ATsetArgument(StateFrm,NewArg2,1);
  }

  if (gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm))
  {
    ATermAppl RegFrm=gstcTraverseRegFrm(Vars,ATAgetArgument(StateFrm,0));
    if (!&*RegFrm)
    {
      return NULL;
    }
    ATermAppl NewArg2=gstcTraverseStateFrm(Vars,StateVars,ATAgetArgument(StateFrm,1));
    if (!&*NewArg2)
    {
      return NULL;
    }
    return ATsetArgument(ATsetArgument(StateFrm,RegFrm,0),NewArg2,1);
  }

  if (gsIsStateDelayTimed(StateFrm) || gsIsStateYaledTimed(StateFrm))
  {
    ATermAppl Time=ATAgetArgument(StateFrm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (!&*NewType)
    {
      return NULL;
    }

    if (!&*gstcTypeMatchA(sort_real::real_(),NewType))
    {
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,&Time);
      if (!&*CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast time value " << core::pp_deprecated(Time) << " to type Real (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
        return NULL;
      }
    }
    return ATsetArgument(StateFrm,Time,0);
  }

  if (gsIsStateVar(StateFrm))
  {
    ATermAppl StateVarName=ATAgetArgument(StateFrm,0);
    ATermList TypeList=ATLtableGet(StateVars,StateVarName);
    if (!&*TypeList)
    {
      mCRL2log(error) << "undefined state variable " << core::pp_deprecated(StateVarName) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
      return NULL;
    }

    ATermList Pars=ATLgetArgument(StateFrm,1);
    if (ATgetLength(TypeList)!=ATgetLength(Pars))
    {
      mCRL2log(error) << "incorrect number of parameters for state variable " << core::pp_deprecated(StateVarName) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
      return NULL;
    }

    ATermList r=ATmakeList0();
    bool success=true;
    for (; !ATisEmpty(Pars); Pars=ATgetNext(Pars),TypeList=ATgetNext(TypeList))
    {
      ATermAppl Par=ATAgetFirst(Pars);
      ATermAppl ParType=ATAgetFirst(TypeList);
      ATermAppl NewParType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,gstcExpandNumTypesDown(ParType));
      if (!&*NewParType)
      {
        mCRL2log(error) << "typechecking " << core::pp_deprecated(StateFrm) << std::endl;
        success=false;
        break;
      }

      if (!&*gstcTypeMatchA(ParType,NewParType))
      {
        //upcasting
        NewParType=gstcUpCastNumericType(ParType,NewParType,&Par);
        if (!&*NewParType)
        {
          mCRL2log(error) << "cannot (up)cast " << core::pp_deprecated(Par) << " to type " << core::pp_deprecated(ParType) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
          success=false;
          break;
        }
      }

      r=ATinsert(r,Par);
    }

    if (!success)
    {
      return NULL;
    }

    return ATsetArgument(StateFrm,ATreverse(r),1);

  }

  if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm))
  {
    ATermTable CopyStateVars=ATtableCreate(63,50);
    gstcATermTableCopy(StateVars,CopyStateVars);

    // Make the new state variable:
    ATermTable FormPars=ATtableCreate(63,50);
    ATermList r=ATmakeList0();
    ATermList t=ATmakeList0();
    bool success=true;
    for (ATermList l=ATLgetArgument(StateFrm,1); !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermAppl o=ATAgetFirst(l);

      ATermAppl VarName=ATAgetArgument(ATAgetArgument(o,0),0);
      if (&*ATAtableGet(FormPars,VarName))
      {
        mCRL2log(error) << "non-unique formal parameter " << core::pp_deprecated(VarName) << " (typechecking " << core::pp_deprecated(StateFrm) << ")" << std::endl;
        success=false;
        break;
      }

      ATermAppl VarType=ATAgetArgument(ATAgetArgument(o,0),1);
      if (!gstcIsSortExprDeclared(VarType))
      {
        mCRL2log(error) << "type error occurred while typechecking " << core::pp_deprecated(StateFrm) << std::endl;
        success=false;
        break;
      }

      ATtablePut(FormPars,VarName, VarType);

      ATermAppl VarInit=ATAgetArgument(o,1);
      ATermAppl VarInitType=gstcTraverseVarConsTypeD(Vars,Vars,&VarInit,gstcExpandNumTypesDown(VarType));
      if (!&*VarInitType)
      {
        mCRL2log(error) << "typechecking " << core::pp_deprecated(StateFrm) << std::endl;
        success=false;
        break;
      }

      if (!&*gstcTypeMatchA(VarType,VarInitType))
      {
        //upcasting
        VarInitType=gstcUpCastNumericType(VarType,VarInitType,&VarInit);
        if (!&*VarInitType)
        {
          mCRL2log(error) << "cannot (up)cast " << core::pp_deprecated(VarInit) << " to type " << core::pp_deprecated(VarType) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << std::endl;
          success=false;
          break;
        }
      }

      r=ATinsert(r,ATsetArgument(o,VarInit,1));
      t=ATinsert(t,VarType);
    }

    if (!success)
    {
      ATtableDestroy(CopyStateVars);
      ATtableDestroy(FormPars);
      return NULL;
    }

    StateFrm=ATsetArgument(StateFrm,ATreverse(r),1);
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);
    gstcATermTableCopy(FormPars,CopyVars);
    ATtableDestroy(FormPars);

    ATtablePut(CopyStateVars,ATAgetArgument(StateFrm,0),ATreverse(t));
    ATermAppl NewArg=gstcTraverseStateFrm(CopyVars,CopyStateVars,ATAgetArgument(StateFrm,2));
    ATtableDestroy(CopyStateVars);
    ATtableDestroy(CopyVars);
    if (!&*NewArg)
    {
      mCRL2log(error) << "while typechecking " << core::pp_deprecated(StateFrm) << std::endl;
      return NULL;
    }
    return ATsetArgument(StateFrm,NewArg,2);
  }

  if (gsIsDataExpr(StateFrm))
  {
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars, Vars, &StateFrm, sort_bool::bool_());
    if (!&*Type)
    {
      return NULL;
    }
    return StateFrm;
  }

  assert(0);
  return NULL;
}

static ATermAppl gstcTraverseRegFrm(const ATermTable &Vars, ATermAppl RegFrm)
{
  mCRL2log(debug) << "gstcTraverseRegFrm: " + core::pp_deprecated(RegFrm) + "" << std::endl;
  if (gsIsRegNil(RegFrm))
  {
    return RegFrm;
  }

  if (gsIsRegSeq(RegFrm) || gsIsRegAlt(RegFrm))
  {
    ATermAppl NewArg1=gstcTraverseRegFrm(Vars,ATAgetArgument(RegFrm,0));
    if (!&*NewArg1)
    {
      return NULL;
    }
    ATermAppl NewArg2=gstcTraverseRegFrm(Vars,ATAgetArgument(RegFrm,1));
    if (!&*NewArg2)
    {
      return NULL;
    }
    return ATsetArgument(ATsetArgument(RegFrm,NewArg1,0),NewArg2,1);
  }

  if (gsIsRegTrans(RegFrm) || gsIsRegTransOrNil(RegFrm))
  {
    ATermAppl NewArg=gstcTraverseRegFrm(Vars,ATAgetArgument(RegFrm,0));
    if (!&*NewArg)
    {
      return NULL;
    }
    return ATsetArgument(RegFrm,NewArg,0);
  }

  if (gsIsActFrm(RegFrm))
  {
    return gstcTraverseActFrm(Vars, RegFrm);
  }

  assert(0);
  return NULL;
}

static ATermAppl gstcTraverseActFrm(const ATermTable &Vars, ATermAppl ActFrm)
{
  mCRL2log(debug) << "gstcTraverseActFrm: " + core::pp_deprecated(ActFrm) + "" << std::endl;

  if (gsIsActTrue(ActFrm) || gsIsActFalse(ActFrm))
  {
    return ActFrm;
  }

  if (gsIsActNot(ActFrm))
  {
    ATermAppl NewArg=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,0));
    if (!&*NewArg)
    {
      return NULL;
    }
    return ATsetArgument(ActFrm,NewArg,0);
  }

  if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm) || gsIsActImp(ActFrm))
  {
    ATermAppl NewArg1=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,0));
    if (!&*NewArg1)
    {
      return NULL;
    }
    ATermAppl NewArg2=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,1));
    if (!&*NewArg2)
    {
      return NULL;
    }
    return ATsetArgument(ATsetArgument(ActFrm,NewArg1,0),NewArg2,1);
  }

  if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=ATLgetArgument(ActFrm,0);
    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,VarList,NewVars))
    {
      ATtableDestroy(CopyVars);
      return NULL;
    }

    ATermAppl NewArg2=gstcTraverseActFrm(NewVars,ATAgetArgument(ActFrm,1));
    if (!&*NewArg2)
    {
      return NULL;
    }
    ATtableDestroy(CopyVars);

    return ATsetArgument(ActFrm,NewArg2,1);
  }

  if (gsIsActAt(ActFrm))
  {
    ATermAppl NewArg1=gstcTraverseActFrm(Vars,ATAgetArgument(ActFrm,0));
    if (!&*NewArg1)
    {
      return NULL;
    }

    ATermAppl Time=ATAgetArgument(ActFrm,1);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (!&*NewType)
    {
      return NULL;
    }

    if (!&*gstcTypeMatchA(sort_real::real_(),NewType))
    {
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,&Time);
      if (!&*CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast time value " << core::pp_deprecated(Time) << " to type Real (typechecking action formula " << core::pp_deprecated(ActFrm) << ")" << std::endl;
        return NULL;
      }
    }
    return ATsetArgument(ATsetArgument(ActFrm,NewArg1,0),Time,1);
  }

  if (gsIsMultAct(ActFrm))
  {
    ATermList r=ATmakeList0();
    for (ATermList l=ATLgetArgument(ActFrm,0); !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermAppl o=ATAgetFirst(l);
      assert(gsIsParamId(o));
      o=gstcTraverseActProcVarConstP(Vars,o);
      if (!&*o)
      {
        return NULL;
      }
      r=ATinsert(r,o);
    }
    return ATsetArgument(ActFrm,ATreverse(r),0);
  }

  if (gsIsDataExpr(ActFrm))
  {
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars, Vars, &ActFrm, sort_bool::bool_());
    if (!&*Type)
    {
      return NULL;
    }
    return ActFrm;
  }

  assert(0);
  return NULL;
}

}
}
