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
#include "mcrl2/new_data/set.h"
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

static ATermAppl make_struct_bag_elt(ATermAppl sort_elt);
//Pre: sort_elt is a sort expression
//Ret: the structured sort "struct bag_elt(sort_elt, Pos)"

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
  ATermAppl fset_expr = gsMakeDataExprFSetEmpty(gsMakeSortExprList(gsGetSort(ATAgetFirst(elts))));
  for (ATermList l = ATreverse(elts) ; !ATisEmpty(l) ; l = ATgetNext(l)) {
    fset_expr = gsMakeDataExprFSetInsert(ATAgetFirst(l), fset_expr);
  }
  return gsMakeDataExprSetFSet(fset_expr, sort_expr);
}

ATermAppl impl_bag_enum(ATermList elts, ATermAppl sort_expr)
{
  assert(ATgetLength(elts) > 0);
  ATermAppl fbag_expr = gsMakeDataExprFBagEmpty(gsMakeSortExprList(make_struct_bag_elt(gsGetSort(ATAgetFirst(elts)))));
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

  function_symbol_list constructors = element_sort.constructor_functions(sort_expression(sort_id));
  function_symbol_list functions;
  function_symbol_list projection_functions = element_sort.projection_functions(sort_expression(sort_id));
  function_symbol_list recogniser_functions = element_sort.recogniser_functions(sort_expression(sort_id));

  // implement argument sorts 
  for (function_symbol_list::const_iterator i = projection_functions.begin();
                                        recursive && i != projection_functions.end(); ++i) {
    impl_exprs_appl(function_sort(i->sort()).codomain(), p_substs, p_data_decls, new_data_equations);
  }

  std::copy(projection_functions.begin(), projection_functions.end(),
                                std::back_insert_iterator< function_symbol_list >(functions));
  std::copy(recogniser_functions.begin(), recogniser_functions.end(),
                                std::back_insert_iterator< function_symbol_list >(functions));

  data_equation_list   equations            = element_sort.constructor_equations(sort_expression(sort_id));
  data_equation_list   projection_equations = element_sort.projection_equations(sort_expression(sort_id));
  data_equation_list   recogniser_equations = element_sort.recogniser_equations(sort_expression(sort_id));

  std::copy(projection_equations.begin(), projection_equations.end(),
                                std::back_insert_iterator< data_equation_list >(equations));
  std::copy(recogniser_equations.begin(), recogniser_equations.end(),
                                std::back_insert_iterator< data_equation_list >(equations));

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
  data_equation_list equations = sort_list::list_generate_equations_code(sort_expression(sort_elt));
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

  function_symbol_list constructors = sort_list::list_generate_constructors_code(element_sort);
  function_symbol_list functions = sort_list::list_generate_functions_code(element_sort);
  data_equation_list equations = sort_list::list_generate_equations_code(element_sort);

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
    impl_sort_nat(p_data_decls, new_data_equations);
  }
}

ATermList build_fset_equations(ATermAppl sort_elt, ATermAppl sort_fset)
{
  //declare equations for sort sort_fset
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), sort_fset);
  ATermAppl t = gsMakeDataVarId(gsString2ATermAppl("t"), sort_fset);
  ATermAppl d = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl e = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermAppl f = gsMakeDataVarId(gsString2ATermAppl("f"), gsMakeSortArrow1(sort_elt, gsMakeSortExprBool()));
  ATermAppl g = gsMakeDataVarId(gsString2ATermAppl("g"), gsMakeSortArrow1(sort_elt, gsMakeSortExprBool()));
  ATermList dl = ATmakeList1((ATerm) d);
  ATermList dsl = ATmakeList2((ATerm) d, (ATerm) s);
  ATermList desl = ATmakeList3((ATerm) d, (ATerm) e, (ATerm) s);
  ATermList fl = ATmakeList1((ATerm) f);
  ATermList dsfl = ATmakeList3((ATerm) d, (ATerm) s, (ATerm) f);
  ATermList etfl = ATmakeList3((ATerm) e, (ATerm) t, (ATerm) f);
  ATermList dstfl = ATmakeList4((ATerm) d, (ATerm) s, (ATerm) t, (ATerm) f);
  ATermList destfl = ATmakeList5((ATerm) d, (ATerm) e, (ATerm) s, (ATerm) t, (ATerm) f);
  ATermList fgl = ATmakeList2((ATerm) f, (ATerm) g);
  ATermList dsfgl = ATmakeList4((ATerm) d, (ATerm) s, (ATerm) f, (ATerm) g);
  ATermList etfgl = ATmakeList4((ATerm) e, (ATerm) t, (ATerm) f, (ATerm) g);
  ATermList dstfgl = ATmakeList5((ATerm) d, (ATerm) s, (ATerm) t, (ATerm) f, (ATerm) g);
  ATermList destfgl = ATmakeList6((ATerm) d, (ATerm) e, (ATerm) s, (ATerm) t, (ATerm) f, (ATerm) g);

  ATermList new_data_eqns = ATmakeList(29,
    //empty set (sort_fset)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprFSetEmpty(sort_fset),
      gsMakeDataExprEmptyList(sort_fset)),
    //insert (sort_elt # sort_fset -> sort_fset)
    (ATerm) gsMakeDataEqn(dl, gsMakeNil(),
      gsMakeDataExprFSetInsert(d, gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprCons(d, gsMakeDataExprEmptyList(sort_fset))),
    (ATerm) gsMakeDataEqn(dsl, gsMakeNil(),
      gsMakeDataExprFSetInsert(d, gsMakeDataExprCons(d, s)),
      gsMakeDataExprCons(d, s)),
    (ATerm) gsMakeDataEqn(desl,
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFSetInsert(d, gsMakeDataExprCons(e, s)),
      gsMakeDataExprCons(d, gsMakeDataExprCons(e, s))),
    (ATerm) gsMakeDataEqn(desl,
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFSetInsert(d, gsMakeDataExprCons(e, s)),
      gsMakeDataExprCons(e, gsMakeDataExprFSetInsert(d, s))),
    //conditional insert (sort_elt # Bool # sort_fset -> sort_fset)
    (ATerm) gsMakeDataEqn(dsl, gsMakeNil(),
      gsMakeDataExprFSetCInsert(d, gsMakeDataExprFalse(), s),
      s),
    (ATerm) gsMakeDataEqn(dsl, gsMakeNil(),
      gsMakeDataExprFSetCInsert(d, gsMakeDataExprTrue(), s),
      gsMakeDataExprFSetInsert(d, s)),
    //element of a finite set (sort_elt # sort_fset -> Bool)
    (ATerm) gsMakeDataEqn(dl, gsMakeNil(),
      gsMakeDataExprFSetIn(d, gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprFalse()),
    (ATerm) gsMakeDataEqn(dsl, gsMakeNil(),
      gsMakeDataExprFSetIn(d, gsMakeDataExprCons(d, s)),
      gsMakeDataExprTrue()),
    (ATerm) gsMakeDataEqn(desl,
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFSetIn(d, gsMakeDataExprCons(e, s)),
      gsMakeDataExprFalse()),
    (ATerm) gsMakeDataEqn(desl,
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFSetIn(d, gsMakeDataExprCons(e, s)),
      gsMakeDataExprFSetIn(d, s)),
    //finite subset or equality ((sort_elt -> Bool) # sort_fset # sort_fset -> Bool)
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprFSetLTE(f, gsMakeDataExprEmptyList(sort_fset), gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprTrue()),
    (ATerm) gsMakeDataEqn(dsfl, gsMakeNil(),
      gsMakeDataExprFSetLTE(f, gsMakeDataExprCons(d, s), gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprAnd(
        gsMakeDataAppl1(f, d),
        gsMakeDataExprFSetLTE(f, s, gsMakeDataExprEmptyList(sort_fset)))),
    (ATerm) gsMakeDataEqn(etfl, gsMakeNil(),
      gsMakeDataExprFSetLTE(f, gsMakeDataExprEmptyList(sort_fset), gsMakeDataExprCons(e, t)),
      gsMakeDataExprAnd(
        gsMakeDataExprNot(gsMakeDataAppl1(f, e)),
        gsMakeDataExprFSetLTE(f, gsMakeDataExprEmptyList(sort_fset), t))),
    (ATerm) gsMakeDataEqn(dstfl, gsMakeNil(),
      gsMakeDataExprFSetLTE(f, gsMakeDataExprCons(d, s), gsMakeDataExprCons(d, t)),
      gsMakeDataExprFSetLTE(f, s, t)),
    (ATerm) gsMakeDataEqn(destfl,
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFSetLTE(f, gsMakeDataExprCons(d, s), gsMakeDataExprCons(e, t)),
      gsMakeDataExprAnd(
        gsMakeDataAppl1(f, d),
        gsMakeDataExprFSetLTE(f, s, gsMakeDataExprCons(e, t)))),
    (ATerm) gsMakeDataEqn(destfl,
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFSetLTE(f, gsMakeDataExprCons(d, s), gsMakeDataExprCons(e, t)),
      gsMakeDataExprAnd(
        gsMakeDataExprNot(gsMakeDataAppl1(f, e)),
        gsMakeDataExprFSetLTE(f, gsMakeDataExprCons(d, s), t))),
    //finite set union ((sort_elt -> Bool) # (sort_elt -> Bool) # sort_fset # sort_fset -> sort_fset)
    (ATerm) gsMakeDataEqn(fgl, gsMakeNil(),
      gsMakeDataExprFSetUnion(f, g, gsMakeDataExprEmptyList(sort_fset), gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprEmptyList(sort_fset)),
    (ATerm) gsMakeDataEqn(dsfgl, gsMakeNil(),
      gsMakeDataExprFSetUnion(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataExprNot(gsMakeDataAppl1(g, d)),
        gsMakeDataExprFSetUnion(f, g, s, gsMakeDataExprEmptyList(sort_fset)))),
    (ATerm) gsMakeDataEqn(etfgl, gsMakeNil(),
      gsMakeDataExprFSetUnion(f, g, gsMakeDataExprEmptyList(sort_fset), gsMakeDataExprCons(e, t)),
      gsMakeDataExprFSetCInsert(
        e,
        gsMakeDataExprNot(gsMakeDataAppl1(f, e)),
        gsMakeDataExprFSetUnion(f, g, gsMakeDataExprEmptyList(sort_fset), t))),
    (ATerm) gsMakeDataEqn(dstfgl, gsMakeNil(),
      gsMakeDataExprFSetUnion(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprCons(d, t)),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataExprEq(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d)),
        gsMakeDataExprFSetUnion(f, g, s, t))),
    (ATerm) gsMakeDataEqn(destfgl,
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFSetUnion(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprCons(e, t)),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataExprNot(gsMakeDataAppl1(g, d)),
        gsMakeDataExprFSetUnion(f, g, s, gsMakeDataExprCons(e, t)))),
    (ATerm) gsMakeDataEqn(destfgl,
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFSetUnion(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprCons(e, t)),
      gsMakeDataExprFSetCInsert(
        e,
        gsMakeDataExprNot(gsMakeDataAppl1(f, e)),
        gsMakeDataExprFSetUnion(f, g, gsMakeDataExprCons(d, s), t))),
    //finite set intersection ((sort_elt -> Bool) # (sort_elt -> Bool) # sort_fset # sort_fset -> sort_fset)
    (ATerm) gsMakeDataEqn(fgl, gsMakeNil(),
      gsMakeDataExprFSetInter(f, g, gsMakeDataExprEmptyList(sort_fset), gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprEmptyList(sort_fset)),
    (ATerm) gsMakeDataEqn(dsfgl, gsMakeNil(),
      gsMakeDataExprFSetInter(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprEmptyList(sort_fset)),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataAppl1(g, d),
        gsMakeDataExprFSetInter(f, g, s, gsMakeDataExprEmptyList(sort_fset)))),
    (ATerm) gsMakeDataEqn(etfgl, gsMakeNil(),
      gsMakeDataExprFSetInter(f, g, gsMakeDataExprEmptyList(sort_fset), gsMakeDataExprCons(e, t)),
      gsMakeDataExprFSetCInsert(
        e,
        gsMakeDataAppl1(f, e),
        gsMakeDataExprFSetInter(f, g, gsMakeDataExprEmptyList(sort_fset), t))),
    (ATerm) gsMakeDataEqn(dstfgl, gsMakeNil(),
      gsMakeDataExprFSetInter(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprCons(d, t)),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataExprEq(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d)),
        gsMakeDataExprFSetInter(f, g, s, t))),
    (ATerm) gsMakeDataEqn(destfgl,
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFSetInter(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprCons(e, t)),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataAppl1(g, d),
        gsMakeDataExprFSetInter(f, g, s, gsMakeDataExprCons(e, t)))),
    (ATerm) gsMakeDataEqn(destfgl,
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFSetInter(f, g, gsMakeDataExprCons(d, s), gsMakeDataExprCons(e, t)),
      gsMakeDataExprFSetCInsert(
        e,
        gsMakeDataAppl1(f, e),
        gsMakeDataExprFSetInter(f, g, gsMakeDataExprCons(d, s), t)))
  );

  return new_data_eqns;
}

void impl_sort_fset(ATermAppl sort_elt, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);

  //implement finite sets of sort sort_elt as finite lists of sort sort_elt
  impl_sort_list(gsMakeSortExprList(sort_elt), sort_id, p_substs, p_data_decls, new_data_equations);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(7,
      (ATerm) gsMakeOpIdFSetEmpty(sort_id),
      (ATerm) gsMakeOpIdFSetInsert(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFSetCInsert(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFSetIn(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFSetLTE(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFSetUnion(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFSetInter(sort_elt, sort_id)
  );

  ATermList new_data_eqns = build_fset_equations(sort_elt, sort_id);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_data_eqns, *new_data_equations);
}

ATermList build_set_equations(ATermAppl sort_elt, ATermAppl sort_fset, ATermAppl sort_set)
{
  //declare data equations for sort sort_id
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), sort_set);
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), sort_set);
  ATermAppl f = gsMakeDataVarId(gsString2ATermAppl("f"), gsMakeSortArrow1(sort_elt, gsMakeSortExprBool()));
  ATermAppl g = gsMakeDataVarId(gsString2ATermAppl("g"), gsMakeSortArrow1(sort_elt, gsMakeSortExprBool()));
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), sort_fset);
  ATermAppl t = gsMakeDataVarId(gsString2ATermAppl("t"), sort_fset);
  ATermAppl e = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermList sl = ATmakeList1((ATerm) s);
  ATermList fl = ATmakeList1((ATerm) f);
  ATermList el = ATmakeList1((ATerm) e);
  ATermList efl = ATmakeList2((ATerm) e, (ATerm) f);
  ATermList efgl = ATmakeList3((ATerm) e, (ATerm) f, (ATerm) g);
  ATermList efsl = ATmakeList3((ATerm) e, (ATerm) f, (ATerm) s);
  ATermList fsl = ATmakeList2((ATerm) f, (ATerm) s);
  ATermList fgstl = ATmakeList4((ATerm) f, (ATerm) g, (ATerm) s, (ATerm) t);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);

  ATermList new_data_eqns = ATmakeList(32,
    //empty set (sort_set)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprEmptySet(sort_set),
      gsMakeDataExprSet(gsMakeDataExprFalseFunc(sort_elt), gsMakeDataExprFSetEmpty(sort_fset), sort_set)),
    //finite set (sort_fset -> sort_set)
    (ATerm) gsMakeDataEqn(sl, gsMakeNil(),
      gsMakeDataExprSetFSet(s, sort_set),
      gsMakeDataExprSet(gsMakeDataExprFalseFunc(sort_elt), s, sort_set)),
    //set comprehension ((sort_elt -> Bool) -> sort_set)
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprSetComp(f, sort_set),
      gsMakeDataExprSet(f, gsMakeDataExprFSetEmpty(sort_fset), sort_set)),
    //element test (sort_elt # sort_set -> Bool)
    (ATerm) gsMakeDataEqn(efsl, gsMakeNil(),
      gsMakeDataExprEltIn(e, gsMakeDataExprSet(f, s, sort_set)),
      gsMakeDataExprNeq(gsMakeDataAppl1(f, e), gsMakeDataExprFSetIn(e, s))),
    //equality (sort_set # sort_set -> Bool)
    (ATerm) gsMakeDataEqn(fgstl, 
      gsMakeDataExprEq(f, g),
      gsMakeDataExprEq(
        gsMakeDataExprSet(f, s, sort_set),
        gsMakeDataExprSet(g, t, sort_set)), 
      gsMakeDataExprEq(s, t)),
    (ATerm) gsMakeDataEqn(fgstl, 
      gsMakeDataExprNeq(f, g),
      gsMakeDataExprEq(
        gsMakeDataExprSet(f, s, sort_set),
        gsMakeDataExprSet(g, t, sort_set)), 
      gsMakeDataExprForall(gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) e),
        gsMakeDataExprEq(
          gsMakeDataExprEltIn(e, gsMakeDataExprSet(f, s, sort_set)),
          gsMakeDataExprEltIn(e, gsMakeDataExprSet(g, t, sort_set))
      )))),
    //proper subset (sort_set # sort_set -> Bool)
    (ATerm) gsMakeDataEqn(xyl, gsMakeNil(),
      gsMakeDataExprLT(x, y),
      gsMakeDataExprAnd(
        gsMakeDataExprLTE(x, y), 
        gsMakeDataExprNeq(x, y)
      )),
    //subset or equal (sort_set # sort_set -> Bool)
    (ATerm) gsMakeDataEqn(fgstl, 
      gsMakeDataExprEq(f, g),
      gsMakeDataExprLTE(
        gsMakeDataExprSet(f, s, sort_set),
        gsMakeDataExprSet(g, t, sort_set)), 
      gsMakeDataExprFSetLTE(f, s, t)),
    (ATerm) gsMakeDataEqn(fgstl, 
      gsMakeDataExprNeq(f, g),
      gsMakeDataExprLTE(
        gsMakeDataExprSet(f, s, sort_set),
        gsMakeDataExprSet(g, t, sort_set)), 
      gsMakeDataExprForall(gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) e),
        gsMakeDataExprImp(
          gsMakeDataExprEltIn(e, gsMakeDataExprSet(f, s, sort_set)),
          gsMakeDataExprEltIn(e, gsMakeDataExprSet(g, t, sort_set))
      )))),
    //complement (sort_set -> sort_set)
    (ATerm) gsMakeDataEqn(fsl, gsMakeNil(),
      gsMakeDataExprSetCompl(gsMakeDataExprSet(f, s, sort_set)),
      gsMakeDataExprSet(gsMakeDataExprNotFunc(f), s, sort_set)),
    //union (sort_set # sort_set -> sort_set)
    (ATerm) gsMakeDataEqn(fgstl, gsMakeNil(),
      gsMakeDataExprSetUnion(
        gsMakeDataExprSet(f, s, sort_set),
        gsMakeDataExprSet(g, t, sort_set)), 
      gsMakeDataExprSet(
        gsMakeDataExprOrFunc(f, g),
        gsMakeDataExprFSetUnion(f, g, s, t),
        sort_set)),
    //intersection (sort_set # sort_set -> sort_set)
    (ATerm) gsMakeDataEqn(fgstl, gsMakeNil(),
      gsMakeDataExprSetInterSect(
        gsMakeDataExprSet(f, s, sort_set),
        gsMakeDataExprSet(g, t, sort_set)), 
      gsMakeDataExprSet(
        gsMakeDataExprAndFunc(f, g),
        gsMakeDataExprFSetInter(f, g, s, t),
        sort_set)),
    //difference (sort_set # sort_set -> sort_set)
    (ATerm) gsMakeDataEqn(xyl, gsMakeNil(),
      gsMakeDataExprSetDiff(x, y),
      gsMakeDataExprSetInterSect(x, gsMakeDataExprSetCompl(y))),
    //false function (sort_elt -> Bool)
    (ATerm) gsMakeDataEqn(el, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprFalseFunc(sort_elt), e),
      gsMakeDataExprFalse()),
    //true function (sort_elt -> Bool)
    (ATerm) gsMakeDataEqn(el, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprTrueFunc(sort_elt), e),
      gsMakeDataExprTrue()),
    //equality on functions ((sort_elt -> Bool) # (sort_elt -> Bool) -> Bool)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprEq(gsMakeDataExprFalseFunc(sort_elt), gsMakeDataExprTrueFunc(sort_elt)),
      gsMakeDataExprFalse()),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprEq(gsMakeDataExprTrueFunc(sort_elt), gsMakeDataExprFalseFunc(sort_elt)),
      gsMakeDataExprFalse()),
    //logical negation function ((sort_elt -> Bool) -> (sort_elt -> Bool))
    (ATerm) gsMakeDataEqn(efl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprNotFunc(f), e),
      gsMakeDataExprNot(gsMakeDataAppl1(f, e))),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprNotFunc(gsMakeDataExprFalseFunc(sort_elt)),
      gsMakeDataExprTrueFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprNotFunc(gsMakeDataExprTrueFunc(sort_elt)),
      gsMakeDataExprFalseFunc(sort_elt)),
    //conjunction function ((sort_elt -> Bool) # (sort_elt -> Bool) -> (sort_elt -> Bool))
    (ATerm) gsMakeDataEqn(efgl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprAndFunc(f, g), e),
      gsMakeDataExprAnd(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e))),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAndFunc(f, f),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAndFunc(f, gsMakeDataExprFalseFunc(sort_elt)),
      gsMakeDataExprFalseFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAndFunc(gsMakeDataExprFalseFunc(sort_elt), f),
      gsMakeDataExprFalseFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAndFunc(f, gsMakeDataExprTrueFunc(sort_elt)),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAndFunc(gsMakeDataExprTrueFunc(sort_elt), f),
      f),
    //disjunction function ((sort_elt -> Bool) # (sort_elt -> Bool) -> (sort_elt -> Bool))
    (ATerm) gsMakeDataEqn(efgl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprOrFunc(f, g), e),
      gsMakeDataExprOr(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e))),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprOrFunc(f, f),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprOrFunc(f, gsMakeDataExprFalseFunc(sort_elt)),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprOrFunc(gsMakeDataExprFalseFunc(sort_elt), f),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprOrFunc(f, gsMakeDataExprTrueFunc(sort_elt)),
      gsMakeDataExprTrueFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprOrFunc(gsMakeDataExprTrueFunc(sort_elt), f),
      gsMakeDataExprTrueFunc(sort_elt))
  );

  return new_data_eqns;
}

void impl_sort_set(ATermAppl sort_set, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortExprSet(sort_set));
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);

  //declare sort sort_id as representative of sort sort_set
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);

  //implement expressions in the target sort of sort_set
  //this needs to be done first to keep the substitutions sound!
  ATermAppl sort_elt = ATAgetArgument(sort_set, 1);
  impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);

  //add substitution sort_set -> sort_id
  ATermAppl subst = gsMakeSubst_Appl(sort_set, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //create finite set sort identifier
  ATermAppl sort_fset = make_fresh_fset_sort_id((ATerm) p_data_decls->sorts);
  //implement finite sets
  impl_sort_fset(sort_elt, sort_fset, p_substs, p_data_decls, new_data_equations);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(14,
      (ATerm) gsMakeOpIdSet(sort_elt, sort_fset, sort_set),
      (ATerm) gsMakeOpIdEmptySet(sort_set),
      (ATerm) gsMakeOpIdSetFSet(sort_fset, sort_set),
      (ATerm) gsMakeOpIdSetComp(sort_elt, sort_set),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_set),
      (ATerm) gsMakeOpIdSetCompl(sort_set),
      (ATerm) gsMakeOpIdSetUnion(sort_set),
      (ATerm) gsMakeOpIdSetIntersect(sort_set),
      (ATerm) gsMakeOpIdSetDiff(sort_set),
      (ATerm) gsMakeOpIdFalseFunc(sort_elt),
      (ATerm) gsMakeOpIdTrueFunc(sort_elt),
      (ATerm) gsMakeOpIdNotFunc(sort_elt),
      (ATerm) gsMakeOpIdAndFunc(sort_elt),
      (ATerm) gsMakeOpIdOrFunc(sort_elt)
  );

  ATermList new_data_eqns = build_set_equations(sort_elt, sort_fset, sort_set);

  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls, new_data_equations);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_data_eqns, *new_data_equations);
}

ATermList build_fbag_equations(ATermAppl sort_elt, ATermAppl sort_fset, ATermAppl sort_fbag_elt, ATermAppl sort_fbag)
{
  //declare equations for sort sort_fbag
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), sort_fbag);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), sort_fbag);
  ATermAppl d = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl e = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl f = gsMakeDataVarId(gsString2ATermAppl("f"), gsMakeSortArrow1(sort_elt, gsMakeSortExprNat()));
  ATermAppl g = gsMakeDataVarId(gsString2ATermAppl("g"), gsMakeSortArrow1(sort_elt, gsMakeSortExprNat()));
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), sort_fset);

  ATermList new_data_eqns = ATmakeList(40,
    //gsMakeDataExprEmptyList(sort_fbag) bag (sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprFBagEmpty(sort_fbag),
      gsMakeDataExprEmptyList(sort_fbag)),
    //insert (sort_elt # Pos # sort_fbag -> sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) d, (ATerm) p), gsMakeNil(),
      gsMakeDataExprFBagInsert(d, p, gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), gsMakeDataExprEmptyList(sort_fbag))),
    (ATerm) gsMakeDataEqn(ATmakeList4((ATerm) d, (ATerm) p, (ATerm) q, (ATerm) b), gsMakeNil(),
      gsMakeDataExprFBagInsert(d, p, gsMakeDataExprCons(gsMakeDataExprBagElt(d, q, sort_fbag_elt), b)),
      gsMakeDataExprCons(gsMakeDataExprBagElt(d, gsMakeDataExprAdd(p, q), sort_fbag_elt), b)),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) d, (ATerm) e, (ATerm)p, (ATerm) q, (ATerm) b),
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFBagInsert(d, p, gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), b)),
      gsMakeDataExprCons(
        gsMakeDataExprBagElt(d, p, sort_fbag_elt),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), b))),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) e, (ATerm) d, (ATerm)p, (ATerm) q, (ATerm) b),
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFBagInsert(d, p, gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), b)),
      gsMakeDataExprCons(
        gsMakeDataExprBagElt(e, q, sort_fbag_elt),
        gsMakeDataExprFBagInsert(d, p, b))),
    //conditional insert (sort_elt # Nat # sort_fbag -> sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) d, (ATerm) b), gsMakeNil(),
      gsMakeDataExprFBagCInsert(d, gsMakeDataExprC0(), b),
      b),
    (ATerm) gsMakeDataEqn(ATmakeList3((ATerm) d, (ATerm) p, (ATerm) b), gsMakeNil(),
      gsMakeDataExprFBagCInsert(d, gsMakeDataExprCNat(p), b),
      gsMakeDataExprFBagInsert(d, p, b)),
    //count of an element in a finite bag (sort_elt # sort_fbag -> Nat)
    (ATerm) gsMakeDataEqn(ATmakeList1((ATerm) d), gsMakeNil(),
      gsMakeDataExprFBagCount(d, gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprC0()),
    (ATerm) gsMakeDataEqn(ATmakeList3((ATerm) d, (ATerm) p, (ATerm) b), gsMakeNil(),
      gsMakeDataExprFBagCount(d, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b)),
      gsMakeDataExprCNat(p)),
    (ATerm) gsMakeDataEqn(ATmakeList4((ATerm) d, (ATerm) e, (ATerm) p, (ATerm) b),
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFBagCount(d, gsMakeDataExprCons(gsMakeDataExprBagElt(e, p, sort_fbag_elt), b)),
      gsMakeDataExprC0()),
    (ATerm) gsMakeDataEqn(ATmakeList4((ATerm) d, (ATerm) e, (ATerm) p, (ATerm) b),
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFBagCount(d, gsMakeDataExprCons(gsMakeDataExprBagElt(e, p, sort_fbag_elt), b)),
      gsMakeDataExprFBagCount(d, b)),
    //element test (sort_elt # sort_fbag -> Bool)
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) d, (ATerm) b), gsMakeNil(),
      gsMakeDataExprFBagIn(d, b),
      gsMakeDataExprGT(gsMakeDataExprFBagCount(d, b), gsMakeDataExprC0())),
    //finite subbag or equality ((sort_elt -> Nat) # sort_fbag # sort_fbag -> Bool)
    (ATerm) gsMakeDataEqn(ATmakeList1((ATerm) f), gsMakeNil(),
      gsMakeDataExprFBagLTE(f, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprTrue()),
    (ATerm) gsMakeDataEqn(ATmakeList4((ATerm) f, (ATerm) d, (ATerm) p, (ATerm) b) , gsMakeNil(),
      gsMakeDataExprFBagLTE(f, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprAnd(
        gsMakeDataExprSwapZeroLTE(gsMakeDataAppl1(f, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagLTE(f, b, gsMakeDataExprEmptyList(sort_fbag)))),
    (ATerm) gsMakeDataEqn(ATmakeList4((ATerm) f, (ATerm) e, (ATerm) q, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagLTE(f, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprAnd(
        gsMakeDataExprSwapZeroLTE(gsMakeDataAppl1(f, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagLTE(f, gsMakeDataExprEmptyList(sort_fbag), c))),
    (ATerm) gsMakeDataEqn(ATmakeList6((ATerm) f, (ATerm) d, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagLTE(f,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, q, sort_fbag_elt), c)),
      gsMakeDataExprAnd(
        gsMakeDataExprSwapZeroLTE(gsMakeDataAppl1(f, d), gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagLTE(f, b, c))),
    (ATerm) gsMakeDataEqn(ATmakeList(7, (ATerm) f, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFBagLTE(f,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprAnd(
        gsMakeDataExprSwapZeroLTE(gsMakeDataAppl1(f, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagLTE(f, b, gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)))),
    (ATerm) gsMakeDataEqn(ATmakeList(7, (ATerm) f, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFBagLTE(f,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprAnd(
        gsMakeDataExprSwapZeroLTE(gsMakeDataAppl1(f, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagLTE(f, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), c))),
    //finite bag join ((sort_elt -> Nat) # (sort_elt -> Nat) # sort_fbag # sort_fbag -> sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) f, (ATerm) g), gsMakeNil(),
      gsMakeDataExprFBagJoin(f, g, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprEmptyList(sort_fbag)),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) f, (ATerm) g, (ATerm) d, (ATerm) p, (ATerm) b) , gsMakeNil(),
      gsMakeDataExprFBagJoin(f, g, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroAdd(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagJoin(f, g, b, gsMakeDataExprEmptyList(sort_fbag)))),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) f, (ATerm) g, (ATerm) e, (ATerm) q, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagJoin(f, g, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        e,
        gsMakeDataExprSwapZeroAdd(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagJoin(f, g, gsMakeDataExprEmptyList(sort_fbag), c))),
    (ATerm) gsMakeDataEqn(ATmakeList(7, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagJoin(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroAdd(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagJoin(f, g, b, c))),
    (ATerm) gsMakeDataEqn(ATmakeList(8, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFBagJoin(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroAdd(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagJoin(f, g, b, gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)))),
    (ATerm) gsMakeDataEqn(ATmakeList(8, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFBagJoin(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        e,
        gsMakeDataExprSwapZeroAdd(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagJoin(f, g, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), c))),
    //finite bag intersection ((sort_elt -> Nat) # (sort_elt -> Nat) # sort_fbag # sort_fbag -> sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) f, (ATerm) g), gsMakeNil(),
      gsMakeDataExprFBagInter(f, g, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprEmptyList(sort_fbag)),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) f, (ATerm) g, (ATerm) d, (ATerm) p, (ATerm) b) , gsMakeNil(),
      gsMakeDataExprFBagInter(f, g, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroMin(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagInter(f, g, b, gsMakeDataExprEmptyList(sort_fbag)))),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) f, (ATerm) g, (ATerm) e, (ATerm) q, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagInter(f, g, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        e,
        gsMakeDataExprSwapZeroMin(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagInter(f, g, gsMakeDataExprEmptyList(sort_fbag), c))),
    (ATerm) gsMakeDataEqn(ATmakeList(7, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagInter(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroMin(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagInter(f, g, b, c))),
    (ATerm) gsMakeDataEqn(ATmakeList(8, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFBagInter(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroMin(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagInter(f, g, b, gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)))),
    (ATerm) gsMakeDataEqn(ATmakeList(8, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFBagInter(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        e,
        gsMakeDataExprSwapZeroMin(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagInter(f, g, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), c))),
    //finite bag intersection ((sort_elt -> Nat) # (sort_elt -> Nat) # sort_fbag # sort_fbag -> sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) f, (ATerm) g), gsMakeNil(),
      gsMakeDataExprFBagDiff(f, g, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprEmptyList(sort_fbag)),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) f, (ATerm) g, (ATerm) d, (ATerm) p, (ATerm) b) , gsMakeNil(),
      gsMakeDataExprFBagDiff(f, g, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), gsMakeDataExprEmptyList(sort_fbag)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroMonus(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagDiff(f, g, b, gsMakeDataExprEmptyList(sort_fbag)))),
    (ATerm) gsMakeDataEqn(ATmakeList5((ATerm) f, (ATerm) g, (ATerm) e, (ATerm) q, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagDiff(f, g, gsMakeDataExprEmptyList(sort_fbag), gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        e,
        gsMakeDataExprSwapZeroMonus(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagDiff(f, g, gsMakeDataExprEmptyList(sort_fbag), c))),
    (ATerm) gsMakeDataEqn(ATmakeList(7, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c), gsMakeNil(),
      gsMakeDataExprFBagDiff(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroMonus(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagDiff(f, g, b, c))),
    (ATerm) gsMakeDataEqn(ATmakeList(8, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(d, e),
      gsMakeDataExprFBagDiff(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprSwapZeroMonus(gsMakeDataAppl1(f, d), gsMakeDataAppl1(g, d), gsMakeDataExprCNat(p), gsMakeDataExprC0()),
        gsMakeDataExprFBagDiff(f, g, b, gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)))),
    (ATerm) gsMakeDataEqn(ATmakeList(8, (ATerm) f, (ATerm) g, (ATerm) d, (ATerm) e, (ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c),
      gsMakeDataExprLT(e, d),
      gsMakeDataExprFBagDiff(f, g,
        gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b),
        gsMakeDataExprCons(gsMakeDataExprBagElt(e, q, sort_fbag_elt), c)),
      gsMakeDataExprFBagCInsert(
        e,
        gsMakeDataExprSwapZeroMonus(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e), gsMakeDataExprC0(), gsMakeDataExprCNat(q)),
        gsMakeDataExprFBagDiff(f, g, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), c))),
    //convert finite bag to finite set (sort_fbag -> sort_fset)
    (ATerm) gsMakeDataEqn(ATmakeList1((ATerm) f), gsMakeNil(),
      gsMakeDataExprFBag2FSet(f, gsMakeDataExprEmptyList(sort_fbag), sort_fset),
      gsMakeDataExprEmptyList(sort_fset)),
    (ATerm) gsMakeDataEqn(ATmakeList4((ATerm) f, (ATerm) d, (ATerm) p, (ATerm) b), gsMakeNil(),
      gsMakeDataExprFBag2FSet(f, gsMakeDataExprCons(gsMakeDataExprBagElt(d, p, sort_fbag_elt), b), sort_fset),
      gsMakeDataExprFSetCInsert(
        d,
        gsMakeDataExprEq(
          gsMakeDataExprEq(gsMakeDataAppl1(f, d), gsMakeDataExprCNat(p)),
          gsMakeDataExprGT(gsMakeDataAppl1(f, d), gsMakeDataExprC0())),
        gsMakeDataExprFBag2FSet(f, b, sort_fset)
      )),
    //convert finite set to finite bag (sort_fset -> sort_fbag)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprFSet2FBag(gsMakeDataExprEmptyList(sort_fset), sort_fbag),
      gsMakeDataExprEmptyList(sort_fbag)),
    (ATerm) gsMakeDataEqn(ATmakeList2((ATerm) d, (ATerm) s), gsMakeNil(),
      gsMakeDataExprFSet2FBag(gsMakeDataExprCons(d, s), sort_fbag),
      gsMakeDataExprFBagCInsert(
        d,
        gsMakeDataExprCNat(gsMakeDataExprC1()),
        gsMakeDataExprFSet2FBag(s, sort_fbag)
      ))
  );

  return new_data_eqns;
}

void impl_sort_fbag(ATermAppl sort_elt, ATermAppl sort_fset, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);

  //implement finite bags of sort sort_elt as finite lists of pairs bag_elt(e, p),
  //where e is of sort sort_elt and p is of sort Pos
  ATermAppl struct_fbag_elt = make_struct_bag_elt(sort_elt);
  ////The name of struct_fbag_elt is irrelevant, so let the standard routines decide
  //ATermAppl sort_fbag_elt = make_fresh_struct_sort_id((ATerm) p_data_decls->sorts);
  //impl_sort_struct(struct_fbag_elt, sort_fbag_elt, p_substs, p_data_decls);
  impl_sort_list(gsMakeSortExprList(struct_fbag_elt), sort_id, p_substs, p_data_decls, new_data_equations);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(11,
      (ATerm) gsMakeOpIdFBagEmpty(sort_id),
      (ATerm) gsMakeOpIdFBagInsert(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagCInsert(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagCount(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagIn(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagLTE(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagJoin(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagInter(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBagDiff(sort_elt, sort_id),
      (ATerm) gsMakeOpIdFBag2FSet(sort_elt, sort_id, sort_fset),
      (ATerm) gsMakeOpIdFSet2FBag(sort_fset, sort_id)
  );

  ATermList new_data_eqns = build_fbag_equations(sort_elt, sort_fset, struct_fbag_elt, sort_id);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_data_eqns, *new_data_equations);
}

ATermList build_bag_equations(ATermAppl sort_elt, ATermAppl sort_fset, ATermAppl sort_fbag, ATermAppl sort_set, ATermAppl sort_bag)
{
  //declare data equations for sort sort_id
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), sort_bag);
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), sort_bag);
  ATermAppl f = gsMakeDataVarId(gsString2ATermAppl("f"), gsMakeSortArrow1(sort_elt, gsMakeSortExprNat()));
  ATermAppl g = gsMakeDataVarId(gsString2ATermAppl("g"), gsMakeSortArrow1(sort_elt, gsMakeSortExprNat()));
  ATermAppl h = gsMakeDataVarId(gsString2ATermAppl("h"), gsMakeSortArrow1(sort_elt, gsMakeSortExprBool()));
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), sort_fbag);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), sort_fbag);
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), sort_fset);
  ATermAppl e = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermList bl = ATmakeList1((ATerm) b);
  ATermList fl = ATmakeList1((ATerm) f);
  ATermList el = ATmakeList1((ATerm) e);
  ATermList efl = ATmakeList2((ATerm) e, (ATerm) f);
  ATermList ehl = ATmakeList2((ATerm) e, (ATerm) h);
  ATermList exl = ATmakeList2((ATerm) e, (ATerm) x);
  ATermList efgl = ATmakeList3((ATerm) e, (ATerm) f, (ATerm) g);
  ATermList efbl = ATmakeList3((ATerm) e, (ATerm) f, (ATerm) b);
  ATermList fbl = ATmakeList2((ATerm) f, (ATerm) b);
  ATermList fgbcl = ATmakeList4((ATerm) f, (ATerm) g, (ATerm) b, (ATerm) c);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList hsl = ATmakeList2((ATerm) h, (ATerm) s);

  ATermList new_data_eqns = ATmakeList(36,
    //empty bag (sort_bag)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprEmptyBag(sort_bag),
      gsMakeDataExprBag(gsMakeDataExprZeroFunc(sort_elt), gsMakeDataExprFBagEmpty(sort_fbag), sort_bag)),
    //finite bag (sort_fbag -> sort_bag)
    (ATerm) gsMakeDataEqn(bl, gsMakeNil(),
      gsMakeDataExprBagFBag(b, sort_bag),
      gsMakeDataExprBag(gsMakeDataExprZeroFunc(sort_elt), b, sort_bag)),
    //bag comprehension ((sort_elt -> Nat) -> sort_bag)
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprBagComp(f, sort_bag),
      gsMakeDataExprBag(f, gsMakeDataExprFBagEmpty(sort_fbag), sort_bag)),
    //count (sort_elt # sort_bag -> Nat)
    (ATerm) gsMakeDataEqn(efbl, gsMakeNil(),
      gsMakeDataExprCount(e, gsMakeDataExprBag(f, b, sort_bag)),
      gsMakeDataExprSwapZero(gsMakeDataAppl1(f, e), gsMakeDataExprFBagCount(e, b))),
    //element test (sort_elt # sort_bag -> Bool)
    (ATerm) gsMakeDataEqn(exl, gsMakeNil(),
      gsMakeDataExprEltIn(e, x),
      gsMakeDataExprGT(gsMakeDataExprCount(e, x), gsMakeDataExprC0())),
    //equality (sort_bag # sort_bag -> Bool)
    (ATerm) gsMakeDataEqn(fgbcl, 
      gsMakeDataExprEq(f, g),
      gsMakeDataExprEq(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprEq(b, c)),
    (ATerm) gsMakeDataEqn(fgbcl, 
      gsMakeDataExprNeq(f, g),
      gsMakeDataExprEq(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprForall(gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) e),
        gsMakeDataExprEq(
          gsMakeDataExprCount(e, gsMakeDataExprBag(f, b, sort_bag)),
          gsMakeDataExprCount(e, gsMakeDataExprBag(g, c, sort_bag))
      )))),
    //proper subbag (sort_bag # sort_bag -> Bool)
    (ATerm) gsMakeDataEqn(xyl, gsMakeNil(),
      gsMakeDataExprLT(x, y),
      gsMakeDataExprAnd(
        gsMakeDataExprLTE(x, y), 
        gsMakeDataExprNeq(x, y)
      )),
    //subbag or equal (sort_bag # sort_bag -> Bool)
    (ATerm) gsMakeDataEqn(fgbcl, 
      gsMakeDataExprEq(f, g),
      gsMakeDataExprLTE(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprFBagLTE(f, b, c)),
    (ATerm) gsMakeDataEqn(fgbcl, 
      gsMakeDataExprNeq(f, g),
      gsMakeDataExprLTE(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprForall(gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) e),
        gsMakeDataExprLTE(
          gsMakeDataExprCount(e, gsMakeDataExprBag(f, b, sort_bag)),
          gsMakeDataExprCount(e, gsMakeDataExprBag(g, c, sort_bag))
      )))),
    //join (sort_bag # sort_bag -> sort_bag)
    (ATerm) gsMakeDataEqn(fgbcl, gsMakeNil(),
      gsMakeDataExprBagJoin(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprBag(
        gsMakeDataExprAddFunc(f, g),
        gsMakeDataExprFBagJoin(f, g, b, c),
        sort_bag)),
    //intersection (sort_bag # sort_bag -> sort_bag)
    (ATerm) gsMakeDataEqn(fgbcl, gsMakeNil(),
      gsMakeDataExprBagInterSect(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprBag(
        gsMakeDataExprMinFunc(f, g),
        gsMakeDataExprFBagInter(f, g, b, c),
        sort_bag)),
    //difference (sort_bag # sort_bag -> sort_bag)
    (ATerm) gsMakeDataEqn(fgbcl, gsMakeNil(),
      gsMakeDataExprBagDiff(
        gsMakeDataExprBag(f, b, sort_bag),
        gsMakeDataExprBag(g, c, sort_bag)), 
      gsMakeDataExprBag(
        gsMakeDataExprMonusFunc(f, g),
        gsMakeDataExprFBagDiff(f, g, b, c),
        sort_bag)),
    //Bag2Set (sort_bag -> sort_set)
    (ATerm) gsMakeDataEqn(fbl, gsMakeNil(),
      gsMakeDataExprBag2Set(
        gsMakeDataExprBag(f, b, sort_bag),
        sort_set), 
      gsMakeDataExprSet(
        gsMakeDataExprNat2BoolFunc(f),
        gsMakeDataExprFBag2FSet(f, b, sort_fset),
        sort_set)),
    //Bag2Set (sort_bag -> sort_set)
    (ATerm) gsMakeDataEqn(hsl, gsMakeNil(),
      gsMakeDataExprSet2Bag(
        gsMakeDataExprSet(h, s, sort_set),
        sort_bag), 
      gsMakeDataExprBag(
        gsMakeDataExprBool2NatFunc(h),
        gsMakeDataExprFSet2FBag(s, sort_fbag),
        sort_bag)),
    //zero function (sort_elt -> Nat)
    (ATerm) gsMakeDataEqn(el, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprZeroFunc(sort_elt), e),
      gsMakeDataExprC0()),
    //one function (sort_elt -> Nat)
    (ATerm) gsMakeDataEqn(el, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprOneFunc(sort_elt), e),
      gsMakeDataExprCNat(gsMakeDataExprC1())),
    //equality on functions ((sort_elt -> Nat) # (sort_elt -> Nat) -> Bool)
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprEq(gsMakeDataExprZeroFunc(sort_elt), gsMakeDataExprOneFunc(sort_elt)),
      gsMakeDataExprFalse()),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprEq(gsMakeDataExprOneFunc(sort_elt), gsMakeDataExprZeroFunc(sort_elt)),
      gsMakeDataExprFalse()),
    //addition function ((sort_elt -> Nat) # (sort_elt -> Nat) -> (sort_elt -> Nat))
    (ATerm) gsMakeDataEqn(efgl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprAddFunc(f, g), e),
      gsMakeDataExprAdd(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e))),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAddFunc(f, gsMakeDataExprZeroFunc(sort_elt)),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprAddFunc(gsMakeDataExprZeroFunc(sort_elt), f),
      f),
    //minimum function ((sort_elt -> Nat) # (sort_elt -> Nat) -> (sort_elt -> Nat))
    (ATerm) gsMakeDataEqn(efgl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprMinFunc(f, g), e),
      gsMakeDataExprMin(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e))),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprMinFunc(f, f),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprMinFunc(f, gsMakeDataExprZeroFunc(sort_elt)),
      gsMakeDataExprZeroFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprMinFunc(gsMakeDataExprZeroFunc(sort_elt), f),
      gsMakeDataExprZeroFunc(sort_elt)),
    //monus function ((sort_elt -> Nat) # (sort_elt -> Nat) -> (sort_elt -> Nat))
    (ATerm) gsMakeDataEqn(efgl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprMonusFunc(f, g), e),
      gsMakeDataExprMonus(gsMakeDataAppl1(f, e), gsMakeDataAppl1(g, e))),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprMonusFunc(f, f),
      gsMakeDataExprZeroFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprMonusFunc(f, gsMakeDataExprZeroFunc(sort_elt)),
      f),
    (ATerm) gsMakeDataEqn(fl, gsMakeNil(),
      gsMakeDataExprMonusFunc(gsMakeDataExprZeroFunc(sort_elt), f),
      gsMakeDataExprZeroFunc(sort_elt)),
    //Nat2Bool function ((sort_elt -> Nat) -> (sort_elt -> Bool))
    (ATerm) gsMakeDataEqn(efl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprNat2BoolFunc(f), e),
      gsMakeDataExprGT(gsMakeDataAppl1(f, e), gsMakeDataExprC0())),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprNat2BoolFunc(gsMakeDataExprZeroFunc(sort_elt)),
      gsMakeDataExprFalseFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprNat2BoolFunc(gsMakeDataExprOneFunc(sort_elt)),
      gsMakeDataExprTrueFunc(sort_elt)),
    //Bool2Nat function ((sort_elt -> Nat) -> (sort_elt -> Bool))
    (ATerm) gsMakeDataEqn(ehl, gsMakeNil(),
      gsMakeDataAppl1(gsMakeDataExprBool2NatFunc(h), e),
      gsMakeDataExprIf(gsMakeDataAppl1(h, e), gsMakeDataExprCNat(gsMakeDataExprC1()), gsMakeDataExprC0())),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprBool2NatFunc(gsMakeDataExprFalseFunc(sort_elt)),
      gsMakeDataExprZeroFunc(sort_elt)),
    (ATerm) gsMakeDataEqn(ATmakeList0(), gsMakeNil(),
      gsMakeDataExprBool2NatFunc(gsMakeDataExprTrueFunc(sort_elt)),
      gsMakeDataExprOneFunc(sort_elt))
  );

  return new_data_eqns;
}

void impl_sort_bag(ATermAppl sort_bag, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  assert(gsIsSortExprBag(sort_bag));
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);

  //declare sort sort_id as representative of sort sort_bag
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);

  //implement expressions in the target sort of sort_bag
  //this needs to be done first in order to keep the substitutions sound!
  ATermAppl sort_elt = ATAgetArgument(sort_bag, 1);
  impl_exprs_appl(sort_elt, p_substs, p_data_decls, new_data_equations);

  //add implementation of sort Set(sort_elt), if necessary
  ATermAppl sort_set = gsMakeSortExprSet(sort_elt);
  ATermAppl sort_set_impl =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_set, false);
  if (ATisEqual(sort_set_impl, sort_set)) {
    //Set(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    ATermAppl sort_set_impl = make_fresh_set_sort_id((ATerm) p_data_decls->sorts);
    impl_sort_set(sort_set, sort_set_impl, p_substs, p_data_decls, new_data_equations);
  }
  //look up finite set sort identifier
  ATermAppl sort_fset =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) gsMakeSortExprList(sort_elt), false);

  //add substitution sort_bag -> sort_id
  ATermAppl subst = gsMakeSubst_Appl(sort_bag, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //create finite bag sort identifier
  ATermAppl sort_fbag = make_fresh_fbag_sort_id((ATerm) p_data_decls->sorts);
  //implement finite bags
  impl_sort_fbag(sort_elt, sort_fset, sort_fbag, p_substs, p_data_decls, new_data_equations);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(18,
      (ATerm) gsMakeOpIdBag(sort_elt, sort_fbag, sort_bag),
      (ATerm) gsMakeOpIdEmptyBag(sort_bag),
      (ATerm) gsMakeOpIdBagFBag(sort_fbag, sort_bag),
      (ATerm) gsMakeOpIdBagComp(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdCount(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdBagJoin(sort_bag),
      (ATerm) gsMakeOpIdBagIntersect(sort_bag),
      (ATerm) gsMakeOpIdBagDiff(sort_bag),
      (ATerm) gsMakeOpIdBag2Set(sort_bag, sort_set),
      (ATerm) gsMakeOpIdSet2Bag(sort_set, sort_bag),
      (ATerm) gsMakeOpIdZeroFunc(sort_elt),
      (ATerm) gsMakeOpIdOneFunc(sort_elt),
      (ATerm) gsMakeOpIdAddFunc(sort_elt),
      (ATerm) gsMakeOpIdMinFunc(sort_elt),
      (ATerm) gsMakeOpIdMonusFunc(sort_elt),
      (ATerm) gsMakeOpIdNat2BoolFunc(sort_elt),
      (ATerm) gsMakeOpIdBool2NatFunc(sort_elt)
  );

  ATermList new_data_eqns = build_bag_equations(sort_elt, sort_fset, sort_fbag, sort_set, sort_bag);

  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls, new_data_equations);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);
  *new_data_equations = ATconcat(new_data_eqns, *new_data_equations);
}

void impl_sort_bool(t_data_decls *p_data_decls)
{
  //Declare sort Bool
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_bool_::bool_()));
  function_symbol_list constructors = sort_bool_::bool__generate_constructors_code();
  function_symbol_list functions = sort_bool_::bool__generate_functions_code();
  data_equation_list equations = sort_bool_::bool__generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);
}

void impl_sort_pos(t_data_decls *p_data_decls, ATermList* new_data_equations)
{
  //Declare sort Pos
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_pos::pos()));
  function_symbol_list constructors = sort_pos::pos_generate_constructors_code();
  function_symbol_list functions = sort_pos::pos_generate_functions_code();
  data_equation_list equations = sort_pos::pos_generate_equations_code();
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
  function_symbol_list constructors = sort_nat::nat_generate_constructors_code();
  function_symbol_list functions = sort_nat::nat_generate_functions_code();
  data_equation_list equations = sort_nat::nat_generate_equations_code();
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
  function_symbol_list constructors = sort_int_::int__generate_constructors_code();
  function_symbol_list functions = sort_int_::int__generate_functions_code();
  data_equation_list equations = sort_int_::int__generate_equations_code();
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
  function_symbol_list constructors = sort_real_::real__generate_constructors_code();
  function_symbol_list functions = sort_real_::real__generate_functions_code();
  data_equation_list equations = sort_real_::real__generate_equations_code();
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
  function_symbol_list ops = standard_generate_functions_code(sort_expression(sort));
  data_equation_list eqns = standard_generate_equations_code(sort_expression(sort));

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

//ATermAppl make_struct_fset(ATermAppl sort_elt, ATermAppl sort_fset)
//{
//  return gsMakeSortStruct(ATmakeList2(
//    (ATerm) gsMakeStructCons(gsMakeOpIdNameFSetEmpty(), ATmakeList0(), gsMakeNil()),
//    (ATerm) gsMakeStructCons(gsMakeOpIdNameFSetCons(), ATmakeList2(
//      (ATerm) gsMakeStructProj(gsMakeNil(), sort_elt),
//      (ATerm) gsMakeStructProj(gsMakeNil(), sort_fset)
//    ), gsMakeNil())
//  ));
//}

//ATermAppl make_struct_fbag(ATermAppl sort_elt, ATermAppl sort_fset)
//{
//  return gsMakeSortStruct(ATmakeList2(
//    (ATerm) gsMakeStructCons(gsMakeOpIdNameFBagEmpty(), ATmakeList0(), gsMakeNil()),
//    (ATerm) gsMakeStructCons(gsMakeOpIdNameFBagCons(), ATmakeList3(
//      (ATerm) gsMakeStructProj(gsMakeNil(), sort_elt),
//      (ATerm) gsMakeStructProj(gsMakeNil(), gsMakeSortExprPos()),
//      (ATerm) gsMakeStructProj(gsMakeNil(), sort_fset)
//    ), gsMakeNil())
//  ));
//}

ATermAppl make_struct_bag_elt(ATermAppl sort_elt)
{
  return gsMakeSortStruct(ATmakeList1((ATerm)
    gsMakeStructCons(gsMakeOpIdNameBagElt(), ATmakeList2(
      (ATerm) gsMakeStructProj(gsMakeNil(), sort_elt),
      (ATerm) gsMakeStructProj(gsMakeNil(), gsMakeSortExprPos())
    ), gsMakeNil())
  ));
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
  }   // namespace core
}     // namespace mcrl2
