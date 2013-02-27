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
#include "mcrl2/data/typecheck.h"
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

static core::identifier_string_list list_minus(const std::map<core::identifier_string,sort_expression> &l, const std::map<core::identifier_string,sort_expression> &m)
{
  core::identifier_string_list n;
  for (std::map<core::identifier_string,sort_expression>::const_reverse_iterator i=l.rbegin(); i!=l.rend(); ++i)
  {
    if (m.count(i->first)==0)
    {
      n.push_front(i->first);
    }
  }
  return n;
}

// Static data

static bool was_warning_upcasting=false;
static bool was_ambiguous=false;

// system constants and functions
typedef struct
{
  std::map <core::identifier_string,sort_expression_list> constants;   //name -> Set(sort expression)
  std::map <core::identifier_string,sort_expression_list> functions;   //name -> Set(sort expression)
} gsSystem;

static gsSystem gssystem;
// the static context of the spec will be checked and used, not transformed
typedef struct
{
  std::set <aterm_appl> basic_sorts; // contains basic_sorts.
  std::map<core::identifier_string,sort_expression> defined_sorts; //name -> sort expression
  std::map<core::identifier_string,sort_expression> constants;     //name -> sort expression
  std::map<core::identifier_string,sort_expression_list> functions;     //name -> Set(sort expression)
  std::map<core::identifier_string,term_list<sort_expression_list> > actions;   //name -> Set(List(sort expression)) because of action polymorphism
  std::map<core::identifier_string,term_list<sort_expression_list> > processes; //name -> Set(List(sort expression)) because of process polymorphism
  std::map<core::identifier_string,sort_expression> glob_vars;         //name -> Type: global variables (for proc, pbes and init)
  std::map<core::identifier_string,term_list<sort_expression_list> > PBs;
} Context;

static Context context;

// the body may be transformed
typedef struct
{
  aterm_list equations;
  std::map <aterm_appl,variable_list> proc_pars; // process_identifier -> variable_list
  std::map <aterm_appl,aterm_appl> proc_bodies;  // process_identifier -> rhs
} Body;
static Body body;

// Static function declarations
static void gstcDataInit(void);
static void gstcReadInSorts(aterm_list);
static void gstcReadInConstructors(const std::map<core::identifier_string,sort_expression>::const_iterator begin,
                                   const std::map<core::identifier_string,sort_expression>::const_iterator end);
static void gstcReadInConstructors(const core::identifier_string_list::const_iterator begin,
                                   const core::identifier_string_list::const_iterator end);
static void gstcReadInFuncs(aterm_list, aterm_list);
static void gstcReadInActs(aterm_list);
static void gstcReadInProcsAndInit(aterm_list, aterm_appl);
static void gstcReadInPBESAndInit(aterm_appl, aterm_appl);

static void gstcTransformVarConsTypeData(void);
static void gstcTransformActProcVarConst(void);
static void gstcTransformPBESVarConst(void);

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

static void gstcAddVars2Table(std::map<core::identifier_string,sort_expression> &,aterm_list, std::map<core::identifier_string,sort_expression> &);
static std::map<core::identifier_string,sort_expression> gstcRemoveVars(std::map<core::identifier_string,sort_expression> &Vars, variable_list VarDecls);
static bool gstcVarsUnique(aterm_list VarDecls);
static aterm_appl gstcRewrActProc(const std::map<core::identifier_string,sort_expression> &, aterm_appl, bool is_pbes=false);
static inline aterm_appl gstcMakeActionOrProc(bool, aterm_appl, aterm_list, aterm_list);
static aterm_appl gstcTraverseActProcVarConstP(const std::map<core::identifier_string,sort_expression> &, aterm_appl);
static aterm_appl gstcTraversePBESVarConstPB(const std::map<core::identifier_string,sort_expression> &, aterm_appl);

static sort_expression gstcTraverseVarConsTypeD(const std::map<core::identifier_string,sort_expression> &DeclaredVars, 
                                                const std::map<core::identifier_string,sort_expression> &AllowedVars, 
                                                data_expression &, 
                                                sort_expression, 
                                                std::map<core::identifier_string,sort_expression> &FreeVars, 
                                                bool strictly_ambiguous=true, const bool warn_upcasting=false, const bool print_cast_error=true);
static sort_expression gstcTraverseVarConsTypeD(const std::map<core::identifier_string,sort_expression> &DeclaredVars, 
                                                const std::map<core::identifier_string,sort_expression> &AllowedVars, 
                                                data_expression &t1, 
                                                sort_expression t2)
{
  std::map<core::identifier_string,sort_expression> dummy_table;
  return gstcTraverseVarConsTypeD(DeclaredVars, AllowedVars, t1, t2,
        dummy_table, true, false, true);
}
static sort_expression gstcTraverseVarConsTypeDN(const std::map<core::identifier_string,sort_expression> &DeclaredVars, 
                                                 const std::map<core::identifier_string,sort_expression>&AllowedVars, 
                                                 data_expression & , 
                                                 sort_expression,
                                                 std::map<core::identifier_string,sort_expression> &FreeVars, 
                                                 bool strictly_ambiguous=true, 
                                                 size_t nPars = std::string::npos, 
                                                 const bool warn_upcasting=false, 
                                                 const bool print_cast_error=true);

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
static bool gstcMaximumType(const aterm_appl &Type1, const aterm_appl &Type2, aterm_appl &result);

static aterm_list gstcGetNotInferredList(aterm_list TypeListList);
static std::pair<bool,aterm_list> gstcAdjustNotInferredList(aterm_list TypeList, aterm_list TypeListList);
static bool gstcIsNotInferredL(aterm_list TypeListList);
static bool gstcIsTypeAllowedA(aterm_appl Type, aterm_appl PosType);
static bool gstcIsTypeAllowedL(aterm_list TypeList, aterm_list PosTypeList);
static aterm_appl gstcUnwindType(aterm_appl Type);
static bool gstcUnSet(aterm_appl PosType, aterm_appl &result);
static bool gstcUnBag(aterm_appl PosType, aterm_appl &result);
static bool gstcUnList (aterm_appl PosType, aterm_appl &result);
static bool gstcUnArrowProd(aterm_list ArgTypes, aterm_appl PosType, aterm_appl &result);
static aterm_list gstcTypeListsIntersect(aterm_list TypeListList1, aterm_list TypeListList2);
static sort_expression_list gstcGetVarTypes(variable_list VarDecls);
static bool gstcTypeMatchA(const aterm_appl &Type, const aterm_appl &PosType, aterm_appl &result);
static bool gstcTypeMatchL(const aterm_list &TypeList, const aterm_list &PosTypeList, aterm_list &result);
static bool gstcHasUnknown(aterm_appl Type);
static bool gstcIsNumericType(const sort_expression &Type);
static aterm_appl gstcExpandNumTypesUp(aterm_appl Type);
static aterm_appl gstcExpandNumTypesDown(aterm_appl Type);
static aterm_appl gstcMinType(aterm_list TypeList);
static bool gstcMActIn(aterm_list MAct, aterm_list MActs);
static bool gstcMActEq(aterm_list MAct1, aterm_list MAct2);
static bool gstcUnifyMinType(const aterm_appl &Type1, const aterm_appl &Type2,aterm_appl &result);
static bool gstcMatchIf(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchEqNeqComparison(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListOpCons(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListOpSnoc(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListOpConcat(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListOpEltAt(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListOpHead(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListOpTail(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchSetOpSet2Bag(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchListSetBagOpIn(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchSetBagOpUnionDiffIntersect(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchSetOpSetCompl(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchBagOpBag2Set(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchBagOpBagCount(const aterm_appl &Type, aterm_appl &result);
static bool gstcMatchFuncUpdate(const aterm_appl &Type, aterm_appl &result);
static aterm_appl replace_possible_sorts(aterm_appl Type);


static void gstcErrorMsgCannotCast(aterm_appl CandidateType, aterm_list Arguments, aterm_list ArgumentTypes,std::string previous_reason);

// Typechecking modal formulas
static aterm_appl gstcTraverseStateFrm(const std::map<core::identifier_string,sort_expression> &Vars, 
                                       const std::map<core::identifier_string,sort_expression_list> &StateVars, 
                                       aterm_appl StateFrm);
static aterm_appl gstcTraverseRegFrm(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl RegFrm);
static aterm_appl gstcTraverseActFrm(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl ActFrm);


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

static alias_list gstcFoldSortRefsInSortRefs(alias_list SortRefs);
//Pre: SortRefs is a list of sort references
//Ret: SortRefs in which all sort references are maximally folded

static void gstcSplitSortDecls(const aterm_list &SortDecls, 
                               basic_sort_list &PSortIds,
                               alias_list &PSortRefs);
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
inline sort_expression_list ATinsertUnique(const sort_expression_list &list, const sort_expression &el)
{
  if (std::find(list.begin(),list.end(), el) == list.end())
  {
    sort_expression_list result=list;
    result.push_front(el);
    return result;
  }
  return list;
}


inline
alias_list alias_list_replace(const alias_list &list_in, const alias &el, const size_t idx) // Replace one element of a list.
{
  alias_list list=list_in;
  size_t i;
  std::vector<alias> buffer;

  for (i=0; i<idx; i++)
  {
    buffer.push_back(list.front());
    list = list.tail();
  }
  /* Skip the old element */
  list.pop_front();
  /* Add the new element */
  list.push_front(el);
  /* Add the prefix */
  for (i=idx; i>0; i--)
  {
    list.push_front(buffer[i-1]);
  }

  return list;
}

static aterm gsSubstValuesTable(const std::map <sort_expression,sort_expression> &Substs, const aterm &t)
{
  aterm Term=t;
  if (Term.type_is_appl())
  { 
    if (is_sort_expression(aterm_cast<aterm_appl>(Term)))
    {
      std::map <sort_expression,sort_expression>::const_iterator i=Substs.find(aterm_cast<sort_expression>(Term));
      if (i!=Substs.end())
      {
        return i->second;
      }
    }
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
  gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
  
  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                      aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));
  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);
  mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

  mCRL2log(debug) << "type checking transform VarConst phase started" << std::endl;

  gstcTransformVarConsTypeData();
  
  mCRL2log(debug) << "type checking transform VarConst phase finished" << std::endl;

  Result = data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);

  Result = gstcFoldSortRefs(Result);

  mCRL2log(debug) << "type checking phase finished" << std::endl;
  
  
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
  gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
  
  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                      aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));
  
  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);

  gstcReadInActs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[1])[0]));
  
  aterm_appl glob_var_spec = aterm_cast<aterm_appl>(proc_spec[2]);
  aterm_list glob_vars = aterm_cast<aterm_list>(glob_var_spec[0]);
  std::map<core::identifier_string,sort_expression> dummy;
  gstcAddVars2Table(context.glob_vars, glob_vars,dummy);
  
  gstcReadInProcsAndInit(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[3])[0]),
                             aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(proc_spec[4])[0]));
  
  mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

  mCRL2log(debug) << "type checking transform ActProc+VarConst phase started" << std::endl;
  gstcTransformVarConsTypeData();
  
  gstcTransformActProcVarConst();
  
  mCRL2log(debug) << "type checking transform ActProc+VarConst phase finished" << std::endl;

  data_spec=aterm_cast<aterm_appl>(proc_spec[0]);
  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
  Result=proc_spec.set_argument(data_spec,0);
  Result=Result.set_argument(gsMakeProcEqnSpec(gstcWriteProcs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[3])[0]))),3);
  Result=Result.set_argument(gsMakeProcessInit(aterm_cast<aterm_appl>(body.proc_bodies[INIT_KEY()])),4);

  Result=gstcFoldSortRefs(Result);

  mCRL2log(debug) << "type checking phase finished" << std::endl;
  
  
  return Result;
}

} // namespace core

// ------------------------------  Here starts the new class based sort expression checker -----------------------

bool mcrl2::data::sort_expression_checker::check_for_sort_alias_loop_through_function_sort(
  const basic_sort& start_search,
  const basic_sort& end_search,
  std::set < basic_sort > &visited,
  const bool observed_a_sort_constructor)
{
  const std::map<core::identifier_string,sort_expression>::const_iterator i=defined_sorts.find(start_search.name());

  if (i==defined_sorts.end())
  {
    // start_search is not a sort alias, and hence not a recursive sort.
    return false;
  }
  sort_expression aterm_reference= i->second;

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
  return check_for_sort_alias_loop_through_function_sort_via_expression(reference,end_search,visited,observed_a_sort_constructor);
}

bool mcrl2::data::sort_expression_checker::check_for_sort_alias_loop_through_function_sort_via_expression(
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
      return check_for_sort_alias_loop_through_function_sort(start_search,end_search,visited,observed_a_sort_constructor);
    }
  }

  if (is_container_sort(sort_expression_start_search))
  {
    // A loop through a list container is allowed, but a loop through a set or bag container
    // is problematic.
    const container_sort start_search_container(sort_expression_start_search);
    return check_for_sort_alias_loop_through_function_sort_via_expression(
             start_search_container.element_sort(),end_search,visited,
             start_search_container.container_name()!=list_container());
  }

  if (is_function_sort(sort_expression_start_search))
  {
    const function_sort f_start_search(sort_expression_start_search);
    if (check_for_sort_alias_loop_through_function_sort_via_expression(
          f_start_search.codomain(),end_search,visited,true))
    {
      return true;
    }
    for (sort_expression_list::const_iterator i=f_start_search.domain().begin();
         i!=f_start_search.domain().end(); ++i)
    {
      if (check_for_sort_alias_loop_through_function_sort_via_expression(
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
          if (check_for_sort_alias_loop_through_function_sort_via_expression(
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

void mcrl2::data::sort_expression_checker::check_sort(const basic_sort &sort)
{
  if (sort_bool::is_bool(sort))
  {
    throw mcrl2::runtime_error("attempt to redeclare sort Bool");
  }
  if (sort_pos::is_pos(sort))
  {
    throw mcrl2::runtime_error("attempt to redeclare sort Pos");
  }
  if (sort_nat::is_nat(sort))
  {
    throw mcrl2::runtime_error("attempt to redeclare sort Nat");
  }
  if (sort_int::is_int(sort))
  {
    throw mcrl2::runtime_error("attempt to redeclare sort Int");
  }
  if (sort_real::is_real(sort))
  {
    throw mcrl2::runtime_error("attempt to redeclare sort Real");
  }
  if (basic_sorts.count(sort.name())>0 || defined_sorts.count(sort.name())>0)
  {
    throw mcrl2::runtime_error("double declaration of sort " + pp(sort.name()));
  }
}

mcrl2::data::sort_expression_checker::sort_expression_checker(
                const sort_expression_vector::const_iterator sorts_begin,
                const sort_expression_vector::const_iterator sorts_end,
                const alias_vector::const_iterator aliases_begin,
                const alias_vector::const_iterator aliases_end)
{
  for (sort_expression_vector::const_iterator i=sorts_begin; i!=sorts_end; ++i)
  {
    if (is_basic_sort(*i))
    {
      const basic_sort &bsort(*i);
      check_sort(bsort);
      basic_sorts.insert(bsort.name());
    }
    else
    {
std::cerr << "Don't check sort " << pp(*i) << "\n";
    }
  }

  for (alias_vector::const_iterator i=aliases_begin; i!=aliases_end; ++i)
  {
    check_sort(i->name());
    defined_sorts[i->name().name()]=i->reference(); 
    mCRL2log(debug) << "Add sort alias " << pp(i->name()) << "  " << pp(i->reference()) << "" << std::endl;
  }

  // Check for sorts that are recursive through container sorts.
  // E.g. sort L=List(L);
  // This is forbidden.

  for (std::map<core::identifier_string,sort_expression>::const_iterator i=defined_sorts.begin(); 
              i!=defined_sorts.end(); ++i)
  {
    std::set < basic_sort > visited;
    const basic_sort s(core::identifier_string(i->first));
    const sort_expression ar(i->second);
    if (check_for_sort_alias_loop_through_function_sort_via_expression(ar,s,visited,false))
    {
      throw mcrl2::runtime_error("sort " + pp(i->first) + " is recursively defined via a function sort, or a set or a bag type container");
    }
  }
}

void mcrl2::data::sort_expression_checker::IsSortDeclared(const basic_sort &SortName)
{

  if (sort_bool::is_bool(SortName) ||
      sort_pos::is_pos(SortName) ||
      sort_nat::is_nat(SortName) ||
      sort_int::is_int(SortName) ||
      sort_real::is_real(SortName))
  {
    return;
  }
  if (basic_sorts.count(SortName.name())>0)
  {
    return;
  }
  if (defined_sorts.count(SortName.name())>0)
  {
    return;
  }
  throw mcrl2::runtime_error("basic or defined sort " + pp(SortName) + " is not declared");
}

void mcrl2::data::sort_expression_checker::IsSortExprListDeclared(const sort_expression_list &SortExprList)
{
  for (sort_expression_list::const_iterator i=SortExprList.begin(); i!=SortExprList.end(); ++i)
  {
    IsSortExprDeclared(*i);
  }
}



void mcrl2::data::sort_expression_checker::IsSortExprDeclared(const sort_expression &SortExpr)
{
  if (is_basic_sort(SortExpr))
  {
    IsSortDeclared(aterm_cast<basic_sort>(SortExpr));
    return;
  }

  if (is_container_sort(SortExpr))
  {
    IsSortExprDeclared(aterm_cast<container_sort>(SortExpr).element_sort());
    return;
  }

  if (is_function_sort(SortExpr))
  {
    IsSortExprDeclared(aterm_cast<function_sort>(SortExpr).codomain());

    IsSortExprListDeclared(aterm_cast<function_sort>(SortExpr).domain());
    return;
  }

  if (is_structured_sort(SortExpr))
  {
    const structured_sort &ssort(SortExpr);
    const structured_sort_constructor_list &constructors(ssort.struct_constructors());
    for (structured_sort_constructor_list::const_iterator i=constructors.begin(); i!=constructors.end(); ++i)
    {
      const structured_sort_constructor &Constr(*i);

      const structured_sort_constructor_argument_list &Projs(Constr.arguments());
      for (structured_sort_constructor_argument_list::const_iterator j=Projs.begin(); j!=Projs.end(); ++j)
      {
        IsSortExprDeclared(j->sort());
      }
    }
    return;
  }

  throw mcrl2::runtime_error("this is not a sort expression " + pp(SortExpr));
}

void mcrl2::data::sort_expression_checker::operator ()(const sort_expression &sort_expr)
{
  IsSortExprDeclared(sort_expr);
}

// ------------------------------  Here ends the new class based sort expression checker -----------------------
// ------------------------------  Here starts the new class based data expression checker -----------------------
bool mcrl2::data::data_expression_checker::VarsUnique(const variable_list &VarDecls)
{
  std::set<core::identifier_string> Temp;

  for (variable_list::const_iterator i=VarDecls.begin(); i!=VarDecls.end(); ++i)
  {
    variable VarDecl= *i;
    core::identifier_string VarName=VarDecl.name();
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    if (!Temp.insert(VarName).second) // The VarName is already in the set.
    {
      return false;
    }
  }
  
  return true;
}


sort_expression mcrl2::data::data_expression_checker::UpCastNumericType(
                      sort_expression NeededType, 
                      sort_expression Type, 
                      data_expression &Par, 
                      bool warn_upcasting)
{
  // Makes upcasting from Type to Needed Type for Par. Returns the resulting type.
  // Moreover, *Par is extended with the required type transformations.
  mCRL2log(debug) << "UpCastNumericType " << pp(NeededType) << " -- " << pp(Type) << "" << std::endl;

  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (data::is_unknown_sort(data::sort_expression(NeededType)))
  {
    return Type;
  }
  if (EqTypesA(NeededType,Type))
  {
    return Type;
  }

  if (data::is_multiple_possible_sorts(data::sort_expression(NeededType)))
  {
    sort_expression_list l=sort_expression_list(aterm_cast<aterm_list>(NeededType[0]));
    for(sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      bool found_solution=true;
      aterm_appl r;
      try 
      {
        r=UpCastNumericType(*i,Type,Par,warn_upcasting);
      }
      catch (mcrl2::runtime_error &e)
      { 
        found_solution=false;
      }
      if (found_solution)
      {
        return r;
      }
    }
    throw mcrl2::runtime_error("cannot transform " + pp(Type) + " to a number."); 
  }

  if (warn_upcasting && gsIsOpId(Par) && gsIsNumericString(gsATermAppl2String(aterm_cast<aterm_appl>((Par)[0]))))
  {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  sort_expression temp;
  if (TypeMatchA(NeededType,sort_pos::pos(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
    {
      return sort_pos::pos();
    }
  }

  // Try Upcasting to Nat
  if (TypeMatchA(NeededType,sort_nat::nat(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
    {
      data_expression OldPar=Par;
      Par=make_application(sort_nat::cnat(),Par);
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << pp(OldPar) << " to sort Nat by applying Pos2Nat to it." << std::endl;
      }
      return sort_nat::nat();
    }
    if (TypeMatchA(Type,sort_nat::nat(),temp))
    {
      return sort_nat::nat();
    }
  }

  // Try Upcasting to Int
  if (TypeMatchA(NeededType,sort_int::int_(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
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
    if (TypeMatchA(Type,sort_nat::nat(),temp))
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
    if (TypeMatchA(Type,sort_int::int_(),temp))
    {
      return sort_int::int_();
    }
  }

  // Try Upcasting to Real
  if (TypeMatchA(NeededType,sort_real::real_(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
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
    if (TypeMatchA(Type,sort_nat::nat(),temp))
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
    if (TypeMatchA(Type,sort_int::int_(),temp))
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
    if (TypeMatchA(Type,sort_real::real_(),temp))
    {
      return sort_real::real_();
    }
  }

  throw mcrl2::runtime_error("Upcasting " + pp(Type) + " to a number fails");
}

void mcrl2::data::data_expression_checker::ErrorMsgCannotCast(
                        sort_expression CandidateType, 
                        data_expression_list Arguments, 
                        sort_expression_list ArgumentTypes,
                        std::string previous_reason)
{
  //prints more information about impossible cast.
  //at this point we know that Arguments cannot be cast to CandidateType. We need to find out why and print.
  //The code of this routine is weird. It appears to abuse term_list<sort_expression_list> and sort_expression_list,
  //which appear to be incorrectly assigned to each other. This needs further investigation.
  assert(Arguments.size()==ArgumentTypes.size());

  /* sort_expression_list CandidateList;
  if (gsIsSortsPossible(CandidateType))
  {
    CandidateList=aterm_cast<sort_expression_list>(CandidateType[0]);
  }
  else
  {
    CandidateList=make_list<sort_expression>(CandidateType);
  }

  sort_expression_list NewCandidateList;
  for (sort_expression_list l=CandidateList; !l.empty(); l=l.tail())
  {
    sort_expression Candidate=l.front();
    if (!gsIsSortArrow(Candidate))
    {
      continue;
    }
    NewCandidateList.push_front(aterm_cast<sort_expression_list>(Candidate[0]));
  }
  CandidateList=reverse(NewCandidateList);

  sort_expression_list CurrentCandidateList=CandidateList;
  CandidateList=sort_expression_list();
  while (true)
  {
    sort_expression_list NewCurrentCandidateList;
    sort_expression_list NewList;
    for (sort_expression_list l=CurrentCandidateList; !l.empty(); l=l.tail())
    {
      sort_expression List=l.front();
      if (!List.empty())
      {
        NewList.push_front(List);
        NewCurrentCandidateList=core::ATinsertUnique(NewCurrentCandidateList,List);
      }
      else
      {
        NewCurrentCandidateList.push_front(sort_expression_list());
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

  sort_expression_list m=ArgumentTypes;
  sort_expression_list n=CandidateList;
  for (data_expression_list l=Arguments; !(l.empty()||m.empty()||n.empty()); l=l.tail(), m=m.tail(), n=n.tail())
  {
    sort_expression_list PosTypes=n;
    sort_expression NeededType=m.front();
    bool found=true;
    for (sort_expression_list k=PosTypes; !k.empty(); k=k.tail())
    {
      sort_expression temp;
      if (TypeMatchA(k.front(),NeededType,temp))
      {
        found=false;
        break;
      }
    }
    if (found)
    {
      sort_expression Sort;
      if (PosTypes.size()==1)
      {
        Sort=PosTypes.front();
      }
      else
      {
        // Sort=multiple_possible_sorts(atermpp::aterm_list(PosTypes));
        Sort=multiple_possible_sorts(sort_expression_list(PosTypes));
      }
      throw mcrl2::runtime_error(previous_reason + "\nthis is, for instance, because cannot cast " + pp(l.front()) + " to type " + pp(Sort));
    }
  }*/
  throw mcrl2::runtime_error(previous_reason + "\nfailed to construct additional explanation");
}


bool mcrl2::data::data_expression_checker::UnSet(sort_expression PosType, sort_expression &result) 
{
  //select Set(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (sort_set::is_set(sort_expression(PosType)))
  {
    result=aterm_cast<sort_expression>(PosType[1]);
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (sort_expression_list PosTypes=aterm_cast<sort_expression_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (gsIsSortId(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (sort_set::is_set(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<sort_expression>(NewPosType[1]);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

bool mcrl2::data::data_expression_checker::UnBag(sort_expression PosType, sort_expression &result) 
{
  //select Bag(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (sort_bag::is_bag(sort_expression(PosType)))
  {
    result=aterm_cast<sort_expression>(PosType[1]);
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (sort_expression_list PosTypes=aterm_cast<sort_expression_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (gsIsSortId(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (sort_bag::is_bag(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<sort_expression>(NewPosType[1]);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

bool mcrl2::data::data_expression_checker::UnList(sort_expression PosType, sort_expression &result)
{
  //select List(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (sort_list::is_list(sort_expression(PosType)))
  {
    result=aterm_cast<sort_expression>(PosType[1]);
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (sort_expression_list PosTypes=aterm_cast<sort_expression_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (gsIsSortId(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (sort_list::is_list(sort_expression(NewPosType)))
      {
        NewPosType=aterm_cast<sort_expression>(NewPosType[1]);
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}


bool mcrl2::data::data_expression_checker::UnArrowProd(sort_expression_list ArgTypes, sort_expression PosType, sort_expression &result)
{
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //result is TypeX if unique, the set of TypeX as NotInferred if many. 
  //return true if successful, otherwise false.

  if (gsIsSortId(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (gsIsSortArrow(PosType))
  {
    sort_expression_list PosArgTypes=aterm_cast<sort_expression_list>(PosType[0]);

    if (PosArgTypes.size()!=ArgTypes.size())
    {
      return false;
    }
    sort_expression_list temp;
    if (TypeMatchL(PosArgTypes,ArgTypes,temp))
    {
      result=aterm_cast<sort_expression>(PosType[1]);
      return true;
    }
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (gsIsSortsPossible(PosType))
  {
    for (sort_expression_list PosTypes=aterm_cast<sort_expression_list>(PosType[0]); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (gsIsSortId(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (gsIsSortArrow(PosType))
      {
        sort_expression_list PosArgTypes=aterm_cast<sort_expression_list>(PosType[0]);
        if (PosArgTypes.size()!=ArgTypes.size())
        {
          continue;
        }
        sort_expression_list temp_list;
        if (TypeMatchL(PosArgTypes,ArgTypes,temp_list))
        {
          NewPosType=aterm_cast<sort_expression>(NewPosType[1]);
        }
      }
      else if (!data::is_unknown_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=core::ATinsertUnique(NewPosTypes,NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

bool mcrl2::data::data_expression_checker::UnifyMinType(const sort_expression &Type1, const sort_expression &Type2, sort_expression &result)
{
  //Find the minimal type that Unifies the 2. If not possible, return false.
  if (!TypeMatchA(Type1,Type2,result))
  {
    if (!TypeMatchA(Type1,ExpandNumTypesUp(Type2),result))
    {
      if (!TypeMatchA(Type2,ExpandNumTypesUp(Type1),result))
      {
        mCRL2log(debug) << "UnifyMinType: No match: Type1 " << pp(Type1) << "; Type2 " << pp(Type2) << "; " << std::endl;
        return false;
      }
    }
  }

  if (gsIsSortsPossible(result))
  {
    result=aterm_cast<sort_expression_list>(result[0]).front();
  }
  mCRL2log(debug) << "UnifyMinType: Type1 " << pp(Type1) << "; Type2 " << pp(Type2) << "; Res: " << pp(result) << "" << std::endl;
  return true;
}

bool mcrl2::data::data_expression_checker::MatchIf(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  assert(gsIsSortArrow(type));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  assert((Args.size()==3));
  Args=Args.tail();

  if (!UnifyMinType(Res,Args.front(),Res))
  {
    return false;
  }
  Args=Args.tail();
  if (!UnifyMinType(Res,Args.front(),Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(sort_bool::bool_(),Res,Res),Res);
  return true;
}

bool mcrl2::data::data_expression_checker::MatchEqNeqComparison(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));
  sort_expression Arg1=Args.front();
  Args=Args.tail();
  sort_expression Arg2=Args.front();

  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2,Arg))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg,Arg),sort_bool::bool_());
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListOpCons(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(UnwindType(Res))));
  Res=aterm_cast<sort_expression>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));
  sort_expression Arg1=Args.front();
  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (gsIsSortId(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=aterm_cast<sort_expression>(Arg2[1]);

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!UnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Res,static_cast<sort_expression>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListOpSnoc(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<sort_expression>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));
  sort_expression Arg1=Args.front();
  if (gsIsSortId(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<sort_expression>(Arg1[1]);

  Args=Args.tail();
  sort_expression Arg2=Args.front();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!UnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_list::list(sort_expression(Res))),Res),sort_list::list(sort_expression(Res)));
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListOpConcat(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<sort_expression>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));

  sort_expression Arg1=Args.front();
  if (gsIsSortId(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<sort_expression>(Arg1[1]);

  Args=Args.tail();

  sort_expression Arg2=Args.front();
  if (gsIsSortId(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  assert(sort_list::is_list(sort_expression(Arg2)));
  Arg2=aterm_cast<sort_expression>(Arg2[1]);

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!UnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_list::list(sort_expression(Res))),
       static_cast<sort_expression>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListOpEltAt(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));

  sort_expression Arg1=Args.front();
  if (gsIsSortId(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<sort_expression>(Arg1[1]);

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }
  Res=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_list::list(sort_expression(Res))),
               static_cast<sort_expression>(sort_nat::nat())),Res);
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListOpHead(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));
  sort_expression Arg=Args.front();
  if (gsIsSortId(Arg))
  {
    Arg=UnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<sort_expression>(Arg[1]);

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg,new_result))
  {
    return false;
  }
  Res=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_list::list(sort_expression(Res)))),Res);
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListOpTail(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<sort_expression>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));
  sort_expression Arg=Args.front();
  if (gsIsSortId(Arg))
  {
    Arg=UnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<sort_expression>(Arg[1]);

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg,new_result))
  {
    return false;
  }
  Res=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_list::list(sort_expression(Res)))),
                   sort_list::list(sort_expression(Res)));
  return true;
}

//Sets
bool mcrl2::data::data_expression_checker::MatchSetOpSet2Bag(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));

  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  assert(sort_bag::is_bag(sort_expression(Res)));
  Res=aterm_cast<sort_expression>(Res[1]);

  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));

  sort_expression Arg=Args.front();
  if (gsIsSortId(Arg))
  {
    Arg=UnwindType(Arg);
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<sort_expression>(Arg[1]);

  sort_expression new_result;
  if (!UnifyMinType(Arg,Res,new_result))
  {
    return false;
  }
  Arg=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_set::set_(sort_expression(Arg)))),
                  sort_bag::bag(sort_expression(Arg)));
  return true;
}

bool mcrl2::data::data_expression_checker::MatchListSetBagOpIn(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  //assert(gsIsBool(aterm_cast<sort_expression>(type[1])));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));

  sort_expression Arg1=Args.front();

  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (gsIsSortId(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  assert(gsIsSortCons(Arg2));
  sort_expression Arg2s=aterm_cast<sort_expression>(Arg2[1]);

  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2s,Arg))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg,Arg2.set_argument(Arg,1)),sort_bool::bool_());
  return true;
}

bool mcrl2::data::data_expression_checker::MatchSetBagOpUnionDiffIntersect(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  if (core::gstcIsNumericType(Res))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Res))||sort_bag::is_bag(sort_expression(Res)));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));

  sort_expression Arg1=Args.front();
  if (gsIsSortId(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (core::gstcIsNumericType(Arg1))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Arg1))||sort_bag::is_bag(sort_expression(Arg1)));

  Args=Args.tail();

  sort_expression Arg2=Args.front();
  if (gsIsSortId(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (core::gstcIsNumericType(Arg2))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Arg2))||sort_bag::is_bag(sort_expression(Arg2)));

  sort_expression temp_result;
  if (!UnifyMinType(Res,Arg1,temp_result))
  {
    return false;
  }

  if (!UnifyMinType(temp_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Res,Res),Res);
  return true;
}

bool mcrl2::data::data_expression_checker::MatchSetOpSetCompl(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  if (core::gstcIsNumericType(Res))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<sort_expression>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));

  sort_expression Arg=Args.front();
  if (gsIsSortId(Arg))
  {
    Arg=UnwindType(Arg);
  }
  if (core::gstcIsNumericType(Arg))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<sort_expression>(Arg[1]);

  sort_expression temp_result;
  if (!UnifyMinType(Res,Arg,temp_result))
  {
    return false;
  }
  Res=temp_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_set::set_(sort_expression(Res)))),sort_set::set_(sort_expression(Res)));
  return true;
}

//Bags
bool mcrl2::data::data_expression_checker::MatchBagOpBag2Set(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));

  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=UnwindType(Res);
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<sort_expression>(Res[1]);

  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));

  sort_expression Arg=Args.front();
  if (gsIsSortId(Arg))
  {
    Arg=UnwindType(Arg);
  }
  assert(sort_bag::is_bag(sort_expression(Arg)));
  Arg=aterm_cast<sort_expression>(Arg[1]);

  sort_expression temp_result;
  if (!UnifyMinType(Arg,Res,temp_result))
  {
    return false;
  }
  Arg=temp_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<sort_expression>(sort_bag::bag(sort_expression(Arg)))),sort_set::set_(sort_expression(Arg)));
  return true;
}

bool mcrl2::data::data_expression_checker::MatchBagOpBagCount(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  //If the second argument is not a Bag, don't match

  if (!gsIsSortArrow(type))
  {
    result=type;
    return true;
  }
  //assert(gsIsNat(aterm_cast<sort_expression>(type[1])));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  if (!(Args.size()==2))
  {
    result=type;
    return true;
  }

  sort_expression Arg1=Args.front();

  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (gsIsSortId(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!sort_bag::is_bag(sort_expression(Arg2)))
  {
    result=type;
    return true;
  }
  Arg2=aterm_cast<sort_expression>(Arg2[1]);

  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2,Arg))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg,static_cast<sort_expression>(sort_bag::bag(sort_expression(Arg)))),sort_nat::nat());
  return true;
}


bool mcrl2::data::data_expression_checker::MatchFuncUpdate(const sort_expression &type, sort_expression &result)
{
  //tries to sort out the types of FuncUpdate ((A->B)xAxB->(A->B))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==3));
  sort_expression Arg1=Args.front();
  assert(gsIsSortArrow(Arg1));
  Args=Args.tail();
  sort_expression Arg2=Args.front();
  Args=Args.tail();
  sort_expression Arg3=Args.front();
  sort_expression Res=aterm_cast<sort_expression>(type[1]);
  assert(gsIsSortArrow(Res));

  sort_expression temp_result;
  if (!UnifyMinType(Arg1,Res,temp_result))
  {
    return false;
  }
  Arg1=temp_result;

  // determine A and B from Arg1:
  aterm_list LA=aterm_cast<aterm_list>(Arg1[0]);
  assert((LA.size()==1));
  sort_expression A=LA.front();
  sort_expression B=aterm_cast<sort_expression>(Arg1[1]);

  if (!UnifyMinType(A,Arg2,temp_result))
  {
    return false;
  }
  if (!UnifyMinType(B,Arg3,temp_result))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg1,A,B),Arg1);
  return true;
}


bool mcrl2::data::data_expression_checker::MaximumType(const sort_expression &Type1, const sort_expression &Type2, sort_expression &result)
{
  // if Type1 is convertible into Type2 or vice versa, the most general
  // of these types are returned in result. If no conversion is possible false is returned
  // and result is not changed. Conversions only take place between numerical types
  if (EqTypesA(Type1,Type2))
  {
    result=Type1;
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(Type1)))
  {
    result=Type2;
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(Type2)))
  {
    result=Type1;
    return true;
  }
  if (EqTypesA(Type1,sort_real::real_()))
  {
    if (EqTypesA(Type2,sort_int::int_()))
    {
      result=Type1;
      return true;
    }
    if (EqTypesA(Type2,sort_nat::nat()))
    {
      result=Type1;
      return true;
    }
    if (EqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (EqTypesA(Type1,sort_int::int_()))
  {
    if (EqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_nat::nat()))
    {
      result=Type1;
      return true;
    }
    if (EqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (EqTypesA(Type1,sort_nat::nat()))
  {
    if (EqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_int::int_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (EqTypesA(Type1,sort_pos::pos()))
  {
    if (EqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_int::int_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_nat::nat()))
    {
      result=Type2;
      return true;
    }
    return false;
  }
  return false;
}

sort_expression mcrl2::data::data_expression_checker::ExpandNumTypesUp(sort_expression Type)
{
  //Expand Pos.. to possible bigger types.
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (EqTypesA(sort_pos::pos(),Type))
  {
    return multiple_possible_sorts(atermpp::make_list(sort_pos::pos(), sort_nat::nat(), sort_int::int_(),sort_real::real_()));
  }
  if (EqTypesA(sort_nat::nat(),Type))
  {
    return multiple_possible_sorts(atermpp::make_list(sort_nat::nat(), sort_int::int_(),sort_real::real_()));
  }
  if (EqTypesA(sort_int::int_(),Type))
  {
    return multiple_possible_sorts(atermpp::make_list(sort_int::int_(), sort_real::real_()));
  }
  if (gsIsSortId(Type))
  {
    return Type;
  }
  if (gsIsSortCons(Type))
  {
    return Type.set_argument(ExpandNumTypesUp(aterm_cast<aterm_appl>(Type[1])),1);
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
      NewTypeList.push_front(ExpandNumTypesUp(UnwindType(TypeList.front())));
    }
    aterm_appl ResultType=aterm_cast<aterm_appl>(Type[1]);
    if (!gsIsSortArrow(ResultType))
    {
      return Type.set_argument(reverse(NewTypeList),0);
    }
    else
    {
      return gsMakeSortArrow(reverse(NewTypeList),ExpandNumTypesUp(UnwindType(ResultType)));
    }
  }

  return Type;
}

sort_expression mcrl2::data::data_expression_checker::ExpandNumTypesDown(sort_expression Type)
{
  // Expand Numeric types down
  if (data::is_unknown_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (gsIsSortId(Type))
  {
    Type=UnwindType(Type);
  }

  bool function=false;
  aterm_list Args;
  if (gsIsSortArrow(Type))
  {
    function=true;
    Args=aterm_cast<aterm_list>(Type[0]);
    Type=aterm_cast<aterm_appl>(Type[1]);
  }

  if (EqTypesA(sort_real::real_(),Type))
  {
    Type=multiple_possible_sorts(atermpp::make_list(sort_pos::pos(),sort_nat::nat(),sort_int::int_(),sort_real::real_()));
  }
  if (EqTypesA(sort_int::int_(),Type))
  {
    Type=multiple_possible_sorts(atermpp::make_list(sort_pos::pos(),sort_nat::nat(),sort_int::int_()));
  }
  if (EqTypesA(sort_nat::nat(),Type))
  {
    Type=multiple_possible_sorts(atermpp::make_list(sort_pos::pos(),sort_nat::nat()));
  }

  return (function)?gsMakeSortArrow(Args,Type):Type;
}


bool mcrl2::data::data_expression_checker::InTypesA(sort_expression Type, sort_expression_list Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (EqTypesA(Type,Types.front()))
    {
      return true;
    }
  return false;
}

bool mcrl2::data::data_expression_checker::EqTypesA(sort_expression Type1, sort_expression Type2)
{
  if (Type1==Type2)
  {
    return true;
  }

  return UnwindType(Type1)==UnwindType(Type2);
}

bool mcrl2::data::data_expression_checker::InTypesL(sort_expression_list Type, term_list<sort_expression_list> Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (EqTypesL(Type,Types.front()))
    {
      return true;
    }
  return false;
}

bool mcrl2::data::data_expression_checker::EqTypesL(sort_expression_list Type1, sort_expression_list Type2)
{
  if (Type1==Type2)
  {
    return true;
  }
  if (Type1.size()!=Type2.size())
  {
    return false;
  }
  for (; !Type1.empty(); Type1=Type1.tail(),Type2=Type2.tail())
    if (!EqTypesA(Type1.front(),Type2.front()))
    {
      return false;
    }
  return true;
}


sort_expression mcrl2::data::data_expression_checker::TraverseVarConsTypeDN(
  const std::map<core::identifier_string,sort_expression> &DeclaredVars,
  const std::map<core::identifier_string,sort_expression> &AllowedVars,
  data_expression &DataTerm,
  sort_expression PosType,
  std::map<core::identifier_string,sort_expression> &FreeVars,
  const bool strictly_ambiguous,
  const size_t nFactPars,
  const bool warn_upcasting,
  const bool print_cast_error)
{
  // std::string::npos for nFactPars means the number of arguments is not known.
  mCRL2log(debug) << "TraverseVarConsTypeDN: DataTerm " << pp(DataTerm)
                  << " with PosType " << pp(PosType) << ", nFactPars " << nFactPars << "" << std::endl;
  if (gsIsId(DataTerm)||gsIsOpId(DataTerm))
  {
    core::identifier_string Name=aterm_cast<core::identifier_string>((DataTerm)[0]);
    bool variable_=false;
    bool TypeADefined=false;
    sort_expression TypeA;
    std::map<core::identifier_string,sort_expression>::const_iterator i=DeclaredVars.find(Name);
    if (i!=DeclaredVars.end())
    {
      TypeA=i->second;
      TypeADefined=true;
      const sort_expression Type1(UnwindType(TypeA));
      if (is_function_sort(Type1)?(function_sort(Type1).domain().size()==nFactPars):(nFactPars==0))
      {
        variable_=true;
        if (AllowedVars.count(Name)==0)
        {
          throw mcrl2::runtime_error("variable " + pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side");
        }

        //Add to free variables list
        FreeVars[Name]=TypeA;
      }
      else
      {
        TypeADefined=false;
      }
    }

    sort_expression_list ParList;
    if (nFactPars==0)
    {
      std::map<core::identifier_string,sort_expression>::const_iterator i=DeclaredVars.find(Name);
      if (i!=DeclaredVars.end()) 
      {
        TypeA=i->second;
        TypeADefined=true;
        sort_expression temp;
        if (!TypeMatchA(TypeA,PosType,temp))
        {
          throw mcrl2::runtime_error("the type " + pp(TypeA) + " of variable " + pp(Name)
                          + " is incompatible with " + pp(PosType) + " (typechecking " + pp(DataTerm) + ")");
        }
        DataTerm=gsMakeDataVarId(Name,TypeA);
        return TypeA;
      }
      else 
      {
        std::map<core::identifier_string,sort_expression>::const_iterator i=user_constants.find(Name);
        if (i!=user_constants.end())
        {
          TypeA=i->second;
          TypeADefined=true;
          sort_expression temp;
          if (!TypeMatchA(TypeA,PosType,temp))
          {
            throw mcrl2::runtime_error("the type " + pp(TypeA) + " of constant " + pp(Name)
                            + " is incompatible with " + pp(PosType) + " (typechecking " + pp(DataTerm) + ")");
          }
          DataTerm=gsMakeOpId(Name,TypeA);
          return TypeA;
        }
        else
        {
          std::map<core::identifier_string,sort_expression_list>::const_iterator j=system_constants.find(Name);
          
          if (j!=system_constants.end())
          {
            ParList=j->second; 
            if (ParList.size()==1)
            {
              sort_expression Type1=ParList.front();
              DataTerm=function_symbol(Name,Type1);
              return Type1;
            }
            else
            {
              DataTerm=gsMakeOpId(Name,data::unknown_sort());
              throw  mcrl2::runtime_error("ambiguous system constant " + pp(Name));
            }
          }
          else
          {
            throw mcrl2::runtime_error("unknown constant " + pp(Name));
          }
        }
      }
    }

    if (TypeADefined)
    {
      ParList=make_list<sort_expression>(UnwindType(TypeA));
    }
    else
    {
      const std::map <core::identifier_string,sort_expression_list>::const_iterator j_context=user_functions.find(Name);
      const std::map <core::identifier_string,sort_expression_list>::const_iterator j_gssystem=system_functions.find(Name);

      if (j_context==user_functions.end())   
      {
        if (j_gssystem!=system_functions.end())        
        { 
          ParList=j_gssystem->second; // The function only occurs in the system.
        }
        else // None are defined.
        {
          if (nFactPars!=std::string::npos)
          {
            throw mcrl2::runtime_error("unknown operation " + pp(Name) + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":""));
          }
          else
          {
            throw mcrl2::runtime_error("unknown operation " + pp(Name));
          }
        }
      }
      else if (j_gssystem==system_functions.end())          
      {
        ParList=j_context->second; // only the context sorts are defined.
      }
      else  // Both are defined.
      {
        ParList=j_gssystem->second+j_context->second;
      }
    }

    mCRL2log(debug) << "Possible types for Op/Var " << pp(Name) << " with " << nFactPars <<
                " argument are (ParList: " << pp(ParList) << "; PosType: " << pp(PosType) << ")" << std::endl;

    sort_expression_list CandidateParList=ParList;

    {
      // filter ParList keeping only functions A_0#...#A_nFactPars->A
      sort_expression_list NewParList;
      if (nFactPars!=std::string::npos)
      {
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          sort_expression Par=ParList.front();
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
      sort_expression_list BackupParList=ParList;
      NewParList=sort_expression_list();
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        sort_expression Par=ParList.front();
        sort_expression result;
        if (TypeMatchA(Par,PosType,result))
        {
          NewParList=aterm_cast<sort_expression_list>(core::ATinsertUnique(NewParList,result));
        }
      }
      NewParList=reverse(NewParList);

      mCRL2log(debug) << "Possible matches w/o casting for Op/Var " << pp(Name) << " with "<< nFactPars << 
                " argument are (ParList: " << pp(NewParList) << "; PosType: " << pp(PosType) << "" << std::endl;

      if (NewParList.empty())
      {
        //Ok, this looks like a type error. We are not that strict.
        //Pos can be Nat, or even Int...
        //So lets make PosType more liberal
        //We change every Pos to NotInferred(Pos,Nat,Int)...
        //and get the list. Then we take the min of the list.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying casting for Op " << pp(Name) << " with " << nFactPars << " argument (ParList: " << 
                            pp(ParList) << "; PosType: " << pp(PosType) << "" << std::endl;
        PosType=ExpandNumTypesUp(PosType);
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          sort_expression Par=ParList.front();
          sort_expression result;
          if (TypeMatchA(Par,PosType,result))
          {
            NewParList=aterm_cast<sort_expression_list>(core::ATinsertUnique(NewParList,result));
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is [1] " << pp(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=make_list<sort_expression>(core::gstcMinType(NewParList));
        }
      }

      if (NewParList.empty())
      {
        //Ok, casting of the arguments did not help.
        //Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying result casting for Op " << pp(Name) << " with " << nFactPars << " argument (ParList: " << 
                              "; PosType: " << pp(PosType) << "" << std::endl;
        PosType=ExpandNumTypesDown(ExpandNumTypesUp(PosType));
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          sort_expression Par=ParList.front();
          sort_expression result;
          if (TypeMatchA(Par,PosType,result))
          {
            NewParList=aterm_cast<sort_expression_list>(core::ATinsertUnique(NewParList,result));
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is [2]" << pp(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=make_list<sort_expression>(core::gstcMinType(NewParList));
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
        Sort=CandidateParList.front();
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
      }
      else
      {
        throw mcrl2::runtime_error("unknown operation/variable " + pp(Name) + " that matches type " + pp(PosType));
      }
    }

    if (ParList.size()==1)
    {
      // replace PossibleSorts by a single possibility.
      sort_expression Type=ParList.front();

      aterm_appl OldType=Type;
      bool result=true;
      assert(Type.defined());
      if (core::gstcHasUnknown(Type))
      {
        sort_expression new_type;
        result=TypeMatchA(Type,PosType,new_type);
        Type=new_type;
      }

      if (core::gstcHasUnknown(Type) && gsIsOpId(DataTerm))
      {
        sort_expression new_type;
        result=TypeMatchA(Type,aterm_cast<aterm_appl>((DataTerm)[1]),new_type);
        Type=new_type;
      }

      if (!result)
      {
        throw mcrl2::runtime_error("fail to match sort " + pp(OldType) + " with " + pp(PosType));
      }

      if (static_cast<aterm_appl>(data::detail::if_symbol())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing if matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchIf(Type,NewType))
        {
          throw mcrl2::runtime_error("the function if has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
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
        sort_expression NewType;
        if (!MatchEqNeqComparison(Type,NewType))
        {
          throw mcrl2::runtime_error("the function " + pp(aterm_cast<aterm_appl>((DataTerm)[0])) + " has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::cons_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing |> matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchListOpCons(Type,NewType))
        {
          throw mcrl2::runtime_error("the function |> has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::snoc_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing <| matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchListOpSnoc(Type,NewType))
        {
          throw mcrl2::runtime_error("the function <| has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::concat_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing ++ matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchListOpConcat(Type,NewType))
        {
          throw mcrl2::runtime_error("the function ++ has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::element_at_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing @ matching Type " << pp(Type) << ", PosType " << pp(PosType) << ", DataTerm: " << pp(DataTerm) << "" << std::endl;
        sort_expression NewType;
        if (!MatchListOpEltAt(Type,NewType))
        {
          throw mcrl2::runtime_error("the function @ has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::head_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_list::rhead_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {R,L}head matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;

        sort_expression NewType;
        if (!MatchListOpHead(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {R,L}head has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::tail_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_list::rtail_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {R,L}tail matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchListOpTail(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {R,L}tail has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::set2bag_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing Set2Bag matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchSetOpSet2Bag(Type,NewType))
        {
          throw mcrl2::runtime_error("the function Set2Bag has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::in_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {List,Set,Bag} matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchListSetBagOpIn(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {List,Set,Bag}In has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_set::union_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_set::difference_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_set::intersection_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {Set,Bag}{Union,Difference,Intersect} matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchSetBagOpUnionDiffIntersect(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_set::complement_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing SetCompl matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchSetOpSetCompl(Type,NewType))
        {
          throw mcrl2::runtime_error("the function SetCompl has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::bag2set_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing Bag2Set matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchBagOpBag2Set(Type,NewType))
        {
          throw mcrl2::runtime_error("the function Bag2Set has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::count_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing BagCount matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchBagOpBagCount(Type,NewType))
        {
          throw mcrl2::runtime_error("the function BagCount has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }


      if (static_cast<aterm_appl>(data::function_update_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing FuncUpdate matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        sort_expression NewType;
        if (!MatchFuncUpdate(Type,NewType))
        {
          throw mcrl2::runtime_error("function update has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }


      Type=core::replace_possible_sorts(Type); // Set the type to one option in possible sorts, if there are more options.
      DataTerm=gsMakeOpId(Name,Type);
      if (variable_)
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
        }
        else
        {
          throw mcrl2::runtime_error("ambiguous operation " + pp(Name));
        }
      }
      else
      {
        return data::unknown_sort();
      }
    }
  }
  else
  {
    return TraverseVarConsTypeD(DeclaredVars,AllowedVars,DataTerm,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
  }
}


void mcrl2::data::data_expression_checker::AddVars2Table(
                   std::map<core::identifier_string,sort_expression> &Vars, 
                   variable_list VarDecls, 
                   std::map<core::identifier_string,sort_expression> &result)
{
  for (variable_list::const_iterator i=VarDecls.begin(); i!=VarDecls.end(); ++i)
  {
    variable VarDecl= *i;
    core::identifier_string VarName=VarDecl.name();
    sort_expression VarType=VarDecl.sort();
    //test the type
    IsSortExprDeclared(VarType);

    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    Vars[VarName]=VarType;
  }
  result=Vars;
}


sort_expression mcrl2::data::data_expression_checker::TraverseVarConsTypeD(
  const std::map<core::identifier_string,sort_expression> &DeclaredVars,
  const std::map<core::identifier_string,sort_expression> &AllowedVars,
  data_expression &DataTerm,
  sort_expression PosType,
  std::map<core::identifier_string,sort_expression> &FreeVars,
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

  mCRL2log(debug) << "TraverseVarConsTypeD: DataTerm " << pp(DataTerm) <<
              " with PosType " << pp(PosType) << "" << std::endl;

  if (is_abstraction(DataTerm))
  {
    const abstraction abstr=aterm_cast<abstraction>(DataTerm);
    //The variable declaration of a binder should have at least 1 declaration
    if (abstr.variables().size()==0)
    {
      throw mcrl2::runtime_error("binder " + pp(DataTerm) + " should have at least one declared variable");
    }

    binder_type BindingOperator = abstr.binding_operator();
    std::map<core::identifier_string,sort_expression> CopyAllowedVars(AllowedVars);
    std::map<core::identifier_string,sort_expression> CopyDeclaredVars(DeclaredVars);

    if (gsIsSetBagComp(BindingOperator) ||
        gsIsSetComp(BindingOperator) ||
        gsIsBagComp(BindingOperator))
    {
      variable_list VarDecls=abstr.variables();

      //A Set/bag comprehension should have exactly one variable declared
      if (VarDecls.size()!=1)
      {
        throw mcrl2::runtime_error("set/bag comprehension " + pp(DataTerm) + " should have exactly one declared variable");
      }

      variable VarDecl=VarDecls.front();
      sort_expression NewType=VarDecl.sort();
      variable_list VarList=make_list(VarDecl);
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      AddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      AddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
      
      data_expression Data((DataTerm)[2]);

      sort_expression ResType;
      try
      {
        ResType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nthe condition or count of a set/bag comprehension " + pp(DataTerm) + " cannot be determined");
      }
      sort_expression temp;
      if (TypeMatchA(sort_bool::bool_(),ResType,temp))
      {
        NewType=sort_set::set_(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeSetComp(), 0);
      }
      else if (TypeMatchA(sort_nat::nat(),ResType,temp))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeBagComp(), 0);
      }
      else if (TypeMatchA(sort_pos::pos(),ResType,temp))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeBagComp(), 0);
        Data=make_application(sort_nat::cnat(),Data);
      }
      else
      {
        throw mcrl2::runtime_error("the condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " + pp(ResType));
      }

      if (!TypeMatchA(NewType,PosType,NewType))
      {
        throw mcrl2::runtime_error("a set or bag comprehension of type " + pp(VarDecl.sort()) + " does not match possible type " +
                            pp(PosType) + " (while typechecking " + pp(DataTerm) + ")");
      }

      // if (FreeVars)
      {
        core::gstcRemoveVars(FreeVars,VarList);
      }
      DataTerm=DataTerm.set_argument(Data,2);
      return NewType;
    }

    if (gsIsForall(BindingOperator) || gsIsExists(BindingOperator))
    {
      variable_list VarList=abstr.variables();
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      AddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      AddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);

      data_expression Data=abstr.body();
      sort_expression temp;
      if (!TypeMatchA(sort_bool::bool_(),PosType,temp))
      {
        throw mcrl2::runtime_error("the type of an exist/forall for " + pp(DataTerm) + " cannot be determined");
      }
      sort_expression NewType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,sort_bool::bool_(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      if (!TypeMatchA(sort_bool::bool_(),NewType,temp))
      {
        throw mcrl2::runtime_error("the type of an exist/forall for " + pp(DataTerm) + " cannot be determined");
      }

      // if (FreeVars)
      {
        core::gstcRemoveVars(FreeVars,VarList);
      }
      DataTerm=DataTerm.set_argument(Data,2);
      return sort_bool::bool_();
    }

    if (gsIsLambda(BindingOperator))
    {
      variable_list VarList=abstr.variables();
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      AddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      AddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
      
      sort_expression_list ArgTypes=core::gstcGetVarTypes(VarList);
      sort_expression NewType;
      if (!UnArrowProd(ArgTypes,PosType,NewType))
      {
        throw mcrl2::runtime_error("no functions with arguments " + pp(ArgTypes) + " among " + pp(PosType) + " (while typechecking " + pp(DataTerm) + ")");
      }
      data_expression Data=abstr.body();

      try 
      {
        NewType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,NewType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error &e)
      {
        core::gstcRemoveVars(FreeVars,VarList);
        throw e;
      }
      mCRL2log(debug) << "Result of TraverseVarConsTypeD: DataTerm " << pp(Data) << "" << std::endl;

      core::gstcRemoveVars(FreeVars,VarList);
      
      DataTerm=DataTerm.set_argument(Data,2);
      return gsMakeSortArrow(ArgTypes,NewType);
    }
  }

  if (is_where_clause(DataTerm))
  {
    const where_clause &where=aterm_cast<const where_clause>(DataTerm);
    variable_list WhereVarList;
    aterm_list NewWhereList;
    const assignment_expression_list& where_asss=where.declarations();
    for (assignment_expression_list::const_iterator i=where_asss.begin(); i!=where_asss.end(); ++i)
// aterm_list WhereList=aterm_cast<aterm_list>((DataTerm)[1]); !WhereList.empty(); WhereList=WhereList.tail())
    {
      const assignment_expression WhereElem= *i;
      data_expression WhereTerm=WhereElem[1];
      sort_expression WhereType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,WhereTerm,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      variable NewWhereVar;
      if (gsIsDataVarId(aterm_cast<aterm_appl>(WhereElem[0])))
      {
        // The variable in WhereElem is type checked, and a proper variable.
        NewWhereVar=WhereElem[0];
      }
      else
      {
        // The variable in WhereElem is just a string and needs to be transformed to a DataVarId.
        NewWhereVar=variable(aterm_cast<core::identifier_string>(WhereElem[0]),WhereType);
      }
      WhereVarList.push_front(NewWhereVar);
      NewWhereList.push_front(gsMakeDataVarIdInit(NewWhereVar,WhereTerm));
    }
    NewWhereList=reverse(NewWhereList);

    std::map<core::identifier_string,sort_expression> CopyAllowedVars(AllowedVars);
    std::map<core::identifier_string,sort_expression> CopyDeclaredVars(DeclaredVars);

    variable_list VarList=reverse(WhereVarList);
    std::map<core::identifier_string,sort_expression> NewAllowedVars;
    AddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
    
    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    AddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
    
    data_expression Data=aterm_cast<aterm_appl>((DataTerm)[0]);
    sort_expression NewType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

    core::gstcRemoveVars(FreeVars,VarList);
    
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
        sort_expression Type;
        if (!UnList(PosType,Type))
        {
          throw mcrl2::runtime_error("not possible to cast list to " + pp(PosType) + " (while typechecking " + pp(data_expression_list(appl.begin(),appl.end())) + ")");
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only!
        data_expression_list NewArguments;
        bool Type_is_stable=true;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i; 
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,false);
          }
          catch (mcrl2::runtime_error &e)
          {
            // Try again, but now without Type as the suggestion.
            // If this does not work, it will be caught in the second pass below.
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
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
            sort_expression Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
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
        sort_expression Type;
        if (!UnSet(PosType,Type))
        {
          throw mcrl2::runtime_error("not possible to cast set to " + pp(PosType) + " (while typechecking " + pp(data_expression_list(appl.begin(),appl.end())) + ")");
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only (which will be NewType)!
        bool NewTypeDefined=false;
        aterm_appl NewType;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument) + ")");
          }

          aterm_appl OldNewType=NewType;
          if (!NewTypeDefined)
          {
            NewType=Type0;
            NewTypeDefined=true;
          }
          else
          {
            sort_expression temp;
            if (!MaximumType(NewType,Type0,temp))
            {
              throw mcrl2::runtime_error("Set contains incompatible elements of sorts " + pp(OldNewType) + " and " + pp(Type0) + " (while typechecking " + pp(Argument));
            }
            NewType=temp;
            NewTypeDefined=true;
          }
        }

        // Type is now the most generic type to be used.
        assert(Type.defined());
        assert(NewTypeDefined);
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        aterm_list NewArguments;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument) + ")");
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
        sort_expression Type;
        if (!UnBag(PosType,Type))
        {
          throw mcrl2::runtime_error("not possible to cast bag to " + pp(PosType) + "(while typechecking " + 
                                      pp(data_expression_list(appl.begin(),appl.end())) + ")");
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only!
        aterm_appl NewType;
        bool NewTypeDefined=false;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          sort_expression Type0;
          try 
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument0) + ")");
          }
          sort_expression Type1;
          try 
          { 
            Type1=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast number to " + pp(sort_nat::nat()) + " (while typechecking " + pp(Argument1) + ")");
            }
            else
            {
              throw e;
            }
          }
          aterm_appl OldNewType=NewType;
          if (!NewTypeDefined)
          {
            NewType=Type0;
            NewTypeDefined=true;
          }
          else
          {
            sort_expression temp;
            if (!MaximumType(NewType,Type0,temp))
            {
              throw mcrl2::runtime_error("Bag contains incompatible elements of sorts " + pp(OldNewType) + " and " + pp(Type0) + " (while typechecking " + pp(Argument0) + ")");
            }

            NewType=temp;
            NewTypeDefined=true;
          }
        }
        assert(Type.defined());
        assert(NewTypeDefined);
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        aterm_list NewArguments;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          sort_expression Type0;
          try 
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument0) + ")");
            }
            else 
            {
              throw e;
            }
          }
          sort_expression Type1;
          try
          { 
            Type1=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast number to " + pp(sort_nat::nat()) + " (while typechecking " + pp(Argument1) + ")");
            }
            else
            {
              throw e;
            }
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
    sort_expression_list NewArgumentTypes;
    data_expression_list NewArguments;

    for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
    {
      data_expression Arg= *i;
      sort_expression Type=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,data::unknown_sort(),FreeVars,false,warn_upcasting,print_cast_error);
      assert(Type.defined());
      NewArguments.push_front(Arg);
      NewArgumentTypes.push_front(Type);
    }
    data_expression_list Arguments=reverse(NewArguments);
    sort_expression_list ArgumentTypes=reverse(NewArgumentTypes);

    //function
    data_expression Data=appl.head();
    sort_expression NewType;
    try
    {
      NewType=TraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                      Data,
                      data::unknown_sort() /* gsMakeSortArrow(ArgumentTypes,PosType) */,
                      FreeVars,false,nArguments,warn_upcasting,print_cast_error);
      mCRL2log(debug) << "Result of TraverseVarConsTypeD: DataTerm " << pp(Data) << "" << std::endl;
    }
    catch (mcrl2::runtime_error &e)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else if (gsIsOpId(Data)||gsIsDataVarId(Data))
      {
        ErrorMsgCannotCast(aterm_cast<aterm_appl>(Data[1]),Arguments,ArgumentTypes,e.what());
      }
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while trying to cast " + 
                            pp(application(Data,aterm_cast<data_expression_list>(Arguments))) + " to type " + pp(PosType));
    }

    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.


    if (is_function_sort(UnwindType(NewType)))
    {
      aterm_list NeededArgumentTypes=aterm_cast<aterm_list>(UnwindType(NewType)[0]);

      if (NeededArgumentTypes.size()!=Arguments.size())
      {
         throw mcrl2::runtime_error("need argumens of sorts " + pp(NeededArgumentTypes) +
                         " which does not match the number of provided arguments "
                            + pp(Arguments) + " (while typechecking "
                            + pp(DataTerm) + ")");
      }
      //arguments again
      sort_expression_list NewArgumentTypes;
      data_expression_list NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        assert(!Arguments.empty());
        assert(!NeededArgumentTypes.empty());
        data_expression Arg=Arguments.front();
        sort_expression NeededType=NeededArgumentTypes.front();
        sort_expression Type=ArgumentTypes.front();
        if (!EqTypesA(NeededType,Type))
        {
          //upcasting
          try 
          { 
            Type=UpCastNumericType(NeededType,Type,Arg,warn_upcasting);
          }
          catch (mcrl2::runtime_error &e)
          {
          }
        }
        if (!EqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on [1] " << pp(Arg) << ", Type: " << pp(Type) << ", Needed type: " << pp(NeededType) << "" << std::endl;
          sort_expression NewArgType;
          if (!TypeMatchA(NeededType,Type,NewArgType))
          {
            if (!TypeMatchA(NeededType,ExpandNumTypesUp(Type),NewArgType))
            {
              NewArgType=NeededType;
            }
          }
          try 
          {
            NewArgType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nneeded type " + pp(NeededType) + " does not match possible type "
                            + pp(Type) + " (while typechecking " + pp(Arg) + " in " + pp(DataTerm) + ")");
          }
          mCRL2log(debug) << "Result of Doing again TraverseVarConsTypeD: DataTerm " << pp(Arg) << "" << std::endl;
          Type=NewArgType;
        }
        NewArguments.push_front(Arg);
        NewArgumentTypes.push_front(Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    //the function again
    try 
    {
      NewType=TraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                        Data,gsMakeSortArrow(ArgumentTypes,PosType),
                                        FreeVars,strictly_ambiguous,nArguments,warn_upcasting,print_cast_error);

      mCRL2log(debug) << "Result of TraverseVarConsTypeDN: DataTerm " << pp(Data) << "" << std::endl;
    }
    catch (mcrl2::runtime_error &e)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else
      {
        ErrorMsgCannotCast(aterm_cast<aterm_appl>(Data[1]),Arguments,ArgumentTypes,e.what());
      }
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while trying to cast " + 
                   pp(application(Data,aterm_cast<data_expression_list>(Arguments))) + " to type " + pp(PosType));
    }

    mCRL2log(debug) << "Arguments once more: Arguments " << pp(Arguments) << ", ArgumentTypes: " <<
                pp(ArgumentTypes) << ", NewType: " << pp(NewType) << "" << std::endl;

    //and the arguments once more
    if (gsIsSortArrow(UnwindType(NewType)))
    {
      sort_expression_list NeededArgumentTypes=aterm_cast<sort_expression_list>(UnwindType(NewType)[0]);
      sort_expression_list NewArgumentTypes;
      data_expression_list NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        data_expression Arg=Arguments.front();
        sort_expression NeededType=NeededArgumentTypes.front();
        sort_expression Type=ArgumentTypes.front();

        if (!EqTypesA(NeededType,Type))
        {
          //upcasting
          try 
          {
            Type=UpCastNumericType(NeededType,Type,Arg,warn_upcasting);
          }
          catch (mcrl2::runtime_error &e)
          {
          }
        }
        if (!EqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on [2] " << pp(Arg) << ", Type: " << pp(Type) << ", Needed type: " << pp(NeededType) << "" << std::endl;
          sort_expression NewArgType;
          if (!TypeMatchA(NeededType,Type,NewArgType))
          {
            if (!TypeMatchA(NeededType,ExpandNumTypesUp(Type),NewArgType))
            {
              NewArgType=NeededType;
            }
          }
          try 
          {
            NewArgType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nneeded type " + pp(NeededType) + " does not match possible type "
                            + pp(Type) + " (while typechecking " + pp(Arg) + " in " + pp(DataTerm) + ")");
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

    if (gsIsSortArrow(UnwindType(NewType)))
    {
      return aterm_cast<aterm_appl>(UnwindType(NewType)[1]);
    }

    sort_expression temp_type;
    if (!UnArrowProd(ArgumentTypes,NewType,temp_type))
    {
      throw mcrl2::runtime_error("Fail to properly type " + pp(DataTerm));
    }
    if (core::gstcHasUnknown(temp_type))
    {
      throw mcrl2::runtime_error("Fail to properly type " + pp(DataTerm));
    }
    return temp_type;
  }

  if (gsIsId(DataTerm)||gsIsOpId(DataTerm)||gsIsDataVarId(DataTerm))
  {
    core::identifier_string Name=aterm_cast<core::identifier_string>((DataTerm)[0]);
    if (gsIsNumericString(gsATermAppl2String(Name)))
    {
      aterm_appl Sort=sort_int::int_();
      if (core::gstcIsPos(Name))
      {
        Sort=sort_pos::pos();
      }
      else if (core::gstcIsNat(Name))
      {
        Sort=sort_nat::nat();
      }
      DataTerm=gsMakeOpId(Name,Sort);

      sort_expression temp;
      if (TypeMatchA(Sort,PosType,temp))
      {
        return Sort;
      }

      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(PosType,Sort,DataTerm,warn_upcasting);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast number " + pp(DataTerm) + " to type " + pp(PosType));
      }
      return CastedNewType;
    }

    std::map<core::identifier_string,sort_expression>::const_iterator it=DeclaredVars.find(Name);
    if (it!=DeclaredVars.end())
    {
      sort_expression Type=it->second;
      mCRL2log(debug) << "Recognised declared variable " << pp(Name) << ", Type: " << pp(Type) << "" << std::endl;
      DataTerm=gsMakeDataVarId(Name,Type);

      if (AllowedVars.count(Name)==0)
      {
        throw mcrl2::runtime_error("variable " + pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side");
      }

      sort_expression NewType;
      if (TypeMatchA(Type,PosType,NewType))
      {
        Type=NewType;
      }
      else
      {
        //upcasting
        aterm_appl CastedNewType;
        try 
        {
          CastedNewType=UpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        }
        catch (mcrl2::runtime_error &e)
        {
          if (print_cast_error)
          { 
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast variable " + pp(DataTerm) + " to type " + pp(PosType));
          }
          else
          {
            throw e;
          }
        }

        Type=CastedNewType;
      }

      //Add to free variables list
      FreeVars[Name]=Type;
      return Type;
    }

    std::map<core::identifier_string,sort_expression>::const_iterator i=user_constants.find(Name);
    if (i!=user_constants.end())
    {
      sort_expression Type=i->second;
      sort_expression NewType;
      if (TypeMatchA(Type,PosType,NewType))
      {
        Type=NewType;
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        // The type cannot be unified. Try upcasting the type.
        DataTerm=gsMakeOpId(Name,Type);
        try
        {
          return UpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nno constant " + pp(DataTerm) + " with type " + pp(PosType));
        }
      }
    }

    std::map<core::identifier_string,sort_expression_list>::const_iterator j=system_constants.find(Name);
    if (j!=system_constants.end())
    {
      sort_expression_list ParList=j->second;
      sort_expression_list NewParList;
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        sort_expression Par=ParList.front();
        sort_expression result;
        if (TypeMatchA(Par,PosType,result))
        {
          NewParList.push_front(result);
        }
      }
      ParList=reverse(NewParList);
      if (ParList.empty())
      {
        throw mcrl2::runtime_error("no system constant " + pp(DataTerm) + " with type " + pp(PosType));
      }

      if (ParList.size()==1)
      {
        sort_expression Type=ParList.front();
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        DataTerm=gsMakeOpId(Name,data::unknown_sort());
        return data::unknown_sort();
      }
    }

    const std::map <core::identifier_string,sort_expression_list>::const_iterator j_context=user_functions.find(Name);
    const std::map <core::identifier_string,sort_expression_list>::const_iterator j_gssystem=system_functions.find(Name);

    sort_expression_list ParList;
    if (j_context==user_functions.end())   
    {
      if (j_gssystem!=system_functions.end())        
      { 
        ParList=j_gssystem->second; // The function only occurs in the system.
      }
      else // None are defined.
      {
        throw mcrl2::runtime_error("unknown operation " + pp(Name));
      }
    }
    else if (j_gssystem==system_functions.end())          
    {
      ParList=j_context->second; // only the context sorts are defined.
    }
    else  // Both are defined.
    {
      ParList=j_gssystem->second+j_context->second;
    }


    if (ParList.size()==1)
    {
      sort_expression Type=ParList.front();
      DataTerm=gsMakeOpId(Name,Type);
      try
      {
        return UpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nno constant " + pp(DataTerm) + " with type " + pp(PosType));
      }
    }
    else
    {
      return TraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strictly_ambiguous, std::string::npos, warn_upcasting,print_cast_error);
    }
  }

  throw mcrl2::runtime_error("Internal type checking error: " + pp(DataTerm) + " does not match any type checking case." );
}


std::map < data::sort_expression, data::basic_sort > mcrl2::data::data_expression_checker::construct_normalised_aliases()
{
  // This function does the same as data_specification::reconstruct_m_normalised_aliases().
  // Therefore, it should be replaced by that function, after restructuring the type checker.
  // First reset the normalised aliases and the mappings and constructors that have been
  // inherited to basic sort aliases during a previous round of sort normalisation.
  std::map < data::sort_expression, data::basic_sort > normalised_aliases;

  // Fill normalised_aliases. Simple aliases are stored from left to
  // right. If the right hand side is non trivial (struct, list, set or bag)
  // the alias is stored from right to left.
  for (std::map<core::identifier_string,sort_expression>::const_iterator sort_walker=defined_sorts.begin();  
               sort_walker!=defined_sorts.end(); ++sort_walker)
  {
    const core::identifier_string sort_name(sort_walker->first);
    const data::basic_sort first(sort_name);
    const data::sort_expression second(sort_walker->second);
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


void mcrl2::data::data_expression_checker::check_for_empty_constructor_domains(function_symbol_list constructor_list)
{
  // First add the constructors for structured sorts to the constructor list;
  try
  {
    std::map < sort_expression, basic_sort > normalised_aliases=construct_normalised_aliases();
    std::set< sort_expression > all_sorts;
    for (std::map<core::identifier_string,sort_expression>::const_iterator i=defined_sorts.begin();
              i!=defined_sorts.end(); ++i)
    {
      // const basic_sort s(core::identifier_string(gstcUnwindType(i->second)));
      const sort_expression reference=i->second;
      // if (is_container_sort(i->first) || is_function_sort(i->first))
      find_sort_expressions<sort_expression>(reference, std::inserter(all_sorts, all_sorts.end()));
    }

    for (std::set< sort_expression > ::const_iterator i=all_sorts.begin(); i!=all_sorts.end(); ++i)
    {
      if (is_structured_sort(*i))
      {
        const function_symbol_vector r=structured_sort(*i).constructor_functions();
        for (function_symbol_vector::const_iterator j=r.begin(); j!=r.end(); ++j)
        {
          constructor_list.push_front(*j);
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


void mcrl2::data::data_expression_checker::ReadInFuncs(const function_symbol_vector &Cons, const function_symbol_vector &Maps)
{
  mCRL2log(debug) << "Start Read-in Func" << std::endl;

  size_t constr_number=Cons.size();
  function_symbol_vector functions_and_constructors=Cons;
  functions_and_constructors.insert(functions_and_constructors.end(),Maps.begin(),Maps.end());
  for (function_symbol_vector::const_iterator i=functions_and_constructors.begin(); i!=functions_and_constructors.end(); ++i)
  {
    const function_symbol Func= *i;
    const core::identifier_string FuncName=Func.name();
    sort_expression FuncType=Func.sort();

    IsSortExprDeclared(FuncType);

    //if FuncType is a defined function sort, unwind it
    if (is_basic_sort(FuncType))
    {
      const sort_expression NewFuncType=UnwindType(FuncType);
      if (is_function_sort(NewFuncType))
      {
        FuncType=NewFuncType;
      }
    }

    if (is_function_sort(FuncType))
    {
      AddFunction(gsMakeOpId(FuncName,FuncType),"function");
    }
    else
    {
      try 
      {
        AddConstant(gsMakeOpId(FuncName,FuncType),"constant");
      }
      catch (mcrl2::runtime_error &e)  
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not add constant");
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
      ConstructorType=UnwindType(ConstructorType);
      if (!gsIsSortId(ConstructorType) ||
          sort_bool::is_bool(sort_expression(ConstructorType)) ||
          sort_pos::is_pos(sort_expression(ConstructorType)) ||
          sort_nat::is_nat(sort_expression(ConstructorType)) ||
          sort_int::is_int(sort_expression(ConstructorType)) ||
          sort_real::is_real(sort_expression(ConstructorType))
          )
      {
        throw mcrl2::runtime_error("Could not add constructor " + pp(FuncName) + " of sort " + pp(FuncType) + ". Constructors of built-in sorts are not allowed.");
      }
    }

    mCRL2log(debug) << "Read-in Func " << pp(FuncName) << ", Types " << pp(FuncType) << "" << std::endl;
  }

  // Check that the constructors are defined such that they cannot generate an empty sort.
  // E.g. in the specification sort D; cons f:D->D; the sort D must be necessarily empty, which is
  // forbidden. The function below checks whether such malicious specifications occur.

  check_for_empty_constructor_domains(function_symbol_list(Cons.begin(),Cons.end())); // throws exception if not ok.
}

void mcrl2::data::data_expression_checker::AddConstant(const data::function_symbol &f, const std::string msg)
{
  core::identifier_string Name = f.name();
  sort_expression Sort = f.sort();

  if (user_constants.count(Name)>0)
  {
    throw mcrl2::runtime_error("double declaration of " + msg + " " + pp(Name));
  }

  if (system_constants.count(Name)>0 || system_functions.count(Name)>0)
  {
    throw mcrl2::runtime_error("attempt to declare a constant with the name that is a built-in identifier (" + pp(Name) + ")");
  }

  user_constants[Name]=Sort;
}


bool mcrl2::data::data_expression_checker::TypeMatchL(
                     const sort_expression_list &TypeList, 
                     const sort_expression_list &PosTypeList, 
                     sort_expression_list &result)
{
  mCRL2log(debug) << "gstcTypeMatchL TypeList: " << pp(TypeList) << ";    PosTypeList: " <<
              pp(PosTypeList) << "" << std::endl;

  if (TypeList.size()!=PosTypeList.size())
  {
    return false;
  }

  sort_expression_list Result;
  sort_expression_list::const_iterator j=PosTypeList.begin();
  for (sort_expression_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i, ++j)
  {
    sort_expression Type;
    if (!TypeMatchA(*i,*j,Type))
    {
      return false;
    }
    Result.push_front(Type);
  }
  result=reverse(Result);
  return true;
}


sort_expression mcrl2::data::data_expression_checker::UnwindType(const sort_expression &Type)
{
  if (is_container_sort(Type))
  {
    const container_sort &cs=aterm_cast<const container_sort>(Type);
    return container_sort(cs.container_name(),UnwindType(cs.element_sort()));
  }
  if (is_function_sort(Type))
  {
    const function_sort &fs=aterm_cast<function_sort>(Type);
    aterm_list NewArgs;
    for (sort_expression_list::const_iterator i=fs.domain().begin(); i!=fs.domain().end(); ++i)
    {
      NewArgs.push_front(UnwindType(*i));
    }
    NewArgs=reverse(NewArgs);
    return function_sort(NewArgs,UnwindType(fs.codomain()));
  }

  if (is_basic_sort(Type))
  {
    const basic_sort &bs=aterm_cast<const basic_sort>(Type);
    std::map<core::identifier_string,sort_expression>::const_iterator i=defined_sorts.find(bs.name());
    if (i==defined_sorts.end())
    {
      return Type;
    }
    return UnwindType(i->second);
  }

  return Type;
}

bool mcrl2::data::data_expression_checker::TypeMatchA(
                 const sort_expression &Type_in, 
                 const sort_expression &PosType_in, 
                 sort_expression &result)
{
  // Checks if Type and PosType match by instantiating unknown sorts.
  // It returns the matching instantiation of Type in result. If matching fails,
  // it returns false, otherwise true.

  sort_expression Type=Type_in;
  sort_expression PosType=PosType_in;

  mCRL2log(debug) << "TypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) << " " << std::endl;

  if (data::is_unknown_sort(Type))
  {
    result=PosType;
    return true;
  }
  if (data::is_unknown_sort(PosType) || EqTypesA(Type,PosType))
  {
    result=Type;
    return true;
  }
  if (is_multiple_possible_sorts(Type) && !is_multiple_possible_sorts(PosType))
  {
    PosType.swap(Type);
  }
  if (is_multiple_possible_sorts(PosType))
  {
    sort_expression_list NewTypeList;
    const multiple_possible_sorts &mps=aterm_cast<const multiple_possible_sorts>(PosType);
    for (sort_expression_list::const_iterator i=mps.sorts().begin(); i!=mps.sorts().end(); ++i) 
    {
      sort_expression NewPosType= *i;
      mCRL2log(debug) << "Matching candidate TypeMatchA Type: " << pp(Type) << ";    PosType: "
                  << pp(PosType) << " New Type: " << pp(NewPosType) << "" << std::endl;

      sort_expression new_type;
      if (TypeMatchA(Type,NewPosType,new_type))
      {
        NewPosType=new_type;
        mCRL2log(debug) << "Match TypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) <<
                    " New Type: " << pp(NewPosType) << "" << std::endl;
        NewTypeList.push_front(NewPosType);
      }
    }
    if (NewTypeList.empty())
    {
      mCRL2log(debug) << "No match TypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) << " " << std::endl;
      return false;
    }

    if (NewTypeList.tail().empty())
    {
      result=NewTypeList.front();
      return true;
    }

    result=multiple_possible_sorts(sort_expression_list(reverse(NewTypeList)));
    return true;
  }

  if (is_basic_sort(Type))
  {
    Type=UnwindType(Type);
  }
  if (gsIsSortId(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (gsIsSortCons(Type))
  {
    aterm_appl ConsType = aterm_cast<aterm_appl>(Type[0]);
    if (gsIsSortList(ConsType))
    {
      if (!sort_list::is_list(sort_expression(PosType)))
      {
        return false;
      }
      sort_expression Res;
      if (!TypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),Res))
      {
        return false;
      }
      result=sort_list::list(sort_expression(Res));
      return true;
    }

    if (gsIsSortSet(ConsType))
    {
      if (!sort_set::is_set(sort_expression(PosType)))
      {
        return false;
      }
      else
      {
        sort_expression Res;
        if (!TypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),Res))
        {
          return false;
        }
        result=sort_set::set_(sort_expression(Res));
        return true;
      }
    }

    if (gsIsSortBag(ConsType))
    {
      if (!sort_bag::is_bag(sort_expression(PosType)))
      {
        return false;
      }
      else
      {
        sort_expression Res;
        if (!TypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),Res))
        {
          return false;
        }
        result=sort_bag::bag(sort_expression(Res));
        return true;
      }
    }
  }

  if (is_function_sort(Type))
  {
    if (!is_function_sort(PosType))
    {
      return false;
    }
    else
    {
      const function_sort fs=aterm_cast<const function_sort>(Type);
      const function_sort posfs=aterm_cast<const function_sort>(PosType);
      sort_expression_list ArgTypes;
      if (!TypeMatchL(fs.domain(),posfs.domain(),ArgTypes))
      {
        return false;
      }
      sort_expression ResType;
      if (!TypeMatchA(fs.codomain(),posfs.codomain(),ResType))
      {
        return false;
      }
      result=function_sort(ArgTypes,ResType);
      return true;
    }
  }

  return false;
}


void mcrl2::data::data_expression_checker::AddSystemConstant(const data::function_symbol &f)
{
  // append the Type to the entry of the Name of the OpId in system constants table

  const core::identifier_string &OpIdName = f.name();
  const sort_expression &Type = f.sort();

  std::map<core::identifier_string,sort_expression_list>::const_iterator i=system_constants.find(OpIdName);

  sort_expression_list Types;
  if (i!=system_constants.end())
  { 
    Types=i->second;
  }
  Types=push_back(Types,Type);
  system_constants[OpIdName]=Types;
}

void mcrl2::data::data_expression_checker::AddSystemFunction(const data::function_symbol &f)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  const core::identifier_string &OpIdName = f.name();
  const sort_expression & Type = f.sort();
  assert(is_function_sort(Type));

  const std::map <core::identifier_string,sort_expression_list>::const_iterator j=system_functions.find(OpIdName);

  sort_expression_list Types;
  if (j!=system_functions.end())
  {
    Types=j->second;
  }
  Types=Types+make_list<sort_expression>(Type);  // TODO: Avoid concatenate but the order is essential.
  system_functions[OpIdName]=Types;
}


void mcrl2::data::data_expression_checker::initialise_system_defined_functions(void)
{
  //Creation of operation identifiers for system defined operations.
  //Bool
  AddSystemConstant(sort_bool::true_());
  AddSystemConstant(sort_bool::false_());
  AddSystemFunction(sort_bool::not_());
  AddSystemFunction(sort_bool::and_());
  AddSystemFunction(sort_bool::or_());
  AddSystemFunction(sort_bool::implies());
  AddSystemFunction(equal_to(data::unknown_sort()));
  AddSystemFunction(not_equal_to(data::unknown_sort()));
  AddSystemFunction(if_(data::unknown_sort()));
  AddSystemFunction(less(data::unknown_sort()));
  AddSystemFunction(less_equal(data::unknown_sort()));
  AddSystemFunction(greater_equal(data::unknown_sort()));
  AddSystemFunction(greater(data::unknown_sort()));
  //Numbers
  AddSystemFunction(sort_nat::pos2nat());
  AddSystemFunction(sort_nat::cnat());
  AddSystemFunction(sort_int::pos2int());
  AddSystemFunction(sort_real::pos2real());
  AddSystemFunction(sort_nat::nat2pos());
  AddSystemFunction(sort_int::nat2int());
  AddSystemFunction(sort_int::cint());
  AddSystemFunction(sort_real::nat2real());
  AddSystemFunction(sort_int::int2pos());
  AddSystemFunction(sort_int::int2nat());
  AddSystemFunction(sort_real::int2real());
  AddSystemFunction(sort_real::creal());
  AddSystemFunction(sort_real::real2pos());
  AddSystemFunction(sort_real::real2nat());
  AddSystemFunction(sort_real::real2int());
  AddSystemConstant(sort_pos::c1());
  //more
  AddSystemFunction(sort_real::maximum(sort_pos::pos(),sort_pos::pos()));
  AddSystemFunction(sort_real::maximum(sort_pos::pos(),sort_nat::nat()));
  AddSystemFunction(sort_real::maximum(sort_nat::nat(),sort_pos::pos()));
  AddSystemFunction(sort_real::maximum(sort_nat::nat(),sort_nat::nat()));
  AddSystemFunction(sort_real::maximum(sort_pos::pos(),sort_int::int_()));
  AddSystemFunction(sort_real::maximum(sort_int::int_(),sort_pos::pos()));
  AddSystemFunction(sort_real::maximum(sort_nat::nat(),sort_int::int_()));
  AddSystemFunction(sort_real::maximum(sort_int::int_(),sort_nat::nat()));
  AddSystemFunction(sort_real::maximum(sort_int::int_(),sort_int::int_()));
  AddSystemFunction(sort_real::maximum(sort_real::real_(),sort_real::real_()));
  //more
  AddSystemFunction(sort_real::minimum(sort_pos::pos(), sort_pos::pos()));
  AddSystemFunction(sort_real::minimum(sort_nat::nat(), sort_nat::nat()));
  AddSystemFunction(sort_real::minimum(sort_int::int_(), sort_int::int_()));
  AddSystemFunction(sort_real::minimum(sort_real::real_(), sort_real::real_()));
  //more
  // AddSystemFunction(sort_real::abs(sort_pos::pos()));
  // AddSystemFunction(sort_real::abs(sort_nat::nat()));
  AddSystemFunction(sort_real::abs(sort_int::int_()));
  AddSystemFunction(sort_real::abs(sort_real::real_()));
  //more
  AddSystemFunction(sort_real::negate(sort_pos::pos()));
  AddSystemFunction(sort_real::negate(sort_nat::nat()));
  AddSystemFunction(sort_real::negate(sort_int::int_()));
  AddSystemFunction(sort_real::negate(sort_real::real_()));
  AddSystemFunction(sort_real::succ(sort_pos::pos()));
  AddSystemFunction(sort_real::succ(sort_nat::nat()));
  AddSystemFunction(sort_real::succ(sort_int::int_()));
  AddSystemFunction(sort_real::succ(sort_real::real_()));
  AddSystemFunction(sort_real::pred(sort_pos::pos()));
  AddSystemFunction(sort_real::pred(sort_nat::nat()));
  AddSystemFunction(sort_real::pred(sort_int::int_()));
  AddSystemFunction(sort_real::pred(sort_real::real_()));
  AddSystemFunction(sort_real::plus(sort_pos::pos(),sort_pos::pos()));
  AddSystemFunction(sort_real::plus(sort_pos::pos(),sort_nat::nat()));
  AddSystemFunction(sort_real::plus(sort_nat::nat(),sort_pos::pos()));
  AddSystemFunction(sort_real::plus(sort_nat::nat(),sort_nat::nat()));
  AddSystemFunction(sort_real::plus(sort_int::int_(),sort_int::int_()));
  AddSystemFunction(sort_real::plus(sort_real::real_(),sort_real::real_()));
  //more
  AddSystemFunction(sort_real::minus(sort_pos::pos(), sort_pos::pos()));
  AddSystemFunction(sort_real::minus(sort_nat::nat(), sort_nat::nat()));
  AddSystemFunction(sort_real::minus(sort_int::int_(), sort_int::int_()));
  AddSystemFunction(sort_real::minus(sort_real::real_(), sort_real::real_()));
  AddSystemFunction(sort_real::times(sort_pos::pos(), sort_pos::pos()));
  AddSystemFunction(sort_real::times(sort_nat::nat(), sort_nat::nat()));
  AddSystemFunction(sort_real::times(sort_int::int_(), sort_int::int_()));
  AddSystemFunction(sort_real::times(sort_real::real_(), sort_real::real_()));
  //more
  // AddSystemFunction(sort_int::div(sort_pos::pos(), sort_pos::pos()));
  AddSystemFunction(sort_int::div(sort_nat::nat(), sort_pos::pos()));
  AddSystemFunction(sort_int::div(sort_int::int_(), sort_pos::pos()));
  // AddSystemFunction(sort_int::mod(sort_pos::pos(), sort_pos::pos()));
  AddSystemFunction(sort_int::mod(sort_nat::nat(), sort_pos::pos()));
  AddSystemFunction(sort_int::mod(sort_int::int_(), sort_pos::pos()));
  AddSystemFunction(sort_real::divides(sort_pos::pos(), sort_pos::pos()));
  AddSystemFunction(sort_real::divides(sort_nat::nat(), sort_nat::nat()));
  AddSystemFunction(sort_real::divides(sort_int::int_(), sort_int::int_()));
  AddSystemFunction(sort_real::divides(sort_real::real_(), sort_real::real_()));
  AddSystemFunction(sort_real::exp(sort_pos::pos(), sort_nat::nat()));
  AddSystemFunction(sort_real::exp(sort_nat::nat(), sort_nat::nat()));
  AddSystemFunction(sort_real::exp(sort_int::int_(), sort_nat::nat()));
  AddSystemFunction(sort_real::exp(sort_real::real_(), sort_int::int_()));
  AddSystemFunction(sort_real::floor());
  AddSystemFunction(sort_real::ceil());
  AddSystemFunction(sort_real::round());
  //Lists
  AddSystemConstant(sort_list::empty(data::unknown_sort()));
  AddSystemFunction(sort_list::cons_(data::unknown_sort()));
  AddSystemFunction(sort_list::count(data::unknown_sort()));
  AddSystemFunction(sort_list::snoc(data::unknown_sort()));
  AddSystemFunction(sort_list::concat(data::unknown_sort()));
  AddSystemFunction(sort_list::element_at(data::unknown_sort()));
  AddSystemFunction(sort_list::head(data::unknown_sort()));
  AddSystemFunction(sort_list::tail(data::unknown_sort()));
  AddSystemFunction(sort_list::rhead(data::unknown_sort()));
  AddSystemFunction(sort_list::rtail(data::unknown_sort()));
  AddSystemFunction(sort_list::in(data::unknown_sort()));
  //Sets
  AddSystemFunction(sort_bag::set2bag(data::unknown_sort()));
  AddSystemConstant(sort_set::empty(data::unknown_sort()));
  AddSystemFunction(sort_set::in(data::unknown_sort()));
  AddSystemFunction(sort_set::union_(data::unknown_sort()));
  AddSystemFunction(sort_set::difference(data::unknown_sort()));
  AddSystemFunction(sort_set::intersection(data::unknown_sort()));
  AddSystemFunction(sort_set::complement(data::unknown_sort()));

  //Bags
  AddSystemFunction(sort_bag::bag2set(data::unknown_sort()));
  AddSystemConstant(sort_bag::empty(data::unknown_sort()));
  AddSystemFunction(sort_bag::in(data::unknown_sort()));
  AddSystemFunction(sort_bag::count(data::unknown_sort()));
  AddSystemFunction(sort_bag::join(data::unknown_sort()));
  AddSystemFunction(sort_bag::difference(data::unknown_sort()));
  AddSystemFunction(sort_bag::intersection(data::unknown_sort()));

  // function update
  AddSystemFunction(data::function_update(data::unknown_sort(),data::unknown_sort()));
}

void mcrl2::data::data_expression_checker::AddFunction(const data::function_symbol &f, const std::string msg, bool allow_double_decls)
{
  const sort_expression_list domain=function_sort(f.sort()).domain();
  const core::identifier_string Name = f.name();
  const sort_expression Sort = f.sort();

  if (domain.size()==0)
  {
    if (system_constants.count(Name)>0)
    {
      throw mcrl2::runtime_error("attempt to redeclare the system constant with " + msg + " " + pp(f));
    }
  }
  else // domain.size()>0
  {
    std::map <core::identifier_string,sort_expression_list>::const_iterator j=system_functions.find(Name);
    if (j!=system_functions.end())
    {
      sort_expression_list L= j->second;
      for (; L!=aterm_list() ; L=L.tail())
      {
        sort_expression temp;
        if (TypeMatchA(Sort,L.front(),temp))
        {
          // f matches a predefined function
          throw mcrl2::runtime_error("attempt to redeclare a system function with " + msg + " " + pp(f) + ":" + pp(Sort));
        }
      }
    }
  }

  std::map <core::identifier_string,sort_expression_list>::const_iterator j=user_functions.find(Name);

  // the table user_functions contains a list of types for each
  // function name. We need to check if there is already such a type
  // in the list. If so -- error, otherwise -- add
  if (j!=user_functions.end())
  {
    sort_expression_list Types=j->second;
    if (InTypesA(Sort, Types))
    {
      if (!allow_double_decls)
      {
        throw mcrl2::runtime_error("double declaration of " + msg + " " + pp(Name));
      }
    }
    Types=Types+make_list<sort_expression>(Sort);
    user_functions[Name]=Types;
  }
  else
  {
    user_functions[Name]=make_list<sort_expression>(Sort);
  }
}

void mcrl2::data::data_expression_checker::ReadInSortStruct(const sort_expression &SortExpr)
{
  if (is_basic_sort(SortExpr))
  {
    IsSortDeclared(aterm_cast<basic_sort>(SortExpr).name());
    return;
  }

  if (is_container_sort(SortExpr))
  {
    return ReadInSortStruct(aterm_cast<container_sort>(SortExpr).element_sort());
  }

  if (is_function_sort(SortExpr))
  {
    const function_sort &fs(SortExpr);
    ReadInSortStruct(fs.codomain());

    for (sort_expression_list::const_iterator i=fs.domain().begin(); i!=fs.domain().end(); ++i)
    {
      ReadInSortStruct(*i);
    }
    return;
  }

  if (is_structured_sort(SortExpr))
  {
    const structured_sort &struct_sort(SortExpr);
    // for (aterm_list Constrs=aterm_cast<aterm_list>(SortExpr[0]); !Constrs.empty(); Constrs=Constrs.tail())
    for (structured_sort_constructor_list::const_iterator i=struct_sort.struct_constructors().begin();
               i!=struct_sort.struct_constructors().end(); ++i)
    {
      const structured_sort_constructor &Constr(*i); 

      // recognizer -- if present -- a function from SortExpr to Bool
      core::identifier_string Name=Constr.recogniser();
      if (Name!=no_identifier())
      {
        AddFunction(gsMakeOpId(Name,gsMakeSortArrow(make_list<aterm>(SortExpr),sort_bool::bool_())),"recognizer");
      }

      // constructor type and projections
      structured_sort_constructor_argument_list Projs=Constr.arguments();
      Name=Constr.name();
      if (Projs.empty())
      {
        AddConstant(gsMakeOpId(Name,SortExpr),"constructor constant");
        continue;
      }

      sort_expression_list ConstructorType;
      for (structured_sort_constructor_argument_list::const_iterator j=Projs.begin(); j!=Projs.end(); ++j)
      {
        structured_sort_constructor_argument Proj= *j;
        sort_expression ProjSort=Proj.sort(); 

        // not to forget, recursive call for ProjSort ;-)
        ReadInSortStruct(ProjSort);

        core::identifier_string ProjName=Proj.name();
        if (ProjName!=no_identifier())
        {
          AddFunction(function_symbol(ProjName,function_sort(make_list(SortExpr),ProjSort)),"projection",true);
        }
        ConstructorType.push_front(ProjSort);
      }
      AddFunction(gsMakeOpId(Name,gsMakeSortArrow(reverse(ConstructorType),SortExpr)),"constructor");
    }
    return;
  }
}

void mcrl2::data::data_expression_checker::ReadInConstructors(const std::map<core::identifier_string,sort_expression>::const_iterator begin,
                        const std::map<core::identifier_string,sort_expression>::const_iterator end)
{
  for (std::map<core::identifier_string,sort_expression>::const_iterator i=begin; i!=end; ++i)
  {
    static_cast<sort_expression_checker>(*this)(i->second); // Type check sort expression.
    ReadInSortStruct(i->second);
  }
}

/* void ReadInConstructors(const sort_expression_list::const_iterator begin,
                                   const sort_expression_list::const_iterator end)
{
  for (sort_expression_list::const_iterator i=begin; i!=end; ++i)
  {
    assert(user_defined_sorts.count(*i)>0);
    const sort_expression SortExpr=user_defined_sorts[*i];
    gstcIsSortExprDeclared(SortExpr);

    gstcReadInSortStruct(SortExpr);
  }
} */


mcrl2::data::data_expression_checker::data_expression_checker(const data_specification &data_spec)
      : sort_expression_checker(data_spec),
        was_warning_upcasting(false),
        was_ambiguous(false)

{ 
  mCRL2log(verbose) << "type checking data expression..." << std::endl;
  initialise_system_defined_functions();

  // aterm_list constructors = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]);
  // aterm_list mappings = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]);

  //XXX read-in from spec (not finished)
  try
  {
    ReadInConstructors(defined_sorts.begin(),defined_sorts.end());
    ReadInFuncs(data_spec.user_defined_constructors(),data_spec.user_defined_mappings());
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of data expression failed");
  }
  mCRL2log(debug) << "type checking of data expression read-in phase finished" << std::endl;
}

data_expression mcrl2::data::data_expression_checker::operator ()(
           const data_expression &data_expr,
           const std::map<core::identifier_string,sort_expression> &Vars)
{
  data_expression data=data_expr;
  sort_expression Type; 
  try 
  {
    Type=TraverseVarConsTypeD(Vars,Vars,data,data::unknown_sort());
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of data expression failed");
  }
  if (data::is_unknown_sort(Type))
  {
    throw mcrl2::runtime_error("type checking of data expression failed. Result is an unknown sort.");
  }
  
  return data;
} 

// ------------------------------  Here ends the new class based data expression checker -----------------------
// ------------------------------  Here starts the new class based data specification checker -----------------------

// Type check and replace user defined equations.
void mcrl2::data::data_specification_checker::TransformVarConsTypeData(data_specification &data_spec)
{
  std::map<core::identifier_string,sort_expression> DeclaredVars;
  std::map<core::identifier_string,sort_expression> FreeVars;

  //Create a new specification; admittedly, this is somewhat clumsy.
  data_specification new_specification;
  for(sort_expression_vector::const_iterator i=data_spec.user_defined_sorts().begin(); i!=data_spec.user_defined_sorts().end(); ++i)
  {
    new_specification.add_sort(*i);
  }
  for(alias_vector::const_iterator i=data_spec.user_defined_aliases().begin(); i!=data_spec.user_defined_aliases().end(); ++i)
  {
    new_specification.add_alias(*i);
  }
  for(function_symbol_vector::const_iterator i=data_spec.user_defined_constructors().begin(); i!=data_spec.user_defined_constructors().end(); ++i)
  {
    new_specification.add_constructor(*i);
  }
  for(function_symbol_vector::const_iterator i=data_spec.user_defined_mappings().begin(); i!=data_spec.user_defined_mappings().end(); ++i)
  {
    new_specification.add_mapping(*i);
  }
  
  data_equation_vector equations=data_spec.user_defined_equations();
  for (data_equation_vector::const_iterator i=equations.begin(); i!=equations.end(); ++i)
  {
    data_equation Eqn= *i;
    variable_list VarList=Eqn.variables();

    if (!VarsUnique(VarList))
    {
      throw mcrl2::runtime_error("the variables " + pp(VarList) + " in equation declaration " + pp(Eqn) + " are not unique");
    }

    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    AddVars2Table(DeclaredVars,VarList,NewDeclaredVars);
    DeclaredVars=NewDeclaredVars;

    data_expression Left=Eqn.lhs();

    sort_expression LeftType;
    try
    {
      LeftType=TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,data::unknown_sort(),FreeVars,false,true);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nerror occurred while typechecking " + pp(Left) + " as left hand side of equation " + pp(Eqn));
    }
    
    if (was_warning_upcasting)
    {
      was_warning_upcasting=false;
      mCRL2log(warning) << "warning occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
    }

    data_expression Cond=Eqn.condition();
    TraverseVarConsTypeD(DeclaredVars,FreeVars,Cond,sort_bool::bool_());
    
    data_expression Right=Eqn.rhs();
    std::map<core::identifier_string,sort_expression> dummy_empty_table;
    sort_expression RightType;
    try
    {
      RightType=TraverseVarConsTypeD(DeclaredVars,FreeVars,Right,LeftType,dummy_empty_table,false);
    }
    catch (mcrl2::runtime_error &e)
    { 
      throw mcrl2::runtime_error(std::string(e.what()) + "\nerror occurred while typechecking " + pp(Right) + " as right hand side of equation " + pp(Eqn));
    }

    //If the types are not uniquely the same now: do once more:
    if (!EqTypesA(LeftType,RightType))
    {
      sort_expression Type;
      if (!TypeMatchA(LeftType,RightType,Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " do not match.");
      }
      Left=Eqn.lhs();
      FreeVars.clear();
      try
      {
        LeftType=TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,Type,FreeVars,true);
      }
      catch (mcrl2::runtime_error &e)
      {      
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypes of the left- and right-hand-sides of the equation " + pp(Eqn) + " do not match.");
      }
      if (was_warning_upcasting)
      {
        was_warning_upcasting=false;
        mCRL2log(warning) << "warning occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
      }
      Right=Eqn.rhs();
      try 
      {
        RightType=TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Right,LeftType,FreeVars);
      }
      catch (mcrl2::runtime_error &e)
      {    
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypes of the left- and right-hand-sides of the equation " + pp(Eqn) + " do not match.");
      }
      if (!TypeMatchA(LeftType,RightType,Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " do not match");
      }
      if (core::gstcHasUnknown(Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " cannot be uniquely determined");
      }
    }
    DeclaredVars.clear();
    // NewEqns.push_back(data_equation(VarList,Cond,Left,Right));
    new_specification.add_equation(data_equation(VarList,Cond,Left,Right));
  }
  data_spec=new_specification;
}

mcrl2::data::data_specification_checker::data_specification_checker(const data_specification &data_spec)
  : data_expression_checker(data_spec)
{
  mCRL2log(debug) << "type checking phase started" << std::endl;

  type_checked_spec=data_spec;
  type_checked_spec.declare_data_specification_to_be_type_checked();
  
  // Type check equations and add them to the specification.
  TransformVarConsTypeData(type_checked_spec);
  
  // type_checked_spec = gstcFoldSortRefs(type_checked_spec); TODO OUGHT TO BE ADDED 

  mCRL2log(debug) << "type checking phase finished" << std::endl;
  
}

const data_specification mcrl2::data::data_specification_checker::operator()()
{
  return type_checked_spec;
}

// ------------------------------  Here ends the new class based data specification checker -----------------------





namespace core 
{
sort_expression type_check_sort_expr(const sort_expression &sort_expr, aterm_appl spec)
{
  mCRL2log(verbose) << "type checking sort expression..." << std::endl;
  //check correctness of the sort expression in sort_expr
  //using the specification in spec
  assert(gsIsSortExpr(sort_expr));
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));

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

  if (is_unknown_sort(sort_expr) || is_multiple_possible_sorts(sort_expr))
  {
    throw mcrl2::runtime_error("type checking of sort expressions failed (" + pp(sort_expr) + ") is not a sort expression)");
  }
  gstcIsSortExprDeclared(sort_expr);
    
  return sort_expr;
}

data_expression type_check_data_expr(const data_expression &data_expr, aterm_appl spec, const std::map<core::identifier_string,sort_expression> &Vars)
{
  mCRL2log(verbose) << "type checking data expression..." << std::endl;
  //check correctness of the data expression in data_expr using
  //the specification in spec

  //check preconditions
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  assert(gsIsDataExpr(data_expr));

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
    gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
    gstcReadInFuncs(constructors,mappings);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of data expression failed");
  }
  mCRL2log(debug) << "type checking of data expression read-in phase finished" << std::endl;

  data_expression data=data_expr;
  sort_expression Type; 
  try 
  {
    Type=gstcTraverseVarConsTypeD(Vars,Vars,data,data::unknown_sort());
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of data expression failed");
  }
  if (data::is_unknown_sort(Type))
  {
    throw mcrl2::runtime_error("type checking of data expression failed. Result is an unknown sort.");
  }
  
  return data;
  
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
    gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
    gstcReadInFuncs(constructors,mappings);
    gstcReadInActs(action_labels);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading from LPS failed");
  }
  {
    mCRL2log(debug) << "type checking of multiactions read-in phase finished" << std::endl;

    if (gsIsMultAct(mult_act))
    {
      std::map<core::identifier_string,sort_expression> Vars;
      aterm_list r;
      for (aterm_list l=aterm_cast<aterm_list>(mult_act[0]); !l.empty(); l=l.tail())
      {
        aterm_appl o=ATAgetFirst(l);
        assert(gsIsParamId(o));
        o=gstcTraverseActProcVarConstP(Vars,o);
        r.push_front(o);
      }
      Result=mult_act.set_argument(reverse(r),0);
    }
    else
    {
      throw mcrl2::runtime_error("type checking of multiactions failed (" + pp(mult_act) + " is not a multiaction)");
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
    gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
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
    std::map<core::identifier_string,sort_expression> Vars;
    aterm_list r;

    for (aterm_list l=(aterm_list)(mult_actions.front()) ; !l.empty(); l=l.tail())
    {
      aterm_appl o=ATAgetFirst(l);
      assert(gsIsParamId(o));
      try 
      {
        o=gstcTraverseActProcVarConstP(Vars,o);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nTypechecking action failed: "+ pp(ATAgetFirst(l)));
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
    gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
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
  
  try
  {
    gstcReadInActs(action_labels);
  }
  catch (mcrl2::runtime_error &e)
  {
    mCRL2log(warning) << std::string(e.what()) << "\nignoring the previous error(s), the formula will be typechecked without action label information" << std::endl;
  }
  
  mCRL2log(debug) << "type checking of state formulas read-in phase finished" << std::endl;

  std::map<core::identifier_string,sort_expression> Vars;
  std::map<core::identifier_string,sort_expression_list> StateVars;
  return gstcTraverseStateFrm(Vars,StateVars,state_frm);
  
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

  std::map<core::identifier_string,term_list<sort_expression_list> > actions_from_lps;

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
    gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end());
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
  
  std::map<core::identifier_string,sort_expression> LPSSorts=context.defined_sorts; // remember the sorts from the LPS.
  gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]));
  
  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file sorts finished" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  core::identifier_string_list sorts=list_minus(context.defined_sorts, LPSSorts);
  gstcReadInConstructors(sorts.begin(),sorts.end());

  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file constructors finished" << std::endl;

  gstcReadInFuncs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[1])[0]),
                       aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[2])[0]));
  
  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file functions finished" << std::endl;

  body.equations=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[3])[0]);

  //Save the actions from LPS only for later use.
  actions_from_lps=context.actions;
  gstcReadInActs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(ar_spec[1])[0]));

  mCRL2log(debug) << "type checking action rename specification read-in phase of the ActionRenameSpec finished" << std::endl;

  gstcTransformVarConsTypeData();
  
  mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;

  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
  Result=ar_spec.set_argument(data_spec,0);
  Result=gstcFoldSortRefs(Result);


  // now the action renaming rules themselves.
  aterm_appl ActionRenameRules=aterm_cast<aterm_appl>(ar_spec[2]);
  aterm_list NewRules;

  std::map<core::identifier_string,sort_expression> DeclaredVars;
  std::map<core::identifier_string,sort_expression> FreeVars;

  for (aterm_list l=aterm_cast<aterm_list>(ActionRenameRules[0]); !l.empty(); l=l.tail())
  {
    aterm_appl Rule=ATAgetFirst(l);
    assert(gsIsActionRenameRule(Rule));

    aterm_list VarList=aterm_cast<aterm_list>(Rule[0]);
    if (!gstcVarsUnique(VarList))
    {
      throw mcrl2::runtime_error("the variables " + pp(VarList) + " in action rename rule " + pp(Rule) + " are not unique");
    }

    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    gstcAddVars2Table(DeclaredVars,VarList,NewDeclaredVars);
    
    DeclaredVars=NewDeclaredVars;

    aterm_appl Left=aterm_cast<aterm_appl>(Rule[2]);
    assert(gsIsParamId(Left));
    {
      //extra check requested by Tom: actions in the LHS can only come from the LPS
      context.actions.swap(actions_from_lps);
      Left=gstcTraverseActProcVarConstP(DeclaredVars,Left);
      actions_from_lps.swap(context.actions);
    }

    data_expression Cond=aterm_cast<data_expression>(Rule[1]);
    gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Cond,sort_bool::bool_());

    aterm_appl Right(Rule[3]);
    assert(gsIsParamId(Right) || gsIsTau(Right) || gsIsDelta(Right));
    Right=gstcTraverseActProcVarConstP(DeclaredVars,Right);

    NewRules.push_front(gsMakeActionRenameRule(VarList,Cond,Left,Right));
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
  std::map<core::identifier_string,sort_expression> dummy;

  gstcReadInSorts(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(data_spec[0])[0]));
    
  mCRL2log(debug) << "type checking of PBES specification read-in phase of sorts finished" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  gstcReadInConstructors(context.defined_sorts.begin(),context.defined_sorts.end()); 
  
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
  gstcTransformVarConsTypeData();

  gstcTransformPBESVarConst();
  
  mCRL2log(debug) << "type checking transform Data+PBES phase finished" << std::endl;

  data_spec=data_spec.set_argument(gsMakeDataEqnSpec(body.equations),3);
  Result=pbes_spec.set_argument(data_spec,0);

  pb_eqn_spec=pb_eqn_spec.set_argument(gstcWritePBES(aterm_cast<aterm_list>(pb_eqn_spec[0])),0);
  Result=Result.set_argument(pb_eqn_spec,2);

  pb_init=pb_init.set_argument(aterm_cast<aterm_appl>(body.proc_bodies[INIT_KEY()]),0);
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

  std::map<core::identifier_string,sort_expression> Vars;
  std::map<core::identifier_string,sort_expression> NewVars;
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

void gstcSplitSortDecls(const aterm_list &SortDecls, 
                        basic_sort_list &PSortIds,
                        alias_list &PSortRefs)
{
  basic_sort_vector SortIds;
  alias_vector SortRefs;
  for(aterm_list::const_iterator i=SortDecls.begin(); i!=SortDecls.end(); ++i)
  {
    const aterm_appl &SortDecl(*i);
    if (gsIsSortRef(SortDecl))
    {
      SortRefs.push_back(SortDecl);
    }
    else     //gsIsSortId(SortDecl)
    {
      SortIds.push_back(SortDecl);
    }
  }
  PSortIds = basic_sort_list(SortIds.begin(),SortIds.end());
  PSortRefs = alias_list(SortRefs.begin(),SortRefs.end());
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
  basic_sort_list SortIds;
  alias_list SortRefs;
  gstcSplitSortDecls(SortDecls, SortIds, SortRefs);
  //fold sort references in the sort references themselves
  SortRefs = gstcFoldSortRefsInSortRefs(SortRefs);
  //substitute sort references in the rest of Spec, i.e.
  //(a) remove sort references from Spec
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(SortIds));
  //(b) build substitution table
  std::map <sort_expression,sort_expression> Substs;
  aterm_list l = SortRefs;
  for(alias_list::const_iterator l=SortRefs.begin(); l!=SortRefs.end(); ++l)
  {
    const alias &SortRef = *l;
    //add substitution for SortRef
    const basic_sort LHS = SortRef.name(); 
    const sort_expression &RHS = SortRef.reference();
    if (is_basic_sort(RHS) || is_function_sort(RHS))
    {
      //add forward substitution
      Substs[LHS]=RHS;
    }
    else
    {
      //add backward substitution
      Substs[RHS]=LHS;
    }
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
  Spec = gstcUpdateSortSpec(Spec, gsMakeSortSpec(aterm_cast<aterm_list>(SortIds)+aterm_cast<aterm_list>(SortRefs)));
  mCRL2log(debug) << "specification after folding:\n" << pp(Spec) << "\n" ;
  return Spec;
}

alias_list gstcFoldSortRefsInSortRefs(alias_list SortRefs)
{
  //fold sort references in SortRefs by means of repeated forward and backward
  //substitution
  alias_list NewSortRefs = SortRefs;
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
      const aterm_appl &SortRef = aterm_cast<aterm_appl>(element_at(NewSortRefs, i));
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
          const aterm_appl &OldSortRef = aterm_cast<aterm_appl>(element_at(NewSortRefs, j));
          aterm_appl NewSortRef = aterm_cast<aterm_appl>(Subst(OldSortRef));
          if (NewSortRef!=OldSortRef)
          {
            NewSortRefs = alias_list_replace(NewSortRefs, NewSortRef, j);
          }
        }
      }
    }
    mCRL2log(debug) << std::endl;
  }
  while (NewSortRefs!=SortRefs);
  //remove self references
  alias_list l;
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
  gssystem.constants.clear();
  gssystem.functions.clear();
  context.basic_sorts.clear();
  context.defined_sorts.clear();
  context.constants.clear();
  context.functions.clear();
  context.actions.clear();
  context.processes.clear();
  context.PBs.clear();
  context.glob_vars.clear();
  body.proc_pars.clear();
  body.proc_bodies.clear();
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
  const std::map<core::identifier_string,sort_expression>::const_iterator i=context.defined_sorts.find(start_search.name());

  if (i==context.defined_sorts.end())
  {
    // start_search is not a sort alias, and hence not a recursive sort.
    return false;
  }
  aterm_appl aterm_reference= i->second;

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
  for (; !Sorts.empty(); Sorts=Sorts.tail())
  {
    aterm_appl Sort=ATAgetFirst(Sorts);
    core::identifier_string SortName=aterm_cast<core::identifier_string>(Sort[0]);
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
    if (context.basic_sorts.count(SortName)>0 || context.defined_sorts.count(SortName)>0)
    {
      throw mcrl2::runtime_error("double declaration of sort " + pp(SortName));
    }
    if (gsIsSortId(Sort))
    {
      context.basic_sorts.insert(SortName);
    }
    else if (gsIsSortRef(Sort))
    {
      context.defined_sorts[SortName]=aterm_cast<aterm_appl>(Sort[1]);
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

  for (std::map<core::identifier_string,sort_expression>::const_iterator i=context.defined_sorts.begin(); 
              i!=context.defined_sorts.end(); ++i)
  {
    std::set < basic_sort > visited;
    const basic_sort s(core::identifier_string(i->first));
    const sort_expression ar(i->second);
    if (gstc_check_for_sort_alias_loop_through_function_sort_via_expression(ar,s,visited,false))
    {
      throw mcrl2::runtime_error("sort " + pp(i->first) + " is recursively defined via a function sort, or a set or a bag type container");
    }
  }
}

static void gstcReadInConstructors(const std::map<core::identifier_string,sort_expression>::const_iterator begin,
                                   const std::map<core::identifier_string,sort_expression>::const_iterator end)
{
  for (std::map<core::identifier_string,sort_expression>::const_iterator i=begin; i!=end; ++i) 
  {
    gstcIsSortExprDeclared(i->second);
    gstcReadInSortStruct(i->second);
  }
}

static void gstcReadInConstructors(const core::identifier_string_list::const_iterator begin,
                                   const core::identifier_string_list::const_iterator end)
{
  for (core::identifier_string_list::const_iterator i=begin; i!=end; ++i) 
  {
    assert(context.defined_sorts.count(*i)>0);
    const sort_expression SortExpr=context.defined_sorts[*i];
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
  for (std::map<core::identifier_string,sort_expression>::const_iterator sort_walker=context.defined_sorts.begin();  
               sort_walker!=context.defined_sorts.end(); ++sort_walker)
  {
    const core::identifier_string sort_name(sort_walker->first);
    const data::basic_sort first(sort_name);
    const data::sort_expression second(sort_walker->second);
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
    // aterm_list defined_sorts=context.defined_sorts.keys();
    std::map < sort_expression, basic_sort > normalised_aliases=construct_normalised_aliases();
    std::set< sort_expression > all_sorts;
    for (std::map<core::identifier_string,sort_expression>::const_iterator i=context.defined_sorts.begin();
              i!=context.defined_sorts.end(); ++i)
    {
      // const basic_sort s(core::identifier_string(gstcUnwindType(i->second)));
      const sort_expression reference=i->second;
      // if (is_container_sort(i->first) || is_function_sort(i->first))
      find_sort_expressions<sort_expression>(reference, std::inserter(all_sorts, all_sorts.end()));
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
    core::identifier_string ActName=aterm_cast<core::identifier_string>(Act[0]);
    sort_expression_list ActType=aterm_cast<sort_expression_list>(Act[1]);

    gstcIsSortExprListDeclared(ActType);

    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.actions.find(ActName);
    term_list<sort_expression_list> Types;
    if (j==context.actions.end())
    {
      // Types=make_list<sort_expression_list>(ActType);
      Types=make_list<sort_expression_list>(ActType);
    }
    else
    {
      Types=j->second;
      // the table context.actions contains a list of types for each
      // action name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ActType, Types))
      {
        throw mcrl2::runtime_error("double declaration of action " + pp(ActName));
      }
      else
      {
        Types=Types+make_list<sort_expression_list>(ActType);
      }
    }
    context.actions[ActName]=Types;
  }
}

static void gstcReadInProcsAndInit(aterm_list Procs, aterm_appl Init)
{
  for (; !Procs.empty(); Procs=Procs.tail())
  {
    aterm_appl Proc=ATAgetFirst(Procs);
    core::identifier_string ProcName=aterm_cast<core::identifier_string>(aterm_cast<aterm_appl>(Proc[0])[0]);

    if (context.actions.count(ProcName)>0)
    {
      throw mcrl2::runtime_error("declaration of both process and action " + pp(ProcName));
    }

    sort_expression_list ProcType=aterm_cast<sort_expression_list>(aterm_cast<aterm_appl>(Proc[0])[1]);

    gstcIsSortExprListDeclared(ProcType);

    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.processes.find(ProcName);
    term_list<sort_expression_list> Types;
    if (j==context.processes.end())
    {
      Types=make_list<sort_expression_list>(ProcType);
    }
    else
    {
      Types=j->second;
      // the table context.processes contains a list of types for each
      // process name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ProcType, Types))
      {
        throw mcrl2::runtime_error("double declaration of process " + pp(ProcName));
      }
      else
      {
        Types=Types+make_list<sort_expression_list>(ProcType);
      }
    }
    context.processes[ProcName]=Types;

    //check that all formal parameters of the process are unique.
    aterm_list ProcVars=aterm_cast<aterm_list>(Proc[1]);
    if (!gstcVarsUnique(ProcVars))
    {
      throw mcrl2::runtime_error("the formal variables in process " + pp(Proc) + " are not unique");
    }

    body.proc_pars[aterm_cast<aterm_appl>(Proc[0])]=aterm_cast<variable_list>(Proc[1]);
    body.proc_bodies[aterm_cast<aterm_appl>(Proc[0])]=aterm_cast<aterm_appl>(Proc[2]);
  }
  body.proc_pars[INIT_KEY()]=variable_list();
  body.proc_bodies[INIT_KEY()]=Init;

}

static void gstcReadInPBESAndInit(aterm_appl PBEqnSpec, aterm_appl PBInit)
{
  aterm_list PBEqns=aterm_cast<aterm_list>(PBEqnSpec[0]);

  for (; !PBEqns.empty(); PBEqns=PBEqns.tail())
  {
    aterm_appl PBEqn=ATAgetFirst(PBEqns);
    core::identifier_string PBName=aterm_cast<core::identifier_string>(aterm_cast<aterm_appl>(PBEqn[1])[0]);

    const variable_list &PBVars=aterm_cast<variable_list>(aterm_cast<aterm_appl>(PBEqn[1])[1]);

    sort_expression_list PBType;
    for (variable_list::const_iterator l=PBVars.begin(); l!=PBVars.end(); ++l)
    {
      PBType.push_front(l->sort());
    }
    PBType=reverse(PBType);

    gstcIsSortExprListDeclared(PBType);

    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator i=context.PBs.find(PBName);
    term_list<sort_expression_list> Types;
    if (i==context.PBs.end())
    {
      Types=make_list<sort_expression_list>(PBType);
    }
    else
    {
      Types=i->second;
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
        Types=Types+make_list(PBType);
      }
    }
    context.PBs[PBName]=Types;

    //This is a fake ProcVarId (There is no PBVarId)
    aterm_appl Index=gsMakeProcVarId(PBName,PBType);
    body.proc_pars[Index]=PBVars;
    body.proc_bodies[Index]=aterm_cast<aterm_appl>(PBEqn[2]);
  }
  body.proc_pars[INIT_KEY()]=variable_list();
  body.proc_bodies[INIT_KEY()]=aterm_cast<aterm_appl>(PBInit[0]);
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
                    aterm_cast<aterm_list>(body.proc_pars[ProcVar]),
                    aterm_cast<aterm_appl>(body.proc_bodies[ProcVar])));
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
    Result.push_front(PBEqn.set_argument(aterm_cast<aterm_appl>(body.proc_bodies[Index]),2));
  }
  return reverse(Result);
}


static void gstcTransformVarConsTypeData(void)
{
  std::map<core::identifier_string,sort_expression> DeclaredVars;
  std::map<core::identifier_string,sort_expression> FreeVars;

  //data terms in equations
  aterm_list NewEqns;
  
  for (aterm_list Eqns=body.equations; !Eqns.empty(); Eqns=Eqns.tail())
  {
    aterm_appl Eqn=ATAgetFirst(Eqns);
    aterm_list VarList=aterm_cast<aterm_list>(Eqn[0]);

    if (!gstcVarsUnique(VarList))
    {
      throw mcrl2::runtime_error("the variables " + pp(VarList) + " in equation declaration " + pp(Eqn) + " are not unique");
    }

    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    gstcAddVars2Table(DeclaredVars,VarList,NewDeclaredVars);
    DeclaredVars=NewDeclaredVars;

    data_expression Left=aterm_cast<aterm_appl>(Eqn[2]);

    aterm_appl LeftType;
    try
    {
      LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,data::unknown_sort(),FreeVars,false,true);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nerror occurred while typechecking " + pp(Left) + " as left hand side of equation " + pp(Eqn));
    }
    
    if (was_warning_upcasting)
    {
      was_warning_upcasting=false;
      mCRL2log(warning) << "warning occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
    }

    data_expression Cond=aterm_cast<aterm_appl>(Eqn[1]);
    gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,Cond,sort_bool::bool_());
    
    data_expression Right=aterm_cast<aterm_appl>(Eqn[3]);
    std::map<core::identifier_string,sort_expression> dummy_empty_table;
    aterm_appl RightType;
    try
    {
      RightType=gstcTraverseVarConsTypeD(DeclaredVars,FreeVars,Right,LeftType,dummy_empty_table,false);
    }
    catch (mcrl2::runtime_error &e)
    { 
      throw mcrl2::runtime_error(std::string(e.what()) + "\nerror occurred while typechecking " + pp(Right) + " as right hand side of equation " + pp(Eqn));
    }

    //If the types are not uniquely the same now: do once more:
    if (!gstcEqTypesA(LeftType,RightType))
    {
      aterm_appl Type;
      if (!gstcTypeMatchA(LeftType,RightType,Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " do not match.");
      }
      Left=aterm_cast<aterm_appl>(Eqn[2]);
      FreeVars.clear();
      try
      {
        LeftType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,Type,FreeVars,true);
      }
      catch (mcrl2::runtime_error &e)
      {      
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypes of the left- and right-hand-sides of the equation " + pp(Eqn) + " do not match.");
      }
      if (was_warning_upcasting)
      {
        was_warning_upcasting=false;
        mCRL2log(warning) << "warning occurred while typechecking " << pp(Left) << " as left hand side of equation " << pp(Eqn) << std::endl;
      }
      Right=aterm_cast<aterm_appl>(Eqn[3]);
      try 
      {
        RightType=gstcTraverseVarConsTypeD(DeclaredVars,DeclaredVars,Right,LeftType,FreeVars);
      }
      catch (mcrl2::runtime_error &e)
      {    
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypes of the left- and right-hand-sides of the equation " + pp(Eqn) + " do not match.");
      }
      if (!gstcTypeMatchA(LeftType,RightType,Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " do not match");
      }
      if (gstcHasUnknown(Type))
      {
        throw mcrl2::runtime_error("types of the left- (" + pp(LeftType) + ") and right- (" + pp(RightType) + ") hand-sides of the equation " + pp(Eqn) + " cannot be uniquely determined");
      }
    }
    DeclaredVars.clear();
    NewEqns.push_front(gsMakeDataEqn(VarList,Cond,Left,Right));
  }
  body.equations=reverse(NewEqns);

}

static void gstcTransformActProcVarConst(void)
{
  std::map<core::identifier_string,sort_expression> Vars;

  //process and data terms in processes and init
  for (std::map <aterm_appl,variable_list>::const_iterator i=body.proc_pars.begin(); i!=body.proc_pars.end(); ++i)
  {
    aterm_appl ProcVar=i->first;

    Vars=context.glob_vars;

    std::map<core::identifier_string,sort_expression> NewVars;
    gstcAddVars2Table(Vars,i->second,NewVars);
    Vars=NewVars;

    aterm_appl NewProcTerm=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(body.proc_bodies[ProcVar]));
    body.proc_bodies[ProcVar]=NewProcTerm;
  }
}

static void gstcTransformPBESVarConst(void)
{
  std::map<core::identifier_string,sort_expression> Vars;

  //PBEs and data terms in PBEqns and init
  // for (aterm_list PBVars=body.proc_pars.keys(); !PBVars.empty(); PBVars=PBVars.tail())
  for (std::map <aterm_appl,variable_list>::const_iterator i=body.proc_pars.begin(); i!=body.proc_pars.end(); ++i)
  {
    aterm_appl PBVar=i->first;
    
    Vars=context.glob_vars;

    std::map<core::identifier_string,sort_expression> NewVars;
    gstcAddVars2Table(Vars,i->second,NewVars);
    Vars=NewVars;

    aterm_appl NewPBTerm=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(body.proc_bodies[PBVar]));
    body.proc_bodies[PBVar]=NewPBTerm;
  }
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
  if (context.basic_sorts.count(aterm_cast<core::identifier_string>(SortName))>0)
  {
    return;
  }
  if (context.defined_sorts.count(aterm_cast<core::identifier_string>(SortName))>0)
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

  core::identifier_string Name = data::function_symbol(OpId).name();
  aterm_appl Sort = data::function_symbol(OpId).sort();

  if (context.constants.count(Name)>0)
  {
    throw mcrl2::runtime_error("double declaration of " + msg + " " + pp(Name));
  }

  if (gssystem.constants.count(Name)>0 || gssystem.functions.count(Name)>0)
  {
    throw mcrl2::runtime_error("attempt to declare a constant with the name that is a built-in identifier (" + pp(Name) + ")");
  }

  context.constants[Name]=Sort;
}

static void gstcAddFunction(aterm_appl OpId, const std::string msg, bool allow_double_decls)
{
  assert(gsIsOpId(OpId));
  const data::function_symbol f(OpId);
  const sort_expression_list domain=function_sort(f.sort()).domain();
  core::identifier_string Name = f.name();
  const sort_expression Sort = f.sort();

  if (domain.size()==0)
  {
    if (gssystem.constants.count(Name)>0)
    {
      throw mcrl2::runtime_error("attempt to redeclare the system constant with " + msg + " " + pp(OpId));
    }
  }
  else // domain.size()>0
  {
    std::map <core::identifier_string,sort_expression_list>::const_iterator j=gssystem.functions.find(Name);
    if (j!=gssystem.functions.end())
    {
      aterm_list L= j->second;
      for (; L!=aterm_list() ; L=L.tail())
      {
        aterm_appl temp;
        if (gstcTypeMatchA(Sort,(aterm_appl)L.front(),temp))
        {
          // f matches a predefined function
          throw mcrl2::runtime_error("attempt to redeclare a system function with " + msg + " " + pp(OpId) + ":" + pp(Sort));
        }
      }
    }
  }

  std::map <core::identifier_string,sort_expression_list>::const_iterator j=context.functions.find(Name);

  // the table context.functions contains a list of types for each
  // function name. We need to check if there is already such a type
  // in the list. If so -- error, otherwise -- add
  if (j!=context.functions.end())
  {
    sort_expression_list Types=j->second;
    if (gstcInTypesA(Sort, Types))
    {
      if (!allow_double_decls)
      {
        throw mcrl2::runtime_error("double declaration of " + msg + " " + pp(Name));
      }
    }
    Types=Types+make_list<sort_expression>(Sort);
    context.functions[Name]=Types;
  }
  else
  {
    context.functions[Name]=make_list<sort_expression>(Sort);
  }
}

static void gstcAddSystemConstant(aterm_appl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.constants table
  assert(gsIsOpId(OpId));
  core::identifier_string OpIdName = data::function_symbol(OpId).name();
  const sort_expression Type = data::function_symbol(OpId).sort();

  std::map<core::identifier_string,sort_expression_list>::const_iterator i=gssystem.constants.find(OpIdName);

  sort_expression_list Types;
  if (i!=gssystem.constants.end())
  { 
    Types=i->second;
  }
  Types=push_back(Types,Type);
  gssystem.constants[OpIdName]=Types;
}

static void gstcAddSystemFunction(aterm_appl OpId)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  assert(gsIsOpId(OpId));
  core::identifier_string OpIdName = data::function_symbol(OpId).name();
  aterm_appl Type = data::function_symbol(OpId).sort();
  assert(gsIsSortArrow(Type));

  const std::map <core::identifier_string,sort_expression_list>::const_iterator j=gssystem.functions.find(OpIdName);

  sort_expression_list Types;
  if (j!=gssystem.functions.end())
  {
    Types=j->second;
  }
  Types=Types+make_list<sort_expression>(Type);  // TODO: Avoid concatenate but the order is essential.
  gssystem.functions[OpIdName]=Types;
}


static bool gstcVarsUnique(aterm_list VarDecls)
{
  bool Result=true;
  std::set<aterm_appl> Temp;

  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    aterm_appl VarDecl=ATAgetFirst(VarDecls);
    aterm_appl VarName=aterm_cast<aterm_appl>(VarDecl[0]);
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    if (!Temp.insert(VarName).second) // The VarName is already in the set.
    {
      return false;
    }
  }

  return Result;
}

static void gstcAddVars2Table(std::map<core::identifier_string,sort_expression> &Vars, aterm_list VarDecls, std::map<core::identifier_string,sort_expression> &result)
{
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    aterm_appl VarDecl=ATAgetFirst(VarDecls);
    core::identifier_string VarName=aterm_cast<core::identifier_string>(VarDecl[0]);
    aterm_appl VarType=aterm_cast<aterm_appl>(VarDecl[1]);
    //test the type
    gstcIsSortExprDeclared(VarType);

    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    Vars[VarName]=VarType;
  }
  result=Vars;
}

static std::map<core::identifier_string,sort_expression> gstcRemoveVars(
                      std::map<core::identifier_string,sort_expression> &Vars, 
                      variable_list VarDecls)
{
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    aterm_appl VarDecl=ATAgetFirst(VarDecls);
    core::identifier_string VarName=aterm_cast<core::identifier_string>(VarDecl[0]);

    Vars.erase(VarName);
  }

  return Vars;
}

static aterm_appl gstcRewrActProc(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl ProcTerm, bool is_pbes)
{
  aterm_appl Result;
  core::identifier_string Name(ProcTerm[0]);
  term_list<sort_expression_list> ParList;

  bool action=false;

  if (!is_pbes)
  {
    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.actions.find(Name);

    if (j!=context.actions.end())
    {
      ParList=j->second;
      action=true;
    }
    else
    {
      const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.processes.find(Name);
      if (j!=context.processes.end()) 
      {
        ParList=j->second;
        action=false;
      }
      else
      {
        throw mcrl2::runtime_error("action or process " + pp(Name) + " not declared");
      }
    }
  }
  else
  {
    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.PBs.find(Name);
    if (j==context.PBs.end())
    {
      throw mcrl2::runtime_error("propositional variable " + pp(Name) + " not declared");
    }
    ParList=j->second;
  }
  assert(!ParList.empty());

  size_t nFactPars=aterm_cast<aterm_list>(ProcTerm[1]).size();
  const std::string msg=(is_pbes)?"propositional variable":((action)?"action":"process");

  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    term_list <sort_expression_list> NewParList;
    for (; !ParList.empty(); ParList=ParList.tail())
    {
      sort_expression_list Par=ParList.front();
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

    aterm_appl NewPosType;
    try
    {
      NewPosType=gstcTraverseVarConsTypeD(Vars,Vars,Par,PosType); //gstcExpandNumTypesDown(PosType));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + pp(Par) + " as type " + pp(gstcExpandNumTypesDown(PosType)) + " (while typechecking " + pp(ProcTerm) + ")");
    }
    NewPars.push_front(Par);
    NewPosTypeList.push_front(NewPosType);
  }
  NewPars=reverse(NewPars);
  NewPosTypeList=reverse(NewPosTypeList);

  std::pair<bool,aterm_list>p=gstcAdjustNotInferredList(NewPosTypeList,ParList);
  PosTypeList=p.second;

  if (!p.first)
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

      aterm_appl CastedNewPosType;
      try
      { 
        CastedNewPosType=gstcUpCastNumericType(PosType,NewPosType,Par);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot cast " + pp(NewPosType) + " to " + pp(PosType) + "(while typechecking " + pp(Par) + " in " + pp(ProcTerm));
      }

      NewPars.push_front(Par);
      CastedPosTypeList.push_front(CastedNewPosType);
    }
    NewPars=reverse(NewPars);
    NewPosTypeList=reverse(CastedPosTypeList);

    std::pair<bool,aterm_list>p=gstcAdjustNotInferredList(NewPosTypeList,ParList);
    PosTypeList=p.second;

    if (!p.first)
    {
      throw mcrl2::runtime_error("no " + msg + " " + pp(Name) + "with type " + pp(NewPosTypeList) + " is declared (while typechecking " + pp(ProcTerm) + ")");
    }
  }

  if (gstcIsNotInferredL(PosTypeList))
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + pp(Name));
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

static aterm_appl gstcTraverseActProcVarConstP(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl ProcTerm)
{
  size_t n = ProcTerm.size();
  if (n==0)
  {
    return ProcTerm;
  }

  //Here the code for short-hand assignments begins.
  if (gsIsIdAssignment(ProcTerm))
  {
    mCRL2log(debug) << "typechecking a process call with short-hand assignments " << ProcTerm << "" << std::endl;
    core::identifier_string Name=aterm_cast<core::identifier_string>(ProcTerm[0]);
    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.processes.find(Name);
    
    if (j==context.processes.end())
    {
      throw mcrl2::runtime_error("process " + pp(Name) + " not declared");
    }

    term_list <sort_expression_list> ParList=j->second;
    // Put the assignments into a table
    std::map <aterm_appl,aterm_appl> As;    // variable -> expression (both untyped, still)
    const aterm_list &al=aterm_cast<aterm_list>(ProcTerm[1]);
    for (aterm_list::const_iterator l=al.begin(); l!=al.end(); ++l)
    {
      const aterm_appl a= aterm_cast<aterm_appl>(*l);
      const std::map <aterm_appl,aterm_appl>::const_iterator i=As.find(aterm_cast<aterm_appl>(a[0]));
      if (i!=As.end()) // An assignment of the shape x:=t already exists, this is not OK.
      {
        throw mcrl2::runtime_error("Double assignment to variable " + pp(aterm_cast<aterm_appl>(a[0])) + " (detected assigned values are " + pp(i->second) + " and " + pp(aterm_cast<aterm_appl>(a[1])) + ")");
      }
      As[aterm_cast<aterm_appl>(a[0])]=aterm_cast<aterm_appl>(a[1]);
    }

    {
      // Now filter the ParList to contain only the processes with parameters in this process call with assignments
      term_list <sort_expression_list> NewParList;
      assert(!ParList.empty());
      aterm_appl Culprit; // Variable used for more intelligible error messages.
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        sort_expression_list Par=ParList.front();

        // get the formal parameter names
        variable_list FormalPars=body.proc_pars[gsMakeProcVarId(Name,Par)];
        // we only need the names of the parameters, not the types
        aterm_list FormalParNames;
        for (; !FormalPars.empty(); FormalPars=FormalPars.tail())
        {
          FormalParNames.push_front(aterm_cast<aterm_appl>(ATAgetFirst(FormalPars)[0]));
        }

        aterm_list As_lhss;
        for(std::map <aterm_appl,aterm_appl> ::const_iterator i=As.begin(); i!=As.end(); ++i)
        {
          As_lhss.push_front(i->first);
        }
        aterm_list l=list_minus(As_lhss,FormalParNames);
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
      }
      if (!ParList.tail().empty())
      {
        throw mcrl2::runtime_error("ambiguous process " + pp(Name) + " containing all assignments in " + pp(ProcTerm) + ".");
      }
    }

    // get the formal parameter names
    aterm_list ActualPars;
    aterm_list FormalPars=aterm_cast<aterm_list>(body.proc_pars[gsMakeProcVarId(Name,ATLgetFirst(ParList))]);
    {
      // we only need the names of the parameters, not the types
      for (aterm_list l=FormalPars; !l.empty(); l= l.tail())
      {
        aterm_appl FormalParName=aterm_cast<aterm_appl>(ATAgetFirst(l)[0]);
        aterm_appl ActualPar;
        const std::map <aterm_appl,aterm_appl> ::const_iterator i=As.find(FormalParName);
        if (i==As.end())  // Not found.
        {
          ActualPar=gsMakeId(FormalParName);
        }
        else
        { 
          ActualPar=i->second;
        }
        ActualPars.push_front(ActualPar);
      }
      ActualPars=reverse(ActualPars);
    }

    aterm_appl TypeCheckedProcTerm;
    try 
    {
      TypeCheckedProcTerm=gstcRewrActProc(Vars, gsMakeParamId(Name,ActualPars));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking the process call with short-hand assignments " + pp(ProcTerm));
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
      As[aterm_cast<variable>(form_par[0])]=gsMakeDataVarIdInit(form_par,act_par);
    }

    aterm_list TypedAssignments;
    for (aterm_list l=aterm_cast<aterm_list>(ProcTerm[1]); !l.empty(); l=l.tail())
    {
      aterm_appl a=ATAgetFirst(l);
      const std::map <aterm_appl,aterm_appl> ::const_iterator i=As.find(aterm_cast<aterm_appl>(a[0]));
      if (i==As.end())
      {
        continue;
      }
      TypedAssignments.push_front(i->second);
    }
    TypedAssignments=reverse(TypedAssignments);

    return gsMakeProcessAssignment(aterm_cast<aterm_appl>(TypeCheckedProcTerm[0]),TypedAssignments);
  }
  //Here the section dealing with assignments ends.

  if (gsIsParamId(ProcTerm))
  {
    return gstcRewrActProc(Vars,ProcTerm);
  }

  if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm) ||
      gsIsRename(ProcTerm) || gsIsComm(ProcTerm) || gsIsAllow(ProcTerm))
  {

    //block & hide
    if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm))
    {
      const std::string msg=gsIsBlock(ProcTerm)?"Blocking":"Hiding";
      core::identifier_string_list ActList=aterm_cast<core::identifier_string_list>(ProcTerm[0]);
      if (ActList.empty())
      {
        mCRL2log(warning) << msg << " empty set of actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }

      std::set<aterm_appl> Acts;
      for (; !ActList.empty(); ActList=ActList.tail())
      {
        core::identifier_string Act=ActList.front();

        //Actions must be declared
        if (context.actions.count(Act)==0)
        {
          throw mcrl2::runtime_error(msg + " an undefined action " + pp(Act) + " (typechecking " + pp(ProcTerm) + ")");
        }
        if (!Acts.insert(Act).second)  // The action was already in the set.
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

      std::set<aterm_appl> ActsFrom;

      for (; !RenList.empty(); RenList=RenList.tail())
      {
        aterm_appl Ren=ATAgetFirst(RenList);
        core::identifier_string ActFrom=aterm_cast<core::identifier_string>(Ren[0]);
        aterm_appl ActTo=aterm_cast<aterm_appl>(Ren[1]);

        if (ActFrom==ActTo)
        {
          mCRL2log(warning) << "renaming action " << pp(ActFrom) << " into itself (typechecking " << pp(ProcTerm) << ")" << std::endl;
        }

        //Actions must be declared and of the same types
        aterm_list TypesFrom,TypesTo;
        const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j_from=context.actions.find(ActFrom);
        if (j_from==context.actions.end())
        {
          throw mcrl2::runtime_error("renaming an undefined action " + pp(ActFrom) + " (typechecking " + pp(ProcTerm) + ")");
        }
        TypesFrom=j_from->second;
        const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j_to=context.actions.find(ActFrom);
        if (j_to==context.actions.end())
        {
          throw mcrl2::runtime_error("renaming into an undefined action " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
        }
        TypesTo=j_to->second;

        TypesTo=gstcTypeListsIntersect(TypesFrom,TypesTo);
        if (TypesTo.empty())
        {
          throw mcrl2::runtime_error("renaming action " + pp(ActFrom) + " into action " + pp(ActTo) + ": these two have no common type (typechecking " + pp(ProcTerm) + ")");
        }

        if (!ActsFrom.insert(ActFrom).second) // The element was already in the set.
        {
          throw mcrl2::runtime_error("renaming action " + pp(ActFrom) + " twice (typechecking " + pp(ProcTerm) + ")");
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
          core::identifier_string ActTo=aterm_cast<core::identifier_string>(Comm[1]);

          if (MActFrom.size()==1)
          {
            throw mcrl2::runtime_error("using synchronization as renaming/hiding of action " + pp(MActFrom.front()) + " into " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
          }

          //Actions must be declared
          aterm_list ResTypes;

          if (!gsIsNil(ActTo))
          {
            const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.actions.find(ActTo);
            if (j==context.actions.end())
            {
              throw mcrl2::runtime_error("synchronizing to an undefined action " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
            }
            ResTypes=j->second;
          }

          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            core::identifier_string Act=aterm_cast<core::identifier_string>(MActFrom.front());
            const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=context.actions.find(Act);
            term_list<sort_expression_list> Types;
            if (j==context.actions.end())
            {
              throw mcrl2::runtime_error("synchronizing an undefined action " + pp(Act) + " in (multi)action " + pp(MActFrom) + " (typechecking " + pp(ProcTerm) + ")");
            }
            Types=j->second;
            ResTypes=gstcTypeListsIntersect(ResTypes,Types);
            if (ResTypes.empty())
            {
              throw mcrl2::runtime_error("synchronizing action " + pp(Act) + " from (multi)action " + pp(BackupMActFrom) + " into action " + pp(ActTo) + ": these have no common type (typechecking " + pp(ProcTerm) + "), ResTypes: " + pp(ResTypes));
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
            core::identifier_string Act=aterm_cast<core::identifier_string>(MAct.front());
            if (context.actions.count(Act)==0)
            {
              throw mcrl2::runtime_error("allowing an undefined action " + pp(Act) + " in (multi)action " + pp(MAct) + " (typechecking " + pp(ProcTerm) + ")");
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
    return ProcTerm.set_argument(NewProc,1);
  }

  if (gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
      gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm))
  {
    aterm_appl NewLeft=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));
    aterm_appl NewRight=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    aterm_appl a=ProcTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
    return a;
  }

  if (gsIsAtTime(ProcTerm))
  {
    aterm_appl NewProc=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));
    data_expression Time=aterm_cast<data_expression>(ProcTerm[1]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Time,gstcExpandNumTypesDown(sort_real::real_()));

    aterm_appl temp;
    if (!gstcTypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real");
      }
    }

    return gsMakeAtTime(NewProc,Time);
  }

  if (gsIsIfThen(ProcTerm))
  {
    data_expression Cond=aterm_cast<aterm_appl>(ProcTerm[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Cond,sort_bool::bool_());
    aterm_appl NewThen=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    return gsMakeIfThen(Cond,NewThen);
  }

  if (gsIsIfThenElse(ProcTerm))
  {
    data_expression Cond=aterm_cast<aterm_appl>(ProcTerm[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Cond,sort_bool::bool_());
    aterm_appl NewThen=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    aterm_appl NewElse=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[2]));
    return gsMakeIfThenElse(Cond,NewThen,NewElse);
  }

  if (gsIsSum(ProcTerm))
  {
    std::map<core::identifier_string,sort_expression> CopyVars;
    CopyVars=Vars;

    std::map<core::identifier_string,sort_expression> NewVars;
    try 
    {
      gstcAddVars2Table(CopyVars,aterm_cast<aterm_list>(ProcTerm[0]),NewVars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pp(ProcTerm));
    }
    aterm_appl NewProc;
    try
    {
      NewProc=gstcTraverseActProcVarConstP(NewVars,aterm_cast<aterm_appl>(ProcTerm[1]));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pp(ProcTerm));
    }
    return ProcTerm.set_argument(NewProc,1);
  }

  throw mcrl2::runtime_error("Internal error. Process " + pp(ProcTerm) + " fails to match known processes.");
}

static aterm_appl gstcTraversePBESVarConstPB(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl PBESTerm)
{

  if (gsIsDataExpr(PBESTerm))
  {
    data_expression d(PBESTerm);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,d,sort_bool::bool_());
    return d;
  }

  if (gsIsPBESTrue(PBESTerm) || gsIsPBESFalse(PBESTerm))
  {
    return PBESTerm;
  }

  if (gsIsPBESNot(PBESTerm))
  {
    aterm_appl NewArg=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm[0]));
    return PBESTerm.set_argument(NewArg,0);
  }

  if (gsIsPBESAnd(PBESTerm) || gsIsPBESOr(PBESTerm) || gsIsPBESImp(PBESTerm))
  {
    aterm_appl NewLeft=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm[0]));
    aterm_appl NewRight=gstcTraversePBESVarConstPB(Vars,aterm_cast<aterm_appl>(PBESTerm[1]));
    return PBESTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
  }

  if (gsIsPBESForall(PBESTerm)||gsIsPBESExists(PBESTerm))
  {
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    std::map<core::identifier_string,sort_expression> NewVars;
    try
    {
      gstcAddVars2Table(CopyVars,aterm_cast<aterm_list>(PBESTerm[0]),NewVars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pp(PBESTerm));
    }
    aterm_appl NewPBES;
    try
    {
      NewPBES=gstcTraversePBESVarConstPB(NewVars,aterm_cast<aterm_appl>(PBESTerm[1]));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pp(PBESTerm));
    }
    return PBESTerm.set_argument(NewPBES,1);
  }

  if (gsIsPropVarInst(PBESTerm))
  {
    return gstcRewrActProc(Vars, PBESTerm, true);
  }

  throw mcrl2::runtime_error("Internal error. The pbes term " + pp(PBESTerm) + " fails to match any known form in typechecking case analysis");
}

static sort_expression gstcTraverseVarConsTypeD(
  const std::map<core::identifier_string,sort_expression> &DeclaredVars,
  const std::map<core::identifier_string,sort_expression> &AllowedVars,
  data_expression &DataTerm,
  sort_expression PosType,
  std::map<core::identifier_string,sort_expression> &FreeVars,
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

  mCRL2log(debug) << "gstcTraverseVarConsTypeD: DataTerm " << pp(DataTerm) <<
              " with PosType " << pp(PosType) << "" << std::endl;

  if (is_abstraction(DataTerm))
  {
    //The variable declaration of a binder should have at least 1 declaration
    if (aterm_cast<aterm_list>(DataTerm[1]).size()==0)
    {
      throw mcrl2::runtime_error("binder " + pp(DataTerm) + " should have at least one declared variable");
    }

    aterm_appl BindingOperator = aterm_cast<aterm_appl>((DataTerm)[0]);
    std::map<core::identifier_string,sort_expression> CopyAllowedVars(AllowedVars);
    std::map<core::identifier_string,sort_expression> CopyDeclaredVars(DeclaredVars);

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
      }

      aterm_appl NewType=aterm_cast<aterm_appl>(VarDecl[1]);
      variable_list VarList=make_list<variable>(VarDecl);
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
      
      data_expression Data((DataTerm)[2]);

      aterm_appl ResType;
      try
      {
        ResType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nthe condition or count of a set/bag comprehension " + pp(DataTerm) + " cannot be determined");
      }
      aterm_appl temp;
      if (gstcTypeMatchA(sort_bool::bool_(),ResType,temp))
      {
        NewType=sort_set::set_(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeSetComp(), 0);
      }
      else if (gstcTypeMatchA(sort_nat::nat(),ResType,temp))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeBagComp(), 0);
      }
      else if (gstcTypeMatchA(sort_pos::pos(),ResType,temp))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = DataTerm.set_argument(gsMakeBagComp(), 0);
        Data=make_application(sort_nat::cnat(),Data);
      }
      else
      {
        throw mcrl2::runtime_error("the condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " + pp(ResType));
      }

      if (!gstcTypeMatchA(NewType,PosType,NewType))
      {
        throw mcrl2::runtime_error("a set or bag comprehension of type " + pp(aterm_cast<aterm_appl>(VarDecl[1])) + " does not match possible type " +
                            pp(PosType) + " (while typechecking " + pp(DataTerm) + ")");
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
      variable_list VarList=aterm_cast<variable_list>((DataTerm)[1]);
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);

      data_expression Data=aterm_cast<aterm_appl>((DataTerm)[2]);
      aterm_appl temp;
      if (!gstcTypeMatchA(sort_bool::bool_(),PosType,temp))
      {
        throw mcrl2::runtime_error("the type of an exist/forall for " + pp(DataTerm) + " cannot be determined");
      }
      aterm_appl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,sort_bool::bool_(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      if (!gstcTypeMatchA(sort_bool::bool_(),NewType,temp))
      {
        throw mcrl2::runtime_error("the type of an exist/forall for " + pp(DataTerm) + " cannot be determined");
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
      variable_list VarList=aterm_cast<variable_list>((DataTerm)[1]);
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
      
      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
      
      aterm_list ArgTypes=gstcGetVarTypes(VarList);
      aterm_appl NewType;
      if (!gstcUnArrowProd(ArgTypes,PosType,NewType))
      {
        throw mcrl2::runtime_error("no functions with arguments " + pp(ArgTypes) + " among " + pp(PosType) + " (while typechecking " + pp(DataTerm) + ")");
      }
      data_expression Data=aterm_cast<aterm_appl>((DataTerm)[2]);

      try 
      {
        NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,NewType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error &e)
      {
        gstcRemoveVars(FreeVars,VarList);
        throw e;
      }
      mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << pp(Data) << "" << std::endl;

      gstcRemoveVars(FreeVars,VarList);
      
      DataTerm=DataTerm.set_argument(Data,2);
      return gsMakeSortArrow(ArgTypes,NewType);
    }
  }

  if (is_where_clause(DataTerm))
  {
    variable_list WhereVarList;
    aterm_list NewWhereList;
    for (aterm_list WhereList=aterm_cast<aterm_list>((DataTerm)[1]); !WhereList.empty(); WhereList=WhereList.tail())
    {
      aterm_appl WhereElem=ATAgetFirst(WhereList);
      data_expression WhereTerm=aterm_cast<aterm_appl>(WhereElem[1]);
      aterm_appl WhereType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,WhereTerm,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

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

    std::map<core::identifier_string,sort_expression> CopyAllowedVars(AllowedVars);
    std::map<core::identifier_string,sort_expression> CopyDeclaredVars(DeclaredVars);

    variable_list VarList=reverse(WhereVarList);
    std::map<core::identifier_string,sort_expression> NewAllowedVars;
    gstcAddVars2Table(CopyAllowedVars,VarList,NewAllowedVars);
    
    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    gstcAddVars2Table(CopyDeclaredVars,VarList,NewDeclaredVars);
    
    data_expression Data=aterm_cast<aterm_appl>((DataTerm)[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

    gstcRemoveVars(FreeVars,VarList);
    
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
        aterm_appl Type;
        if (!gstcUnList(PosType,Type))
        {
          throw mcrl2::runtime_error("not possible to cast list to " + pp(PosType) + " (while typechecking " + pp(data_expression_list(appl.begin(),appl.end())) + ")");
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only!
        data_expression_list NewArguments;
        bool Type_is_stable=true;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i; 
          aterm_appl Type0;
          try
          {
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,false);
          }
          catch (mcrl2::runtime_error &e)
          {
            // Try again, but now without Type as the suggestion.
            // If this does not work, it will be caught in the second pass below.
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,data::unknown_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
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
        aterm_appl Type;
        if (!gstcUnSet(PosType,Type))
        {
          throw mcrl2::runtime_error("not possible to cast set to " + pp(PosType) + " (while typechecking " + pp(data_expression_list(appl.begin(),appl.end())) + ")");
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only (which will be NewType)!
        bool NewTypeDefined=false;
        aterm_appl NewType;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          aterm_appl Type0;
          try
          {
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument) + ")");
          }

          aterm_appl OldNewType=NewType;
          if (!NewTypeDefined)
          {
            NewType=Type0;
            NewTypeDefined=true;
          }
          else
          {
            aterm_appl temp;
            if (!gstcMaximumType(NewType,Type0,temp))
            {
              throw mcrl2::runtime_error("Set contains incompatible elements of sorts " + pp(OldNewType) + " and " + pp(Type0) + " (while typechecking " + pp(Argument));
            }
            NewType=temp;
            NewTypeDefined=true;
          }
        }

        // Type is now the most generic type to be used.
        assert(Type.defined());
        assert(NewTypeDefined);
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        aterm_list NewArguments;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          aterm_appl Type0;
          try
          {
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument) + ")");
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
        aterm_appl Type;
        if (!gstcUnBag(PosType,Type))
        {
          throw mcrl2::runtime_error("not possible to cast bag to " + pp(PosType) + "(while typechecking " + 
                                      pp(data_expression_list(appl.begin(),appl.end())) + ")");
        }

        // aterm_list OldArguments=Arguments;

        //First time to determine the common type only!
        aterm_appl NewType;
        bool NewTypeDefined=false;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          aterm_appl Type0;
          try 
          {
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument0) + ")");
          }
          aterm_appl Type1;
          try 
          { 
            Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast number to " + pp(sort_nat::nat()) + " (while typechecking " + pp(Argument1) + ")");
            }
            else
            {
              throw e;
            }
          }
          aterm_appl OldNewType=NewType;
          if (!NewTypeDefined)
          {
            NewType=Type0;
            NewTypeDefined=true;
          }
          else
          {
            aterm_appl temp;
            if (!gstcMaximumType(NewType,Type0,temp))
            {
              throw mcrl2::runtime_error("Bag contains incompatible elements of sorts " + pp(OldNewType) + " and " + pp(Type0) + " (while typechecking " + pp(Argument0) + ")");
            }

            NewType=temp;
            NewTypeDefined=true;
          }
        }
        assert(Type.defined());
        assert(NewTypeDefined);
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        aterm_list NewArguments;
        for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          aterm_appl Type0;
          try 
          {
            Type0=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast element to " + pp(Type) + " (while typechecking " + pp(Argument0) + ")");
            }
            else 
            {
              throw e;
            }
          }
          aterm_appl Type1;
          try
          { 
            Type1=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nnot possible to cast number to " + pp(sort_nat::nat()) + " (while typechecking " + pp(Argument1) + ")");
            }
            else
            {
              throw e;
            }
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
    sort_expression_list NewArgumentTypes;
    aterm_list NewArguments;

    for (data_expression_list::const_iterator i=appl.begin(); i!=appl.end(); ++i)
    {
      data_expression Arg= *i;
      aterm_appl Type=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,data::unknown_sort(),FreeVars,false,warn_upcasting,print_cast_error);
      assert(Type.defined());
      NewArguments.push_front(Arg);
      NewArgumentTypes.push_front(Type);
    }
    aterm_list Arguments=reverse(NewArguments);
    sort_expression_list ArgumentTypes=reverse(NewArgumentTypes);

    //function
    data_expression Data=appl.head();
    aterm_appl NewType;
    try
    {
      NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                      Data,
                      data::unknown_sort() /* gsMakeSortArrow(ArgumentTypes,PosType) */,
                      FreeVars,false,nArguments,warn_upcasting,print_cast_error);
      mCRL2log(debug) << "Result of gstcTraverseVarConsTypeD: DataTerm " << pp(Data) << "" << std::endl;
    }
    catch (mcrl2::runtime_error &e)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else if (gsIsOpId(Data)||gsIsDataVarId(Data))
      {
        gstcErrorMsgCannotCast(aterm_cast<aterm_appl>(Data[1]),Arguments,ArgumentTypes,e.what());
      }
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while trying to cast " + 
                            pp(application(Data,aterm_cast<data_expression_list>(Arguments))) + " to type " + pp(PosType));
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
      }
      //arguments again
      sort_expression_list NewArgumentTypes;
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
          try 
          { 
            Type=gstcUpCastNumericType(NeededType,Type,Arg,warn_upcasting);
          }
          catch (mcrl2::runtime_error &e)
          {
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on [1] " << pp(Arg) << ", Type: " << pp(Type) << ", Needed type: " << pp(NeededType) << "" << std::endl;
          sort_expression NewArgType;
          if (!gstcTypeMatchA(NeededType,Type,NewArgType))
          {
            if (!gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type),NewArgType))
            {
              NewArgType=NeededType;
            }
          }
          try 
          {
            NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nneeded type " + pp(NeededType) + " does not match possible type "
                            + pp(Type) + " (while typechecking " + pp(Arg) + " in " + pp(DataTerm) + ")");
          }
          mCRL2log(debug) << "Result of Doing again gstcTraverseVarConsTypeD: DataTerm " << pp(Arg) << "" << std::endl;
          Type=NewArgType;
        }
        NewArguments.push_front(Arg);
        NewArgumentTypes.push_front(Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    //the function again
    try 
    {
      NewType=gstcTraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                        Data,gsMakeSortArrow(ArgumentTypes,PosType),
                                        FreeVars,strictly_ambiguous,nArguments,warn_upcasting,print_cast_error);

      mCRL2log(debug) << "Result of gstcTraverseVarConsTypeDN: DataTerm " << pp(Data) << "" << std::endl;
    }
    catch (mcrl2::runtime_error &e)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      else
      {
        gstcErrorMsgCannotCast(aterm_cast<aterm_appl>(Data[1]),Arguments,ArgumentTypes,e.what());
      }
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while trying to cast " + 
                   pp(application(Data,aterm_cast<data_expression_list>(Arguments))) + " to type " + pp(PosType));
    }

    mCRL2log(debug) << "Arguments once more: Arguments " << pp(Arguments) << ", ArgumentTypes: " <<
                pp(ArgumentTypes) << ", NewType: " << pp(NewType) << "" << std::endl;

    //and the arguments once more
    if (gsIsSortArrow(gstcUnwindType(NewType)))
    {
      aterm_list NeededArgumentTypes=aterm_cast<aterm_list>(gstcUnwindType(NewType)[0]);
      sort_expression_list NewArgumentTypes;
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
          try 
          {
            Type=gstcUpCastNumericType(NeededType,Type,Arg,warn_upcasting);
          }
          catch (mcrl2::runtime_error &e)
          {
          }
        }
        if (!gstcEqTypesA(NeededType,Type))
        {
          mCRL2log(debug) << "Doing again on [2] " << pp(Arg) << ", Type: " << pp(Type) << ", Needed type: " << pp(NeededType) << "" << std::endl;
          sort_expression NewArgType;
          if (!gstcTypeMatchA(NeededType,Type,NewArgType))
          {
            if (!gstcTypeMatchA(NeededType,gstcExpandNumTypesUp(Type),NewArgType))
            {
              NewArgType=NeededType;
            }
          }
          try 
          {
            NewArgType=gstcTraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nneeded type " + pp(NeededType) + " does not match possible type "
                            + pp(Type) + " (while typechecking " + pp(Arg) + " in " + pp(DataTerm) + ")");
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

    aterm_appl temp_type;
    if (!gstcUnArrowProd(ArgumentTypes,NewType,temp_type))
    {
      throw mcrl2::runtime_error("Fail to properly type " + pp(DataTerm));
    }
    if (gstcHasUnknown(temp_type))
    {
      throw mcrl2::runtime_error("Fail to properly type " + pp(DataTerm));
    }
    return temp_type;
  }

  if (gsIsId(DataTerm)||gsIsOpId(DataTerm)||gsIsDataVarId(DataTerm))
  {
    core::identifier_string Name=aterm_cast<core::identifier_string>((DataTerm)[0]);
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

      aterm_appl temp;
      if (gstcTypeMatchA(Sort,PosType,temp))
      {
        return Sort;
      }

      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=gstcUpCastNumericType(PosType,Sort,DataTerm,warn_upcasting);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast number " + pp(DataTerm) + " to type " + pp(PosType));
      }
      return CastedNewType;
    }

    std::map<core::identifier_string,sort_expression>::const_iterator it=DeclaredVars.find(Name);
    if (it!=DeclaredVars.end())
    {
      sort_expression Type=it->second;
      mCRL2log(debug) << "Recognised declared variable " << pp(Name) << ", Type: " << pp(Type) << "" << std::endl;
      DataTerm=gsMakeDataVarId(Name,Type);

      if (AllowedVars.count(Name)==0)
      {
        throw mcrl2::runtime_error("variable " + pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side");
      }

      aterm_appl NewType;
      if (gstcTypeMatchA(Type,PosType,NewType))
      {
        Type=NewType;
      }
      else
      {
        //upcasting
        aterm_appl CastedNewType;
        try 
        {
          CastedNewType=gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        }
        catch (mcrl2::runtime_error &e)
        {
          if (print_cast_error)
          { 
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast variable " + pp(DataTerm) + " to type " + pp(PosType));
          }
          else
          {
            throw e;
          }
        }

        Type=CastedNewType;
      }

      //Add to free variables list
      FreeVars[Name]=Type;
      return Type;
    }

    std::map<core::identifier_string,sort_expression>::const_iterator i=context.constants.find(Name);
    if (i!=context.constants.end())
    {
      sort_expression Type=i->second;
      aterm_appl NewType;
      if (gstcTypeMatchA(Type,PosType,NewType))
      {
        Type=NewType;
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        // The type cannot be unified. Try upcasting the type.
        DataTerm=gsMakeOpId(Name,Type);
        try
        {
          return gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nno constant " + pp(DataTerm) + " with type " + pp(PosType));
        }
      }
    }

    std::map<core::identifier_string,sort_expression_list>::const_iterator j=gssystem.constants.find(Name);
    if (j!=gssystem.constants.end())
    {
      sort_expression_list ParList=j->second;
      sort_expression_list NewParList;
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        aterm_appl Par=ATAgetFirst(ParList);
        aterm_appl result;
        if (gstcTypeMatchA(Par,PosType,result))
        {
          NewParList.push_front(result);
        }
      }
      ParList=reverse(NewParList);
      if (ParList.empty())
      {
        throw mcrl2::runtime_error("no system constant " + pp(DataTerm) + " with type " + pp(PosType));
      }

      if (ParList.size()==1)
      {
        sort_expression Type=ParList.front();
        DataTerm=gsMakeOpId(Name,Type);
        return Type;
      }
      else
      {
        DataTerm=gsMakeOpId(Name,data::unknown_sort());
        return data::unknown_sort();
      }
    }

    const std::map <core::identifier_string,sort_expression_list>::const_iterator j_context=context.functions.find(Name);
    const std::map <core::identifier_string,sort_expression_list>::const_iterator j_gssystem=gssystem.functions.find(Name);

    sort_expression_list ParList;
    if (j_context==context.functions.end())   
    {
      if (j_gssystem!=gssystem.functions.end())        
      { 
        ParList=j_gssystem->second; // The function only occurs in the system.
      }
      else // None are defined.
      {
        throw mcrl2::runtime_error("unknown operation " + pp(Name));
      }
    }
    else if (j_gssystem==gssystem.functions.end())          
    {
      ParList=j_context->second; // only the context sorts are defined.
    }
    else  // Both are defined.
    {
      ParList=j_gssystem->second+j_context->second;
    }


    if (ParList.size()==1)
    {
      aterm_appl Type=ATAgetFirst(ParList);
      DataTerm=gsMakeOpId(Name,Type);
      try
      {
        return gstcUpCastNumericType(PosType,Type,DataTerm,warn_upcasting);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nno constant " + pp(DataTerm) + " with type " + pp(PosType));
      }
    }
    else
    {
      return gstcTraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strictly_ambiguous, std::string::npos, warn_upcasting,print_cast_error);
    }
  }

  throw mcrl2::runtime_error("Internal type checking error: " + pp(DataTerm) + " does not match any type checking case." );
}

static sort_expression gstcTraverseVarConsTypeDN(
  const std::map<core::identifier_string,sort_expression> &DeclaredVars,
  const std::map<core::identifier_string,sort_expression> &AllowedVars,
  data_expression &DataTerm,
  sort_expression PosType,
  std::map<core::identifier_string,sort_expression> &FreeVars,
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
    core::identifier_string Name=aterm_cast<core::identifier_string>((DataTerm)[0]);
    bool variable_=false;
    bool TypeADefined=false;
    sort_expression TypeA;
    std::map<core::identifier_string,sort_expression>::const_iterator i=DeclaredVars.find(Name);
    if (i!=DeclaredVars.end())
    {
      TypeA=i->second;
      TypeADefined=true;
      const sort_expression Type1(gstcUnwindType(TypeA));
      if (is_function_sort(Type1)?(function_sort(Type1).domain().size()==nFactPars):(nFactPars==0))
      {
        variable_=true;
        if (AllowedVars.count(Name)==0)
        {
          throw mcrl2::runtime_error("variable " + pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side");
        }

        //Add to free variables list
        FreeVars[Name]=TypeA;
      }
      else
      {
        TypeADefined=false;
      }
    }

    sort_expression_list ParList;
    if (nFactPars==0)
    {
      std::map<core::identifier_string,sort_expression>::const_iterator i=DeclaredVars.find(Name);
      if (i!=DeclaredVars.end()) 
      {
        TypeA=i->second;
        TypeADefined=true;
        aterm_appl temp;
        if (!gstcTypeMatchA(TypeA,PosType,temp))
        {
          throw mcrl2::runtime_error("the type " + pp(TypeA) + " of variable " + pp(Name)
                          + " is incompatible with " + pp(PosType) + " (typechecking " + pp(DataTerm) + ")");
        }
        DataTerm=gsMakeDataVarId(Name,TypeA);
        return TypeA;
      }
      else 
      {
        std::map<core::identifier_string,sort_expression>::const_iterator i=context.constants.find(Name);
        if (i!=context.constants.end())
        {
          TypeA=i->second;
          TypeADefined=true;
          aterm_appl temp;
          if (!gstcTypeMatchA(TypeA,PosType,temp))
          {
            throw mcrl2::runtime_error("the type " + pp(TypeA) + " of constant " + pp(Name)
                            + " is incompatible with " + pp(PosType) + " (typechecking " + pp(DataTerm) + ")");
          }
          DataTerm=gsMakeOpId(Name,TypeA);
          return TypeA;
        }
        else
        {
          std::map<core::identifier_string,sort_expression_list>::const_iterator j=gssystem.constants.find(Name);
          
          if (j!=gssystem.constants.end())
          {
            ParList=j->second; 
            if (ParList.size()==1)
            {
              aterm_appl Type1=ATAgetFirst(ParList);
              DataTerm=gsMakeOpId(Name,Type1);
              return Type1;
            }
            else
            {
              DataTerm=gsMakeOpId(Name,data::unknown_sort());
              throw  mcrl2::runtime_error("ambiguous system constant " + pp(Name));
            }
          }
          else
          {
            throw mcrl2::runtime_error("unknown constant " + pp(Name));
          }
        }
      }
    }

    if (TypeADefined)
    {
      ParList=make_list<sort_expression>(gstcUnwindType(TypeA));
    }
    else
    {
      const std::map <core::identifier_string,sort_expression_list>::const_iterator j_context=context.functions.find(Name);
      const std::map <core::identifier_string,sort_expression_list>::const_iterator j_gssystem=gssystem.functions.find(Name);

      if (j_context==context.functions.end())   
      {
        if (j_gssystem!=gssystem.functions.end())        
        { 
          ParList=j_gssystem->second; // The function only occurs in the system.
        }
        else // None are defined.
        {
          if (nFactPars!=std::string::npos)
          {
            throw mcrl2::runtime_error("unknown operation " + pp(Name) + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":""));
          }
          else
          {
            throw mcrl2::runtime_error("unknown operation " + pp(Name));
          }
        }
      }
      else if (j_gssystem==gssystem.functions.end())          
      {
        ParList=j_context->second; // only the context sorts are defined.
      }
      else  // Both are defined.
      {
        ParList=j_gssystem->second+j_context->second;
      }
    }

    mCRL2log(debug) << "Possible types for Op/Var " << pp(Name) << " with " << nFactPars <<
                " argument are (ParList: " << pp(ParList) << "; PosType: " << pp(PosType) << ")" << std::endl;

    sort_expression_list CandidateParList=ParList;

    {
      // filter ParList keeping only functions A_0#...#A_nFactPars->A
      sort_expression_list NewParList;
      if (nFactPars!=std::string::npos)
      {
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
      sort_expression_list BackupParList=ParList;
      NewParList=sort_expression_list();
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        aterm_appl Par=ATAgetFirst(ParList);
        aterm_appl result;
        if (gstcTypeMatchA(Par,PosType,result))
        {
          NewParList=aterm_cast<sort_expression_list>(ATinsertUnique(NewParList,result));
        }
      }
      NewParList=reverse(NewParList);

      mCRL2log(debug) << "Possible matches w/o casting for Op/Var " << pp(Name) << " with "<< nFactPars << 
                " argument are (ParList: " << pp(NewParList) << "; PosType: " << pp(PosType) << "" << std::endl;

      if (NewParList.empty())
      {
        //Ok, this looks like a type error. We are not that strict.
        //Pos can be Nat, or even Int...
        //So lets make PosType more liberal
        //We change every Pos to NotInferred(Pos,Nat,Int)...
        //and get the list. Then we take the min of the list.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying casting for Op " << pp(Name) << " with " << nFactPars << " argument (ParList: " << 
                            pp(ParList) << "; PosType: " << pp(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesUp(PosType);
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          aterm_appl Par=ATAgetFirst(ParList);
          aterm_appl result;
          if (gstcTypeMatchA(Par,PosType,result))
          {
            NewParList=aterm_cast<sort_expression_list>(ATinsertUnique(NewParList,result));
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is [1] " << pp(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=make_list<sort_expression>(gstcMinType(NewParList));
        }
      }

      if (NewParList.empty())
      {
        //Ok, casting of the arguments did not help.
        //Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

        ParList=BackupParList;
        mCRL2log(debug) << "Trying result casting for Op " << pp(Name) << " with " << nFactPars << " argument (ParList: " << 
                              "; PosType: " << pp(PosType) << "" << std::endl;
        PosType=gstcExpandNumTypesDown(gstcExpandNumTypesUp(PosType));
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          aterm_appl Par=ATAgetFirst(ParList);
          aterm_appl result;
          if (gstcTypeMatchA(Par,PosType,result))
          {
            NewParList=aterm_cast<sort_expression_list>(ATinsertUnique(NewParList,result));
          }
        }
        NewParList=reverse(NewParList);
        mCRL2log(debug) << "The result of casting is [2]" << pp(NewParList) << "" << std::endl;
        if (NewParList.size()>1)
        {
          NewParList=make_list<sort_expression>(gstcMinType(NewParList));
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
      }
      else
      {
        throw mcrl2::runtime_error("unknown operation/variable " + pp(Name) + " that matches type " + pp(PosType));
      }
    }

    if (ParList.size()==1)
    {
      // replace PossibleSorts by a single possibility.
      aterm_appl Type=ATAgetFirst(ParList);

      aterm_appl OldType=Type;
      bool result=true;
      assert(Type.defined());
      if (gstcHasUnknown(Type))
      {
        aterm_appl new_type;
        result=gstcTypeMatchA(Type,PosType,new_type);
        Type=new_type;
      }

      if (gstcHasUnknown(Type) && gsIsOpId(DataTerm))
      {
        aterm_appl new_type;
        result=gstcTypeMatchA(Type,aterm_cast<aterm_appl>((DataTerm)[1]),new_type);
        Type=new_type;
      }

      if (!result)
      {
        throw mcrl2::runtime_error("fail to match sort " + pp(OldType) + " with " + pp(PosType));
      }

      if (static_cast<aterm_appl>(data::detail::if_symbol())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing if matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchIf(Type,NewType))
        {
          throw mcrl2::runtime_error("the function if has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
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
        aterm_appl NewType;
        if (!gstcMatchEqNeqComparison(Type,NewType))
        {
          throw mcrl2::runtime_error("the function " + pp(aterm_cast<aterm_appl>((DataTerm)[0])) + " has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::cons_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing |> matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchListOpCons(Type,NewType))
        {
          throw mcrl2::runtime_error("the function |> has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::snoc_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing <| matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchListOpSnoc(Type,NewType))
        {
          throw mcrl2::runtime_error("the function <| has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::concat_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing ++ matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchListOpConcat(Type,NewType))
        {
          throw mcrl2::runtime_error("the function ++ has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::element_at_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing @ matching Type " << pp(Type) << ", PosType " << pp(PosType) << ", DataTerm: " << pp(DataTerm) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchListOpEltAt(Type,NewType))
        {
          throw mcrl2::runtime_error("the function @ has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::head_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_list::rhead_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {R,L}head matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;

        aterm_appl NewType;
        if (!gstcMatchListOpHead(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {R,L}head has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::tail_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_list::rtail_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {R,L}tail matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchListOpTail(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {R,L}tail has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::set2bag_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing Set2Bag matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchSetOpSet2Bag(Type,NewType))
        {
          throw mcrl2::runtime_error("the function Set2Bag has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_list::in_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {List,Set,Bag} matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchListSetBagOpIn(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {List,Set,Bag}In has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_set::union_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_set::difference_name())==aterm_cast<aterm_appl>((DataTerm)[0])||
          static_cast<aterm_appl>(sort_set::intersection_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing {Set,Bag}{Union,Difference,Intersect} matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchSetBagOpUnionDiffIntersect(Type,NewType))
        {
          throw mcrl2::runtime_error("the function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_set::complement_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing SetCompl matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchSetOpSetCompl(Type,NewType))
        {
          throw mcrl2::runtime_error("the function SetCompl has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::bag2set_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing Bag2Set matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchBagOpBag2Set(Type,NewType))
        {
          throw mcrl2::runtime_error("the function Bag2Set has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }

      if (static_cast<aterm_appl>(sort_bag::count_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing BagCount matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchBagOpBagCount(Type,NewType))
        {
          throw mcrl2::runtime_error("the function BagCount has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }


      if (static_cast<aterm_appl>(data::function_update_name())==aterm_cast<aterm_appl>((DataTerm)[0]))
      {
        mCRL2log(debug) << "Doing FuncUpdate matching Type " << pp(Type) << ", PosType " << pp(PosType) << "" << std::endl;
        aterm_appl NewType;
        if (!gstcMatchFuncUpdate(Type,NewType))
        {
          throw mcrl2::runtime_error("function update has incompatible argument types " + pp(Type) + " (while typechecking " + pp(DataTerm) + ")");
        }
        Type=NewType;
      }


      Type=replace_possible_sorts(Type); // Set the type to one option in possible sorts, if there are more options.
      DataTerm=gsMakeOpId(Name,Type);
      if (variable_)
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
        }
        else
        {
          throw mcrl2::runtime_error("ambiguous operation " + pp(Name));
        }
      }
      else
      {
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
      bool found_solution=true;
      aterm_appl r;
      try 
      {
        r=gstcUpCastNumericType(*i,Type,Par,warn_upcasting);
      }
      catch (mcrl2::runtime_error &e)
      { 
        found_solution=false;
      }
      if (found_solution)
      {
        return r;
      }
    }
    throw mcrl2::runtime_error("cannot transform " + pp(Type) + " to a number."); 
  }

  if (warn_upcasting && gsIsOpId(Par) && gsIsNumericString(gsATermAppl2String(aterm_cast<aterm_appl>((Par)[0]))))
  {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  aterm_appl temp;
  if (gstcTypeMatchA(NeededType,sort_pos::pos(),temp))
  {
    if (gstcTypeMatchA(Type,sort_pos::pos(),temp))
    {
      return sort_pos::pos();
    }
  }

  // Try Upcasting to Nat
  if (gstcTypeMatchA(NeededType,sort_nat::nat(),temp))
  {
    if (gstcTypeMatchA(Type,sort_pos::pos(),temp))
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
    if (gstcTypeMatchA(Type,sort_nat::nat(),temp))
    {
      return sort_nat::nat();
    }
  }

  // Try Upcasting to Int
  if (gstcTypeMatchA(NeededType,sort_int::int_(),temp))
  {
    if (gstcTypeMatchA(Type,sort_pos::pos(),temp))
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
    if (gstcTypeMatchA(Type,sort_nat::nat(),temp))
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
    if (gstcTypeMatchA(Type,sort_int::int_(),temp))
    {
      return sort_int::int_();
    }
  }

  // Try Upcasting to Real
  if (gstcTypeMatchA(NeededType,sort_real::real_(),temp))
  {
    if (gstcTypeMatchA(Type,sort_pos::pos(),temp))
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
    if (gstcTypeMatchA(Type,sort_nat::nat(),temp))
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
    if (gstcTypeMatchA(Type,sort_int::int_(),temp))
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
    if (gstcTypeMatchA(Type,sort_real::real_(),temp))
    {
      return sort_real::real_();
    }
  }

  throw mcrl2::runtime_error("Upcasting " + pp(Type) + " to a number fails");
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

static std::pair<bool,aterm_list> gstcAdjustNotInferredList(aterm_list PosTypeList, aterm_list TypeListList)
{
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  //if PosTypeList has only normal types -- check if it is in TypeListList,
  //if so return PosTypeList, otherwise return false.
  if (!gstcIsNotInferredL(PosTypeList))
  {
    if (gstcInTypesL(PosTypeList,TypeListList))
    {
      return std::pair<bool,aterm_list>(true,PosTypeList);
    }
    else
    {
      return std::pair<bool,aterm_list>(false, aterm_list());
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
    return std::pair<bool,aterm_list>(false, aterm_list());
  }
  if (NewTypeListList.size()==1)
  {
    return std::pair<bool,aterm_list>(true,ATLgetFirst(NewTypeListList));
  }

  // otherwise return not inferred.
  return std::pair<bool,aterm_list>(true,gstcGetNotInferredList(reverse(NewTypeListList)));
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

static bool gstcTypeMatchA(const aterm_appl &Type_in, const aterm_appl &PosType_in, aterm_appl &result)
{
  // Checks if Type and PosType match by instantiating unknown sorts.
  // It returns the matching instantiation of Type in result. If matching fails,
  // it returns false, otherwise true.

  aterm_appl Type=Type_in;
  aterm_appl PosType=PosType_in;

  mCRL2log(debug) << "gstcTypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) << " " << std::endl;

  if (data::is_unknown_sort(Type))
  {
    result=PosType;
    return true;
  }
  if (data::is_unknown_sort(PosType) || gstcEqTypesA(Type,PosType))
  {
    result=Type;
    return true;
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

      aterm_appl new_type;
      if (gstcTypeMatchA(Type,NewPosType,new_type))
      {
        NewPosType=new_type;
        mCRL2log(debug) << "Match gstcTypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) <<
                    " New Type: " << pp(NewPosType) << "" << std::endl;
        NewTypeList.push_front(NewPosType);
      }
    }
    if (NewTypeList.empty())
    {
      mCRL2log(debug) << "No match gstcTypeMatchA Type: " << pp(Type) << ";    PosType: " << pp(PosType) << " " << std::endl;
      return false;
    }

    if (NewTypeList.tail().empty())
    {
      result=ATAgetFirst(NewTypeList);
      return true;
    }

    result=multiple_possible_sorts(sort_expression_list(reverse(NewTypeList)));
    return true;
  }

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
        return false;
      }
      aterm_appl Res;
      if (!gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),Res))
      {
        return false;
      }
      result=sort_list::list(sort_expression(Res));
      return true;
    }

    if (gsIsSortSet(ConsType))
    {
      if (!sort_set::is_set(sort_expression(PosType)))
      {
        return false;
      }
      else
      {
        aterm_appl Res;
        if (!gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),Res))
        {
          return false;
        }
        result=sort_set::set_(sort_expression(Res));
        return true;
      }
    }

    if (gsIsSortBag(ConsType))
    {
      if (!sort_bag::is_bag(sort_expression(PosType)))
      {
        return false;
      }
      else
      {
        aterm_appl Res;
        if (!gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),Res))
        {
          return false;
        }
        result=sort_bag::bag(sort_expression(Res));
        return true;
      }
    }
  }

  if (gsIsSortArrow(Type))
  {
    if (!gsIsSortArrow(PosType))
    {
      return false;
    }
    else
    {
      aterm_list ArgTypes;
      if (!gstcTypeMatchL(aterm_cast<aterm_list>(Type[0]),aterm_cast<aterm_list>(PosType[0]),ArgTypes))
      {
        return false;
      }
      aterm_appl ResType;
      if (!gstcTypeMatchA(aterm_cast<aterm_appl>(Type[1]),aterm_cast<aterm_appl>(PosType[1]),ResType))
      {
        return false;
      }
      result=gsMakeSortArrow(ArgTypes,ResType);
      return true;
    }
  }

  return false;
}

static bool gstcTypeMatchL(const aterm_list &TypeList, const aterm_list &PosTypeList, aterm_list &result)
{
  mCRL2log(debug) << "gstcTypeMatchL TypeList: " << pp(TypeList) << ";    PosTypeList: " <<
              pp(PosTypeList) << "" << std::endl;

  if (TypeList.size()!=PosTypeList.size())
  {
    return false;
  }

  aterm_list Result;
  aterm_list::const_iterator j=PosTypeList.begin();
  for (aterm_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i, ++j)
  {
    aterm_appl Type;
    if (!gstcTypeMatchA(aterm_cast<aterm_appl>(*i),aterm_cast<aterm_appl>(*j),Type))
    {
      return false;
    }
    Result.push_front(Type);
  }
  result=reverse(Result);
  return true;
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
    std::map<core::identifier_string,sort_expression>::const_iterator i=context.defined_sorts.find(aterm_cast<core::identifier_string>(Type[0]));
    if (i==context.defined_sorts.end())
    {
      return Type;
    }
    return gstcUnwindType(i->second);
  }

  return Type;
}

static bool gstcUnSet(aterm_appl PosType, aterm_appl &result) 
{
  //select Set(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_set::is_set(sort_expression(PosType)))
  {
    result=aterm_cast<aterm_appl>(PosType[1]);
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
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
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

static bool gstcUnBag(aterm_appl PosType, aterm_appl &result) 
{
  //select Bag(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_bag::is_bag(sort_expression(PosType)))
  {
    result=aterm_cast<aterm_appl>(PosType[1]);
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
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
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

static bool gstcUnList(aterm_appl PosType, aterm_appl &result)
{
  //select List(Type), elements, return their list of arguments.
  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (sort_list::is_list(sort_expression(PosType)))
  {
    result=aterm_cast<aterm_appl>(PosType[1]);
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
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
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

static bool gstcUnArrowProd(aterm_list ArgTypes, aterm_appl PosType, aterm_appl &result)
{
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //result is TypeX if unique, the set of TypeX as NotInferred if many. 
  //return true if successful, otherwise false.

  if (gsIsSortId(PosType))
  {
    PosType=gstcUnwindType(PosType);
  }
  if (gsIsSortArrow(PosType))
  {
    aterm_list PosArgTypes=aterm_cast<aterm_list>(PosType[0]);

    if (PosArgTypes.size()!=ArgTypes.size())
    {
      return false;
    }
    aterm_list temp;
    if (gstcTypeMatchL(PosArgTypes,ArgTypes,temp))
    {
      result=aterm_cast<aterm_appl>(PosType[1]);
      return true;
    }
  }
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
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
        aterm_list temp_list;
        if (gstcTypeMatchL(PosArgTypes,ArgTypes,temp_list))
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
    result=multiple_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

static sort_expression_list gstcGetVarTypes(variable_list VarDecls)
{
  sort_expression_list Result;
  for (variable_list::const_iterator i=VarDecls.begin(); i!=VarDecls.end(); ++i)
  {
    Result.push_front(i->sort());
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

static bool gstcIsNumericType(const sort_expression &Type)
{
  //returns true if Type is Bool,Pos,Nat,Int or Real
  //otherwise return fase
  if (data::is_unknown_sort(Type))
  {
    return false;
  }
  return (bool)(sort_bool::is_bool(Type)||
                  sort_pos::is_pos(Type)||
                  sort_nat::is_nat(Type)||
                  sort_int::is_int(Type)||
                  sort_real::is_real(Type));
}

static bool gstcMaximumType(const aterm_appl &Type1, const aterm_appl &Type2, aterm_appl &result)
{
  // if Type1 is convertible into Type2 or vice versa, the most general
  // of these types are returned in result. If no conversion is possible false is returned
  // and result is not changed. Conversions only take place between numerical types
  if (gstcEqTypesA(Type1,Type2))
  {
    result=Type1;
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(Type1)))
  {
    result=Type2;
    return true;
  }
  if (data::is_unknown_sort(data::sort_expression(Type2)))
  {
    result=Type1;
    return true;
  }
  if (gstcEqTypesA(Type1,sort_real::real_()))
  {
    if (gstcEqTypesA(Type2,sort_int::int_()))
    {
      result=Type1;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_nat::nat()))
    {
      result=Type1;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (gstcEqTypesA(Type1,sort_int::int_()))
  {
    if (gstcEqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_nat::nat()))
    {
      result=Type1;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (gstcEqTypesA(Type1,sort_nat::nat()))
  {
    if (gstcEqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_int::int_()))
    {
      result=Type2;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (gstcEqTypesA(Type1,sort_pos::pos()))
  {
    if (gstcEqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_int::int_()))
    {
      result=Type2;
      return true;
    }
    if (gstcEqTypesA(Type2,sort_nat::nat()))
    {
      result=Type2;
      return true;
    }
    return false;
  }
  return false;
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
  aterm_list Args;
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

static bool gstcUnifyMinType(const aterm_appl &Type1, const aterm_appl &Type2, aterm_appl &result)
{
  //Find the minimal type that Unifies the 2. If not possible, return false.
  if (!gstcTypeMatchA(Type1,Type2,result))
  {
    if (!gstcTypeMatchA(Type1,gstcExpandNumTypesUp(Type2),result))
    {
      if (!gstcTypeMatchA(Type2,gstcExpandNumTypesUp(Type1),result))
      {
        mCRL2log(debug) << "gstcUnifyMinType: No match: Type1 " << pp(Type1) << "; Type2 " << pp(Type2) << "; " << std::endl;
        return false;
      }
    }
  }

  if (gsIsSortsPossible(result))
  {
    result=ATAgetFirst(aterm_cast<aterm_list>(result[0]));
  }
  mCRL2log(debug) << "gstcUnifyMinType: Type1 " << pp(Type1) << "; Type2 " << pp(Type2) << "; Res: " << pp(result) << "" << std::endl;
  return true;
}

static bool gstcMatchIf(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  assert(gsIsSortArrow(type));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  assert((Args.size()==3));
  Args=Args.tail();

  if (!gstcUnifyMinType(Res,ATAgetFirst(Args),Res))
  {
    return false;
  }
  Args=Args.tail();
  if (!gstcUnifyMinType(Res,ATAgetFirst(Args),Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(sort_bool::bool_(),Res,Res),Res);
  return true;
}

static bool gstcMatchEqNeqComparison(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));
  aterm_appl Arg1=ATAgetFirst(Args);
  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);

  aterm_appl Arg;
  if (!gstcUnifyMinType(Arg1,Arg2,Arg))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg,Arg),sort_bool::bool_());
  return true;
}

static bool gstcMatchListOpCons(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(gstcUnwindType(Res))));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
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

  aterm_appl new_result;
  if (!gstcUnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!gstcUnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Res,static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
  return true;
}

static bool gstcMatchListOpSnoc(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
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

  aterm_appl new_result;
  if (!gstcUnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!gstcUnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res))),Res),sort_list::list(sort_expression(Res)));
  return true;
}

static bool gstcMatchListOpConcat(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
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

  aterm_appl new_result;
  if (!gstcUnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!gstcUnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res))),
       static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),sort_list::list(sort_expression(Res)));
  return true;
}

static bool gstcMatchListOpEltAt(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));

  aterm_appl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  assert(sort_list::is_list(sort_expression(Arg1)));
  Arg1=aterm_cast<aterm_appl>(Arg1[1]);

  aterm_appl new_result;
  if (!gstcUnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }
  Res=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res))),
               static_cast<aterm_appl>(sort_nat::nat())),Res);
  return true;
}

static bool gstcMatchListOpHead(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));
  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  aterm_appl new_result;
  if (!gstcUnifyMinType(Res,Arg,new_result))
  {
    return false;
  }
  Res=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),Res);
  return true;
}

static bool gstcMatchListOpTail(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_list::is_list(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));
  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_list::is_list(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  aterm_appl new_result;
  if (!gstcUnifyMinType(Res,Arg,new_result))
  {
    return false;
  }
  Res=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_list::list(sort_expression(Res)))),
                   sort_list::list(sort_expression(Res)));
  return true;
}

//Sets
static bool gstcMatchSetOpSet2Bag(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));

  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_bag::is_bag(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);

  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));

  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  aterm_appl new_result;
  if (!gstcUnifyMinType(Arg,Res,new_result))
  {
    return false;
  }
  Arg=new_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_set::set_(sort_expression(Arg)))),
                  sort_bag::bag(sort_expression(Arg)));
  return true;
}

static bool gstcMatchListSetBagOpIn(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  //assert(gsIsBool(aterm_cast<aterm_appl>(type[1])));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
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

  aterm_appl Arg;
  if (!gstcUnifyMinType(Arg1,Arg2s,Arg))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg,Arg2.set_argument(Arg,1)),sort_bool::bool_());
  return true;
}

static bool gstcMatchSetBagOpUnionDiffIntersect(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Res))||sort_bag::is_bag(sort_expression(Res)));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==2));

  aterm_appl Arg1=ATAgetFirst(Args);
  if (gsIsSortId(Arg1))
  {
    Arg1=gstcUnwindType(Arg1);
  }
  if (gstcIsNumericType(Arg1))
  {
    result=type;
    return true;
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
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Arg2))||sort_bag::is_bag(sort_expression(Arg2)));

  aterm_appl temp_result;
  if (!gstcUnifyMinType(Res,Arg1,temp_result))
  {
    return false;
  }

  if (!gstcUnifyMinType(temp_result,Arg2,Res))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Res,Res),Res);
  return true;
}

static bool gstcMatchSetOpSetCompl(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  if (gstcIsNumericType(Res))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));

  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  if (gstcIsNumericType(Arg))
  {
    result=type;
    return true;
  }
  assert(sort_set::is_set(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  aterm_appl temp_result;
  if (!gstcUnifyMinType(Res,Arg,temp_result))
  {
    return false;
  }
  Res=temp_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_set::set_(sort_expression(Res)))),sort_set::set_(sort_expression(Res)));
  return true;
}

//Bags
static bool gstcMatchBagOpBag2Set(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));

  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  if (gsIsSortId(Res))
  {
    Res=gstcUnwindType(Res);
  }
  assert(sort_set::is_set(sort_expression(Res)));
  Res=aterm_cast<aterm_appl>(Res[1]);

  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==1));

  aterm_appl Arg=ATAgetFirst(Args);
  if (gsIsSortId(Arg))
  {
    Arg=gstcUnwindType(Arg);
  }
  assert(sort_bag::is_bag(sort_expression(Arg)));
  Arg=aterm_cast<aterm_appl>(Arg[1]);

  aterm_appl temp_result;
  if (!gstcUnifyMinType(Arg,Res,temp_result))
  {
    return false;
  }
  Arg=temp_result;

  result=gsMakeSortArrow(make_list<aterm>(static_cast<aterm_appl>(sort_bag::bag(sort_expression(Arg)))),sort_set::set_(sort_expression(Arg)));
  return true;
}

static bool gstcMatchBagOpBagCount(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  //If the second argument is not a Bag, don't match

  if (!gsIsSortArrow(type))
  {
    result=type;
    return true;
  }
  //assert(gsIsNat(aterm_cast<aterm_appl>(type[1])));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  if (!(Args.size()==2))
  {
    result=type;
    return true;
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
    result=type;
    return true;
  }
  Arg2=aterm_cast<aterm_appl>(Arg2[1]);

  aterm_appl Arg;
  if (!gstcUnifyMinType(Arg1,Arg2,Arg))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg,static_cast<aterm_appl>(sort_bag::bag(sort_expression(Arg)))),sort_nat::nat());
  return true;
}


static bool gstcMatchFuncUpdate(const aterm_appl &type, aterm_appl &result)
{
  //tries to sort out the types of FuncUpdate ((A->B)xAxB->(A->B))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrow(type));
  aterm_list Args=aterm_cast<aterm_list>(type[0]);
  assert((Args.size()==3));
  aterm_appl Arg1=ATAgetFirst(Args);
  assert(gsIsSortArrow(Arg1));
  Args=Args.tail();
  aterm_appl Arg2=ATAgetFirst(Args);
  Args=Args.tail();
  aterm_appl Arg3=ATAgetFirst(Args);
  aterm_appl Res=aterm_cast<aterm_appl>(type[1]);
  assert(gsIsSortArrow(Res));

  aterm_appl temp_result;
  if (!gstcUnifyMinType(Arg1,Res,temp_result))
  {
    return false;
  }
  Arg1=temp_result;

  // determine A and B from Arg1:
  aterm_list LA=aterm_cast<aterm_list>(Arg1[0]);
  assert((LA.size()==1));
  aterm_appl A=ATAgetFirst(LA);
  aterm_appl B=aterm_cast<aterm_appl>(Arg1[1]);

  if (!gstcUnifyMinType(A,Arg2,temp_result))
  {
    return false;
  }
  if (!gstcUnifyMinType(B,Arg3,temp_result))
  {
    return false;
  }

  result=gsMakeSortArrow(make_list<aterm>(Arg1,A,B),Arg1);
  return true;
}

static void gstcErrorMsgCannotCast(aterm_appl CandidateType, aterm_list Arguments, aterm_list ArgumentTypes,std::string previous_reason)
{
  //prints more information about impossible cast.
  //at this point we know that Arguments cannot be cast to CandidateType. We need to find out why and print.
  assert(Arguments.size()==ArgumentTypes.size());

  /* aterm_list CandidateList;
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
    sort_expression_list NewCurrentCandidateList;
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
      aterm_appl temp;
      if (gstcTypeMatchA(ATAgetFirst(k),NeededType,temp))
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
      throw mcrl2::runtime_error(previous_reason + "\nthis is, for instance, because cannot cast " + pp(ATAgetFirst(l)) + " to type " + pp(Sort));
    }
  } */
  throw mcrl2::runtime_error(previous_reason + "\nNo attempt is made to explain this further");
}

//===================================
// Type checking modal formulas
//===================================

static aterm_appl gstcTraverseStateFrm(const std::map<core::identifier_string,sort_expression> &Vars, 
                                       const std::map<core::identifier_string,sort_expression_list> &StateVars, 
                                       aterm_appl StateFrm)
{
  mCRL2log(debug) << "gstcTraverseStateFrm: " + pp(StateFrm) + "" << std::endl;

  if (gsIsStateTrue(StateFrm) || gsIsStateFalse(StateFrm) || gsIsStateDelay(StateFrm) || gsIsStateYaled(StateFrm))
  {
    return StateFrm;
  }

  if (gsIsStateNot(StateFrm))
  {
    aterm_appl NewArg=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[0]));
    return StateFrm.set_argument(NewArg,0);
  }

  if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm) || gsIsStateImp(StateFrm))
  {
    aterm_appl NewArg1=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[0]));
    aterm_appl NewArg2=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    return StateFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm))
  {
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    aterm_list VarList=aterm_cast<aterm_list>(StateFrm[0]);
    std::map<core::identifier_string,sort_expression> NewVars;
    gstcAddVars2Table(CopyVars,VarList,NewVars);

    aterm_appl NewArg2=gstcTraverseStateFrm(NewVars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    return StateFrm.set_argument(NewArg2,1);
  }

  if (gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm))
  {
    aterm_appl RegFrm=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(StateFrm[0]));
    aterm_appl NewArg2=gstcTraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    return StateFrm.set_argument(RegFrm,0).set_argument(NewArg2,1);
  }

  if (gsIsStateDelayTimed(StateFrm) || gsIsStateYaledTimed(StateFrm))
  {
    data_expression Time=aterm_cast<aterm_appl>(StateFrm[0]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Time,gstcExpandNumTypesDown(sort_real::real_()));

    aterm_appl temp;
    if (!gstcTypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
      }
    }
    return StateFrm.set_argument(Time,0);
  }

  if (gsIsStateVar(StateFrm))
  {
    core::identifier_string StateVarName=aterm_cast<core::identifier_string>(StateFrm[0]);
    std::map<core::identifier_string,sort_expression_list>::const_iterator i=StateVars.find(StateVarName);
    if (i==StateVars.end())
    {
      throw mcrl2::runtime_error("undefined state variable " + pp(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
    }
    sort_expression_list TypeList=i->second;

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
      aterm_appl NewParType;
      try 
      { 
        NewParType=gstcTraverseVarConsTypeD(Vars,Vars,Par,gstcExpandNumTypesDown(ParType));
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm));
      }

      aterm_appl temp;
      if (!gstcTypeMatchA(ParType,NewParType,temp))
      {
        //upcasting
        try
        {
          NewParType=gstcUpCastNumericType(ParType,NewParType,Par);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(Par) + " to type " + pp(ParType) + " (typechecking state formula " + pp(StateFrm) + ")");
        }
      }

      r.push_front(Par);
    }

    return StateFrm.set_argument(reverse(r),1);

  }

  if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm))
  {
    std::map<core::identifier_string,sort_expression_list> CopyStateVars(StateVars);

    // Make the new state variable:
    std::map<core::identifier_string,sort_expression> FormPars;
    aterm_list r;
    sort_expression_list t;
    for (aterm_list l=aterm_cast<aterm_list>(StateFrm[1]); !l.empty(); l=l.tail())
    {
      aterm_appl o=ATAgetFirst(l);

      core::identifier_string VarName=aterm_cast<core::identifier_string>(aterm_cast<aterm_appl>(o[0])[0]);
      if (FormPars.count(VarName)>0)
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

      FormPars[VarName]=VarType;

      data_expression VarInit=aterm_cast<aterm_appl>(o[1]);
      aterm_appl VarInitType;
      try
      {
        VarInitType=gstcTraverseVarConsTypeD(Vars,Vars,VarInit,gstcExpandNumTypesDown(VarType));
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
      }

      aterm_appl temp;
      if (!gstcTypeMatchA(VarType,VarInitType,temp))
      {
        //upcasting
        try 
        {
          VarInitType=gstcUpCastNumericType(VarType,VarInitType,VarInit);
        }
        catch (mcrl2::runtime_error &e)
        { 
          throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
        }
      }

      r.push_front(o.set_argument(VarInit,1));
      t.push_front(VarType);
    }

    StateFrm=StateFrm.set_argument(reverse(r),1);
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);
    CopyVars.insert(FormPars.begin(),FormPars.end());
    

    CopyStateVars[aterm_cast<core::identifier_string>(StateFrm[0])]=reverse(t);
    
    aterm_appl NewArg;
    try
    {
      NewArg=gstcTraverseStateFrm(CopyVars,CopyStateVars,aterm_cast<aterm_appl>(StateFrm[2]));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pp(StateFrm));
    }
    return StateFrm.set_argument(NewArg,2);
  }

  if (gsIsDataExpr(StateFrm))
  {
    data_expression d(StateFrm);
    aterm_appl Type=gstcTraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    return d;
  }

  throw mcrl2::runtime_error("Internal error. The state formula " + pp(StateFrm) + " fails to match any known form in typechecking case analysis");
}

static aterm_appl gstcTraverseRegFrm(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl RegFrm)
{
  mCRL2log(debug) << "gstcTraverseRegFrm: " + pp(RegFrm) + "" << std::endl;
  if (gsIsRegNil(RegFrm))
  {
    return RegFrm;
  }

  if (gsIsRegSeq(RegFrm) || gsIsRegAlt(RegFrm))
  {
    aterm_appl NewArg1=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[0]));
    aterm_appl NewArg2=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[1]));
    return RegFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsRegTrans(RegFrm) || gsIsRegTransOrNil(RegFrm))
  {
    aterm_appl NewArg=gstcTraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[0]));
    return RegFrm.set_argument(NewArg,0);
  }

  if (gsIsActFrm(RegFrm))
  {
    return gstcTraverseActFrm(Vars, RegFrm);
  }

  throw mcrl2::runtime_error("Internal error. The regularformula " + pp(RegFrm) + " fails to match any known form in typechecking case analysis");
}

static aterm_appl gstcTraverseActFrm(const std::map<core::identifier_string,sort_expression> &Vars, aterm_appl ActFrm)
{
  mCRL2log(debug) << "gstcTraverseActFrm: " + pp(ActFrm) + "" << std::endl;

  if (gsIsActTrue(ActFrm) || gsIsActFalse(ActFrm))
  {
    return ActFrm;
  }

  if (gsIsActNot(ActFrm))
  {
    aterm_appl NewArg=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));
    return ActFrm.set_argument(NewArg,0);
  }

  if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm) || gsIsActImp(ActFrm))
  {
    aterm_appl NewArg1=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));
    aterm_appl NewArg2=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[1]));
    return ActFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm))
  {
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    aterm_list VarList=aterm_cast<aterm_list>(ActFrm[0]);
    std::map<core::identifier_string,sort_expression> NewVars;
    gstcAddVars2Table(CopyVars,VarList,NewVars);
    
    aterm_appl NewArg2=gstcTraverseActFrm(NewVars,aterm_cast<aterm_appl>(ActFrm[1]));
    return ActFrm.set_argument(NewArg2,1);
  }

  if (gsIsActAt(ActFrm))
  {
    aterm_appl NewArg1=gstcTraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));

    data_expression Time=aterm_cast<aterm_appl>(ActFrm[1]);
    aterm_appl NewType=gstcTraverseVarConsTypeD(Vars,Vars,Time,gstcExpandNumTypesDown(sort_real::real_()));

    aterm_appl temp;
    if (!gstcTypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=gstcUpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking action formula " + pp(ActFrm) + ")");
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
      r.push_front(o);
    }
    return ActFrm.set_argument(reverse(r),0);
  }

  if (gsIsDataExpr(ActFrm))
  {
    data_expression d(ActFrm);
    aterm_appl Type=gstcTraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    return ActFrm;
  }

  throw mcrl2::runtime_error("Internal error. The action formula " + pp(ActFrm) + " fails to match any known form in typechecking case analysis");
}

}
}
