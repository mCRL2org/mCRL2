// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file dataimpl.cpp

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/detail/data_implementation_concrete.h"
#include "mcrl2/core/detail/data_common.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace ::mcrl2::utilities;
using namespace mcrl2::core::detail;
using namespace atermpp;

 namespace mcrl2 {
  namespace data {
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

static ATermAppl impl_sort_refs(ATermAppl spec);
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
//     TODO:
//     - p+k, where k is a constant of sort Pos: replace by p,
//       add condition p>=k, and add substitution [p -> Int2Pos(p-k)]
//       (for the condition and the rhs)
//     - n+k, where k is a constant of sort Nat: replace by n,
//       add condition n>=k, and add substitution [n -> Int2Nat(n-k)]
//     - -p: replace by cNeg(p)
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

static ATermList impl_exprs_list(ATermList parts, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: parts consists of parts of a specification that adheres to the internal
//     syntax after type checking
//     p_substs is a pointer to a list of substitutions induced by the context
//     of parts
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of part
//Ret: parts in which:
//     - all substitutions of *p_substs are performed on the elements of parts
//     - each substituted element is implemented, where the new data
//       declarations are stored in *p_data_decls

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

static ATermAppl impl_sort_struct(ATermAppl sort_struct, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_struct is a structured sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_struct
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_struct
//Post:an implementation of sort_struct is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_struct

static void split_sort_decls(ATermList sort_decls, ATermList *p_sort_ids,
  ATermList *p_sort_refs);
//Pre: sort_decls is a list of sort_id's and sort_ref's
//Post:*p_sort_ids and *p_sort_refs contain the sort_id's and sort_ref's from
//     sort_decls, in the same order

//implementation
//--------------

ATermAppl implement_data_spec(ATermAppl spec)
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
  ATermList substs     = ATmakeList0();
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  spec = impl_exprs_appl(spec, &substs, &data_decls);
  //perform substitutions on data declarations
  data_decls.sorts     = gsSubstValues_List(substs, data_decls.sorts,     true);
  data_decls.cons_ops  = gsSubstValues_List(substs, data_decls.cons_ops,  true);
  data_decls.ops       = gsSubstValues_List(substs, data_decls.ops,       true);
  data_decls.data_eqns = gsSubstValues_List(substs, data_decls.data_eqns, true);
  //add implementation of sort Pos and Bool
  impl_sort_pos(&data_decls);
  impl_sort_bool(&data_decls);
  //add new data declarations to spec
  spec = add_data_decls(spec, data_decls);
  //implement numerical pattern matching
  spec = impl_numerical_pattern_matching(spec);
  //implement sort references
  spec = impl_sort_refs(spec);
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
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  lps_spec = impl_exprs_appl(lps_spec, &substs, &data_decls);
  ar_spec = impl_exprs_appl(ar_spec, &substs, &data_decls);
  //perform substitutions on data declarations
  data_decls.sorts     = gsSubstValues_List(substs, data_decls.sorts,     true);
  data_decls.cons_ops  = gsSubstValues_List(substs, data_decls.cons_ops,  true);
  data_decls.ops       = gsSubstValues_List(substs, data_decls.ops,       true);
  data_decls.data_eqns = gsSubstValues_List(substs, data_decls.data_eqns, true);
  //add implementation of sort Pos and Bool
  impl_sort_pos(&data_decls);
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
  lps_spec = impl_sort_refs(lps_spec);
  ar_spec = impl_sort_refs(ar_spec);
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
    if (is_struct_sort_id(rhs) || is_list_sort_id(rhs) || is_set_sort_id(rhs) ||
      is_bag_sort_id(rhs))
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

ATermAppl impl_sort_refs(ATermAppl spec)
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
  ATermList substs = ATmakeList0();
  while (!ATisEmpty(sort_refs))
  {
    ATermAppl sort_ref = ATAgetFirst(sort_refs);
    ATermAppl lhs = gsMakeSortId(ATAgetArgument(sort_ref, 0));
    ATermAppl rhs = ATAgetArgument(sort_ref, 1);
    //if rhs is the first occurrence of an implementation of a type constructor
    //at the rhs of a sort reference, add rhs := lhs; otherwise add lhs := rhs
    ATermAppl subst;
    if (is_struct_sort_id(rhs) || is_list_sort_id(rhs) || is_set_sort_id(rhs) ||
      is_bag_sort_id(rhs))
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
  //perform substitutions on spec
  spec = gsSubstValues_Appl(substs, spec, true);
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
      data_expr = gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(ATAgetFirst(ATLgetArgument(data_expr, 1)))));
    } else if (gsIsDataExprNat2Int(data_expr)) {
      data_expr = gsMakeDataExprCInt(ATAgetFirst(ATLgetArgument(data_expr, 1)));
    } else if (gsIsDataExprNat2Real(data_expr)) {
      data_expr = gsMakeDataExprCReal(gsMakeDataExprCInt(ATAgetFirst(ATLgetArgument(data_expr, 1))));
    } else if (gsIsDataExprInt2Real(data_expr)) {
      data_expr = gsMakeDataExprCReal(ATAgetFirst(ATLgetArgument(data_expr, 1)));
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
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  spec = impl_exprs_appl(spec, &substs, &data_decls);
  part = impl_exprs_appl(part, &substs, &data_decls);
  //perform substitutions on data declarations
  data_decls.sorts     = gsSubstValues_List(substs, data_decls.sorts,     true);
  data_decls.cons_ops  = gsSubstValues_List(substs, data_decls.cons_ops,  true);
  data_decls.ops       = gsSubstValues_List(substs, data_decls.ops,       true);
  data_decls.data_eqns = gsSubstValues_List(substs, data_decls.data_eqns, true);
  //add implementation of sort Pos and Bool
  impl_sort_pos(&data_decls);
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
  spec = impl_sort_refs(spec);
  //implement standard functions
  spec = impl_standard_functions_spec(spec);
  return part;
}

ATermAppl impl_exprs_appl(ATermAppl part, ATermList *p_substs,
  t_data_decls *p_data_decls)
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
    part = impl_sort_struct(part, p_substs, p_data_decls);
  } else if (gsIsSortExprList(part)) {
    //part is a list sort; replace by a new sort and add data declarations for
    //this sort
    part = impl_sort_list(part, p_substs, p_data_decls);
  } else if (gsIsSortExprSet(part)) {
    //part is a set sort; replace by a new sort and add data declarations for
    //this sort
    part = impl_sort_set(part, p_substs, p_data_decls);
  } else if (gsIsSortExprBag(part)) {
    //part is a bag sort; replace by a new sort and add data declarations for
    //this sort
    part = impl_sort_bag(part, p_substs, p_data_decls);
  } else if (gsIsSortId(part)) {
    //part is a sort identifier; add data declarations for this sort, if needed
    if (ATisEqual(part,gsMakeSortIdNat()))
    {
      //add implementation of sort Nat, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
        impl_sort_nat(p_data_decls);
      }
    } else if (ATisEqual(part, gsMakeSortIdInt())) {
      //add implementation of sort Int, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
        impl_sort_int(p_data_decls);
      }
    } else if (ATisEqual(part, gsMakeSortIdReal())) {
      //add implementation of sort Int, if necessary
      if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdReal(), 0) == -1) {
        impl_sort_real(p_data_decls);
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
    else //sort of part is wrong
      gsWarningMsg("%P can not be implemented because its sort differs from "
        "Pos, Nat or Int\n", part);
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
        p_substs, p_data_decls);
      assert(!ATisEmpty(bound_vars));
      ATermAppl body = impl_exprs_appl(ATAgetArgument(part, 2),
        p_substs, p_data_decls);
      ATermList free_vars = impl_exprs_list(get_free_vars(part),
        p_substs, p_data_decls);
      //create sort for the new operation identifier
      ATermAppl op_id_sort = gsMakeSortArrowList(gsGetSorts(free_vars), gsMakeSortArrowList(gsGetSorts(bound_vars), gsGetSort(body)));
      //create new operation identifier
      ATermAppl op_id = make_fresh_lambda_op_id(op_id_sort, (ATerm) p_data_decls->ops);
      //add operation identifier to the data declarations
      p_data_decls->ops = ATinsert(p_data_decls->ops, (ATerm) op_id);
      //add data equation for the operation to the data declarations
      p_data_decls->data_eqns = ATinsert(p_data_decls->data_eqns, (ATerm)
        gsMakeDataEqn(ATconcat(free_vars, bound_vars), gsMakeNil(), gsMakeDataApplList(gsMakeDataApplList(op_id, free_vars), bound_vars), body));
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
            p_data_decls);
        else //ATgetType(arg) == AT_LIST
          args[i] = (ATerm) impl_exprs_list((ATermList) arg, p_substs,
            p_data_decls);
      }
      part = ATmakeApplArray(head, args);
      FREE_A(args);
    }
  }
  return part;
}

ATermList impl_exprs_list(ATermList parts, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  ATermList result = ATmakeList0();
  while (!ATisEmpty(parts))
  {
    result = ATinsert(result, (ATerm)
      impl_exprs_appl(ATAgetFirst(parts), p_substs, p_data_decls));
    parts = ATgetNext(parts);
  }
  return ATreverse(result);
}

ATermAppl impl_set_enum(ATermList elts, ATermAppl sort_expr)
{
  assert(ATgetLength(elts) > 0);
  ATermAppl result;
  //introduce a fresh variable
  ATermAppl var =
    gsMakeDataVarId(gsFreshString2ATermAppl("x", (ATerm) elts, true),
      gsGetSort(ATAgetFirst(elts)));
  //make body for the lambda abstraction
  elts = ATreverse(elts);
  ATermAppl elt = ATAgetFirst(elts);
  result = gsMakeDataExprEq(var, elt);
  elts = ATgetNext(elts);
  while (!ATisEmpty(elts)) {
    elt = ATAgetFirst(elts);
    result = gsMakeDataExprOr(gsMakeDataExprEq(var, elt), result);
    elts = ATgetNext(elts);
  }
  //make lambda abstraction
  result = gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) var), result);
  //make set comprehension
  result = gsMakeDataExprSetComp(result, sort_expr);
  //return result
  return result;
}

ATermAppl impl_bag_enum(ATermList elts, ATermAppl sort_expr)
{
  assert(ATgetLength(elts) > 0);
  ATermAppl result;
  //introduce a fresh variable
  ATermAppl var =
    gsMakeDataVarId(gsFreshString2ATermAppl("x", (ATerm) elts, true),
      gsGetSort(ATAgetFirst(elts)));
  //make body for the lambda abstraction
  elts = ATreverse(elts);
  ATermAppl amt = ATAgetFirst(elts);
  elts = ATgetNext(elts);
  ATermAppl elt = ATAgetFirst(elts);
  result = gsMakeDataExprIf(gsMakeDataExprEq(var, elt),
    amt, gsMakeDataExprC0());
  elts = ATgetNext(elts);
  while (!ATisEmpty(elts)) {
    amt = ATAgetFirst(elts);
    elts = ATgetNext(elts);
    elt = ATAgetFirst(elts);
    result = gsMakeDataExprAdd(
      gsMakeDataExprIf(gsMakeDataExprEq(var, elt),
      amt, gsMakeDataExprC0()), result);
    elts = ATgetNext(elts);
  }
  //make lambda abstraction
  result = gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) var), result);
  //make bag comprehension
  result = gsMakeDataExprBagComp(result, sort_expr);
  return result;
}

ATermAppl apply_op_id_to_vars(ATermAppl op_id, ATermList *p_args,
  ATermList *p_vars, ATerm context)
{
  ATermAppl sort = gsGetSort(op_id);
  assert(gsIsSortId(sort) || gsIsSortArrow(sort));
  *p_args = ATmakeList0();
  if (gsIsSortId(sort)) {
    return op_id;
  }
  
  assert(gsIsSortArrow(sort));
  
  ATermAppl t = op_id;
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
 
    // Add v to *p_args
    *p_args = ATinsert(*p_args,(ATerm) v);

    // Next
    sort_dom = ATgetNext(sort_dom);
  }
  
  // Apply t to p_args
  *p_args = ATreverse(*p_args);
  t = gsMakeDataAppl(t, *p_args);
  *p_vars = ATreverse(*p_vars);
  return t;
}     

ATermAppl impl_sort_struct(ATermAppl sort_struct, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortStruct(sort_struct));
  //declare fresh sort identifier for sort_struct
  ATermAppl sort_id = make_fresh_struct_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for this identifier
  ATermAppl subst = gsMakeSubst_Appl(sort_struct, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);
  //store constructor, projection and recogniser operations for this identifier
  ATermList cons_ops = ATmakeList0();
  ATermList proj_ops = ATmakeList0();
  ATermList projs = ATmakeList0();
  ATermList rec_ops = ATmakeList0();
  ATermList recs = ATmakeList0();
  ATermList struct_conss = ATLgetArgument(sort_struct, 0);
  while (!ATisEmpty(struct_conss))
  {
    ATermAppl struct_cons = ATAgetFirst(struct_conss);
    ATermAppl cons_name = ATAgetArgument(struct_cons, 0);
    ATermList struct_projs = ATLgetArgument(struct_cons, 1);
    ATermAppl rec_name = ATAgetArgument(struct_cons, 2);
    ATermList struct_cons_sorts = ATmakeList0();
    //store projection operations in proj_ops and store the implementations of
    //the sorts in struct_cons_sorts
    int i = 0;
    while (!ATisEmpty(struct_projs))
    {
      ATermAppl struct_proj = ATAgetFirst(struct_projs);
      ATermAppl proj_name = ATAgetArgument(struct_proj, 0);
      ATermAppl proj_sort = impl_exprs_appl(ATAgetArgument(struct_proj, 1),
        p_substs, p_data_decls);
      struct_cons_sorts = ATinsert(struct_cons_sorts, (ATerm) proj_sort);
      //store projection operation in proj_ops and projs
      if (!gsIsNil(proj_name)) {
        ATermAppl proj_op = gsMakeOpId(proj_name, gsMakeSortArrow1(sort_id, proj_sort));
        proj_ops = ATinsert(proj_ops, (ATerm) proj_op);
        projs = ATinsert(projs, (ATerm) ATmakeList2((ATerm) proj_op, (ATerm) ATmakeInt(i)));
      }
      struct_projs = ATgetNext(struct_projs);
      i++;
    }
    struct_cons_sorts = ATreverse(struct_cons_sorts);
    //store constructor operation in cons_ops
    ATermAppl cons_op;
    if (ATisEmpty(struct_cons_sorts))
    {
      cons_op = gsMakeOpId(cons_name, sort_id);
    } else {
      cons_op = gsMakeOpId(cons_name, gsMakeSortArrow(struct_cons_sorts, sort_id));
    }
    cons_ops = ATinsert(cons_ops, (ATerm) cons_op);
    //store recogniser in rec_ops and recs
    if (!gsIsNil(rec_name)) {
      ATermAppl rec_op = gsMakeOpId(rec_name, gsMakeSortArrow1(sort_id, gsMakeSortExprBool()));
      rec_ops = ATinsert(rec_ops, (ATerm) rec_op);
      recs = ATinsert(recs, (ATerm) ATmakeList2((ATerm) rec_op, (ATerm) cons_op));
    }
    //add constructor to projs
    ATermList tmpl = ATmakeList0();
    for (; !ATisEmpty(projs); projs=ATgetNext(projs))
    {
      tmpl = ATinsert(tmpl, (ATerm) ATappend(ATLgetFirst(projs), (ATerm) cons_op));
    }
    projs = ATreverse(tmpl);
    struct_conss = ATgetNext(struct_conss);
  }
  //add declarations for the constructor, projection and recogniser operations
  p_data_decls->cons_ops = ATconcat(ATreverse(cons_ops), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(ATconcat(ATreverse(proj_ops), ATreverse(rec_ops)),
    p_data_decls->ops);
  //Declare data equations for structured sort
  ATermList op_eqns = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  //ATermAppl t = gsMakeDataExprTrue();
  //ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  // XXX more intelligent variable names would be nice
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), sort_id);
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), sort_id);
  ATermList vars = ATmakeList3((ATerm) b, (ATerm) x, (ATerm) y);
  ATermList rhsv = ATmakeList0();
  ATermList lhsv = ATmakeList0();
  ATermList id_ctx = ATconcat(p_data_decls->sorts,
                      ATconcat(p_data_decls->ops,p_data_decls->cons_ops));
  //store equations for projections in op_eqns
  for (; !ATisEmpty(projs); projs=ATgetNext(projs))
  {
    ATermList l = ATLgetFirst(projs);
    ATermAppl proj_op = ATAgetFirst(l);
    l = ATgetNext(l);
    int proj_op_index = ATgetInt((ATermInt) ATgetFirst(l));
    l = ATgetNext(l);
    ATermAppl cons_op = ATAgetFirst(l);
    //Apply constructor cons_op to (fresh) variables and store its arguments in lhsv
    ATermAppl cons_expr = apply_op_id_to_vars(cons_op, &lhsv, &vars, (ATerm) id_ctx);
    //Add equation for projection function proj_op
    op_eqns = ATinsert(op_eqns,
      (ATerm) gsMakeDataEqn(lhsv, nil,
        gsMakeDataAppl1(proj_op, cons_expr),
        ATAelementAt(lhsv, proj_op_index)));
  }
  //store equations for recognition in op_eqns
  for (; !ATisEmpty(recs); recs=ATgetNext(recs))
  {
    ATermList l = ATLgetFirst(recs);
    ATermAppl rec_op = ATAgetFirst(l);
    l = ATgetNext(l);
    ATermAppl rec_cons_op = ATAgetFirst(l);
    // Add equation for every constructor
    for (ATermList m = cons_ops; !ATisEmpty(m); m=ATgetNext(m))
    {
      ATermAppl cons_op = ATAgetFirst(m);
      // Apply constructor cons_op to (fresh) variables and store its
      // arguments in lhsv
      ATermAppl cons_expr = apply_op_id_to_vars(cons_op, &lhsv, &vars, (ATerm) id_ctx);
      // Add right equation to op_eqns
      op_eqns = ATinsert(op_eqns, (ATerm) gsMakeDataEqn(lhsv, nil,
         gsMakeDataAppl1(rec_op, cons_expr),
         ATisEqual(ATAgetFirst(m), rec_cons_op)?gsMakeDataExprTrue():gsMakeDataExprFalse()));
    }
  }
  //store equations for equalities in op_eqns
  //one equation for every pair of constructors
  for (ATermList l=cons_ops; !ATisEmpty(l); l=ATgetNext(l))
  {
    for (ATermList m=cons_ops; !ATisEmpty(m); m=ATgetNext(m))
    {
      ATermAppl cons_op_lhs = ATAgetFirst(l);
      ATermAppl cons_op_rhs = ATAgetFirst(m);
      // Save vars list
      // Apply constructor cons_op_lhs to (fresh) variables and store its
      // arguments in lhsv
      ATermAppl cons_expr_lhs = apply_op_id_to_vars(cons_op_lhs, &lhsv, &vars, (ATerm) id_ctx);
      // Apply constructor cons_op_rhs to (fresh) variables and store its
      // arguments in rhsv (making sure we don't use the vars that occur in t)
      ATermList tmpvars = subtract_list(vars, lhsv);
      ATermAppl cons_expr_rhs = apply_op_id_to_vars(ATAgetFirst(m), &rhsv, &tmpvars, (ATerm) ATconcat(lhsv, id_ctx));
      // Update vars
      vars = merge_list(vars, rhsv);
      // Combine variable lists of lhs and rhs
      ATermList vs = ATconcat(lhsv, rhsv);
      // Create right result
      ATermAppl result_expr = NULL;
      if ( ATisEqual(cons_op_lhs, cons_op_rhs) )
      {
        // Constructors are the same, so match all variables
        if (ATisEmpty(lhsv)) {
          result_expr = gsMakeDataExprTrue();
        } else {
          for (; !ATisEmpty(lhsv); lhsv = ATgetNext(lhsv), rhsv = ATgetNext(rhsv))
          {
            if ( result_expr == NULL )
            {
              result_expr = gsMakeDataExprEq(ATAgetFirst(lhsv), ATAgetFirst(rhsv));
            } else {
              result_expr = gsMakeDataExprAnd(result_expr,
                gsMakeDataExprEq(ATAgetFirst(lhsv), ATAgetFirst(rhsv)));
            }
          }
        }
      } else {
        // Different constructor, so not equal
        result_expr = gsMakeDataExprFalse();
      }
      // Add equation to op_eqns
      op_eqns = ATinsert(op_eqns, (ATerm) gsMakeDataEqn(vs, nil,
        gsMakeDataExprEq(cons_expr_lhs, cons_expr_rhs), result_expr));
    }
  }
  //Add op_eqns to data_eqns
  p_data_decls->data_eqns = ATconcat(op_eqns, p_data_decls->data_eqns);

  return sort_id;
}

ATermList build_list_equations(ATermAppl sort_elt, ATermAppl sort_list)
{
  data_equation_list equations = sort_list::list_generate_equations_code(sort_expression(sort_elt));
  return atermpp::term_list<data_equation>(equations.begin(), equations.end());
  /*
  //Declare data equations for sort sort_id
  ATermList el = ATmakeList0();
  ATermAppl el_sort_id = gsMakeDataExprEmptyList(sort_list);
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_list);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_list);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl e_sort_elt = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl ds = gsMakeDataExprCons(d_sort_elt, s_sort_id);
  ATermAppl es = gsMakeDataExprCons(e_sort_elt, s_sort_id);
  ATermAppl et = gsMakeDataExprCons(e_sort_elt, t_sort_id);
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList dl = ATmakeList1((ATerm) d_sort_elt);
  ATermList sl = ATmakeList1((ATerm) s_sort_id);
  ATermList dsl = ATmakeList2((ATerm) d_sort_elt, (ATerm) s_sort_id);
  ATermList desl = ATmakeList3((ATerm) d_sort_elt, (ATerm) e_sort_elt,
    (ATerm) s_sort_id);
  ATermList dstl = ATmakeList3((ATerm) d_sort_elt, (ATerm) s_sort_id,
    (ATerm) t_sort_id);
  ATermList destl = ATmakeList4((ATerm) d_sort_elt, (ATerm) e_sort_elt,
    (ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList dspl = ATmakeList3((ATerm) d_sort_elt, (ATerm) s_sort_id, (ATerm) p);

  ATermList new_data_eqns = ATmakeList(21,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(el_sort_id, ds), f),
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(ds, el_sort_id), f),
      (ATerm) gsMakeDataEqn(destl, nil,
        gsMakeDataExprEq(ds, et),
        gsMakeDataExprAnd(
          gsMakeDataExprEq(d_sort_elt, e_sort_elt),
          gsMakeDataExprEq(s_sort_id, t_sort_id))),
      (ATerm) gsMakeDataEqn(sl, nil, gsMakeDataExprEq(s_sort_id, s_sort_id), t),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprEltIn(d_sort_elt, el_sort_id),
        f),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprEltIn(d_sort_elt, es),
        gsMakeDataExprOr(
          gsMakeDataExprEq(d_sort_elt, e_sort_elt),
          gsMakeDataExprEltIn(d_sort_elt, s_sort_id))),
      //list size (sort_id -> Nat)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprListSize(el_sort_id),
        gsMakeDataExprC0()),
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprListSize(ds),
        gsMakeDataExprCNat(
          gsMakeDataExprSucc(gsMakeDataExprListSize(s_sort_id)))),
      //list snoc (sort_id -> sort_elt -> sort_id)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprSnoc(el_sort_id, d_sort_elt),
        gsMakeDataExprCons(d_sort_elt, el_sort_id)),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprSnoc(ds, e_sort_elt),
        gsMakeDataExprCons(d_sort_elt, gsMakeDataExprSnoc(s_sort_id, e_sort_elt))),
      //list concatenation (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprConcat(el_sort_id, s_sort_id),
        s_sort_id),
      (ATerm) gsMakeDataEqn(dstl, nil,
        gsMakeDataExprConcat(ds, t_sort_id),
        gsMakeDataExprCons(d_sort_elt, gsMakeDataExprConcat(s_sort_id, t_sort_id))),
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprConcat(s_sort_id, el_sort_id),
        s_sort_id),
      //list element at (sort_id -> Nat -> sort_elt)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprEltAt(ds, zero, sort_elt),
        d_sort_elt),
      (ATerm) gsMakeDataEqn(dspl, nil,
        gsMakeDataExprEltAt(ds, gsMakeDataExprCNat(p), sort_elt),
        gsMakeDataExprEltAt(s_sort_id, gsMakeDataExprPred(p), sort_elt)),
      //head (sort_id -> sort_elt)
      (ATerm) gsMakeDataEqn(dsl, nil,
         gsMakeDataExprHead(ds, sort_elt),
         d_sort_elt),
      //tail (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(dsl, nil,
         gsMakeDataExprTail(ds),
         s_sort_id),
      //right head (sort_id -> sort_elt)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprRHead(gsMakeDataExprCons(d_sort_elt, el_sort_id), sort_elt),
        d_sort_elt),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprRHead(gsMakeDataExprCons(d_sort_elt,
          gsMakeDataExprCons(e_sort_elt, s_sort_id)), sort_elt),
        gsMakeDataExprRHead(gsMakeDataExprCons(e_sort_elt, s_sort_id), sort_elt)),
      //right tail (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprRTail(gsMakeDataExprCons(d_sort_elt, el_sort_id)),
        el_sort_id),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprRTail(gsMakeDataExprCons(d_sort_elt,
          gsMakeDataExprCons(e_sort_elt, s_sort_id))),
        gsMakeDataExprCons(d_sort_elt,
          gsMakeDataExprRTail(gsMakeDataExprCons(e_sort_elt, s_sort_id)))));

  return new_data_eqns;
  */
}

ATermAppl impl_sort_list(ATermAppl sort_list, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprList(sort_list));
  ATermAppl sort_elt = ATAgetArgument(sort_list, 1);

  ATermAppl sort_id = make_fresh_list_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);

  //implement sort_elt
  //this needs to be done first in order to keep the substitutions sound!
  sort_elt = impl_exprs_appl(sort_elt, p_substs, p_data_decls);
  sort_expression element_sort(sort_elt);
  //add substitution for sort_list
  ATermAppl subst = gsMakeSubst_Appl(sort_list, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);
  function_symbol_list constructors = sort_list::list_generate_constructors_code(element_sort);
  function_symbol_list functions = sort_list::list_generate_functions_code(element_sort);
  data_equation_list equations = sort_list::list_generate_equations_code(element_sort);

  //perform substitutions
//  constructors = gsSubstValues_List(*p_substs, constructors, true);
//  functions = gsSubstValues_List(*p_substs, functions, true);
//  equations = gsSubstValues_List(*p_substs, equations, true);

  p_data_decls->cons_ops = ATconcat(gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), true), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(gsSubstValues_List(*p_substs, atermpp::term_list<function_symbol>(functions.begin(), functions.end()), true), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(gsSubstValues_List(*p_substs, atermpp::term_list<data_equation>(equations.begin(), equations.end()), true), p_data_decls->data_eqns);

  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;

/*
  //declare constructors for sort sort_id
  ATermList new_cons_ops = ATmakeList2(
      (ATerm) gsMakeOpIdEmptyList(sort_list),
      (ATerm) gsMakeOpIdCons(sort_elt, sort_list));

  //Declare operations for sort sort_id
  ATermList new_ops = ATmakeList(9,
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_list),
      (ATerm) gsMakeOpIdListSize(sort_list),
      (ATerm) gsMakeOpIdSnoc(sort_list, sort_elt),
      (ATerm) gsMakeOpIdConcat(sort_list),
      (ATerm) gsMakeOpIdEltAt(sort_list, sort_elt),
      (ATerm) gsMakeOpIdHead(sort_list, sort_elt),
      (ATerm) gsMakeOpIdTail(sort_list),
      (ATerm) gsMakeOpIdRHead(sort_list, sort_elt),
      (ATerm) gsMakeOpIdRTail(sort_list));

  ATermList new_data_eqns = build_list_equations(sort_elt, sort_list);

  //declare fresh sort identifier for sort_list
  ATermAppl sort_id = make_fresh_list_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //implement sort_elt
  //this needs to be done first in order to keep the substitutions sound!
  sort_elt = impl_exprs_appl(sort_elt, p_substs, p_data_decls);
  //add substitution for sort_list
  ATermAppl subst = gsMakeSubst_Appl(sort_list, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //perform substitutions
  new_cons_ops = gsSubstValues_List(*p_substs, new_cons_ops, true);
  p_data_decls->cons_ops = ATconcat(new_cons_ops, p_data_decls->cons_ops);
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;
*/
}

ATermList build_set_equations(ATermAppl sort_elt, ATermAppl sort_set)
{
  //declare data equations for sort sort_id
  ATermAppl sort_func = gsMakeSortArrow1(sort_elt, gsMakeSortExprBool());
  ATermList el = ATmakeList0();
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_set);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_set);
  ATermAppl f_sort_func = gsMakeDataVarId(gsString2ATermAppl("f"), sort_func);
  ATermAppl g_sort_func = gsMakeDataVarId(gsString2ATermAppl("g"), sort_func);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl x_sort_elt = gsMakeDataVarId(gsString2ATermAppl("x"), sort_elt);
  ATermAppl nil = gsMakeNil();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList stl = ATmakeList2((ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList fl = ATmakeList1((ATerm) f_sort_func);
  ATermList dfl = ATmakeList2((ATerm) d_sort_elt, (ATerm) f_sort_func);
  ATermList fgl = ATmakeList2((ATerm) f_sort_func, (ATerm) g_sort_func);
  ATermAppl false_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt), f);
  ATermAppl imp_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprImp(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl OrFunc =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprOr(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl and_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprAnd(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl not_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprNot(gsMakeDataAppl1(f_sort_func, x_sort_elt))
    );

  ATermList new_data_eqns = ATmakeList(9,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprEq(f_sort_func, g_sort_func)),
      //empty set (sort_id)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptySet(sort_set),
        gsMakeDataExprSetComp(false_func, sort_set)),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprEltIn(d_sort_elt, gsMakeDataExprSetComp(f_sort_func, sort_set)),
        gsMakeDataAppl1(f_sort_func, d_sort_elt)),
      //subset or equal (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubSetEq(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprForall(imp_func)),
      //proper subset (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSubSet(s_sort_id, t_sort_id),
        gsMakeDataExprAnd(
          gsMakeDataExprSubSetEq(s_sort_id, t_sort_id), 
          gsMakeDataExprNeq(s_sort_id, t_sort_id)
        )),
      //union (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetUnion(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprSetComp(OrFunc, sort_set)),
      //difference (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSetDiff(s_sort_id, t_sort_id),
        gsMakeDataExprSetInterSect(s_sort_id, gsMakeDataExprSetCompl(t_sort_id))),
      //intersection (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetInterSect(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprSetComp(and_func, sort_set)),
      //complement (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fl, nil,
        gsMakeDataExprSetCompl(gsMakeDataExprSetComp(f_sort_func, sort_set)),
        gsMakeDataExprSetComp(not_func, sort_set)));
      //simplification of combinations of functions false, not, imp, and, or
      ////left unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), false_func, f_sort_func),
      //  f_sort_func),
      ////right unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), f_sort_func, false_func),
      //  f_sort_func)

  return new_data_eqns;
}

ATermAppl impl_sort_set(ATermAppl sort_set, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprSet(sort_set));
  ATermAppl sort_elt = ATAgetArgument(sort_set, 1);

  //implement expressions in the target sort of sort_set
  //this needs to be done first to keep the substitutions sound!
  impl_exprs_appl(sort_elt, p_substs, p_data_decls);

  //declare fresh sort identifier for sort_set
  ATermAppl sort_id = make_fresh_set_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for sort_set
  ATermAppl subst = gsMakeSubst_Appl(sort_set, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(9,
      (ATerm) gsMakeOpIdSetComp(sort_elt, sort_set),
      (ATerm) gsMakeOpIdEmptySet(sort_set),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_set),
      (ATerm) gsMakeOpIdSubSetEq(sort_set),
      (ATerm) gsMakeOpIdSubSet(sort_set),
      (ATerm) gsMakeOpIdSetUnion(sort_set),
      (ATerm) gsMakeOpIdSetDiff(sort_set),
      (ATerm) gsMakeOpIdSetIntersect(sort_set),
      (ATerm) gsMakeOpIdSetCompl(sort_set));

  ATermList new_data_eqns = build_set_equations(sort_elt, sort_set);

  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);

  return sort_id;
}

ATermList build_bag_equations(ATermAppl sort_elt, ATermAppl sort_bag, ATermAppl sort_set)
{
  //declare data equations for sort sort_id
  ATermAppl sort_func = gsMakeSortArrow1(sort_elt, gsMakeSortExprNat());
  ATermList el = ATmakeList0();
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_bag);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_bag);
  ATermAppl f_sort_func = gsMakeDataVarId(gsString2ATermAppl("f"), sort_func);
  ATermAppl g_sort_func = gsMakeDataVarId(gsString2ATermAppl("g"), sort_func);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl x_sort_elt = gsMakeDataVarId(gsString2ATermAppl("x"), sort_elt);
  ATermAppl y_sort_elt = gsMakeDataVarId(gsString2ATermAppl("y"), sort_elt);
  ATermAppl u_sort_set = gsMakeDataVarId(gsString2ATermAppl("u"), sort_set);
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermList sl = ATmakeList1((ATerm) s_sort_id);
  ATermList stl = ATmakeList2((ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList dsl = ATmakeList2((ATerm) d_sort_elt, (ATerm) s_sort_id);
  ATermList ul = ATmakeList1((ATerm) u_sort_set);
  ATermList dfl = ATmakeList2((ATerm) d_sort_elt, (ATerm) f_sort_func);
  ATermList fgl = ATmakeList2((ATerm) f_sort_func, (ATerm) g_sort_func);
  ATermAppl zero_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt), zero);
  ATermAppl lte_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprLTE(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl add_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprAdd(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl subt_max0_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) y_sort_elt),
      gsMakeDataAppl2(
        gsMakeBinder(gsMakeLambda(), ATmakeList2((ATerm) m, (ATerm) n),
          gsMakeDataExprIf(
            gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
          )
        ),
        gsMakeDataAppl1(f_sort_func, y_sort_elt),
        gsMakeDataAppl1(g_sort_func, y_sort_elt)));
  /*
  ATermAppl subt_max0_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeWhr(
        gsMakeDataExprIf(
          gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
        ), ATmakeList2(
          (ATerm) gsMakeDataVarIdInit(m, gsMakeDataAppl1(f_sort_func, x_sort_elt)),
          (ATerm) gsMakeDataVarIdInit(n, gsMakeDataAppl1(g_sort_func, x_sort_elt))
        )
      )
    );
  */
  ATermAppl min_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprMin(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl bag2set_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprEltIn(x_sort_elt, s_sort_id)
    );
  ATermAppl set2bag_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprIf(
        gsMakeDataExprEltIn(x_sort_elt, u_sort_set),
        gsMakeDataExprNat_int(1),
        gsMakeDataExprNat_int(0)
      )
    );

  ATermList new_data_eqns = ATmakeList(11,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprEq(f_sort_func, g_sort_func)),
      //empty bag (sort_id)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptyBag(sort_bag),
        gsMakeDataExprBagComp(zero_func, sort_bag)),
      //count (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprCount(d_sort_elt, gsMakeDataExprBagComp(f_sort_func, sort_bag)),
        gsMakeDataAppl1(f_sort_func, d_sort_elt)),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprEltIn(d_sort_elt, s_sort_id),
        gsMakeDataExprGT(gsMakeDataExprCount(d_sort_elt, s_sort_id), zero)),
      //subbag or equal (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubBagEq(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprForall(lte_func)),
      //proper subbag (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSubBag(s_sort_id, t_sort_id),
        gsMakeDataExprAnd(
          gsMakeDataExprSubBagEq(s_sort_id, t_sort_id), 
          gsMakeDataExprNeq(s_sort_id, t_sort_id)
        )),
      //union (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagUnion(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprBagComp(add_func, sort_bag)),
      //difference (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagDiff(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprBagComp(subt_max0_func, sort_bag)),
      //intersection (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagInterSect(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprBagComp(min_func, sort_bag)),
      //Bag2Set (sort_id -> sort_set)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprBag2Set(s_sort_id, sort_set),
        gsMakeDataExprSetComp(bag2set_func, sort_set)),
      //Set2Bag (sort_set -> sort_id)
      (ATerm) gsMakeDataEqn(ul, nil,
        gsMakeDataExprSet2Bag(u_sort_set, sort_bag),
        gsMakeDataExprBagComp(set2bag_func, sort_bag)));

  return new_data_eqns;
}

ATermAppl impl_sort_bag(ATermAppl sort_bag, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprBag(sort_bag));
  ATermAppl sort_elt = ATAgetArgument(sort_bag, 1);
  ATermAppl sort_set = gsMakeSortExprSet(sort_elt);

  //implement expressions in the target sort of sort_bag
  //this needs to be done first in order to keep the substitutions sound!
  impl_exprs_appl(sort_elt, p_substs, p_data_decls);
  //add implementation of sort Set(sort_elt), if necessary
  ATermAppl sort_set_impl =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_set, false);
  if (ATisEqual(sort_set_impl, sort_set)) {
    //Set(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    sort_set_impl = impl_sort_set(sort_set, p_substs, p_data_decls);
  }
  //declare fresh sort identifier for sort_bag
  ATermAppl sort_id = make_fresh_bag_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for this identifier
  ATermAppl subst = gsMakeSubst_Appl(sort_bag, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(11,
      (ATerm) gsMakeOpIdBagComp(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdEmptyBag(sort_bag),
      (ATerm) gsMakeOpIdCount(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdSubBagEq(sort_bag),
      (ATerm) gsMakeOpIdSubBag(sort_bag),
      (ATerm) gsMakeOpIdBagUnion(sort_bag),
      (ATerm) gsMakeOpIdBagDiff(sort_bag),
      (ATerm) gsMakeOpIdBagIntersect(sort_bag),
      (ATerm) gsMakeOpIdBag2Set(sort_bag, sort_set),
      (ATerm) gsMakeOpIdSet2Bag(sort_set, sort_bag));

  ATermList new_data_eqns = build_bag_equations(sort_elt, sort_bag, sort_set);

  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);

  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;
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

void impl_sort_pos(t_data_decls *p_data_decls)
{
  //Declare sort Pos
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_pos::pos()));
  function_symbol_list constructors = sort_pos::pos_generate_constructors_code();
  function_symbol_list functions = sort_pos::pos_generate_functions_code();
  data_equation_list equations = sort_pos::pos_generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);
}

void impl_sort_nat(t_data_decls *p_data_decls)
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
}

void impl_sort_int(t_data_decls *p_data_decls)
{
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_int_::int_()));
  function_symbol_list constructors = sort_int_::int__generate_constructors_code();
  function_symbol_list functions = sort_int_::int__generate_functions_code();
  data_equation_list equations = sort_int_::int__generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);

  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
}

void impl_sort_real(t_data_decls *p_data_decls)
{
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, static_cast<const aterm&>(sort_real_::real_()));
  function_symbol_list constructors = sort_real_::real__generate_constructors_code();
  function_symbol_list functions = sort_real_::real__generate_functions_code();
  data_equation_list equations = sort_real_::real__generate_equations_code();
  p_data_decls->cons_ops = ATconcat(atermpp::term_list<function_symbol>(constructors.begin(), constructors.end()), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(atermpp::term_list<function_symbol>(functions.begin(), functions.end()), p_data_decls->ops);
  p_data_decls->data_eqns = ATconcat(atermpp::term_list<data_equation>(equations.begin(), equations.end()), p_data_decls->data_eqns);

  //add implementation of sort Int, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
    impl_sort_int(p_data_decls);
  }
}

void impl_standard_functions_sort(ATermAppl sort, t_data_decls *p_data_decls)
{
  assert(gsIsSortExpr(sort));
  //Declare operations for sort
  p_data_decls->ops = ATconcat(ATmakeList(3,
      (ATerm) gsMakeOpIdEq(sort),
      (ATerm) gsMakeOpIdNeq(sort),
      (ATerm) gsMakeOpIdIf(sort)
    ), p_data_decls->ops);
  //Declare data equations for sort sort
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), sort);
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), sort);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList xl = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList bxl = ATmakeList2((ATerm) b, (ATerm) x);
  p_data_decls->data_eqns = ATconcat(ATmakeList(5,
      //equality (sort_arrow -> sort_arrow -> Bool)
      (ATerm) gsMakeDataEqn(xl, nil,
        gsMakeDataExprEq(x, x), t),
      //inequality (sort_arrow -> sort_arrow -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprNeq(x, y),
        gsMakeDataExprNot(gsMakeDataExprEq(x, y))),
      //conditional (Bool -> sort_arrow -> sort_arrow -> sort_arrow)
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprIf(t, x, y),
        x),
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprIf(f, x, y),
        y),
      (ATerm) gsMakeDataEqn(bxl,nil,
        gsMakeDataExprIf(b, x, x),
        x)
    ), p_data_decls->data_eqns);
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
