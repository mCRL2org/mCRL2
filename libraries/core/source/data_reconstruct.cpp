// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct.cpp
//
//This file contains the implementation of data reconstruction. I.e.
//it attempts to revert the data implementation.

#include <cassert>
#include <aterm2.h>
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/table.h"

#include "mcrl2/core/data_reconstruct.h"
#include "mcrl2/core/detail/data_implementation_concrete.h"
#include "mcrl2/core/detail/data_common.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"
#include "mcrl2/core/messaging.h"

#include "workarounds.h" // DECL_A

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2 {
  namespace core {

// declarations
// ----------------------------------------------

// t_reconstruct_context can contain all context information that is needed
// for reconstructing the data declaration part of the specification in two
// passes.
// sorts_table: lookup table containing all sorts in the specification
// sort_constructors: for each sort the constructors for that sort
// num_sort_contructors: num_sort_constructors[sort] == |sort_constructors[sort]|
// sort_mappings: for each sort the (possibly relevant) mappings linked to that
//                sort
// num_sort_mappings: num_sort_mappings[sort] == |sort_mappings[sort]|
// sort_cons_equations: for each sort the equations induced by the constructors
//                      of that sort
// num_sort_cons_equations: num_sort_cons_equations[sort] == |sort_cons_equations[sort]|
// map_equations: for each mapping, the equations for that mapping.
// num_map_equations: num_map_equations[map] == |map_equations[map]|
// recognises: for each mapping f that is a recogniser, recognises[f] is the
//             constructor which is recognised by f.
// is_recognised_by: for each constructor c of a structured sort,
//                   is_recognised_by[c] is its recogniser.
// projects: for each mapping f that is a projection function, projects[f] is
//             the (constructor, argument) pair for which f is the projection
//             fuction.
typedef struct {
  atermpp::table                                     sorts_table;
  atermpp::table                                     system_defined_sorts;
  atermpp::table                                     composite_sorts;
  atermpp::table                                     non_composite_sorts;
  atermpp::map<ATermAppl, atermpp::indexed_set>      sort_constructors;
  atermpp::map<ATermAppl, int>                       num_sort_constructors;
  atermpp::map<ATermAppl, atermpp::indexed_set>      sort_mappings;
  atermpp::map<ATermAppl, int>                       num_sort_mappings;
  atermpp::map<ATermAppl, atermpp::indexed_set>      sort_cons_equations;
  atermpp::map<ATermAppl, int>                       num_sort_cons_equations;
  atermpp::map<ATermAppl, atermpp::indexed_set>      map_equations;
  atermpp::map<ATermAppl, int>                       num_map_equations;
  atermpp::map<ATermAppl, ATermAppl>                 recognises;
  atermpp::map<ATermAppl, ATermAppl>                 is_recognised_by;
  atermpp::map<std::pair<ATermAppl, int>, ATermAppl> projects;
} t_reconstruct_context;

//ret: The reconstructed version of Part.
static ATermAppl reconstruct_exprs_appl(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec = NULL);

//ret: The reconstructed version of Parts.
static ATermList reconstruct_exprs_list(ATermList Parts, ATermList* p_substs, const ATermAppl Spec = NULL);

//pre: Part is a data expression
//ret: The reconstructed version of Part.
static ATermAppl reconstruct_data_expr(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec, bool* recursive);

//pre: PosExpr is a data expression of sort Pos
//     Mult is a string representation of a positive number
//ret: a data expression of sort Pos, representing PosExpr * Mult,
//     only containing essential multiplications
static ATermAppl reconstruct_pos_mult(const ATermAppl PosExpr, char const* Mult);

//pre: data_expr is a set comprehension or a bag comprehension
//ret: data_expr if data_expr is not a set or bag enumeration,
//     the set or bag enumeration denoted by data_expr otherwise.
static ATermAppl reconstruct_set_bag_enum(ATermAppl data_expr);

//pre: data_expr is a data expression.
//ret: true if data_expr is of the form x == e_0 || x == e_1 || ... || x == e_n
//     false, otherwise
//post: if true is returned, enumeration contains [e_0,...,e_n], i.e. the
//      elements of the enumeration.
static bool reconstruct_set_enumeration(ATermAppl data_expr, ATermList* enumeration);

//pre: data_expr is a data expression
//ret: true if data_expr is of the form if(x == e_0, a_0, 0) + ... + if(x ==
//     e_n, a_n, 0)
//     false, otherwise.
//post: if true is returned, enumeration contains [e_0, a_0, ..., e_n, a_n]
static bool reconstruct_bag_enumeration(ATermAppl data_expr, ATermList* enumeration);

//pre: Part is a data expression
//ret: true if Part is a lambda expression (DataAppl(lambda@x, [...])),
//     false otherwise.
static inline bool is_lambda_expr(ATermAppl Part);

//pre: Part is a data expression
//ret: true if Part is an application of a lambda binder
//     (DataAppl(Binder(Lambda, [x_1, ..., x_n], e), [t_1, ..., t_n])
//     false otherwise.
static inline bool is_lambda_binder_application(ATermAppl data_expr);

//pre: Spec is a Specification
//ret: Spec from which system defined functions and equations
//     have been removed.
static ATermAppl remove_headers_without_binders_from_spec(ATermAppl Spec, ATermList* p_substs);

//post: p_data_decls constains the reconstructed version of the data
//      declarations in p_data_decls.
//      p_substs is extended with the substitutions induced by the context of
//      the data declarations.
static void reconstruct_data_decls(t_data_decls* p_data_decls, ATermList* p_substs);

//pre: spec is a specification
//ret: true if Spec contains a sort reference, false otherwise.
static bool has_sort_reference(const ATermAppl spec);

//pre: data_expr is a data expression
//ret: true if data_expr is of the form v_1 == w_1 && ... && v_n == w_n, where
//     v_i, w_j are data variables,
//     false, otherwise
static bool is_and_of_data_var_equalities(const ATermAppl data_expr);

//pre: data_expr is a data expression
//param: equal: the last inequality may also be <= instead of <
//ret: true if data_expr is of the form
//     x0 < y0 || (x0 == y0 && (x1 < y1 || (x1 == y1 && ... )))
static bool is_and_or_of_data_var_eq_lt(const ATermAppl data_expr, bool equal = false);

//ret: true if all elements of l are DataVarIds,
//     false otherwise
static bool is_list_of_data_var_ids(ATermList l);

//ret: list l from which all elements that occur in t are removed
static ATermList filter_table_elements_from_list(ATermList l, atermpp::table& t);

//pre: data_expr is a data expression
//ret: true if data_expr is a system defined function on lists
static bool is_list_operator(const ATermAppl data_expr);

//pre: data_expr is a data expression
//ret: true if data_expr is a system defined function on sets
static bool is_set_operator(const ATermAppl data_expr);

//pre: data_expr is a data expression
//ret: true if data_expr is a system defined function on bags
static bool is_bag_operator(const ATermAppl data_expr);

//pre: data_eqn is a data equation, sort_constructors holds the constructors
//     for all relevant sorts.
//ret: true if data_eqn has the form such that it belongs to a constructor
//     false otherwise.
static bool is_constructor_induced_equation(const ATermAppl data_eqn, atermpp::map<ATermAppl, atermpp::indexed_set>& sort_constructors);

//pre: data_eqn is a data equation
//ret: true if data_eqn has the form of an equation for a recogniser function.
static bool is_recogniser_equation(const ATermAppl data_eqn);

//pre: data_eqn is a data equation
//ret: true if data_eqn has the form of an equation for a projection function.
static bool is_projection_equation(const ATermAppl data_eqn);

//pre: op is an OpId, sort is a sort expression, sort_mappings contains mappings
//     of sort->indexed set of functions, map_equations contains mappings of
//     functions -> indexed set of equations. num_map_equations[s] ==
//     |map_equations[s]|
static void remove_mapping_not_list(const ATermAppl op,
                        const ATermAppl sort,
                        t_reconstruct_context* p_ctx);

//pre: generic_list is a list of terms which may be annotated with label @dummy
//ret: true if there is a match for term in generic_list
static bool find_match_in_list(const ATermAppl term, ATermList generic_list);

//pre: aterm_ann is a term which may have annotation with label @dummy,
//     aterm is a term, p_substs is a list with substitutions, induced by adding
//     equations for annotated terms with terms from term.
//ret: true if aterm_ann with p_substs applied, and aterm match.
static bool match(ATerm aterm_ann, ATerm aterm, ATermList* p_substs);

//similar to match
static bool match_appl(ATermAppl aterm_ann, const ATermAppl aterm, ATermList* p_substs);

//similar to match
static bool match_list(ATermList aterm_ann, ATermList aterm, ATermList* p_substs);

//post: p_substs is extended with substitutions for the implementations of the
//      lambda expressions in p_data_decls->data_eqns
static void calculate_lambda_expressions(const t_data_decls* p_data_decls, ATermList* p_substs);

//ret: true if data_expr is a system defined expression
static bool is_standard_function(const ATermAppl data_expr);

//ret: true if data_eqn is a system defined equation
static bool is_system_defined_equation(const ATermAppl data_eqn);

//pre: op_id is an operation identifier.
//ret: the sort for which op_id was introduced.
static ATermAppl get_linked_sort(const ATermAppl op_id);

//! \brief All sorts in p_data_decls are collected, and added to
//p_ctx->sorts_table. Furthermore, the other fields in p_ctx, that are indexed
//by sorts are initialised empty.
static void initialise_sorts(const t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx);

//! \brief Add all constructors in p_data_decls to the appropriate sortt
// in p_ctx->sort_constructors
static void initialise_sort_constructors(const t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx);

//! \brief Add all relevant functions to the appropriate sort in
//p_ctx->sort_mappings.
// Functions are relevant if it is:
// - EmptySet
// - EmptyBag
// - if
// - ==
// - !=
// - Belongs to the implementation of a list
// - Belongs to the implementation of a set
// - Belongs to the implementation of a bag
// - Might be a projection function
// - Might be a recogniser function
// As a side effect, add substitutions to *p_substs for:
// - Bag@k := Bag(S) (0<=k),
// - Set@k := Set(S) (0<=k)
// whenever a set comprehension or bag comprehension is seen.
static void initialise_mappings(const t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx,
                             ATermList* p_substs);

//! \brief Collect all data equations that are candidate for removal.
// First of all, these are the system defined equations, which are generated for
// all sorts. i.e. equations of the form:
// - x==x = true
// - x!=y = !(x==y)
// - if(b, x, x) = x
// - if(true, x, y) = x
// - if(false, x, y) = y
// where x,y are variables of the same sort, and b is a boolean variable.
// Second, these are the equations that belong to a contructor, or
// have the signature to belong to a recogniser,
// or have the signature to belong to a projection function.
// Third, if an equation is generated in the data implementation of a List, Set
// or Bag sort.
static void collect_data_equations(const t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx,
                             ATermList* p_substs);

//! \brief Remove sorts from data declarations.
// This removes:
// - system defined sorts,
// - structured sorts.
static void check_completeness(t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx);

//! \brief Determine what mappings are recogniser functions and projection
//functions.
// Functions that are not a recogniser or projection functions,
// and are not system defined are removed from p_ctx, because they should not be
// removed from the data declarations.
static void calculate_recognisers_and_projections(t_reconstruct_context* p_ctx);

//! \brief Flatten the indexed sets containing the data equations that should be
//removed (i.e. sort_cons_equations and map_equations) into p_data_equations,
//and the mappings that should be removed, i.e. sort_mappings into two
//hashtables. This is for performance reasons in removing superfluous elements
//from the data declarations.
static void flatten_mappings_and_equations(atermpp::table* p_mappings,
                                           atermpp::table* p_data_equations,
                                           t_reconstruct_context* p_ctx);

//!\brief Remove the constructors of which the sort is not in p_ctx->sorts from
//p_data_decls.
static void remove_constructors(t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx);

//!\brief Remove the mappings that are not in p_mappings, and that are not
//lambda binders from p_data_decls.
static void remove_mappings(t_data_decls* p_data_decls,
                            atermpp::table* p_mappings);

//!\brief Remove the data equations that are not in p_data_eqns, and of which
//the left hand side is not a lambda expression from p_data_decls.
static void remove_data_equations(t_data_decls* p_data_decls,
                                  atermpp::table* p_data_equations,
                                  ATermList* p_substs);

//!\brief Based on the information in p_ctx, compute the reconstructed
//declarations for the structured sorts. Note that this also introduces sort
//references, and adds substitutions to p_substs.
static void compute_sort_decls(t_data_decls* p_data_decls,
                             t_reconstruct_context* p_ctx,
                             ATermList* p_substs);

//!\brief Determine, based on the context, whether sort is a list, set of bag
//sort.
static bool is_set_bag_list_sort(ATermAppl sort, t_reconstruct_context* p_ctx);

//!\brief Get element sort of a set or bag sort from a set/bag comprehension
///\pre data_expr is a set or bag comprehension
static ATermAppl get_element_sort(ATermAppl data_expr);


// implementation
// ----------------------------------------------
ATerm reconstruct_exprs(ATerm Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) ||
    gsIsPBES(Spec) || gsIsDataSpec(Spec));

  ATerm Result;
  ATermList substs = ATmakeList0();
  if (ATgetType(Part) == AT_APPL) {
    Result = (ATerm) reconstruct_exprs_appl((ATermAppl) Part, &substs, Spec);
  } else { //(ATgetType(Part) == AT_LIST) {
    Result = (ATerm) reconstruct_exprs_list((ATermList) Part, &substs, Spec);
  }
  //  gsDebugMsg("Finished data reconstruction\n");
    return Result;
}

ATermAppl reconstruct_exprs_appl(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) ||
    gsIsPBES(Spec) || gsIsDataSpec(Spec));
  bool recursive = true;

  if (gsIsDataSpec(Part) && Spec != NULL) {
//    gsDebugMsg("Removing headers from specification\n");
    if (has_sort_reference(Part))
    {
      return Part;
    }
    Part = remove_headers_without_binders_from_spec(Part, p_substs);
  }

  // Perform substitutions
  if (gsIsSortExpr(Part)) {
    Part = gsSubstValues_Appl(*p_substs, Part, false);
  }

  if (gsIsDataExpr(Part)) {
    // Reconstruct Data Expressions
    Part = reconstruct_data_expr(Part, p_substs, Spec, &recursive);
  }

  if (recursive) {
    // Reconstruct expressions in the arguments of part
    AFun head = ATgetAFun(Part);
    int nr_args = ATgetArity(head);
    if (nr_args > 0) {
      DECL_A(args,ATerm,nr_args);
      for (int i = 0; i < nr_args; i++) {
        ATerm arg = ATgetArgument(Part, i);
        if (ATgetType(arg) == AT_APPL)
          args[i] = (ATerm) reconstruct_exprs_appl((ATermAppl) arg, p_substs, Spec);
        else //ATgetType(arg) == AT_LIST
          args[i] = (ATerm) reconstruct_exprs_list((ATermList) arg, p_substs, Spec);
      }
      Part = ATmakeApplArray(head, args);
      FREE_A(args);
    }
  }

  if (gsIsBinder(Part)) {
    ATermAppl binder = ATAgetArgument(Part, 0);
    if (gsIsSetComp(binder) || gsIsBagComp(binder)) {
      Part = reconstruct_set_bag_enum(Part);
    }
  }

  return Part;
}

ATermList reconstruct_exprs_list(ATermList Parts, ATermList* p_substs, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) ||
    gsIsPBES(Spec) || gsIsDataSpec(Spec));

  ATermList result = ATmakeList0();
  while (!ATisEmpty(Parts))
  {
    result = ATinsert(result, (ATerm)
      reconstruct_exprs_appl(ATAgetFirst(Parts), p_substs, Spec));
    Parts = ATgetNext(Parts);
  }
  return ATreverse(result);
}

ATermAppl reconstruct_data_expr(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec, bool* recursive)
{
  assert ((Spec == NULL) || gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) ||
    gsIsPBES(Spec) || gsIsDataSpec(Spec));
  assert(gsIsDataExpr(Part));

  if (gsIsDataExprBagComp(Part)) {
//    gsDebugMsg("Reconstructing implementation of bag comprehension\n", Part);
    //part is an implementation of a bag comprehension;
    //replace by a bag comprehension.
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl Body = ATAelementAt(Args, 0);
    ATermList Vars = ATmakeList0();
    ATermList BodySortDomain = ATLgetArgument(gsGetSort(Body), 0);
    ATermList Context = ATmakeList1((ATerm) Body);
    for(;!ATisEmpty(BodySortDomain); BodySortDomain = ATgetNext(BodySortDomain))
    {
      ATermAppl Var = gsMakeDataVarId(
                        gsFreshString2ATermAppl("x", (ATerm) Context, true),
                        ATAgetFirst(BodySortDomain));
      Context = ATinsert(Context, (ATerm) Var);
      Vars = ATinsert(Vars, (ATerm) Var);
    }
    Vars = ATreverse(Vars);
    Body = gsMakeDataAppl(Body, Vars);
    Part = gsMakeBinder(gsMakeBagComp(),Vars, Body);
  } else if (gsIsDataExprSetComp(Part)) {
//    gsDebugMsg("Reconstructing implementation of set comprehension\n");
    //part is an implementation of a set comprehension;
    //replace by a set comprehension.
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl Body = ATAelementAt(Args, 0);
    ATermList Vars = ATmakeList0();
    ATermList BodySortDomain = ATLgetArgument(gsGetSort(Body), 0);
    ATermList Context = ATmakeList1((ATerm) Body);
    for(;!ATisEmpty(BodySortDomain); BodySortDomain = ATgetNext(BodySortDomain))
    {
      ATermAppl Var = gsMakeDataVarId(
                        gsFreshString2ATermAppl("x", (ATerm) Context, true),
                        ATAgetFirst(BodySortDomain));
      Context = ATinsert(Context, (ATerm) Var);
      Vars = ATinsert(Vars, (ATerm) Var);
    }
    Vars = ATreverse(Vars);
    Body = gsMakeDataAppl(Body, Vars);
    Part = gsMakeBinder(gsMakeSetComp(), Vars, Body);
  } else if (gsIsDataExprForall(Part)) {
//    gsDebugMsg("Reconstructing implementation of universal quantification\n");
    //part is an implementation of a universal quantification;
    //replace by a universal quantification.
    ATermList Args = ATLgetArgument(Part, 1);
    assert(ATgetLength(Args) == 1);
    ATermAppl Body = ATAelementAt(Args, 0);
    ATermList BodySortDomain = ATLgetArgument(gsGetSort(Body), 0);
    ATermList Vars = ATmakeList0();
    ATermList Context = ATmakeList1((ATerm) Body);
    for(;!ATisEmpty(BodySortDomain); BodySortDomain = ATgetNext(BodySortDomain))
    {
      ATermAppl Var = gsMakeDataVarId(
                        gsFreshString2ATermAppl("x", (ATerm) Context, true),
                        ATAgetFirst(BodySortDomain));
      Context = ATinsert(Context, (ATerm) Var);
      Vars = ATinsert(Vars, (ATerm) Var);
    }
    Vars = ATreverse(Vars);
    Body = gsMakeDataAppl(Body, Vars);
    Part = gsMakeBinder(gsMakeForall(), Vars, Body);
  } else if (gsIsDataExprExists(Part)) {
//    gsDebugMsg("Reconstructing implementation of existential quantification\n");
    //part is an implementation of an existential quantification;
    //replace by an existential quantification.
    ATermList Args = ATLgetArgument(Part, 1);
    assert(ATgetLength(Args) == 1);
    ATermAppl Body = ATAelementAt(Args, 0);
    ATermList BodySortDomain = ATLgetArgument(gsGetSort(Body), 0);
    ATermList Vars = ATmakeList0();
    ATermList Context = ATmakeList1((ATerm) Body);
    for(;!ATisEmpty(BodySortDomain); BodySortDomain = ATgetNext(BodySortDomain))
    {
      ATermAppl Var = gsMakeDataVarId(
                        gsFreshString2ATermAppl("x", (ATerm) Context, true),
                        ATAgetFirst(BodySortDomain));
      Context = ATinsert(Context, (ATerm) Var);
      Vars = ATinsert(Vars, (ATerm) Var);
    }
    Vars = ATreverse(Vars);
    Body = gsMakeDataAppl(Body, Vars);
    Part = gsMakeBinder(gsMakeExists(), Vars, Body);
  } else if (is_lambda_expr(Part)) {
//    gsDebugMsg("Reconstructing implementation of a lambda expression\n");
    Part = gsSubstValues_Appl(*p_substs, Part, true);
    // If the specification was not provided, substitution will not change Part,
    // hence beta reduction will not change the term, and the recursive call
    // would be on exactly the same term, hence running out of the call stack
    // eventually.
    if (!is_lambda_expr(Part)) {
      Part = (ATermAppl) beta_reduce_term((ATerm) Part);
      *recursive = false;
      Part = reconstruct_exprs_appl(Part, p_substs, Spec);
    }
  } else if (gsIsLambdaOpId(Part)) {
//    gsDebugMsg("Reconstructing implementation of a lambda op id\n");
    Part = gsSubstValues_Appl(*p_substs, Part, true);
  } else if (gsIsDataExprC1(Part) || gsIsDataExprCDub(Part)) {
//    gsDebugMsg("Reconstructing implementation of a positive number (%T)\n", Part);
    if (gsIsPosConstant(Part)) {
      Part = gsMakeOpId(gsString2ATermAppl(gsPosValue(Part)), gsMakeSortExprPos());
    } else {
      Part = reconstruct_pos_mult(Part, "1");
    }
  } else if (gsIsDataExprC0(Part)) {
//    gsDebugMsg("Reconstructing implementation of %T\n", Part);
    Part = gsMakeOpId(gsString2ATermAppl("0"), gsMakeSortExprNat());
  } else if ((gsIsDataExprCNat(Part) || gsIsDataExprPos2Nat(Part))
            && (ATisEqual(gsGetSort(ATAgetFirst(ATLgetArgument(Part, 1))), gsMakeSortExprPos()))) {
//    gsDebugMsg("Reconstructing implementation of CNat or Pos2Nat (%T)\n", Part);
    ATermAppl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_exprs_appl(value, p_substs, Spec);
    Part = gsMakeDataExprPos2Nat(value);
    if (gsIsOpId(value)) {
      ATermAppl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name))) {
        Part = gsMakeOpId(name, gsMakeSortExprNat());
      }
    }
  } else if (gsIsDataExprCPair(Part)) {
//    gsDebugMsg("Currently not reconstructing implementation of CPair (%T)\n", Part);
  } else if (gsIsDataExprCNeg(Part)) {
//    gsDebugMsg("Reconstructing implementation of CNeg (%T)\n", Part);
    Part = gsMakeDataExprNeg(ATAgetFirst(ATLgetArgument(Part, 1)));
  } else if ((gsIsDataExprCInt(Part) || gsIsDataExprNat2Int(Part))
            && (ATisEqual(gsGetSort(ATAgetFirst(ATLgetArgument(Part, 1))), gsMakeSortExprNat()))) {
//    gsDebugMsg("Reconstructing implementation of CInt or Nat2Int (%T)\n", Part);
    ATermAppl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_exprs_appl(value, p_substs, Spec);
    Part = gsMakeDataExprNat2Int(value);
    if (gsIsOpId(value)) {
      ATermAppl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name))) {
        Part = gsMakeOpId(name, gsMakeSortExprInt());
      }
    }
  } else if (gsIsDataExprInt2Real(Part)
            && (ATisEqual(gsGetSort(ATAgetFirst(ATLgetArgument(Part, 1))), gsMakeSortExprInt()))) {
//    gsDebugMsg("Reconstructing implementation of Int2Real (%T)\n", Part);
    ATermAppl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_exprs_appl(value, p_substs, Spec);
    Part = gsMakeDataExprInt2Real(value);
    if (gsIsOpId(value)) {
      ATermAppl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name))) {
        Part = gsMakeOpId(name, gsMakeSortExprReal());
      }
    }
  } else if (gsIsDataExprCReal(Part)) {
//    gsDebugMsg("Reconstructing implementation of CReal (%T)\n", Part);
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl ArgNumerator = reconstruct_exprs_appl(ATAelementAt(Args, 0), p_substs, Spec);
    ATermAppl ArgDenominator = reconstruct_exprs_appl(ATAelementAt(Args, 1), p_substs, Spec);
    if (ATisEqual(ArgDenominator, gsMakeOpId(gsString2ATermAppl("1"), gsMakeSortExprPos()))) {
      Part = gsMakeDataExprInt2Real(ArgNumerator);
      if (gsIsOpId(ArgNumerator)) {
        ATermAppl name = ATAgetArgument(ArgNumerator, 0);
        if (gsIsNumericString(gsATermAppl2String(name))) {
          Part = gsMakeOpId(name, gsMakeSortExprReal());
        }
      }
    } else { 
      Part = gsMakeDataExprDivide(ArgNumerator, gsMakeDataExprPos2Int(ArgDenominator));
      if (gsIsOpId(ArgDenominator)) {
        ATermAppl name = ATAgetArgument(ArgDenominator, 0);
        if (gsIsNumericString(gsATermAppl2String(name))) {
          Part = gsMakeDataExprDivide(ArgNumerator, gsMakeOpId(name, gsMakeSortExprInt()));
        }
      }
    }
  } else if (gsIsDataExprDub(Part)) {
//    gsDebugMsg("Reconstructing implementation of Dub (%T)\n", Part);
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl PosArg = ATAelementAt(Args, 1);
    ATermAppl Sort = gsGetSortExprResult(gsGetSort(PosArg));
    ATermAppl Mult = gsMakeDataExprMult(gsMakeOpId(gsString2ATermAppl("2"), Sort), PosArg);
    if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
      Part = gsMakeDataExprAdd(Mult, gsMakeOpId(gsString2ATermAppl("1"), Sort));
    } else if (ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      Part = Mult;
    } else {
      Part = gsMakeDataExprAdd(Mult, bool_to_numeric(BoolArg, gsMakeSortExprNat()));
    }
  } else if (gsIsDataExprAddC(Part)) {
//    gsDebugMsg("Reconstructing implementation of AddC (%T)\n", Part);
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl LHS = ATAelementAt(Args, 1);
    ATermAppl RHS = ATAelementAt(Args, 2);
    ATermAppl Sum = gsMakeDataExprAdd(LHS, RHS);
    ATermAppl Sort = gsGetSortExprResult(gsGetSort(LHS));
    if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
      Part = gsMakeDataExprAdd(Sum, gsMakeOpId(gsString2ATermAppl("1"), Sort));
    } else if (ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      Part = Sum;
    } else {
      Part = gsMakeDataExprAdd(Sum, bool_to_numeric(BoolArg, gsMakeSortExprNat()));
    }
  } else if (gsIsDataExprGTESubt(Part)) {
//    gsDebugMsg("Reconstructing implementation of GTESubt (%T)\n", Part);
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl LHS = ATAelementAt(Args, 0);
    ATermAppl RHS = ATAelementAt(Args, 1);
    Part = gsMakeDataExprSubt(LHS, RHS);
    // Subt always has at least type Int
    if (ATisEqual(gsGetSort(Part), gsMakeSortExprInt())) {
      Part = gsMakeDataExprInt2Nat(gsMakeDataExprSubt(LHS, RHS));
    } else {
      Part = gsMakeDataExprReal2Nat(gsMakeDataExprSubt(LHS, RHS));
    }
  } else if (gsIsDataExprGTESubtB(Part)) {
//    gsDebugMsg("Reconstructing implementation of GTESubtB (%T)\n", Part);
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl LHS = ATAelementAt(Args, 1);
    ATermAppl RHS = ATAelementAt(Args, 2);
    // LHS and RHS are always of type Pos, so LHS - RHS has type Int
    Part = gsMakeDataExprSubt(LHS, RHS);
    ATermAppl Sort = gsGetSort(Part);
    if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
      Part = gsMakeDataExprSubt(Part, gsMakeOpId(gsString2ATermAppl("1"), Sort));
    } else if (!ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      Part = gsMakeDataExprSubt(Part, bool_to_numeric(BoolArg, gsMakeSortExprInt()));
    }
    Part = gsMakeDataExprInt2Nat(Part);
  } else if (gsIsDataExprMultIR(Part)) {
//    gsDebugMsg("Reconstructing implementation of MultIR (%T)\n", Part);
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl Bit = ATAelementAt(Args, 0);
    ATermAppl IR = ATAelementAt(Args, 1);
    ATermAppl LHS = ATAelementAt(Args, 2);
    ATermAppl RHS = ATAelementAt(Args, 3);
    ATermAppl Mult = gsMakeDataExprMult(LHS, RHS);
    if (gsIsDataExprTrue(Bit)) {
      Part = gsMakeDataExprAdd(IR, Mult);
    } else if (gsIsDataExprFalse(Bit)) {
      Part = Mult;
    } else {
      Part = gsMakeDataExprAdd(
               gsMakeDataExprMult(bool_to_numeric(Bit, gsMakeSortExprNat()), gsMakeDataExprPos2Nat(IR)),
               Mult);
    }
  } else if (gsIsDataExprDivMod(Part)) {
    gsDebugMsg("Currently not reconstructing implementation of DivMod (%T)\n", Part);
    // TODO
  } else if (gsIsDataExprGDivMod(Part)) {
    gsDebugMsg("Currently not reconstructing implementation of GDivMod (%T)\n", Part);
    // TODO
  } else if (gsIsDataExprGGDivMod(Part)) {
    gsDebugMsg("Currently not reconstructing implementation of GGDivMod (%T)\n", Part);
    // TODO
  } else if (gsIsDataExprEven(Part)) {
//    gsDebugMsg("Reconstructing implementation of even (%T)\n", Part);
    ATermAppl Arg = ATAgetFirst(ATLgetArgument(Part, 1));
    Part = gsMakeDataExprEq(gsMakeDataExprMod(Arg,
               gsMakeOpId(gsString2ATermAppl("2"),gsMakeSortExprPos())),
             gsMakeOpId(gsString2ATermAppl("0"), gsMakeSortExprNat()));
  } else if (is_list_enum_impl(Part)) {
    if(!gsIsDataExprEmptyList(Part)) {
      ATermList Elts = ATmakeList0();
      while (gsIsDataExprCons(Part)) {
        ATermList Args = ATLgetArgument(Part, 1);
        ATermAppl Arg0 = ATAgetFirst(Args);
        Elts = ATinsert(Elts, (ATerm) Arg0);
        Part = ATAelementAt(Args, 1);
      }
      Elts = ATreverse(Elts);
      ATermAppl Sort = gsMakeSortExprList(gsGetSort(ATAgetFirst(Elts)));
      Part = gsMakeDataExprListEnum(Elts, Sort);
    }
  }
  return Part;
}

ATermAppl reconstruct_pos_mult(const ATermAppl PosExpr, char const* Mult)
{
  ATermAppl Head = gsGetDataExprHead(PosExpr);
  ATermList Args = gsGetDataExprArgs(PosExpr);
  if (ATisEqual(PosExpr, gsMakeOpIdC1())) {
    //PosExpr is 1; return Mult
    return gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos());
  } else if (ATisEqual(Head, gsMakeOpIdCDub())) {
    //PosExpr is of the form cDub(b,p); return (Mult*2)*v(p) + Mult*v(b)
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl PosArg = ATAelementAt(Args, 1);
    char* NewMult = gsStringDub(Mult, 0);
    PosArg = reconstruct_pos_mult(PosArg, NewMult);
    if (ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      //Mult*v(b) = 0
      return PosArg;
    } else if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
      //Mult*v(b) = Mult
      return gsMakeDataExprAdd(PosArg, 
               gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos()));
    } else if (strcmp(Mult, "1") == 0) {
      //Mult*v(b) = v(b)
      return gsMakeDataExprAdd(PosArg, bool_to_numeric(BoolArg, gsMakeSortExprNat()));
    } else {
      //Mult*v(b)
      return gsMakeDataExprAdd(PosArg, 
               gsMakeDataExprMult(gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprNat()), 
                                  bool_to_numeric(BoolArg, gsMakeSortExprNat())));
    }
  } else {
    //PosExpr is not a Pos constructor
    if (strcmp(Mult, "1") == 0) {
      return PosExpr;
    } else {
      return gsMakeDataExprMult(
               gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos()), 
               PosExpr);
    }
  }
}

ATermAppl reconstruct_set_bag_enum(ATermAppl data_expr)
{
  assert (gsIsBinder(data_expr));
  data_expr = (ATermAppl) beta_reduce_term((ATerm) data_expr);
  ATermAppl binder = ATAgetArgument(data_expr, 0);
  assert(gsIsSetComp(binder) || gsIsBagComp(binder));
  ATermAppl body = ATAgetArgument(data_expr, 2);
  ATermList enumeration = ATmakeList0();
  if (gsIsSetComp(binder)) {
    if (reconstruct_set_enumeration(body, &enumeration)) {
      assert(!ATisEmpty(enumeration));
      ATermAppl sort = gsMakeSortExprSet(gsGetSort(ATAgetFirst(enumeration)));
      data_expr = gsMakeDataExprSetEnum(enumeration, sort);
    }
  } else {
    if (reconstruct_bag_enumeration(body, &enumeration)) {
      assert(!ATisEmpty(enumeration));
      ATermAppl sort = gsMakeSortExprBag(gsGetSort(ATAgetFirst(enumeration)));
      data_expr = gsMakeDataExprBagEnum(enumeration, sort);
    }
  }
  return data_expr;
}

bool reconstruct_set_enumeration(ATermAppl data_expr, ATermList* enumeration)
{
  assert(gsIsDataExpr(data_expr));
  while (gsIsDataExprOr(data_expr)) {
    ATermAppl lhs = ATAgetFirst(ATLgetArgument(data_expr, 1));
    if (!gsIsDataExprEq(lhs))                                           return false;
    if (!gsIsDataVarId(ATAgetFirst(ATLgetArgument(lhs, 1))))            return false;
    *enumeration = ATinsert(*enumeration, ATgetFirst(ATgetNext(ATLgetArgument(lhs, 1))));
    data_expr = ATAgetFirst(ATgetNext(ATLgetArgument(data_expr, 1)));
  }
  if (!gsIsDataExprEq(data_expr))                                       return false;
  if (!gsIsDataVarId(ATAgetFirst(ATLgetArgument(data_expr, 1))))        return false;
  *enumeration = ATinsert(*enumeration, ATgetFirst(ATgetNext(ATLgetArgument(data_expr, 1))));
  *enumeration = ATreverse(*enumeration);
  return true;
}

bool reconstruct_bag_enumeration(ATermAppl data_expr, ATermList* enumeration)
{
  assert(gsIsDataExpr(data_expr));
  while (gsIsDataExprAdd(data_expr)) {
    ATermAppl lhs = ATAgetFirst(ATLgetArgument(data_expr, 1));
    if (!gsIsDataExprIf(lhs)) return false;
    ATermList args = ATLgetArgument(lhs, 1);
    ATermAppl arg1 = ATAgetFirst(args);
    ATermAppl amt = ATAgetFirst(ATgetNext(args));
    if (!gsIsDataExprEq(arg1)) return false;
    if (!gsIsDataVarId(ATAgetFirst(ATLgetArgument(arg1, 1)))) return false;
    *enumeration = ATinsert(*enumeration, ATelementAt(ATLgetArgument(arg1, 1), 1));
    *enumeration = ATinsert(*enumeration, (ATerm) amt);
    data_expr = ATAgetFirst(ATgetNext(ATLgetArgument(data_expr, 1)));
  }
  if (!gsIsDataExprIf(data_expr)) return false;
  ATermList args = ATLgetArgument(data_expr, 1);
  ATermAppl arg1 = ATAgetFirst(args);
  ATermAppl amt = ATAgetFirst(ATgetNext(args));
  if (!gsIsDataExprEq(arg1)) return false;
  if (!gsIsDataVarId(ATAgetFirst(ATLgetArgument(arg1, 1)))) return false;
  *enumeration = ATinsert(*enumeration, ATelementAt(ATLgetArgument(arg1, 1), 1));
  *enumeration = ATinsert(*enumeration, (ATerm) amt);
  *enumeration = ATreverse(*enumeration);
  return true;
}

bool is_lambda_expr(ATermAppl Part)
{
  while(gsIsDataAppl(Part)) {
    Part = ATAgetArgument(Part, 0);
  }
  return gsIsLambdaOpId(Part);
}

bool is_lambda_binder_application(ATermAppl data_expr)
{
  if (!gsIsDataAppl(data_expr)) return false;
  while (gsIsDataAppl(data_expr)) {
    data_expr = ATAgetArgument(data_expr, 0);
  }
  if (gsIsBinder(data_expr)) {
    return gsIsLambda(ATAgetArgument(data_expr, 0));
  }
  return false;
}

ATermAppl remove_headers_without_binders_from_spec(ATermAppl Spec, ATermList* p_substs)
{
//  gsDebugMsg("Removing headers from specification\n");
  assert (gsIsProcSpec(Spec) || gsIsLinProcSpec(Spec) ||
          gsIsPBES(Spec) || gsIsDataSpec(Spec));
  ATermAppl DataSpec = NULL;
  if (gsIsDataSpec(Spec)) {
    DataSpec = Spec;
  } else {
    DataSpec = ATAgetArgument(Spec, 0);
  }

//  gsDebugMsg("Dissecting data specification\n");
  // Dissect Data specification
  ATermAppl SortSpec    = ATAgetArgument(DataSpec, 0);
  ATermAppl ConsSpec    = ATAgetArgument(DataSpec, 1);
  ATermAppl MapSpec     = ATAgetArgument(DataSpec, 2);
  ATermAppl DataEqnSpec = ATAgetArgument(DataSpec, 3);

  // Get the lists for data declarations
//  gsDebugMsg("Retrieving data declarations\n");
  t_data_decls data_decls;
  data_decls.sorts     = ATLgetArgument(SortSpec, 0);
  data_decls.cons_ops  = ATLgetArgument(ConsSpec, 0);
  data_decls.ops       = ATLgetArgument(MapSpec, 0);
  data_decls.data_eqns = ATLgetArgument(DataEqnSpec, 0);

  // Pruning data declarations needs to be skipped when we are reconstructing a specification in the
  // internal format before data implementation. For determining this, we use a
  // heuristic that the constructors do not contain true:Bool
  if (ATindexOf(data_decls.cons_ops, (ATerm) gsMakeDataExprTrue(), 0) == -1)
  {
    // true:Bool does not occur, assume this is a specification in the internal
    // format before data implementation.
    return Spec;
  }
  else
  {
    atermpp::table sorts_table(15, 75); // Collect sorts for efficient lookup

    atermpp::table superfluous_sorts(10, 75);
    atermpp::table superfluous_cons_ops(100, 75);
    atermpp::table superfluous_ops(100,75);
    atermpp::table superfluous_data_eqns(100,75);

    for (ATermList l = data_decls.sorts; !ATisEmpty(l); l = ATgetNext(l)) {
      sorts_table.put(ATgetFirst(l), (ATerm) ATtrue);
    }

    // Construct lists of data declarations for system defined sorts
    t_data_decls data_decls_impl;
    initialize_data_decls(&data_decls_impl);

  //  gsDebugMsg("Removing system defined sorts from data declarations\n");
    if (sorts_table.get(gsMakeSortExprBool()) != NULL) {
      impl_sort_bool    (&data_decls_impl);
    }
    if (sorts_table.get(gsMakeSortExprPos()) != NULL) {
      impl_sort_pos     (&data_decls_impl);
    }
    if (sorts_table.get(gsMakeSortExprNat()) != NULL) {
      // Nat is included in the implementation of other sorts, as well as that it
      // includes the implementation of natpair, so needs to be
      // removed with the rest of these.
      impl_sort_nat     (&data_decls_impl);
    }
    if (sorts_table.get(gsMakeSortExprNatPair()) != NULL) {
      // NatPair includes implementation of Nat, so it needs to be included in a larger
      // batch.
      impl_sort_nat_pair(&data_decls_impl);
    }
    if (sorts_table.get(gsMakeSortExprInt()) != NULL) {
      // Int includes implementation of Nat, so it needs to be included in a
      // larger batch.
      impl_sort_int     (&data_decls_impl);
    }
    if (sorts_table.get(gsMakeSortExprReal()) != NULL) {
      // Real includes implementation of Int, so it needs to be included in a
      // larger batch.
      impl_sort_real    (&data_decls_impl);
    }

    while(!ATisEmpty(data_decls_impl.sorts)) {
      superfluous_sorts.put(ATAgetFirst(data_decls_impl.sorts), (ATerm) ATtrue);
      data_decls_impl.sorts = ATgetNext(data_decls_impl.sorts);
    }
    while(!ATisEmpty(data_decls_impl.cons_ops)) {
      superfluous_cons_ops.put(ATAgetFirst(data_decls_impl.cons_ops), (ATerm) ATtrue);
      data_decls_impl.cons_ops = ATgetNext(data_decls_impl.cons_ops);
    }
    while(!ATisEmpty(data_decls_impl.ops)) {
      superfluous_ops.put(ATAgetFirst(data_decls_impl.ops), (ATerm) ATtrue);
      data_decls_impl.ops = ATgetNext(data_decls_impl.ops);
    }
    while(!ATisEmpty(data_decls_impl.data_eqns)) {
      superfluous_data_eqns.put(ATAgetFirst(data_decls_impl.data_eqns), (ATerm) ATtrue);
      data_decls_impl.data_eqns = ATgetNext(data_decls_impl.data_eqns);
    }

    data_decls.sorts = filter_table_elements_from_list(data_decls.sorts, superfluous_sorts);
    data_decls.cons_ops = filter_table_elements_from_list(data_decls.cons_ops, superfluous_cons_ops);
    data_decls.ops = filter_table_elements_from_list(data_decls.ops, superfluous_ops);
    data_decls.data_eqns = filter_table_elements_from_list(data_decls.data_eqns, superfluous_data_eqns);

    // Additional processing of data declarations by manually recognising
    // system defined sorts, operators and data equations
    // these are removed from their respective parts of the data declarations
    // on the fly.
    reconstruct_data_decls(&data_decls, p_substs);

    // Construct new DataSpec and Specification
    SortSpec    = gsMakeSortSpec   (data_decls.sorts);
    ConsSpec    = gsMakeConsSpec   (data_decls.cons_ops);
    MapSpec     = gsMakeMapSpec    (data_decls.ops);
    DataEqnSpec = gsMakeDataEqnSpec(data_decls.data_eqns);
    DataSpec    = gsMakeDataSpec   (SortSpec, ConsSpec, MapSpec, DataEqnSpec);
    if (gsIsDataSpec(Spec)) {
      Spec = DataSpec;
    } else {
      Spec = ATsetArgument(Spec, (ATerm) DataSpec, 0);
    }
    return Spec;
  }
}

void reconstruct_data_decls(t_data_decls* p_data_decls, ATermList* p_substs)
{
  assert(p_substs != NULL);

//  gsDebugMsg("Reconstructing structured sorts\n");

  t_reconstruct_context ctx; 

  calculate_lambda_expressions(p_data_decls, p_substs);
  // TODO: use hashtable for substitutions
  ATermList lambda_substs = *p_substs; // Copy needed for interfering substitutions that
                                       // are added in processing the mappings!

  initialise_sorts(p_data_decls, &ctx);
  initialise_sort_constructors(p_data_decls, &ctx);
  initialise_mappings(p_data_decls, &ctx, p_substs);

//  gsDebugMsg("Inititialization done, traverse equations\n");

  collect_data_equations(p_data_decls, &ctx, &lambda_substs);
//  gsDebugMsg("Determined equations\n");

  check_completeness(p_data_decls, &ctx);
//  gsDebugMsg("Determining recognisers\n");

  calculate_recognisers_and_projections(&ctx);

  // At this point, all sorts that are still in sort_cons_equations and
  // sort_constructors are structured sorts.
  // TODO: to context
  atermpp::table struct_mappings(25, 50);
  atermpp::table struct_equations(100, 50);
  flatten_mappings_and_equations(&struct_mappings, &struct_equations, &ctx);
  remove_constructors(p_data_decls, &ctx);
  remove_mappings(p_data_decls, &struct_mappings);
  remove_data_equations(p_data_decls, &struct_equations, &lambda_substs);

  compute_sort_decls(p_data_decls, &ctx, p_substs);
}

bool has_sort_reference(const ATermAppl spec)
{
  assert(gsIsDataSpec(spec));
  ATermList sorts = ATLgetArgument(ATAgetArgument(spec,0),0);
  while(!ATisEmpty(sorts))
  {
    if(gsIsSortRef(ATAgetFirst(sorts)))
    {
      return true;
    }
    sorts = ATgetNext(sorts);
  }
  return false;
}

bool is_and_of_data_var_equalities(const ATermAppl data_expr)
{
  assert(gsIsDataExpr(data_expr));
  if (gsIsDataExprAnd(data_expr)) {
    ATermAppl lhs = ATAgetFirst(ATLgetArgument(data_expr, 1));
    ATermAppl rhs = ATAgetFirst(ATgetNext(ATLgetArgument(data_expr, 1)));
    return is_and_of_data_var_equalities(lhs) && is_and_of_data_var_equalities(rhs);
  } else {
    if (gsIsDataExprEq(data_expr) &&
        gsIsDataVarId(ATAgetFirst(ATLgetArgument(data_expr, 1))) &&
        gsIsDataVarId(ATAgetFirst(ATgetNext(ATLgetArgument(data_expr, 1))))) {
      return true;
    } else {
      return false;
    }
  }
}

bool is_and_or_of_data_var_eq_lt(const ATermAppl data_expr, bool equal)
{
  assert(gsIsDataExpr(data_expr));
  if (gsIsDataExprOr(data_expr)) {
    ATermList arguments = ATLgetArgument(data_expr, 1);
    ATermAppl lhs = ATAgetFirst(arguments);
    ATermAppl rhs = ATAgetFirst(ATgetNext(arguments));
    if(gsIsDataExprLT(lhs) &&
       gsIsDataVarId(ATAgetFirst(ATLgetArgument(lhs, 1))) &&
       gsIsDataVarId(ATAgetFirst(ATgetNext(ATLgetArgument(lhs, 1)))))
    {
      if(gsIsDataExprAnd(rhs))
      {
        ATermList rhs_arguments = ATLgetArgument(rhs, 1);
        ATermAppl rhs_lhs = ATAgetFirst(rhs_arguments);
        ATermAppl rhs_rhs = ATAgetFirst(ATgetNext(rhs_arguments));
        return (gsIsDataExprEq(rhs_lhs) &&
           gsIsDataVarId(ATAgetFirst(ATLgetArgument(rhs_lhs, 1))) &&
           gsIsDataVarId(ATAgetFirst(ATgetNext(ATLgetArgument(rhs_lhs, 1))))) &&
           is_and_or_of_data_var_eq_lt(rhs_rhs, equal);
      }
    }
  }
  else if ((!equal && gsIsDataExprLT(data_expr)) ||
             equal && gsIsDataExprLTE(data_expr))
  {
    return gsIsDataVarId(ATAgetFirst(ATLgetArgument(data_expr, 1))) &&
           gsIsDataVarId(ATAgetFirst(ATgetNext(ATLgetArgument(data_expr, 1))));
  }
  return false;
}

bool is_list_of_data_var_ids(ATermList l)
{
  while (!ATisEmpty(l)) {
    if (!gsIsDataVarId(ATAgetFirst(l))) {
      return false;
    }
    l = ATgetNext(l);
  }
  return true;
}

ATermList filter_table_elements_from_list(ATermList l, atermpp::table& t)
{
  ATermList result = ATmakeList0();
  while(!ATisEmpty(l)) {
    if (t.get(ATAgetFirst(l)) == NULL) {
      result = ATinsert(result, (ATerm) ATAgetFirst(l));
    }
    l = ATgetNext(l);
  }
  result = ATreverse(result);
  return result;
}

bool is_list_operator(const ATermAppl data_expr)
{
  if (!gsIsOpId(data_expr)) return false;
  return gsIsOpIdListSize(data_expr) ||
         gsIsOpIdEltIn(data_expr) ||
         gsIsOpIdSnoc(data_expr) ||
         gsIsOpIdConcat(data_expr) ||
         gsIsOpIdEltAt(data_expr) ||
         gsIsOpIdHead(data_expr) ||
         gsIsOpIdTail(data_expr) ||
         gsIsOpIdRHead(data_expr) ||
         gsIsOpIdRTail(data_expr);
}

bool is_set_operator(const ATermAppl data_expr)
{
  if (!gsIsOpId(data_expr)) return false;
  return gsIsOpIdSetComp(data_expr) ||
         gsIsOpIdEmptySet(data_expr) ||
         gsIsOpIdEltIn(data_expr) ||
         gsIsOpIdSetUnion(data_expr) ||
         gsIsOpIdSetDiff(data_expr) ||
         gsIsOpIdSetIntersect(data_expr) ||
         gsIsOpIdSetCompl(data_expr);
}

bool is_bag_operator(const ATermAppl data_expr)
{
  if (!gsIsOpId(data_expr)) return false;
  return gsIsOpIdBagComp(data_expr) ||
         gsIsOpIdEmptyBag(data_expr) ||
         gsIsOpIdCount(data_expr) ||
         gsIsOpIdEltIn(data_expr) ||
         gsIsOpIdBagUnion(data_expr) ||
         gsIsOpIdBagDiff(data_expr) ||
         gsIsOpIdBagIntersect(data_expr) ||
         gsIsOpIdBag2Set(data_expr) ||
         gsIsOpIdSet2Bag(data_expr);
}

bool is_constructor_induced_equation(const ATermAppl data_eqn, atermpp::map<ATermAppl, atermpp::indexed_set>& sort_constructors)
{
  assert(gsIsDataEqn(data_eqn));
  ATermAppl lhs = ATAgetArgument(data_eqn, 2);
  ATermAppl rhs = ATAgetArgument(data_eqn, 3);

  // Look at left hand side
  if (gsIsDataExprEq(lhs)) {
    ATermAppl op_sort = gsGetSort(ATAgetArgument(lhs, 0));
    ATermAppl sort = ATAgetFirst(ATLgetArgument(op_sort, 0));
    ATermList args = ATLgetArgument(lhs, 1);
    ATermAppl arg0 = ATAgetFirst(args);
    ATermAppl arg1 = ATAgetFirst(ATgetNext(args));

    if (gsIsDataAppl(arg0)) {
      if (gsIsDataAppl(arg1)) {
        if (ATisEqual(ATAgetArgument(arg0, 0), (ATAgetArgument(arg1, 0)))
          && (ATgetLength(ATLgetArgument(arg0, 1)) == ATgetLength(ATLgetArgument(arg1, 1)))) {
          return is_and_of_data_var_equalities(rhs) &&
            (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0);
        } else {
          return ATisEqual(rhs, gsMakeDataExprFalse()) &&
            (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0) &&
            (sort_constructors[sort].index(ATgetArgument(arg1, 0)) >= 0);
        }
      } else if (gsIsOpId(arg1)) {
        return gsIsOpId(ATAgetArgument(arg0, 0)) &&
               ATisEqual(rhs, gsMakeDataExprFalse()) &&
               (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0) &&
               (sort_constructors[sort].index(arg1) >= 0);
      }
    } else if (gsIsOpId(arg0)) {
      if (gsIsDataAppl(arg1)) {
        return gsIsOpId(ATAgetArgument(arg1, 0)) &&
               ATisEqual(rhs, gsMakeDataExprFalse()) &&
               (sort_constructors[sort].index(arg0) >= 0) &&
               (sort_constructors[sort].index(ATgetArgument(arg1, 0)) >= 0);
      } else if (gsIsOpId(arg1)) {
        return (ATisEqual(arg0, arg1) && ATisEqual(rhs, gsMakeDataExprTrue())) ||
               (!ATisEqual(arg0, arg1) && ATisEqual(rhs, gsMakeDataExprFalse()));
      }
    }
  }
  else if (gsIsDataExprLT(lhs)) {
    ATermAppl op_sort = gsGetSort(ATAgetArgument(lhs, 0));
    ATermAppl sort = ATAgetFirst(ATLgetArgument(op_sort, 0));
    ATermList args = ATLgetArgument(lhs, 1);
    ATermAppl arg0 = ATAgetFirst(args);
    ATermAppl arg1 = ATAgetFirst(ATgetNext(args));

    if (gsIsDataAppl(arg0)) {
      if(gsIsDataAppl(arg1)) {
        if (ATisEqual(ATAgetArgument(arg0, 0), ATAgetArgument(arg1, 0)) &&
          (ATgetLength(ATLgetArgument(arg0, 1)) == ATgetLength(ATLgetArgument(arg1, 1)))) {
            return is_and_or_of_data_var_eq_lt(rhs, false) &&
                   (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0);
        } else {
          return ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(ATAgetArgument(arg0, 0)) < sort_constructors[sort].index(ATAgetArgument(arg1, 0)))) &&
                 (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0) &&
                 (sort_constructors[sort].index(ATgetArgument(arg1, 0)) >= 0);
        }
      }
      else if (gsIsOpId(arg1)) {
        return gsIsOpId(ATAgetArgument(arg0, 0)) &&
               ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(ATAgetArgument(arg0, 0)) < sort_constructors[sort].index(arg1))) &&
               (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0) &&
               (sort_constructors[sort].index(arg1) >= 0);
      }
    } else if (gsIsOpId(arg0)) {
      if(gsIsDataAppl(arg1)) {
        return gsIsOpId(ATAgetArgument(arg1, 0)) &&
               ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(arg0) < sort_constructors[sort].index(ATAgetArgument(arg1, 0)))) &&
               (sort_constructors[sort].index(ATgetArgument(arg1, 0)) >= 0) &&
               (sort_constructors[sort].index(arg0) >= 0);
      }
      else if (gsIsOpId(arg1)) {
        return (ATisEqual(arg0, arg1) && ATisEqual(rhs, gsMakeDataExprFalse())) ||
               (!ATisEqual(arg0, arg1) && ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(arg0) < sort_constructors[sort].index(arg1))));
      }
    }
  }
  else if (gsIsDataExprLTE(lhs)) {
    ATermAppl op_sort = gsGetSort(ATAgetArgument(lhs, 0));
    ATermAppl sort = ATAgetFirst(ATLgetArgument(op_sort, 0));
    ATermList args = ATLgetArgument(lhs, 1);
    ATermAppl arg0 = ATAgetFirst(args);
    ATermAppl arg1 = ATAgetFirst(ATgetNext(args));

    if (gsIsDataAppl(arg0)) {
      if(gsIsDataAppl(arg1)) {
        if (ATisEqual(ATAgetArgument(arg0, 0), ATAgetArgument(arg1, 0)) &&
          (ATgetLength(ATLgetArgument(arg0, 1)) == ATgetLength(ATLgetArgument(arg1, 1)))) {
            return is_and_or_of_data_var_eq_lt(rhs, true) &&
                   (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0);
        } else {
          return ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(ATAgetArgument(arg0, 0)) < sort_constructors[sort].index(ATAgetArgument(arg1, 0)))) &&
                 (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0) &&
                 (sort_constructors[sort].index(ATgetArgument(arg1, 0)) >= 0);
        }
      }
      else if (gsIsOpId(arg1)) {
        return gsIsOpId(ATAgetArgument(arg0, 0)) &&
               ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(ATgetArgument(arg0, 0)) < sort_constructors[sort].index(arg1))) &&
               (sort_constructors[sort].index(ATgetArgument(arg0, 0)) >= 0) &&
               (sort_constructors[sort].index(arg1) >= 0);
      }
    } else if (gsIsOpId(arg0)) {
      if(gsIsDataAppl(arg1)) {
        return gsIsOpId(ATAgetArgument(arg1, 0)) &&
               ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(arg0) < sort_constructors[sort].index(ATAgetArgument(arg1, 0)))) &&
               (sort_constructors[sort].index(ATgetArgument(arg1, 0)) >= 0) &&
               (sort_constructors[sort].index(arg0) >= 0);
      }
      else if (gsIsOpId(arg1)) {
        return (ATisEqual(arg0, arg1) && ATisEqual(rhs, gsMakeDataExprTrue())) ||
               (!ATisEqual(arg0, arg1) && ATisEqual(rhs, gsMakeDataExprBool_bool(sort_constructors[sort].index(arg0) < sort_constructors[sort].index(arg1))));
      }
    }
  }
  return false;
}

bool is_recogniser_equation(const ATermAppl data_eqn)
{
  assert(gsIsDataEqn(data_eqn));
  ATermAppl lhs = ATAgetArgument(data_eqn, 2);
  ATermAppl rhs = ATAgetArgument(data_eqn, 3);

  // lhs is of the form head(args) or head(arg0(args))
  ATermAppl head = ATAgetArgument(lhs, 0);
  ATermList args = ATLgetArgument(lhs, 1);
  ATermAppl arg0 = ATAgetFirst(args);
  if (gsIsDataAppl(arg0)) {
    args = ATLgetArgument(arg0, 1);
    arg0 = ATAgetArgument(arg0, 0);
  } else if (gsIsOpId(arg0)) {
    args = ATmakeList0();
  }
  ATermAppl head_sort = gsGetSort(head);
  assert(gsIsSortArrow(head_sort));

  return gsIsOpId(head) && gsIsOpId(arg0) &&
         (ATgetLength(ATLgetArgument(head_sort, 0)) == 1) &&
         (ATisEqual(rhs, gsMakeDataExprTrue()) ||
           ATisEqual(rhs, gsMakeDataExprFalse())) &&
         is_list_of_data_var_ids(args);
}

bool is_projection_equation(const ATermAppl data_eqn)
{
  assert(gsIsDataEqn(data_eqn));
  ATermAppl lhs = ATAgetArgument(data_eqn, 2);
  ATermAppl rhs = ATAgetArgument(data_eqn, 3);

    // lhs is of the form head(args) or head(arg0(args))
  ATermAppl head = ATAgetArgument(lhs, 0);
  ATermList args = ATLgetArgument(lhs, 1);
  ATermAppl arg0 = ATAgetFirst(args);
  if (gsIsDataAppl(arg0)) {
    args = ATLgetArgument(arg0, 1);
    arg0 = ATAgetArgument(arg0, 0);
  } else if (gsIsOpId(arg0)) {
    args = ATmakeList0();
  }
  ATermAppl head_sort = gsGetSort(head);
  assert(gsIsSortArrow(head_sort));

  return gsIsOpId(head) && gsIsOpId(arg0) &&
         (ATgetLength(ATLgetArgument(head_sort, 0)) == 1) &&
         is_list_of_data_var_ids(args) &&
         gsIsSortArrow(gsGetSort(arg0)) &&
         (ATindexOf(ATLgetArgument(gsGetSort(arg0), 0), (ATerm) gsGetSortExprResult(gsGetSort(head)), 0) != -1) &&
         gsIsDataVarId(rhs);
}

void remove_mapping_not_list(const ATermAppl op,
                        const ATermAppl sort,
                        t_reconstruct_context* p_ctx)
{
  if (!is_list_operator(op) && !is_set_operator(op) && !is_bag_operator(op)) {
    p_ctx->map_equations.erase(op);
    p_ctx->num_map_equations.erase(op);
    if (p_ctx->sort_mappings[sort].index(op) >= 0) {
      p_ctx->sort_mappings[sort].remove(op);
    }
  }
}

bool find_match_in_list(const ATermAppl term, ATermList generic_list)
{
  bool result = false;
  while (!result && !ATisEmpty(generic_list))
  {
    ATermList substs = ATmakeList0();
    result = match_appl(ATAgetFirst(generic_list), term, &substs);
    generic_list = ATgetNext(generic_list);
  }
  return result;
}

bool match(ATerm aterm_ann, ATerm aterm, ATermList* p_substs)
{
  if (ATisEqual(aterm_ann, aterm)) return true;

  if (ATgetType(aterm_ann) == ATgetType(aterm)) {
    if (ATgetType(aterm) == AT_APPL) {
      return match_appl((ATermAppl) aterm_ann, (ATermAppl) aterm, p_substs);
    } else { //ATgetType(arg) == AT_LIST
      return match_list((ATermList) aterm_ann, (ATermList) aterm, p_substs);
    }
  }
  return false;
}

bool match_appl(ATermAppl aterm_ann, const ATermAppl aterm, ATermList* p_substs)
{
  aterm_ann = gsSubstValues_Appl(*p_substs, aterm_ann, false);

  ATerm dummy_str = (ATerm) gsString2ATermAppl("@dummy");

  if (ATisEqual(aterm_ann, aterm)) return true;

  if (gsIsSortId(aterm_ann) && gsIsSortId(aterm)) {
    if (ATgetAnnotation((ATerm) aterm_ann, dummy_str) != NULL) {
      *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(aterm_ann, aterm), *p_substs);
      return true;
    } 
  }

  if (gsIsBinder(aterm_ann) && gsIsBinder(aterm)) {
    ATermList vars_ann = ATLgetArgument(aterm_ann, 1);
    ATermAppl expr_ann = ATAgetArgument(aterm_ann, 2);
    ATermList vars = ATLgetArgument(aterm, 1);
    ATermAppl expr = ATAgetArgument(aterm, 2);

    if (ATgetLength(vars_ann) != ATgetLength(vars)) {
      return false;
    }

    while(!ATisEmpty(vars_ann)) {
      ATermAppl var_ann = ATAgetFirst(vars_ann);
      ATermAppl var = ATAgetFirst(vars);
      ATermAppl sort_ann = gsGetSort(var_ann);
      ATermAppl sort = gsGetSort(var);
      if (ATgetAnnotation((ATerm) sort_ann, dummy_str) != NULL) {
        // match(lambda x:S_a.e, lambda x:T.e') = [S_a := T] match(e, e')
        // match(lambda x:S_a.e, lambda y:T.e') = [S_a := T] match(e[x:=y], e')
        *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(sort_ann, sort), *p_substs);
        if (!ATisEqual(gsGetName(var_ann), gsGetName(var))) {
          expr_ann = capture_avoiding_substitutions(expr_ann, 
            ATmakeList1((ATerm) ATmakeList2((ATerm) var_ann, 
                                            (ATerm) var)));
        }
      } else if (!ATisEqual(sort_ann, sort)) {
        // match(lambda x:S.e, lambda x:T.e') = false
        // match(lambda x:S.e, lambda y:T.e') = false
        return false;
      } else {
        // match(lambda x:S.e, lambda y:S.e') = match(e[x:=y], e')
        if (!ATisEqual(gsGetName(var_ann), gsGetName(var))) {
          expr_ann = capture_avoiding_substitutions(expr_ann, 
            ATmakeList1((ATerm) ATmakeList2((ATerm) var_ann, 
                                            (ATerm) var)));
        }
      }
      vars_ann = ATgetNext(vars_ann);
      vars = ATgetNext(vars);
    }
    if ((is_lambda_binder_application(expr_ann) && is_lambda_binder_application(expr)) ||
        (!is_lambda_binder_application(expr_ann) && !is_lambda_binder_application(expr))) {
    } else if (is_lambda_binder_application(expr_ann)) {
      return match_appl((ATermAppl) beta_reduce_term((ATerm) expr_ann), expr, p_substs);
    } else {
      return match_appl(expr_ann, (ATermAppl) beta_reduce_term((ATerm) expr), p_substs);
    }
  }

  AFun head_ann = ATgetAFun(aterm_ann);
  AFun head = ATgetAFun(aterm);
  if (ATgetArity(head_ann) == ATgetArity(head)) {
    int nr_args = ATgetArity(head);
    if (nr_args > 0) {
      bool result = true;
      for (int i = 0; (i < nr_args) && result; i++) {
        ATerm arg_ann = ATgetArgument(aterm_ann, i);
        ATerm arg = ATgetArgument(aterm, i);
        result = match(arg_ann, arg, p_substs);
      }
      return result;
    }
  }
  return false;
}

bool match_list(ATermList aterm_ann, ATermList aterm, ATermList* p_substs)
{
  bool result = true;
  while(!ATisEmpty(aterm_ann) && !ATisEmpty(aterm) && result) {
    result = match(ATgetFirst(aterm_ann), ATgetFirst(aterm), p_substs);
    aterm_ann = ATgetNext(aterm_ann);
    aterm = ATgetNext(aterm);
  }
  return result && ATisEmpty(aterm_ann) && ATisEmpty(aterm);
}

void calculate_lambda_expressions(const t_data_decls* p_data_decls, ATermList* p_substs)
{
  for (ATermList l = p_data_decls->data_eqns; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl data_eqn = ATAgetFirst(l);
    ATermAppl expr = ATAgetArgument(data_eqn, 2);
    while (gsIsDataAppl(expr)) {
      ATermList bound_vars = ATLgetArgument(expr, 1);
      expr = ATAgetArgument(expr, 0);
      if (gsIsLambdaOpId(expr)) {
        ATermList vars = ATLgetArgument(data_eqn, 0);
        ATermAppl body = ATAgetArgument(data_eqn, 3);
        if (ATgetLength(vars) != ATgetLength(bound_vars)) {
          ATermList free_vars = ATmakeList0();
          while(!ATisEmpty(vars)) {
            ATermAppl var = ATAgetFirst(vars);
            if (ATindexOf(bound_vars, (ATerm) var, 0) == -1) {
              free_vars = ATinsert(free_vars, (ATerm) var);
            }
            vars = ATgetNext(vars);
          }
          body = gsMakeBinder(gsMakeLambda(), free_vars, body);
        }
        body = gsMakeBinder(gsMakeLambda(), bound_vars, body);
        *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(expr, body), *p_substs);
      }
    }
  }
}

bool is_standard_function(const ATermAppl data_expr)
{
  assert(gsIsDataExpr(data_expr));

  bool result = gsIsOpIdEq(data_expr) ||
         gsIsOpIdNeq(data_expr) ||
         gsIsOpIdIf(data_expr) ||
         gsIsOpIdLT(data_expr) ||
         gsIsOpIdLTE(data_expr) ||
         gsIsOpIdGTE(data_expr) ||
         gsIsOpIdGT(data_expr);

  return result;
}

bool is_system_defined_equation(const ATermAppl data_eqn)
{
  assert(gsIsDataEqn(data_eqn));
  ATermAppl data_eqn_lhs = ATAgetArgument(data_eqn, 2);
  ATermAppl data_eqn_rhs = ATAgetArgument(data_eqn, 3);
  if (gsIsDataAppl(data_eqn_lhs) &&
      is_standard_function(ATAgetArgument(data_eqn_lhs, 0)))
  {
    ATermList args = ATLgetArgument(data_eqn_lhs, 1);
    ATermAppl arg0 = ATAgetFirst(args);
    ATermAppl arg1 = ATAgetFirst(ATgetNext(args));
    if (gsIsDataExprIf(data_eqn_lhs))
    {
      // Special case, if: if(true, x, y) == x
      //                   if(false, x, y) == y
      //                   if(b, x, x) == x
      ATermAppl arg2 = ATAgetFirst(ATgetNext(ATgetNext(args)));
      return (gsIsDataExprTrue(arg0) && ATisEqual(data_eqn_rhs, arg1)) ||
             (gsIsDataExprFalse(arg0) && ATisEqual(data_eqn_rhs, arg2)) ||
             (gsIsDataVarId(arg0) && ATisEqual(arg1, arg2) && ATisEqual(data_eqn_rhs, arg1));
    }
    else if (gsIsDataExprEq(data_eqn_lhs)) {
      // Special case, data_eqn is of the form var == var = true;
      return gsIsDataVarId(arg0) &&
             gsIsDataVarId(arg1) &&
             ATisEqual(arg0, arg1) &&
             gsIsDataExprTrue(data_eqn_rhs);
    } else if (gsIsDataExprNeq(data_eqn_lhs)) {
      // Special case, inequality: x != y == !(x==y)
      return gsIsDataVarId(arg0) &&
             gsIsDataVarId(arg1) &&
             ATisEqual(data_eqn_rhs, gsMakeDataExprNot(gsMakeDataExprEq(arg0, arg1)));
    } else if (gsIsDataExprLT(data_eqn_lhs)) {
      // x < x == false
      return gsIsDataVarId(arg0) &&
             gsIsDataVarId(arg1) &&
             ATisEqual(arg0, arg1) &&
             gsIsDataExprFalse(data_eqn_rhs);
    } else if (gsIsDataExprLTE(data_eqn_lhs)) {
      // x <= x == true
      return gsIsDataVarId(arg0) &&
             gsIsDataVarId(arg1) &&
             ATisEqual(arg0, arg1) &&
             gsIsDataExprTrue(data_eqn_rhs);
    } else if (gsIsDataExprGTE(data_eqn_lhs)) {
      // x >= y == y <= x
      return gsIsDataVarId(arg0) &&
             gsIsDataVarId(arg1) &&
             ATisEqual(data_eqn_rhs, gsMakeDataExprLTE(arg1, arg0));
    } else if (gsIsDataExprGT(data_eqn_lhs)) {
      // x > y == y < x
      return gsIsDataVarId(arg0) &&
             gsIsDataVarId(arg1) &&
             ATisEqual(data_eqn_rhs, gsMakeDataExprLT(arg1, arg0));
    }
  }
  return false;
}

ATermAppl get_linked_sort(const ATermAppl op_id)
{
  assert(gsIsOpId(op_id));
  ATermAppl sort = gsGetSort(op_id);
  ATermList sort_domain = ATLgetArgument(sort, 0);
  ATermAppl sort_range = ATAgetArgument(sort, 1);
  ATermAppl linked_sort = ATAgetFirst(sort_domain);
  if (gsIsOpIdIf(op_id) ||
      gsIsOpIdEltIn(op_id) ||
      gsIsOpIdCount(op_id))
  {
    linked_sort = ATAgetFirst(ATgetNext(sort_domain));
  } else if (gsIsOpIdSetComp(op_id) ||
             gsIsOpIdBagComp(op_id)) {
    linked_sort = sort_range;
  }
  return linked_sort;
}

void initialise_sorts(const t_data_decls* p_data_decls, t_reconstruct_context* p_ctx)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);

//  gsDebugMsg("Initialising table with sorts\n");
// Retrieve all sorts from the data declarations
  ATermList sorts = ATconcat(get_sorts((ATerm) p_data_decls->sorts),
                    ATconcat(get_sorts((ATerm) p_data_decls->cons_ops),
                    ATconcat(get_sorts((ATerm) p_data_decls->ops),
                             get_sorts((ATerm) p_data_decls->data_eqns))));

  // Initialisation
  for (ATermList l = sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    if (p_ctx->sorts_table.get(sort) == NULL) { // Unique sorts in the table
      p_ctx->sorts_table.put                (sort, (ATerm) ATtrue);
      p_ctx->sort_constructors.insert       (std::make_pair(sort, atermpp::indexed_set(20,50)));
      p_ctx->num_sort_constructors.insert   (std::make_pair(sort, 0));
      p_ctx->sort_mappings.insert           (std::make_pair(sort, atermpp::indexed_set(20,50)));
      p_ctx->num_sort_mappings.insert       (std::make_pair(sort, 0));
      p_ctx->sort_cons_equations.insert     (std::make_pair(sort, atermpp::indexed_set(20,50)));
      p_ctx->num_sort_cons_equations.insert (std::make_pair(sort, 0));
    }
  }
}

void initialise_sort_constructors(const t_data_decls* p_data_decls, t_reconstruct_context* p_ctx)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);

//  gsDebugMsg("Initialising constructors\n");
  // Initialise sort_constructors, such that for each sort s in
  // sorts sort_constructors(s) contains exactly the
  // constructors of s.
  for (ATermList l = p_data_decls->cons_ops; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl constructor = ATAgetFirst(l);
    ATermAppl sort = gsGetSortExprResult(gsGetSort(constructor));
    if((p_ctx->sorts_table.get(sort) != NULL)) {
      assert(gsIsOpId(constructor));
      std::pair<long, bool> put_result = p_ctx->sort_constructors[sort].put(constructor);
      if (put_result.second) {
        p_ctx->num_sort_constructors[sort]++;
      }
    }
  }
}

void initialise_mappings(const t_data_decls* p_data_decls, t_reconstruct_context* p_ctx, ATermList* p_substs)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);
  assert(p_substs != NULL);

//  gsDebugMsg("Initialising mappings\n");
  std::pair<long, bool> put_result;
  // Traverse mappings to find relevant functions, and mark possible recogniser
  // and projection functions as such.
  for (ATermList l = p_data_decls->ops; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl op = ATAgetFirst(l);
    assert(gsIsOpId(op));

    ATermAppl sort = gsGetSort(op);

    // The only relevant constant functions are empty set and empty bag,
    // treat these separately
    if (gsIsOpIdEmptyBag(op) ||
        gsIsOpIdEmptySet(op)) {
      assert(p_ctx->sorts_table.get(sort) != NULL);
      put_result = p_ctx->sort_mappings[sort].put(op);
      if (put_result.second) {
        p_ctx->num_sort_mappings[sort]++;
        p_ctx->map_equations.insert(std::make_pair(op, atermpp::indexed_set(6, 50)));
        p_ctx->num_map_equations.insert(std::make_pair(op, 0));
      }
    } else if (gsIsSortArrow(sort)) {
      // For all relevant functions, the sort it belongs to, is the first sort
      // in the domain of the function, except for If, EltIn, Count, SetComp,
      // BagComp.
      assert(gsIsOpId(op));
      ATermAppl linked_sort = get_linked_sort(op);

      assert(p_ctx->sorts_table.get(linked_sort) != NULL);

      // Note that we can only partly identify projection and recogniser
      // functions at this point, and we use that the length of sort_domain == 1
      // for these.
      // We need to make sure that functions with |sort_domain| == 1, which are
      // later identified as not being a constructor function, which are also
      // not system defined, should be removed from sort_mappings.
      if (ATisEqual(gsMakeOpIdIf(linked_sort), op)
        || ATisEqual(gsMakeOpIdEq(linked_sort), op)
        || ATisEqual(gsMakeOpIdNeq(linked_sort), op)
        || ATisEqual(gsMakeOpIdLT(linked_sort), op)
        || ATisEqual(gsMakeOpIdLTE(linked_sort), op)
        || ATisEqual(gsMakeOpIdGT(linked_sort), op)
        || ATisEqual(gsMakeOpIdGTE(linked_sort), op)
        || is_list_operator(op)
        || is_set_operator(op)
        || is_bag_operator(op)
        || (ATgetLength(ATLgetArgument(sort, 0)) == 1)) {
        put_result = p_ctx->sort_mappings[linked_sort].put(op);
        if (put_result.second) {
          p_ctx->num_sort_mappings[linked_sort]++;
          p_ctx->map_equations.insert(std::make_pair(op, atermpp::indexed_set(6, 50)));
          p_ctx->num_map_equations.insert(std::make_pair(op, 0));
        }
        if (gsIsOpIdSetComp(op)) {
          ATermAppl sort_set = ATAgetArgument(gsGetSort(op), 1);
          if(gsIsSetSortId(sort_set))
          {
            ATermAppl element_sort = get_element_sort(op);
            *p_substs = gsAddSubstToSubsts(
              gsMakeSubst_Appl(sort_set,
                               gsMakeSortExprSet(element_sort)),
              *p_substs);
          }
        } else if (gsIsOpIdBagComp(op)) {
          ATermAppl sort_bag = ATAgetArgument(gsGetSort(op), 1);
          if(gsIsBagSortId(sort_bag))
          {
            ATermAppl element_sort = get_element_sort(op);
            *p_substs = gsAddSubstToSubsts(
              gsMakeSubst_Appl(sort_bag,
                               gsMakeSortExprBag(element_sort)),
              *p_substs);
          }
        }
      }
    }
  }
}

void collect_data_equations(const t_data_decls* p_data_decls, t_reconstruct_context* p_ctx, ATermList* p_substs)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);
  assert(p_substs != NULL);

//  gsDebugMsg("Collecting data equations\n");
 // for matching against list, set, bag equations
  ATermAppl elt_sort = gsMakeSortId(gsString2ATermAppl("sort_elt"));
  ATermAppl list_sort = gsMakeSortId(gsString2ATermAppl("sort_list"));
  ATermAppl set_sort = gsMakeSortId(gsString2ATermAppl("sort_set"));
  ATermAppl bag_sort = gsMakeSortId(gsString2ATermAppl("sort_bag"));
  ATerm dummy = (ATerm) gsString2ATermAppl("@dummy");
  elt_sort =  (ATermAppl) ATsetAnnotation((ATerm) elt_sort,  dummy, dummy);
  list_sort = (ATermAppl) ATsetAnnotation((ATerm) list_sort, dummy, dummy);
  set_sort =  (ATermAppl) ATsetAnnotation((ATerm) set_sort,  dummy, dummy);
  bag_sort =  (ATermAppl) ATsetAnnotation((ATerm) bag_sort,  dummy, dummy);
  ATermList generic_equations = ATconcat(build_list_equations(elt_sort, list_sort),
                                ATconcat(build_set_equations(elt_sort, set_sort),
                                         build_bag_equations(elt_sort, bag_sort, set_sort)));
  std::pair<long, bool> put_result;

  // Traverse data equations and:
  // - Identify structured sorts
  // - Mark system defined data equations for removal.
  for (ATermList data_eqns = p_data_decls->data_eqns; !ATisEmpty(data_eqns);
       data_eqns = ATgetNext(data_eqns))
  {
    ATermAppl data_eqn = ATAgetFirst(data_eqns);

    data_eqn = gsSubstValues_Appl(*p_substs, data_eqn, true);
    data_eqn = (ATermAppl) beta_reduce_term((ATerm) data_eqn);
    ATermAppl data_eqn_lhs = ATAgetArgument(data_eqn, 2);
    ATermAppl data_eqn_rhs = ATAgetArgument(data_eqn, 3);

    if (gsIsDataAppl(data_eqn_lhs)) {
      ATermAppl head = ATAgetArgument(data_eqn_lhs, 0);
      // Special cases, data_eqn is a system defined equation, which is
      // generated for all sorts.
      if (is_system_defined_equation(data_eqn)) {
        assert(gsIsOpId(head));
        assert(p_ctx->sorts_table.get(get_linked_sort(head)) != NULL);
        assert(p_ctx->sort_mappings[get_linked_sort(head)].index(head) >= 0);
        put_result = p_ctx->map_equations[head].put(data_eqn);
        if (put_result.second) {
          p_ctx->num_map_equations[head]++;
        }
      } else if (is_constructor_induced_equation(data_eqn, p_ctx->sort_constructors)
        || find_match_in_list(data_eqn, generic_equations)) {
        assert(gsIsOpId(head));
        ATermAppl sort = get_linked_sort(head);
        put_result = p_ctx->sort_cons_equations[sort].put(data_eqn);
        if (put_result.second) {
          p_ctx->num_sort_cons_equations[sort]++;
        }
      } else if (is_recogniser_equation(data_eqn)) {
        assert(gsIsOpId(head));
        ATermAppl arg0 = ATAgetFirst(ATLgetArgument(data_eqn_lhs, 1));
        while (gsIsDataAppl(arg0)) {
          arg0 = ATAgetArgument(arg0, 0);
        }
        if (p_ctx->sort_constructors[get_linked_sort(head)].index(arg0) >= 0
          && p_ctx->sort_mappings[get_linked_sort(head)].index(head) >= 0)
        {
          assert(p_ctx->sorts_table.get(get_linked_sort(head)) != NULL);
          put_result = p_ctx->map_equations[head].put(data_eqn);
          if (put_result.second) {
            p_ctx->num_map_equations[head]++;
          }
          if (ATisEqual(data_eqn_rhs, gsMakeDataExprTrue())) {
            p_ctx->recognises[head] = arg0;
          }
        }
      } else if (is_projection_equation(data_eqn)) {
        ATermList args = ATLgetArgument(data_eqn_lhs, 1);
        ATermAppl arg0 = ATAgetFirst(args);
        while (gsIsDataAppl(arg0)) {
          args = ATLgetArgument(arg0, 1);
          arg0 = ATAgetArgument(arg0, 0);
        }
        assert(p_ctx->sorts_table.get(get_linked_sort(head)) != NULL);
        //assert(p_ctx->sort_mappings[get_linked_sort(head)].index(head) >= 0);
        //assert(p_ctx->sort_constructors[get_linked_sort(head)].index(arg0) >= 0);
        if(p_ctx->sort_constructors[get_linked_sort(head)].index(arg0) >= 0 &&
           p_ctx->sort_mappings[get_linked_sort(head)].index(head) >= 0)
        {
          if (p_ctx->num_map_equations[head] > 0) {
            // there can be only one data equation for a projection function,
            // hence head is not a projection function.
            p_ctx->projects.erase(std::make_pair(arg0, ATindexOf(args, (ATerm) data_eqn_rhs, 0)));
            //remove_mapping_not_list(head, sort, sort_mappings, map_equations, num_map_equations);
          } else {
            put_result = p_ctx->map_equations[head].put(data_eqn);
            p_ctx->projects[std::make_pair(arg0, ATindexOf(args, (ATerm) data_eqn_rhs, 0))] = head;
            if (put_result.second) {
              p_ctx->num_map_equations[head]++;
            }
          }
        }
      } else if (find_match_in_list(data_eqn, generic_equations)) {
        put_result = p_ctx->map_equations[head].put(data_eqn);
        if (put_result.second) {
          p_ctx->num_map_equations[head]++;
        } else {
        //  recognises.erase(head);
        //  remove_mapping_not_list(head, sort, sort_mappings, map_equations, num_map_equations);
        }
      }
    } else if (gsIsOpId(data_eqn_lhs)) {
      if (find_match_in_list(data_eqn, generic_equations)) {
        put_result = p_ctx->map_equations[data_eqn_lhs].put(data_eqn);
        if (put_result.second) {
          p_ctx->num_map_equations[data_eqn_lhs]++;
        }
      } else {
        ATermAppl sort = gsGetSort(data_eqn_lhs);
        //XXX hackish solution
        if (gsIsSortArrow(sort)) {
          sort = ATAgetFirst(ATLgetArgument(sort, 0));
        }
        p_ctx->recognises.erase(data_eqn_lhs);
        remove_mapping_not_list(data_eqn_lhs, sort, p_ctx);
      }
    } 
  }
}

void check_completeness(t_data_decls* p_data_decls, t_reconstruct_context* p_ctx)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);

//  gsDebugMsg("Checking completeness\n");
  // Check for completeness and remove structured sort from data declarations
  for (ATermList l = p_ctx->sorts_table.table_keys(); !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    if (!is_set_bag_list_sort(sort, p_ctx)) {
      // The sort type depends on the number of constructors
      // equations for ==, < and <=
      if (p_ctx->num_sort_cons_equations[sort] != 3 * (p_ctx->num_sort_constructors[sort] * p_ctx->num_sort_constructors[sort])) {
        // The sort is not composite...
        if (ATindexOf(p_data_decls->sorts, (ATerm) sort, 0) != -1) {
          // ... but it is in the original specification, leave it in.
          p_ctx->non_composite_sorts.put(sort, sort);
        } else {
          // ... but it is not in the original specification, hence it is system
          // defined.
          p_ctx->system_defined_sorts.put(sort, sort);
          p_ctx->sorts_table.remove(sort);
          p_ctx->sort_constructors.erase(sort);
          p_ctx->num_sort_constructors.erase(sort);
          p_ctx->sort_mappings.erase(sort);
          p_ctx->num_sort_mappings.erase(sort);
          p_ctx->sort_cons_equations.erase(sort);
          p_ctx->num_sort_cons_equations.erase(sort);
        }
      } else if (ATindexOf(p_data_decls->sorts, (ATerm) sort, 0) != -1) {
        // Sort is in the original specification, leave it in
        if (p_ctx->num_sort_cons_equations[sort] == 0) {
          // Sort has not got any constructors, hence it is not a structured sort
          p_ctx->non_composite_sorts.put(sort, sort);
        } else {
          // sort is a structured sort
          p_ctx->composite_sorts.put(sort, sort);
        }
      } else {
        // Sort is not in the original specification
        p_ctx->system_defined_sorts.put(sort, sort);
      }
    } else {
      // List, set and bag sorts are always treated as composite
      p_ctx->composite_sorts.put(sort, sort);
    }
  }
}

void calculate_recognisers_and_projections(t_reconstruct_context* p_ctx)
{
  assert(p_ctx != NULL);

  ATermList struct_sorts = p_ctx->sorts_table.table_keys();

  // Check for equations that have sufficient equations to be a recogniser
  // or projection function
  // and schedule the equations and mappings for these for removal
  // Note:
  // * We do not remove if, ==, != functions from sort_mappings, these functions
  //   should be removed.
  // * If a mapping has sort A -> Bool, |constructors| ==  |equations|, where
  //   |constructors| > 0 must hold, then it is concidered a recogniser function.
  // * If a mapping has sort A -> Bool, and |constructors| != |equations|, or
  // |constructors| == 0, then it is neither a recogniser, nor a projection
  // function.
  // * If a mapping has sort A -> B, with B != Bool, and |equations| != 1, then
  // it cannot be a recogniser nor a projection function.
  for (ATermList l = struct_sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    assert(p_ctx->sort_mappings.count(sort) == 1);
    // mappings corresponding to the sort
    ATermList maps = p_ctx->sort_mappings[sort].elements();
    // determine for each of the mappings whether it could be a recogniser or a
    // projection function.
    while(!ATisEmpty(maps))
    {
      ATermAppl map = ATAgetFirst(maps);
      assert(gsIsOpId(map));
      if (!is_standard_function(map))
      { // ==. if. != cannot be recogniser/projection
        // map is possibly a recogniser or a projection function.
        ATermAppl map_sort = gsGetSort(map);
        if (gsIsSortArrow(map_sort))
        { // A recogniser or projection function always has a function type...
          if(ATgetLength(ATLgetArgument(map_sort, 0)) == 1)
          { //... with exactly one argument
            if(ATisEqual(gsGetSortExprResult(map_sort), gsMakeSortExprBool()))
            { // if the result sort is a boolean map might be a recogniser
              // a recogniser satisfies #equations = #constructors, and there must
              // be constructors because it belongs to a structured sort.
              if(p_ctx->num_map_equations[map] != p_ctx->num_sort_constructors[sort]
                || p_ctx->num_sort_constructors[sort] == 0)
              { // If this is not the case, map could still be a projection function,
                // but then it has to have 1 equation.
                if (p_ctx->num_map_equations[map] != 1)
                { // This is also not a projection function
                  remove_mapping_not_list(map, sort, p_ctx);
                }
                else
                { // This might be a projection function, hence check if the equation has the right form.
                  ATermAppl eqn = ATAgetFirst(p_ctx->map_equations[map].elements());
                  if (!is_projection_equation(eqn))
                  {
                    remove_mapping_not_list(map, sort, p_ctx);
                  }
                } // end if (p_ctx->num_map_equations[map] != 1)
              } else {
                // map has the right form to be a recogniser, but beware, it
                // might as well be a projection function in some degenerate
                // cases!
                // First check whether there is only one equation, in which case
                // we need to check both recogniser and projection.
                if (p_ctx->num_map_equations[map] == 1)
                {
                  ATermAppl eqn = ATAgetFirst(p_ctx->map_equations[map].elements());
                  if (is_recogniser_equation(eqn) &&
                      ATAgetArgument(eqn, 3) == gsMakeDataExprTrue() &&
                      (p_ctx->is_recognised_by.find(p_ctx->recognises[map]) ==
                      p_ctx->is_recognised_by.end()))
                  {
                    p_ctx->is_recognised_by[p_ctx->recognises[map]] = map;
                  }
                  else if (!is_projection_equation(eqn))
                  {
                    remove_mapping_not_list(map, sort, p_ctx);
                  }
                }
                else
                {
                  // Check that indeed all equations satisfy recogniser properties
                  ATermList l = p_ctx->map_equations[map].elements();
                  bool r = true;
                  int true_count = 0;
                  while (!ATisEmpty(l) && r)
                  {
                    r = r && is_recogniser_equation(ATAgetFirst(l));
                    if(ATAgetArgument(ATAgetFirst(l), 3) == gsMakeDataExprTrue())
                    {
                      ++true_count;
                    }
                    l = ATgetNext(l);
                  }
                  if(r && true_count == 1 &&
                     (p_ctx->is_recognised_by.find(p_ctx->recognises[map]) ==
                     p_ctx->is_recognised_by.end()))
                  {
                    p_ctx->is_recognised_by[p_ctx->recognises[map]] = map;
                  }
                  else
                  {
                    remove_mapping_not_list(map, sort, p_ctx);
                  } // end if r
                } // end if (p_ctx->num_map_equations[map] == 1)
              } // end if (p_ctx->num_map_equations[map] != p_ctx->num_sort_constructors[sort]
                //        || p_ctx->num_sort_constructors[sort] == 0)
            } // end if (ATisEqual(gsGetSortExprResult(map_sort), gsMakeSortExprBool()))
            else if (p_ctx->num_map_equations[map] == 1)
            { // (ATgetLength(ATLgetArgument(map_sort, 0)) != 1)
              // This might be a projection function, check the signature
              ATermAppl eqn = ATAgetFirst(p_ctx->map_equations[map].elements());
              if (!is_projection_equation(eqn))
              {
                remove_mapping_not_list(map, sort, p_ctx);
              }
            }
            else
            { // This is neither a recogniser, nor a projection function
              // (ATgetLength(ATLgetArgument(map_sort, 0)) == 1) && #equations != 1
              remove_mapping_not_list(map, sort, p_ctx);
            }
          } // end if (ATgetLength(ATLgetArgument(map_sort, 0)) == 1)
        } //endif gsIsSortArrow
        else
        { // A constant function is also not a recogniser, nor a projection function
          // !gsIsSortArrow(sort)
          remove_mapping_not_list(map, sort, p_ctx);
        }
      } // endif if, ==, !=
      // else the function is ==, if or !=, which is not a recogniser or a
      // projection function, but should still be removed from the
      // specification.
      maps = ATgetNext(maps);
    }
  }
}

void flatten_mappings_and_equations(atermpp::table* mappings, atermpp::table* equations, t_reconstruct_context* p_ctx)
{
  assert(mappings != NULL);
  assert(equations != NULL);
  assert(p_ctx != NULL);

//  gsDebugMsg("Flatten mappings and equations\n");
  for (ATermList l = p_ctx->sorts_table.table_keys(); !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
      // Mappings
      ATermList elts = p_ctx->sort_mappings[ATAgetFirst(l)].elements();
      while (!ATisEmpty(elts)) {
        ATermAppl elt = ATAgetFirst(elts);
        if (is_standard_function(elt) ||
            p_ctx->composite_sorts.get(sort) == sort)
        {
          mappings->put(ATAgetFirst(elts), (ATerm) ATtrue);
        }
        elts = ATgetNext(elts);
      }
      // Constructor induced equations
      elts = p_ctx->sort_cons_equations[ATAgetFirst(l)].elements();
      while(!ATisEmpty(elts)) {
        ATermAppl elt = ATAgetFirst(elts);
        if (is_system_defined_equation(elt) ||
            p_ctx->composite_sorts.get(sort) == sort)
        {
          equations->put(ATgetFirst(elts), (ATerm) ATtrue);
        }
        elts = ATgetNext(elts);
      }
      // Per mapping equations for sort
      ATermList mappings = p_ctx->sort_mappings[ATAgetFirst(l)].elements();
      while(!ATisEmpty(mappings)) {
        elts = p_ctx->map_equations[ATAgetFirst(mappings)].elements();
        while (!ATisEmpty(elts)) {
          ATermAppl elt = ATAgetFirst(elts);
          if (is_system_defined_equation(elt) ||
              p_ctx->composite_sorts.get(sort) == sort)
          {
            equations->put(ATgetFirst(elts), (ATerm) ATtrue);
          }
          elts = ATgetNext(elts);
        }
        mappings = ATgetNext(mappings);
      }
    }
}

void remove_constructors(t_data_decls* p_data_decls, t_reconstruct_context* p_ctx)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);

//  gsDebugMsg("Remove constructors\n");
  // Remove constructors for structured sorts from declarations
  ATermList constructors = ATmakeList0();
  while (!ATisEmpty(p_data_decls->cons_ops)) {
    ATermAppl cons_op = ATAgetFirst(p_data_decls->cons_ops);
    ATermAppl sort = gsGetSortExprResult(gsGetSort(cons_op));
    if (p_ctx->composite_sorts.get(sort) == NULL) {
      // Sort is not a structured sort
      constructors = ATinsert(constructors, (ATerm) cons_op);
    }
    p_data_decls->cons_ops = ATgetNext(p_data_decls->cons_ops);
  }
  p_data_decls->cons_ops = ATreverse(constructors);
}

void remove_mappings(t_data_decls* p_data_decls, atermpp::table* p_mappings)
{
  assert(p_data_decls != NULL);
  assert(p_mappings != NULL);

//  gsDebugMsg("Remove mappings\n");

  // Remove mappings from declarations
  ATermList mappings = ATmakeList0();
  while (!ATisEmpty(p_data_decls->ops)) {
    ATermAppl op = ATAgetFirst(p_data_decls->ops);
    if (!ATisEqual(p_mappings->get(op), ATtrue) &&
        !gsIsLambdaOpId(op)) {
      mappings = ATinsert(mappings, (ATerm) op);
    }
    p_data_decls->ops = ATgetNext(p_data_decls->ops);
  }
  p_data_decls->ops = ATreverse(mappings);
}

void remove_data_equations(t_data_decls* p_data_decls, atermpp::table* p_data_eqns, ATermList* p_substs)
{
  assert(p_data_decls != NULL);
  assert(p_data_eqns != NULL);
  assert(p_substs != NULL);

//  gsDebugMsg("Remove equations\n");

  // Remove equations for structured sorts
  ATermList data_eqns = ATmakeList0();
  while(!ATisEmpty(p_data_decls->data_eqns)) {
    ATermAppl data_eqn = ATAgetFirst(p_data_decls->data_eqns);
    ATermAppl lhs = ATAgetArgument(data_eqn, 2);
    // Unconditionally remove equations if the lhs is a lambda opid
    if (!is_lambda_expr(lhs)) {
      data_eqn = gsSubstValues_Appl(*p_substs, data_eqn, true);
      data_eqn = (ATermAppl) beta_reduce_term((ATerm) data_eqn);
      if(!ATisEqual(p_data_eqns->get(data_eqn), ATtrue)) {
        data_eqns = ATinsert(data_eqns, (ATerm) data_eqn);
      }
    }
    p_data_decls->data_eqns = ATgetNext(p_data_decls->data_eqns);
  }
  p_data_decls->data_eqns = ATreverse(data_eqns);
}

void compute_sort_decls(t_data_decls* p_data_decls, t_reconstruct_context* p_ctx, ATermList* p_substs)
{
  assert(p_data_decls != NULL);
  assert(p_ctx != NULL);
  assert(p_substs != NULL);

  // Insert non-composite sorts
  p_data_decls->sorts = ATreverse(p_ctx->non_composite_sorts.table_keys());

//  gsDebugMsg("Building structured sort declarations\n");
  // Insert struct sort declarations
  for (ATermList l = ATreverse(p_ctx->composite_sorts.table_keys()); !ATisEmpty(l); l = ATgetNext(l))
  {
    // Build up struct
    ATermList struct_constructors = ATmakeList0();
    ATermAppl sort = ATAgetFirst(l);
    ATermList constructors = p_ctx->sort_constructors[sort].elements();

    if(p_ctx->num_sort_constructors[sort] != 0) {
      if(p_ctx->num_sort_constructors[sort] == 2 &&
         (ATindexOf(constructors, (ATerm) gsMakeOpIdEmptyList(sort), 0) != -1)) {
        // sort is a list sort
        ATermAppl element_sort = NULL;
        while (!ATisEmpty(constructors)) {
          ATermAppl constructor = ATAgetFirst(constructors);
          if (gsIsOpIdCons(constructor)) {
            element_sort = ATAgetFirst(ATLgetArgument(gsGetSort(constructor), 0));
          }
          constructors = ATgetNext(constructors);
        }
        if (element_sort != NULL) {
          if (gsIsListSortId(sort)) {
            *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(sort, gsMakeSortExprList(element_sort)), *p_substs);
          } else {
            ATermAppl sort_name = ATAgetArgument(sort, 0);
            assert(ATindexOf(p_data_decls->sorts, (ATerm) sort, 0) == -1); // Do not add duplicate sorts
            p_data_decls->sorts = ATinsert(p_data_decls->sorts,
                                    (ATerm) gsMakeSortRef(sort_name, gsMakeSortExprList(element_sort)));
          }
        }
      } else {
        // sort is a structured sort
        while (!ATisEmpty(constructors)) {
          ATermAppl constructor = ATAgetFirst(constructors);
          ATermAppl cons_sort = gsGetSort(constructor);
          ATermList struct_projs = ATmakeList0();
          // If a constructor has arguments, add the arguments in the structured
          // sort.
          if (gsIsSortArrow(cons_sort)) {
            ATermList cons_sort_domain = ATLgetArgument(cons_sort, 0);
            int index = 0;
            while (!ATisEmpty(cons_sort_domain)) {
              ATermAppl proj_name = gsMakeNil();
              if (p_ctx->projects.count(std::make_pair(constructor, index)) > 0) {
                proj_name = gsGetName(p_ctx->projects[std::make_pair(constructor, index)]);
              }
              struct_projs = ATinsert(struct_projs, (ATerm) gsMakeStructProj(proj_name, ATAgetFirst(cons_sort_domain)));
              cons_sort_domain = ATgetNext(cons_sort_domain);
              index++;
            }
            struct_projs = ATreverse(struct_projs);
          }
          ATermAppl recogniser = gsMakeNil();
          if (p_ctx->is_recognised_by.count(constructor) > 0) {
            recogniser = gsGetName(p_ctx->is_recognised_by[constructor]);
          }
          ATermAppl struct_cons = gsMakeStructCons(gsGetName(constructor), struct_projs, recogniser);
          struct_constructors = ATinsert(struct_constructors, (ATerm) struct_cons);
          constructors = ATgetNext(constructors);
        }
        struct_constructors = ATreverse(struct_constructors);
        ATermAppl struct_sort = gsMakeSortStruct(struct_constructors);
        assert(gsIsSortId(sort));
        if (gsIsStructSortId(sort)) {
          *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(sort, struct_sort), *p_substs);
        } else {
          ATermAppl sort_name = ATAgetArgument(sort, 0);
          p_data_decls->sorts = ATinsert(p_data_decls->sorts,
                                  (ATerm) gsMakeSortRef(sort_name, struct_sort));
        }
      }
    } else if (p_ctx->sort_mappings[sort].index(gsMakeOpIdEmptySet(sort)) >= 0 ||
                 p_ctx->sort_mappings[sort].index(gsMakeOpIdEmptyBag(sort)) >= 0) {
      // Zero constructors, might be a reference to a Set or Bag
      if(!gsIsSetSortId(sort) && !gsIsBagSortId(sort))
      {
        // sort is a sort reference S = Set(T) or S = Bag(T), for some sort T
        for(ATermList mappings = p_ctx->sort_mappings[sort].elements();
            !ATisEmpty(mappings);
            mappings = ATgetNext(mappings))
        {
          ATermAppl mapping = ATAgetFirst(mappings);
          if(gsIsOpIdSetComp(mapping))
          {
            ATermAppl sort_name = ATAgetArgument(sort, 0);
            ATermAppl set_sort = gsMakeSortExprSet(get_element_sort(mapping));
            p_data_decls->sorts = ATinsert(p_data_decls->sorts,
                                    (ATerm) gsMakeSortRef(sort_name, set_sort));

          }
          else if(gsIsOpIdBagComp(mapping))
          {
            ATermAppl sort_name = ATAgetArgument(sort, 0);
            ATermAppl bag_sort = gsMakeSortExprBag(get_element_sort(mapping));
            p_data_decls->sorts = ATinsert(p_data_decls->sorts,
                                    (ATerm) gsMakeSortRef(sort_name, bag_sort));
          }
        }
      }
    }
  }
//  gsDebugMsg("Done reconstructing structured sorts\n");
}

// This is a workaround to determine whether a sort might also be a list sort.
// For conciseness this has been taken together with the check for being a set
// or bag sort. Note that the hackish solution is not needed in case of sets or
// bags, because these do not have constructors.
bool is_set_bag_list_sort(ATermAppl sort, t_reconstruct_context* p_ctx)
{
  // simple sort ids
  if (gsIsSetSortId(sort) || gsIsBagSortId(sort) || gsIsListSortId(sort)) {
    return true;
  }

  // Sort reference to set/bag sort
  if (p_ctx->sort_mappings[sort].index(gsMakeOpIdEmptySet(sort)) >= 0 ||
      p_ctx->sort_mappings[sort].index(gsMakeOpIdEmptyBag(sort)) >= 0)
  {
    return true;
  }

  // sort reference to list sort
  if(p_ctx->num_sort_constructors[sort] == 2 &&
     p_ctx->sort_constructors[sort].index(gsMakeOpIdEmptyList(sort)) >= 0) {
    return true;
  }
  return false;
}

ATermAppl get_element_sort(ATermAppl data_expr)
{
  assert(gsIsOpIdSetComp(data_expr) || gsIsOpIdBagComp(data_expr));
  ATermList domain = ATLgetArgument(gsGetSort(data_expr), 0);
  assert(ATgetLength(domain) == 1);
  ATermAppl element_sort = ATAgetFirst(domain);
  // Element sort should be of the form S -> Bool or S -> Nat
  assert(gsIsSortArrow(element_sort));
  assert(ATAgetArgument(element_sort, 1) == gsMakeSortExprBool() ||
         ATAgetArgument(element_sort, 1) == gsMakeSortExprNat());
  assert(ATgetLength(ATLgetArgument(element_sort, 0)));
  element_sort = ATAgetFirst(ATLgetArgument(element_sort, 0));

  return element_sort;
}

  }   //namespace core
}     //namespace mcrl2
