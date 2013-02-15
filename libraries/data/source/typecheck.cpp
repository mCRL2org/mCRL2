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
#include "mcrl2/atermpp/substitute.h"
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

// #define pp core::pp_deprecated

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
  table constants;   //name -> Set(sort expression)
  table functions;   //name -> Set(sort expression)
} gsSystem;

static gsSystem gssystem;

static aterm_list list_minus(aterm_list l, aterm_list m)
{
  aterm_list n;
  for (; !l.empty(); l=l.tail())
  {
    if (std::find(m.begin(),m.end(),l.front()) == m.end())
    {
      n.push_front(l.front());
    }
  }
  return reverse(n);
}

// the static context of the spec will be checked and used, not transformed
typedef struct
{
  indexed_set basic_sorts;
  table defined_sorts; //name -> sort expression
  table constants;   //name -> Set(sort expression)
  table functions;   //name -> Set(sort expression)
  table actions;         //name -> Set(List(sort expression)) because of action polymorphism
  table processes;         //name -> Set(List(sort expression)) because of process polymorphism
  table glob_vars;   //name -> Type: global variables (for proc, pbes and init)
  table PBs;
} Context;

static Context context;

// the body may be transformed
typedef struct
{
  aterm_list equations;
  table proc_pars;         //name#type -> List(Vars)
  table proc_bodies; //name#type -> rhs
} Body;
static Body body;

// Static function declarations
static void gstcDataInit(void);
static void gstcReadInSorts(aterm_list);
static void gstcReadInConstructors(aterm_list NewSorts=aterm_list(aterm()));
static void gstcReadInFuncs(aterm_list, aterm_list);
static void gstcReadInActs(aterm_list);
static void gstcReadInProcsAndInit(aterm_list, aterm_appl);
static void gstcReadInPBESAndInit(aterm_appl, aterm_appl);

static bool gstcTransformVarConsTypeData(void);
static bool gstcTransformActProcVarConst(void);
static bool gstcTransformPBESVarConst(void);

static aterm_list gstcWriteProcs(aterm_list);
static aterm_list gstcWritePBES(aterm_list);

static bool gstcInTypesA(aterm_appl, aterm_list);
static bool gstcEqTypesA(aterm_appl, aterm_appl);
static bool gstcInTypesL(aterm_list, aterm_list);
static bool gstcEqTypesL(aterm_list, aterm_list);

static void gstcIsSortDeclared(aterm_appl SortName);
static void gstcIsSortExprDeclared(const aterm_appl &SortExpr);
static void gstcIsSortExprListDeclared(aterm_list SortExprList);
static void gstcReadInSortStruct(aterm_appl);
static void gstcAddConstant(aterm_appl, const std::string);
static void gstcAddFunction(aterm_appl, const std::string, bool allow_double_decls=false);

static void gstcAddSystemConstant(aterm_appl);
static void gstcAddSystemFunction(aterm_appl);

static void gstcATermTableCopy(const table &Vars, table &CopyVars);

static void gstcAddVars2Table(table &,aterm_list, table &);
static table gstcRemoveVars(table &Vars, aterm_list VarDecls);
static bool gstcVarsUnique(aterm_list VarDecls);
static aterm_appl gstcRewrActProc(const table &, aterm_appl, bool is_pbes=false);
static inline aterm_appl gstcMakeActionOrProc(bool, aterm_appl, aterm_list, aterm_list);
static aterm_appl gstcTraverseActProcVarConstP(const table &, aterm_appl);
static aterm_appl gstcTraversePBESVarConstPB(const table &, aterm_appl);

static aterm_appl gstcTraverseVarConsTypeD(const table &DeclaredVars, 
                                           const table &AllowedVars, 
                                           data_expression &, 
                                           aterm_appl, 
                                           table &FreeVars, 
                                           bool strictly_ambiguous=true, const bool warn_upcasting=false, const bool print_cast_error=true);
static aterm_appl gstcTraverseVarConsTypeD(const table &DeclaredVars, const table &AllowedVars, data_expression &t1, aterm_appl t2)
{
  table dummy_table;
  return gstcTraverseVarConsTypeD(DeclaredVars, AllowedVars, t1, t2,
        dummy_table, true, false, true);
}
static aterm_appl gstcTraverseVarConsTypeDN(const table &DeclaredVars, const table &AllowedVars, data_expression & , aterm_appl,
               table &FreeVars, bool strictly_ambiguous=true, size_t nPars = std::string::npos, const bool warn_upcasting=false, const bool print_cast_error=true);

static aterm_list gstcInsertType(aterm_list TypeList, aterm_appl Type);

static inline bool gstcIsPos(aterm_appl Number)
{
  char c=gsATermAppl2String(Number)[0];
  return (bool)(isdigit(c) && c>'0');
}
static inline bool gstcIsNat(aterm_appl Number)
{
  return isdigit(gsATermAppl2String(Number)[0]) != 0;
}

static inline aterm_appl INIT_KEY(void)
{
  return gsMakeProcVarId(gsString2ATermAppl("init"),aterm_list());
}

static aterm_appl gstcUpCastNumericType(aterm_appl NeededType, aterm_appl Type, data_expression &Par, bool warn_upcasting=false);
static aterm_appl gstcMaximumType(aterm_appl Type1, aterm_appl Type2);

static aterm_list gstcGetNotInferredList(aterm_list TypeListList);
static aterm_list gstcAdjustNotInferredList(aterm_list TypeList, aterm_list TypeListList);
static bool gstcIsNotInferredL(aterm_list TypeListList);
static bool gstcIsTypeAllowedA(aterm_appl Type, aterm_appl PosType);
static bool gstcIsTypeAllowedL(aterm_list TypeList, aterm_list PosTypeList);
static aterm_appl gstcUnwindType(aterm_appl Type);
static aterm_appl gstcUnSet(aterm_appl PosType);
static aterm_appl gstcUnBag(aterm_appl PosType);
static aterm_appl gstcUnList(aterm_appl PosType);
static aterm_appl gstcUnArrowProd(aterm_list ArgTypes, aterm_appl PosType);
static aterm_list gstcTypeListsIntersect(aterm_list TypeListList1, aterm_list TypeListList2);
static aterm_list gstcGetVarTypes(aterm_list VarDecls);
static aterm_appl gstcTypeMatchA(aterm_appl Type, aterm_appl PosType);
static aterm_list gstcTypeMatchL(aterm_list TypeList, aterm_list PosTypeList);
static bool gstcHasUnknown(aterm_appl Type);
static bool gstcIsNumericType(aterm_appl Type);
static aterm_appl gstcExpandNumTypesUp(aterm_appl Type);
static aterm_appl gstcExpandNumTypesDown(aterm_appl Type);
static aterm_appl gstcMinType(aterm_list TypeList);
static bool gstcMActIn(aterm_list MAct, aterm_list MActs);
static bool gstcMActEq(aterm_list MAct1, aterm_list MAct2);
static aterm_appl gstcUnifyMinType(aterm_appl Type1, aterm_appl Type2);
static aterm_appl gstcMatchIf(aterm_appl Type);
static aterm_appl gstcMatchEqNeqComparison(aterm_appl Type);
static aterm_appl gstcMatchListOpCons(aterm_appl Type);
static aterm_appl gstcMatchListOpSnoc(aterm_appl Type);
static aterm_appl gstcMatchListOpConcat(aterm_appl Type);
static aterm_appl gstcMatchListOpEltAt(aterm_appl Type);
static aterm_appl gstcMatchListOpHead(aterm_appl Type);
static aterm_appl gstcMatchListOpTail(aterm_appl Type);
static aterm_appl gstcMatchSetOpSet2Bag(aterm_appl Type);
static aterm_appl gstcMatchListSetBagOpIn(aterm_appl Type);
static aterm_appl gstcMatchSetBagOpUnionDiffIntersect(aterm_appl Type);
static aterm_appl gstcMatchSetOpSetCompl(aterm_appl Type);
static aterm_appl gstcMatchBagOpBag2Set(aterm_appl Type);
static aterm_appl gstcMatchBagOpBagCount(aterm_appl Type);
static aterm_appl gstcMatchFuncUpdate(aterm_appl Type);
static aterm_appl replace_possible_sorts(aterm_appl Type);


static void gstcErrorMsgCannotCast(aterm_appl CandidateType, aterm_list Arguments, aterm_list ArgumentTypes);

// Typechecking modal formulas
static aterm_appl gstcTraverseStateFrm(const table &Vars, const table &StateVars, aterm_appl StateFrm);
static aterm_appl gstcTraverseRegFrm(const table &Vars, aterm_appl RegFrm);
static aterm_appl gstcTraverseActFrm(const table &Vars, aterm_appl ActFrm);


static aterm_appl gstcFoldSortRefs(aterm_appl Spec);
//Pre: Spec is a specification that adheres to the internal syntax after
//     type checking
//Ret: Spec in which all sort references are maximally folded, i.e.:
//     - sort references to SortId's and SortArrow's are removed from the
//       rest of Spec (including the other sort references) by means of
//       forward substitition
//     - other sort references are removed from the rest of Spec by means of
//       backward substitution
//     - self references are removed, i.e. sort references of the form A = A

static aterm_list gstcFoldSortRefsInSortRefs(aterm_list SortRefs);
//Pre: SortRefs is a list of sort references
//Ret: SortRefs in which all sort references are maximally folded

static void gstcSplitSortDecls(aterm_list SortDecls, aterm_list* PSortIds,
                               aterm_list* PSortRefs);
//Pre: SortDecls is a list of SortId's and SortRef's
//Post:*PSortIds and *PSortRefs contain the SortId's and SortRef's from
//     SortDecls, in the same order

static aterm_appl gstcUpdateSortSpec(aterm_appl Spec, aterm_appl SortSpec);
//Pre: Spec and SortSpec are resp. specifications and sort specifications that
//     adhere to the internal syntax after type checking
//Ret: Spec in which the sort specification is replaced by SortSpec

///\brief Increases the value of each key in map
///\param[in] m A mapping from an aterm to a boolean
///\return m in which all values are negated
static inline std::map<atermpp::aterm,bool> neg_values(std::map<atermpp::aterm,bool> m)
{
  for (std::map<atermpp::aterm,bool>::iterator i = m.begin() ; i != m.end(); i++)
  {
    m[i->first] = !i->second;
  }
  return m;
}

/* aterm extensions */
/**
 * \brief Conditional prepend operation on aterm_list
 * \param[in] list an aterm list
 * \param[in] el the aterm to prepend
 * \return el ++ list if not el in list, list if el in list
**/
inline aterm_list ATinsertUnique(const aterm_list &list, const aterm &el)
{
  if (std::find(list.begin(),list.end(), el) == list.end())
  {
    aterm_list result=list;
    result.push_front(el);
    return result;
  }
  return list;
}

inline
size_t     ATindexedSetPut(indexed_set &set, const aterm &elem, bool* isnew)
{
  std::pair<size_t, bool> p= set.put(elem);
  *isnew=p.second;
  return p.first;
}


inline
aterm_list ATreplace(const aterm_list &list_in, const aterm &el, const size_t idx) // Replace one element of a list.
{
  aterm_list list=list_in;
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
  list.push_front(el);
  /* Add the prefix */
  for (i=idx; i>0; i--)
  {
    list.push_front(buffer[i-1]);
  }

  return list;
}

static aterm gsSubstValuesTable(const table &Substs, const aterm &t)
{
  aterm Term=t;
  aterm Result = Substs.get(Term);
  if (Result.defined())
  {
    return Result;
  }

  //distribute substitutions over the arguments/elements of Term
  if (Term.type_is_appl())
  {
    //Term is an aterm_appl; distribute substitutions over the arguments
    atermpp::function_symbol Head = aterm_cast<aterm_appl>(Term).function();
    const size_t NrArgs = Head.arity();
    if (NrArgs > 0)
    {
      std::vector <aterm> Args;
      for (size_t i = 0; i < NrArgs; i++)
      {
         Args.push_back(gsSubstValuesTable(Substs, ((aterm_appl) Term)[i]));
      }
      return aterm_appl(Head, Args.begin(), Args.end());
    }
    else
    {
      return Term;
    }
  }
  else if (Term.type_is_list())
  {
    //Term is an aterm_list; distribute substitutions over the elements
    aterm_list Result;
    while (!((aterm_list) Term).empty())
    {
      Result.push_front(gsSubstValuesTable(Substs, ((aterm_list) Term).front()));
      Term = ((aterm_list) Term).tail();
    }
    return reverse(Result);
  }
  else
  {
    return Term;
  }

}


/**
 *  * \brief Gets the first argument as aterm_appl
 *   **/
inline const aterm_appl &ATAgetFirst(const aterm_list &List)
{
  const aterm &Result = List.front();
  return aterm_cast<const aterm_appl>(Result);
}

/**
 * \brief Gets the first argument as aterm_list
**/
inline const aterm_list &ATLgetFirst(const aterm_list &List)
{
  const aterm &Result = List.front();
  return aterm_cast<const aterm_list>(Result);
}

//type checking functions
//-----------------------

aterm_appl type_check_data_spec(aterm_appl data_spec)
{
  mCRL2log(verbose) << "type checking data specification..." << std::endl;

  aterm_appl Result;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking read-in phase started" << std::endl;

  gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]));
  
  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  gstcReadInConstructors();
  
  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                      aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));
  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);
  mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

  mCRL2log(debug) << "type checking transform VarConst phase started" << std::endl;

  if (gstcTransformVarConsTypeData())
  {
    mCRL2log(debug) << "type checking transform VarConst phase finished" << std::endl;

    Result = data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);

    Result = gstcFoldSortRefs(Result);

    mCRL2log(debug) << "type checking phase finished" << std::endl;
  }
  
  return Result;
}

aterm_appl type_check_proc_spec(aterm_appl proc_spec)
{
  mCRL2log(verbose) << "type checking process specification..." << std::endl;

  aterm_appl Result;

  mCRL2log(debug) << "type checking phase started: " << pp(proc_spec) << "" << std::endl;
  gstcDataInit();

  aterm_appl data_spec = aterm_cast<aterm_appl>(proc_spec[0]);
  gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]));
  
  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  gstcReadInConstructors();
  
  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                      aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));
  
  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);

  gstcReadInActs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[1])[0]));
  
  aterm_appl glob_var_spec = aterm_cast<aterm_appl>(proc_spec[2]);
  aterm_list glob_vars = aterm_cast<aterm_list>(glob_var_spec[0]);
  table dummy;
  gstcAddVars2Table(context.glob_vars, glob_vars,dummy);
  
  gstcReadInProcsAndInit(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[3])[0]),
                             aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(proc_spec[4])[0]));
  
  mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

  mCRL2log(debug) << "type checking transform ActProc+VarConst phase started" << std::endl;
  if (gstcTransformVarConsTypeData())
  {
    if (gstcTransformActProcVarConst())
    {
      mCRL2log(debug) << "type checking transform ActProc+VarConst phase finished" << std::endl;

      data_spec=aterm_cast<aterm_appl>(proc_spec[0]);
      data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
      Result=proc_spec.set_argument(data_spec,0);
      Result=Result.set_argument(gsMakeProcEqnSpec(gstcWriteProcs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[3])[0]))),3);
      Result=Result.set_argument(gsMakeProcessInit(aterm_cast<aterm_appl>(body.proc_bodies.get(INIT_KEY()))),4);

      Result=gstcFoldSortRefs(Result);

      mCRL2log(debug) << "type checking phase finished" << std::endl;
    }
  }
  
  
  
  return Result;
}

aterm_appl type_check_sort_expr(aterm_appl sort_expr, aterm_appl spec)
{
  mCRL2log(verbose) << "type checking sort expression..." << std::endl;
  //check correctness of the sort expression in sort_expr
  //using the specification in spec
  assert(gsIsSortExpr(sort_expr));
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));

  aterm_appl Result;

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of sort expressions read-in phase started" << std::endl;

  aterm_appl data_spec;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = aterm_cast<aterm_appl>(spec[0]);
  }
  aterm_list sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]);

  //XXX read-in from spec (not finished)
  try
  { 
    gstcReadInSorts(sorts);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading Sorts from LPS failed");
  }
  
  mCRL2log(debug) << "type checking of sort expressions read-in phase finished" << std::endl;

  if (!is_unknown_sort(sort_expr) && !is_multiple_possible_sorts(sort_expr))
  {
    gstcIsSortExprDeclared(sort_expr);
    
    Result=sort_expr;
  }
  else
  {
    throw mcrl2::runtime_error("type checking of sort expressions failed (" + pp(sort_expr) + ") is not a sort expression)");
    // mCRL2log(error) << "type checking of sort expressions failed (" << atermpp::aterm(sort_expr) << ") is not a sort expression)" << std::endl;
  }
  

  return Result;
}

aterm_appl type_check_data_expr(const data_expression &data_expr, aterm_appl sort_expr, aterm_appl spec, const table &Vars)
{
  mCRL2log(verbose) << "type checking data expression..." << std::endl;
  //check correctness of the data expression in data_expr using
  //the specification in spec

  //check preconditions
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  assert(gsIsDataExpr(data_expr));
  assert(!sort_expr.defined() || gsIsSortExpr(sort_expr));

  aterm_appl Result;

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of data expression read-in phase started" << std::endl;

  aterm_appl data_spec;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = aterm_cast<aterm_appl>(spec[0]);
  }
  aterm_list sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]);
  aterm_list constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]);
  aterm_list mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]);

  //XXX read-in from spec (not finished)
  try
  {
    gstcReadInSorts(sorts);
    gstcReadInConstructors();
    gstcReadInFuncs(constructors,mappings);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of data expression failed");
  }
  mCRL2log(debug) << "type checking of data expression read-in phase finished" << std::endl;

  if (sort_expr.defined() && (is_unknown_sort(sort_expr) || is_multiple_possible_sorts(sort_expr)))
  {
    throw mcrl2::runtime_error("type checking of data expression failed (" + pp(sort_expr) + " is not a sort expression)");
  }
  else if (!sort_expr.defined())
  {
    data_expression data=data_expr;
    aterm_appl Type=gstcTraverseVarConsTypeD(Vars,Vars,data,(sort_expr==aterm_appl())?(aterm_appl)data::unknown_sort():sort_expr);

    if (Type.defined() && !data::is_unknown_sort(data::sort_expression(Type)))
    {
      Result=data;
    }
    else 
    {
      throw mcrl2::runtime_error("type checking of data expression failed");
    }
  }
  else 
  {
    gstcIsSortExprDeclared(sort_expr);
  
    data_expression data=data_expr;
    aterm_appl Type=gstcTraverseVarConsTypeD(Vars,Vars,data,(sort_expr==aterm_appl())?(aterm_appl)data::unknown_sort():sort_expr);

    if (Type.defined() && !data::is_unknown_sort(data::sort_expression(Type)))
    {
      Result=data;
    }
    else 
    {
      throw mcrl2::runtime_error("type checking of data expression failed");
    }
  }

  return Result;
}

aterm_appl type_check_mult_act(
  aterm_appl mult_act,
  aterm_appl data_spec,
  aterm_list action_labels)
{
  mCRL2log(debug) << "type checking multiaction..." << std::endl;
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  // assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  aterm_appl Result;

  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of multiactions read-in phase started" << std::endl;

  aterm_list sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]);
  aterm_list constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]);
  aterm_list mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]);

  //XXX read-in from spec (not finished)
  try
  {
    gstcReadInSorts(sorts);
    gstcReadInConstructors();
    gstcReadInFuncs(constructors,mappings);
    gstcReadInActs(action_labels);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading from LPS failed");
    // mCRL2log(error) << "reading from LPS failed" << std::endl;
  }
  {
    mCRL2log(debug) << "type checking of multiactions read-in phase finished" << std::endl;

    if (gsIsMultAct(mult_act))
    {
      table Vars(63,50);
      aterm_list r;
      for (aterm_list l=aterm_cast<aterm_list>(mult_act[0]); !l.empty(); l=l.tail())
      {
        aterm_appl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        if (!o.defined())
        {
          return aterm_appl();
          // return NULL;
        }
        r.push_front(o);
      }
      Result=mult_act.set_argument(reverse(r),0);
    }
    else
    {
      throw mcrl2::runtime_error("type checking of multiactions failed (" + pp(mult_act) + " is not a multiaction)");
      // mCRL2log(error) << "type checking of multiactions failed (" << atermpp::aterm(mult_act) << " is not a multiaction)" << std::endl;
    }
  }
  return Result;
}

aterm_list type_check_mult_actions(
  aterm_list mult_actions,
  aterm_appl data_spec,
  aterm_list action_labels)
{
  mCRL2log(debug) << "type checking multiactions..." << std::endl;
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  // assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));

  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of multiactions read-in phase started" << std::endl;

  aterm_list sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]);
  aterm_list constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]);
  aterm_list mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]);

  //XXX read-in from spec (not finished)
  aterm_list result;
  try
  {
    gstcReadInSorts(sorts);
    gstcReadInConstructors();
    gstcReadInFuncs(constructors,mappings);
    gstcReadInActs(action_labels);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading data/action specification failed");
  }
  
  mCRL2log(debug) << "type checking of multiactions read-in phase finished" << std::endl;

  for (; !mult_actions.empty(); mult_actions=mult_actions.tail())
  {
    table Vars(63,50);
    aterm_list r;

    for (aterm_list l=(aterm_list)(mult_actions.front()) ; !l.empty(); l=l.tail())
    {
      aterm_appl o=ATAgetFirst(l);
      assert(gsIsParamId(o));
      o=gstcTraverseActProcVarConstP(Vars,o);
      if (!o.defined())
      {
        throw mcrl2::runtime_error("Typechecking action failed: "+ pp(ATAgetFirst(l)));
      }
      r.push_front(o);
    }
    result.push_front(reverse(r));
  }
  
  return reverse(result);
}

aterm_appl type_check_proc_expr(aterm_appl proc_expr, aterm_appl spec)
{
  mCRL2log(verbose) << "type checking process expression..." << std::endl;

  //check correctness of the process expression in proc_expr using
  //the process specification or LPS in spec
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  mCRL2log(warning) << "type checking of process expressions is not yet implemented" << std::endl;
  return proc_expr;
}

aterm_appl type_check_state_frm(aterm_appl state_frm, aterm_appl spec)
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

  aterm_appl Result;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of state formulas read-in phase started" << std::endl;

  aterm_appl data_spec = aterm_cast<aterm_appl>(spec[0]);
  aterm_list action_labels = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec[1])[0]);

  aterm_list sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]);
  aterm_list constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]);
  aterm_list mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]);

  //XXX read-in from spec (not finished)
  try
  { 
    gstcReadInSorts(sorts);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading sorts from LPS failed");
  }
  
  try 
  { 
    gstcReadInConstructors();
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading constructors from LPS failed.");
  }
  try
  {
    gstcReadInFuncs(constructors,mappings);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading functions from LPS failed.");
  }
  
  if (action_labels.defined())
  {
    try
    {
      gstcReadInActs(action_labels);
    }
    catch (mcrl2::runtime_error &e)
    {
      mCRL2log(warning) << std::string(e.what()) << "\nignoring the previous error(s), the formula will be typechecked without action label information" << std::endl;
    }
  }
  
  mCRL2log(debug) << "type checking of state formulas read-in phase finished" << std::endl;

  table Vars(63,50);
  Result=gstcTraverseStateFrm(Vars,Vars,state_frm);
  
  return Result;
}

aterm_appl type_check_action_rename_spec(aterm_appl ar_spec, aterm_appl lps_spec)
{

  mCRL2log(verbose) << "type checking action rename specification..." << std::endl;

  //check precondition
  assert(gsIsActionRenameSpec(ar_spec));

  aterm_appl Result;
  mCRL2log(debug) << "type checking phase started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of action rename specification read-in phase started" << std::endl;

  table actions_from_lps(63,50);

  aterm_appl lps_data_spec = aterm_cast<aterm_appl>(lps_spec[0]);
  aterm_list lps_sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_data_spec[0])[0]);
  aterm_list lps_constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_data_spec[1])[0]);
  aterm_list lps_mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_data_spec[2])[0]);
  aterm_list lps_action_labels = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(lps_spec[1])[0]);

  //XXX read-in from LPS (not finished)
  try
  { 
    gstcReadInSorts((aterm_list) lps_sorts);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading sorts from LPS failed");
  }
  
  try
  {
    gstcReadInConstructors();
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading structure constructors from LPS failed");
  }

  
  try
  { 
    gstcReadInFuncs(lps_constructors, lps_mappings);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading functions from LPS failed");
  }

  try
  { 
    gstcReadInActs(lps_action_labels);
  }
  catch (mcrl2::runtime_error &e)
  {
    mCRL2log(warning) << e.what() << "\nignoring the previous error(s), the formula will be typechecked without action label information" << std::endl;
  }
  mCRL2log(debug) << "type checking of action rename specification read-in phase of LPS finished" << std::endl;
  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file started" << std::endl;

  aterm_appl data_spec = aterm_cast<aterm_appl>(ar_spec[0]);
  aterm_list LPSSorts=context.defined_sorts.keys(); // remember the sorts from the LPS.
  gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]));
  
  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file sorts finished" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  gstcReadInConstructors(list_minus(context.defined_sorts.keys(),LPSSorts));

  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file constructors finished" << std::endl;

  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                       aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));
  
  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file functions finished" << std::endl;

  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);

  //Save the actions from LPS only for the latter use.
  gstcATermTableCopy(context.actions,actions_from_lps);
  gstcReadInActs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(ar_spec[1])[0]));

  mCRL2log(debug) << "type checking action rename specification read-in phase of the ActionRenameSpec finished" << std::endl;

  if (!gstcTransformVarConsTypeData())
  {
    return Result;
  }
  mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;

  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
  Result=ar_spec.set_argument(data_spec,0);
  Result=gstcFoldSortRefs(Result);


  // now the action renaming rules themselves.
  aterm_appl ActionRenameRules=aterm_cast<aterm_appl>(ar_spec[2]);
  aterm_list NewRules;

  table DeclaredVars(63,50);
  table FreeVars(63,50);

  bool b = true;

  for (aterm_list l=aterm_cast<aterm_list>(ActionRenameRules[0]); !l.empty(); l=l.tail())
  {
    aterm_appl Rule=ATAgetFirst(l);
    assert(gsIsActionRenameRule(Rule));

    aterm_list VarList=aterm_cast<aterm_list>(Rule[0]);
    if (!gstcVarsUnique(VarList))
    {
      b = false;
      throw mcrl2::runtime_error("the variables " + pp(VarList) + " in action rename rule " + pp(Rule) + " are not unique");
      // mCRL2log(error) << "the variables " << pp(VarList) << " in action rename rule " << pp(Rule) << " are not unique" << std::endl;
      break;
    }

    table NewDeclaredVars;
    gstcAddVars2Table(DeclaredVars,VarList,NewDeclaredVars);
    
    DeclaredVars=NewDeclaredVars;

    aterm_appl Left=aterm_cast<aterm_appl>(Rule[2]);
    assert(gsIsParamId(Left));
    {
      //extra check requested by Tom: actions in the LHS can only come from the LPS
      table temp=context.actions;
      context.actions=actions_from_lps;
      Left=gstcTraverseActProcVarConstP(DeclaredVars,Left);
      context.actions=temp;
      if (!Left.defined())
      {
        b = false;
        break;
      }
    }

    data_expression Cond=aterm_cast<aterm_appl>(Rule[1]);
    if (!gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Cond,sort_bool::bool_()).defined())
    {
      b = false;  // JK 15/10/2009 remove gsIsNil check
      break;
    }

    aterm_appl Right(Rule[3]);
    assert(gsIsParamId(Right) || gsIsTau(Right) || gsIsDelta(Right));
    Right=gstcTraverseActProcVarConstP(DeclaredVars,Right);
    if (!Right.defined())
    {
      b = false;
      break;
    }

    NewRules.push_front(gsMakeActionRenameRule(VarList,Cond,Left,Right));
  }
  if (!b)
  {
    Result = aterm_appl();
    return Result;
  }

  ActionRenameRules=ActionRenameRules.set_argument(reverse(NewRules),0);
  Result=Result.set_argument(ActionRenameRules,2);
  mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;
  
  
  

  return Result;
}

aterm_appl type_check_pbes_spec(aterm_appl pbes_spec)
{
  //check correctness of the PBES specification in pbes_spec

  mCRL2log(verbose) << "type checking PBES specification..." << std::endl;

  assert(gsIsPBES(pbes_spec));

  aterm_appl Result;
  mCRL2log(debug) << "type checking phase of PBES specifications started" << std::endl;
  gstcDataInit();

  mCRL2log(debug) << "type checking of PBES specification read-in phase started" << std::endl;


  aterm_appl data_spec = aterm_cast<aterm_appl>(pbes_spec[0]);
  aterm_appl pb_eqn_spec = aterm_cast<aterm_appl>(pbes_spec[2]);
  aterm_appl pb_init = aterm_cast<aterm_appl>(pbes_spec[3]);
  aterm_appl glob_var_spec = aterm_cast<aterm_appl>(pbes_spec[1]);
  table dummy;

  gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]));
    
  mCRL2log(debug) << "type checking of PBES specification read-in phase of sorts finished" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  gstcReadInConstructors();
  
  mCRL2log(debug) << "type checking of PBES specification read-in phase of constructors finished" << std::endl;

  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                       aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));

  mCRL2log(debug) << "type checking of PBES specification read-in phase of functions finished" << std::endl;

  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);

  gstcAddVars2Table(context.glob_vars, aterm_cast<aterm_list>(glob_var_spec[0]),dummy);
  
  mCRL2log(debug) << "type checking of PBES specification read-in phase of global variables finished" << std::endl;

  gstcReadInPBESAndInit(pb_eqn_spec,pb_init);
  
  mCRL2log(debug) << "type checking PBES read-in phase finished" << std::endl;

  mCRL2log(debug) << "type checking transform Data+PBES phase started" << std::endl;
  if (!gstcTransformVarConsTypeData())
  {
    return Result;
  }
  if (!gstcTransformPBESVarConst())
  {
    return Result;
  }
  mCRL2log(debug) << "type checking transform Data+PBES phase finished" << std::endl;

  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
  Result=pbes_spec.set_argument(data_spec,0);

  pb_eqn_spec=pb_eqn_spec.set_argument(gstcWritePBES(aterm_cast<aterm_list>(pb_eqn_spec[0])),0);
  Result=Result.set_argument(pb_eqn_spec,2);

  pb_init=pb_init.set_argument(aterm_cast<aterm_appl>(body.proc_bodies.get(INIT_KEY())),0);
  Result=Result.set_argument(pb_init,3);

  Result=gstcFoldSortRefs(Result);

  return Result;
}

aterm_list type_check_data_vars(aterm_list data_vars, aterm_appl spec)
{
  mCRL2log(verbose) << "type checking data variables..." << std::endl;
  //check correctness of the data variable declaration in sort_expr
  //using the specification in spec

  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));

  mCRL2log(debug) << "type checking phase started" << std::endl;

  gstcDataInit();

  mCRL2log(debug) << "type checking of data variables read-in phase started" << std::endl;

  aterm_appl data_spec;
  if (gsIsDataSpec(spec))
  {
    data_spec = spec;
  }
  else
  {
    data_spec = aterm_cast<aterm_appl>(spec[0]);
  }
  aterm_list sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]);

  //XXX read-in from spec (not finished)
  try
  {
    gstcReadInSorts(sorts);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(to_string(e.what()) + "\nreading from LPS failed");
  }
  
  mCRL2log(debug) << "type checking of data variables read-in phase finished" << std::endl;

  table Vars(63,50);
  table NewVars;
  try
  {
    gstcAddVars2Table(Vars,data_vars,NewVars);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking data variables");
  }
  

  return data_vars;
}

//local functions
//---------------

// fold functions

void gstcSplitSortDecls(aterm_list SortDecls, aterm_list* PSortIds,
                        aterm_list* PSortRefs)
{
  aterm_list SortIds;
  aterm_list SortRefs;
  while (!SortDecls.empty())
  {
    aterm_appl SortDecl = ATAgetFirst(SortDecls);
    if (gsIsSortRef(SortDecl))
    {
      SortRefs.push_front(SortDecl);
    }
    else     //gsIsSortId(SortDecl)
    {
      SortIds.push_front(SortDecl);
    }
    SortDecls = SortDecls.tail();
  }
  *PSortIds = reverse(SortIds);
  *PSortRefs = reverse(SortRefs);
}

aterm_appl gstcUpdateSortSpec(aterm_appl Spec, aterm_appl SortSpec)
{
  assert(gsIsDataSpec(Spec) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) || gsIsPBES(Spec) || gsIsActionRenameSpec(Spec));
  assert(gsIsSortSpec(SortSpec));
  if (gsIsDataSpec(Spec))
  {
    Spec = Spec.set_argument( SortSpec, 0);
  }
  else
  {
    aterm_appl DataSpec = aterm_cast<aterm_appl>(Spec[0]);
    DataSpec = DataSpec.set_argument( SortSpec, 0);
    Spec = Spec.set_argument( DataSpec, 0);
  }
  return Spec;
}

aterm_appl gstcFoldSortRefs(aterm_appl Spec)
{
  assert(gsIsDataSpec(Spec) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) || gsIsPBES(Spec) || gsIsActionRenameSpec(Spec));
  mCRL2log(debug) << "specification before folding:" << pp(Spec) << "" << std::endl;
  //get sort declarations
  aterm_appl DataSpec;
  if (gsIsDataSpec(Spec))
  {
    DataSpec = Spec;
  }
  else
  {
    DataSpec = aterm_cast<aterm_appl>(Spec[0]);
  }
  aterm_appl SortSpec = aterm_cast<aterm_appl>(DataSpec[0]);
  aterm_list SortDecls = aterm_cast<aterm_list>(SortSpec[0]);
  //split sort declarations in sort id's and sort references
  aterm_list SortIds=aterm_list(aterm());
  aterm_list SortRefs=aterm_list(aterm());
  gstcSplitSortDecls(SortDecls, &SortIds, &SortRefs);
  //fold sort references in the sort references themselves
  SortRefs = gstcFoldSortRefsInSortRefs(SortRefs);
  //substitute sort references in the rest of Spec, i.e.
  //(a) remove sort references from Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(SortIds));
  //(b) build substitution table
  table Substs(2*SortRefs.size(),50);
  aterm_list l = SortRefs;
  while (!l.empty())
  {
    aterm_appl SortRef = ATAgetFirst(l);
    //add substitution for SortRef
    aterm_appl LHS = gsMakeSortId(aterm_cast<aterm_appl>(SortRef[0]));
    aterm_appl RHS = aterm_cast<aterm_appl>(SortRef[1]);
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
  aterm_appl NewSpec = Spec;
  do
  {
    mCRL2log(debug) << "substituting sort references in specification" << std::endl;
    Spec = NewSpec;
    NewSpec = (aterm_appl) gsSubstValuesTable(Substs, Spec);
  }
  while (NewSpec!=Spec);

  //add the removed sort references back to Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(SortIds+ SortRefs));
  mCRL2log(debug) << "specification after folding:\n" << pp(Spec) << "\n" ;
  return Spec;
}

aterm_list gstcFoldSortRefsInSortRefs(aterm_list SortRefs)
{
  //fold sort references in SortRefs by means of repeated forward and backward
  //substitution
  aterm_list NewSortRefs = SortRefs;
  size_t n = SortRefs.size();
  //perform substitutions until the list of sort references becomes stable
  do
  {
    SortRefs = NewSortRefs;
    mCRL2log(debug) << "SortRefs contains the following sort references:\n" << pp(gsMakeSortSpec(SortRefs)) << "" << std::endl;
    //perform substitutions implied by sort references in NewSortRefs to the
    //other elements in NewSortRefs
    for (size_t i = 0; i < n; i++)
    {
      aterm_appl SortRef = ATAelementAt(NewSortRefs, i);
      //turn SortRef into a substitution
      aterm_appl LHS = gsMakeSortId(aterm_cast<aterm_appl>(SortRef[0]));
      aterm_appl RHS = aterm_cast<aterm_appl>(SortRef[1]);
      const substitution Subst=((gsIsSortId(RHS) || gsIsSortArrow(RHS))?substitution(LHS, RHS):substitution(RHS, LHS));
      // mCRL2log(debug) << "performing substition " << pp(Subst[0]) << " := " << pp(Subst[1]) << "" << std::endl;
      //perform Subst on all elements of NewSortRefs except for the i'th
      for (size_t j = 0; j < n; j++)
      {
        if (i != j)
        {
          aterm_appl OldSortRef = ATAelementAt(NewSortRefs, j);
          aterm_appl NewSortRef = aterm_cast<aterm_appl>(Subst(OldSortRef));
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
  aterm_list l;
  while (!SortRefs.empty())
  {
    aterm_appl SortRef = ATAgetFirst(SortRefs);
    if (gsMakeSortId(aterm_cast<aterm_appl>(SortRef[0]))!=aterm_cast<aterm_appl>(SortRef[1]))
    {
      l.push_front(SortRef);
    }
    SortRefs = SortRefs.tail();
  }
  SortRefs = reverse(l);
  mCRL2log(debug) << "SortRefs, after removing self references:\n" << pp(gsMakeSortSpec(SortRefs)) << "" << std::endl;
  return SortRefs;
}

// ========= main processing functions
void gstcDataInit(void)
{
  gssystem.constants=table(63,50);
  gssystem.functions=table(63,50);
  context.basic_sorts=indexed_set(63,50);
  context.defined_sorts=table(63,50);
  context.constants=table(63,50);
  context.functions=table(63,50);
  context.actions=table(63,50);
  context.processes=table(63,50);
  context.PBs=table(63,50);
  context.glob_vars=table(63,50);
  body.proc_pars=table(63,50);
  body.proc_bodies=table(63,50);
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
  gstcAddSystemConstant(sort_list::empty(data::unknown_sort()));
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
  aterm_appl aterm_reference=(aterm_appl)context.defined_sorts.get(static_cast<aterm_appl>(start_search.name()));

  if (aterm_reference==aterm_appl())
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

static void gstcReadInSorts(aterm_list Sorts)
{
  bool nnew;
  for (; !Sorts.empty(); Sorts=Sorts.tail())
  {
    aterm_appl Sort=ATAgetFirst(Sorts);
    aterm_appl SortName=aterm_cast<aterm_appl>(Sort[0]);
    if (sort_bool::is_bool(basic_sort(core::identifier_string(SortName))))
    {
      throw mcrl2::runtime_error("attempt to redeclare sort Bool");
    }
    if (sort_pos::is_pos(basic_sort(core::identifier_string(SortName))))
    {
      throw mcrl2::runtime_error("attempt to redeclare sort Pos");
    }
    if (sort_nat::is_nat(basic_sort(core::identifier_string(SortName))))
    {
      throw mcrl2::runtime_error("attempt to redeclare sort Nat");
    }
    if (sort_int::is_int(basic_sort(core::identifier_string(SortName))))
    {
      throw mcrl2::runtime_error("attempt to redeclare sort Int");
    }
    if (sort_real::is_real(basic_sort(core::identifier_string(SortName))))
    {
      throw mcrl2::runtime_error("attempt to redeclare sort Real");
    }
    if (context.basic_sorts.index(SortName)>=0 || context.defined_sorts.get(SortName).defined())
    {
      throw mcrl2::runtime_error("double declaration of sort " + pp(SortName));
    }
    if (gsIsSortId(Sort))
    {
      ATindexedSetPut(context.basic_sorts, SortName, &nnew);
    }
    else if (gsIsSortRef(Sort))
    {
      context.defined_sorts.put(SortName, aterm_cast<aterm_appl>(Sort[1]));
      mCRL2log(debug) << "Add sort alias " << pp(SortName) << "  " << pp(aterm_cast<aterm_appl>(Sort[1])) << "" << std::endl;
    }
    else
    {
      assert(0);
    }
  }

  // Check for sorts that are recursive through container sorts.
  // E.g. sort L=List(L);
  // This is forbidden.

  aterm_list sort_aliases=context.defined_sorts.keys();
  for (; sort_aliases!=aterm_list() ; sort_aliases=sort_aliases.tail())
  {
    std::set < basic_sort > visited;
    const basic_sort s(core::identifier_string((aterm_appl)(sort_aliases.front())));
    aterm_appl aterm_reference=(aterm_appl)context.defined_sorts.get(static_cast<aterm_appl>(s.name()));
    assert(aterm_reference!=aterm_appl());
    const sort_expression ar(aterm_reference);
    if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(ar,s,visited,false))
    {
      throw mcrl2::runtime_error("sort " + pp(sort_aliases.front()) + " is recursively defined via a function sort, or a set or a bag type container");
    }
  }
}

static void gstcReadInConstructors(aterm_list NewSorts)
{
  aterm_list Sorts=NewSorts;
  if (!Sorts.defined())
  {
    Sorts=context.defined_sorts.keys();
  }
  for (; !Sorts.empty(); Sorts=Sorts.tail())
  {
    aterm_appl SortExpr=aterm_cast<aterm_appl>(context.defined_sorts.get(Sorts.front()));
    gstcIsSortExprDeclared(SortExpr);

    gstcReadInSortStruct(SortExpr);
  }
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
  for (aterm_list sort_walker=context.defined_sorts.keys();  sort_walker!=aterm_list(); sort_walker=sort_walker.tail())
  {
    const core::identifier_string sort_name(ATAgetFirst(sort_walker));
    const data::basic_sort first(sort_name);
    const data::sort_expression second(atermpp::aterm_appl((aterm_appl)context.defined_sorts.get(static_cast<aterm_appl>(sort_name))));
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
        throw mcrl2::runtime_error("Sort alias " + pp(result_sort) + " is defined in terms of itself.");
      }

      for (std::set< sort_expression >::const_iterator j = all_sorts.begin(); j != all_sorts.end(); ++j)
      {
        if (*j==result_sort)
        {
          throw mcrl2::runtime_error("Sort alias " + pp(i->first) + " depends on sort" +
                                     pp(result_sort) + ", which is circularly defined.\n");
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

static void gstc_check_for_empty_constructor_domains(aterm_list constructor_list)
{
  // First add the constructors for structured sorts to the constructor list;
  try
  {
    aterm_list defined_sorts=context.defined_sorts.keys();
    std::map < sort_expression, basic_sort > normalised_aliases=construct_normalised_aliases();
    std::set< sort_expression > all_sorts;
    for (; defined_sorts!=aterm_list(); defined_sorts=defined_sorts.tail())
    {
      const basic_sort s(core::identifier_string(gstcUnwindType(ATAgetFirst(defined_sorts))));
      aterm_appl reference=aterm_cast<aterm_appl>(context.defined_sorts.get(static_cast<aterm_appl>(s.name())));
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
          constructor_list.push_front(aterm_cast<aterm_appl>(*j));
        }
      }

      if (is_structured_sort(*i))
      {
        const function_symbol_vector r=structured_sort(*i).constructor_functions();
        for (function_symbol_vector::const_iterator i=r.begin();
             i!=r.end(); ++i)
        {
          constructor_list.push_front(aterm_cast<aterm_appl>(*i));
        }
      }

    }

    std::set < sort_expression > possibly_empty_constructor_sorts;
    for (aterm_list constructor_list_walker=constructor_list;
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
      for (aterm_list constructor_list_walker=constructor_list;
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
      return; // There are no empty sorts
    }
    else
    {
      std::string reason="the following domains are empty due to recursive constructors:";
      for (std::set < sort_expression >:: const_iterator i=possibly_empty_constructor_sorts.begin();
           i!=possibly_empty_constructor_sorts.end(); ++i)
      {
        reason = reason + "\n" + pp(*i);
      }
      throw mcrl2::runtime_error(reason);
    }
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(e.what());
  }
}

static void gstcReadInFuncs(aterm_list Cons, aterm_list Maps)
{
  mCRL2log(debug) << "Start Read-in Func" << std::endl;

  size_t constr_number=Cons.size();
  for (aterm_list Funcs=Cons+Maps; !Funcs.empty(); Funcs=Funcs.tail())
  {
    aterm_appl Func=ATAgetFirst(Funcs);
    aterm_appl FuncName=aterm_cast<aterm_appl>(Func[0]);
    aterm_appl FuncType=aterm_cast<aterm_appl>(Func[1]);

    gstcIsSortExprDeclared(FuncType);

    //if FuncType is a defined function sort, unwind it
    if (gsIsSortId(FuncType))
    {
      aterm_appl NewFuncType=gstcUnwindType(FuncType);
      if (gsIsSortArrow(NewFuncType))
      {
        FuncType=NewFuncType;
      }
    }

    if ((gsIsSortArrow(FuncType)))
    {
      gstcAddFunction(gsMakeOpId(FuncName,FuncType),"function");
    }
    else
    {
      try 
      {
        gstcAddConstant(gsMakeOpId(FuncName,FuncType),"constant");
      }
      catch (mcrl2::runtime_error &e)  
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not add constant");
      // mCRL2log(error) << "could not add constant" << std::endl;
      // return false;
      }
    }

    if (constr_number)
    {
      constr_number--;

      //Here checks for the constructors
      aterm_appl ConstructorType=FuncType;
      if (gsIsSortArrow(ConstructorType))
      {
        ConstructorType=aterm_cast<aterm_appl>(ConstructorType[1]);
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
        throw mcrl2::runtime_error("Could not add constructor " + pp(FuncName) + " of sort " + pp(FuncType) + ". Constructors of built-in sorts are not allowed.");
        // mCRL2log(error) << "Could not add constructor " << pp(FuncName) << " of sort " << pp(FuncType) << ". Constructors of built-in sorts are not allowed." << std::endl;
        // return false;
      }
    }

    mCRL2log(debug) << "Read-in Func " << pp(FuncName) << ", Types " << pp(FuncType) << "" << std::endl;
  }

  // Check that the constructors are defined such that they cannot generate an empty sort.
  // E.g. in the specification sort D; cons f:D->D; the sort D must be necessarily empty, which is
  // forbidden. The function below checks whether such malicious specifications occur.

  gstc_check_for_empty_constructor_domains(Cons); // throws exception if not ok.
}

static void gstcReadInActs(aterm_list Acts)
{
  for (; !Acts.empty(); Acts=Acts.tail())
  {
    aterm_appl Act=ATAgetFirst(Acts);
    aterm_appl ActName=aterm_cast<aterm_appl>(Act[0]);
    aterm_list ActType=aterm_cast<aterm_list>(Act[1]);

    gstcIsSortExprListDeclared(ActType);

    aterm_list Types=aterm_cast<aterm_list>(context.actions.get(ActName));
    if (!Types.defined())
    {
      Types=make_list<aterm>(ActType);
    }
    else
    {
      // the table context.actions contains a list of types for each
      // action name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ActType, Types))
      {
        throw mcrl2::runtime_error("double declaration of action " + pp(ActName));
      }
      else
      {
        Types=Types+make_list<aterm>(ActType);
      }
    }
    context.actions.put(ActName,Types);
  }
}

static void gstcReadInProcsAndInit(aterm_list Procs, aterm_appl Init)
{
  for (; !Procs.empty(); Procs=Procs.tail())
  {
    aterm_appl Proc=ATAgetFirst(Procs);
    aterm_appl ProcName=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(Proc[0])[0]);

    if (aterm_cast<aterm_list>(context.actions.get(ProcName)).defined())
    {
      throw mcrl2::runtime_error("declaration of both process and action " + pp(ProcName));
    }

    aterm_list ProcType=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Proc[0])[1]);

    gstcIsSortExprListDeclared(ProcType);

    aterm_list Types=aterm_cast<aterm_list>(context.processes.get(ProcName));
    if (!Types.defined())
    {
      Types=make_list<aterm>(ProcType);
    }
    else
    {
      // the table context.processes contains a list of types for each
      // process name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ProcType, Types))
      {
        throw mcrl2::runtime_error("double declaration of process " + pp(ProcName));
      }
      else
      {
        Types=Types+make_list<aterm>(ProcType);
      }
    }
    context.processes.put(ProcName,Types);

    //check that all formal parameters of the process are unique.
    aterm_list ProcVars=aterm_cast<aterm_list>(Proc[1]);
    if (!gstcVarsUnique(ProcVars))
    {
      throw mcrl2::runtime_error("the formal variables in process " + pp(Proc) + " are not unique");
    }

    body.proc_pars.put(aterm_cast<aterm_appl>(Proc[0]),aterm_cast<aterm_list>(Proc[1]));
    body.proc_bodies.put(aterm_cast<aterm_appl>(Proc[0]),aterm_cast<aterm_appl>(Proc[2]));
  }
  body.proc_pars.put(INIT_KEY(),aterm_list());
  body.proc_bodies.put(INIT_KEY(),Init);

}

static void gstcReadInPBESAndInit(aterm_appl PBEqnSpec, aterm_appl PBInit)
{
  aterm_list PBEqns=aterm_cast<aterm_list>(PBEqnSpec[0]);

  for (; !PBEqns.empty(); PBEqns=PBEqns.tail())
  {
    aterm_appl PBEqn=ATAgetFirst(PBEqns);
    aterm_appl PBName=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(PBEqn[1])[0]);

    aterm_list PBVars=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(PBEqn[1])[1]);

    aterm_list PBType;
    for (aterm_list l=PBVars; !l.empty(); l=l.tail())
    {
      PBType.push_front(aterm_cast<aterm_appl>(ATAgetFirst(l)[1]));
    }
    PBType=reverse(PBType);

    gstcIsSortExprListDeclared(PBType);

    aterm_list Types=aterm_cast<aterm_list>(context.PBs.get(PBName));
    if (!Types.defined())
    {
      Types=make_list<aterm>(PBType);
    }
    else
    {
      // temporarily prohibit overloading here
      throw mcrl2::runtime_error("attempt to overload propositional variable " + pp(PBName));
      
      // the table context.PBs contains a list of types for each
      // PBES name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(PBType, Types))
      {
        throw mcrl2::runtime_error("double declaration of propositional variable " + pp(PBName));
      }
      else
      {
        Types=Types+make_list<aterm>(PBType);
      }
    }
    context.PBs.put(PBName,Types);

    //This is a fake ProcVarId (There is no PBVarId)
    aterm_appl Index=gsMakeProcVarId(PBName,PBType);
    body.proc_pars.put(Index,PBVars);
    body.proc_bodies.put(Index,aterm_cast<aterm_appl>(PBEqn[2]));
  }
  body.proc_pars.put(INIT_KEY(),aterm_list());
  body.proc_bodies.put(INIT_KEY(),aterm_cast<aterm_appl>(PBInit[0]));
}

static aterm_list gstcWriteProcs(aterm_list oldprocs)
{
  aterm_list Result;
  for (aterm_list l=oldprocs; !l.empty(); l=l.tail())
  {
    aterm_appl ProcVar=aterm_cast<aterm_appl>(ATAgetFirst(l)[0]);
    if (ProcVar==INIT_KEY())
    {
      continue;
    }
    Result.push_front(gsMakeProcEqn(ProcVar,
                    aterm_cast<aterm_list>(body.proc_pars.get(ProcVar)),
                    aterm_cast<aterm_appl>(body.proc_bodies.get(ProcVar))
                                               )
                   );
  }
  Result=reverse(Result);
  return Result;
}

static aterm_list gstcWritePBES(aterm_list oldPBES)
{
  aterm_list Result;
  for (aterm_list PBEqns=oldPBES; !PBEqns.empty(); PBEqns=PBEqns.tail())
  {
    aterm_appl PBEqn=ATAgetFirst(PBEqns);
    aterm_appl PBESVar=aterm_cast<aterm_appl>(PBEqn[1]);

    aterm_list PBType;
    for (aterm_list l=aterm_cast<aterm_list>(PBESVar[1]); !l.empty(); l=l.tail())
    {
      PBType.push_front(aterm_cast<aterm_appl>(ATAgetFirst(l)[1]));
    }
    PBType=reverse(PBType);

    aterm_appl Index=gsMakeProcVarId(aterm_cast<aterm_appl>(PBESVar[0]),PBType);

    if (Index==INIT_KEY())
    {
      continue;
    }
    Result.push_front(PBEqn.set_argument(aterm_cast<aterm_appl>(body.proc_bodies.get(Index)),2));
  }
  return reverse(Result);
}


static bool gstcTransformVarConsTypeData(void)
{
  bool Result=true;
  table DeclaredVars=table(63,50);
  table FreeVars=table(63,50);

  //data terms in equations
  aterm_list NewEqns;
  bool b = true;
  for (aterm_list Eqns=body.equations; !Eqns.empty(); Eqns=Eqns.tail())
  {
    aterm_appl Eqn=ATAgetFirst(Eqns);
    aterm_list VarList=aterm_cast<aterm_list>(Eqn[0]);

    if (!gstcVarsUnique(VarList))
    {
      b = false;
      throw mcrl2::runtime_error("the variables " + pp(VarList) + " in equation declaration " + pp(Eqn) + " are not unique");
      // mCRL2log(error) << "the variables " << pp(VarList) << " in equation declaration " << pp(Eqn) << " are not unique" << std::endl;
      break;
    }

    table NewDeclaredVars;
    gstcAddVars2Table(DeclaredVars,VarList,NewDeclaredVars);
    DeclaredVars=NewDeclaredVars;

    data_expression Left=aterm_cast<aterm_appl>(Eqn[2]);
    aterm_appl LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,data::unknown_sort(),FreeVars,false,true);
    if (!LeftType.defined())
    {
      b = false;
      throw mcrl2::runtime_error("error occurred while typechecking " + pp(Left) + " as left hand side of equation " + pp(Eqn));
      // mCRL2log(error) << "error occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
      break;
    }
    if (was_warning_upcasting)
    {
      was_warning_upcasting=false;
      mCRL2log(warning) << "warning occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
    }

    data_expression Cond=aterm_cast<aterm_appl>(Eqn[1]);
    if (!gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,Cond,sort_bool::bool_()).defined())
    {
      b = false;
      break;
    }
    data_expression Right=aterm_cast<aterm_appl>(Eqn[3]);
    table dummy_empty_table;
    aterm_appl RightType=gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,Right,LeftType,dummy_empty_table,false);
    if (!RightType.defined())
    {
      b = false;
      throw mcrl2::runtime_error("error occurred while typechecking " + pp(Right) + " as right hand side of equation " + pp(Eqn));
      // mCRL2log(error) << "error occurred while typechecking " << pp(Right) << " as right hand side of equation " << pp(Eqn) << std::endl;
      break;
    }

    //If the types are not uniquely the same now: do once more:
    if (!gstcEqTypesA(LeftType,RightType))
    {
      aterm_appl Type=gstcTypeMatchA(LeftType,RightType);
      if (!Type.defined())
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " do not match");
        // mCRL2log(error) << "types of the left- (" << pp(LeftType) << ") and right- (" << pp(RightType) << ") hand-sides of the equation " << pp(Eqn) << " do not match" << std::endl;
        b = false;
        break;
      }
      Left=aterm_cast<aterm_appl>(Eqn[2]);
      FreeVars.clear();
      LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,Type,FreeVars,true);
      if (!LeftType.defined())
      {
        b = false;
        throw mcrl2::runtime_error("types of the left- and right-hand-sides of the equation " + pp(Eqn) + " do not match");
        // mCRL2log(error) << "types of the left- and right-hand-sides of the equation " << pp(Eqn) << " do not match" << std::endl;
        break;
      }
      if (was_warning_upcasting)
      {
        was_warning_upcasting=false;
        mCRL2log(warning) << "warning occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
      }
      Right=aterm_cast<aterm_appl>(Eqn[3]);
      RightType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Right,LeftType,FreeVars);
      if (!RightType.defined())
      {
        b = false;
        throw mcrl2::runtime_error("types of the left- and right-hand-sides of the equation " + pp(Eqn) + " do not match");
        // mCRL2log(error) << "types of the left- and right-hand-sides of the equation " << pp(Eqn) << " do not match" << std::endl;
        break;
      }
      Type=gstcTypeMatchA(LeftType,RightType);
      if (!Type.defined())
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " do not match");
        // mCRL2log(error) << "types of the left- (" << pp(LeftType) << ") and right- (" << pp(RightType) << ") hand-sides of the equation " << pp(Eqn) << " do not match" << std::endl;
        b = false;
        break;
      }
      if (gstcHasUnknown(Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " cannot be uniquely determined");
        // mCRL2log(error) << "types of the left- (" << pp(LeftType) << ") and right- (" << pp(RightType) << ") hand-sides of the equation " << pp(Eqn) << " cannot be uniquely determined" << std::endl;
        b = false;
        break;
      }
    }
    DeclaredVars.clear();
    NewEqns.push_front(gsMakeDataEqn(VarList,Cond,Left,Right));
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
  table Vars=table(63,50);

  //process and data terms in processes and init
  for (aterm_list ProcVars=body.proc_pars.keys(); !ProcVars.empty(); ProcVars=ProcVars.tail())
  {
    aterm_appl ProcVar=ATAgetFirst(ProcVars);
    Vars.clear();
    gstcATermTableCopy(context.glob_vars,Vars);

    table NewVars;
    gstcAddVars2Table(Vars,aterm_cast<aterm_list>(body.proc_pars.get(ProcVar)),NewVars);
    Vars=NewVars;

    aterm_appl NewProcTerm=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(body.proc_bodies.get(ProcVar)));
    if (!NewProcTerm.defined())
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
  table Vars=table(63,50);

  //PBEs and data terms in PBEqns and init
  for (aterm_list PBVars=body.proc_pars.keys(); !PBVars.empty(); PBVars=PBVars.tail())
  {
    aterm_appl PBVar=ATAgetFirst(PBVars);
    Vars.clear();
    gstcATermTableCopy(context.glob_vars,Vars);

    table NewVars;
    gstcAddVars2Table(Vars,aterm_cast<aterm_list>(body.proc_pars.get(PBVar)),NewVars);
    Vars=NewVars;

    aterm_appl NewPBTerm=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(body.proc_bodies.get(PBVar)));
    if (!NewPBTerm.defined())
    {
      Result = false;
      break;
    }
    body.proc_bodies.put(PBVar,NewPBTerm);
  }

  return Result;
}


// ======== Auxiliary functions
static bool gstcInTypesA(aterm_appl Type, aterm_list Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (gstcEqTypesA(Type,ATAgetFirst(Types)))
    {
      return true;
    }
  return false;
}

static bool gstcEqTypesA(aterm_appl Type1, aterm_appl Type2)
{
  if (Type1==Type2)
  {
    return true;
  }

  if (!Type1.defined() || !Type2.defined())
  {
    return false;
  }

  return gstcUnwindType(Type1)==gstcUnwindType(Type2);
}

static bool gstcInTypesL(aterm_list Type, aterm_list Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (gstcEqTypesL(Type,ATLgetFirst(Types)))
    {
      return true;
    }
  return false;
}

static bool gstcEqTypesL(aterm_list Type1, aterm_list Type2)
{
  if (Type1==Type2)
  {
    return true;
  }
  if (!Type1.defined() || !Type2.defined())
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

static void gstcIsSortDeclared(aterm_appl SortName)
{

  if (sort_bool::is_bool(basic_sort(core::identifier_string(SortName))) ||
      sort_pos::is_pos(basic_sort(core::identifier_string(SortName))) ||
      sort_nat::is_nat(basic_sort(core::identifier_string(SortName))) ||
      sort_int::is_int(basic_sort(core::identifier_string(SortName))) ||
      sort_real::is_real(basic_sort(core::identifier_string(SortName))))
  {
    return;
  }
  if (context.basic_sorts.index(SortName)>=0)
  {
    return;
  }
  if (context.defined_sorts.get(SortName).defined())
  {
    return;
  }
  throw mcrl2::runtime_error("basic or defined sort " + pp(SortName) + " is not declared");
}

static void gstcIsSortExprDeclared(const aterm_appl &SortExpr)
{
  if (gsIsSortId(SortExpr))
  {
    aterm_appl SortName=aterm_cast<aterm_appl>(SortExpr[0]);
    gstcIsSortDeclared(SortName);
    return;
  }

  if (gsIsSortCons(SortExpr))
  {
    gstcIsSortExprDeclared(aterm_cast<aterm_appl>(SortExpr[1]));
    return;
  }

  if (gsIsSortArrow(SortExpr))
  {
    gstcIsSortExprDeclared(aterm_cast<aterm_appl>(SortExpr[1]));

    gstcIsSortExprListDeclared(aterm_cast<aterm_list>(SortExpr[0]));
    return;
  }

  if (gsIsSortStruct(SortExpr))
  {
    for (aterm_list Constrs=aterm_cast<aterm_list>(SortExpr[0]); !Constrs.empty(); Constrs=Constrs.tail())
    {
      aterm_appl Constr=ATAgetFirst(Constrs);

      aterm_list Projs=aterm_cast<aterm_list>(Constr[1]);
      for (; !Projs.empty(); Projs=Projs.tail())
      {
        aterm_appl Proj=ATAgetFirst(Projs);
        aterm_appl ProjSort=aterm_cast<aterm_appl>(Proj[1]);

        // not to forget, recursive call for ProjSort ;-)
        gstcIsSortExprDeclared(ProjSort);
      }
    }
    return;
  }

  throw mcrl2::runtime_error("this is not a sort expression " + pp(SortExpr));
}

static void gstcIsSortExprListDeclared(aterm_list SortExprList)
{
  for (; !SortExprList.empty(); SortExprList=SortExprList.tail())
  {
    gstcIsSortExprDeclared(ATAgetFirst(SortExprList));
  }
}


static void gstcReadInSortStruct(aterm_appl SortExpr)
{
  if (gsIsSortId(SortExpr))
  {
    aterm_appl SortName=aterm_cast<aterm_appl>(SortExpr[0]);
    gstcIsSortDeclared(SortName);
    return;
  }

  if (gsIsSortCons(SortExpr))
  {
    return gstcReadInSortStruct(aterm_cast<aterm_appl>(SortExpr[1]));
  }

  if (gsIsSortArrow(SortExpr))
  {
    gstcReadInSortStruct(aterm_cast<aterm_appl>(SortExpr[1]));

    for (aterm_list Sorts=aterm_cast<aterm_list>(SortExpr[0]); !Sorts.empty(); Sorts=Sorts.tail())
    {
      gstcReadInSortStruct(ATAgetFirst(Sorts));
    }
    return;
  }

  if (gsIsSortStruct(SortExpr))
  {
    for (aterm_list Constrs=aterm_cast<aterm_list>(SortExpr[0]); !Constrs.empty(); Constrs=Constrs.tail())
    {
      aterm_appl Constr=ATAgetFirst(Constrs);

      // recognizer -- if present -- a function from SortExpr to Bool
      aterm_appl Name=aterm_cast<aterm_appl>(Constr[2]);
      if (!gsIsNil(Name))
      {
        gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(make_list<aterm>(SortExpr),sort_bool::bool_())),"recognizer");
      }

      // constructor type and projections
      aterm_list Projs=aterm_cast<aterm_list>(Constr[1]);
      Name=aterm_cast<aterm_appl>(Constr[0]);
      if (Projs.empty())
      {
        gstcAddConstant(gsMakeOpId(Name,SortExpr),"constructor constant");
        continue;
      }

      aterm_list ConstructorType;
      for (; !Projs.empty(); Projs=Projs.tail())
      {
        aterm_appl Proj=ATAgetFirst(Projs);
        aterm_appl ProjSort=aterm_cast<aterm_appl>(Proj[1]);

        // not to forget, recursive call for ProjSort ;-)
        gstcReadInSortStruct(ProjSort);

        aterm_appl ProjName=aterm_cast<aterm_appl>(Proj[0]);
        if (!gsIsNil(ProjName))
        {
          gstcAddFunction(gsMakeOpId(ProjName,gsMakeSortArrow(make_list<aterm>(SortExpr),ProjSort)),"projection",true);
        }
        ConstructorType.push_front(ProjSort);
      }
      gstcAddFunction(gsMakeOpId(Name,gsMakeSortArrow(reverse(ConstructorType),SortExpr)),"constructor");
    }
    return;
  }
}

static void gstcAddConstant(aterm_appl OpId, const std::string msg)
{
  assert(gsIsOpId(OpId));

  aterm_appl Name = data::function_symbol(OpId).name();
  aterm_appl Sort = data::function_symbol(OpId).sort();

  if (context.constants.get(Name).defined())
  {
    throw mcrl2::runtime_error("double declaration of " + msg + " " + pp(Name));
  }

  if (gssystem.constants.get(Name).defined() || gssystem.functions.get(Name).defined())
  {
    throw mcrl2::runtime_error("attempt to declare a constant with the name that is a built-in identifier (" + pp(Name) + ")");
  }

  context.constants.put( Name, Sort);
}

static void gstcAddFunction(aterm_appl OpId, const std::string msg, bool allow_double_decls)
{
  assert(gsIsOpId(OpId));
  const data::function_symbol f(OpId);
  const sort_expression_list domain=function_sort(f.sort()).domain();
  aterm_appl Name = f.name();
  aterm_appl Sort = f.sort();

  if (domain.size()==0)
  {
    if (gssystem.constants.get(Name).defined())
    {
      throw mcrl2::runtime_error("attempt to redeclare the system constant with " + msg + " " + pp(OpId));
    }
  }
  else // domain.size()>0
  {
    aterm_list L=aterm_cast<aterm_list>(gssystem.functions.get(Name));
    for (; L.defined() && L!=aterm_list() ; L=L.tail())
    {
      if (gstcTypeMatchA(Sort,(aterm_appl)L.front()).defined())
      {
        // f matches a predefined function
        throw mcrl2::runtime_error("attempt to redeclare a system function with " + msg + " " + pp(OpId) + ":" + pp(Sort));
      }
    }
  }

  aterm_list Types=aterm_cast<aterm_list>(context.functions.get(Name));
  // the table context.functions contains a list of types for each
  // function name. We need to check if there is already such a type
  // in the list. If so -- error, otherwise -- add
  if (Types.defined() && gstcInTypesA(Sort, Types))
  {
    if (!allow_double_decls)
    {
      throw mcrl2::runtime_error("double declaration of " + msg + " " + pp(Name));
    }
  }
  else
  {
    if (!Types.defined())
    {
      Types=aterm_list();
    }
    Types=Types+make_list<aterm>(Sort);
    context.functions.put(Name,Types);
  }
  mCRL2log(debug) << "Read-in " << msg << " " << pp(Name) << ". Type " << pp(Types) << "" << std::endl;
}

static void gstcAddSystemConstant(aterm_appl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.constants table
  assert(gsIsOpId(OpId));
  aterm_appl OpIdName = data::function_symbol(OpId).name();
  aterm_appl Type = data::function_symbol(OpId).sort();

  aterm_list Types=aterm_cast<aterm_list>(gssystem.constants.get(OpIdName));

  if (!Types.defined())
  {
    Types=aterm_list();
  }
  Types=Types+make_list<aterm>(Type);
  gssystem.constants.put(OpIdName,Types);
}

static void gstcAddSystemFunction(aterm_appl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  assert(gsIsOpId(OpId));
  aterm_appl OpIdName = data::function_symbol(OpId).name();
  aterm_appl Type = data::function_symbol(OpId).sort();
  assert(gsIsSortArrow(Type));

  aterm_list Types=aterm_cast<aterm_list>(gssystem.functions.get(OpIdName));

  if (!Types.defined())
  {
    Types=aterm_list();
  }
  Types=Types+make_list<aterm>(Type);  // TODO: Avoid concatenate but the order is essential.
  gssystem.functions.put(OpIdName,Types);
}

static void gstcATermTableCopy(const table &Orig, table &Copy)
{
  for (aterm_list Keys=Orig.keys(); !Keys.empty(); Keys=Keys.tail())
  {
    aterm Key=Keys.front();
    Copy.put(Key,Orig.get(Key));
  }
}


static bool gstcVarsUnique(aterm_list VarDecls)
{
  bool Result=true;
  indexed_set Temp(63,50);

  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    aterm_appl VarDecl=ATAgetFirst(VarDecls);
    aterm_appl VarName=aterm_cast<aterm_appl>(VarDecl[0]);
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    bool nnew;
    ATindexedSetPut(Temp, VarName, &nnew);
    if (!nnew)
    {
      return false;
    }
  }

  return Result;
}

static void gstcAddVars2Table(table &Vars, aterm_list VarDecls, table &result)
{
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    aterm_appl VarDecl=ATAgetFirst(VarDecls);
    aterm_appl VarName=aterm_cast<aterm_appl>(VarDecl[0]);
    aterm_appl VarType=aterm_cast<aterm_appl>(VarDecl[1]);
    //test the type
    gstcIsSortExprDeclared(VarType);

    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    Vars.put( VarName, VarType);
  }
  result=Vars;
}

static table gstcRemoveVars(table &Vars, aterm_list VarDecls)
{
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    aterm_appl VarDecl=ATAgetFirst(VarDecls);
    aterm_appl VarName=aterm_cast<aterm_appl>(VarDecl[0]);

    Vars.erase(VarName);
  }

  return Vars;
}

static aterm_appl gstcRewrActProc(const table &Vars, aterm_appl ProcTerm, bool is_pbes)
{
  aterm_appl Result;
  aterm_appl Name(ProcTerm[0]);
  aterm_list ParList;

  bool action=false;

  if (!is_pbes)
  {
    if ((ParList=aterm_cast<aterm_list>(context.actions.get(Name))).defined())
    {
      action=true;
    }
    else
    {
      if ((ParList=aterm_cast<aterm_list>(context.processes.get(Name))).defined())
      {
        action=false;
      }
      else
      {
        throw mcrl2::runtime_error("action or process " + pp(Name) + " not declared");
        // mCRL2log(error) << "action or process " << pp(Name) << " not declared" << std::endl;
        return aterm_appl();
      }
    }
  }
  else
  {
    if (!(ParList=aterm_cast<aterm_list>(context.PBs.get(Name))).defined())
    {
      throw mcrl2::runtime_error("propositional variable " + pp(Name) + " not declared");
      // mCRL2log(error) << "propositional variable " << pp(Name) << " not declared" << std::endl;
      return aterm_appl();
    }
  }
  assert(!ParList.empty());

  size_t nFactPars=aterm_cast<aterm_list>(ProcTerm[1]).size();
  const std::string msg=(is_pbes)?"propositional variable":((action)?"action":"process");

  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    aterm_list NewParList;
    for (; !ParList.empty(); ParList=ParList.tail())
    {
      aterm_list Par=ATLgetFirst(ParList);
      if (Par.size()==nFactPars)
      {
        NewParList.push_front(Par);
      }
    }
    ParList=reverse(NewParList);
  }

  if (ParList.empty())
  {
    throw mcrl2::runtime_error("no " + msg + " " + pp(Name)
                    + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                    + " is declared (while typechecking " + pp(ProcTerm) + ")");
    // mCRL2log(error) << "no " << msg << " " << pp(Name)
    //                 << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"")
    //                 << " is declared (while typechecking " << pp(ProcTerm) << ")" << std::endl;
    return aterm_appl();
  }

  if (ParList.size()==1)
  {
    Result=gstcMakeActionOrProc(action,Name,ATLgetFirst(ParList),aterm_cast<aterm_list>(ProcTerm[1]));
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=gstcMakeActionOrProc(action,Name,gstcGetNotInferredList(ParList),aterm_cast<aterm_list>(ProcTerm[1]));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  aterm_list PosTypeList=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Result[0])[1]);

  aterm_list NewPars;
  aterm_list NewPosTypeList;
  for (aterm_list Pars=aterm_cast<aterm_list>(ProcTerm[1]); !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
  {
    data_expression Par=ATAgetFirst(Pars);
    aterm_appl PosType=ATAgetFirst(PosTypeList);

    aterm_appl NewPosType=gstcTraverseVarConsTypeD(Vars,Vars,Par,PosType); //gstcExpandNumTypesDown(PosType));

    if (!NewPosType.defined())
    {
      throw mcrl2::runtime_error("cannot typecheck " + pp(Par) + " as type " + pp(gstcExpandNumTypesDown(PosType)) + " (while typechecking " + pp(ProcTerm) + ")");
      // mCRL2log(error) << "cannot typecheck " << pp(Par) << " as type " << pp(gstcExpandNumTypesDown(PosType)) << " (while typechecking " << pp(ProcTerm) << ")" << std::endl;
      return aterm_appl();
    }
    NewPars.push_front(Par);
    NewPosTypeList.push_front(NewPosType);
  }
  NewPars=reverse(NewPars);
  NewPosTypeList=reverse(NewPosTypeList);

  PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);

  if (!PosTypeList.defined())
  {
    PosTypeList=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Result[0])[1]);
    aterm_list Pars=NewPars;
    NewPars=variable_list();
    aterm_list CastedPosTypeList=aterm_list();
    for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
    {
      data_expression Par=ATAgetFirst(Pars);
      aterm_appl PosType=ATAgetFirst(PosTypeList);
      aterm_appl NewPosType=ATAgetFirst(NewPosTypeList);

      aterm_appl CastedNewPosType=gstcUpCastNumericType(PosType,NewPosType,Par);
      if (!CastedNewPosType.defined())
      {
        throw mcrl2::runtime_error("cannot cast " + pp(NewPosType) + " to " + pp(PosType) + "(while typechecking " + pp(Par) + " in " + pp(ProcTerm));
        // mCRL2log(error) << "cannot cast " << pp(NewPosType) << " to " << pp(PosType) << "(while typechecking " << pp(Par) << " in " << pp(ProcTerm) << std::endl;
        return aterm_appl();
      }

      NewPars.push_front(Par);
      CastedPosTypeList.push_front(CastedNewPosType);
    }
    NewPars=reverse(NewPars);
    NewPosTypeList=reverse(CastedPosTypeList);

    PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);
  }

  if (!PosTypeList.defined())
  {
    throw mcrl2::runtime_error("no " + msg + " " + pp(Name) + "with type " + pp(NewPosTypeList) + " is declared (while typechecking " + pp(ProcTerm) + ")");
    // mCRL2log(error) << "no " << msg << " " << pp(Name) << "with type " << pp(NewPosTypeList) << " is declared (while typechecking " << pp(ProcTerm) << ")" << std::endl;
    return aterm_appl();
  }

  if (gstcIsNotInferredL(PosTypeList))
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + pp(Name));
    // mCRL2log(error) << "ambiguous " << msg << " " << pp(Name) << std::endl;
    return aterm_appl();
  }

  Result=gstcMakeActionOrProc(action,Name,PosTypeList,NewPars);

  if (is_pbes)
  {
    Result=ProcTerm.set_argument(NewPars,1);
  }

  return Result;
}

static inline aterm_appl gstcMakeActionOrProc(bool action, aterm_appl Name,
    aterm_list FormParList, aterm_list FactParList)
{
  return (action)?gsMakeAction(gsMakeActId(Name,FormParList),FactParList)
         :gsMakeProcess(gsMakeProcVarId(Name,FormParList),FactParList);
}

static aterm_appl gstcTraverseActProcVarConstP(const table &Vars, aterm_appl ProcTerm)
{
  aterm_appl Result;
  size_t n = ProcTerm.size();
  if (n==0)
  {
    return ProcTerm;
  }

  //Here the code for short-hand assignments begins.
  if (gsIsIdAssignment(ProcTerm))
  {
    mCRL2log(debug) << "typechecking a process call with short-hand assignments " << ProcTerm << "" << std::endl;
    aterm_appl Name=aterm_cast<aterm_appl>(ProcTerm[0]);
    aterm_list ParList=aterm_cast<aterm_list>(context.processes.get(Name));
    if (!ParList.defined())
    {
      throw mcrl2::runtime_error("process " + pp(Name) + " not declared");
      // mCRL2log(error) << "process " << pp(Name) << " not declared" << std::endl;
      return aterm_appl();
    }

    // Put the assignments into a table
    table As=table(63,50);
    for (aterm_list l=aterm_cast<aterm_list>(ProcTerm[1]); !l.empty(); l=l.tail())
    {
      aterm_appl a=ATAgetFirst(l);
      aterm existing_rhs = As.get(a[0]);
      if (!existing_rhs.defined()) // An assignment of the shape x:=t does not yet exist, this is OK.
      {
        As.put(a[0],a[1]);
        // As.put(aterm_cast<aterm_appl>(a[0]),aterm_cast<aterm_appl>(a[1]));
      }
      else
      {
        throw mcrl2::runtime_error("Double assignment to variable " + pp(aterm_cast<aterm_appl>(a[0])) + " (detected assigned values are " + pp(existing_rhs) + " and " + pp(aterm_cast<aterm_appl>(a[1])) + ")");
        // mCRL2log(error) << "Double assignment to variable " << pp(aterm_cast<aterm_appl>(a[0])) << " (detected assigned values are " << pp(existing_rhs) << " and " << pp(aterm_cast<aterm_appl>(a[1])) << ")" << std::endl;
        return aterm_appl();
      }
    }

    {
      // Now filter the ParList to contain only the processes with parameters in this process call with assignments
      aterm_list NewParList;
      assert(!ParList.empty());
      aterm_appl Culprit; // Variable used for more intelligible error messages.
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        aterm_list Par=ATLgetFirst(ParList);

        // get the formal parameter names
        aterm_list FormalPars=aterm_cast<aterm_list>(body.proc_pars.get(gsMakeProcVarId(Name,Par)));
        // we only need the names of the parameters, not the types
        aterm_list FormalParNames;
        for (; !FormalPars.empty(); FormalPars=FormalPars.tail())
        {
          FormalParNames.push_front(aterm_cast<aterm_appl>(ATAgetFirst(FormalPars)[0]));
        }

        aterm_list l=list_minus(As.keys(),FormalParNames);
        if (l.empty())
        {
          NewParList.push_front(Par);
        }
        else
        {
          Culprit=ATAgetFirst(l);
        }
      }
      ParList=reverse(NewParList);

      if (ParList.empty())
      {
        throw mcrl2::runtime_error("no process " + pp(Name) + " containing all assignments in " + pp(ProcTerm) + ".\n" + "Problematic variable is " + pp(Culprit) + ".");
        // mCRL2log(error) << "no process " << pp(Name) << " containing all assignments in " << pp(ProcTerm) << "." << std::endl << "Problematic variable is " << pp(Culprit) << "." << std::endl;
        return aterm_appl();
      }
      if (!ParList.tail().empty())
      {
        throw mcrl2::runtime_error("ambiguous process " + pp(Name) + " containing all assignments in " + pp(ProcTerm) + ".");
        // mCRL2log(error) << "ambiguous process " << pp(Name) << " containing all assignments in " << pp(ProcTerm) << "." << std::endl;
        return aterm_appl();
      }
    }

    // get the formal parameter names
    aterm_list ActualPars;
    aterm_list FormalPars=aterm_cast<aterm_list>(body.proc_pars.get(gsMakeProcVarId(Name,ATLgetFirst(ParList))));
    {
      // we only need the names of the parameters, not the types
      for (aterm_list l=FormalPars; !l.empty(); l= l.tail())
      {
        aterm_appl FormalParName=aterm_cast<aterm_appl>(ATAgetFirst(l)[0]);
        aterm_appl ActualPar=aterm_cast<aterm_appl>(As.get(FormalParName));
        if (!ActualPar.defined())
        {
          ActualPar=gsMakeId(FormalParName);
        }
        ActualPars.push_front(ActualPar);
      }
      ActualPars=reverse(ActualPars);
    }

    aterm_appl TypeCheckedProcTerm=gstcRewrActProc(Vars, gsMakeParamId(Name,ActualPars));
    if (!TypeCheckedProcTerm.defined())
    {
      throw mcrl2::runtime_error("type error occurred while typechecking the process call with short-hand assignments " + pp(ProcTerm));
      // mCRL2log(error) << "type error occurred while typechecking the process call with short-hand assignments " << pp(ProcTerm) << std::endl;
      return aterm_appl();
    }

    //reverse the assignments
    As.clear();
    for (aterm_list l=aterm_cast<aterm_list>(TypeCheckedProcTerm[1]),m=FormalPars; !l.empty(); l=l.tail(),m=m.tail())
    {
      aterm_appl act_par=ATAgetFirst(l);
      aterm_appl form_par=ATAgetFirst(m);
      /* if (form_par==act_par) This removal is unsound. Consider P(x:D)=sum x:D.a(x).P(x=x); Here the x=x cannot be removed, as the first
                                x refers to the parameter of the process, and the second refers to the bound x in the sum.
      {
        continue;  //parameter does not change 
      } */
      As.put(aterm_cast<aterm_appl>(form_par[0]),gsMakeDataVarIdInit(form_par,act_par));
    }

    aterm_list TypedAssignments;
    for (aterm_list l=aterm_cast<aterm_list>(ProcTerm[1]); !l.empty(); l=l.tail())
    {
      aterm_appl a=ATAgetFirst(l);
      a=aterm_cast<aterm_appl>(As.get(aterm_cast<aterm_appl>(a[0])));
      if (!a.defined())
      {
        continue;
      }
      TypedAssignments.push_front(a);
    }
    TypedAssignments=reverse(TypedAssignments);

    TypeCheckedProcTerm=gsMakeProcessAssignment(aterm_cast<aterm_appl>(TypeCheckedProcTerm[0]),TypedAssignments);

    return TypeCheckedProcTerm;
  }
  //Here the section dealing with assignments ends.

  if (gsIsParamId(ProcTerm))
  {
    aterm_appl a= gstcRewrActProc(Vars,ProcTerm);
    return a;
  }

  if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm) ||
      gsIsRename(ProcTerm) || gsIsComm(ProcTerm) || gsIsAllow(ProcTerm))
  {

    //block & hide
    if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm))
    {
      const std::string msg=gsIsBlock(ProcTerm)?"Blocking":"Hiding";
      aterm_list ActList=aterm_cast<aterm_list>(ProcTerm[0]);
      if (ActList.empty())
      {
        mCRL2log(warning) << msg << " empty set of actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }

      indexed_set Acts(63,50);
      for (; !ActList.empty(); ActList=ActList.tail())
      {
        aterm_appl Act=ATAgetFirst(ActList);

        //Actions must be declared
        if (!context.actions.get(Act).defined())
        {
          throw mcrl2::runtime_error(msg + " an undefined action " + pp(Act) + " (typechecking " + pp(ProcTerm) + ")");
          // mCRL2log(error) << msg << " an undefined action " << pp(Act) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }
        bool nnew;
        ATindexedSetPut(Acts,Act,&nnew);
        if (!nnew)
        {
          mCRL2log(warning) << msg << " action " << pp(Act) << " twice (typechecking " << pp(ProcTerm) << ")" << std::endl;
        }
      }
    }

    //rename
    if (gsIsRename(ProcTerm))
    {
      aterm_list RenList=aterm_cast<aterm_list>(ProcTerm[0]);

      if (RenList.empty())
      {
        mCRL2log(warning) << "renaming empty set of actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }

      indexed_set ActsFrom(63,50);

      for (; !RenList.empty(); RenList=RenList.tail())
      {
        aterm_appl Ren=ATAgetFirst(RenList);
        aterm_appl ActFrom=aterm_cast<aterm_appl>(Ren[0]);
        aterm_appl ActTo=aterm_cast<aterm_appl>(Ren[1]);

        if (ActFrom==ActTo)
        {
          mCRL2log(warning) << "renaming action " << pp(ActFrom) << " into itself (typechecking " << pp(ProcTerm) << ")" << std::endl;
        }

        //Actions must be declared and of the same types
        aterm_list TypesFrom,TypesTo;
        if (!(TypesFrom=aterm_cast<aterm_list>(context.actions.get(ActFrom))).defined())
        {
          throw mcrl2::runtime_error("renaming an undefined action " + pp(ActFrom) + " (typechecking " + pp(ProcTerm) + ")");
          // mCRL2log(error) << "renaming an undefined action " << pp(ActFrom) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }
        if (!(TypesTo=aterm_cast<aterm_list>(context.actions.get(ActTo))).defined())
        {
          throw mcrl2::runtime_error("renaming into an undefined action " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
          // mCRL2log(error) << "renaming into an undefined action " << pp(ActTo) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }

        TypesTo=gstcTypeListsIntersect(TypesFrom,TypesTo);
        if (!TypesTo.defined() || TypesTo.empty())
        {
          throw mcrl2::runtime_error("renaming action " + pp(ActFrom) + " into action " + pp(ActTo) + ": these two have no common type (typechecking " + pp(ProcTerm) + ")");
          // mCRL2log(error) << "renaming action " << pp(ActFrom) << " into action " << pp(ActTo) << ": these two have no common type (typechecking " << pp(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }

        bool nnew;
        ATindexedSetPut(ActsFrom,ActFrom,&nnew);
        if (!nnew)
        {
          throw mcrl2::runtime_error("renaming action " + pp(ActFrom) + " twice (typechecking " + pp(ProcTerm) + ")");
          // mCRL2log(error) << "renaming action " << pp(ActFrom) << " twice (typechecking " << pp(ProcTerm) << ")" << std::endl;
          return aterm_appl();
        }
      }
    }

    //comm: like renaming multiactions (with the same parameters) to action/tau
    if (gsIsComm(ProcTerm))
    {
      aterm_list CommList=aterm_cast<aterm_list>(ProcTerm[0]);

      if (CommList.empty())
      {
        mCRL2log(warning) << "synchronizing empty set of (multi)actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }
      else
      {
        aterm_list ActsFrom;

        for (; !CommList.empty(); CommList=CommList.tail())
        {
          aterm_appl Comm=ATAgetFirst(CommList);
          aterm_list MActFrom=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Comm[0])[0]);
          aterm_list BackupMActFrom=MActFrom;
          assert(!MActFrom.empty());
          aterm_appl ActTo=aterm_cast<aterm_appl>(Comm[1]);

          if (MActFrom.size()==1)
          {
            throw mcrl2::runtime_error("using synchronization as renaming/hiding of action " + pp(MActFrom.front()) + " into " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
            // mCRL2log(error) << "using synchronization as renaming/hiding of action " << pp(MActFrom.front()) << " into " << pp(ActTo) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
            return aterm_appl();
          }

          //Actions must be declared
          aterm_list ResTypes;

          if (!gsIsNil(ActTo))
          {
            ResTypes=aterm_cast<aterm_list>(context.actions.get(ActTo));
            if (!ResTypes.defined())
            {
              throw mcrl2::runtime_error("synchronizing to an undefined action " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
              // mCRL2log(error) << "synchronizing to an undefined action " << pp(ActTo) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
          }

          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            aterm_appl Act=ATAgetFirst(MActFrom);
            aterm_list Types=aterm_cast<aterm_list>(context.actions.get(Act));
            if (!Types.defined())
            {
              throw mcrl2::runtime_error("synchronizing an undefined action " + pp(Act) + " in (multi)action " + pp(MActFrom) + " (typechecking " + pp(ProcTerm) + ")");
              // mCRL2log(error) << "synchronizing an undefined action " << pp(Act) << " in (multi)action " << pp(MActFrom) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
            ResTypes=(ResTypes.defined())?gstcTypeListsIntersect(ResTypes,Types):Types;
            if (!ResTypes.defined() || ResTypes.empty())
            {
              throw mcrl2::runtime_error("synchronizing action " + pp(Act) + " from (multi)action " + pp(BackupMActFrom) + " into action " + pp(ActTo) + ": these have no common type (typechecking " + pp(ProcTerm) + "), ResTypes: " + pp(ResTypes));
              // mCRL2log(error) << "synchronizing action " << pp(Act) << " from (multi)action " << pp(BackupMActFrom) << " into action " << pp(ActTo) << ": these have no common type (typechecking " << pp(ProcTerm) << "), ResTypes: " << atermpp::aterm(ResTypes) << std::endl;
              return aterm_appl();
            }
          }
          MActFrom=BackupMActFrom;

          //the multiactions in the lhss of comm should not intersect.
          //make the list of unique actions
          aterm_list Acts;
          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            aterm_appl Act=ATAgetFirst(MActFrom);
            if (std::find(Acts.begin(),Acts.end(),Act)==Acts.end())
            {
              Acts.push_front(Act);
            }
          }
          for (; !Acts.empty(); Acts=Acts.tail())
          {
            aterm_appl Act=ATAgetFirst(Acts);
            if (std::find(ActsFrom.begin(),ActsFrom.end(),Act)!=ActsFrom.end())
            {
              throw mcrl2::runtime_error("synchronizing action " + pp(Act) + " in different ways (typechecking " + pp(ProcTerm) + ")");
              // mCRL2log(error) << "synchronizing action " << pp(Act) << " in different ways (typechecking " << pp(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
            else
            {
              ActsFrom.push_front(Act);
            }
          }
        }
      }
    }

    //allow
    if (gsIsAllow(ProcTerm))
    {
      aterm_list MActList=aterm_cast<aterm_list>(ProcTerm[0]);

      if (MActList.empty())
      {
        mCRL2log(warning) << "allowing empty set of (multi) actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }
      else
      {
        aterm_list MActs;

        for (; !MActList.empty(); MActList=MActList.tail())
        {
          aterm_list MAct=aterm_cast<aterm_list>(ATAgetFirst(MActList)[0]);

          //Actions must be declared
          for (; !MAct.empty(); MAct=MAct.tail())
          {
            aterm_appl Act=ATAgetFirst(MAct);
            if (!context.actions.get(Act).defined())
            {
              throw mcrl2::runtime_error("allowing an undefined action " + pp(Act) + " in (multi)action " + pp(MAct) + " (typechecking " + pp(ProcTerm) + ")");
              // mCRL2log(error) << "allowing an undefined action " << pp(Act) << " in (multi)action " << pp(MAct) << " (typechecking " << pp(ProcTerm) << ")" << std::endl;
              return aterm_appl();
            }
          }

          MAct=aterm_cast<aterm_list>(ATAgetFirst(MActList)[0]);
          if (gstcMActIn(MAct,MActs))
          {
            mCRL2log(warning) << "allowing (multi)action " << pp(MAct) << " twice (typechecking " << pp(ProcTerm) << ")" << std::endl;
          }
          else
          {
            MActs.push_front(MAct);
          }
        }
      }
    }

    aterm_appl NewProc=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    if (!NewProc.defined())
    {
      return aterm_appl();
    }
    return ProcTerm.set_argument(NewProc,1);
  }

  if (gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
      gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm))
  {
    aterm_appl NewLeft=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));

    if (!NewLeft.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewRight=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    if (!NewRight.defined())
    {
      return aterm_appl();
    }
    aterm_appl a=ProcTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
    return a;
  }

  if (gsIsAtTime(ProcTerm))
  {
    aterm_appl NewProc=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));
    if (!NewProc.defined())
    {
      return aterm_appl();
    }
    data_expression Time=aterm_cast<data_expression>(ProcTerm[1]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (!NewType.defined())
    {
      return aterm_appl();
    }

    if (!gstcTypeMatchA(sort_real::real_(),NewType).defined())
    {
      //upcasting
      aterm_appl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,Time);
      if (!CastedNewType.defined())
      {
        throw mcrl2::runtime_error("cannot (up)cast time value " + pp(Time) + " to type Real");
        // mCRL2log(error) << "cannot (up)cast time value " << pp(Time) << " to type Real" << std::endl;
        return aterm_appl();
      }
    }

    return gsMakeAtTime(NewProc,Time);
  }

  if (gsIsIfThen(ProcTerm))
  {
    data_expression Cond=aterm_cast<aterm_appl>(ProcTerm[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Cond,sort_bool::bool_());
    if (!NewType.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewThen=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    if (!NewThen.defined())
    {
      return aterm_appl();
    }
    return gsMakeIfThen(Cond,NewThen);
  }

  if (gsIsIfThenElse(ProcTerm))
  {
    data_expression Cond=aterm_cast<aterm_appl>(ProcTerm[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Cond,sort_bool::bool_());
    if (!NewType.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewThen=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    if (!NewThen.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewElse=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[2]));
    if (!NewElse.defined())
    {
      return aterm_appl();
    }
    return gsMakeIfThenElse(Cond,NewThen,NewElse);
  }

  if (gsIsSum(ProcTerm))
  {
    table CopyVars=table(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    table NewVars;
    try 
    {
      gstcAddVars2Table(CopyVars,aterm_cast<aterm_list>(ProcTerm[0]),NewVars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pp(ProcTerm));
    }
    aterm_appl NewProc=gstcTraverseActProcVarConstP(NewVars,aterm_cast<aterm_appl>(ProcTerm[1]));
    if (!NewProc.defined())
    {
      throw mcrl2::runtime_error("while typechecking " + pp(ProcTerm));
    }
    return ProcTerm.set_argument(NewProc,1);
  }

  assert(0);
  return Result;
}

static aterm_appl gstcTraversePBESVarConstPB(const table &Vars, aterm_appl PBESTerm)
{
  aterm_appl Result;

  if (gsIsDataExpr(PBESTerm))
  {
    data_expression d(PBESTerm);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,d,sort_bool::bool_());
    if (!NewType.defined())
    {
      return aterm_appl();
    }
    return d;
  }

  if (gsIsPBESTrue(PBESTerm) || gsIsPBESFalse(PBESTerm))
  {
    return PBESTerm;
  }

  if (gsIsPBESNot(PBESTerm))
  {
    aterm_appl NewArg=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm[0]));
    if (!NewArg.defined())
    {
      return aterm_appl();
    }
    return PBESTerm.set_argument(NewArg,0);
  }

  if (gsIsPBESAnd(PBESTerm) || gsIsPBESOr(PBESTerm) || gsIsPBESImp(PBESTerm))
  {
    aterm_appl NewLeft=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm[0]));
    if (!NewLeft.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewRight=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm[1]));
    if (!NewRight.defined())
    {
      return aterm_appl();
    }
    return PBESTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
  }

  if (gsIsPBESForall(PBESTerm)||gsIsPBESExists(PBESTerm))
  {
    table CopyVars=table(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    table NewVars;
    try
    {
      gstcAddVars2Table(CopyVars,aterm_cast<aterm_list>(PBESTerm[0]),NewVars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pp(PBESTerm));
    }
    aterm_appl NewPBES=gstcTraversePBESVarConstPB(NewVars,aterm_cast<aterm_appl>(PBESTerm[1]));
    if (!NewPBES.defined())
    {
      throw mcrl2::runtime_error("while typechecking " + pp(PBESTerm));
      // mCRL2log(error) << "while typechecking " << pp(PBESTerm) << std::endl;
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

static aterm_appl gstcTraverseVarConsTypeD(
  const table &DeclaredVars,
  const table &AllowedVars,
  data_expression &DataTerm,
  aterm_appl PosType,
  table &FreeVars,
  const bool strictly_ambiguous,
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

  aterm_appl Result;

  mCRL2log(debug) << "gstcTraverseVarConsTypeD: DataTerm " << pp(DataTerm) <<
              " with PosType " << pp(PosType) << "" << std::endl;

  if (is_abstraction(DataTerm))
  {
    //The variable declaration of a binder should have at least 1 declaration
    if (!ATAgetFirst(aterm_cast<aterm_list>((DataTerm)[1])).defined())
    {
      throw mcrl2::runtime_error("binder " + pp(DataTerm) + " should have at least one declared variable");
      // mCRL2log(error) << "binder " << pp(DataTerm) << " should have at least one declared variable" << std::endl;
      return aterm_appl();
    }

    aterm_appl BindingOperator = aterm_cast<aterm_appl>((DataTerm)[0]);
    table CopyAllowedVars=table(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    table CopyDeclaredVars=table(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    if (gsIsSetBagComp(BindingOperator) ||
        gsIsSetComp(BindingOperator) ||
        gsIsBagComp(BindingOperator))
    {
      aterm_list VarDecls=aterm_cast<aterm_list>((DataTerm)[1]);
      aterm_appl VarDecl=ATAgetFirst(VarDecls);

      //A Set/bag comprehension should have exactly one variable declared
      VarDecls=VarDecls.tail();
      if (VarDecls != aterm_list())
      {
        throw mcrl2::runtime_error("set/bag comprehension " + pp(DataTerm) + " should have exactly one declared variable");
        // mCRL2log(error) << "set/bag comprehension " << pp(DataTerm) << " should have exactly one declared variable" << std::endl;
        return aterm_appl();
      }

      aterm_appl NewType=aterm_cast<aterm_appl>(VarDecl[1]);
      aterm_list VarList=make_list<aterm>(VarDecl);
      table NewAllowedVars;
      gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      table NewDeclaredVars;
      gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
      
      data_expression Data((DataTerm)[2]);

      aterm_appl ResType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      if (!ResType.defined())
      {
        throw mcrl2::runtime_error("the condition or count of a set/bag comprehension " + pp(DataTerm) + " cannot be determined");
        // mCRL2log(error) << "the condition or count of a set/bag comprehension " << pp(DataTerm) << " cannot be determined\n";
        return aterm_appl();
      }
      if (gstcTypeMatchA(sort_bool::bool_(),ResType).defined())
      {
        NewType=sort_set::set_(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeSetComp(), 0);
      }
      else if (gstcTypeMatchA(sort_nat::nat(),ResType).defined())
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeBagComp(), 0);
      }
      else if (gstcTypeMatchA(sort_pos::pos(),ResType).defined())
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeBagComp(), 0);
        Data=make_application(sort_nat::cnat(),Data);
      }
      else
      {
        throw mcrl2::runtime_error("the condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " + pp(ResType));
        // mCRL2log(error) << "the condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " << pp(ResType) << std::endl;
        return aterm_appl();
      }

      if (!(NewType=gstcTypeMatchA(NewType,PosType)).defined())
      {
        throw mcrl2::runtime_error("a set or bag comprehension of type " + pp(aterm_cast<aterm_appl>(VarDecl[1])) + " does not match possible type " +
                            pp(PosType) + " (while typechecking " + pp(DataTerm) + ")");
        // mCRL2log(error) << "a set or bag comprehension of type " << pp(aterm_cast<aterm_appl>(VarDecl[1])) << " does not match possible type " <<
        //                    pp(PosType) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
        return aterm_appl();
      }

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      DataTerm=DataTerm.set_argument(Data,2);
      return NewType;
    }

    if (gsIsForall(BindingOperator) || gsIsExists(BindingOperator))
    {
      aterm_list VarList=aterm_cast<aterm_list>((DataTerm)[1]);
      table NewAllowedVars;
      gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      table NewDeclaredVars;
      gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);

      data_expression Data=aterm_cast<aterm_appl>((DataTerm)[2]);
      if (!gstcTypeMatchA(sort_bool::bool_(),PosType).defined())
      {
        return aterm_appl();
      }
      aterm_appl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,sort_bool::bool_(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      if (!NewType.defined())
      {
        return aterm_appl();
      }
      if (!gstcTypeMatchA(sort_bool::bool_(),NewType).defined())
      {
        return aterm_appl();
      }

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      DataTerm=DataTerm.set_argument(Data,2);
      return sort_bool::bool_();
    }

    if (gsIsLambda(BindingOperator))
    {
      aterm_list VarList=aterm_cast<aterm_list>((DataTerm)[1]);
      table NewAllowedVars;
      gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      table NewDeclaredVars;
      gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
      
      aterm_list ArgTypes=gstcGetVarTypes(VarList);
      aterm_appl NewType=gstcUnArrowProd(ArgTypes,PosType);
      if (!NewType.defined())
      {
        // mCRL2log(error) << "no functions with arguments " << pp(ArgTypes) << " among " << pp(PosType) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
        throw mcrl2::runtime_error("no functions with arguments " + pp(ArgTypes) + " among " + pp(PosType) + " (while typechecking " + pp(DataTerm) + ")");
        return aterm_appl();
      }
      data_expression Data=aterm_cast<aterm_appl>((DataTerm)[2]);

      NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,NewType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << pp(Data) << "" << std::endl;

      // if (FreeVars)
      {
        gstcRemoveVars(FreeVars,VarList);
      }
      if (!NewType.defined())
      {
        return aterm_appl();
      }
      DataTerm=DataTerm.set_argument(Data,2);
      return gsMakeSortArrow(ArgTypes,NewType);
    }
  }

  if (is_where_clause(DataTerm))
  {
    aterm_list WhereVarList;
    aterm_list NewWhereList;
    for (aterm_list WhereList=aterm_cast<aterm_list>((DataTerm)[1]); !WhereList.empty(); WhereList=WhereList.tail())
    {
      aterm_appl WhereElem=ATAgetFirst(WhereList);
      data_expression WhereTerm=aterm_cast<aterm_appl>(WhereElem[1]);
      aterm_appl WhereType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,WhereTerm,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      if (!WhereType.defined())
      {
        return aterm_appl();
      }

      aterm_appl NewWhereVar;
      if (gsIsDataVarId(aterm_cast<aterm_appl>(WhereElem[0])))
      {
        // The variable in WhereElem is type checked, and a proper variable.
        NewWhereVar=aterm_cast<aterm_appl>(WhereElem[0]);
      }
      else
      {
        // The variable in WhereElem is just a string and needs to be transformed to a DataVarId.
        NewWhereVar=gsMakeDataVarId(aterm_cast<aterm_appl>(WhereElem[0]),WhereType);
      }
      WhereVarList.push_front(NewWhereVar);
      NewWhereList.push_front(gsMakeDataVarIdInit(NewWhereVar,WhereTerm));
    }
    NewWhereList=reverse(NewWhereList);

    table CopyAllowedVars=table(63,50);
    gstcATermTableCopy(AllowedVars,CopyAllowedVars);
    table CopyDeclaredVars=table(63,50);
    //if(AllowedVars!=DeclaredVars)
    gstcATermTableCopy(DeclaredVars,CopyDeclaredVars);

    aterm_list VarList=reverse(WhereVarList);
    table NewAllowedVars;
    gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
    
    table NewDeclaredVars;
    gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
    
    data_expression Data=aterm_cast<aterm_appl>((DataTerm)[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

    if (!NewType.defined())
    {
      return aterm_appl();
    }
    // if (FreeVars)
    {
      gstcRemoveVars(FreeVars,VarList);
    }
    DataTerm=gsMakeWhr(Data,NewWhereList);
    return NewType;
  }

  if (is_application(DataTerm))
  {
    //arguments
    application appl=aterm_cast<application>(DataTerm);
    // aterm_list Arguments=aterm_cast<aterm_list>((DataTerm)[1]);
    size_t nArguments=appl.size();

    //The following is needed to check enumerations
    data_expression Arg0 = appl.head();
    if (gsIsOpId(Arg0) || gsIsId(Arg0))
    {
      aterm_appl Name = aterm_cast<aterm_appl>(Arg0[0]);
      if (Name == sort_list::list_enumeration_name())
      {
        aterm_appl Type=gstcUnList(PosType);
        if (!Type.defined())
        {
          // mCRL2log(error) << "not possible to cast list to " << pp(PosType) << " (while typechecking " << pp(data_expression_list(appl.begin(),appl.end())) << ")" << std::endl;
          throw mcrl2::runtime_error("not possible to cast list to " + pp(PosType) + " (while typechecking " + pp(data_expression_list(appl.begin(),appl.end())) + ")");
          return aterm_appl();
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only!
        data_expression_list NewArguments;
        bool Type_is_stable=true;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i; 
          aterm_appl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,false);
          if (!Type0.defined())
          {
            // Try again, but now without Type as the suggestion.
            // If this does not work, it will be caught in the second pass below.
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
            if (!Type0.defined())
            {
              return aterm_appl();
            }
          }
          NewArguments.push_front(Argument);
          Type_is_stable=Type_is_stable && (Type==Type0);
          Type=Type0;
        }
        // Arguments=OldArguments;

        //Second time to do the real work, but only if the elements in the list have different types.
        if (!Type_is_stable)
        {
          NewArguments=data_expression_list();
          for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i) 
          {
            data_expression Argument= *i;
            aterm_appl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
            if (!Type0.defined())
            {
              return aterm_appl();
            }
            NewArguments.push_front(Argument);
            Type=Type0;
          }
        }

        Type=sort_list::list(sort_expression(Type));
        DataTerm=sort_list::list_enumeration(sort_expression(Type), data_expression_list(reverse(NewArguments)));
        return Type;
      }
      if (Name == sort_set::set_enumeration_name())
      {
        aterm_appl Type=gstcUnSet(PosType);
        if (!Type.defined())
        {
          throw mcrl2::runtime_error("not possible to cast set to " + pp(PosType) + " (while typechecking " + pp(data_expression_list(appl.begin(),appl.end())) + ")");
          // mCRL2log(error) << "not possible to cast set to " << pp(PosType) << " (while typechecking " << pp(data_expression_list(appl.begin(),appl.end())) << ")" << std::endl;
          return aterm_appl();
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only (which will be NewType)!
        aterm_appl NewType;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          aterm_appl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!Type0.defined())
          {
            throw mcrl2::runtime_error("not possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument) + ")");
            // mCRL2log(error) << "not possible to cast element to " << pp(Type) << " (while typechecking " << pp(Argument) << ")" << std::endl;
            return aterm_appl();
          }

          aterm_appl OldNewType=NewType;
          if (NewType==aterm_appl())
          {
            NewType=Type0;
          }
          else
          {
            NewType=gstcMaximumType(NewType,Type0);
          }

          if (NewType==aterm_appl())
          {
            throw mcrl2::runtime_error("Set contains incompatible elements of sorts " + pp(OldNewType) + " and " + pp(Type0) + " (while typechecking " + pp(Argument));
            // mCRL2log(error) << "Set contains incompatible elements of sorts " << pp(OldNewType) << " and " << pp(Type0) << " (while typechecking " << pp(Argument) << std::endl;
            return aterm_appl();
          }
        }

        // Type is now the most generic type to be used.
        assert(Type!=aterm_appl());
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        aterm_list NewArguments;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          aterm_appl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!Type0.defined())
          {
            throw mcrl2::runtime_error("not possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument) + ")");
            // mCRL2log(error) << "not possible to cast element to " << pp(Type) << " (while typechecking " << pp(Argument) << ")" << std::endl;
            return aterm_appl();
          }
          NewArguments.push_front(Argument);
          Type=Type0;
        }
        Type=sort_set::set_(sort_expression(Type));
        DataTerm=sort_set::set_enumeration(sort_expression(Type),data_expression_list(reverse(NewArguments)));
        return Type;
      }
      if (Name == sort_bag::bag_enumeration_name())
      {
        aterm_appl Type=gstcUnBag(PosType);
        if (!Type.defined())
        {
          throw mcrl2::runtime_error("not possible to cast bag to " + pp(PosType) + "(while typechecking " + 
                                      pp(data_expression_list(appl.begin(),appl.end())) + ")");
          // mCRL2log(error) << "not possible to cast bag to " << pp(PosType) << "(while typechecking " << 
          //                             pp(data_expression_list(appl.begin(),appl.end())) << ")" << std::endl;
          return aterm_appl();
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only!
        aterm_appl NewType;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          aterm_appl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!Type0.defined())
          {
            throw mcrl2::runtime_error("not possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument0) + ")");
            // mCRL2log(error) << "not possible to cast element to " << pp(Type) << " (while typechecking " << pp(Argument0) << ")" << std::endl;
            return aterm_appl();
          }
          aterm_appl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!Type1.defined() && print_cast_error)
          {
            throw mcrl2::runtime_error("not possible to cast number to " + pp(sort_nat::nat()) + " (while typechecking " + pp(Argument1) + ")");
            // mCRL2log(error) << "not possible to cast number to " << pp(sort_nat::nat()) << " (while typechecking " << pp(Argument1) << ")" << std::endl;
            return aterm_appl();
          }
          aterm_appl OldNewType=NewType;
          if (NewType==aterm_appl())
          {
            NewType=Type0;
          }
          else
          {
            NewType=gstcMaximumType(NewType,Type0);
          }
          if (NewType==aterm_appl())
          {
            throw mcrl2::runtime_error("Bag contains incompatible elements of sorts " + pp(OldNewType) + " and " + pp(Type0) + " (while typechecking " + pp(Argument0) + ")");
            // mCRL2log(error) << "Bag contains incompatible elements of sorts " << pp(OldNewType) << " and " << pp(Type0) << " (while typechecking " << pp(Argument0) << ")" << std::endl;
            return aterm_appl();
          }
        }
        assert(Type!=aterm_appl());
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        aterm_list NewArguments;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          aterm_appl Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!Type0.defined() && print_cast_error)
          {
            throw mcrl2::runtime_error("not possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument0) + ")");
            // mCRL2log(error) << "not possible to cast element to " << pp(Type) << " (while typechecking " << pp(Argument0) << ")" << std::endl;
            return aterm_appl();
          }
          aterm_appl Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!Type1.defined() && print_cast_error)
          {
            throw mcrl2::runtime_error("not possible to cast number to " + pp(sort_nat::nat()) + " (while typechecking " + pp(Argument1) + ")");
            // mCRL2log(error) << "not possible to cast number to " << pp(sort_nat::nat()) << " (while typechecking " << pp(Argument1) << ")" << std::endl;
            return aterm_appl();
          }
          NewArguments.push_front(Argument0);
          NewArguments.push_front(Argument1);
          Type=Type0;
        }
        Type=sort_bag::bag(sort_expression(Type));
        DataTerm=sort_bag::bag_enumeration(sort_expression(Type), data_expression_list(reverse(NewArguments)));
        return Type;
      }
    }
    aterm_list NewArgumentTypes;
    aterm_list NewArguments;

    for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
    {
      data_expression Arg= *i;
      aterm_appl Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,data::unknown_sort(),FreeVars,false,warn_upcasting,print_cast_error);
      if (!Type.defined())
      {
        return aterm_appl();
      }
      NewArguments.push_front(Arg);
      NewArgumentTypes.push_front(Type);
    }
    aterm_list Arguments=reverse(NewArguments);
    aterm_list ArgumentTypes=reverse(NewArgumentTypes);

    //function
    data_expression Data=appl.head();
    aterm_appl NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                      Data,
                      data::unknown_sort() /* gsMakeSortArrow(ArgumentTypes,PosType) */,
                      FreeVars,false,nArguments,warn_upcasting,print_cast_error);
    mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << pp(Data) << "" << std::endl;

    if (!NewType.defined())
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else if (gsIsOpId(Data)||gsIsDataVarId(Data))
      {
        gstcErrorMsgCannotCast(aterm_cast<aterm_appl>(Data[1]),Arguments,ArgumentTypes);
      }
      throw mcrl2::runtime_error("type error while trying to cast " + pp(application(Data,aterm_cast<data_expression_list>(Arguments))) 
                                + " to type " + pp(PosType));
      // mCRL2log(error) << "type error while trying to cast " << pp(application(Data,aterm_cast<data_expression_list>(Arguments))) 
      //                           << " to type " << pp(PosType) << std::endl;
      return aterm_appl();
    }

    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.


    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      aterm_list NeededArgumentTypes=aterm_cast<aterm_list>(gstcUnwindType(NewType)[0]);

      if (NeededArgumentTypes.size()!=Arguments.size())
      {
         throw mcrl2::runtime_error("need argumens of sorts " + pp(NeededArgumentTypes) +
                         " which does not match the number of provided arguments "
                            + pp(Arguments) + " (while typechecking "
                            + pp(DataTerm) + ")");
         // mCRL2log(error) << "need argumens of sorts " << pp(NeededArgumentTypes) <<
         //                 " which does not match the number of provided arguments "
         //                    << pp(Arguments) << " (while typechecking "
         //                    << pp(DataTerm) << ")" << std::endl;
         return aterm_appl();

      }
      //arguments again
      aterm_list NewArgumentTypes;
      aterm_list NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        assert(!Arguments.empty());
        assert(!NeededArgumentTypes.empty());
        data_expression Arg=ATAgetFirst(Arguments);
        aterm_appl NeededType=ATAgetFirst(NeededArgumentTypes);
        aterm_appl Type=ATAgetFirst(ArgumentTypes);
        if (!gstcEqTypesA(NeededType,Type))
        {
          //upcasting
          aterm_appl CastedNewType=gstcUpCastNumericType(NeededType,Type,Arg,warn_upcasting);
          if (CastedNewType.defined())
          {
            Type=CastedNewType;
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on [1] " << pp(Arg) << ", Type: " << pp(Type) << ", Needed type: " << pp(NeededType) << "" << std::endl;
          aterm_appl NewArgType=gstcTypeMatchA(NeededType,Type);
          if (!NewArgType.defined())
          {
            NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          }
          if (!NewArgType.defined())
          {
            NewArgType=NeededType;
          }
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          mCRL2log(debug) << "Result of Doing again gstcTraverseVarConsTypeD: DataTerm " << pp(Arg) << "" << std::endl;
          if (!NewArgType.defined())
          {
            throw mcrl2::runtime_error("needed type " + pp(NeededType) + " does not match possible type "
                            + pp(Type) + " (while typechecking " + pp(Arg) + " in " + pp(DataTerm) + ")");
            // mCRL2log(error) << "needed type " << pp(NeededType) << " does not match possible type "
            //                 << pp(Type) << " (while typechecking " << pp(Arg) << " in " << pp(DataTerm) << ")" << std::endl;
            return aterm_appl();
          }
          Type=NewArgType;
        }
        NewArguments.push_front(Arg);
        NewArgumentTypes.push_front(Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    //the function again
    NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                      Data,gsMakeSortArrow(ArgumentTypes,PosType),FreeVars,strictly_ambiguous,nArguments,warn_upcasting,print_cast_error);

    mCRL2log(debug) << "Result of gstcTraverseVarConsTypeDN: DataTerm " << pp(Data) << "" << std::endl;

    if (!NewType.defined())
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else
      {
        gstcErrorMsgCannotCast(aterm_cast<aterm_appl>(Data[1]),Arguments,ArgumentTypes);
      }
      throw mcrl2::runtime_error("type error while trying to cast " + pp(application(Data,aterm_cast<data_expression_list>(Arguments))) 
                                  + " to type " + pp(PosType));
      // mCRL2log(error) << "type error while trying to cast " << pp(application(Data,aterm_cast<data_expression_list>(Arguments))) 
      //                             << " to type " << pp(PosType) << std::endl;
      return aterm_appl();
    }

    mCRL2log(debug) << "Arguments once more: Arguments " << pp(Arguments) << ", ArgumentTypes: " <<
                pp(ArgumentTypes) << ", NewType: " << pp(NewType) << "" << std::endl;

    //and the arguments once more
    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      aterm_list NeededArgumentTypes=aterm_cast<aterm_list>(gstcUnwindType(NewType)[0]);
      aterm_list NewArgumentTypes;
      aterm_list NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        data_expression Arg=ATAgetFirst(Arguments);
        aterm_appl NeededType=ATAgetFirst(NeededArgumentTypes);
        aterm_appl Type=ATAgetFirst(ArgumentTypes);

        if (!gstcEqTypesA(NeededType,Type))
        {
          //upcasting
          aterm_appl CastedNewType=gstcUpCastNumericType(NeededType,Type,Arg,warn_upcasting);
          if (CastedNewType.defined())
          {
            Type=CastedNewType;
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on [2] " << pp(Arg) << ", Type: " << pp(Type) << ", Needed type: " << pp(NeededType) << "" << std::endl;
          aterm_appl NewArgType=gstcTypeMatchA(NeededType,Type);
          if (!NewArgType.defined())
          {
            NewArgType=gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type));
          }
          if (!NewArgType.defined())
          {
            NewArgType=NeededType;
          }
          NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          if (!NewArgType.defined())
          {
            throw mcrl2::runtime_error("needed type " + pp(NeededType) + " does not match possible type "
                            + pp(Type) + " (while typechecking " + pp(Arg) + " in " + pp(DataTerm) + ")");
            // mCRL2log(error) << "needed type " << pp(NeededType) << " does not match possible type "
            //                 << pp(Type) << " (while typechecking " << pp(Arg) << " in " << pp(DataTerm) << ")" << std::endl;
            return aterm_appl();
          }
          Type=NewArgType;
        }

        NewArguments.push_front(Arg);
        NewArgumentTypes.push_front(Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    mCRL2log(debug) << "Arguments after once more: Arguments " << pp(Arguments) << ", ArgumentTypes: " << pp(ArgumentTypes) << "" << std::endl;

    DataTerm=application(Data,aterm_cast<data_expression_list>(Arguments));

    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      return aterm_cast<aterm_appl>(gstcUnwindType(NewType)[1]);
    }

    if (gstcHasUnknown(gstcUnArrowProd(ArgumentTypes,NewType)))
    {
      throw mcrl2::runtime_error("Fail to properly type " + pp(DataTerm));
      // mCRL2log(error) << "Fail to properly type " << pp(DataTerm) << std::endl;
      return aterm_appl();
    }
    return gstcUnArrowProd(ArgumentTypes,NewType);
  }

  if (gsIsId(DataTerm)||gsIsOpId(DataTerm)||gsIsDataVarId(DataTerm))
  {
    aterm_appl Name=aterm_cast<aterm_appl>((DataTerm)[0]);
    if (gsIsNumericString(gsATermAppl2String(Name)))
    {
      aterm_appl Sort=sort_int::int_();
      if (gstcIsPos(Name))
      {
        Sort=sort_pos::pos();
      }
      else if (gstcIsNat(Name))
      {
        Sort=sort_nat::nat();
      }
      DataTerm=gsMakeOpId(Name,Sort);

      if (gstcTypeMatchA(Sort,PosType).defined())
      {
        return Sort;
      }

      //upcasting
      aterm_appl CastedNewType=gstcUpCastNumericType(PosType,Sort,DataTerm,warn_upcasting);
      if (!CastedNewType.defined())
      {
        throw mcrl2::runtime_error("cannot (up)cast number " + pp(DataTerm) + " to type " + pp(PosType));
        // mCRL2log(error) << "cannot (up)cast number " << pp(DataTerm) << " to type " << pp(PosType) << std::endl;
        return aterm_appl();
      }
      return CastedNewType;
    }

    aterm_appl Type=aterm_cast<aterm_appl>(DeclaredVars.get(Name));
    if (Type.defined())
    {
      mCRL2log(debug) << "Recognised declared variable " << pp(Name) << ", Type: " << pp(Type) << "" << std::endl;
      DataTerm=gsMakeDataVarId(Name,Type);

      if (!AllowedVars.get(Name).defined())
      {
        throw mcrl2::runtime_error("variable " + pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side");
        // mCRL2log(error) << "variable " << pp(Name) << " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side" << std::endl;
        return aterm_appl();
      }

      aterm_appl NewType=gstcTypeMatchA(Type,PosType);
      if (NewType.defined())
      {
        Type=NewType;
      }
      else
      {
        //upcasting
        aterm_appl CastedNewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        if (!CastedNewType.defined() && print_cast_error)
        {
          throw mcrl2::runtime_error("cannot (up)cast variable " + pp(DataTerm) + " to type " + pp(PosType));
          // mCRL2log(error) << "cannot (up)cast variable " << pp(DataTerm) << " to type " << pp(PosType) << std::endl;
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

    if ((Type=aterm_cast<aterm_appl>(context.constants.get(Name))).defined())
    {
      aterm_appl NewType=gstcTypeMatchA(Type,PosType);
      if (NewType.defined())
      {
        Type=NewType;
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        // The type cannot be unified. Try upcasting the type.
        DataTerm=gsMakeOpId(Name,Type);
        aterm_appl NewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        if (NewType==aterm_appl())
        {
          throw mcrl2::runtime_error("no constant " + pp(DataTerm) + " with type " + pp(PosType));
          // mCRL2log(error) << "no constant " << pp(DataTerm) << " with type " << pp(PosType) << std::endl;
          return aterm_appl();
        }
        else
        {
          return NewType;
        }
      }
    }

    aterm_list ParList=aterm_cast<aterm_list>(gssystem.constants.get(Name));
    if (ParList.defined())
    {
      aterm_list NewParList;
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        aterm_appl Par=ATAgetFirst(ParList);
        if ((Par=gstcTypeMatchA(Par,PosType)).defined())
        {
          NewParList.push_front(Par);
        }
      }
      ParList=reverse(NewParList);
      if (ParList.empty())
      {
        throw mcrl2::runtime_error("no system constant " + pp(DataTerm) + " with type " + pp(PosType));
        // mCRL2log(error) << "no system constant " << pp(DataTerm) << " with type " << pp(PosType) << std::endl;
        return aterm_appl();
      }

      if (ParList.size()==1)
      {
        Type=ATAgetFirst(ParList);
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        DataTerm=gsMakeOpId(Name,data::unknown_sort());
        return data::unknown_sort();
      }
    }

    aterm_list ParListS=aterm_cast<aterm_list>(gssystem.functions.get(Name));
    ParList=aterm_cast<aterm_list>(context.functions.get(Name));
    if (!ParList.defined())
    {
      ParList=ParListS;
    }
    else if (ParListS.defined())
    {
      ParList=ParListS+ParList;
    }

    if (!ParList.defined())
    {
      throw mcrl2::runtime_error("unknown operation " + pp(Name));
      // mCRL2log(error) << "unknown operation " << pp(Name) << std::endl;
      return aterm_appl();
    }

    if (ParList.size()==1)
    {
      aterm_appl Type=ATAgetFirst(ParList);
      DataTerm=gsMakeOpId(Name,Type);
      aterm_appl NewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
      if (NewType==aterm_appl())
      {
        throw mcrl2::runtime_error("no constant " + pp(DataTerm) + " with type " + pp(PosType));
        // mCRL2log(error) << "no constant " << pp(DataTerm) << " with type " << pp(PosType) << std::endl;
        return aterm_appl();
      }
      return NewType;
    }
    else
    {
      return gstcTraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strictly_ambiguous, std::string::npos, warn_upcasting,print_cast_error);
    }
  }

  // if(gsIsDataVarId(DataTerm)){
  //   return aterm_cast<aterm_appl>((DataTerm)[1]);
  // }

  return Result;
}

static aterm_appl gstcTraverseVarConsTypeDN(
  const table &DeclaredVars,
  const table &AllowedVars,
  data_expression &DataTerm,
  aterm_appl PosType,
  table &FreeVars,
  const bool strictly_ambiguous,
  const size_t nFactPars,
  const bool warn_upcasting,
  const bool print_cast_error)
{
  // std::string::npos for nFactPars means the number of arguments is not known.
  mCRL2log(debug) << "gstcTraverseVarConsTypeDN: DataTerm " << pp(DataTerm)
                  << " with PosType " << pp(PosType) << ", nFactPars " << nFactPars << "" << std::endl;
  if (gsIsId(DataTerm)||gsIsOpId(DataTerm))
  {
    aterm_appl Name=aterm_cast<aterm_appl>((DataTerm)[0]);
    bool variable=false;
    aterm_appl Type=aterm_cast<aterm_appl>(DeclaredVars.get(Name));
    if (Type.defined())
    {
      const sort_expression Type1(gstcUnwindType(Type));
      if (is_function_sort(Type1)?(function_sort(Type1).domain().size()==nFactPars):(nFactPars==0))
      {
        variable=true;
        if (!AllowedVars.get(Name).defined())
        {
          throw mcrl2::runtime_error("variable " + pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side");
          // mCRL2log(error) << "variable " << pp(Name) << " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side" << std::endl;
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
    aterm_list ParList;

    if (nFactPars==0)
    {
      if ((Type=aterm_cast<aterm_appl>(DeclaredVars.get(Name))).defined())
      {
        if (!gstcTypeMatchA(Type,PosType).defined())
        {
          throw mcrl2::runtime_error("the type " + pp(Type) + " of variable " + pp(Name)
                          + " is incompatible with " + pp(PosType) + " (typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the type " << pp(Type) << " of variable " << pp(Name)
          //                << " is incompatible with " << pp(PosType) << " (typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        DataTerm=gsMakeDataVarId(Name,Type);
        return Type;
      }
      else if ((Type=aterm_cast<aterm_appl>(context.constants.get(Name))).defined())
      {
        if (!gstcTypeMatchA(Type,PosType).defined())
        {
          throw mcrl2::runtime_error("the type " + pp(Type) + " of constant " + pp(Name)
                          + " is incompatible with " + pp(PosType) + " (typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the type " << pp(Type) << " of constant " << pp(Name)
          //                << " is incompatible with " << pp(PosType) << " (typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        if ((ParList=aterm_cast<aterm_list>(gssystem.constants.get(Name))).defined())
        {
          if (ParList.size()==1)
          {
            aterm_appl Type=ATAgetFirst(ParList);
            DataTerm=gsMakeOpId(Name,Type);
            return Type;
          }
          else
          {
            mCRL2log(warning) << "ambiguous system constant " << pp(Name) << std::endl;
            DataTerm=gsMakeOpId(Name,data::unknown_sort());
            return Type;
          }
        }
        else
        {
          throw mcrl2::runtime_error("unknown constant " + pp(Name));
          // mCRL2log(error) << "unknown constant " << pp(Name) << std::endl;
          return aterm_appl();
        }
      }
    }

    if (Type.defined())
    {
      ParList=make_list<aterm>(gstcUnwindType(Type));
    }
    else
    {
      aterm_list ParListS=aterm_cast<aterm_list>(gssystem.functions.get(Name));
      ParList=aterm_cast<aterm_list>(context.functions.get(Name));
      if (!ParList.defined())
      {
        ParList=ParListS;
      }
      else if (ParListS.defined())
      {
        ParList=ParListS+ParList;
      }
    }

    if (!ParList.defined())
    {
      if (nFactPars!=std::string::npos)
      {
        throw mcrl2::runtime_error("unknown operation " + pp(Name) + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":""));
        // mCRL2log(error) << "unknown operation " << pp(Name) << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"") << std::endl;
      }
      else
      {
        throw mcrl2::runtime_error("unknown operation " + pp(Name));
        // mCRL2log(error) << "unknown operation " << pp(Name) << std::endl;
      }
      return aterm_appl();
    }
    mCRL2log(debug) << "Possible types for Op/Var " << pp(Name) << " with " << nFactPars <<
                " argument are (ParList: " << pp(ParList) << "; PosType: " << pp(PosType) << ")" << std::endl;

    aterm_list CandidateParList=ParList;

    {
      // filter ParList keeping only functions A_0#...#A_nFactPars->A
      aterm_list NewParList;
      if (nFactPars!=std::string::npos)
      {
        NewParList=aterm_list();
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          aterm_appl Par=ATAgetFirst(ParList);
          if (!gsIsSortArrow(Par))
          {
            continue;
          }
          if (aterm_cast<aterm_list>(Par[0]).size()!=nFactPars)
          {
            continue;
          }
          NewParList.push_front(Par);
        }
        ParList=reverse(NewParList);
      }

      if (!ParList.empty())
      {
        CandidateParList=ParList;
      }

      // filter ParList keeping only functions of the right type
      aterm_list BackupParList=ParList;
      NewParList=aterm_list();
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        aterm_appl Par=ATAgetFirst(ParList);
        if ((Par=gstcTypeMatchA(Par,PosType)).defined())
        {
          NewParList=ATinsertUnique(NewParList,Par);
        }
      }
      NewParList=reverse(NewParList);

      mCRL2log(debug) << "Possible matches w/o casting for Op/Var " << pp(Name) << " with "<< nFactPars <<               " argument are (ParList: " << pp(NewParList) << "; PosType: " << pp(PosType) << "" << std::endl;

      if (NewParList.empty())
      {
        //Ok, this looks like a type error. We are not that strict.
        //Pos can be Nat, or even Int...
        //So lets make PosType more liberal
        //We change every Pos to NotInferred(Pos,Nat,Int)...
        //and get the list. Then we take the min of the list.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying casting for Op " << pp(Name) << " with " << nFactPars << " argument (ParList: " <<                             pp(ParList) << "; PosType: " << pp(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesUp(PosType);
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          aterm_appl Par=ATAgetFirst(ParList);
          if ((Par=gstcTypeMatchA(Par,PosType)).defined())
          {
            NewParList=ATinsertUnique(NewParList,Par);
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is [1] " << pp(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=make_list<aterm>(gstcMinType(NewParList));
        }
      }

      if (NewParList.empty())
      {
        //Ok, casting of the arguments did not help.
        //Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying result casting for Op " << pp(Name) << " with " << nFactPars << " argument (ParList: "                       << "; PosType: " << pp(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesDown(gstcExpandNumTypesUp(PosType));
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          aterm_appl Par=ATAgetFirst(ParList);
          if ((Par=gstcTypeMatchA(Par,PosType)).defined())
          {
            NewParList=ATinsertUnique(NewParList,Par);
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is [2]" << pp(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=make_list<aterm>(gstcMinType(NewParList));
        }
      }

      ParList=NewParList;
    }
    if (ParList.empty())
    {
      //provide some information to the upper layer for a better error message
      aterm_appl Sort;
      if (CandidateParList.size()==1)
      {
        Sort=ATAgetFirst(CandidateParList);
      }
      else
      {
        // Sort=multiple_possible_sorts(atermpp::aterm_list(CandidateParList));
        Sort=multiple_possible_sorts(sort_expression_list(CandidateParList));
      }
      DataTerm=gsMakeOpId(Name,Sort);
      if (nFactPars!=std::string::npos)
      {
        throw mcrl2::runtime_error("unknown operation/variable " + pp(Name)
                        + " with " + to_string(nFactPars) + " argument" + ((nFactPars != 1)?"s":"")
                        + " that matches type " + pp(PosType));
        // mCRL2log(error) << "unknown operation/variable " << pp(Name)
        //                 << " with " << nFactPars << " argument" << ((nFactPars != 1)?"s":"")
        //                 << " that matches type " << pp(PosType) << std::endl;
      }
      else
      {
        throw mcrl2::runtime_error("unknown operation/variable " + pp(Name) + " that matches type " + pp(PosType));
        // mCRL2log(error) << "unknown operation/variable " << pp(Name) << " that matches type " << pp(PosType) << std::endl;
      }
      return aterm_appl();
    }

    if (ParList.size()==1)
    {
      // replace PossibleSorts by a single possibility.
      aterm_appl Type=ATAgetFirst(ParList);

      aterm_appl OldType=Type;
      if (gstcHasUnknown(Type))
      {
        Type=gstcTypeMatchA(Type,PosType);
      }

      if (gstcHasUnknown(Type) && gsIsOpId(DataTerm))
      {
        Type=gstcTypeMatchA(Type,aterm_cast<aterm_appl>((DataTerm)[1]));
      }
      if (Type==aterm_appl())
      {
        throw mcrl2::runtime_error("fail to match sort " + pp(OldType) + " with " + pp(PosType));
        // mCRL2log(error) << "fail to match sort " << pp(OldType) << " with " << pp(PosType) << std::endl;
        return aterm_appl();
      }

      if (static_cast<aterm_appl>(data::detail::if_symbol())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing if matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchIf(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function if has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function if has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(data::detail::equal_symbol())==aterm_cast<aterm_appl>((DataTerm)[0])
          || static_cast<aterm_appl>(data::detail::not_equal_symbol())==aterm_cast<aterm_appl>((DataTerm)[0])
          || static_cast<aterm_appl>(data::detail::less_symbol())==aterm_cast<aterm_appl>((DataTerm)[0])
          || static_cast<aterm_appl>(data::detail::less_equal_symbol())==aterm_cast<aterm_appl>((DataTerm)[0])
          || static_cast<aterm_appl>(data::detail::greater_symbol())==aterm_cast<aterm_appl>((DataTerm)[0])
          || static_cast<aterm_appl>(data::detail::greater_equal_symbol())==aterm_cast<aterm_appl>((DataTerm)[0])
         )
      {
        mCRL2log(debug) << "Doing ==, !=, <, <=, >= or > matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchEqNeqComparison(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function " + pp(aterm_cast<aterm_appl>((DataTerm)[0])) + " has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function " << pp(aterm_cast<aterm_appl>((DataTerm)[0])) << " has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::cons_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing |> matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchListOpCons(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function |> has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function |> has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::snoc_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing <| matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchListOpSnoc(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function <| has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function <| has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::concat_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing ++ matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchListOpConcat(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function ++ has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function ++ has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::element_at_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing @ matching Type " << pp(Type) << ", PosType " << pp(PosType) << ", DataTerm: " << pp(DataTerm) << "" << std::endl;
        aterm_appl NewType=gstcMatchListOpEltAt(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function @ has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function @ has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::head_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_list::rhead_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {R,L}head matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
// Type==NULL
        aterm_appl NewType=gstcMatchListOpHead(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function {R,L}head has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function {R,L}head has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::tail_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_list::rtail_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {R,L}tail matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchListOpTail(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function {R,L}tail has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function {R,L}tail has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::set2bag_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing Set2Bag matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchSetOpSet2Bag(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function Set2Bag has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function Set2Bag has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::in_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {List,Set,Bag} matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchListSetBagOpIn(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function {List,Set,Bag}In has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function {List,Set,Bag}In has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_set::union_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_set::difference_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_set::intersection_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {Set,Bag}{Union,Difference,Intersect} matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchSetBagOpUnionDiffIntersect(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_set::complement_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing SetCompl matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchSetOpSetCompl(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function SetCompl has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function SetCompl has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::bag2set_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing Bag2Set matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchBagOpBag2Set(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function Bag2Set has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function Bag2Set has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::count_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing BagCount matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchBagOpBagCount(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("the function BagCount has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "the function BagCount has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }


      if (static_cast<aterm_appl>(data::function_update_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing FuncUpdate matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType=gstcMatchFuncUpdate(Type);
        if (!NewType.defined())
        {
          throw mcrl2::runtime_error("function update has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
          // mCRL2log(error) << "function update has incompatible argument types " << pp(Type) << " (while typechecking " << pp(DataTerm) << ")" << std::endl;
          return aterm_appl();
        }
        Type=NewType;
      }


      Type=replace_possible_sorts(Type); // Set the type to one option in possible sorts, if there are more options.
      DataTerm=gsMakeOpId(Name,Type);
      if (variable)
      {
        DataTerm=gsMakeDataVarId(Name,Type);
      }

      assert(Type.defined());
      return Type;
    }
    else
    {
      was_ambiguous=true;
      if (strictly_ambiguous)
      {
        mCRL2log(debug) << "ambiguous operation " << pp(Name) << " (ParList " << pp(ParList) << ")" << std::endl;
        if (nFactPars!=std::string::npos)
        {
          throw mcrl2::runtime_error("ambiguous operation " + pp(Name) + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":""));
          // mCRL2log(error) << "ambiguous operation " << pp(Name) << " with " << nFactPars << " parameter" << ((nFactPars != 1)?"s":"") << std::endl;
        }
        else
        {
          throw mcrl2::runtime_error("ambiguous operation " + pp(Name));
          // mCRL2log(error) << "ambiguous operation " << pp(Name) << std::endl;
        }
        return aterm_appl();
      }
      else
      {
        //DataTerm=gsMakeOpId(Name,data::unknown_sort());
        //if(variable) DataTerm=gsMakeDataVarId(Name,data::unknown_sort());
        return data::unknown_sort();
      }
    }
  }
  else
  {
    return gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,DataTerm,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
  }
}

// ================================================================================
// Phase 2 -- type inference
// ================================================================================
static aterm_list gstcGetNotInferredList(aterm_list TypeListList)
{
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1

  aterm_list Result;
  size_t nFormPars=((aterm_list)TypeListList.front()).size();
  std::vector<aterm_list> Pars(nFormPars);
  /* if (nFormPars > 0)
  {
    Pars = (aterm_list*) malloc(nFormPars*sizeof(aterm_list));
  } */
  //DECLA(aterm_list,Pars,nFormPars);
  for (size_t i=0; i<nFormPars; i++)
  {
    Pars[i]=aterm_list();
  }

  for (; !TypeListList.empty(); TypeListList=TypeListList.tail())
  {
    aterm_list TypeList=ATLgetFirst(TypeListList);
    for (size_t i=0; i<nFormPars; TypeList=TypeList.tail(),i++)
    {
      Pars[i]=gstcInsertType(Pars[i],ATAgetFirst(TypeList));
    }
  }

  for (size_t i=nFormPars; i>0; i--)
  {
    aterm_appl Sort;
    if (Pars[i-1].size()==1)
    {
      Sort=ATAgetFirst(Pars[i-1]);
    }
    else
    {
      // Sort=multiple_possible_sorts(atermpp::aterm_list(reverse(Pars[i-1])));
      Sort=multiple_possible_sorts(sort_expression_list(reverse(Pars[i-1])));
    }
    Result.push_front(Sort);
  }
  // free(Pars);
  return Result;
}

static aterm_appl gstcUpCastNumericType(aterm_appl NeededType, aterm_appl Type, data_expression &Par, bool warn_upcasting)
{
  // Makes upcasting from Type to Needed Type for Par. Returns the resulting type.
  // Moreover, *Par is extended with the required type transformations.
  mCRL2log(debug) << "gstcUpCastNumericType " << pp(NeededType) << " -- " << pp(Type) << "" << std::endl;

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
    sort_expression_list l=sort_expression_list(aterm_cast<aterm_list>(NeededType[0]));
    for(sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      aterm_appl r=gstcUpCastNumericType(*i,Type,Par,warn_upcasting);
      if (r!=aterm_appl())
      {
        return r;
      }
    }
    return aterm_appl();
  }

  if (warn_upcasting && gsIsOpId(Par) && gsIsNumericString(gsATermAppl2String(aterm_cast<aterm_appl>((Par)[0]))))
  {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  if (gstcTypeMatchA(NeededType,sort_pos::pos()).defined())
  {
    if (gstcTypeMatchA(Type,sort_pos::pos()).defined())
    {
      return sort_pos::pos();
    }
  }

  // Try Upcasting to Nat
  if (gstcTypeMatchA(NeededType,sort_nat::nat()).defined())
  {
    if (gstcTypeMatchA(Type,sort_pos::pos()).defined())
    {
      aterm_appl OldPar=Par;
      Par=make_application(sort_nat::cnat(),Par);
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Nat by applying Pos2Nat to it." << std::endl;
      }
      return sort_nat::nat();
    }
    if (gstcTypeMatchA(Type,sort_nat::nat()).defined())
    {
      return sort_nat::nat();
    }
  }

  // Try Upcasting to Int
  if (gstcTypeMatchA(NeededType,sort_int::int_()).defined())
  {
    if (gstcTypeMatchA(Type,sort_pos::pos()).defined())
    {
      aterm_appl OldPar=Par;
      Par=make_application(sort_int::cint(),make_application(sort_nat::cnat(),Par));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Int by applying Pos2Int to it." << std::endl;
      }
      return sort_int::int_();
    }
    if (gstcTypeMatchA(Type,sort_nat::nat()).defined())
    {
      aterm_appl OldPar=Par;
      Par=make_application(sort_int::cint(),Par);
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Int by applying Nat2Int to it." << std::endl;
      }
      return sort_int::int_();
    }
    if (gstcTypeMatchA(Type,sort_int::int_()).defined())
    {
      return sort_int::int_();
    }
  }

  // Try Upcasting to Real
  if (gstcTypeMatchA(NeededType,sort_real::real_()).defined())
  {
    if (gstcTypeMatchA(Type,sort_pos::pos()).defined())
    {
      aterm_appl OldPar=Par;
      Par=make_application(sort_real::creal(),
                              make_application(sort_int::cint(), make_application(sort_nat::cnat(),Par)),
                              sort_pos::c1());
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Real by applying Pos2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (gstcTypeMatchA(Type,sort_nat::nat()).defined())
    {
      aterm_appl OldPar=Par;
      Par=make_application(sort_real::creal(),
                             make_application(sort_int::cint(),Par),
                             sort_pos::c1());
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Real by applying Nat2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (gstcTypeMatchA(Type,sort_int::int_()).defined())
    {
      aterm_appl OldPar=Par;
      Par=make_application(sort_real::creal(),Par, sort_pos::c1());
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Real by applying Int2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (gstcTypeMatchA(Type,sort_real::real_()).defined())
    {
      return sort_real::real_();
    }
  }

  return aterm_appl();
}

static aterm_list gstcInsertType(aterm_list TypeList, aterm_appl Type)
{
  for (aterm_list OldTypeList=TypeList; !OldTypeList.empty(); OldTypeList=OldTypeList.tail())
  {
    if (gstcEqTypesA(ATAgetFirst(OldTypeList),Type))
    {
      return TypeList;
    }
  }
  aterm_list result=TypeList;
  result.push_front(Type);
  return result;
}

static aterm_list gstcTypeListsIntersect(aterm_list TypeListList1, aterm_list TypeListList2)
{
  // returns the intersection of the 2 type list lists

  aterm_list Result;

  for (; !TypeListList2.empty(); TypeListList2=TypeListList2.tail())
  {
    aterm_list TypeList2=ATLgetFirst(TypeListList2);
    if (gstcInTypesL(TypeList2,TypeListList1))
    {
      Result.push_front(TypeList2);
    }
  }
  return reverse(Result);
}

static aterm_list gstcAdjustNotInferredList(aterm_list PosTypeList, aterm_list TypeListList)
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
  aterm_list NewTypeListList;
  for (; !TypeListList.empty(); TypeListList=TypeListList.tail())
  {
    aterm_list TypeList=ATLgetFirst(TypeListList);
    if (gstcIsTypeAllowedL(TypeList,PosTypeList))
    {
      NewTypeListList.push_front(TypeList);
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

static bool gstcIsTypeAllowedL(aterm_list TypeList, aterm_list PosTypeList)
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

static bool gstcIsTypeAllowedA(aterm_appl Type, aterm_appl PosType)
{
  //Checks if Type is allowed by PosType
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return true;
  }
  if (gsIsSortsPossible(PosType))
  {
    return gstcInTypesA(Type,aterm_cast<aterm_list>(PosType[0]));
  }

  //PosType is a normal type
  return gstcEqTypesA(Type,PosType);
}

static aterm_appl gstcTypeMatchA(aterm_appl Type, aterm_appl PosType)
{
  // Checks if Type and PosType match by instantiating unknown sorts.
  // It returns the matching instantiation of Type. If matching fails,
  // it returns NULL.

  mCRL2log(debug) << "gstcTypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) << " " << std::endl;

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
    aterm_appl TmpType=PosType;
    PosType=Type;
    Type=TmpType;
  }
  if (gsIsSortsPossible(PosType))
  {
    aterm_list NewTypeList;
    for (aterm_list PosTypeList=aterm_cast<aterm_list>(PosType[0]); !PosTypeList.empty(); PosTypeList=PosTypeList.tail())
    {
      aterm_appl NewPosType=ATAgetFirst(PosTypeList);
      mCRL2log(debug) << "Matching candidate gstcTypeMatchA Type: " << pp(Type) << ";    PosType: "
                  << pp(PosType) << " New Type: " << pp(NewPosType) << "" << std::endl;

      if ((NewPosType=gstcTypeMatchA(Type,NewPosType)).defined())
      {
        mCRL2log(debug) << "Match gstcTypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) <<
                    " New Type: " << pp(NewPosType) << "" << std::endl;
        NewTypeList.push_front(NewPosType);
      }
    }
    if (NewTypeList.empty())
    {
      mCRL2log(debug) << "No match gstcTypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) << " " << std::endl;
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
    aterm_appl ConsType = aterm_cast<aterm_appl>(Type[0]);
    if (gsIsSortList(ConsType))
    {
      if (!sort_list::is_list(sort_expression(PosType)))
      {
        return aterm_appl();
      }
      aterm_appl Res=gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]));
      if (!Res.defined())
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
        aterm_appl Res=gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]));
        if (!Res.defined())
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
        aterm_appl Res=gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]));
        if (!Res.defined())
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
      aterm_list ArgTypes=gstcTypeMatchL(aterm_cast<aterm_list>(Type[0]),aterm_cast<aterm_list>(PosType[0]));
      if (!ArgTypes.defined())
      {
        return aterm_appl();
      }
      aterm_appl ResType=gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]));
      if (!ResType.defined())
      {
        return aterm_appl();
      }
      Type=gsMakeSortArrow(ArgTypes,ResType);
      // mCRL2log(debug) << "gstcTypeMatchA Done: Type: " << pp(Type) << ";    PosType: " << pp(PosType) << "" << std::endl;
      return Type;
    }
  }

  return aterm_appl();
}

static aterm_list gstcTypeMatchL(aterm_list TypeList, aterm_list PosTypeList)
{
  mCRL2log(debug) << "gstcTypeMatchL TypeList: " << pp(TypeList) << ";    PosTypeList: " <<
              pp(PosTypeList) << "" << std::endl;

  if (TypeList.size()!=PosTypeList.size())
  {
    return aterm_list(aterm_appl());
  }

  aterm_list Result;
  for (; !TypeList.empty(); TypeList=TypeList.tail(),PosTypeList=PosTypeList.tail())
  {
    aterm_appl Type=gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList));
    if (!Type.defined())
    {
      return aterm_list(aterm_appl());
    }
    Result.push_front(Type);
  }
  return reverse(Result);
}

static bool gstcIsNotInferredL(aterm_list TypeList)
{
  for (; !TypeList.empty(); TypeList=TypeList.tail())
  {
    aterm_appl Type=ATAgetFirst(TypeList);
    if (is_unknown_sort(Type) || is_multiple_possible_sorts(Type))
    {
      return true;
    }
  }
  return false;
}

static aterm_appl gstcUnwindType(aterm_appl Type)
{
  if (gsIsSortCons(Type))
  {
    return Type.set_argument(gstcUnwindType(aterm_cast<aterm_appl>(Type[1])),1);
  }
  if (gsIsSortArrow(Type))
  {
    Type=Type.set_argument(gstcUnwindType(aterm_cast<aterm_appl>(Type[1])),1);
    aterm_list Args=aterm_cast<aterm_list>(Type[0]);
    aterm_list NewArgs;
    for (; !Args.empty(); Args=Args.tail())
    {
      NewArgs.push_front(gstcUnwindType(ATAgetFirst(Args)));
    }
    NewArgs=reverse(NewArgs);
    Type=Type.set_argument(NewArgs,0);
    return Type;
  }

  if (gsIsSortId(Type))
  {
    aterm_appl Value=aterm_cast<aterm_appl>(context.defined_sorts.get(aterm_cast<aterm_appl>(Type[0])));
    if (!Value.defined())
    {
      return Type;
    }
    return gstcUnwindType(Value);
  }

  return Type;
}

static aterm_appl gstcUnSet(aterm_appl PosType)
{
  //select Set(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_set::is_set(sort_expression(PosType)))
  {
    return aterm_cast<aterm_appl>(PosType[1]);
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  aterm_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (aterm_list PosTypes=aterm_cast<aterm_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      aterm_appl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_set::is_set(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<aterm_appl>(NewPosType[1]);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
}

static aterm_appl gstcUnBag(aterm_appl PosType)
{
  //select Bag(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_bag::is_bag(sort_expression(PosType)))
  {
    return aterm_cast<aterm_appl>(PosType[1]);
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  aterm_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (aterm_list PosTypes=aterm_cast<aterm_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      aterm_appl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_bag::is_bag(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<aterm_appl>(NewPosType[1]);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
}

static aterm_appl gstcUnList(aterm_appl PosType)
{
  //select List(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_list::is_list(sort_expression(PosType)))
  {
    return aterm_cast<aterm_appl>(PosType[1]);
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  aterm_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (aterm_list PosTypes=aterm_cast<aterm_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      aterm_appl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (sort_list::is_list(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<aterm_appl>(NewPosType[1]);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    return multiple_possible_sorts(sort_expression_list(NewPosTypes));
    // return multiple_possible_sorts(atermpp::aterm_list(NewPosTypes));
  }
  return aterm_appl();
}

static aterm_appl gstcUnArrowProd(aterm_list ArgTypes, aterm_appl PosType)
{
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //return TypeX if unique, the set of TypeX as NotInferred if many, NULL otherwise

  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (gsIsSortArrow(PosType))
  {
    aterm_list PosArgTypes=aterm_cast<aterm_list>(PosType[0]);

    if (PosArgTypes.size()!=ArgTypes.size())
    {
      return aterm_appl();
    }
    if (gstcTypeMatchL(PosArgTypes,ArgTypes).defined())
    {
      return aterm_cast<aterm_appl>(PosType[1]);
    }
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return PosType;
  }

  aterm_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (aterm_list PosTypes=aterm_cast<aterm_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      aterm_appl NewPosType=ATAgetFirst(PosTypes);
      if (gsIsSortId(NewPosType))
      {
        NewPosType=gstcUnwindType(NewPosType);
      }
      if (gsIsSortArrow(PosType))
      {
        aterm_list PosArgTypes=aterm_cast<aterm_list>(PosType[0]);
        if (PosArgTypes.size()!=ArgTypes.size())
        {
          continue;
        }
        if (gstcTypeMatchL(PosArgTypes,ArgTypes).defined())
        {
          NewPosType=aterm_cast<aterm_appl>(NewPosType[1]);
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

static aterm_list gstcGetVarTypes(aterm_list VarDecls)
{
  aterm_list Result;
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    Result.push_front(aterm_cast<aterm_appl>(ATAgetFirst(VarDecls)[1]));
  }
  return reverse(Result);
}

// Replace occurrences of multiple_possible_sorts([s1,...,sn]) by selecting
// one of the possible sorts from s1,...,sn. Currently, the first is chosen.
static aterm_appl replace_possible_sorts(aterm_appl Type)
{
  if (gsIsSortsPossible(data::sort_expression(Type)))
  {
    return ATAgetFirst(aterm_cast<aterm_list>(Type[0])); // get the first element of the possible sorts.
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
    return Type.set_argument(replace_possible_sorts(aterm_cast<aterm_appl>(Type[1])),1);
  }

  if (gsIsSortStruct(Type))
  {
    return Type;  // I assume that there are no possible sorts in sort constructors. JFG.
  }

  if (gsIsSortArrow(Type))
  {
    aterm_list NewTypeList;
    for (aterm_list TypeList=aterm_cast<aterm_list>(Type[0]); !TypeList.empty(); TypeList=TypeList.tail())
    {
      NewTypeList.push_front(replace_possible_sorts(ATAgetFirst(TypeList)));
    }
    aterm_appl ResultType=aterm_cast<aterm_appl>(Type[1]);
    return gsMakeSortArrow(reverse(NewTypeList),replace_possible_sorts(ResultType));
  }
  assert(0); // All cases are dealt with above.
  return Type; // Avoid compiler warnings.
}


static bool gstcHasUnknown(aterm_appl Type)
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
    return gstcHasUnknown(aterm_cast<aterm_appl>(Type[1]));
  }
  if (gsIsSortStruct(Type))
  {
    return false;
  }

  if (gsIsSortArrow(Type))
  {
    for (aterm_list TypeList=aterm_cast<aterm_list>(Type[0]); !TypeList.empty(); TypeList=TypeList.tail())
      if (gstcHasUnknown(ATAgetFirst(TypeList)))
      {
        return true;
      }
    return gstcHasUnknown(aterm_cast<aterm_appl>(Type[1]));
  }

  return true;
}

static bool gstcIsNumericType(aterm_appl Type)
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

static aterm_appl gstcMaximumType(aterm_appl Type1, aterm_appl Type2)
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

static aterm_appl gstcExpandNumTypesUp(aterm_appl Type)
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
    return Type.set_argument(gstcExpandNumTypesUp(aterm_cast<aterm_appl>(Type[1])),1);
  }
  if (gsIsSortStruct(Type))
  {
    return Type;
  }

  if (gsIsSortArrow(Type))
  {
    //the argument types, and if the resulting type is SortArrow -- recursively
    aterm_list NewTypeList;
    for (aterm_list TypeList=aterm_cast<aterm_list>(Type[0]); !TypeList.empty(); TypeList=TypeList.tail())
    {
      NewTypeList.push_front(gstcExpandNumTypesUp(gstcUnwindType(ATAgetFirst(TypeList))));
    }
    aterm_appl ResultType=aterm_cast<aterm_appl>(Type[1]);
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

static aterm_appl gstcExpandNumTypesDown(aterm_appl Type)
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
  aterm_list Args=aterm_list(aterm());
  if (gsIsSortArrow(Type))
  {
    function=true;
    Args=aterm_cast<aterm_list>(Type[0]);
    Type=aterm_cast<aterm_appl>(Type[1]);
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

static aterm_appl gstcMinType(aterm_list TypeList)
{
  return ATAgetFirst(TypeList);
}


// =========================== MultiActions
static bool gstcMActIn(aterm_list MAct, aterm_list MActs)
{
  //returns true if MAct is in MActs
  for (; !MActs.empty(); MActs=MActs.tail())
    if (gstcMActEq(MAct,ATLgetFirst(MActs)))
    {
      return true;
    }

  return false;
}

static bool gstcMActEq(aterm_list MAct1, aterm_list MAct2)
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
  aterm_appl Act1=ATAgetFirst(MAct1);
  MAct1=MAct1.tail();

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  aterm_list NewMAct2;
  for (; !MAct2.empty(); MAct2=MAct2.tail())
  {
    aterm_appl Act2=ATAgetFirst(MAct2);
    if (Act1==Act2)
    {
      MAct2=reverse(NewMAct2)+MAct2.tail();
      return gstcMActEq(MAct1,MAct2);
    }
    else
    {
      NewMAct2.push_front(Act2);
    }
  }
  return false;
}

static aterm_appl gstcUnifyMinType(aterm_appl Type1, aterm_appl Type2)
{
  //Find the minimal type that Unifies the 2. If not possible, return aterm_appl().
  aterm_appl Res=gstcTypeMatchA(Type1,Type2);
  if (!Res.defined())
  {
    Res=gstcTypeMatchA(Type1,gstcExpandNumTypesUp(Type2));
    if (!Res.defined())
    {
      Res=gstcTypeMatchA(Type2,gstcExpandNumTypesUp(Type1));
    }
    if (!Res.defined())
    {
      mCRL2log(debug) << "gstcUnifyMinType: No match: Type1 " << pp(Type1) << "; Type2 " << pp(Type2) << "; " << std::endl;
      return aterm_appl();
    }
  }

  if (gsIsSortsPossible(Res))
  {
    Res=ATAgetFirst(aterm_cast<aterm_list>(Res[0]));
  }
  mCRL2log(debug) << "gstcUnifyMinType: Type1 " << pp(Type1) << "; Type2 " << pp(Type2) << "; Res: " << pp(Res) << "" << std::endl;
  return Res;
}

static aterm_appl gstcMatchIf(aterm_appl Type)
{
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  assert(gsIsSortArrow(Type));
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  assert((Args.size()==3));
  Args=Args.tail();

  if (!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args))).defined())
  {
    return aterm_appl();
  }
  Args=Args.tail();
  if (!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args))).defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(sort_bool::bool_(),Res,Res),Res);
}

static aterm_appl gstcMatchEqNeqComparison(aterm_appl Type)
{
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));
  aterm_appl Arg1=ATAgetFirst(Args);
  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);

  aterm_appl Arg=gstcUnifyMinType(Arg1,Arg2);
  if (!Arg.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(Arg,Arg),sort_bool::bool_());
}

static aterm_appl gstcMatchListOpCons(aterm_appl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(gstcUnwindType(Res))));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));
  aterm_appl Arg1=ATAgetFirst(Args);
  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=aterm_cast<aterm_appl>(Arg2[1]);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(Res,static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
}

static aterm_appl gstcMatchListOpSnoc(aterm_appl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));
  aterm_appl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1[1]);

  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res))),Res),sort_list::list(sort_expression(Res)));
}

static aterm_appl gstcMatchListOpConcat(aterm_appl Type)
{
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));

  aterm_appl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1[1]);

  Args=Args.tail();

  aterm_appl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=aterm_cast<aterm_appl>(Arg2[1]);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res))),
       static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
}

static aterm_appl gstcMatchListOpEltAt(aterm_appl Type)
{
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));

  aterm_appl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1[1]);

  Res=gstcUnifyMinType(Res,Arg1);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  //assert((gsIsSortNat(ATAgetFirst(Args.tail())));

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res))),
               static_cast<aterm_appl>(sort_nat::nat())),Res);
}

static aterm_appl gstcMatchListOpHead(aterm_appl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==1));
  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  Res=gstcUnifyMinType(Res,Arg);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),Res);
}

static aterm_appl gstcMatchListOpTail(aterm_appl Type)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==1));
  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  Res=gstcUnifyMinType(Res,Arg);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),
                   sort_list::list(sort_expression(Res)));
}

//Sets
static aterm_appl gstcMatchSetOpSet2Bag(aterm_appl Type)
{
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_bag::is_bag(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);

  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==1));

  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  Arg=gstcUnifyMinType(Arg,Res);
  if (!Arg.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_set::set_(sort_expression(Arg)))),
                  sort_bag::bag(sort_expression(Arg)));
}

static aterm_appl gstcMatchListSetBagOpIn(aterm_appl Type)
{
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  //assert(gsIsBool(aterm_cast<aterm_appl>(Type[1])));
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));

  aterm_appl Arg1=ATAgetFirst(Args);

  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  assert(gsIsSortCons(Arg2));
  aterm_appl Arg2s=aterm_cast<aterm_appl>(Arg2[1]);

  aterm_appl Arg=gstcUnifyMinType(Arg1,Arg2s);
  if (!Arg.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(Arg,Arg2.set_argument(Arg,1)),sort_bool::bool_());
}

static aterm_appl gstcMatchSetBagOpUnionDiffIntersect(aterm_appl Type)
{
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Res))||sort_bag::is_bag(sort_expression(Res)));
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==2));

  aterm_appl Arg1=ATAgetFirst(Args);
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

  aterm_appl Arg2=ATAgetFirst(Args);
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
  if (!Res.defined())
  {
    return aterm_appl();
  }

  Res=gstcUnifyMinType(Res,Arg2);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(Res,Res),Res);
}

static aterm_appl gstcMatchSetOpSetCompl(aterm_appl Type)
{
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==1));

  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  if (gstcIsNumericType(Arg))
  {
    return Type;
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  Res=gstcUnifyMinType(Res,Arg);
  if (!Res.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_set::set_(sort_expression(Res)))),sort_set::set_(sort_expression(Res)));
}

//Bags
static aterm_appl gstcMatchBagOpBag2Set(aterm_appl Type)
{
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));

  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);

  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==1));

  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_bag::is_bag(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  Arg=gstcUnifyMinType(Arg,Res);
  if (!Arg.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_bag::bag(sort_expression(Arg)))),sort_set::set_(sort_expression(Arg)));
}

static aterm_appl gstcMatchBagOpBagCount(aterm_appl Type)
{
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  //If the second argument is not a Bag, don't match

  if (!gsIsSortArrow(Type))
  {
    return Type;
  }
  //assert(gsIsNat(aterm_cast<aterm_appl>(Type[1])));
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  if (!(Args.size()==2))
  {
    return Type;
  }

  aterm_appl Arg1=ATAgetFirst(Args);

  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);
  if (gsIsSortId(Arg2))
  {
    Arg2=gstcUnwindType(Arg2);
  }
  if (!sort_bag::is_bag(sort_expression(Arg2)))
  {
    return Type;
  }
  Arg2=aterm_cast<aterm_appl>(Arg2[1]);

  aterm_appl Arg=gstcUnifyMinType(Arg1,Arg2);
  if (!Arg.defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(Arg,static_cast<aterm_appl>(sort_bag::bag(sort_expression(Arg)))),sort_nat::nat());
}


static aterm_appl gstcMatchFuncUpdate(aterm_appl Type)
{
  //tries to sort out the types of FuncUpdate ((A->B)xAxB->(A->B))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(Type));
  aterm_list Args=aterm_cast<aterm_list>(Type[0]);
  assert((Args.size()==3));
  aterm_appl Arg1=ATAgetFirst(Args);
  assert(gsIsSortArrow(Arg1));
  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);
  Args=Args.tail();
  aterm_appl Arg3=ATAgetFirst(Args);
  aterm_appl Res=aterm_cast<aterm_appl>(Type[1]);
  assert(gsIsSortArrow(Res));

  Arg1=gstcUnifyMinType(Arg1,Res);
  if (!Arg1.defined())
  {
    return aterm_appl();
  }

  // determine A and B from Arg1:
  aterm_list LA=aterm_cast<aterm_list>(Arg1[0]);
  assert((LA.size()==1));
  aterm_appl A=ATAgetFirst(LA);
  aterm_appl B=aterm_cast<aterm_appl>(Arg1[1]);

  if (!gstcUnifyMinType(A,Arg2).defined())
  {
    return aterm_appl();
  }
  if (!gstcUnifyMinType(B,Arg3).defined())
  {
    return aterm_appl();
  }

  return gsMakeSortArrow(make_list<aterm>(Arg1,A,B),Arg1);
}

static void gstcErrorMsgCannotCast(aterm_appl CandidateType, aterm_list Arguments, aterm_list ArgumentTypes)
{
  //prints more information about impossible cast.
  //at this point we know that Arguments cannot be cast to CandidateType. We need to find out why and print.
  assert(Arguments.size()==ArgumentTypes.size());

  aterm_list CandidateList;
  if (gsIsSortsPossible(CandidateType))
  {
    CandidateList=aterm_cast<aterm_list>(CandidateType[0]);
  }
  else
  {
    CandidateList=make_list<aterm>(CandidateType);
  }

  aterm_list NewCandidateList;
  for (aterm_list l=CandidateList; !l.empty(); l=l.tail())
  {
    aterm_appl Candidate=ATAgetFirst(l);
    if (!gsIsSortArrow(Candidate))
    {
      continue;
    }
    NewCandidateList.push_front(aterm_cast<aterm_list>(Candidate[0]));
  }
  CandidateList=reverse(NewCandidateList);

  //CandidateList=gstcTraverseListList(CandidateList);
  aterm_list CurrentCandidateList=CandidateList;
  CandidateList=aterm_list();
  while (true)
  {
    aterm_list NewCurrentCandidateList;
    aterm_list NewList;
    for (aterm_list l=CurrentCandidateList; !l.empty(); l=l.tail())
    {
      aterm_list List=ATLgetFirst(l);
      if (!List.empty())
      {
        NewList.push_front(ATAgetFirst(List));
        NewCurrentCandidateList=ATinsertUnique(NewCurrentCandidateList,List.tail());
      }
      else
      {
        NewCurrentCandidateList.push_front(aterm_list());
      }
    }
    if (NewList.empty())
    {
      break;
    }
    CurrentCandidateList=reverse(NewCurrentCandidateList);
    CandidateList.push_front(reverse(NewList));
  }
  CandidateList=reverse(CandidateList);

  for (aterm_list l=Arguments, m=ArgumentTypes, n=CandidateList; !(l.empty()||m.empty()||n.empty()); l=l.tail(), m=m.tail(), n=n.tail())
  {
    aterm_list PosTypes=ATLgetFirst(n);
    aterm_appl NeededType=ATAgetFirst(m);
    bool found=true;
    for (aterm_list k=PosTypes; !k.empty(); k=k.tail())
    {
      if (gstcTypeMatchA(ATAgetFirst(k),NeededType).defined())
      {
        found=false;
        break;
      }
    }
    if (found)
    {
      aterm_appl Sort;
      if (PosTypes.size()==1)
      {
        Sort=ATAgetFirst(PosTypes);
      }
      else
      {
        // Sort=multiple_possible_sorts(atermpp::aterm_list(PosTypes));
        Sort=multiple_possible_sorts(sort_expression_list(PosTypes));
      }
      throw mcrl2::runtime_error("this is, for instance, because cannot cast " + pp(ATAgetFirst(l)) + " to type " + pp(Sort));
      // mCRL2log(error) << "this is, for instance, because cannot cast " << pp(ATAgetFirst(l)) << " to type " << pp(Sort) << std::endl;
      break;
    }
  }
}

//===================================
// Type checking modal formulas
//===================================

static aterm_appl gstcTraverseStateFrm(const table &Vars, const table &StateVars, aterm_appl StateFrm)
{
  mCRL2log(debug) << "gstcTraverseStateFrm: " + pp(StateFrm) + "" << std::endl;

  if (gsIsStateTrue(StateFrm) || gsIsStateFalse(StateFrm) || gsIsStateDelay(StateFrm) || gsIsStateYaled(StateFrm))
  {
    return StateFrm;
  }

  if (gsIsStateNot(StateFrm))
  {
    aterm_appl NewArg=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[0]));
    if (!NewArg.defined())
    {
      return aterm_appl();
    }
    return StateFrm.set_argument(NewArg,0);
  }

  if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm) || gsIsStateImp(StateFrm))
  {
    aterm_appl NewArg1=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[0]));
    if (!NewArg1.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewArg2=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    if (!NewArg2.defined())
    {
      return aterm_appl();
    }
    return StateFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm))
  {
    table CopyVars=table(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    aterm_list VarList=aterm_cast<aterm_list>(StateFrm[0]);
    table NewVars;
    gstcAddVars2Table(CopyVars,VarList,NewVars);

    aterm_appl NewArg2=gstcTraverseStateFrm(NewVars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    if (!NewArg2.defined())
    {
      return aterm_appl();
    }

    return StateFrm.set_argument(NewArg2,1);
  }

  if (gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm))
  {
    aterm_appl RegFrm=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(StateFrm[0]));
    if (!RegFrm.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewArg2=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    if (!NewArg2.defined())
    {
      return aterm_appl();
    }
    return StateFrm.set_argument(RegFrm,0).set_argument(NewArg2,1);
  }

  if (gsIsStateDelayTimed(StateFrm) || gsIsStateYaledTimed(StateFrm))
  {
    data_expression Time=aterm_cast<aterm_appl>(StateFrm[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (!NewType.defined())
    {
      return aterm_appl();
    }

    if (!gstcTypeMatchA(sort_real::real_(),NewType).defined())
    {
      //upcasting
      aterm_appl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,Time);
      if (!CastedNewType.defined())
      {
        throw mcrl2::runtime_error("cannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
      }
    }
    return StateFrm.set_argument(Time,0);
  }

  if (gsIsStateVar(StateFrm))
  {
    aterm_appl StateVarName=aterm_cast<aterm_appl>(StateFrm[0]);
    aterm_list TypeList=aterm_cast<aterm_list>(StateVars.get(StateVarName));
    if (!TypeList.defined())
    {
      throw mcrl2::runtime_error("undefined state variable " + pp(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
    }

    aterm_list Pars=aterm_cast<aterm_list>(StateFrm[1]);
    if (TypeList.size()!=Pars.size())
    {
      throw mcrl2::runtime_error("incorrect number of parameters for state variable " + pp(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
    }

    aterm_list r;
    
    for (; !Pars.empty(); Pars=Pars.tail(),TypeList=TypeList.tail())
    {
      data_expression Par=ATAgetFirst(Pars);
      aterm_appl ParType=ATAgetFirst(TypeList);
      aterm_appl NewParType=gstcTraverseVarConsTypeD(Vars,Vars,Par,gstcExpandNumTypesDown(ParType));
      if (!NewParType.defined())
      {
        throw mcrl2::runtime_error("typechecking " + pp(StateFrm));
      }

      if (!gstcTypeMatchA(ParType,NewParType).defined())
      {
        //upcasting
        NewParType=gstcUpCastNumericType(ParType,NewParType,Par);
        if (!NewParType.defined())
        {
          throw mcrl2::runtime_error("cannot (up)cast " + pp(Par) + " to type " + pp(ParType) + " (typechecking state formula " + pp(StateFrm) + ")");
        }
      }

      r.push_front(Par);
    }

    return StateFrm.set_argument(reverse(r),1);

  }

  if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm))
  {
    table CopyStateVars=table(63,50);
    gstcATermTableCopy(StateVars,CopyStateVars);

    // Make the new state variable:
    table FormPars=table(63,50);
    aterm_list r;
    aterm_list t;
    for (aterm_list l=aterm_cast<aterm_list>(StateFrm[1]); !l.empty(); l=l.tail())
    {
      aterm_appl o=ATAgetFirst(l);

      aterm_appl VarName=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(o[0])[0]);
      if (FormPars.get(VarName).defined())
      {
        throw mcrl2::runtime_error("non-unique formal parameter " + pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
      }

      aterm_appl VarType=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(o[0])[1]);
      try
      {
        gstcIsSortExprDeclared(VarType);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
      }

      FormPars.put(VarName, VarType);

      data_expression VarInit=aterm_cast<aterm_appl>(o[1]);
      aterm_appl VarInitType=gstcTraverseVarConsTypeD(Vars,Vars,VarInit,gstcExpandNumTypesDown(VarType));
      if (!VarInitType.defined())
      {
        throw mcrl2::runtime_error("typechecking " + pp(StateFrm));
      }

      if (!gstcTypeMatchA(VarType,VarInitType).defined())
      {
        //upcasting
        VarInitType=gstcUpCastNumericType(VarType,VarInitType,VarInit);
        if (!VarInitType.defined())
        {
          throw mcrl2::runtime_error("cannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
        }
      }

      r.push_front(o.set_argument(VarInit,1));
      t.push_front(VarType);
    }

    StateFrm=StateFrm.set_argument(reverse(r),1);
    table CopyVars=table(63,50);
    gstcATermTableCopy(Vars,CopyVars);
    gstcATermTableCopy(FormPars,CopyVars);

    CopyStateVars.put(aterm_cast<aterm_appl>(StateFrm[0]),reverse(t));
    aterm_appl NewArg=gstcTraverseStateFrm(CopyVars,CopyStateVars,aterm_cast<aterm_appl>(StateFrm[2]));
    if (!NewArg.defined())
    {
      throw mcrl2::runtime_error("while typechecking " + pp(StateFrm));
    }
    return StateFrm.set_argument(NewArg,2);
  }

  if (gsIsDataExpr(StateFrm))
  {
    data_expression d(StateFrm);
    aterm_appl Type=gstcTraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    if (!Type.defined())
    {
      return aterm_appl();
    }
    return StateFrm;
  }

  assert(0);
  return aterm_appl();
}

static aterm_appl gstcTraverseRegFrm(const table &Vars, aterm_appl RegFrm)
{
  mCRL2log(debug) << "gstcTraverseRegFrm: " + pp(RegFrm) + "" << std::endl;
  if (gsIsRegNil(RegFrm))
  {
    return RegFrm;
  }

  if (gsIsRegSeq(RegFrm) || gsIsRegAlt(RegFrm))
  {
    aterm_appl NewArg1=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[0]));
    if (!NewArg1.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewArg2=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[1]));
    if (!NewArg2.defined())
    {
      return aterm_appl();
    }
    return RegFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsRegTrans(RegFrm) || gsIsRegTransOrNil(RegFrm))
  {
    aterm_appl NewArg=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[0]));
    if (!NewArg.defined())
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

static aterm_appl gstcTraverseActFrm(const table &Vars, aterm_appl ActFrm)
{
  mCRL2log(debug) << "gstcTraverseActFrm: " + pp(ActFrm) + "" << std::endl;

  if (gsIsActTrue(ActFrm) || gsIsActFalse(ActFrm))
  {
    return ActFrm;
  }

  if (gsIsActNot(ActFrm))
  {
    aterm_appl NewArg=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));
    if (!NewArg.defined())
    {
      return aterm_appl();
    }
    return ActFrm.set_argument(NewArg,0);
  }

  if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm) || gsIsActImp(ActFrm))
  {
    aterm_appl NewArg1=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));
    if (!NewArg1.defined())
    {
      return aterm_appl();
    }
    aterm_appl NewArg2=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[1]));
    if (!NewArg2.defined())
    {
      return aterm_appl();
    }
    return ActFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm))
  {
    table CopyVars=table(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    aterm_list VarList=aterm_cast<aterm_list>(ActFrm[0]);
    table NewVars;
    gstcAddVars2Table(CopyVars,VarList,NewVars);
    
    aterm_appl NewArg2=gstcTraverseActFrm(NewVars,aterm_cast<aterm_appl>(ActFrm[1]));
    if (!NewArg2.defined())
    {
      return aterm_appl();
    }

    return ActFrm.set_argument(NewArg2,1);
  }

  if (gsIsActAt(ActFrm))
  {
    aterm_appl NewArg1=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));
    if (!NewArg1.defined())
    {
      return aterm_appl();
    }

    data_expression Time=aterm_cast<aterm_appl>(ActFrm[1]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Time,gstcExpandNumTypesDown(sort_real::real_()));
    if (!NewType.defined())
    {
      return aterm_appl();
    }

    if (!gstcTypeMatchA(sort_real::real_(),NewType).defined())
    {
      //upcasting
      aterm_appl CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,Time);
      if (!CastedNewType.defined())
      {
        throw mcrl2::runtime_error("cannot (up)cast time value " + pp(Time) + " to type Real (typechecking action formula " + pp(ActFrm) + ")");
      }
    }
    return ActFrm.set_argument(NewArg1,0).set_argument(Time,1);
  }

  if (gsIsMultAct(ActFrm))
  {
    aterm_list r;
    for (aterm_list l=aterm_cast<aterm_list>(ActFrm[0]); !l.empty(); l=l.tail())
    {
      aterm_appl o=ATAgetFirst(l);
      assert(gsIsParamId(o));
      o=gstcTraverseActProcVarConstP(Vars,o);
      if (!o.defined())
      {
        return aterm_appl();
      }
      r.push_front(o);
    }
    return ActFrm.set_argument(reverse(r),0);
  }

  if (gsIsDataExpr(ActFrm))
  {
    data_expression d(ActFrm);
    aterm_appl Type=gstcTraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    if (!Type.defined())
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
