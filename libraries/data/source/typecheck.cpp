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
  ATermList n;
  for (; !l.empty(); l=l.tail())
  {
    if (std::find(m.begin(),m.end(),l.front()) == m.end())
    {
      n = push_front(n,l.front());
    }
  }
  return reverse(n);
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
static bool gstcReadInConstructors(ATermList NewSorts=aterm_list(aterm()));
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
               ATermTable &FreeVars, bool strict_ambiguous=true, size_t nPars = std::string::npos, const bool warn_upcasting=false, const bool print_cast_error=true);

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
  return gsMakeProcVarId(gsString2ATermAppl("init"),aterm_list());
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

/* ATerm extensions */
/**
 * \brief Conditional prepend operation on ATermList
 * \param[in] list an ATerm list
 * \param[in] el the aterm to prepend
 * \return el ++ list if not el in list, list if el in list
**/
inline ATermList ATinsertUnique(const ATermList &list, const ATerm &el)
{
  if (std::find(list.begin(),list.end(), el) == list.end())
  {
    return push_front(list, el);
  }
  return list;
}

inline
ATermList ATreplace(const ATermList &list_in, const ATerm &el, const size_t idx) // Replace one element of a list.
{
  ATermList list=list_in;
  size_t i;
  std::vector<aterm> buffer;

  for (i=0; i<idx; i++)
  {
    buffer.push_back(list.front());
    list = list.tail();
  }
  /* Skip the old element */
  list = list.tail();
  /* Add the new element */
  list = push_front(list, el);
  /* Add the prefix */
  for (i=idx; i>0; i--)
  {
    list = push_front(list, aterm(buffer[i-1]));
  }

  return list;
}


/**
 *  * \brief Gets the first argument as ATermAppl
 *   **/
inline const ATermAppl &ATAgetFirst(const ATermList &List)
{
  const ATerm &Result = List.front();
  return aterm_cast<const ATermAppl>(Result);
}
  
/**
 * \brief Gets the first argument as ATermList
**/
inline const ATermList &ATLgetFirst(const ATermList &List)
{
  const ATerm &Result = List.front();
  return aterm_cast<const ATermList>(Result);
}

//type checking functions
//-----------------------

ATermAppl type_check_data_spec(ATermAppl data_spec)
{
  mCRL2log(verbose) << "type checking data specification..." << std::endl;

  ATermAppl Result;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking read-in phase started" << std::endl;

  if (gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0))))
  {
    // Check sorts for loops
    // Unwind sorts to enable equiv and subtype relations
    if (gstcReadInConstructors())
    {
      if (gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0)),
                          aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0))))
      {
        body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(3))(0));
        mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

        mCRL2log(debug) << "type checking transform VarConst phase started" << std::endl;

        if (gstcTransformVarConsTypeData())
        {
          mCRL2log(debug) << "type checking transform VarConst phase finished" << std::endl;

          Result = data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);

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

  ATermAppl Result;

  mCRL2log(debug) << "type checking phase started: " << core::pp_deprecated(proc_spec) << "" << std::endl;
  gstcDataInit();

  ATermAppl data_spec = aterm_cast<aterm_appl>(proc_spec(0));
  if (gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0))))
  {
    // Check sorts for loops
    // Unwind sorts to enable equiv and subtype relations
    if (gstcReadInConstructors())
    {
      if (gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0)),
                          aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0))))
      {
        body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(3))(0));
        if (gstcReadInActs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec(1))(0))))
        {
          ATermAppl glob_var_spec = aterm_cast<aterm_appl>(proc_spec(2));
          ATermList glob_vars = aterm_cast<aterm_list>(glob_var_spec(0));
          ATermTable dummy;
          if (gstcAddVars2Table(context.glob_vars, glob_vars,dummy))
          {
            if (gstcReadInProcsAndInit(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec(3))(0)),
                                       aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(proc_spec(4))(0))))
            {
              mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

              mCRL2log(debug) << "type checking transform ActProc+VarConst phase started" << std::endl;
              if (gstcTransformVarConsTypeData())
              {
                if (gstcTransformActProcVarConst())
                {
                  mCRL2log(debug) << "type checking transform ActProc+VarConst phase finished" << std::endl;

                  data_spec=aterm_cast<aterm_appl>(proc_spec(0));
                  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
                  Result=proc_spec.set_argument(data_spec,0);
                  Result=Result.set_argument(gsMakeProcEqnSpec(gstcWriteProcs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec(3))(0)))),3);
                  Result=Result.set_argument(gsMakeProcessInit(aterm_cast<aterm_appl>(ATtableGet(body.proc_bodies,INIT_KEY()))),4);

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

  ATermAppl Result;

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of sort expressions read-in phase started" << std::endl;

  ATermAppl data_spec;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = aterm_cast<aterm_appl>(spec(0));
  }
  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0));

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

  ATermAppl Result;

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of data expression read-in phase started" << std::endl;

  ATermAppl data_spec;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = aterm_cast<aterm_appl>(spec(0));
  }
  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0));
  ATermList constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0));
  ATermList mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0));

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
      if (aterm()!=Type && !data::is_unknown_sort(data::sort_expression(Type)))
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
  ATermAppl Result;

  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of multiactions read-in phase started" << std::endl;

  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0));
  ATermList constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0));
  ATermList mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0));

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
      ATermList r;
      for (ATermList l=aterm_cast<aterm_list>(mult_act(0)); !l.empty(); l=l.tail())
      {
        ATermAppl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        if (aterm()==o)
        {
          gstcDataDestroy();
          return ATermAppl();
          // return NULL;
        }
        r=push_front<aterm>(r,o);
      }
      Result=mult_act.set_argument(reverse(r),0);
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

  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0));
  ATermList constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0));
  ATermList mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0));

  //XXX read-in from spec (not finished)
  ATermList result;
  if (gstcReadInSorts(sorts)
      && gstcReadInConstructors()
      && gstcReadInFuncs(constructors,mappings)
      && gstcReadInActs(action_labels))
  {
    mCRL2log(debug) << "type checking of multiactions read-in phase finished" << std::endl;

    for (; !mult_actions.empty(); mult_actions=mult_actions.tail())
    {
      ATermTable Vars=ATtableCreate(63,50);
      ATermList r;

      for (ATermList l=(ATermList)(mult_actions.front()) ; !l.empty(); l=l.tail())
      {
        ATermAppl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        if (aterm()==o)
        {
          throw mcrl2::runtime_error("Typechecking action failed: "+ core::pp_deprecated(ATAgetFirst(l)));
        }
        r=push_front<aterm>(r,o);
      }
      result = push_front<aterm>(result,reverse(r));
    }
  }
  else
  {
    throw mcrl2::runtime_error("reading data/action specification failed");
  }
  gstcDataDestroy();
  return reverse(result);
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

  ATermAppl Result;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of state formulas read-in phase started" << std::endl;

  ATermAppl data_spec = aterm_cast<aterm_appl>(spec(0));
  ATermList action_labels = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec(1))(0));

  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0));
  ATermList constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0));
  ATermList mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0));

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

  ATermAppl Result;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of action rename specification read-in phase started" << std::endl;

  ATermTable actions_from_lps=ATtableCreate(63,50);

  ATermAppl lps_data_spec = aterm_cast<aterm_appl>(lps_spec(0));
  ATermList lps_sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_data_spec(0))(0));
  ATermList lps_constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_data_spec(1))(0));
  ATermList lps_mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_data_spec(2))(0));
  ATermList lps_action_labels = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_spec(1))(0));

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

        ATermAppl data_spec = aterm_cast<aterm_appl>(ar_spec(0));
        ATermList LPSSorts=ATtableKeys(context.defined_sorts); // remember the sorts from the LPS.
        if (!gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0))))
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

        if (!gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0)),
                             aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0))))
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file functions finished" << std::endl;

        body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(3))(0));

        //Save the actions from LPS only for the latter use.
        gstcATermTableCopy(context.actions,actions_from_lps);
        if (!gstcReadInActs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(ar_spec(1))(0))))
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking action rename specification read-in phase of the ActionRenameSpec finished" << std::endl;

        if (!gstcTransformVarConsTypeData())
        {
          goto finally;
        }
        mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;

        data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
        Result=ar_spec.set_argument(data_spec,0);
        Result=gstcFoldSortRefs(Result);


        // now the action renaming rules themselves.
        ATermAppl ActionRenameRules=aterm_cast<aterm_appl>(ar_spec(2));
        ATermList NewRules;

        ATermTable DeclaredVars=ATtableCreate(63,50);
        ATermTable FreeVars=ATtableCreate(63,50);

        bool b = true;

        for (ATermList l=aterm_cast<aterm_list>(ActionRenameRules(0)); !l.empty(); l=l.tail())
        {
          ATermAppl Rule=ATAgetFirst(l);
          assert(gsIsActionRenameRule(Rule));

          ATermList VarList=aterm_cast<aterm_list>(Rule(0));
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

          ATermAppl Left=aterm_cast<aterm_appl>(Rule(2));
          assert(gsIsParamId(Left));
          {
            //extra check requested by Tom: actions in the LHS can only come from the LPS
            ATermTable temp=context.actions;
            context.actions=actions_from_lps;
            Left=gstcTraverseActProcVarConstP(DeclaredVars,Left);
            context.actions=temp;
            if (aterm()==Left)
            {
              b = false;
              break;
            }
          }

          ATermAppl Cond=aterm_cast<aterm_appl>(Rule(1));
          if (aterm()==(gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Cond,sort_bool::bool_())))
          {
            b = false;  // JK 15/10/2009 remove gsIsNil check
            break;
          }

          ATermAppl Right=aterm_cast<aterm_appl>(Rule(3));
          assert(gsIsParamId(Right) || gsIsTau(Right) || gsIsDelta(Right));
          Right=gstcTraverseActProcVarConstP(DeclaredVars,Right);
          if (aterm()==Right)
          {
            b = false;
            break;
          }

          NewRules=push_front<aterm>(NewRules,gsMakeActionRenameRule(VarList,Cond,Left,Right));
        }
        if (!b)
        {
          Result = aterm_appl();
          goto finally;
        }

        ActionRenameRules=ActionRenameRules.set_argument(reverse(NewRules),0);
        Result=Result.set_argument(ActionRenameRules,2);
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
  gstcDataDestroy();
  return Result;
}

ATermAppl type_check_pbes_spec(ATermAppl pbes_spec)
{
  //check correctness of the PBES specification in pbes_spec

  mCRL2log(verbose) << "type checking PBES specification..." << std::endl;

  assert(gsIsPBES(pbes_spec));

  ATermAppl Result;
  mCRL2log(debug) << "type checking phase of PBES specifications started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of PBES specification read-in phase started" << std::endl;


  ATermAppl data_spec = aterm_cast<aterm_appl>(pbes_spec(0));
  ATermAppl pb_eqn_spec = aterm_cast<aterm_appl>(pbes_spec(2));
  ATermAppl pb_init = aterm_cast<aterm_appl>(pbes_spec(3));
  ATermAppl glob_var_spec = aterm_cast<aterm_appl>(pbes_spec(1));
  ATermTable dummy;

  if (!gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0))))
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

  if (!gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(1))(0)),
                       aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(2))(0))))
  {
    goto finally;
  }
  mCRL2log(debug) << "type checking of PBES specification read-in phase of functions finished" << std::endl;

  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(3))(0));

  if (!gstcAddVars2Table(context.glob_vars, aterm_cast<aterm_list>(glob_var_spec(0)),dummy))
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

  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
  Result=pbes_spec.set_argument(data_spec,0);

  pb_eqn_spec=pb_eqn_spec.set_argument(gstcWritePBES(aterm_cast<aterm_list>(pb_eqn_spec(0))),0);
  Result=Result.set_argument(pb_eqn_spec,2);

  pb_init=pb_init.set_argument(aterm_cast<aterm_appl>(ATtableGet(body.proc_bodies,INIT_KEY())),0);
  Result=Result.set_argument(pb_init,3);

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

  ATermAppl data_spec;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = aterm_cast<aterm_appl>(spec(0));
  }
  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec(0))(0));

  //XXX read-in from spec (not finished)
  if (gstcReadInSorts(sorts))
  {
    mCRL2log(debug) << "type checking of data variables read-in phase finished" << std::endl;

    ATermTable Vars=ATtableCreate(63,50);
    ATermTable NewVars;
    if (!gstcAddVars2Table(Vars,data_vars,NewVars))
    {
      mCRL2log(error) << "type error while typechecking data variables" << std::endl;
      return aterm_list(aterm());
    }
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
  ATermList SortIds;
  ATermList SortRefs;
  while (!SortDecls.empty())
  {
    ATermAppl SortDecl = ATAgetFirst(SortDecls);
    if (gsIsSortRef(SortDecl))
    {
      SortRefs = push_front<aterm>(SortRefs, SortDecl);
    }
    else     //gsIsSortId(SortDecl)
    {
      SortIds = push_front<aterm>(SortIds, SortDecl);
    }
    SortDecls = SortDecls.tail();
  }
  *PSortIds = reverse(SortIds);
  *PSortRefs = reverse(SortRefs);
}

ATermAppl gstcUpdateSortSpec(ATermAppl Spec, ATermAppl SortSpec)
{
  assert(gsIsDataSpec(Spec) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) || gsIsPBES(Spec) || gsIsActionRenameSpec(Spec));
  assert(gsIsSortSpec(SortSpec));
  if (gsIsDataSpec(Spec))
  {
    Spec = Spec.set_argument( SortSpec, 0);
  }
  else
  {
    ATermAppl DataSpec = aterm_cast<aterm_appl>(Spec(0));
    DataSpec = DataSpec.set_argument( SortSpec, 0);
    Spec = Spec.set_argument( DataSpec, 0);
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
    DataSpec = aterm_cast<aterm_appl>(Spec(0));
  }
  ATermAppl SortSpec = aterm_cast<aterm_appl>(DataSpec(0));
  ATermList SortDecls = aterm_cast<aterm_list>(SortSpec(0));
  //split sort declarations in sort id's and sort references
  ATermList SortIds=aterm_list(aterm());
  ATermList SortRefs=aterm_list(aterm());
  gstcSplitSortDecls(SortDecls, &SortIds, &SortRefs);
  //fold sort references in the sort references themselves
  SortRefs = gstcFoldSortRefsInSortRefs(SortRefs);
  //substitute sort references in the rest of Spec, i.e.
  //(a) remove sort references from Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(SortIds));
  //(b) build substitution table
  ATermTable Substs = ATtableCreate(2*SortRefs.size(),50);
  ATermList l = SortRefs;
  while (!l.empty())
  {
    ATermAppl SortRef = ATAgetFirst(l);
    //add substitution for SortRef
    ATermAppl LHS = gsMakeSortId(aterm_cast<aterm_appl>(SortRef(0)));
    ATermAppl RHS = aterm_cast<aterm_appl>(SortRef(1));
    if (gsIsSortId(RHS) || gsIsSortArrow(RHS))
    {
      //add forward substitution
      Substs.put( LHS, RHS);
    }
    else
    {
      //add backward substitution
      Substs.put( RHS, LHS);
    }
    l = l.tail();
  }
  //(c) perform substitutions until the specification becomes stable
  ATermAppl NewSpec = Spec;
  do
  {
    mCRL2log(debug) << "substituting sort references in specification" << std::endl;
    Spec = NewSpec;
    NewSpec = (ATermAppl) gsSubstValuesTable(Substs, Spec, true);
  }
  while (NewSpec!=Spec);

  //add the removed sort references back to Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(SortIds+ SortRefs));
  mCRL2log(debug) << "specification after folding:\n" << core::pp_deprecated(Spec) << "\n" ;
  return Spec;
}

ATermList gstcFoldSortRefsInSortRefs(ATermList SortRefs)
{
  //fold sort references in SortRefs by means of repeated forward and backward
  //substitution
  ATermList NewSortRefs = SortRefs;
  size_t n = SortRefs.size();
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
      ATermAppl LHS = gsMakeSortId(aterm_cast<aterm_appl>(SortRef(0)));
      ATermAppl RHS = aterm_cast<aterm_appl>(SortRef(1));
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
      mCRL2log(debug) << "performing substition " << core::pp_deprecated(Subst(0)) << " := " << core::pp_deprecated(Subst(1)) << "" << std::endl;
      //perform Subst on all elements of NewSortRefs except for the i'th
      ATermList Substs = ATmakeList1(Subst);
      for (size_t j = 0; j < n; j++)
      {
        if (i != j)
        {
          ATermAppl OldSortRef = ATAelementAt(NewSortRefs, j);
          ATermAppl NewSortRef = gsSubstValues_Appl(Substs, OldSortRef, true);
          if (NewSortRef!=OldSortRef)
          {
            NewSortRefs = ATreplace(NewSortRefs, NewSortRef, j);
          }
        }
      }
    }
    mCRL2log(debug) << std::endl;
  }
  while (NewSortRefs!=SortRefs);
  //remove self references
  ATermList l;
  while (!SortRefs.empty())
  {
    ATermAppl SortRef = ATAgetFirst(SortRefs);
    if (gsMakeSortId(aterm_cast<aterm_appl>(SortRef(0)))!=aterm_cast<aterm_appl>(SortRef(1)))
    {
      l = push_front<aterm>(l, SortRef);
    }
    SortRefs = SortRefs.tail();
  }
  SortRefs = reverse(l);
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
  body.equations = aterm_list();

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
  ATindexedSetDestroy(context.basic_sorts);
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
  for (; !Sorts.empty(); Sorts=Sorts.tail())
  {
    ATermAppl Sort=ATAgetFirst(Sorts);
    ATermAppl SortName=aterm_cast<aterm_appl>(Sort(0));
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
        || aterm()!=aterm_cast<aterm_appl>(ATtableGet(context.defined_sorts, SortName)))
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
      context.defined_sorts.put(SortName, aterm_cast<aterm_appl>(Sort(1)));
      mCRL2log(debug) << "Add sort alias " << core::pp_deprecated(SortName) << "  " << core::pp_deprecated(aterm_cast<aterm_appl>(Sort(1))) << "" << std::endl;
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
  for (; sort_aliases!=aterm_list() ; sort_aliases=sort_aliases.tail())
  {
    std::set < basic_sort > visited;
    const basic_sort s(core::identifier_string((ATermAppl)(sort_aliases.front())));
    ATermAppl aterm_reference=(ATermAppl)ATtableGet(context.defined_sorts,
                              static_cast<ATermAppl>(s.name()));
    assert(aterm_reference!=ATermAppl());
    const sort_expression ar(aterm_reference);
    if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(ar,s,visited,false))
    {
      mCRL2log(error) << "sort " << core::pp_deprecated(sort_aliases.front()) << " is recursively defined via a function sort, or a set or a bag type container" << std::endl;
      return false;
    }
  }

  return Result;
}

static bool gstcReadInConstructors(ATermList NewSorts)
{
  ATermList Sorts=NewSorts;
  if (aterm()==Sorts)
  {
    Sorts=ATtableKeys(context.defined_sorts);
  }
  for (; !Sorts.empty(); Sorts=Sorts.tail())
  {
    ATermAppl SortExpr=aterm_cast<aterm_appl>(ATtableGet(context.defined_sorts,Sorts.front()));
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
  for (ATermList sort_walker=ATtableKeys(context.defined_sorts);  sort_walker!=aterm_list(); sort_walker=sort_walker.tail())
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
    for (; defined_sorts!=aterm_list(); defined_sorts=defined_sorts.tail())
    {
      const basic_sort s(core::identifier_string(gstcUnwindType(ATAgetFirst(defined_sorts))));
      ATermAppl reference=aterm_cast<aterm_appl>(ATtableGet(context.defined_sorts,static_cast<ATermAppl>(s.name())));
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
          constructor_list=push_front<aterm>(constructor_list,static_cast<ATermAppl>(*j));
        }
      }

      if (is_structured_sort(*i))
      {
        const function_symbol_vector r=structured_sort(*i).constructor_functions();
        for (function_symbol_vector::const_iterator i=r.begin();
             i!=r.end(); ++i)
        {
          constructor_list=push_front<aterm>(constructor_list,static_cast<ATermAppl>(*i));
        }
      }

    }

    std::set < sort_expression > possibly_empty_constructor_sorts;
    for (ATermList constructor_list_walker=constructor_list;
         constructor_list_walker!=aterm_list(); constructor_list_walker=constructor_list_walker.tail())
    {
      const sort_expression s=data::function_symbol(constructor_list_walker.front()).sort();
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
           constructor_list_walker!=aterm_list(); constructor_list_walker=constructor_list_walker.tail())
      {
        const sort_expression s=data::function_symbol(constructor_list_walker.front()).sort();
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

  size_t constr_number=Cons.size();
  for (ATermList Funcs=Cons+Maps; !Funcs.empty(); Funcs=Funcs.tail())
  {
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=aterm_cast<aterm_appl>(Func(0));
    ATermAppl FuncType=aterm_cast<aterm_appl>(Func(1));

    if (!gstcIsSortExprDeclared(FuncType))
    {
      return false;
    }

    //if FuncType is a defined function sort, unwind it
    //{ ATermAppl NewFuncType;
    //  if(gsIsSortId(FuncType)
    //   && (NewFuncType=aterm_cast<aterm_appl>(ATtableGet(context.defined_sorts,aterm_cast<aterm_appl>(FuncType(0)))))
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
        ConstructorType=aterm_cast<aterm_appl>(ConstructorType(1));
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
  for (; !Acts.empty(); Acts=Acts.tail())
  {
    ATermAppl Act=ATAgetFirst(Acts);
    ATermAppl ActName=aterm_cast<aterm_appl>(Act(0));
    ATermList ActType=aterm_cast<aterm_list>(Act(1));

    if (!gstcIsSortExprListDeclared(ActType))
    {
      return false;
    }

    ATermList Types=aterm_cast<aterm_list>(ATtableGet(context.actions, ActName));
    if (aterm()==Types)
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
        Types=Types+push_front<aterm>(aterm_list(),ActType);
      }
    }
    context.actions.put(ActName,Types);
  }

  return Result;
}

static bool gstcReadInProcsAndInit(ATermList Procs, ATermAppl Init)
{
  bool Result=true;
  for (; !Procs.empty(); Procs=Procs.tail())
  {
    ATermAppl Proc=ATAgetFirst(Procs);
    ATermAppl ProcName=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(Proc(0))(0));

    if (aterm()!=aterm_cast<aterm_list>(ATtableGet(context.actions, ProcName)))
    {
      mCRL2log(error) << "declaration of both process and action " << core::pp_deprecated(ProcName) << std::endl;
      return false;
    }

    ATermList ProcType=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Proc(0))(1));

    if (!gstcIsSortExprListDeclared(ProcType))
    {
      return false;
    }

    ATermList Types=aterm_cast<aterm_list>(ATtableGet(context.processes,ProcName));
    if (aterm()==Types)
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
        Types=Types+push_front<aterm>(aterm_list(),ProcType);
      }
    }
    context.processes.put(ProcName,Types);

    //check that all formal parameters of the process are unique.
    ATermList ProcVars=aterm_cast<aterm_list>(Proc(1));
    if (!gstcVarsUnique(ProcVars))
    {
      mCRL2log(error) << "the formal variables in process " << core::pp_deprecated(Proc) << " are not unique" << std::endl;
      return false;
    }

    body.proc_pars.put(aterm_cast<aterm_appl>(Proc(0)),aterm_cast<aterm_list>(Proc(1)));
    body.proc_bodies.put(aterm_cast<aterm_appl>(Proc(0)),aterm_cast<aterm_appl>(Proc(2)));
  }
  body.proc_pars.put(INIT_KEY(),aterm_list());
  body.proc_bodies.put(INIT_KEY(),Init);

  return Result;
}

static bool gstcReadInPBESAndInit(ATermAppl PBEqnSpec, ATermAppl PBInit)
{
  bool Result=true;

  ATermList PBEqns=aterm_cast<aterm_list>(PBEqnSpec(0));

  for (; !PBEqns.empty(); PBEqns=PBEqns.tail())
  {
    ATermAppl PBEqn=ATAgetFirst(PBEqns);
    ATermAppl PBName=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(PBEqn(1))(0));

    ATermList PBVars=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(PBEqn(1))(1));

    ATermList PBType;
    for (ATermList l=PBVars; !l.empty(); l=l.tail())
    {
      PBType=push_front<aterm>(PBType,aterm_cast<aterm_appl>(ATAgetFirst(l)(1)));
    }
    PBType=reverse(PBType);

    if (!gstcIsSortExprListDeclared(PBType))
    {
      return false;
    }

    ATermList Types=aterm_cast<aterm_list>(ATtableGet(context.PBs,PBName));
    if (aterm()==Types)
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
        Types=Types+push_front<aterm>(aterm_list(),PBType);
      }
    }
    context.PBs.put(PBName,Types);

    //This is a fake ProcVarId (There is no PBVarId)
    ATermAppl Index=gsMakeProcVarId(PBName,PBType);
    //body.proc_freevars.put(Index,PBFreeVars);
    body.proc_pars.put(Index,PBVars);
    body.proc_bodies.put(Index,aterm_cast<aterm_appl>(PBEqn(2)));
  }
  //body.proc_freevars.put((ATerm)INIT_KEY(),(ATerm)PBFreeVars);
  body.proc_pars.put(INIT_KEY(),aterm_list());
  body.proc_bodies.put(INIT_KEY(),aterm_cast<aterm_appl>(PBInit(0)));

  return Result;
}

static ATermList gstcWriteProcs(ATermList oldprocs)
{
  ATermList Result;
  for (ATermList l=oldprocs; !l.empty(); l=l.tail())
  {
    ATermAppl ProcVar=aterm_cast<aterm_appl>(ATAgetFirst(l)(0));
    if (ProcVar==INIT_KEY())
    {
      continue;
    }
    Result=push_front<aterm>(Result,gsMakeProcEqn(ProcVar,
                    aterm_cast<aterm_list>(ATtableGet(body.proc_pars,ProcVar)),
                    aterm_cast<aterm_appl>(ATtableGet(body.proc_bodies,ProcVar))
                                               )
                   );
  }
  Result=reverse(Result);
  return Result;
}

static ATermList gstcWritePBES(ATermList oldPBES)
{
  ATermList Result;
  for (ATermList PBEqns=oldPBES; !PBEqns.empty(); PBEqns=PBEqns.tail())
  {
    ATermAppl PBEqn=ATAgetFirst(PBEqns);
    ATermAppl PBESVar=aterm_cast<aterm_appl>(PBEqn(1));

    ATermList PBType;
    for (ATermList l=aterm_cast<aterm_list>(PBESVar(1)); !l.empty(); l=l.tail())
    {
      PBType=push_front<aterm>(PBType,aterm_cast<aterm_appl>(ATAgetFirst(l)(1)));
    }
    PBType=reverse(PBType);

    ATermAppl Index=gsMakeProcVarId(aterm_cast<aterm_appl>(PBESVar(0)),PBType);

    if (Index==INIT_KEY())
    {
      continue;
    }
    Result=push_front<aterm>(Result,PBEqn.set_argument(aterm_cast<aterm_appl>(ATtableGet(body.proc_bodies,Index)),2));
  }
  return reverse(Result);
}


static bool gstcTransformVarConsTypeData(void)
{
  bool Result=true;
  ATermTable DeclaredVars=ATtableCreate(63,50);
  ATermTable FreeVars=ATtableCreate(63,50);

  //data terms in equations
  ATermList NewEqns;
  bool b = true;
  for (ATermList Eqns=body.equations; !Eqns.empty(); Eqns=Eqns.tail())
  {
    ATermAppl Eqn=ATAgetFirst(Eqns);
    ATermList VarList=aterm_cast<aterm_list>(Eqn(0));

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

    ATermAppl Left=aterm_cast<aterm_appl>(Eqn(2));
    ATermAppl LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,data::unknown_sort(),FreeVars,false,true);
    if (aterm()==LeftType)
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

    ATermAppl Cond=aterm_cast<aterm_appl>(Eqn(1));
    if (aterm()==gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Cond,sort_bool::bool_()))
    {
      b = false;
      break;
    }
    ATermAppl Right=aterm_cast<aterm_appl>(Eqn(3));
    ATermTable dummy_empty_table;
    ATermAppl RightType=gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,&Right,LeftType,dummy_empty_table,false);
    if (aterm()==RightType)
    {
      b = false;
      mCRL2log(error) << "error occurred while typechecking " << core::pp_deprecated(Right) << " as right hand side of equation " << core::pp_deprecated(Eqn) << std::endl;
      break;
    }

    //If the types are not uniquely the same now: do once more:
    if (!gstcEqTypesA(LeftType,RightType))
    {
      ATermAppl Type=gstcTypeMatchA(LeftType,RightType);
      if (aterm()==Type)
      {
        mCRL2log(error) << "types of the left- (" << core::pp_deprecated(LeftType) << ") and right- (" << core::pp_deprecated(RightType) << ") hand-sides of the equation " << core::pp_deprecated(Eqn) << " do not match" << std::endl;
        b = false;
        break;
      }
      Left=aterm_cast<aterm_appl>(Eqn(2));
      FreeVars.reset();
      LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Left,Type,FreeVars,true);
      if (aterm()==LeftType)
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
      Right=aterm_cast<aterm_appl>(Eqn(3));
      RightType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,&Right,LeftType,FreeVars);
      if (aterm()==RightType)
      {
        b = false;
        mCRL2log(error) << "types of the left- and right-hand-sides of the equation " << core::pp_deprecated(Eqn) << " do not match" << std::endl;
        break;
      }
      Type=gstcTypeMatchA(LeftType,RightType);
      if (aterm()==Type)
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
    DeclaredVars.reset();
    NewEqns=push_front<aterm>(NewEqns,gsMakeDataEqn(VarList,Cond,Left,Right));
  }
  if (b)
  {
    body.equations=reverse(NewEqns);
  }

  return b?Result:false;
}

static bool gstcTransformActProcVarConst(void)
{
  bool Result=true;
  ATermTable Vars=ATtableCreate(63,50);

  //process and data terms in processes and init
  for (ATermList ProcVars=ATtableKeys(body.proc_pars); !ProcVars.empty(); ProcVars=ProcVars.tail())
  {
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    Vars.reset();
    gstcATermTableCopy(context.glob_vars,Vars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(Vars,aterm_cast<aterm_list>(ATtableGet(body.proc_pars,ProcVar)),NewVars))
    {
      Result = false;
      break;
    }
    else
    {
      Vars=NewVars;
    }

    ATermAppl NewProcTerm=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ATtableGet(body.proc_bodies,ProcVar)));
    if (aterm()==NewProcTerm)
    {
      Result = false;
      break;
    }
    body.proc_bodies.put(ProcVar,NewProcTerm);
  }

  return Result;
}

static bool gstcTransformPBESVarConst(void)
{
  bool Result=true;
  ATermTable Vars=ATtableCreate(63,50);

  //PBEs and data terms in PBEqns and init
  for (ATermList PBVars=ATtableKeys(body.proc_pars); !PBVars.empty(); PBVars=PBVars.tail())
  {
    ATermAppl PBVar=ATAgetFirst(PBVars);
    Vars.reset();
    gstcATermTableCopy(context.glob_vars,Vars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(Vars,aterm_cast<aterm_list>(ATtableGet(body.proc_pars,PBVar)),NewVars))
    {
      Result = false;
      break;
    }
    else
    {
      Vars=NewVars;
    }

    ATermAppl NewPBTerm=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(ATtableGet(body.proc_bodies,PBVar)));
    if (aterm()==NewPBTerm)
    {
      Result = false;
      break;
    }
    body.proc_bodies.put(PBVar,NewPBTerm);
  }

  return Result;
}


// ======== Auxiliary functions
static bool gstcInTypesA(ATermAppl Type, ATermList Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (gstcEqTypesA(Type,ATAgetFirst(Types)))
    {
      return true;
    }
  return false;
}

static bool gstcEqTypesA(ATermAppl Type1, ATermAppl Type2)
{
  if (Type1==Type2)
  {
    return true;
  }

  if (aterm()==Type1 || aterm()==Type2)
  {
    return false;
  }

  return gstcUnwindType(Type1)==gstcUnwindType(Type2);
}

static bool gstcInTypesL(ATermList Type, ATermList Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (gstcEqTypesL(Type,ATLgetFirst(Types)))
    {
      return true;
    }
  return false;
}

static bool gstcEqTypesL(ATermList Type1, ATermList Type2)
{
  if (Type1==Type2)
  {
    return true;
  }
  if (aterm()==Type1 || aterm()==Type2)
  {
    return false;
  }
  if (Type1.size()!=Type2.size())
  {
    return false;
  }
  for (; !Type1.empty(); Type1=Type1.tail(),Type2=Type2.tail())
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
  if (aterm()!=aterm_cast<aterm_appl>(ATtableGet(context.defined_sorts,SortName)))
  {
    return true;
  }
  return false;
}

static bool gstcIsSortExprDeclared(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr))
  {
    ATermAppl SortName=aterm_cast<aterm_appl>(SortExpr(0));
    if (!gstcIsSortDeclared(SortName))
    {
      mCRL2log(error) << "basic or defined sort " << core::pp_deprecated(SortName) << " is not declared" << std::endl;
      return false;
    }
    return true;
  }

  if (gsIsSortCons(SortExpr))
  {
    return gstcIsSortExprDeclared(aterm_cast<aterm_appl>(SortExpr(1)));
  }

  if (gsIsSortArrow(SortExpr))
  {
    if (!gstcIsSortExprDeclared(aterm_cast<aterm_appl>(SortExpr(1))))
    {
      return false;
    }
    if (!gstcIsSortExprListDeclared(aterm_cast<aterm_list>(SortExpr(0))))
    {
      return false;
    }
    return true;
  }

  if (gsIsSortStruct(SortExpr))
  {
    for (ATermList Constrs=aterm_cast<aterm_list>(SortExpr(0)); !Constrs.empty(); Constrs=Constrs.tail())
    {
      ATermAppl Constr=ATAgetFirst(Constrs);

      ATermList Projs=aterm_cast<aterm_list>(Constr(1));
      for (; !Projs.empty(); Projs=Projs.tail())
      {
        ATermAppl Proj=ATAgetFirst(Projs);
        ATermAppl ProjSort=aterm_cast<aterm_appl>(Proj(1));

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
  for (; !SortExprList.empty(); SortExprList=SortExprList.tail())
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
    ATermAppl SortName=aterm_cast<aterm_appl>(SortExpr(0));
    if (!gstcIsSortDeclared(SortName))
    {
      mCRL2log(error) << "basic or defined sort " << core::pp_deprecated(SortName) << " is not declared" << std::endl;
      return false;
    }
    return true;
  }

  if (gsIsSortCons(SortExpr))
  {
    return gstcReadInSortStruct(aterm_cast<aterm_appl>(SortExpr(1)));
  }

  if (gsIsSortArrow(SortExpr))
  {
    if (!gstcReadInSortStruct(aterm_cast<aterm_appl>(SortExpr(1))))
    {
      return false;
    }
    for (ATermList Sorts=aterm_cast<aterm_list>(SortExpr(0)); !Sorts.empty(); Sorts=Sorts.tail())
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
    for (ATermList Constrs=aterm_cast<aterm_list>(SortExpr(0)); !Constrs.empty(); Constrs=Constrs.tail())
    {
      ATermAppl Constr=ATAgetFirst(Constrs);

      // recognizer -- if present -- a function from SortExpr to Bool
      ATermAppl Name=aterm_cast<aterm_appl>(Constr(2));
      if (!gsIsNil(Name) &&
          !gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(ATmakeList1(SortExpr),sort_bool::bool_())),"recognizer"))
      {
        return false;
      }

      // constructor type and projections
      ATermList Projs=aterm_cast<aterm_list>(Constr(1));
      Name=aterm_cast<aterm_appl>(Constr(0));
      if (Projs.empty())
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

      ATermList ConstructorType;
      for (; !Projs.empty(); Projs=Projs.tail())
      {
        ATermAppl Proj=ATAgetFirst(Projs);
        ATermAppl ProjSort=aterm_cast<aterm_appl>(Proj(1));

        // not to forget, recursive call for ProjSort ;-)
        if (!gstcReadInSortStruct(ProjSort))
        {
          return false;
        }

        ATermAppl ProjName=aterm_cast<aterm_appl>(Proj(0));
        if (!gsIsNil(ProjName) &&
            !gstcAddFunction(gsMakeOpId(ProjName,gsMakeSortArrow(ATmakeList1(SortExpr),ProjSort)),"projection",true))
        {
          return false;
        }
        ConstructorType=push_front<aterm>(ConstructorType,ProjSort);
      }
      if (!gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(reverse(ConstructorType),SortExpr)),"constructor"))
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

  if (aterm()!=aterm_cast<aterm_appl>(ATtableGet(context.constants, Name)) /*|| aterm_cast<aterm_list>(ATtableGet(context.functions, (ATerm)Name))*/)
  {
    mCRL2log(error) << "double declaration of " << msg << " " << core::pp_deprecated(Name) << std::endl;
    return false;
  }

  if (aterm()!=aterm_cast<aterm_list>(ATtableGet(gssystem.constants, Name)) || aterm()!=aterm_cast<aterm_list>(ATtableGet(gssystem.functions, Name)))
  {
    mCRL2log(error) << "attempt to declare a constant with the name that is a built-in identifier (" << core::pp_deprecated(Name) << ")" << std::endl;
    return false;
  }

  context.constants.put( Name, Sort);
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
  //  if(aterm_cast<aterm_appl>(ATtableGet(context.constants, (ATerm)Name))){
  //    ThrowMF("Double declaration of constant and %s %T\n", msg, Name);
  //  }

  if (domain.size()==0)
  {
    if (aterm()!=aterm_cast<aterm_appl>(ATtableGet(gssystem.constants, Name)))
    {
      mCRL2log(error) << "attempt to redeclare the system constant with " << msg << " " << core::pp_deprecated(OpId) << std::endl;
      return false;
    }
  }
  else // domain.size()>0
  {
    ATermList L=aterm_cast<aterm_list>(ATtableGet(gssystem.functions, Name));
    for (; L!=ATerm() && L!=aterm_list() ; L=L.tail())
    {
      if (gstcTypeMatchA(Sort,(ATermAppl)L.front())!=ATerm())
      {
        // f matches a predefined function
        mCRL2log(error) << "attempt to redeclare a system function with " << msg << " " << core::pp_deprecated(OpId) << ":" << core::pp_deprecated(Sort) << std::endl;
        return false;
      }
    }
  }

  ATermList Types=aterm_cast<aterm_list>(ATtableGet(context.functions, Name));
  // the table context.functions contains a list of types for each
  // function name. We need to check if there is already such a type
  // in the list. If so -- error, otherwise -- add
  if ((aterm()!=Types) && gstcInTypesA(Sort, Types))
  {
    if (!allow_double_decls)
    {
      mCRL2log(error) << "double declaration of " << msg << " " << core::pp_deprecated(Name) << std::endl;
      return false;
    }
  }
  else
  {
    if (aterm()==Types)
    {
      Types=aterm_list();
    }
    Types=Types+push_front<aterm>(aterm_list(),Sort);
    context.functions.put(Name,Types);
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

  ATermList Types=aterm_cast<aterm_list>(ATtableGet(gssystem.constants, OpIdName));

  if (Types==aterm())
  {
    Types=aterm_list();
  }
  Types=Types+push_front<aterm>(aterm_list(),Type);
  gssystem.constants.put(OpIdName,Types);
}

static void gstcAddSystemFunction(ATermAppl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  assert(gsIsOpId(OpId));
  ATermAppl OpIdName = data::function_symbol(OpId).name();
  ATermAppl Type = data::function_symbol(OpId).sort();
  assert(gsIsSortArrow(Type));

  ATermList Types=aterm_cast<aterm_list>(ATtableGet(gssystem.functions, OpIdName));

  if (Types==aterm())
  {
    Types=aterm_list();
  }
  Types=Types+push_front<aterm>(aterm_list(),Type);  // TODO: Avoid concatenate but the order is essential.
  gssystem.functions.put(OpIdName,Types);
}

static void gstcATermTableCopy(const ATermTable &Orig, ATermTable &Copy)
{
  for (ATermList Keys=ATtableKeys(Orig); !Keys.empty(); Keys=Keys.tail())
  {
    ATerm Key=Keys.front();
    Copy.put(Key,ATtableGet(Orig,Key));
  }
}


static bool gstcVarsUnique(ATermList VarDecls)
{
  bool Result=true;
  ATermIndexedSet Temp=ATindexedSetCreate(63,50);

  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=aterm_cast<aterm_appl>(VarDecl(0));
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
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=aterm_cast<aterm_appl>(VarDecl(0));
    ATermAppl VarType=aterm_cast<aterm_appl>(VarDecl(1));
    //test the type
    if (!gstcIsSortExprDeclared(VarType))
    {
      return false;
    }

    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    Vars.put( VarName, VarType);
  }
  result=Vars;
  return true;
}

static ATermTable gstcRemoveVars(ATermTable &Vars, ATermList VarDecls)
{
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=aterm_cast<aterm_appl>(VarDecl(0));

    Vars.remove(VarName);
  }

  return Vars;
}

static ATermAppl gstcRewrActProc(const ATermTable &Vars, ATermAppl ProcTerm, bool is_pbes)
{
  ATermAppl Result;
  ATermAppl Name=aterm_cast<aterm_appl>(ProcTerm(0));
  ATermList ParList;

  bool action=false;

  if (!is_pbes)
  {
    if (aterm()!=(ParList=aterm_cast<aterm_list>(ATtableGet(context.actions,Name))))
    {
      action=true;
    }
    else
    {
      if (aterm()!=(ParList=aterm_cast<aterm_list>(ATtableGet(context.processes,Name))))
      {
        action=false;
      }
      else
      {
        mCRL2log(error) << "action or process " << core::pp_deprecated(Name) << " not declared" << std::endl;
        return aterm_appl();
      }
    }
  }
  else
  {
    if (aterm()==(ParList=aterm_cast<aterm_list>(ATtableGet(context.PBs,Name))))
    {
      mCRL2log(error) << "propositional variable " << core::pp_deprecated(Name) << " not declared" << std::endl;
      return aterm_appl();
    }
  }
  assert(!ParList.empty());

  size_t nFactPars=aterm_cast<aterm_list>(ProcTerm(1)).size();
  const char* msg=(is_pbes)?"propositional variable":((action)?"action":"process");

  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    ATermList NewParList;
    for (; !ParList.empty(); ParList=ParList.tail())
    {
      ATermList Par=ATLgetFirst(ParList);
      if (Par.size()==nFactPars)
      {
        NewParList=push_front<aterm>(NewParList,Par);
      }
    }
    ParList=reverse(NewParList);
  }

  if (ParList.empty())
  {
    mCRL2log(error) << "no " << msg << " " << core::pp_deprecated(Name)
                    << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"")
                    << " is declared (while typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
    return aterm_appl();
  }

  if (ParList.size()==1)
  {
    Result=gstcMakeActionOrProc(action,Name,ATLgetFirst(ParList),aterm_cast<aterm_list>(ProcTerm(1)));
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=gstcMakeActionOrProc(action,Name,gstcGetNotInferredList(ParList),aterm_cast<aterm_list>(ProcTerm(1)));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  ATermList PosTypeList=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Result(0))(1));

  ATermList NewPars;
  ATermList NewPosTypeList;
  for (ATermList Pars=aterm_cast<aterm_list>(ProcTerm(1)); !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
  {
    ATermAppl Par=ATAgetFirst(Pars);
    ATermAppl PosType=ATAgetFirst(PosTypeList);

    ATermAppl NewPosType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,PosType); //gstcExpandNumTypesDown(PosType));

    if (NewPosType==aterm())
    {
      mCRL2log(error) << "cannot typecheck " << core::pp_deprecated(Par) << " as type " << core::pp_deprecated(gstcExpandNumTypesDown(PosType)) << " (while typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      return aterm_appl();
    }
    NewPars=push_front<aterm>(NewPars,Par);
    NewPosTypeList=push_front<aterm>(NewPosTypeList,NewPosType);
  }
  NewPars=reverse(NewPars);
  NewPosTypeList=reverse(NewPosTypeList);

  PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);

  if (PosTypeList==aterm())
  {
    PosTypeList=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Result(0))(1));
    ATermList Pars=NewPars;
    NewPars=aterm_list();
    ATermList CastedPosTypeList=aterm_list();
    for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
    {
      ATermAppl Par=ATAgetFirst(Pars);
      ATermAppl PosType=ATAgetFirst(PosTypeList);
      ATermAppl NewPosType=ATAgetFirst(NewPosTypeList);

      ATermAppl CastedNewPosType=gstcUpCastNumericType(PosType,NewPosType,&Par);
      if (CastedNewPosType==aterm())
      {
        mCRL2log(error) << "cannot cast " << core::pp_deprecated(NewPosType) << " to "
                        << core::pp_deprecated(PosType) << "(while typechecking " << core::pp_deprecated(Par)
                        << " in " << core::pp_deprecated(ProcTerm) << std::endl;
        return aterm_appl();
      }

      NewPars=push_front<aterm>(NewPars,Par);
      CastedPosTypeList=push_front<aterm>(CastedPosTypeList,CastedNewPosType);
    }
    NewPars=reverse(NewPars);
    NewPosTypeList=reverse(CastedPosTypeList);

    PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);
  }

  if (PosTypeList==aterm())
  {
    mCRL2log(error) << "no " << msg << " " << core::pp_deprecated(Name) << "with type "
                    << core::pp_deprecated(NewPosTypeList) << " is declared (while typechecking "
                    << core::pp_deprecated(ProcTerm) << ")" << std::endl;
    return aterm_appl();
  }

  if (gstcIsNotInferredL(PosTypeList))
  {
    mCRL2log(error) << "ambiguous " << msg << " " << core::pp_deprecated(Name) << std::endl;
    return aterm_appl();
  }

  Result=gstcMakeActionOrProc(action,Name,PosTypeList,NewPars);

  if (is_pbes)
  {
    Result=ProcTerm.set_argument(NewPars,1);
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
  ATermAppl Result;
  size_t n = ProcTerm.size();
  if (n==0)
  {
    return ProcTerm;
  }

  //Here the code for short-hand assignments begins.
  if (gsIsIdAssignment(ProcTerm))
  {
    mCRL2log(debug) << "typechecking a process call with short-hand assignments " << ProcTerm << "" << std::endl;
    ATermAppl Name=aterm_cast<aterm_appl>(ProcTerm(0));
    ATermList ParList=aterm_cast<aterm_list>(ATtableGet(context.processes,Name));
    if (ParList==aterm())
    {
      mCRL2log(error) << "process " << core::pp_deprecated(Name) << " not declared" << std::endl;
      return aterm_appl();
    }

    // Put the assignments into a table
    ATermTable As=ATtableCreate(63,50);
    for (ATermList l=aterm_cast<aterm_list>(ProcTerm(1)); !l.empty(); l=l.tail())
    {
      ATermAppl a=ATAgetFirst(l);
      ATerm existing_rhs = ATtableGet(As,aterm_cast<aterm_appl>(a(0)));
      if (existing_rhs == aterm()) // An assignment of the shape x:=t does not yet exist, this is OK.
      {
        As.put(aterm_cast<aterm_appl>(a(0)),aterm_cast<aterm_appl>(a(1)));
      }
      else
      {
        mCRL2log(error) << "Double assignment to variable " << core::pp_deprecated(aterm_cast<aterm_appl>(a(0)))
                        << " (detected assigned values are " << core::pp_deprecated(existing_rhs)
                        << " and " << core::pp_deprecated(aterm_cast<aterm_appl>(a(1))) << ")" << std::endl;
        return aterm_appl();
      }
    }

    {
      // Now filter the ParList to contain only the processes with parameters in this process call with assignments
      ATermList NewParList;
      assert(!ParList.empty());
      ATermAppl Culprit; // Variable used for more intelligible error messages.
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        ATermList Par=ATLgetFirst(ParList);

        // get the formal parameter names
        ATermList FormalPars=aterm_cast<aterm_list>(ATtableGet(body.proc_pars,gsMakeProcVarId(Name,Par)));
        // we only need the names of the parameters, not the types
        ATermList FormalParNames;
        for (; !FormalPars.empty(); FormalPars=FormalPars.tail())
        {
          FormalParNames=push_front<aterm>(FormalParNames,aterm_cast<aterm_appl>(ATAgetFirst(FormalPars)(0)));
        }

        ATermList l=list_minus(ATtableKeys(As),FormalParNames);
        if (l.empty())
        {
          NewParList=push_front<aterm>(NewParList,Par);
        }
        else
        {
          Culprit=ATAgetFirst(l);
        }
      }
      ParList=reverse(NewParList);

      if (ParList.empty())
      {
        mCRL2log(error) << "no process " << core::pp_deprecated(Name) << " containing all assignments in "
                        << core::pp_deprecated(ProcTerm) << "." << std::endl
                        << "Problematic variable is " << core::pp_deprecated(Culprit) << "." << std::endl;
        return aterm_appl();
      }
      if (!ParList.tail().empty())
      {
        mCRL2log(error) << "ambiguous process " << core::pp_deprecated(Name) << " containing all assignments in " << core::pp_deprecated(ProcTerm) << "." << std::endl;
        return aterm_appl();
      }
    }

    // get the formal parameter names
    ATermList ActualPars;
    ATermList FormalPars=aterm_cast<aterm_list>(ATtableGet(body.proc_pars,gsMakeProcVarId(Name,ATLgetFirst(ParList))));
    {
      // we only need the names of the parameters, not the types
      for (ATermList l=FormalPars; !l.empty(); l= l.tail())
      {
        ATermAppl FormalParName=aterm_cast<aterm_appl>(ATAgetFirst(l)(0));
        ATermAppl ActualPar=aterm_cast<aterm_appl>(ATtableGet(As,FormalParName));
        if (ActualPar==aterm())
        {
          ActualPar=gsMakeId(FormalParName);
        }
        ActualPars=push_front<aterm>(ActualPars,ActualPar);
      }
      ActualPars=reverse(ActualPars);
    }

    ATermAppl TypeCheckedProcTerm=gstcRewrActProc(Vars, gsMakeParamId(Name,ActualPars));
    if (TypeCheckedProcTerm==aterm())
    {
      mCRL2log(error) << "type error occurred while typechecking the process call with short-hand assignments " << core::pp_deprecated(ProcTerm) << std::endl;
      return aterm_appl();
    }

    //reverse the assignments
    As.reset();
    for (ATermList l=aterm_cast<aterm_list>(TypeCheckedProcTerm(1)),m=FormalPars; !l.empty(); l=l.tail(),m=m.tail())
    {
      ATermAppl act_par=ATAgetFirst(l);
      ATermAppl form_par=ATAgetFirst(m);
      if (form_par==act_par)
      {
        continue;  //parameter does not change
      }
      As.put(aterm_cast<aterm_appl>(form_par(0)),gsMakeDataVarIdInit(form_par,act_par));
    }

    ATermList TypedAssignments;
    for (ATermList l=aterm_cast<aterm_list>(ProcTerm(1)); !l.empty(); l=l.tail())
    {
      ATermAppl a=ATAgetFirst(l);
      a=aterm_cast<aterm_appl>(ATtableGet(As,aterm_cast<aterm_appl>(a(0))));
      if (a==aterm())
      {
        continue;
      }
      TypedAssignments=push_front<aterm>(TypedAssignments,a);
    }
    TypedAssignments=reverse(TypedAssignments);

    TypeCheckedProcTerm=gsMakeProcessAssignment(aterm_cast<aterm_appl>(TypeCheckedProcTerm(0)),TypedAssignments);

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
      ATermList ActList=aterm_cast<aterm_list>(ProcTerm(0));
      if (ActList.empty())
      {
        mCRL2log(warning) << msg << " empty set of actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }

      ATermIndexedSet Acts=ATindexedSetCreate(63,50);
      for (; !ActList.empty(); ActList=ActList.tail())
      {
        ATermAppl Act=ATAgetFirst(ActList);

        //Actions must be declared
        if (ATtableGet(context.actions,Act)==aterm())
        {
          mCRL2log(error) << msg << " an undefined action " << core::pp_deprecated(Act) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return aterm_appl();
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
      ATermList RenList=aterm_cast<aterm_list>(ProcTerm(0));

      if (RenList.empty())
      {
        mCRL2log(warning) << "renaming empty set of actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }

      ATermIndexedSet ActsFrom=ATindexedSetCreate(63,50);

      for (; !RenList.empty(); RenList=RenList.tail())
      {
        ATermAppl Ren=ATAgetFirst(RenList);
        ATermAppl ActFrom=aterm_cast<aterm_appl>(Ren(0));
        ATermAppl ActTo=aterm_cast<aterm_appl>(Ren(1));

        if (ActFrom==ActTo)
        {
          mCRL2log(warning) << "renaming action " << core::pp_deprecated(ActFrom) << " into itself (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
        }

        //Actions must be declared and of the same types
        ATermList TypesFrom,TypesTo;
        if (aterm()==(TypesFrom=aterm_cast<aterm_list>(ATtableGet(context.actions,ActFrom))))
        {
          mCRL2log(error) << "renaming an undefined action " << core::pp_deprecated(ActFrom) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }
        if (aterm()==(TypesTo=aterm_cast<aterm_list>(ATtableGet(context.actions,ActTo))))
        {
          mCRL2log(error) << "renaming into an undefined action " << core::pp_deprecated(ActTo) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }

        TypesTo=gstcTypeListsIntersect(TypesFrom,TypesTo);
        if (TypesTo==aterm() || TypesTo.empty())
        {
          mCRL2log(error) << "renaming action " << core::pp_deprecated(ActFrom) << " into action " << core::pp_deprecated(ActTo) << ": these two have no common type (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }

        bool nnew;
        ATindexedSetPut(ActsFrom,ActFrom,&nnew);
        if (!nnew)
        {
          mCRL2log(error) << "renaming action " << core::pp_deprecated(ActFrom) << " twice (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }
      }
      ATindexedSetDestroy(ActsFrom);
    }

    //comm: like renaming multiactions (with the same parameters) to action/tau
    if (gsIsComm(ProcTerm))
    {
      ATermList CommList=aterm_cast<aterm_list>(ProcTerm(0));

      if (CommList.empty())
      {
        mCRL2log(warning) << "synchronizing empty set of (multi)actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }
      else
      {
        ATermList ActsFrom;

        for (; !CommList.empty(); CommList=CommList.tail())
        {
          ATermAppl Comm=ATAgetFirst(CommList);
          ATermList MActFrom=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Comm(0))(0));
          ATermList BackupMActFrom=MActFrom;
          assert(!MActFrom.empty());
          ATermAppl ActTo=aterm_cast<aterm_appl>(Comm(1));

          if (MActFrom.size()==1)
          {
            mCRL2log(error) << "using synchronization as renaming/hiding of action " << core::pp_deprecated(MActFrom.front())
                            << " into " << core::pp_deprecated(ActTo) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
            return aterm_appl();
          }

          //Actions must be declared
          ATermList ResTypes;

          if (!gsIsNil(ActTo))
          {
            ResTypes=aterm_cast<aterm_list>(ATtableGet(context.actions,ActTo));
            if (ResTypes==aterm())
            {
              mCRL2log(error) << "synchronizing to an undefined action " << core::pp_deprecated(ActTo) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
          }

          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            ATermAppl Act=ATAgetFirst(MActFrom);
            ATermList Types=aterm_cast<aterm_list>(ATtableGet(context.actions,Act));
            if (Types==aterm())
            {
              mCRL2log(error) << "synchronizing an undefined action " << core::pp_deprecated(Act) << " in (multi)action " << core::pp_deprecated(MActFrom) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
            ResTypes=(ResTypes!=aterm())?gstcTypeListsIntersect(ResTypes,Types):Types;
            if (ResTypes==aterm() || ResTypes.empty())
            {
              mCRL2log(error) << "synchronizing action " << core::pp_deprecated(Act) << " from (multi)action " << core::pp_deprecated(BackupMActFrom)
                              << " into action " << core::pp_deprecated(ActTo) << ": these have no common type (typechecking " << core::pp_deprecated(ProcTerm) << "), ResTypes: " << atermpp::aterm(ResTypes) << std::endl;
              return aterm_appl();
            }
          }
          MActFrom=BackupMActFrom;

          //the multiactions in the lhss of comm should not intersect.
          //make the list of unique actions
          ATermList Acts;
          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            ATermAppl Act=ATAgetFirst(MActFrom);
            if (std::find(Acts.begin(),Acts.end(),Act)==Acts.end())
            {
              Acts=push_front<aterm>(Acts,Act);
            }
          }
          for (; !Acts.empty(); Acts=Acts.tail())
          {
            ATermAppl Act=ATAgetFirst(Acts);
            if (std::find(ActsFrom.begin(),ActsFrom.end(),Act)!=ActsFrom.end())
            {
              mCRL2log(error) << "synchronizing action " << core::pp_deprecated(Act) << " in different ways (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
            else
            {
              ActsFrom=push_front<aterm>(ActsFrom,Act);
            }
          }
        }
      }
    }

    //allow
    if (gsIsAllow(ProcTerm))
    {
      ATermList MActList=aterm_cast<aterm_list>(ProcTerm(0));

      if (MActList.empty())
      {
        mCRL2log(warning) << "allowing empty set of (multi) actions (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
      }
      else
      {
        ATermList MActs;

        for (; !MActList.empty(); MActList=MActList.tail())
        {
          ATermList MAct=aterm_cast<aterm_list>(ATAgetFirst(MActList)(0));

          //Actions must be declared
          for (; !MAct.empty(); MAct=MAct.tail())
          {
            ATermAppl Act=ATAgetFirst(MAct);
            if (aterm()==aterm_cast<aterm_list>(ATtableGet(context.actions,Act)))
            {
              mCRL2log(error) << "allowing an undefined action " << core::pp_deprecated(Act) << " in (multi)action " << core::pp_deprecated(MAct) << " (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
          }

          MAct=aterm_cast<aterm_list>(ATAgetFirst(MActList)(0));
          if (gstcMActIn(MAct,MActs))
          {
            mCRL2log(warning) << "allowing (multi)action " << core::pp_deprecated(MAct) << " twice (typechecking " << core::pp_deprecated(ProcTerm) << ")" << std::endl;
          }
          else
          {
            MActs=push_front<aterm>(MActs,MAct);
          }
        }
      }
    }

    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(1)));
    if (NewProc==aterm())
    {
      return aterm_appl();
    }
    return ProcTerm.set_argument(NewProc,1);
  }

  if (gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
      gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm))
  {
    ATermAppl NewLeft=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(0)));

    if (NewLeft==aterm())
    {
      return aterm_appl();
    }
    ATermAppl NewRight=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(1)));
    if (NewRight==aterm())
    {
      return aterm_appl();
    }
    ATermAppl a=ProcTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
    return a;
  }

  if (gsIsAtTime(ProcTerm))
  {
    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(0)));
    if (NewProc==aterm())
    {
      return aterm_appl();
    }
    ATermAppl Time=aterm_cast<aterm_appl>(ProcTerm(1));
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (NewType==aterm())
    {
      return aterm_appl();
    }

    if (aterm()==gstcTypeMatchA(sort_real::real_(),NewType))
    {
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,&Time);
      if (aterm()==CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast time value " << core::pp_deprecated(Time) << " to type Real" << std::endl;
        return aterm_appl();
      }
    }

    return gsMakeAtTime(NewProc,Time);
  }

  if (gsIsIfThen(ProcTerm))
  {
    ATermAppl Cond=aterm_cast<aterm_appl>(ProcTerm(0));
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,sort_bool::bool_());
    if (aterm()==NewType)
    {
      return aterm_appl();
    }
    ATermAppl NewThen=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(1)));
    if (aterm()==NewThen)
    {
      return aterm_appl();
    }
    return gsMakeIfThen(Cond,NewThen);
  }

  if (gsIsIfThenElse(ProcTerm))
  {
    ATermAppl Cond=aterm_cast<aterm_appl>(ProcTerm(0));
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Cond,sort_bool::bool_());
    if (aterm()==NewType)
    {
      return aterm_appl();
    }
    ATermAppl NewThen=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(1)));
    if (aterm()==NewThen)
    {
      return aterm_appl();
    }
    ATermAppl NewElse=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm(2)));
    if (aterm()==NewElse)
    {
      return aterm_appl();
    }
    return gsMakeIfThenElse(Cond,NewThen,NewElse);
  }

  if (gsIsSum(ProcTerm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,aterm_cast<aterm_list>(ProcTerm(0)),NewVars))
    {
      mCRL2log(error) << "type error while typechecking " << core::pp_deprecated(ProcTerm) << std::endl;
      return aterm_appl();
    }
    ATermAppl NewProc=gstcTraverseActProcVarConstP(NewVars,aterm_cast<aterm_appl>(ProcTerm(1)));
    if (aterm()==NewProc)
    {
      mCRL2log(error) << "while typechecking " << core::pp_deprecated(ProcTerm) << std::endl;
      return aterm_appl();
    }
    return ProcTerm.set_argument(NewProc,1);
  }

  assert(0);
  return Result;
}

static ATermAppl gstcTraversePBESVarConstPB(const ATermTable &Vars, ATermAppl PBESTerm)
{
  ATermAppl Result;

  if (gsIsDataExpr(PBESTerm))
  {
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&PBESTerm,sort_bool::bool_());
    if (aterm()==NewType)
    {
      return aterm_appl();
    }
    return PBESTerm;
  }

  if (gsIsPBESTrue(PBESTerm) || gsIsPBESFalse(PBESTerm))
  {
    return PBESTerm;
  }

  if (gsIsPBESNot(PBESTerm))
  {
    ATermAppl NewArg=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm(0)));
    if (aterm()==NewArg)
    {
      return aterm_appl();
    }
    return PBESTerm.set_argument(NewArg,0);
  }

  if (gsIsPBESAnd(PBESTerm) || gsIsPBESOr(PBESTerm) || gsIsPBESImp(PBESTerm))
  {
    ATermAppl NewLeft=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm(0)));
    if (aterm()==NewLeft)
    {
      return aterm_appl();
    }
    ATermAppl NewRight=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm(1)));
    if (aterm()==NewRight)
    {
      return aterm_appl();
    }
    return PBESTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
  }

  if (gsIsPBESForall(PBESTerm)||gsIsPBESExists(PBESTerm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,aterm_cast<aterm_list>(PBESTerm(0)),NewVars))
    {
      mCRL2log(error) << "type error while typechecking " << core::pp_deprecated(PBESTerm) << std::endl;
      return aterm_appl();
    }
    ATermAppl NewPBES=gstcTraversePBESVarConstPB(NewVars,aterm_cast<aterm_appl>(PBESTerm(1)));
    if (aterm()==NewPBES)
    {
      mCRL2log(error) << "while typechecking " << core::pp_deprecated(PBESTerm) << std::endl;
      return aterm_appl();
    }
    return PBESTerm.set_argument(NewPBES,1);
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

  ATermAppl Result;

  mCRL2log(debug) << "gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(*DataTerm) <<
              " with PosType " << core::pp_deprecated(PosType) << "" << std::endl;

  if (is_abstraction(*DataTerm))
  {
    //The variable declaration of a binder should have at least 1 declaration
    if (ATAgetFirst(aterm_cast<aterm_list>((*DataTerm)(1))) == ATerm())
    {
      mCRL2log(error) << "binder " << core::pp_deprecated(*DataTerm) << " should have at least one declared variable" << std::endl;
      return aterm_appl();
    }

    ATermAppl BindingOperator = aterm_cast<aterm_appl>((*DataTerm)(0));
    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    if (gsIsSetBagComp(BindingOperator) ||
        gsIsSetComp(BindingOperator) ||
        gsIsBagComp(BindingOperator))
    {
      ATermList VarDecls=aterm_cast<aterm_list>((*DataTerm)(1));
      ATermAppl VarDecl=ATAgetFirst(VarDecls);

      //A Set/bag comprehension should have exactly one variable declared
      VarDecls=VarDecls.tail();
      if (VarDecls != aterm_list())
      {
        mCRL2log(error) << "set/bag comprehension " << core::pp_deprecated(*DataTerm) << " should have exactly one declared variable" << std::endl;
        return aterm_appl();
      }

      ATermAppl NewType=aterm_cast<aterm_appl>(VarDecl(1));
      ATermList VarList=ATmakeList1(VarDecl);
      ATermTable NewAllowedVars;
      if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
      {
        return aterm_appl();
      }
      ATermTable NewDeclaredVars;
      if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
      {
        return aterm_appl();
      }
      ATermAppl Data=aterm_cast<aterm_appl>((*DataTerm)(2));

      ATermAppl ResType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,data::unknown_sort(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);

      if (aterm()==ResType)
      {
        mCRL2log(error) << "the condition or count of a set/bag comprehension " << core::pp_deprecated(*DataTerm) << " cannot be determined\n";
        return aterm_appl();
      }
      if (aterm()!=gstcTypeMatchA(sort_bool::bool_(),ResType))
      {
        NewType=sort_set::set_(sort_expression(NewType));
        *DataTerm = DataTerm->set_argument(gsMakeSetComp(), 0);
      }
      else if (aterm()!=gstcTypeMatchA(sort_nat::nat(),ResType))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        *DataTerm = DataTerm->set_argument(gsMakeBagComp(), 0);
      }
      else if (aterm()!=gstcTypeMatchA(sort_pos::pos(),ResType))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        *DataTerm = DataTerm->set_argument(gsMakeBagComp(), 0);
        Data=gsMakeDataAppl(sort_nat::cnat(),ATmakeList1(Data));
      }
      else
      {
        mCRL2log(error) << "the condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " << core::pp_deprecated(ResType) << std::endl;
        return aterm_appl();
      }

      if (aterm()==(NewType=gstcTypeMatchA(NewType,PosType)))
      {
        mCRL2log(error) << "a set or bag comprehension of type " << core::pp_deprecated(aterm_cast<aterm_appl>(VarDecl(1))) << " does not match possible type " <<
                            core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
        return aterm_appl();
      }

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      *DataTerm=DataTerm->set_argument(Data,2);
      return NewType;
    }

    if (gsIsForall(BindingOperator) || gsIsExists(BindingOperator))
    {
      ATermList VarList=aterm_cast<aterm_list>((*DataTerm)(1));
      ATermTable NewAllowedVars;
      if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
      {
        return aterm_appl();
      }
      ATermTable NewDeclaredVars;
      if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
      {
        return aterm_appl();
      }

      ATermAppl Data=aterm_cast<aterm_appl>((*DataTerm)(2));
      if (aterm()==gstcTypeMatchA(sort_bool::bool_(),PosType))
      {
        return aterm_appl();
      }
      ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,sort_bool::bool_(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);

      if (aterm()==NewType)
      {
        return aterm_appl();
      }
      if (aterm()==gstcTypeMatchA(sort_bool::bool_(),NewType))
      {
        return aterm_appl();
      }

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      *DataTerm=DataTerm->set_argument(Data,2);
      return sort_bool::bool_();
    }

    if (gsIsLambda(BindingOperator))
    {
      ATermList VarList=aterm_cast<aterm_list>((*DataTerm)(1));
      ATermTable NewAllowedVars;
      if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
      {
        return aterm_appl();
      }
      ATermTable NewDeclaredVars;
      if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
      {
        return aterm_appl();
      }

      ATermList ArgTypes=gstcGetVarTypes(VarList);
      ATermAppl NewType=gstcUnArrowProd(ArgTypes,PosType);
      if (aterm()==NewType)
      {
        mCRL2log(error) << "no functions with arguments " << core::pp_deprecated(ArgTypes) << " among " << core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
        return aterm_appl();
      }
      ATermAppl Data=aterm_cast<aterm_appl>((*DataTerm)(2));

      NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,NewType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);

      mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(Data) << "" << std::endl;

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      if (aterm()==NewType)
      {
        return aterm_appl();
      }
      *DataTerm=DataTerm->set_argument(Data,2);
      return gsMakeSortArrow(ArgTypes,NewType);
    }
  }

  if (is_where_clause(*DataTerm))
  {
    ATermList WhereVarList;
    ATermList NewWhereList;
    for (ATermList WhereList=aterm_cast<aterm_list>((*DataTerm)(1)); !WhereList.empty(); WhereList=WhereList.tail())
    {
      ATermAppl WhereElem=ATAgetFirst(WhereList);
      ATermAppl WhereTerm=aterm_cast<aterm_appl>(WhereElem(1));
      ATermAppl WhereType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&WhereTerm,data::unknown_sort(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
      if (aterm()==WhereType)
      {
        return aterm_appl();
      }

      ATermAppl NewWhereVar;
      if (gsIsDataVarId(aterm_cast<aterm_appl>(WhereElem(0))))
      {
        // The variable in WhereElem is type checked, and a proper variable.
        NewWhereVar=aterm_cast<aterm_appl>(WhereElem(0));
      }
      else
      {
        // The variable in WhereElem is just a string and needs to be transformed to a DataVarId.
        NewWhereVar=gsMakeDataVarId(aterm_cast<aterm_appl>(WhereElem(0)),WhereType);
      }
      WhereVarList=push_front<aterm>(WhereVarList,NewWhereVar);
      NewWhereList=push_front<aterm>(NewWhereList,gsMakeDataVarIdInit(NewWhereVar,WhereTerm));
    }
    NewWhereList=reverse(NewWhereList);

    ATermTable CopyAllowedVars=ATtableCreate(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    ATermTable CopyDeclaredVars=ATtableCreate(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    ATermList VarList=reverse(WhereVarList);
    ATermTable NewAllowedVars;
    if (!gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars))
    {
      return aterm_appl();
    }
    ATermTable NewDeclaredVars;
    if (!gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars))
    {
      return aterm_appl();
    }

    ATermAppl Data=aterm_cast<aterm_appl>((*DataTerm)(0));
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,&Data,PosType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);

    if (aterm()==NewType)
    {
      return aterm_appl();
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
    ATermList Arguments=aterm_cast<aterm_list>((*DataTerm)(1));
    size_t nArguments=Arguments.size();

    //The following is needed to check enumerations
    ATermAppl Arg0 = aterm_cast<aterm_appl>((*DataTerm)(0));
    if (gsIsOpId(Arg0) || gsIsId(Arg0))
    {
      ATermAppl Name = aterm_cast<aterm_appl>(Arg0(0));
      if (Name == sort_list::list_enumeration_name())
      {
        ATermAppl Type=gstcUnList(PosType);
        if (aterm()==Type)
        {
          mCRL2log(error) << "not possible to cast list to " << core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(Arguments) << ")" << std::endl;
          return aterm_appl();
        }

        ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermList NewArguments;
        bool Type_is_stable=true;
        for (; !Arguments.empty(); Arguments=Arguments.tail())
        {
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,false);
          if (aterm()==Type0)
          {
            // Try again, but now without Type as the suggestion. 
            // If this does not work, it will be caught in the second pass below.
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,data::unknown_sort(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
            if (aterm()==Type0)
            { 
              return aterm_appl();
            }
          }
          NewArguments=push_front<aterm>(NewArguments,Argument);
          Type_is_stable=Type_is_stable && (Type==Type0);
          Type=Type0;
        }
        Arguments=OldArguments;

        //Second time to do the real work, but only if the elements in the list have different types.
        if (!Type_is_stable)
        {
          NewArguments=aterm_list();
          for (; !Arguments.empty(); Arguments=Arguments.tail())
          {
            ATermAppl Argument=ATAgetFirst(Arguments);
            ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
            if (aterm()==Type0)
            {
              return aterm_appl();
            }
            NewArguments=push_front<aterm>(NewArguments,Argument);
            Type=Type0;
          }
        }
        Arguments=reverse(NewArguments);

        Type=sort_list::list(sort_expression(Type));
        *DataTerm=sort_list::list_enumeration(sort_expression(Type), data_expression_list(Arguments));
        return Type;
      }
      if (Name == sort_set::set_enumeration_name())
      {
        ATermAppl Type=gstcUnSet(PosType);
        if (aterm()==Type)
        {
          mCRL2log(error) << "not possible to cast set to " << core::pp_deprecated(PosType) << " (while typechecking " << core::pp_deprecated(Arguments) << ")" << std::endl;
          return aterm_appl();
        }

        ATermList OldArguments=Arguments;

        //First time to determine the common type only (which will be NewType)!
        ATermAppl NewType;
        for (; !Arguments.empty(); Arguments=Arguments.tail())
        {
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (aterm()==Type0)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument) << ")" << std::endl;
            return aterm_appl();
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
            return aterm_appl();
          }
        }

        // Type is now the most generic type to be used.
        assert(Type!=ATermAppl());
        Type=NewType;
        Arguments=OldArguments;

        //Second time to do the real work.
        ATermList NewArguments;
        for (; !Arguments.empty(); Arguments=Arguments.tail())
        {
          ATermAppl Argument=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (aterm()==Type0)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument) << ")" << std::endl;
            return aterm_appl();
          }
          NewArguments=push_front<aterm>(NewArguments,Argument);
          Type=Type0;
        }
        Arguments=reverse(NewArguments);
        Type=sort_set::set_(sort_expression(Type));
        *DataTerm=sort_set::set_enumeration(sort_expression(Type),data_expression_list(Arguments));
        return Type;
      }
      if (Name == sort_bag::bag_enumeration_name())
      {
        ATermAppl Type=gstcUnBag(PosType);
        if (aterm()==Type)
        {
          mCRL2log(error) << "not possible to cast bag to " << core::pp_deprecated(PosType) << "(while typechecking " << core::pp_deprecated(Arguments) << ")" << std::endl;
          return aterm_appl();
        }

        ATermList OldArguments=Arguments;

        //First time to determine the common type only!
        ATermAppl NewType;
        for (; !Arguments.empty(); Arguments=Arguments.tail())
        {
          ATermAppl Argument0=ATAgetFirst(Arguments);
          Arguments=Arguments.tail();
          ATermAppl Argument1=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument0,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (aterm()==Type0)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument0) << ")" << std::endl;
            return aterm_appl();
          }
          ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument1,sort_nat::nat(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if ((aterm()==Type1) && print_cast_error)
          {
            mCRL2log(error) << "not possible to cast number to " << core::pp_deprecated(sort_nat::nat()) << " (while typechecking " << core::pp_deprecated(Argument1) << ")" << std::endl;
            return aterm_appl();
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
            return aterm_appl();
          }
        }
        assert(Type!=ATermAppl());
        Type=NewType;
        Arguments=OldArguments;

        //Second time to do the real work.
        ATermList NewArguments;
        for (; !Arguments.empty(); Arguments=Arguments.tail())
        {
          ATermAppl Argument0=ATAgetFirst(Arguments);
          Arguments=Arguments.tail();
          ATermAppl Argument1=ATAgetFirst(Arguments);
          ATermAppl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument0,Type,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if ((aterm()==Type0) && print_cast_error)
          {
            mCRL2log(error) << "not possible to cast element to " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Argument0) << ")" << std::endl;
            return aterm_appl();
          }
          ATermAppl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Argument1,sort_nat::nat(),FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if ((aterm()==Type1) && print_cast_error)
          {
            mCRL2log(error) << "not possible to cast number to " << core::pp_deprecated(sort_nat::nat()) << " (while typechecking " << core::pp_deprecated(Argument1) << ")" << std::endl;
            return aterm_appl();
          }
          NewArguments=push_front<aterm>(NewArguments,Argument0);
          NewArguments=push_front<aterm>(NewArguments,Argument1);
          Type=Type0;
        }
        Arguments=reverse(NewArguments);
        Type=sort_bag::bag(sort_expression(Type));
        *DataTerm=sort_bag::bag_enumeration(sort_expression(Type), data_expression_list(Arguments));
        return Type;
      }
    }
    ATermList NewArgumentTypes;
    ATermList NewArguments;

    for (; !Arguments.empty(); Arguments=Arguments.tail())
    {
      ATermAppl Arg=ATAgetFirst(Arguments);
      ATermAppl Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,data::unknown_sort(),FreeVars,false,warn_upcasting,print_cast_error);
      if (aterm()==Type)
      {
        return aterm_appl();
      }
      NewArguments=push_front<aterm>(NewArguments,Arg);
      NewArgumentTypes=push_front<aterm>(NewArgumentTypes,Type);
    }
    Arguments=reverse(NewArguments);
    ATermList ArgumentTypes=reverse(NewArgumentTypes);

    //function
    ATermAppl Data=aterm_cast<aterm_appl>((*DataTerm)(0));
    ATermAppl NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                      &Data,
                      data::unknown_sort() /* gsMakeSortArrow(ArgumentTypes,PosType) */,
                      FreeVars,false,nArguments,warn_upcasting,print_cast_error);
    mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(Data) << "" << std::endl;

    if (aterm()==NewType)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else if (gsIsOpId(Data)||gsIsDataVarId(Data))
      {
        gstcErrorMsgCannotCast(aterm_cast<aterm_appl>(Data(1)),Arguments,ArgumentTypes);
      }
      mCRL2log(error) << "type error while trying to cast " << core::pp_deprecated(gsMakeDataAppl(Data,Arguments)) << " to type " << core::pp_deprecated(PosType) << std::endl;
      return aterm_appl();
    }

    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.


    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      ATermList NeededArgumentTypes=aterm_cast<aterm_list>(gstcUnwindType(NewType)(0));

      if (NeededArgumentTypes.size()!=Arguments.size())
      {
         mCRL2log(error) << "need argumens of sorts " << core::pp_deprecated(NeededArgumentTypes) << 
                         " which does not match the number of provided arguments "
                            << core::pp_deprecated(Arguments) << " (while typechecking " 
                            << core::pp_deprecated(*DataTerm) << ")" << std::endl;
         return aterm_appl();

      }
      //arguments again
      ATermList NewArgumentTypes;
      ATermList NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        assert(!Arguments.empty());
        assert(!NeededArgumentTypes.empty());
        ATermAppl Arg=ATAgetFirst(Arguments);
        ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
        ATermAppl Type=ATAgetFirst(ArgumentTypes);
        if (!gstcEqTypesA(NeededType,Type))
        {
          //upcasting
          ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg,warn_upcasting);
          if (aterm()!=CastedNewType)
          {
            Type=CastedNewType;
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on (1) " << core::pp_deprecated(Arg) << ", Type: " << core::pp_deprecated(Type) << ", Needed type: " << core::pp_deprecated(NeededType) << "" << std::endl;
          ATermAppl NewArgType=gstcTypeMatchA(NeededType,Type);
          if (aterm()==NewArgType)
          {
            NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          }
          if (aterm()==NewArgType)
          {
            NewArgType=NeededType;
          }
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,NewArgType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          mCRL2log(debug) << "Result of Doing again gstcTraverseVarConsTypeD: DataTerm " << core::pp_deprecated(Arg) << "" << std::endl;
          if (aterm()==NewArgType)
          {
            mCRL2log(error) << "needed type " << core::pp_deprecated(NeededType) << " does not match possible type "
                            << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Arg) << " in " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
            return aterm_appl();
          }
          Type=NewArgType;
        }
        NewArguments=push_front<aterm>(NewArguments,Arg);
        NewArgumentTypes=push_front<aterm>(NewArgumentTypes,Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    //the function again
    NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                      &Data,gsMakeSortArrow(ArgumentTypes,PosType),FreeVars,strict_ambiguous,nArguments,warn_upcasting,print_cast_error);

    mCRL2log(debug) << "Result of gstcTraverseVarConsTypeDN: DataTerm " << core::pp_deprecated(Data) << "" << std::endl;

    if (aterm()==NewType)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else
      {
        gstcErrorMsgCannotCast(aterm_cast<aterm_appl>(Data(1)),Arguments,ArgumentTypes);
      }
      mCRL2log(error) << "type error while trying to cast " << core::pp_deprecated(gsMakeDataAppl(Data,Arguments)) << " to type " << core::pp_deprecated(PosType) << std::endl;
      return aterm_appl();
    }

    mCRL2log(debug) << "Arguments once more: Arguments " << core::pp_deprecated(Arguments) << ", ArgumentTypes: " <<
                core::pp_deprecated(ArgumentTypes) << ", NewType: " << core::pp_deprecated(NewType) << "" << std::endl;

    //and the arguments once more
    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      ATermList NeededArgumentTypes=aterm_cast<aterm_list>(gstcUnwindType(NewType)(0));
      ATermList NewArgumentTypes;
      ATermList NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        ATermAppl Arg=ATAgetFirst(Arguments);
        ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
        ATermAppl Type=ATAgetFirst(ArgumentTypes);

        if (!gstcEqTypesA(NeededType,Type))
        {
          //upcasting
          ATermAppl CastedNewType=gstcUpCastNumericType(NeededType,Type,&Arg,warn_upcasting);
          if (aterm()!=CastedNewType)
          {
            Type=CastedNewType;
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on (2) " << core::pp_deprecated(Arg) << ", Type: " << core::pp_deprecated(Type) << ", Needed type: " << core::pp_deprecated(NeededType) << "" << std::endl;
          ATermAppl NewArgType=gstcTypeMatchA(NeededType,Type);
          if (aterm()==NewArgType)
          {
            NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          }
          if (aterm()==NewArgType)
          {
            NewArgType=NeededType;
          }
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,&Arg,NewArgType,FreeVars,strict_ambiguous,warn_upcasting,print_cast_error);
          if (aterm()==NewArgType)
          {
            mCRL2log(error) << "needed type " << core::pp_deprecated(NeededType) << " does not match possible type "
                            << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(Arg) << " in " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
            return aterm_appl();
          }
          Type=NewArgType;
        }

        NewArguments=push_front<aterm>(NewArguments,Arg);
        NewArgumentTypes=push_front<aterm>(NewArgumentTypes,Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    mCRL2log(debug) << "Arguments after once more: Arguments " << core::pp_deprecated(Arguments) << ", ArgumentTypes: " << core::pp_deprecated(ArgumentTypes) << "" << std::endl;

    *DataTerm=gsMakeDataAppl(Data,Arguments);

    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      return aterm_cast<aterm_appl>(gstcUnwindType(NewType)(1));
    }

    if (gstcHasUnknown(gstcUnArrowProd(ArgumentTypes,NewType)))
    {
      mCRL2log(error) << "Fail to properly type " << core::pp_deprecated(*DataTerm) << std::endl;
      return aterm_appl();
    }
    return gstcUnArrowProd(ArgumentTypes,NewType);
  }

  if (gsIsId(*DataTerm)||gsIsOpId(*DataTerm)||gsIsDataVarId(*DataTerm))
  {
    ATermAppl Name=aterm_cast<aterm_appl>((*DataTerm)(0));
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

      if (aterm()!=gstcTypeMatchA(Sort,PosType))
      {
        return Sort;
      }

      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Sort,DataTerm,warn_upcasting);
      if (aterm()==CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast number " << core::pp_deprecated(*DataTerm) << " to type " << core::pp_deprecated(PosType) << std::endl;
        return aterm_appl();
      }
      return CastedNewType;
    }

    ATermAppl Type=aterm_cast<aterm_appl>(ATtableGet(DeclaredVars,Name));
    if (aterm()!=Type)
    {
      mCRL2log(debug) << "Recognised declared variable " << core::pp_deprecated(Name) << ", Type: " << core::pp_deprecated(Type) << "" << std::endl;
      *DataTerm=gsMakeDataVarId(Name,Type);

      if (aterm()==aterm_cast<aterm_appl>(ATtableGet(AllowedVars,Name)))
      {
        mCRL2log(error) << "variable " << core::pp_deprecated(Name) << " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side" << std::endl;
        return aterm_appl();
      }

      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if (aterm()!=NewType)
      {
        Type=NewType;
      }
      else
      {
        //upcasting
        ATermAppl CastedNewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        if ((aterm()==CastedNewType) && print_cast_error)
        {
          mCRL2log(error) << "cannot (up)cast variable " << core::pp_deprecated(*DataTerm) << " to type " << core::pp_deprecated(PosType) << std::endl;
          return aterm_appl();
        }

        Type=CastedNewType;
      }

      //Add to free variables list
      // if (FreeVars)
      {
        FreeVars.put( Name, Type);
      }

      return Type;
    }

    if (aterm()!=(Type=aterm_cast<aterm_appl>(ATtableGet(context.constants,Name))))
    {
      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if (aterm()!=NewType)
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
          return aterm_appl();
        }
        else
        {
          return NewType;
        }
      }
    }

    ATermList ParList=aterm_cast<aterm_list>(ATtableGet(gssystem.constants,Name));
    if (aterm()!=ParList)
    {
      ATermList NewParList;
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        ATermAppl Par=ATAgetFirst(ParList);
        if (aterm()!=(Par=gstcTypeMatchA(Par,PosType)))
        {
          NewParList=push_front<aterm>(NewParList,Par);
        }
      }
      ParList=reverse(NewParList);
      if (ParList.empty())
      {
        mCRL2log(error) << "no system constant " << core::pp_deprecated(*DataTerm) << " with type " << core::pp_deprecated(PosType) << std::endl;
        return aterm_appl();
      }

      if (ParList.size()==1)
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

    ATermList ParListS=aterm_cast<aterm_list>(ATtableGet(gssystem.functions,Name));
    ParList=aterm_cast<aterm_list>(ATtableGet(context.functions,Name));
    if (aterm()==ParList)
    {
      ParList=ParListS;
    }
    else if (aterm()!=ParListS)
    {
      ParList=ParListS+ParList;
    }

    if (aterm()==ParList)
    {
      mCRL2log(error) << "unknown operation " << core::pp_deprecated(Name) << std::endl;
      return aterm_appl();
    }

    if (ParList.size()==1)
    {
      ATermAppl Type=ATAgetFirst(ParList);
      *DataTerm=gsMakeOpId(Name,Type);
      ATermAppl NewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
      if (NewType==ATermAppl())
      {
        mCRL2log(error) << "no constant " << core::pp_deprecated(*DataTerm) << " with type " << core::pp_deprecated(PosType) << std::endl;
        return aterm_appl();
      }
      return NewType;
    }
    else
    {
      return gstcTraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strict_ambiguous, std::string::npos, warn_upcasting,print_cast_error);
    }
  }

  // if(gsIsDataVarId(*DataTerm)){
  //   return aterm_cast<aterm_appl>((*DataTerm)(1));
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
  // std::string::npos for nFactPars means the number of arguments is not known.
  mCRL2log(debug) << "gstcTraverseVarConsTypeDN: DataTerm " << core::pp_deprecated(*DataTerm)
                  << " with PosType " << core::pp_deprecated(PosType) << ", nFactPars " << nFactPars << "" << std::endl;
  if (gsIsId(*DataTerm)||gsIsOpId(*DataTerm))
  {
    ATermAppl Name=aterm_cast<aterm_appl>((*DataTerm)(0));
    bool variable=false;
    ATermAppl Type=aterm_cast<aterm_appl>(ATtableGet(DeclaredVars,Name));
    if (aterm()!=Type)
    {
      const sort_expression Type1(gstcUnwindType(Type));
      if (is_function_sort(Type1)?(function_sort(Type1).domain().size()==nFactPars):(nFactPars==0))
      {
        variable=true;
        if (aterm()==aterm_cast<aterm_appl>(ATtableGet(AllowedVars,Name)))
        {
          mCRL2log(error) << "variable " << core::pp_deprecated(Name) << " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side" << std::endl;
          return aterm_appl();
        }

        //Add to free variables list
        // if (FreeVars)
        {
          FreeVars.put( Name, Type);
        }
      }
      else
      {
        Type=aterm_appl();
      }
    }
    ATermList ParList;

    if (nFactPars==0)
    {
      if (aterm()!=(Type=aterm_cast<aterm_appl>(ATtableGet(DeclaredVars,Name))))
      {
        if (aterm()==gstcTypeMatchA(Type,PosType))
        {
          mCRL2log(error) << "the type " << core::pp_deprecated(Type) << " of variable " << core::pp_deprecated(Name)
                          << " is incompatible with " << core::pp_deprecated(PosType) << " (typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        *DataTerm=gsMakeDataVarId(Name,Type);
        return Type;
      }
      else if (aterm()!=(Type=aterm_cast<aterm_appl>(ATtableGet(context.constants,Name))))
      {
        if (aterm()==gstcTypeMatchA(Type,PosType))
        {
          mCRL2log(error) << "the type " << core::pp_deprecated(Type) << " of constant " << core::pp_deprecated(Name)
                          << " is incompatible with " << core::pp_deprecated(PosType) << " (typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        *DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        if (aterm()!=(ParList=aterm_cast<aterm_list>(ATtableGet(gssystem.constants,Name))))
        {
          if (ParList.size()==1)
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
          return aterm_appl();
        }
      }
    }

    if (aterm()!=Type)
    {
      ParList=ATmakeList1(gstcUnwindType(Type));
    }
    else
    {
      ATermList ParListS=aterm_cast<aterm_list>(ATtableGet(gssystem.functions,Name));
      ParList=aterm_cast<aterm_list>(ATtableGet(context.functions,Name));
      if (aterm()==ParList)
      {
        ParList=ParListS;
      }
      else if (aterm()!=ParListS)
      {
        ParList=ParListS+ParList;
      }
    }

    if (aterm()==ParList)
    {
      if (nFactPars!=std::string::npos)
      {
        mCRL2log(error) << "unknown operation " << core::pp_deprecated(Name) << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"") << std::endl;
      }
      else
      {
        mCRL2log(error) << "unknown operation " << core::pp_deprecated(Name) << std::endl;;
      }
      return aterm_appl();
    }
    mCRL2log(debug) << "Possible types for Op/Var " << core::pp_deprecated(Name) << " with " << nFactPars <<
                " argument are (ParList: " << core::pp_deprecated(ParList) << "; PosType: " << core::pp_deprecated(PosType) << ")" << std::endl;

    ATermList CandidateParList=ParList;

    {
      // filter ParList keeping only functions A_0#...#A_nFactPars->A
      ATermList NewParList;
      if (nFactPars!=std::string::npos)
      {
        NewParList=aterm_list();
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          ATermAppl Par=ATAgetFirst(ParList);
          if (!gsIsSortArrow(Par))
          {
            continue;
          }
          if (aterm_cast<aterm_list>(Par(0)).size()!=nFactPars)
          {
            continue;
          }
          NewParList=push_front<aterm>(NewParList,Par);
        }
        ParList=reverse(NewParList);
      }

      if (!ParList.empty())
      {
        CandidateParList=ParList;
      }

      // filter ParList keeping only functions of the right type
      ATermList BackupParList=ParList;
      NewParList=aterm_list();
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        ATermAppl Par=ATAgetFirst(ParList);
        if (aterm()!=(Par=gstcTypeMatchA(Par,PosType)))
        {
          NewParList=ATinsertUnique(NewParList,Par);
        }
      }
      NewParList=reverse(NewParList);

      mCRL2log(debug) << "Possible matches w/o casting for Op/Var " << core::pp_deprecated(Name) << " with "<< nFactPars <<               " argument are (ParList: " << core::pp_deprecated(NewParList) << "; PosType: " << core::pp_deprecated(PosType) << "" << std::endl;

      if (NewParList.empty())
      {
        //Ok, this looks like a type error. We are not that strict.
        //Pos can be Nat, or even Int...
        //So lets make PosType more liberal
        //We change every Pos to NotInferred(Pos,Nat,Int)...
        //and get the list. Then we take the min of the list.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying casting for Op " << core::pp_deprecated(Name) << " with " << nFactPars << " argument (ParList: " <<                             core::pp_deprecated(ParList) << "; PosType: " << core::pp_deprecated(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesUp(PosType);
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          ATermAppl Par=ATAgetFirst(ParList);
          if (aterm()!=(Par=gstcTypeMatchA(Par,PosType)))
          {
            NewParList=ATinsertUnique(NewParList,Par);
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is (1) " << core::pp_deprecated(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=ATmakeList1(gstcMinType(NewParList));
        }
      }

      if (NewParList.empty())
      {
        //Ok, casting of the arguments did not help.
        //Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying result casting for Op " << core::pp_deprecated(Name) << " with " << nFactPars << " argument (ParList: "                       << "; PosType: " << core::pp_deprecated(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesDown(gstcExpandNumTypesUp(PosType));
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          ATermAppl Par=ATAgetFirst(ParList);
          if (aterm()!=(Par=gstcTypeMatchA(Par,PosType)))
          {
            NewParList=ATinsertUnique(NewParList,Par);
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is (2)" << core::pp_deprecated(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=ATmakeList1(gstcMinType(NewParList));
        }
      }

      ParList=NewParList;
    }
    if (ParList.empty())
    {
      //provide some information to the upper layer for a better error message
      ATermAppl Sort;
      if (CandidateParList.size()==1)
      {
        Sort=ATAgetFirst(CandidateParList);
      }
      else
      {
        // Sort=multiple_possible_sorts(atermpp::aterm_list(CandidateParList));
        Sort=multiple_possible_sorts(sort_expression_list(CandidateParList));
      }
      *DataTerm=gsMakeOpId(Name,Sort);
      if (nFactPars!=std::string::npos)
      {
        mCRL2log(error) << "unknown operation/variable " << core::pp_deprecated(Name)
                        << " with " << nFactPars << " argument" << ((nFactPars != 1)?"s":"")
                        << " that matches type " << core::pp_deprecated(PosType) << std::endl;
      }
      else
      {
        mCRL2log(error) << "unknown operation/variable " << core::pp_deprecated(Name) << " that matches type " << core::pp_deprecated(PosType) << std::endl;
      }
      return aterm_appl();
    }

    if (ParList.size()==1)
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
        Type=gstcTypeMatchA(Type,aterm_cast<aterm_appl>((*DataTerm)(1)));
      }
      if (Type==ATermAppl())
      {
        mCRL2log(error) << "fail to match sort " << core::pp_deprecated(OldType) << " with " << core::pp_deprecated(PosType) << std::endl;
        return aterm_appl();
      }

      if (static_cast<ATermAppl>(data::detail::if_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing if matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchIf(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function if has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(data::detail::equal_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0))
          || static_cast<ATermAppl>(data::detail::not_equal_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0))
          || static_cast<ATermAppl>(data::detail::less_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0))
          || static_cast<ATermAppl>(data::detail::less_equal_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0))
          || static_cast<ATermAppl>(data::detail::greater_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0))
          || static_cast<ATermAppl>(data::detail::greater_equal_symbol())==aterm_cast<aterm_appl>((*DataTerm)(0))
         )
      {
        mCRL2log(debug) << "Doing ==, !=, <, <=, >= or > matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchEqNeqComparison(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function " << core::pp_deprecated(aterm_cast<aterm_appl>((*DataTerm)(0))) << " has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::cons_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing |> matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpCons(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function |> has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::snoc_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing <| matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpSnoc(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function <| has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::concat_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing ++ matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpConcat(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function ++ has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::element_at_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing @ matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << ", DataTerm: " << core::pp_deprecated(*DataTerm) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpEltAt(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function @ has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::head_name())==aterm_cast<aterm_appl>((*DataTerm)(0))||
          static_cast<ATermAppl>(sort_list::rhead_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing {R,L}head matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
// Type==NULL
        ATermAppl NewType=gstcMatchListOpHead(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function {R,L}head has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::tail_name())==aterm_cast<aterm_appl>((*DataTerm)(0))||
          static_cast<ATermAppl>(sort_list::rtail_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing {R,L}tail matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListOpTail(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function {R,L}tail has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_bag::set2bag_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing Set2Bag matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchSetOpSet2Bag(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function Set2Bag has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_list::in_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing {List,Set,Bag} matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchListSetBagOpIn(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function {List,Set,Bag}In has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_set::union_name())==aterm_cast<aterm_appl>((*DataTerm)(0))||
          static_cast<ATermAppl>(sort_set::difference_name())==aterm_cast<aterm_appl>((*DataTerm)(0))||
          static_cast<ATermAppl>(sort_set::intersection_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing {Set,Bag}{Union,Difference,Intersect} matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchSetBagOpUnionDiffIntersect(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_set::complement_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing SetCompl matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchSetOpSetCompl(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function SetCompl has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_bag::bag2set_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing Bag2Set matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchBagOpBag2Set(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function Bag2Set has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<ATermAppl>(sort_bag::count_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing BagCount matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchBagOpBagCount(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "the function BagCount has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }


      if (static_cast<ATermAppl>(data::function_update_name())==aterm_cast<aterm_appl>((*DataTerm)(0)))
      {
        mCRL2log(debug) << "Doing FuncUpdate matching Type " << core::pp_deprecated(Type) << ", PosType " << core::pp_deprecated(PosType) << "" << std::endl;
        ATermAppl NewType=gstcMatchFuncUpdate(Type);
        if (aterm()==NewType)
        {
          mCRL2log(error) << "function update has incompatible argument types " << core::pp_deprecated(Type) << " (while typechecking " << core::pp_deprecated(*DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }


      Type=replace_possible_sorts(Type); // Set the type to one option in possible sorts, if there are more options.
      *DataTerm=gsMakeOpId(Name,Type);
      if (variable)
      {
        *DataTerm=gsMakeDataVarId(Name,Type);
      }

      assert(aterm()!=Type);
      return Type;
    }
    else
    {
      was_ambiguous=true;
      if (strict_ambiguous)
      {
        mCRL2log(debug) << "ambiguous operation " << core::pp_deprecated(Name) << " (ParList " << core::pp_deprecated(ParList) << ")" << std::endl;
        if (nFactPars!=std::string::npos)
        {
          mCRL2log(error) << "ambiguous operation " << core::pp_deprecated(Name) << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"") << std::endl;
        }
        else
        {
          mCRL2log(error) << "ambiguous operation " << core::pp_deprecated(Name) << std::endl;
        }
        return aterm_appl();
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

  ATermList Result;
  size_t nFormPars=((ATermList)TypeListList.front()).size();
  std::vector<ATermList> Pars(nFormPars);
  /* if (nFormPars > 0)
  {
    Pars = (ATermList*) malloc(nFormPars*sizeof(ATermList));
  } */
  //DECLA(ATermList,Pars,nFormPars);
  for (size_t i=0; i<nFormPars; i++)
  {
    Pars[i]=aterm_list();
  }

  for (; !TypeListList.empty(); TypeListList=TypeListList.tail())
  {
    ATermList TypeList=ATLgetFirst(TypeListList);
    for (size_t i=0; i<nFormPars; TypeList=TypeList.tail(),i++)
    {
      Pars[i]=gstcInsertType(Pars[i],ATAgetFirst(TypeList));
    }
  }

  for (size_t i=nFormPars; i>0; i--)
  {
    ATermAppl Sort;
    if (Pars[i-1].size()==1)
    {
      Sort=ATAgetFirst(Pars[i-1]);
    }
    else
    {
      // Sort=multiple_possible_sorts(atermpp::aterm_list(reverse(Pars[i-1])));
      Sort=multiple_possible_sorts(sort_expression_list(reverse(Pars[i-1])));
    }
    Result=push_front<aterm>(Result,Sort);
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
    sort_expression_list l=sort_expression_list(aterm_cast<aterm_list>(NeededType(0)));
    for(sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      ATermAppl r=gstcUpCastNumericType(*i,Type,Par,warn_upcasting);
      if (r!=ATermAppl())
      {
        return r;
      }
    }
    return aterm_appl();
  }

  if (warn_upcasting && gsIsOpId(*Par) && gsIsNumericString(gsATermAppl2String(aterm_cast<aterm_appl>((*Par)(0)))))
  {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  if (aterm()!=gstcTypeMatchA(NeededType,sort_pos::pos()))
  {
    if (aterm()!=gstcTypeMatchA(Type,sort_pos::pos()))
    {
      return sort_pos::pos();
    }
  }

  // Try Upcasting to Nat
  if (aterm()!=gstcTypeMatchA(NeededType,sort_nat::nat()))
  {
    if (aterm()!=gstcTypeMatchA(Type,sort_pos::pos()))
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
    if (aterm()!=gstcTypeMatchA(Type,sort_nat::nat()))
    {
      return sort_nat::nat();
    }
  }

  // Try Upcasting to Int
  if (aterm()!=gstcTypeMatchA(NeededType,sort_int::int_()))
  {
    if (aterm()!=gstcTypeMatchA(Type,sort_pos::pos()))
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
    if (aterm()!=gstcTypeMatchA(Type,sort_nat::nat()))
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
    if (aterm()!=gstcTypeMatchA(Type,sort_int::int_()))
    {
      return sort_int::int_();
    }
  }

  // Try Upcasting to Real
  if (aterm()!=gstcTypeMatchA(NeededType,sort_real::real_()))
  {
    if (aterm()!=gstcTypeMatchA(Type,sort_pos::pos()))
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
    if (aterm()!=gstcTypeMatchA(Type,sort_nat::nat()))
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
    if (aterm()!=gstcTypeMatchA(Type,sort_int::int_()))
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
    if (aterm()!=gstcTypeMatchA(Type,sort_real::real_()))
    {
      return sort_real::real_();
    }
  }

  return aterm_appl();
}

static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type)
{
  for (ATermList OldTypeList=TypeList; !OldTypeList.empty(); OldTypeList=OldTypeList.tail())
  {
    if (gstcEqTypesA(ATAgetFirst(OldTypeList),Type))
    {
      return TypeList;
    }
  }
  return push_front<aterm>(TypeList,Type);
}

static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2)
{
  // returns the intersection of the 2 type list lists

  ATermList Result;

  for (; !TypeListList2.empty(); TypeListList2=TypeListList2.tail())
  {
    ATermList TypeList2=ATLgetFirst(TypeListList2);
    if (gstcInTypesL(TypeList2,TypeListList1))
    {
      Result=push_front<aterm>(Result,TypeList2);
    }
  }
  return reverse(Result);
}

static ATermList gstcAdjustNotInferredList(ATermList PosTypeList, ATermList TypeListList)
{
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  //if PosTypeList has only normal types -- check if it is in TypeListList,
  //if so return PosTypeList, otherwise return aterm_appl()
  if (!gstcIsNotInferredL(PosTypeList))
  {
    if (gstcInTypesL(PosTypeList,TypeListList))
    {
      return PosTypeList;
    }
    else
    {
      return aterm_list(aterm());
    }
  }

  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  ATermList NewTypeListList;
  for (; !TypeListList.empty(); TypeListList=TypeListList.tail())
  {
    ATermList TypeList=ATLgetFirst(TypeListList);
    if (gstcIsTypeAllowedL(TypeList,PosTypeList))
    {
      NewTypeListList=push_front<aterm>(NewTypeListList,TypeList);
    }
  }
  if (NewTypeListList.empty())
  {
    return aterm_list(aterm());
  }
  if (NewTypeListList.size()==1)
  {
    return ATLgetFirst(NewTypeListList);
  }

  // otherwise return not inferred.
  return gstcGetNotInferredList(reverse(NewTypeListList));
}

static bool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList)
{
  //Checks if TypeList is allowed by PosTypeList (each respective element)
  assert(TypeList.size()==PosTypeList.size());
  for (; !TypeList.empty(); TypeList=TypeList.tail(),PosTypeList=PosTypeList.tail())
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
    return gstcInTypesA(Type,aterm_cast<aterm_list>(PosType(0)));
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
    ATermList NewTypeList;
    for (ATermList PosTypeList=aterm_cast<aterm_list>(PosType(0)); !PosTypeList.empty(); PosTypeList=PosTypeList.tail())
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypeList);
      mCRL2log(debug) << "Matching candidate gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: "
                  << core::pp_deprecated(PosType) << " New Type: " << core::pp_deprecated(NewPosType) << "" << std::endl;

      if (aterm()!=(NewPosType=gstcTypeMatchA(Type,NewPosType)))
      {
        mCRL2log(debug) << "Match gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) <<
                    " New Type: " << core::pp_deprecated(NewPosType) << "" << std::endl;
        NewTypeList=push_front<aterm>(NewTypeList,NewPosType);
      }
    }
    if (NewTypeList.empty())
    {
      mCRL2log(debug) << "No match gstcTypeMatchA Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) << " " << std::endl;
      return aterm_appl();
    }

    if (NewTypeList.tail().empty())
    {
      return ATAgetFirst(NewTypeList);
    }

    return multiple_possible_sorts(sort_expression_list(reverse(NewTypeList)));
    // return multiple_possible_sorts(atermpp::aterm_list(reverse(NewTypeList)));
  }

  //PosType is a normal type
  //if(!gstcHasUnknown(Type)) return aterm_appl();

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
    ATermAppl ConsType = aterm_cast<aterm_appl>(Type(0));
    if (gsIsSortList(ConsType))
    {
      if (!sort_list::is_list(sort_expression(PosType)))
      {
        return aterm_appl();
      }
      ATermAppl Res=gstcTypeMatchA(aterm_cast<aterm_appl>(Type(1)),aterm_cast<aterm_appl>(PosType(1)));
      if (aterm()==Res)
      {
        return aterm_appl();
      }
      return sort_list::list(sort_expression(Res));
    }

    if (gsIsSortSet(ConsType))
    {
      if (!sort_set::is_set(sort_expression(PosType)))
      {
        return aterm_appl();
      }
      else
      {
        ATermAppl Res=gstcTypeMatchA(aterm_cast<aterm_appl>(Type(1)),aterm_cast<aterm_appl>(PosType(1)));
        if (aterm()==Res)
        {
          return aterm_appl();
        }
        return sort_set::set_(sort_expression(Res));
      }
    }

    if (gsIsSortBag(ConsType))
    {
      if (!sort_bag::is_bag(sort_expression(PosType)))
      {
        return aterm_appl();
      }
      else
      {
        ATermAppl Res=gstcTypeMatchA(aterm_cast<aterm_appl>(Type(1)),aterm_cast<aterm_appl>(PosType(1)));
        if (aterm()==Res)
        {
          return aterm_appl();
        }
        return sort_bag::bag(sort_expression(Res));
      }
    }
  }

  if (gsIsSortArrow(Type))
  {
    if (!gsIsSortArrow(PosType))
    {
      return aterm_appl();
    }
    else
    {
      ATermList ArgTypes=gstcTypeMatchL(aterm_cast<aterm_list>(Type(0)),aterm_cast<aterm_list>(PosType(0)));
      if (aterm()==ArgTypes)
      {
        return aterm_appl();
      }
      ATermAppl ResType=gstcTypeMatchA(aterm_cast<aterm_appl>(Type(1)),aterm_cast<aterm_appl>(PosType(1)));
      if (aterm()==ResType)
      {
        return aterm_appl();
      }
      Type=gsMakeSortArrow(ArgTypes,ResType);
      // mCRL2log(debug) << "gstcTypeMatchA Done: Type: " << core::pp_deprecated(Type) << ";    PosType: " << core::pp_deprecated(PosType) << "" << std::endl;
      return Type;
    }
  }

  return aterm_appl();
}

static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList)
{
  mCRL2log(debug) << "gstcTypeMatchL TypeList: " << core::pp_deprecated(TypeList) << ";    PosTypeList: " <<
              core::pp_deprecated(PosTypeList) << "" << std::endl;

  if (TypeList.size()!=PosTypeList.size())
  {
    return aterm_list(aterm_appl());
  }

  ATermList Result;
  for (; !TypeList.empty(); TypeList=TypeList.tail(),PosTypeList=PosTypeList.tail())
  {
    ATermAppl Type=gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList));
    if (aterm()==Type)
    {
      return aterm_list(aterm_appl());
    }
    Result=push_front<aterm>(Result,Type);
  }
  return reverse(Result);
}

static bool gstcIsNotInferredL(ATermList TypeList)
{
  for (; !TypeList.empty(); TypeList=TypeList.tail())
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
    return Type.set_argument(gstcUnwindType(aterm_cast<aterm_appl>(Type(1))),1);
  }
  if (gsIsSortArrow(Type))
  {
    Type=Type.set_argument(gstcUnwindType(aterm_cast<aterm_appl>(Type(1))),1);
    ATermList Args=aterm_cast<aterm_list>(Type(0));
    ATermList NewArgs;
    for (; !Args.empty(); Args=Args.tail())
    {
      NewArgs=push_front<aterm>(NewArgs,gstcUnwindType(ATAgetFirst(Args)));
    }
    NewArgs=reverse(NewArgs);
    Type=Type.set_argument(NewArgs,0);
    return Type;
  }

  if (gsIsSortId(Type))
  {
    ATermAppl Value=aterm_cast<aterm_appl>(ATtableGet(context.defined_sorts,aterm_cast<aterm_appl>(Type(0))));
    if (aterm()==Value)
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
    return aterm_cast<aterm_appl>(PosType(1));
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=aterm_cast<aterm_list>(PosType(0)); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_set::is_set(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<aterm_appl>(NewPosType(1));
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=push_front<aterm>(NewPosTypes,NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
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
    return aterm_cast<aterm_appl>(PosType(1));
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=aterm_cast<aterm_list>(PosType(0)); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_bag::is_bag(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<aterm_appl>(NewPosType(1));
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=push_front<aterm>(NewPosTypes,NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
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
    return aterm_cast<aterm_appl>(PosType(1));
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=aterm_cast<aterm_list>(PosType(0)); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_list::is_list(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<aterm_appl>(NewPosType(1));
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=push_front<aterm>(NewPosTypes,NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
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
    ATermList PosArgTypes=aterm_cast<aterm_list>(PosType(0));

    if (PosArgTypes.size()!=ArgTypes.size())
    {
      return aterm_appl();
    }
    if (aterm()!=gstcTypeMatchL(PosArgTypes,ArgTypes))
    {
      return aterm_cast<aterm_appl>(PosType(1));
    }
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  ATermList NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (ATermList PosTypes=aterm_cast<aterm_list>(PosType(0)); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (gsIsSortArrow(PosType))
      {
        ATermList PosArgTypes=aterm_cast<aterm_list>(PosType(0));
        if (PosArgTypes.size()!=ArgTypes.size())
        {
          continue;
        }
        if (aterm()!=gstcTypeMatchL(PosArgTypes,ArgTypes))
        {
          NewPosType=aterm_cast<aterm_appl>(NewPosType(1));
        }
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=ATinsertUnique(NewPosTypes,NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
}

static ATermList gstcGetVarTypes(ATermList VarDecls)
{
  ATermList Result;
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    Result=push_front<aterm>(Result,aterm_cast<aterm_appl>(ATAgetFirst(VarDecls)(1)));
  }
  return reverse(Result);
}

// Replace occurrences of multiple_possible_sorts([s1,...,sn]) by selecting
// one of the possible sorts from s1,...,sn. Currently, the first is chosen.
static ATermAppl replace_possible_sorts(ATermAppl Type)
{
  if (gsIsSortsPossible(data::sort_expression(Type)))
  {
    return ATAgetFirst(aterm_cast<aterm_list>(Type(0))); // get the first element of the possible sorts.
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
    return Type.set_argument(replace_possible_sorts(aterm_cast<aterm_appl>(Type(1))),1);
  }

  if (gsIsSortStruct(Type))
  {
    return Type;  // I assume that there are no possible sorts in sort constructors. JFG.
  }

  if (gsIsSortArrow(Type))
  {
    ATermList NewTypeList;
    for (ATermList TypeList=aterm_cast<aterm_list>(Type(0)); !TypeList.empty(); TypeList=TypeList.tail())
    {
      NewTypeList=push_front<aterm>(NewTypeList,replace_possible_sorts(ATAgetFirst(TypeList)));
    }
    ATermAppl ResultType=aterm_cast<aterm_appl>(Type(1));
    return gsMakeSortArrow(reverse(NewTypeList),replace_possible_sorts(ResultType));
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
    return gstcHasUnknown(aterm_cast<aterm_appl>(Type(1)));
  }
  if (gsIsSortStruct(Type))
  {
    return false;
  }

  if (gsIsSortArrow(Type))
  {
    for (ATermList TypeList=aterm_cast<aterm_list>(Type(0)); !TypeList.empty(); TypeList=TypeList.tail())
      if (gstcHasUnknown(ATAgetFirst(TypeList)))
      {
        return true;
      }
    return gstcHasUnknown(aterm_cast<aterm_appl>(Type(1)));
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
    return aterm_appl();
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
    return aterm_appl();
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
    return aterm_appl();
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
    return aterm_appl();
  }
  return aterm_appl();
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
    return Type.set_argument(gstcExpandNumTypesUp(aterm_cast<aterm_appl>(Type(1))),1);
  }
  if (gsIsSortStruct(Type))
  {
    return Type;
  }

  if (gsIsSortArrow(Type))
  {
    //the argument types, and if the resulting type is SortArrow -- recursively
    ATermList NewTypeList;
    for (ATermList TypeList=aterm_cast<aterm_list>(Type(0)); !TypeList.empty(); TypeList=TypeList.tail())
    {
      NewTypeList=push_front<aterm>(NewTypeList,gstcExpandNumTypesUp(gstcUnwindType(ATAgetFirst(TypeList))));
    }
    ATermAppl ResultType=aterm_cast<aterm_appl>(Type(1));
    if (!gsIsSortArrow(ResultType))
    {
      return Type.set_argument(reverse(NewTypeList),0);
    }
    else
    {
      return gsMakeSortArrow(reverse(NewTypeList),gstcExpandNumTypesUp(gstcUnwindType(ResultType)));
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
  ATermList Args=aterm_list(aterm());
  if (gsIsSortArrow(Type))
  {
    function=true;
    Args=aterm_cast<aterm_list>(Type(0));
    Type=aterm_cast<aterm_appl>(Type(1));
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
  for (; !MActs.empty(); MActs=MActs.tail())
    if (gstcMActEq(MAct,ATLgetFirst(MActs)))
    {
      return true;
    }

  return false;
}

static bool gstcMActEq(ATermList MAct1, ATermList MAct2)
{
  //returns true if the two multiactions are equal.
  if (MAct1.size()!=MAct2.size())
  {
    return false;
  }
  if (MAct1.empty())
  {
    return true;
  }
  ATermAppl Act1=ATAgetFirst(MAct1);
  MAct1=MAct1.tail();

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  ATermList NewMAct2;
  for (; !MAct2.empty(); MAct2=MAct2.tail())
  {
    ATermAppl Act2=ATAgetFirst(MAct2);
    if (Act1==Act2)
    {
      MAct2=reverse(NewMAct2)+MAct2.tail();
      return gstcMActEq(MAct1,MAct2);
    }
    else
    {
      NewMAct2=push_front<aterm>(NewMAct2,Act2);
    }
  }
  return false;
}

static ATermAppl gstcUnifyMinType(ATermAppl Type1, ATermAppl Type2)
{
  //Find the minimal type that Unifies the 2. If not possible, return aterm_appl().
  ATermAppl Res=gstcTypeMatchA(Type1,Type2);
  if (aterm()==Res)
  {
    Res=gstcTypeMatchA(Type1,gstcExpandNumTypesUp(Type2));
    if (aterm()==Res)
    {
      Res=gstcTypeMatchA(Type2,gstcExpandNumTypesUp(Type1));
    }
    if (aterm()==Res)
    {
      mCRL2log(debug) << "gstcUnifyMinType: No match: Type1 " << core::pp_deprecated(Type1) << "; Type2 " << core::pp_deprecated(Type2) << "; " << std::endl;
      return aterm_appl();
    }
  }

  if (gsIsSortsPossible(Res))
  {
    Res=ATAgetFirst(aterm_cast<aterm_list>(Res(0)));
  }
  mCRL2log(debug) << "gstcUnifyMinType: Type1 " << core::pp_deprecated(Type1) << "; Type2 " << core::pp_deprecated(Type2) << "; Res: " << core::pp_deprecated(Res) << "" << std::endl;
  return Res;
}

static ATermAppl gstcMatchIf(ATermAppl Type)
{
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  assert(gsIsSortArrow(Type));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  assert((Args.size()==3));
  //assert(gsIsBool(ATAgetFirst(Args)));
  Args=Args.tail();

  if (aterm()==(Res=gstcUnifyMinType(Res,ATAgetFirst(Args))))
  {
    return aterm_appl();
  }
  Args=Args.tail();
  if (aterm()==(Res=gstcUnifyMinType(Res,ATAgetFirst(Args))))
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList3(static_cast<ATermAppl>(sort_bool::bool_()),Res,Res),Res);
}

static ATermAppl gstcMatchEqNeqComparison(ATermAppl Type)
{
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=Args.tail();
  ATermAppl Arg2=ATAgetFirst(Args);

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if (aterm()==Arg)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(Arg,Arg),sort_bool::bool_());
}

static ATermAppl gstcMatchListOpCons(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(gstcUnwindType(Res))));
  Res=aterm_cast<aterm_appl>(Res(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=Args.tail();
  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=aterm_cast<aterm_appl>(Arg2(1));

  Res=gstcUnifyMinType(Res,Arg1);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(Res,static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
}

static ATermAppl gstcMatchListOpSnoc(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1(1));

  Args=Args.tail();
  ATermAppl Arg2=ATAgetFirst(Args);

  Res=gstcUnifyMinType(Res,Arg1);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(static_cast<ATermAppl>(sort_list::list(sort_expression(Res))),Res),sort_list::list(sort_expression(Res)));
}

static ATermAppl gstcMatchListOpConcat(ATermAppl Type)
{
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1(1));

  Args=Args.tail();

  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=aterm_cast<aterm_appl>(Arg2(1));

  Res=gstcUnifyMinType(Res,Arg1);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(static_cast<ATermAppl>(sort_list::list(sort_expression(Res))),
       static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
}

static ATermAppl gstcMatchListOpEltAt(ATermAppl Type)
{
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1(1));

  Res=gstcUnifyMinType(Res,Arg1);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  //assert((gsIsSortNat(ATAgetFirst(Args.tail())));

  return gsMakeSortArrow(ATmakeList2(static_cast<ATermAppl>(sort_list::list(sort_expression(Res))),
               static_cast<ATermAppl>(sort_nat::nat())),Res);
}

static ATermAppl gstcMatchListOpHead(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg(1));

  Res=gstcUnifyMinType(Res,Arg);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_list::list(sort_expression(Res)))),Res);
}

static ATermAppl gstcMatchListOpTail(ATermAppl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg(1));

  Res=gstcUnifyMinType(Res,Arg);
  if (aterm()==Res)
  {
    return aterm_appl();
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

  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_bag::is_bag(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res(1));

  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg(1));

  Arg=gstcUnifyMinType(Arg,Res);
  if (aterm()==Arg)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_set::set_(sort_expression(Arg)))),
                  sort_bag::bag(sort_expression(Arg)));
}

static ATermAppl gstcMatchListSetBagOpIn(ATermAppl Type)
{
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  //assert(gsIsBool(aterm_cast<aterm_appl>(Type(1))));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=Args.tail();
  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(gsIsSortCons(Arg2));
  ATermAppl Arg2s=aterm_cast<aterm_appl>(Arg2(1));

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2s);
  if (aterm()==Arg)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(Arg,Arg2.set_argument(Arg,1)),sort_bool::bool_());
}

static ATermAppl gstcMatchSetBagOpUnionDiffIntersect(ATermAppl Type)
{
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Res))||sort_bag::is_bag(sort_expression(Res)));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==2));

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

  Args=Args.tail();

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
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(Res,Res),Res);
}

static ATermAppl gstcMatchSetOpSetCompl(ATermAppl Type)
{
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res(1));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==1));

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
  Arg=aterm_cast<aterm_appl>(Arg(1));

  Res=gstcUnifyMinType(Res,Arg);
  if (aterm()==Res)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList1(static_cast<ATermAppl>(sort_set::set_(sort_expression(Res)))),sort_set::set_(sort_expression(Res)));
}

//Bags
static ATermAppl gstcMatchBagOpBag2Set(ATermAppl Type)
{
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res(1));

  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==1));

  ATermAppl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_bag::is_bag(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg(1));

  Arg=gstcUnifyMinType(Arg,Res);
  if (aterm()==Arg)
  {
    return aterm_appl();
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
  //assert(gsIsNat(aterm_cast<aterm_appl>(Type(1))));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  if (!(Args.size()==2))
  {
    return Type;
  }

  ATermAppl Arg1=ATAgetFirst(Args);

  Args=Args.tail();
  ATermAppl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  if (!sort_bag::is_bag(sort_expression(Arg2)))
  {
    return Type;
  }
  Arg2=aterm_cast<aterm_appl>(Arg2(1));

  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if (aterm()==Arg)
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList2(Arg,static_cast<ATermAppl>(sort_bag::bag(sort_expression(Arg)))),sort_nat::nat());
}


static ATermAppl gstcMatchFuncUpdate(ATermAppl Type)
{
  //tries to sort out the types of FuncUpdate ((A->B)xAxB->(A->B))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  ATermList Args=aterm_cast<aterm_list>(Type(0));
  assert((Args.size()==3));
  ATermAppl Arg1=ATAgetFirst(Args);
  assert(gsIsSortArrow(Arg1));
  Args=Args.tail();
  ATermAppl Arg2=ATAgetFirst(Args);
  Args=Args.tail();
  ATermAppl Arg3=ATAgetFirst(Args);
  ATermAppl Res=aterm_cast<aterm_appl>(Type(1));
  assert(gsIsSortArrow(Res));

  Arg1=gstcUnifyMinType(Arg1,Res);
  if (aterm()==Arg1)
  {
    return aterm_appl();
  }

  // determine A and B from Arg1:
  ATermList LA=aterm_cast<aterm_list>(Arg1(0));
  assert((LA.size()==1));
  ATermAppl A=ATAgetFirst(LA);
  ATermAppl B=aterm_cast<aterm_appl>(Arg1(1));

  if (aterm()==gstcUnifyMinType(A,Arg2))
  {
    return aterm_appl();
  }
  if (aterm()==gstcUnifyMinType(B,Arg3))
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(ATmakeList3(Arg1,A,B),Arg1);
}

static void gstcErrorMsgCannotCast(ATermAppl CandidateType, ATermList Arguments, ATermList ArgumentTypes)
{
  //prints more information about impossible cast.
  //at this point we know that Arguments cannot be cast to CandidateType. We need to find out why and print.
  assert(Arguments.size()==ArgumentTypes.size());

  ATermList CandidateList;
  if (gsIsSortsPossible(CandidateType))
  {
    CandidateList=aterm_cast<aterm_list>(CandidateType(0));
  }
  else
  {
    CandidateList=ATmakeList1(CandidateType);
  }

  ATermList NewCandidateList;
  for (ATermList l=CandidateList; !l.empty(); l=l.tail())
  {
    ATermAppl Candidate=ATAgetFirst(l);
    if (!gsIsSortArrow(Candidate))
    {
      continue;
    }
    NewCandidateList=push_front<aterm>(NewCandidateList,aterm_cast<aterm_list>(Candidate(0)));
  }
  CandidateList=reverse(NewCandidateList);

  //CandidateList=gstcTraverseListList(CandidateList);
  ATermList CurrentCandidateList=CandidateList;
  CandidateList=aterm_list();
  while (true)
  {
    ATermList NewCurrentCandidateList;
    ATermList NewList;
    for (ATermList l=CurrentCandidateList; !l.empty(); l=l.tail())
    {
      ATermList List=ATLgetFirst(l);
      if (!List.empty())
      {
        NewList=push_front<aterm>(NewList,ATAgetFirst(List));
        NewCurrentCandidateList=ATinsertUnique(NewCurrentCandidateList,List.tail());
      }
      else
      {
        NewCurrentCandidateList=push_front<aterm>(NewCurrentCandidateList,aterm_list());
      }
    }
    if (NewList.empty())
    {
      break;
    }
    CurrentCandidateList=reverse(NewCurrentCandidateList);
    CandidateList=push_front<aterm>(CandidateList,reverse(NewList));
  }
  CandidateList=reverse(CandidateList);

  for (ATermList l=Arguments, m=ArgumentTypes, n=CandidateList; !(l.empty()||m.empty()||n.empty()); l=l.tail(), m=m.tail(), n=n.tail())
  {
    ATermList PosTypes=ATLgetFirst(n);
    ATermAppl NeededType=ATAgetFirst(m);
    bool found=true;
    for (ATermList k=PosTypes; !k.empty(); k=k.tail())
    {
      if (aterm()!=gstcTypeMatchA(ATAgetFirst(k),NeededType))
      {
        found=false;
        break;
      }
    }
    if (found)
    {
      ATermAppl Sort;
      if (PosTypes.size()==1)
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
    ATermAppl NewArg=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm(0)));
    if (aterm()==NewArg)
    {
      return aterm_appl();
    }
    return StateFrm.set_argument(NewArg,0);
  }

  if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm) || gsIsStateImp(StateFrm))
  {
    ATermAppl NewArg1=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm(0)));
    if (aterm()==NewArg1)
    {
      return aterm_appl();
    }
    ATermAppl NewArg2=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm(1)));
    if (aterm()==NewArg2)
    {
      return aterm_appl();
    }
    return StateFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=aterm_cast<aterm_list>(StateFrm(0));
    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,VarList,NewVars))
    {
      return aterm_appl();
    }

    ATermAppl NewArg2=gstcTraverseStateFrm(NewVars,StateVars,aterm_cast<aterm_appl>(StateFrm(1)));
    if (aterm()==NewArg2)
    {
      return aterm_appl();
    }

    return StateFrm.set_argument(NewArg2,1);
  }

  if (gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm))
  {
    ATermAppl RegFrm=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(StateFrm(0)));
    if (aterm()==RegFrm)
    {
      return aterm_appl();
    }
    ATermAppl NewArg2=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm(1)));
    if (aterm()==NewArg2)
    {
      return aterm_appl();
    }
    return StateFrm.set_argument(RegFrm,0).set_argument(NewArg2,1);
  }

  if (gsIsStateDelayTimed(StateFrm) || gsIsStateYaledTimed(StateFrm))
  {
    ATermAppl Time=aterm_cast<aterm_appl>(StateFrm(0));
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (aterm()==NewType)
    {
      return aterm_appl();
    }

    if (aterm()==gstcTypeMatchA(sort_real::real_(),NewType))
    {
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,&Time);
      if (aterm()==CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast time value " << core::pp_deprecated(Time) << " to type Real (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
        return aterm_appl();
      }
    }
    return StateFrm.set_argument(Time,0);
  }

  if (gsIsStateVar(StateFrm))
  {
    ATermAppl StateVarName=aterm_cast<aterm_appl>(StateFrm(0));
    ATermList TypeList=aterm_cast<aterm_list>(ATtableGet(StateVars,StateVarName));
    if (aterm()==TypeList)
    {
      mCRL2log(error) << "undefined state variable " << core::pp_deprecated(StateVarName) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
      return aterm_appl();
    }

    ATermList Pars=aterm_cast<aterm_list>(StateFrm(1));
    if (TypeList.size()!=Pars.size())
    {
      mCRL2log(error) << "incorrect number of parameters for state variable " << core::pp_deprecated(StateVarName) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
      return aterm_appl();
    }

    ATermList r;
    bool success=true;
    for (; !Pars.empty(); Pars=Pars.tail(),TypeList=TypeList.tail())
    {
      ATermAppl Par=ATAgetFirst(Pars);
      ATermAppl ParType=ATAgetFirst(TypeList);
      ATermAppl NewParType=gstcTraverseVarConsTypeD(Vars,Vars,&Par,gstcExpandNumTypesDown(ParType));
      if (aterm()==NewParType)
      {
        mCRL2log(error) << "typechecking " << core::pp_deprecated(StateFrm) << std::endl;
        success=false;
        break;
      }

      if (aterm()==gstcTypeMatchA(ParType,NewParType))
      {
        //upcasting
        NewParType=gstcUpCastNumericType(ParType,NewParType,&Par);
        if (aterm()==NewParType)
        {
          mCRL2log(error) << "cannot (up)cast " << core::pp_deprecated(Par) << " to type " << core::pp_deprecated(ParType) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << ")" << std::endl;
          success=false;
          break;
        }
      }

      r=push_front<aterm>(r,Par);
    }

    if (!success)
    {
      return aterm_appl();
    }

    return StateFrm.set_argument(reverse(r),1);

  }

  if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm))
  {
    ATermTable CopyStateVars=ATtableCreate(63,50);
    gstcATermTableCopy(StateVars,CopyStateVars);

    // Make the new state variable:
    ATermTable FormPars=ATtableCreate(63,50);
    ATermList r;
    ATermList t;
    bool success=true;
    for (ATermList l=aterm_cast<aterm_list>(StateFrm(1)); !l.empty(); l=l.tail())
    {
      ATermAppl o=ATAgetFirst(l);

      ATermAppl VarName=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(o(0))(0));
      if (aterm()!=aterm_cast<aterm_appl>(ATtableGet(FormPars,VarName)))
      {
        mCRL2log(error) << "non-unique formal parameter " << core::pp_deprecated(VarName) << " (typechecking " << core::pp_deprecated(StateFrm) << ")" << std::endl;
        success=false;
        break;
      }

      ATermAppl VarType=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(o(0))(1));
      if (!gstcIsSortExprDeclared(VarType))
      {
        mCRL2log(error) << "type error occurred while typechecking " << core::pp_deprecated(StateFrm) << std::endl;
        success=false;
        break;
      }

      FormPars.put(VarName, VarType);

      ATermAppl VarInit=aterm_cast<aterm_appl>(o(1));
      ATermAppl VarInitType=gstcTraverseVarConsTypeD(Vars,Vars,&VarInit,gstcExpandNumTypesDown(VarType));
      if (aterm()==VarInitType)
      {
        mCRL2log(error) << "typechecking " << core::pp_deprecated(StateFrm) << std::endl;
        success=false;
        break;
      }

      if (aterm()==gstcTypeMatchA(VarType,VarInitType))
      {
        //upcasting
        VarInitType=gstcUpCastNumericType(VarType,VarInitType,&VarInit);
        if (aterm()==VarInitType)
        {
          mCRL2log(error) << "cannot (up)cast " << core::pp_deprecated(VarInit) << " to type " << core::pp_deprecated(VarType) << " (typechecking state formula " << core::pp_deprecated(StateFrm) << std::endl;
          success=false;
          break;
        }
      }

      r=push_front<aterm>(r,o.set_argument(VarInit,1));
      t=push_front<aterm>(t,VarType);
    }

    if (!success)
    {
      return aterm_appl();
    }

    StateFrm=StateFrm.set_argument(reverse(r),1);
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);
    gstcATermTableCopy(FormPars,CopyVars);

    CopyStateVars.put(aterm_cast<aterm_appl>(StateFrm(0)),reverse(t));
    ATermAppl NewArg=gstcTraverseStateFrm(CopyVars,CopyStateVars,aterm_cast<aterm_appl>(StateFrm(2)));
    if (aterm()==NewArg)
    {
      mCRL2log(error) << "while typechecking " << core::pp_deprecated(StateFrm) << std::endl;
      return aterm_appl();
    }
    return StateFrm.set_argument(NewArg,2);
  }

  if (gsIsDataExpr(StateFrm))
  {
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars, Vars, &StateFrm, sort_bool::bool_());
    if (aterm()==Type)
    {
      return aterm_appl();
    }
    return StateFrm;
  }

  assert(0);
  return aterm_appl();
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
    ATermAppl NewArg1=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm(0)));
    if (aterm()==NewArg1)
    {
      return aterm_appl();
    }
    ATermAppl NewArg2=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm(1)));
    if (aterm()==NewArg2)
    {
      return aterm_appl();
    }
    return RegFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsRegTrans(RegFrm) || gsIsRegTransOrNil(RegFrm))
  {
    ATermAppl NewArg=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm(0)));
    if (aterm()==NewArg)
    {
      return aterm_appl();
    }
    return RegFrm.set_argument(NewArg,0);
  }

  if (gsIsActFrm(RegFrm))
  {
    return gstcTraverseActFrm(Vars, RegFrm);
  }

  assert(0);
  return aterm_appl();
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
    ATermAppl NewArg=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm(0)));
    if (aterm()==NewArg)
    {
      return aterm_appl();
    }
    return ActFrm.set_argument(NewArg,0);
  }

  if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm) || gsIsActImp(ActFrm))
  {
    ATermAppl NewArg1=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm(0)));
    if (aterm()==NewArg1)
    {
      return aterm_appl();
    }
    ATermAppl NewArg2=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm(1)));
    if (aterm()==NewArg2)
    {
      return aterm_appl();
    }
    return ActFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm))
  {
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=aterm_cast<aterm_list>(ActFrm(0));
    ATermTable NewVars;
    if (!gstcAddVars2Table(CopyVars,VarList,NewVars))
    {
      return aterm_appl();
    }

    ATermAppl NewArg2=gstcTraverseActFrm(NewVars,aterm_cast<aterm_appl>(ActFrm(1)));
    if (aterm()==NewArg2)
    {
      return aterm_appl();
    }

    return ActFrm.set_argument(NewArg2,1);
  }

  if (gsIsActAt(ActFrm))
  {
    ATermAppl NewArg1=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm(0)));
    if (aterm()==NewArg1)
    {
      return aterm_appl();
    }

    ATermAppl Time=aterm_cast<aterm_appl>(ActFrm(1));
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,Vars,&Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (aterm()==NewType)
    {
      return aterm_appl();
    }

    if (aterm()==gstcTypeMatchA(sort_real::real_(),NewType))
    {
      //upcasting
      ATermAppl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,&Time);
      if (aterm()==CastedNewType)
      {
        mCRL2log(error) << "cannot (up)cast time value " << core::pp_deprecated(Time) << " to type Real (typechecking action formula " << core::pp_deprecated(ActFrm) << ")" << std::endl;
        return aterm_appl();
      }
    }
    return ActFrm.set_argument(NewArg1,0).set_argument(Time,1);
  }

  if (gsIsMultAct(ActFrm))
  {
    ATermList r;
    for (ATermList l=aterm_cast<aterm_list>(ActFrm(0)); !l.empty(); l=l.tail())
    {
      ATermAppl o=ATAgetFirst(l);
      assert(gsIsParamId(o));
      o=gstcTraverseActProcVarConstP(Vars,o);
      if (aterm()==o)
      {
        return aterm_appl();
      }
      r=push_front<aterm>(r,o);
    }
    return ActFrm.set_argument(reverse(r),0);
  }

  if (gsIsDataExpr(ActFrm))
  {
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars, Vars, &ActFrm, sort_bool::bool_());
    if (aterm()==Type)
    {
      return aterm_appl();
    }
    return ActFrm;
  }

  assert(0);
  return aterm_appl();
}

}
}
