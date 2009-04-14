// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_implementation_concrete.cpp

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <climits>

#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/fset.h"
#include "mcrl2/new_data/set.h"
#include "mcrl2/new_data/fbag.h"
#include "mcrl2/new_data/bag.h"
#include "mcrl2/new_data/structured_sort.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/detail/data_implementation_concrete.h"
#include "mcrl2/core/detail/data_common.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"

#include "workarounds.h" //DECL_A

using namespace mcrl2::core::detail;
using namespace mcrl2::core;
using namespace atermpp;

 namespace mcrl2 {
  namespace new_data {
     /// \cond INTERNAL_DOCS
     namespace detail {

//local declarations
//------------------


static ATermList compute_sort_ref_substs(ATermAppl spec);
//Pre: spec is a specification that adheres to the internal syntax after
//     data implementation, with the exception that sort references may occur
//Ret: the list of substitution such that if a sort reference is of the form
//     sort_ref(n,e), where e is the implementation of a type constructor and
//     sort_ref(n,e) is the first sort reference with e as a rhs, e := n is in
//     the result; otherwise, n := e is in the result.

static ATermAppl impl_sort_refs(ATermAppl spec, ATermList* substitution_context);
//Pre: spec is a specification that adheres to the internal syntax after
//     data implementation, with the exception that sort references may occur
//Ret: spec in which all sort references are implemented, i.e.:
//     - all sort references are removed from spec
//     - if a sort reference is of the form sort_ref(n, e), where e is the
//       implementation of a type constructor and sort_ref(n, e) is the first
//       sort reference with e as a rhs, e is replaced by n in spec;
//       otherwise, n is replaced by e in spec

static ATermAppl impl_standard_functions_spec(ATermAppl spec);
//Pre: spec is a specification that adheres to the internal syntax after
//     data implementation
//Ret: spec in which an implementation for equality, inequality and if
//     is added for each sort occurring in spec.

static ATermAppl impl_numerical_pattern_matching(ATermAppl spec);
//Pre: spec is a specification that adheres to the internal syntax after
//     data implementation
//Ret: spec in which numerical patterns can be matched; this means that in the
//     left-hand-sides of equations the following patterns are implemented:
//     - Pos2Nat(p) : replace by cNat(p)
//     - Pos2Int(p) : replace by cInt(cNat(p))
//     - Pos2Real(p): replace by cReal(cInt(cNat(p)))
//     - Nat2Int(n) : replace by cInt(n)
//     - Nat2Real(n): replace by cReal(cInt(n))
//     - Int2Real(x): replace by cReal(x)
//     - -p: replace by cNeg(p)
//     TODO:
//     - p+k, where k is a constant of sort Pos: replace by p,
//       add condition p>=k, and add substitution [p -> Int2Pos(p-k)]
//       (for the condition and the rhs)
//     - n+k, where k is a constant of sort Nat: replace by n,
//       add condition n>=k, and add substitution [n -> Int2Nat(n-k)]
//     - -n: replace by the following two patterns:
//           c0,      and add substitution [n -> c0]
//           cNeg(p), and add substitution [p -> cNat(p)]

static ATermAppl impl_numerical_pattern_matching_expr(ATermAppl data_expr,
  bool top_level);
//Pre: data_expr is a data expression that adheres to the internal syntax after
//     data implementation
//     top_level indicates if data_expr contains the top-level operation
//Ret: data_expr in which numerical patterns can be matched if they do not occur
//     at top level

static ATermAppl impl_set_enum(ATermList elts, ATermAppl sort_expr);
//Pre: elts is a list containing 1 or more data expressions, all of the same
//     sort
//     sort_expr is a sort expression
//Ret: Implementation of the set enumeration of the elements in elts with
//     result sort sort_expr

static ATermAppl impl_bag_enum(ATermList elts, ATermAppl sort_expr);
//Pre: elts is a list containing 1 or more data expressions, all of the same
//     sort
//     sort_expr is a sort expression
//Ret: Implementation of the bag enumeration of the elements in elts with
//     result sort sort_expr

static void split_sort_decls(ATermList sort_decls, ATermList *p_sort_ids,
  ATermList *p_sort_refs);
//Pre: sort_decls is a list of sort_id's and sort_ref's
//Post:*p_sort_ids and *p_sort_refs contain the sort_id's and sort_ref's from
//     sort_decls, in the same order

static ATermAppl make_fresh_struct_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: sort identifier for the implementation of a structured sort with prefix
//     gsSortStructPrefix, that does not occur in term

static ATermAppl make_fresh_list_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a list sort with prefix
//     gsSortListPrefix, that does not occur in term

static ATermAppl make_fresh_set_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a set sort with prefix
//     gsSortSetPrefix, that does not occur in term

static ATermAppl make_fresh_fset_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a finite set sort with prefix
//     gsSortFSetPrefix, that does not occur in term

static ATermAppl make_fresh_bag_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a bag sort with prefix
//     gsSortBagPrefix, that does not occur in term

static ATermAppl make_fresh_fbag_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a finite bag sort with prefix
//     gsSortFBagPrefix, that does not occur in term

static ATermAppl make_fresh_lambda_op_id(ATermAppl sort_expr, ATerm term);
//Pre: sort_expr is a sort expression
//     term is not NULL
//Ret: operation identifier op_id(n, s) for the implementation of a lambda
//     abstraction, where s is sort_expr and n is a name with prefix
//     gsLambdaPrefix, that does not occur in term


//implementation
//--------------

ATermAppl implement_data_spec(ATermAppl spec, ATermList* substitution_context)
{
  assert(gsIsDataSpec(spec) || gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec));
  int occ =
    gsCount((ATerm) gsMakeSortUnknown(), (ATerm) spec) +
    gsCountAFun(gsAFunSortsPossible(), (ATerm) spec);
  if (occ > 0) {
    gsErrorMsg("specification contains %d unresolved type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }
  //implement system sorts and data expressions occurring in spec
  ATermList new_data_equations = ATmakeList0();
  t_data_decls declarations;
  initialize_data_decls(&declarations);
  spec = impl_exprs_appl(spec, substitution_context, &declarations, &new_data_equations);
  //perform substitutions on data declarations
  declarations.sorts     = gsSubstValues_List(*substitution_context, declarations.sorts,     true);
  declarations.cons_ops  = gsSubstValues_List(*substitution_context, declarations.cons_ops,  true);
  declarations.ops       = gsSubstValues_List(*substitution_context, declarations.ops,       true);
  declarations.data_eqns = gsSubstValues_List(*substitution_context, declarations.data_eqns, true);
  //add implementation of sort Bool
  impl_sort_bool(&declarations);
  //add new data declarations to spec
  spec = add_data_decls(spec, declarations);
  //implement numerical pattern matching
  spec = impl_numerical_pattern_matching(spec);
  //implement sort references
  spec = impl_sort_refs(spec, substitution_context);
  //implement standard functions
  spec = impl_standard_functions_spec(spec);
  return spec;
}

ATermAppl impl_data_action_rename_spec_detail(ATermAppl ar_spec, ATermAppl& lps_spec)
{
  assert(gsIsActionRenameSpec(ar_spec));
  assert(gsIsLinProcSpec(lps_spec));

  int occ =
    gsCount((ATerm) gsMakeSortUnknown(), (ATerm) lps_spec) +
    gsCountAFun(gsAFunSortsPossible(), (ATerm) lps_spec);
  if (occ > 0) {
    gsErrorMsg("specification contains %d unresolved type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }
  occ = gsCount((ATerm) gsMakeSortUnknown(), (ATerm) ar_spec);
  if (occ > 0) {
    gsErrorMsg("action rename specification contains %d unknown type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }

  //append data declarations and action declarations for the ar_spec to the lps_spec
  t_data_decls lps_data = get_data_decls(lps_spec);
  t_data_decls ar_data = get_data_decls(ar_spec);
  concat_data_decls(&lps_data, &ar_data);
  lps_spec = set_data_decls(lps_spec, lps_data);
  ATermList lps_actions = ATLgetArgument(ATAgetArgument(lps_spec, 1), 0);
  ATermList ar_actions = ATLgetArgument(ATAgetArgument(ar_spec, 1), 0);
  ATermList new_actions = ATconcat(lps_actions, ar_actions);
  lps_spec = ATsetArgument(lps_spec, (ATerm)gsMakeActSpec(new_actions), 1);
  //remove decls from ar_spec
  initialize_data_decls(&ar_data);
  ar_spec = set_data_decls(ar_spec, ar_data);
  ar_spec = ATsetArgument(ar_spec, (ATerm)gsMakeActSpec(ATmakeList0()), 1);

  //implement system sorts and data expressions occurring in spec
  ATermList substs     = ATmakeList0();
  ATermList new_data_equations = ATmakeList0();
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  lps_spec = impl_exprs_appl(lps_spec, &substs, &data_decls, &new_data_equations);
  ar_spec = impl_exprs_appl(ar_spec, &substs, &data_decls, &new_data_equations);
  //perform substitutions on data declarations
  data_decls.sorts     = gsSubstValues_List(substs, data_decls.sorts,     true);
  data_decls.cons_ops  = gsSubstValues_List(substs, data_decls.cons_ops,  true);
  data_decls.ops       = gsSubstValues_List(substs, data_decls.ops,       true);
  data_decls.data_eqns = gsSubstValues_List(substs, data_decls.data_eqns, true);
  //add implementation of sort Bool
  impl_sort_bool(&data_decls);
  //add new data declarations to spec
  lps_spec = add_data_decls(lps_spec, data_decls);
  // We need to copy the data declarations of the lps_spec to the ar_spec.
  // Just adding data_decls does not suffice. This causes implementation of sort
  // references (caused by structured sorts) to fail.
  ar_spec = ATsetArgument(ar_spec, ATgetArgument(lps_spec, 0), 0);
  //implement numerical pattern matching
  lps_spec = impl_numerical_pattern_matching(lps_spec);
  ar_spec = impl_numerical_pattern_matching(ar_spec);
  //implement sort references
  lps_spec = impl_sort_refs(lps_spec, &substs);
  ar_spec = impl_sort_refs(ar_spec, &substs);
  //implement standard functions
  lps_spec = impl_standard_functions_spec(lps_spec);
  ar_spec = impl_standard_functions_spec(ar_spec);
  return ar_spec;
}

ATermList compute_sort_ref_substs(ATermAppl spec)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsActionRenameSpec(spec) || gsIsDataSpec(spec));
  //get sort declarations
  ATermAppl data_spec;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermAppl sort_spec = ATAgetArgument(data_spec, 0);
  ATermList sort_decls = ATLgetArgument(sort_spec, 0);
  //split sort declarations in sort id's and sort references
  ATermList sort_ids = NULL;
  ATermList sort_refs = NULL;
  split_sort_decls(sort_decls, &sort_ids, &sort_refs);
  //make list of substitutions from sort_refs, the list of sort references
  ATermList substs = ATmakeList0();
  while (!ATisEmpty(sort_refs))
  {
    ATermAppl sort_ref = ATAgetFirst(sort_refs);
    ATermAppl lhs = gsMakeSortId(ATAgetArgument(sort_ref, 0));
    ATermAppl rhs = ATAgetArgument(sort_ref, 1);
    //if rhs is the first occurrence of an implementation of a type constructor
    //at the rhs of a sort reference, add rhs := lhs; otherwise add lhs := rhs
    ATermAppl subst;
    if (gsIsStructSortId(rhs) || gsIsListSortId(rhs) || gsIsSetSortId(rhs) ||
      gsIsBagSortId(rhs))
    {
      subst = gsMakeSubst_Appl(rhs, lhs);
    } else {
      subst = gsMakeSubst_Appl(lhs, rhs);
    }
    substs = ATinsert(substs, (ATerm) subst);
    //perform substitution on the remaining elements of sort_refs
    sort_refs = ATgetNext(sort_refs);    
    sort_refs = gsSubstValues_List(ATmakeList1((ATerm) subst), sort_refs, true);
  }
  return substs;
}

ATermAppl impl_sort_refs(ATermAppl spec, ATermList* substitution_context)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsActionRenameSpec(spec) || gsIsDataSpec(spec));
  //get sort declarations
  ATermAppl data_spec;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermAppl sort_spec = ATAgetArgument(data_spec, 0);
  ATermList sort_decls = ATLgetArgument(sort_spec, 0);
  //split sort declarations in sort id's and sort references
  ATermList sort_ids = NULL;
  ATermList sort_refs = NULL;
  split_sort_decls(sort_decls, &sort_ids, &sort_refs);
  //replace the sort declarations in spec by the sort_ids, the list of
  //identifiers
  sort_spec = ATsetArgument(sort_spec, (ATerm) sort_ids, 0);  
  data_spec = ATsetArgument(data_spec, (ATerm) sort_spec, 0);
  if (gsIsDataSpec(spec)) {
    spec = data_spec;
  } else {
    spec = ATsetArgument(spec, (ATerm) data_spec, 0);
  }
  //make list of substitutions from sort_refs, the list of sort references
  //ATermList substs = ATmakeList0();
  while (!ATisEmpty(sort_refs))
  {
    ATermAppl sort_ref = ATAgetFirst(sort_refs);
    ATermAppl lhs = gsMakeSortId(ATAgetArgument(sort_ref, 0));
    ATermAppl rhs = ATAgetArgument(sort_ref, 1);
    //if rhs is the first occurrence of an implementation of a type constructor
    //at the rhs of a sort reference, add rhs := lhs; otherwise add lhs := rhs
    ATermAppl subst;
    if (gsIsStructSortId(rhs) || gsIsListSortId(rhs) || gsIsSetSortId(rhs) ||
      gsIsBagSortId(rhs))
    {
      subst = gsMakeSubst_Appl(rhs, lhs);
    } else {
      subst = gsMakeSubst_Appl(lhs, rhs);
    }
    *substitution_context = gsAddSubstToSubsts(subst, *substitution_context);
//    substs = ATinsert(substs, (ATerm) subst);
    //perform substitution on the remaining elements of sort_refs
    sort_refs = ATgetNext(sort_refs);
    sort_refs = gsSubstValues_List(ATmakeList1((ATerm) subst), sort_refs, true);
  }
  //perform substitutions on spec
  spec = gsSubstValues_Appl(*substitution_context, spec, true);
  return spec;
}

ATermAppl impl_standard_functions_spec(ATermAppl spec)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsActionRenameSpec(spec) || gsIsDataSpec(spec));
  //initalise data declarations
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  //get sorts occurring in spec
  ATermList sorts = get_sorts((ATerm) spec);
  //implement standard functions for each sort in sorts
  while (!ATisEmpty(sorts))
  {
    impl_standard_functions_sort(ATAgetFirst(sorts), &data_decls);
    sorts = ATgetNext(sorts);
  }
  //add new data declarations to spec
  spec = add_data_decls(spec, data_decls);
  return spec;
}

ATermAppl impl_numerical_pattern_matching(ATermAppl spec)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsActionRenameSpec(spec) || gsIsDataSpec(spec));
  //get data equations
  ATermAppl data_spec;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
  ATermAppl data_eqn_spec = ATAgetArgument(data_spec, 3);
  ATermList data_eqns = ATLgetArgument(data_eqn_spec, 0);
  //implement pattern matching for each equation
  ATermList l = ATmakeList0();
  while (!ATisEmpty(data_eqns)) {
    ATermAppl data_eqn = ATAgetFirst(data_eqns);
    //implement pattern matching on the left-hand side of the data equation
    ATermAppl lhs = ATAgetArgument(data_eqn, 2);
    lhs = impl_numerical_pattern_matching_expr(lhs, true);
    data_eqn = ATsetArgument(data_eqn, (ATerm) lhs, 2);
    l = ATinsert(l, (ATerm) data_eqn);
    data_eqns = ATgetNext(data_eqns);
  }
  data_eqns = ATreverse(l);
  data_eqn_spec = ATsetArgument(data_eqn_spec, (ATerm) data_eqns, 0);
  data_spec = ATsetArgument(data_spec, (ATerm) data_eqn_spec, 3);
  if (gsIsDataSpec(spec)) {
    spec = data_spec;
  } else {
    spec = ATsetArgument(spec, (ATerm) data_spec, 0);
  }
  return spec;
}

ATermAppl impl_numerical_pattern_matching_expr(ATermAppl data_expr, bool top_level)
{
  assert(gsIsDataExpr(data_expr));
  if (gsIsDataVarId(data_expr) || gsIsOpId(data_expr)) {
    return data_expr;
  }
  assert(gsIsDataAppl(data_expr));
  if (!top_level) {
    //implement numerical patterns
    if (gsIsDataExprPos2Nat(data_expr)) {
      data_expr = gsMakeDataExprCNat(ATAgetFirst(ATLgetArgument(data_expr, 1)));
    } else if (gsIsDataExprPos2Int(data_expr)) {
      data_expr = gsMakeDataExprCInt(gsMakeDataExprCNat(ATAgetFirst(ATLgetArgument(data_expr, 1))));
    } else if (gsIsDataExprPos2Real(data_expr)) {
      data_expr = gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(ATAgetFirst(ATLgetArgument(data_expr, 1)))), gsMakeDataExprC1());
    } else if (gsIsDataExprNat2Int(data_expr)) {
      data_expr = gsMakeDataExprCInt(ATAgetFirst(ATLgetArgument(data_expr, 1)));
    } else if (gsIsDataExprNat2Real(data_expr)) {
      data_expr = gsMakeDataExprCReal(gsMakeDataExprCInt(ATAgetFirst(ATLgetArgument(data_expr, 1))), gsMakeDataExprC1());
    } else if (gsIsDataExprInt2Real(data_expr)) {
      data_expr = gsMakeDataExprCReal(ATAgetFirst(ATLgetArgument(data_expr, 1)), gsMakeDataExprC1());
    } else if (gsIsDataExprNeg(data_expr)) {
      if (ATisEqual(gsMakeOpIdNeg(gsMakeSortExprPos()),ATAgetArgument(data_expr,0))) {
        data_expr = gsMakeDataExprCNeg(ATAgetFirst(ATLgetArgument(data_expr, 1)));
      }
    }
  }
  //implement pattern matching in the head of data_expr
  ATermAppl head = ATAgetArgument(data_expr, 0);
  head = impl_numerical_pattern_matching_expr(head, top_level);
  //implement pattern matching in the arguments of data_expr
  ATermList args = ATLgetArgument(data_expr, 1);
  ATermList l = ATmakeList0();
  while (!ATisEmpty(args)) {
    ATermAppl arg = ATAgetFirst(args);
    arg = impl_numerical_pattern_matching_expr(arg, false);
    l = ATinsert(l, (ATerm) arg);
    args = ATgetNext(args);
  }
  args = ATreverse(l);
  return gsMakeDataAppl(head, args);
}

ATermAppl impl_exprs_with_spec(ATermAppl part, ATermAppl& spec)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsDataSpec(spec));
  int occ =
    gsCount((ATerm) gsMakeSortUnknown(), (ATerm) spec) +
    gsCountAFun(gsAFunSortsPossible(), (ATerm) spec);
  if (occ > 0) {
    gsErrorMsg("specification contains %d unresolved type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }
  occ = gsCount((ATerm) gsMakeSortUnknown(), (ATerm) part);
  if (occ > 0) {
    gsErrorMsg("part contains %d unknown type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }

  //implement system sorts and data expressions occurring in spec
  ATermList substs     = ATmakeList0();
  ATermList new_data_equations = ATmakeList0();
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  spec = impl_exprs_appl(spec, &substs, &data_decls, &new_data_equations);
  part = impl_exprs_appl(part, &substs, &data_decls, &new_data_equations);
  //perform substitutions on data declarations
  data_decls.sorts     = gsSubstValues_List(substs, data_decls.sorts,     true);
  data_decls.cons_ops  = gsSubstValues_List(substs, data_decls.cons_ops,  true);
  data_decls.ops       = gsSubstValues_List(substs, data_decls.ops,       true);
  data_decls.data_eqns = gsSubstValues_List(substs, data_decls.data_eqns, true);
  //add implementation of sort Bool
  impl_sort_bool(&data_decls);
  //add new data declarations to spec
  spec = add_data_decls(spec, data_decls);
  //implement numerical pattern matching
  spec = impl_numerical_pattern_matching(spec);
  // compute substitutions for sort references
  // and substitute these in the result
  ATermList sort_ref_substs = compute_sort_ref_substs(spec);
  part = gsSubstValues_Appl(sort_ref_substs, part, true);
  //implement sort references
  //note that it is important that this is done AFTER computing the
  //substitutions for part
  spec = impl_sort_refs(spec, &substs);
  //implement standard functions
  spec = impl_standard_functions_spec(spec);
  return part;
}

ATermAppl impl_exprs_appl(ATermAppl part, ATermList *p_substs,
  t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  bool recursive = true;
  //perform substitutions from *p_substs on part
  part = gsSubstValues_Appl(*p_substs, part, false);
  //replace part by an implementation if the head of part is a special
  //expression
  if (gsIsSortArrow(part)) {
    //part is a product arrow sort; skip
  } else if (gsIsSortStruct(part)) {
    //part is a structured sort; replace by a new sort and add data
    //declarations for this sort
    ATermAppl sort_id = make_fresh_struct_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_struct(part, sort_id, p_substs, p_data_decls, true, new_data_equations);
    part = sort_id;
  } else if (gsIsSortExprList(part)) {
    //part is a list sort; replace by a new sort and add data declarations for
    //this sort
    ATermAppl sort_id = make_fresh_list_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_list(part, sort_id, p_substs, p_data_decls, new_data_equations);
    part = sort_id;
  } else if (gsIsSortExprSet(part)) {
    //part is a set sort; replace by a new sort and add data declarations for
    //this sort
    ATermAppl sort_id = make_fresh_set_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_set(part, sort_id, p_substs, p_data_decls, new_data_equations);
    part = sort_id;
  } else if (gsIsSortExprBag(part)) {
    //part is a bag sort; replace by a new sort and add data declarations for
    //this sort
    ATermAppl sort_id = make_fresh_bag_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_bag(part, sort_id, p_substs, p_data_decls, new_data_equations);
    part = sort_id;
  } else if (gsIsSortExprFSet(part)) {
    //part is a finite set sort; replace by a new sort and add data
    //declarations for this sort
    ATermAppl sort_id = make_fresh_fset_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_fset(part, sort_id, p_substs, p_data_decls, new_data_equations);
    part = sort_id;
  } else if (gsIsSortExprFBag(part)) {
    //part is a finite bag sort; replace by a new sort and add data
    //declarations for this sort
    ATermAppl sort_id = make_fresh_fbag_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_fbag(part, sort_id, p_substs, p_data_decls, new_data_equations);
    part = sort_id;
  } else if (gsIsSortId(part)) {
    //part is a sort identifier; add data declarations for this sort, if needed
    if (ATisEqual(part,gsMakeSortIdPos())) {
      //add implementation of sort Pos, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdPos(), 0) == -1) {
        impl_sort_pos(p_data_decls, new_data_equations);
      }
    } else if (ATisEqual(part,gsMakeSortIdNat())) {
      //add implementation of sort Nat, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
        impl_sort_nat(p_data_decls, true, new_data_equations);
      }
    } else if (ATisEqual(part, gsMakeSortIdInt())) {
      //add implementation of sort Int, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
        impl_sort_int(p_data_decls, true, new_data_equations);
      }
    } else if (ATisEqual(part, gsMakeSortIdReal())) {
      //add implementation of sort Int, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdReal(), 0) == -1) {
        impl_sort_real(p_data_decls, true, new_data_equations);
      }
    }
  } else if (gsIsDataAppl(part)) {
    //part is a product data application
    //there are some cases that need implementation work
    ATermList l = ATLgetArgument(part, 1);
    ATermAppl newpart = ATAgetArgument(part, 0);
    if (gsIsOpId(newpart)) {
      ATermAppl Name = ATAgetArgument(newpart, 0);
      if (Name == gsMakeOpIdNameListEnum()) {
        //part is a list enumeration; replace by its internal representation
        ATermAppl sort = gsGetSortExprResult(ATAgetArgument(newpart, 1));
        if (ATgetLength(l) == 0) {
          //enumeration consists of 0 elements
          gsWarningMsg(
            "%P can not be implemented because it has 0 elements\n", newpart);
        } else {
          //make cons list
          l = ATreverse(l);
          part = gsMakeDataExprEmptyList(sort);
          while (!ATisEmpty(l))
          {
            part = gsMakeDataExprCons(ATAgetFirst(l), part);
            l = ATgetNext(l);
          }
        }
      } else if (Name == gsMakeOpIdNameSetEnum()) {
        //part is a set enumeration; replace by a set comprehension
        ATermAppl sort = gsGetSortExprResult(ATAgetArgument(newpart, 1));
        if (ATgetLength(l) == 0) {
          //enumeration consists of 0 elements
          gsWarningMsg(
            "%P can not be implemented because it has 0 elements\n", newpart);
        } else {
          part = impl_set_enum(l, sort);
        }
      } else if (Name == gsMakeOpIdNameBagEnum()) {
        //part is a bag enumeration; replace by a bag comprehension
        ATermAppl sort = gsGetSortExprResult(ATAgetArgument(newpart, 1));
        if (ATgetLength(l) == 0) {
          //enumeration consists of 0 elements
          gsWarningMsg(
            "%P can not be implemented because it has 0 elements\n", newpart);
        } else {
          part = impl_bag_enum(l, sort);
        }
      }
    }
  } else if (gsIsDataExprNumber(part)) {
    //part is a number; replace by its internal representation
    ATermAppl Number = ATAgetArgument(part, 0);
    ATermAppl sort = ATAgetArgument(part, 1);
    if (ATisEqual(sort, gsMakeSortExprPos()))
      part = gsMakeDataExprPos(gsATermAppl2String(Number));
    else if (ATisEqual(sort, gsMakeSortExprNat()))
      part = gsMakeDataExprNat(gsATermAppl2String(Number));
    else if (ATisEqual(sort, gsMakeSortExprInt()))
      part = gsMakeDataExprInt(gsATermAppl2String(Number));
    else if (ATisEqual(sort, gsMakeSortExprReal()))
      part = gsMakeDataExprReal(gsATermAppl2String(Number));
    else //sort of part is wrong
      gsWarningMsg("%P can not be implemented because its sort differs from "
        "Pos, Nat, Int or Real\n", part);
  } else if (gsIsBinder(part)) {
    ATermAppl binding_operator = ATAgetArgument(part, 0);
    if (gsIsSetBagComp(binding_operator)) {
      //part is a set/bag comprehension; replace by its implementation
      ATermList vars = ATLgetArgument(part, 1);
      ATermAppl var = ATAgetFirst(vars);
      ATermAppl body = ATAgetArgument(part, 2);
      ATermAppl body_sort = gsGetSort(body);
      ATermAppl var_sort = gsGetSort(var);
      if (!(ATisEqual(body_sort, gsMakeSortIdBool()) ||
          ATisEqual(body_sort, gsMakeSortIdNat()))) {
        //sort of the comprehension is wrong
        gsWarningMsg("%P can not be implemented because the body is of sort %P "
          "instead of Bool or Nat\n", part, body_sort);
      } else {
        if (ATisEqual(body_sort, gsMakeSortIdBool())) {
          //part is a set comprehension
          part = gsMakeDataExprSetComp(gsMakeBinder(gsMakeLambda(), vars, body), 
            gsMakeSortExprSet(var_sort));
        } else {
          //part is a bag comprehension
          part = gsMakeDataExprBagComp(gsMakeBinder(gsMakeLambda(), vars, body), 
            gsMakeSortExprBag(var_sort));
        }
      }
    } else if (gsIsSetComp(binding_operator)) {
      //part is a set comprehension; replace by its implementation
      ATermList vars = ATLgetArgument(part, 1);
      assert(!ATisEmpty(vars));
      ATermAppl var = ATAgetFirst(vars);
      ATermAppl body = ATAgetArgument(part, 2);
      ATermAppl var_sort = gsGetSort(var);
      part = gsMakeDataExprSetComp(gsMakeBinder(gsMakeLambda(), vars, body), 
        gsMakeSortExprSet(var_sort));
    } else if (gsIsBagComp(binding_operator)) {
      //part is a bag comprehension; replace by its implementation
      ATermList vars = ATLgetArgument(part, 1);
      ATermAppl var = ATAgetFirst(vars);
      ATermAppl body = ATAgetArgument(part, 2);
      ATermAppl var_sort = gsGetSort(var);
      part = gsMakeDataExprBagComp(gsMakeBinder(gsMakeLambda(), vars, body),
        gsMakeSortExprBag(var_sort));
    } else if (gsIsForall(binding_operator) || gsIsExists(binding_operator)) {
      //part is a quantification; replace by its implementation
      ATermList vars = ATLgetArgument(part, 1);
      bool is_forall = gsIsForall(binding_operator);
      part = gsMakeBinder(gsMakeLambda(), vars, ATAgetArgument(part, 2));
      part = is_forall?gsMakeDataExprForall(part):gsMakeDataExprExists(part);
    } else if (gsIsLambda(binding_operator)) {
      //part is a lambda abstraction; replace by a named function
      //implement the body, the bound variables and the free variables
      ATermList bound_vars = impl_exprs_list(ATLgetArgument(part, 1),
        p_substs, p_data_decls, new_data_equations);
      assert(!ATisEmpty(bound_vars));
      ATermAppl body = impl_exprs_appl(ATAgetArgument(part, 2),
        p_substs, p_data_decls, new_data_equations);
      ATermList free_vars = impl_exprs_list(get_free_vars(part),
        p_substs, p_data_decls, new_data_equations);
      //create sort for the new operation identifier
      ATermAppl op_id_sort = gsMakeSortArrowList(gsGetSorts(free_vars), gsMakeSortArrowList(gsGetSorts(bound_vars), gsGetSort(body)));
      //create new operation identifier
      ATermAppl op_id = make_fresh_lambda_op_id(op_id_sort, (ATerm) p_data_decls->ops);
      //add operation identifier to the data declarations
      p_data_decls->ops = ATinsert(p_data_decls->ops, (ATerm) op_id);
      //add data equation for the operation to the data declarations
      ATerm new_equation = (ATerm)
        gsMakeDataEqn(ATconcat(free_vars, bound_vars), gsMakeNil(), gsMakeDataApplList(gsMakeDataApplList(op_id, free_vars), bound_vars), body);

      p_data_decls->data_eqns = ATinsert(p_data_decls->data_eqns, new_equation);
      ATinsert(*new_data_equations, new_equation);
      //replace part
      ATermAppl new_part = gsMakeDataApplList(op_id, free_vars);
      *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(part, new_part), *p_substs);
      part = new_part;
      recursive = false;
    }
  } else if (gsIsWhr(part)) {
    //part is a where clause; replace by its corresponding lambda expression
    ATermAppl body = ATAgetArgument(part, 0);
    ATermList whr_decls = ATLgetArgument(part, 1);
    if (ATgetLength(whr_decls) == 0) {
      //where clause consists of 0 where clause declarations
      gsWarningMsg("%P can not be implemented because it has 0 where clause "
         "declarations\n", part);
    } else {
      //make list of variables and where expressions
      whr_decls = ATreverse(whr_decls);
      ATermList vars = ATmakeList0();
      ATermList exprs = ATmakeList0();
      while (!ATisEmpty(whr_decls))
      {
        ATermAppl whr_decl = ATAgetFirst(whr_decls);
        ATermAppl var = ATAgetArgument(whr_decl, 0);
        ATermAppl expr = ATAgetArgument(whr_decl, 1);
        vars = ATinsert(vars, (ATerm) var);
        exprs = ATinsert(exprs, (ATerm) expr);
        whr_decls = ATgetNext(whr_decls);
      }
      //replace part
      part = gsMakeDataApplList(gsMakeBinder(gsMakeLambda(), vars, body), exprs);
    }
  }
  //implement expressions in the arguments of part
  if (recursive) {
    AFun head = ATgetAFun(part);
    int nr_args = ATgetArity(head);
    if (nr_args > 0) {
      DECL_A(args,ATerm,nr_args);
      for (int i = 0; i < nr_args; i++) {
        ATerm arg = ATgetArgument(part, i);
        if (ATgetType(arg) == AT_APPL)
          args[i] = (ATerm) impl_exprs_appl((ATermAppl) arg, p_substs,
            p_data_decls, new_data_equations);
        else //ATgetType(arg) == AT_LIST
          args[i] = (ATerm) impl_exprs_list((ATermList) arg, p_substs,
            p_data_decls, new_data_equations);
      }
      part = ATmakeApplArray(head, args);
      FREE_A(args);
    }
  }
  return part;
}

ATermList impl_exprs_list(ATermList parts, ATermList *p_substs,
  t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  ATermList result = ATmakeList0();
  while (!ATisEmpty(parts))
  {
    result = ATinsert(result, (ATerm)
      impl_exprs_appl(ATAgetFirst(parts), p_substs, p_data_decls, new_data_equations));
    parts = ATgetNext(parts);
  }
  return ATreverse(result);
}

ATermAppl impl_set_enum(ATermList elts, ATermAppl sort_expr)
{
  assert(ATgetLength(elts) > 0);
  ATermAppl fset_expr = gsMakeDataExprFSetEmpty(gsMakeSortExprFSet(gsGetSort(ATAgetFirst(elts))));
  for (ATermList l = ATreverse(elts) ; !ATisEmpty(l) ; l = ATgetNext(l)) {
    fset_expr = gsMakeDataExprFSetInsert(ATAgetFirst(l), fset_expr);
  }
  return gsMakeDataExprSetFSet(fset_expr, sort_expr);
}

ATermAppl impl_bag_enum(ATermList elts, ATermAppl sort_expr)
{
  assert(ATgetLength(elts) > 0);
  ATermAppl fbag_expr = gsMakeDataExprFBagEmpty(gsMakeSortExprFBag(gsGetSort(ATAgetFirst(elts))));
  for (ATermList l = ATreverse(elts) ; !ATisEmpty(l) ; l = ATgetNext(l)) {
    ATermAppl amt = ATAgetFirst(l);
    l = ATgetNext(l);
    ATermAppl elt = ATAgetFirst(l);
    fbag_expr = gsMakeDataExprFBagCInsert(elt, amt, fbag_expr);
  }
  return gsMakeDataExprBagFBag(fbag_expr, sort_expr);
}

ATermList create_op_id_args(ATermAppl op_id,
  ATermList *p_vars, ATerm context)
{
  ATermAppl sort = gsGetSort(op_id);
  assert(gsIsSortId(sort) || gsIsSortArrow(sort));

  //sort identifier case...
  if (gsIsSortId(sort)) {
    return ATmakeList0();
  }
  
  //sort arrow case...
  assert(gsIsSortArrow(sort));
  
  ATermList args = ATmakeList0(); //args contains the result in reverse order
  ATermList tmpvars = *p_vars; // We only use variables once in a term

  ATermList sort_dom = ATLgetArgument(sort, 0);
  while ( !ATisEmpty(sort_dom) )
  {
    // Find a variable with the right sort...
    ATermAppl sort_dom_elt = ATAgetFirst(sort_dom);
    ATermAppl v = NULL;
    for (ATermList n=tmpvars; !ATisEmpty(n) && (v == NULL); n=ATgetNext(n))
    {
      ATermAppl tmpvar = ATAgetFirst(n);
      if ( ATisEqual(gsGetSort(tmpvar), sort_dom_elt) )
      {
        v = tmpvar;
        tmpvars = ATremoveElement(tmpvars,(ATerm) v);
      }
    }
    // ...or create a new one
    if ( v == NULL )
    {
      v = gsMakeDataVarId(gsFreshString2ATermAppl("v",
            (ATerm) ATmakeList2((ATerm) (*p_vars),context),false), sort_dom_elt);
      // Add it to *p_vars
      *p_vars = ATinsert(*p_vars,(ATerm) v);
    }
    assert(v != NULL);
 
    // Add v to args
    args = ATinsert(args, (ATerm) v);

    // Next
    sort_dom = ATgetNext(sort_dom);
  }
  
  // reverse *p_vars
  *p_vars = ATreverse(*p_vars);

  // return args in reversed order
  return ATreverse(args);
}

void impl_sort_struct(ATermAppl sort_struct, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls, bool recursive, ATermList* new_data_equations)
{
  assert(gsIsSortStruct(sort_struct));
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);

  // declare fresh sort identifier for sort_struct 
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);

  // structured sort
  structured_sort element_sort(static_cast<sort_expression>(sort_struct));

  // add substitution sort_struct -> sort_id
  ATermAppl subst = gsMakeSubst_Appl(sort_struct, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  function_symbol_vector constructors = element_sort.constructor_functions(sort_expression(sort_id));
  function_symbol_vector functions;
  function_symbol_vector projection_functions = element_sort.projection_functions(sort_expression(sort_id));
  function_symbol_vector recogniser_functions = element_sort.recogniser_functions(sort_expression(sort_id));

  // implement argument sorts 
  for (function_symbol_vector::const_iterator i = projection_functions.begin();
                                        recursive && i != projection_functions.end(); ++i) {
    impl_exprs_appl(function_sort(i->sort()).codomain(), p_substs, p_data_decls, new_data_equations);
  }

  std::copy(projection_functions.begin(), projection_functions.end(),
                                std::back_insert_iterator< function_symbol_vector >(functions));
  std::copy(recogniser_functions.begin(), recogniser_functions.end(),
                                std::back_insert_iterator< function_symbol_vector >(functions));

  data_equation_vector   equations            = element_sort.constructor_equations(sort_expression(sort_id));
  data_equation_vector   projection_equations = element_sort.projection_equations(sort_expression(sort_id));
  data_equation_vector   recogniser_equations = element_sort.recogniser_equations(sort_expression(sort_id));

  std::copy(projection_equations.begin(), projection_equations.end(),
                                std::back_insert_iterator< data_equation_vector >(equations));
  std::copy(recogniser_equations.begin(), recogniser_equations.end(),
                                std::back_insert_iterator< data_equation_vector >(equations));

  ATermList new_data_eqns = atermpp::term_list<data_equation>(equations.begin(), equations.end());
  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls, new_data_equations);

  p_data_decls->cons_ops = ATconcat(gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(gsSubstValues_List(*p_substs, new_data_eqns, true), p_data_decls->data_eqns);
  ATermList new_equations = gsSubstValues_List(*p_substs, new_data_eqns, true);

  if (new_data_equations) {
    *new_data_equations = ATconcat(new_equations, *new_data_equations);
  }
}

ATermList build_list_equations(ATermAppl sort_elt, ATermAppl sort_list)
{
  data_equation_vector equations = sort_list::list_generate_equations_code(sort_expression(sort_elt));
  // Workaround to keep data reconstruction happy
  // Apply substitution sort_list::list(sort_elt) := sort_list
  ATerm subst1 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_list::list(sort_expression(sort_elt))), sort_list);
  ATerm subst2 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_expression(sort_elt)), sort_elt);
  ATermList result = atermpp::term_list<data_equation>(equations.begin(), equations.end());
  result = gsSubstValues_List(ATmakeList2(subst1, subst2), result, true);
  return result;
}

void impl_sort_list(ATermAppl sort_list, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortExprList(sort_list));
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);

  ATermAppl sort_elt = ATAgetArgument(sort_list, 1);
  sort_expression element_sort(sort_elt);

  function_symbol_vector constructors = sort_list::list_generate_constructors_code(element_sort);
  function_symbol_vector functions = sort_list::list_generate_functions_code(element_sort);
  data_equation_vector equations = sort_list::list_generate_equations_code(element_sort);

  //declare sort sort_id as representative of sort sort_list
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);

  // Do not implement the element sort too soon, as this breaks substitutions
  sort_elt = impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);
  //add substitution for sort_list
  ATermAppl subst = gsMakeSubst_Appl(sort_list, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //perform substitutions
  ATermList new_constructors = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true);
  p_data_decls->cons_ops = ATconcat(new_constructors, p_data_decls->cons_ops);
  ATermList new_mappings = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true);
  p_data_decls->ops = ATconcat(new_mappings, p_data_decls->ops);
  ATermList new_equations = gsSubstValues_List(*p_substs, atermpp::term_list<data_equation>(equations.begin(), equations.end()), true);
  p_data_decls->data_eqns = ATconcat(new_equations, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_equations, *new_data_equations);

  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls, true, new_data_equations);
  }
}

ATermList build_fset_equations(ATermAppl sort_elt, ATermAppl sort_fset_id)
{
  data_equation_vector equations = sort_fset::fset_generate_equations_code(sort_expression(sort_elt));
  // Workaround to keep data reconstruction happy
  ATerm subst1 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_fset::fset(sort_expression(sort_elt))), sort_fset_id);
  ATerm subst2 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_expression(sort_elt)), sort_elt);
  ATermList result = atermpp::term_list<data_equation>(equations.begin(), equations.end());
  result = gsSubstValues_List(ATmakeList2(subst1, subst2), result, true);
  return result;
}

void impl_sort_fset(ATermAppl sort_fset, ATermAppl sort_fset_id, ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortId(sort_fset_id));
  assert(gsIsSortExprFSet(sort_fset));
  assert(gsCount((ATerm) sort_fset_id, (ATerm) p_data_decls->sorts) == 0);

  //declare sort sort_fset_id as representative of sort sort_fset
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_fset_id);

  //implement expressions in the target sort of sort_fset
  //this needs to be done first to keep the substitutions sound!
  ATermAppl sort_elt = ATAgetArgument(sort_fset, 1);
  sort_expression element_sort(sort_elt);
  impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);

  //add substitution sort_fset -> sort_fset_id
  ATermAppl subst = gsMakeSubst_Appl(sort_fset, sort_fset_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  function_symbol_vector constructors = sort_fset::fset_generate_constructors_code(element_sort);
  function_symbol_vector functions = sort_fset::fset_generate_functions_code(element_sort);
  data_equation_vector equations = sort_fset::fset_generate_equations_code(element_sort);

  //perform substitutions
  ATermList new_constructors = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true);
  p_data_decls->cons_ops = ATconcat(new_constructors, p_data_decls->cons_ops);

  ATermList new_mappings = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true);
  p_data_decls->ops = ATconcat(new_mappings, p_data_decls->ops);

  ATermList new_equations = gsSubstValues_List(*p_substs, atermpp::term_list<data_equation>(equations.begin(), equations.end()), true);
  p_data_decls->data_eqns = ATconcat(new_equations, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_equations, *new_data_equations);
}

ATermList build_set_equations(ATermAppl sort_elt, ATermAppl sort_fset_id, ATermAppl sort_set_id)
{
  data_equation_vector equations = sort_set_::set__generate_equations_code(sort_expression(sort_elt));
  // Workaround to keep data reconstruction happy
  ATerm subst1 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_set_::set_(sort_expression(sort_elt))), sort_set_id);
  ATerm subst1a = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_fset::fset(sort_expression(sort_elt))), sort_fset_id);
  ATerm subst2 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_expression(sort_elt)), sort_elt);
  ATermList result = atermpp::term_list<data_equation>(equations.begin(), equations.end());
  result = gsSubstValues_List(ATmakeList3(subst1, subst1a, subst2), result, true);
  return result;
}

void impl_sort_set(ATermAppl sort_set, ATermAppl sort_set_id, ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortExprSet(sort_set));
  assert(gsIsSortId(sort_set_id));
  assert(gsCount((ATerm) sort_set_id, (ATerm) p_data_decls->sorts) == 0);

  //declare sort sort_set_id as representative of sort sort_set
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_set_id);

  //implement expressions in the target sort of sort_set
  //this needs to be done first to keep the substitutions sound!
  ATermAppl sort_elt = ATAgetArgument(sort_set, 1);
  sort_expression element_sort(sort_elt);

  impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);

  //add substitution sort_set -> sort_set_id
  ATermAppl subst = gsMakeSubst_Appl(sort_set, sort_set_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);


  //add implementation of finite sets of sort sort_elt, if necessary
  ATermAppl sort_fset = gsMakeSortExprFSet(sort_elt);
  ATermAppl sort_fset_id =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_fset, false);
  if (ATisEqual(sort_fset_id, sort_fset)) {
    //sort FSet(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    sort_fset_id = make_fresh_fset_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_fset(sort_fset, sort_fset_id, p_substs, p_data_decls, new_data_equations);
  }

  function_symbol_vector constructors = sort_set_::set__generate_constructors_code(element_sort);
  function_symbol_vector functions = sort_set_::set__generate_functions_code(element_sort);
  data_equation_vector equations = sort_set_::set__generate_equations_code(element_sort);

  // Implement equations as quantifications occur in the right hand side
  ATermList new_equations = impl_exprs_list(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_substs, p_data_decls, new_data_equations);

  //perform substitutions
  ATermList new_constructors = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true);
  p_data_decls->cons_ops = ATconcat(new_constructors, p_data_decls->cons_ops);

  ATermList new_mappings = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true);
  p_data_decls->ops = ATconcat(new_mappings, p_data_decls->ops);

  new_equations = gsSubstValues_List(*p_substs, new_equations, true);
  p_data_decls->data_eqns = ATconcat(new_equations, p_data_decls->data_eqns);

  *new_data_equations = ATconcat(new_equations, *new_data_equations);
}

ATermList build_fbag_equations(ATermAppl sort_elt, ATermAppl sort_fset_id, ATermAppl sort_fbag_id)
{
    data_equation_vector equations = sort_fbag::fbag_generate_equations_code(sort_expression(sort_elt));
  // Workaround to keep data reconstruction happy
  ATerm subst1 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_fbag::fbag(sort_expression(sort_elt))), sort_fbag_id);
  ATerm subst2 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_expression(sort_elt)), sort_elt);
  ATermList result = atermpp::term_list<data_equation>(equations.begin(), equations.end());
  result = gsSubstValues_List(ATmakeList2(subst1, subst2), result, true);
  return result;
}

void impl_sort_fbag(ATermAppl sort_fbag, ATermAppl sort_fbag_id, ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortId(sort_fbag_id));
  assert(gsIsSortExprFBag(sort_fbag));
  assert(gsCount((ATerm) sort_fbag_id, (ATerm) p_data_decls->sorts) == 0);

  //declare sort sort_fbag_id as representative of sort sort_fbag
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_fbag_id);

  //implement expressions in the target sort of sort_fbag
  //this needs to be done first to keep the substitutions sound!
  ATermAppl sort_elt = ATAgetArgument(sort_fbag, 1);
  sort_expression element_sort(sort_elt);
  impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);

  //add substitution sort_fset -> sort_fset_id
  ATermAppl subst = gsMakeSubst_Appl(sort_fbag, sort_fbag_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //add implementation of finite sets of sort sort_elt, if necessary
  ATermAppl sort_fset = gsMakeSortExprFSet(sort_elt);
  ATermAppl sort_fset_id =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_fset, false);
  if (ATisEqual(sort_fset_id, sort_fset)) {
    //sort FSet(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    sort_fset_id = make_fresh_fset_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_fset(sort_fset, sort_fset_id, p_substs, p_data_decls, new_data_equations);
  }

  function_symbol_vector constructors = sort_fbag::fbag_generate_constructors_code(element_sort);
  function_symbol_vector functions = sort_fbag::fbag_generate_functions_code(element_sort);
  data_equation_vector equations = sort_fbag::fbag_generate_equations_code(element_sort);

  //perform substitutions
  ATermList new_constructors = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true);
  p_data_decls->cons_ops = ATconcat(new_constructors, p_data_decls->cons_ops);

  ATermList new_mappings = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true);
  p_data_decls->ops = ATconcat(new_mappings, p_data_decls->ops);

  ATermList new_equations = gsSubstValues_List(*p_substs, atermpp::term_list<data_equation>(equations.begin(), equations.end()), true);
  p_data_decls->data_eqns = ATconcat(new_equations, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_equations, *new_data_equations);
}

ATermList build_bag_equations(ATermAppl sort_elt, ATermAppl sort_fset_id, ATermAppl sort_fbag_id, ATermAppl sort_set_id, ATermAppl sort_bag_id)
{
    data_equation_vector equations = sort_bag::bag_generate_equations_code(sort_expression(sort_elt));
  // Workaround to keep data reconstruction happy
  ATerm subst1 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_bag::bag(sort_expression(sort_elt))), sort_bag_id);
  ATerm subst1a = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_fbag::fbag(sort_expression(sort_elt))), sort_fbag_id);
  ATerm subst1b = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_set_::set_(sort_expression(sort_elt))), sort_set_id);
  ATerm subst1c = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_fset::fset(sort_expression(sort_elt))), sort_fset_id);
  ATerm subst2 = (ATerm)gsMakeSubst_Appl(aterm_appl(sort_expression(sort_elt)), sort_elt);
  ATermList result = atermpp::term_list<data_equation>(equations.begin(), equations.end());
  result = gsSubstValues_List(ATmakeList5(subst1, subst1a, subst1b, subst1c, subst2), result, true);
  return result;
}

void impl_sort_bag(ATermAppl sort_bag, ATermAppl sort_bag_id, ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortExprBag(sort_bag));
  assert(gsIsSortId(sort_bag_id));
  assert(gsCount((ATerm) sort_bag_id, (ATerm) p_data_decls->sorts) == 0);

  //declare sort sort_bag_id as representative of sort sort_bag
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_bag_id);

  //implement expressions in the target sort of sort_bag
  //this needs to be done first in order to keep the substitutions sound!
  ATermAppl sort_elt = ATAgetArgument(sort_bag, 1);
  sort_expression element_sort(sort_elt);
  impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);

  //add implementation of sort Set(sort_elt), if necessary
  ATermAppl sort_set = gsMakeSortExprSet(sort_elt);
  ATermAppl sort_set_id =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_set, false);
  if (ATisEqual(sort_set_id, sort_set)) {
    //Set(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    sort_set_id = make_fresh_set_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_set(sort_set, sort_set_id, p_substs, p_data_decls, new_data_equations);
  }

  //add substitution sort_bag -> sort_bag_id
  ATermAppl subst = gsMakeSubst_Appl(sort_bag, sort_bag_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //add implementation of finite bags of sort sort_elt, if necessary
  ATermAppl sort_fbag = gsMakeSortExprFBag(sort_elt);
  ATermAppl sort_fbag_id =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_fbag, false);
  if (ATisEqual(sort_fbag_id, sort_fbag)) {
    //sort FBag(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    sort_fbag_id = make_fresh_fbag_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_fbag(sort_fbag, sort_fbag_id, p_substs, p_data_decls, new_data_equations);
  }

  function_symbol_vector constructors = sort_bag::bag_generate_constructors_code(element_sort);
  function_symbol_vector functions = sort_bag::bag_generate_functions_code(element_sort);
  data_equation_vector equations = sort_bag::bag_generate_equations_code(element_sort);

  // Implement equations because right hand sides contain quantifiers
  ATermList new_equations = impl_exprs_list(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_substs, p_data_decls, new_data_equations);

  //perform substitutions
  ATermList new_constructors = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true);
  p_data_decls->cons_ops = ATconcat(new_constructors, p_data_decls->cons_ops);

  ATermList new_mappings = gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true);
  p_data_decls->ops = ATconcat(new_mappings, p_data_decls->ops);

  new_equations = gsSubstValues_List(*p_substs, new_equations, true);
  p_data_decls->data_eqns = ATconcat(new_equations, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_equations, *new_data_equations);
}

void impl_sort_bool(t_data_decls *p_data_decls)
{
  //Declare sort Bool
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_bool_::bool_()));
  function_symbol_vector constructors = sort_bool_::bool__generate_constructors_code();
  function_symbol_vector functions = sort_bool_::bool__generate_functions_code();
  data_equation_vector equations = sort_bool_::bool__generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);
}

void impl_sort_pos(t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  //Declare sort Pos
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_pos::pos()));
  function_symbol_vector constructors = sort_pos::pos_generate_constructors_code();
  function_symbol_vector functions = sort_pos::pos_generate_functions_code();
  data_equation_vector equations = sort_pos::pos_generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);
  if (new_data_equations) {
    *new_data_equations = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), *new_data_equations);
  }
}

void impl_sort_nat(t_data_decls *p_data_decls, bool recursive, ATermList* new_data_equations)
{
  //Declare sort NatPair
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_nat::natpair()));
  //Declare sort Nat
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_nat::nat()));
  function_symbol_vector constructors = sort_nat::nat_generate_constructors_code();
  function_symbol_vector functions = sort_nat::nat_generate_functions_code();
  data_equation_vector equations = sort_nat::nat_generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);

  if (new_data_equations) {
    *new_data_equations =  ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), *new_data_equations);
  }

  //add implementation of sort Pos, if necessary
  if (recursive && ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdPos(), 0) == -1) {
    impl_sort_pos(p_data_decls, new_data_equations);
  }
}

void impl_sort_int(t_data_decls *p_data_decls, bool recursive, ATermList* new_data_equations)
{
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_int_::int_()));
  function_symbol_vector constructors = sort_int_::int__generate_constructors_code();
  function_symbol_vector functions = sort_int_::int__generate_functions_code();
  data_equation_vector equations = sort_int_::int__generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);

  if (new_data_equations) {
    *new_data_equations = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), *new_data_equations);
  }

  //add implementation of sort Nat, if necessary
  if (recursive && ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls, recursive, new_data_equations);
  }
}

void impl_sort_real(t_data_decls *p_data_decls, bool recursive, ATermList* new_data_equations)
{
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_real_::real_()));
  function_symbol_vector constructors = sort_real_::real__generate_constructors_code();
  function_symbol_vector functions = sort_real_::real__generate_functions_code();
  data_equation_vector equations = sort_real_::real__generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);

  if (new_data_equations) {
    *new_data_equations = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), *new_data_equations);
  }

  //add implementation of sort Int, if necessary
  if (recursive && ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
    impl_sort_int(p_data_decls, recursive, new_data_equations);
  }
}

void impl_standard_functions_sort(ATermAppl sort, t_data_decls *p_data_decls)
{
  assert(gsIsSortExpr(sort));
  //Declare operations for sort
  function_symbol_vector ops = standard_generate_functions_code(sort_expression(sort));
  data_equation_vector eqns = standard_generate_equations_code(sort_expression(sort));

  p_data_decls->ops = ATconcat(aterm_list(ops.begin(), ops.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(aterm_list(eqns.begin(), eqns.end()), p_data_decls->data_eqns);
}
ATermAppl make_fresh_struct_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSortStructPrefix(), term, false));
}

ATermAppl make_fresh_list_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSortListPrefix(), term, false));
}

ATermAppl make_fresh_set_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSortSetPrefix(), term, false));
}

ATermAppl make_fresh_fset_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSortFSetPrefix(), term, false));
}

ATermAppl make_fresh_bag_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSortBagPrefix(), term, false));
}

ATermAppl make_fresh_fbag_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(gsSortFBagPrefix(), term, false));
}

ATermAppl make_fresh_lambda_op_id(ATermAppl sort_expr, ATerm term)
{
  return gsMakeOpId(gsFreshString2ATermAppl(gsLambdaPrefix(), term, false),
    sort_expr);
}

void split_sort_decls(ATermList sort_decls, ATermList *p_sort_ids,
  ATermList *p_sort_refs)
{
  ATermList sort_ids = ATmakeList0();
  ATermList sort_refs = ATmakeList0();
  while (!ATisEmpty(sort_decls))
  {
    ATermAppl sortDecl = ATAgetFirst(sort_decls);
    if (gsIsSortRef(sortDecl)) {
      sort_refs = ATinsert(sort_refs, (ATerm) sortDecl);
    } else { //gsIsSortId(sortDecl)
      sort_ids = ATinsert(sort_ids, (ATerm) sortDecl);
    }
    sort_decls = ATgetNext(sort_decls);
  }
  *p_sort_ids = ATreverse(sort_ids);
  *p_sort_refs = ATreverse(sort_refs);
}

    } // namespace detail
    /// \endcond
  }   // namespace core
}     // namespace mcrl2
