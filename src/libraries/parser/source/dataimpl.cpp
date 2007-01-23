#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "dataimpl.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"


//local declarations
//------------------


static ATermAppl impl_exprs_spec(ATermAppl spec);
//Pre: spec is a specification that adheres to the internal syntax after
//     type checking in which sort references are maximally folded
//Ret: spec in which all expressions are implemented

static ATermAppl impl_exprs(ATermAppl expr, lpe::specification &lpe_spec);
//Pre: expr represents a part of the internal syntax after type checking
//     in which sort references are maximally folded
//     lpe_spec represents an LPE specification
//Post:the datatypes of expr are implemented as in lpe_spec
//Ret: expr in which all expressions are implemented

static ATermAppl impl_sort_refs(ATermAppl spec);
//Pre: spec is a specification that adheres to the internal syntax after
//     data implementation, with the exception that sort references may occur
//Ret: spec in which all sort references are implemented, i.e.:
//     - all sort references are removed from spec
//     - if a sort reference is of the form sort_ref(n, e), where e is the
//       implementation of a type constructor and sort_ref(n, e) is the first
//       sort reference with e as a rhs, e is replaced by n in spec;
//       otherwise, n is replaced by e in spec

static ATermAppl impl_function_sorts_spec(ATermAppl spec);
//Pre: spec is a specification that adheres to the internal syntax after
//     data implementation
//Ret: spec in which an implementation is added for each function
//     sort occurring in spec that has not already been implemented

static void impl_function_sorts(ATerm term, lpe::specification &lpe_spec);
//Pre: term represents a part of the internal syntax after data
//     implementation
//     lpe_spec represents an LPE specification
//Post:an implementation is added to lpe_spec for each function sort
//     occurring in term that has not already been implemented

typedef struct {
  ATermList sorts;
  ATermList cons_ops;
  ATermList ops;
  ATermList data_eqns;
} t_data_decls;
//The type t_data_decls represents data declarations, i.e. sort, constructor,
//operation and data equation declarations

#define data_decls_is_initialised(data_decls)\
(data_decls.sorts != NULL && data_decls.cons_ops  != NULL &&\
 data_decls.ops   != NULL && data_decls.data_eqns != NULL)

//Ret: indicates whether the elements of data_decls are initialised

static ATermAppl add_data_decls(ATermAppl spec, t_data_decls data_decls);
//Pre: spec is a specification that adheres to the internal syntax of an
//     arbitary phase
//Ret: spec in which the data declarations from data_decls are added

static t_data_decls get_data_decls(lpe::specification &lpe_spec);
//Ret: data declarations of lpe_spec

static void set_data_decls(lpe::specification &lpe_spec, t_data_decls data_decls);
//Ret: lpe_spec in which the data declarations are replaced by data_decls

static ATermAppl impl_exprs_appl(ATermAppl part, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: part is a part of a specification that adheres to the internal syntax
//     after type checking
//     p_substs is a pointer to a list of substitutions induced by the context
//     of part
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of part
//Ret: part in which:
//     - all substitutions of *p_substs are performed on the elements of part
//     - each substituted element is implemented, where the new data
//       declarations are stored in *p_data_decls

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

static ATermList get_free_vars(ATermAppl data_expr);
//Pre: data_expr is a data expression that adheres to the internal syntax after
//     type checking
//Ret: The free variables in data_expr

static void get_free_vars_appl(ATermAppl data_expr, ATermList bound_vars,
  ATermList* p_free_vars);
//Pre: data_expr is a data expression or a bag enumeration element that adheres
//     to the internal format after type checking
//     bound_vars and *p_free_vars are lists of data variables, and represent the
//     bound/free variables of the context of data_expr
//Post:*p_free_vars is extended with the free variables in data_expr that did not
//     already occur in *p_free_vars or bound_vars

static void get_free_vars_list(ATermList data_exprs, ATermList bound_vars,
  ATermList* p_free_vars);
//Pre: data_exprs is a list of data expressions or bag enumeration elements that
//     adhere to the internal format after type checking
//     bound_vars and *p_free_vars are lists of data variables, and represent the
//     bound/free variables of the context of data_exprs
//Post:*p_free_vars is extended with the free variables in data_exprs that did not
//     already occur in *p_free_vars or bound_vars

static ATermList get_function_sorts(ATerm term);
//Pre: term adheres to the internal format
//Ret: a list of all function sorts occurring in term, where each element is
//     unique

static void get_function_sorts_appl(ATermAppl part, ATermList *p_func_sorts);
//Pre: part is a part of a specification
//     *p_func_sorts represents the function sorts that are already found
//Post:*p_func_sorts is extended with the function sorts in part that did not
//     already occur in *p_func_sorts

static void get_function_sorts_list(ATermList parts, ATermList *p_func_sorts);
//Pre: parts is a list of parts of a specification
//     *p_func_sorts represents the function sorts that are already found
//Post:*p_funct_sorts is extended with the function sorts in parts that did not
//     already occur in *p_func_sorts

static void impl_function_sort(ATermAppl sort_arrow, t_data_decls *p_data_decls);
//Pre: sort_arrow is an arrow sort that adheres to the internal syntax after
//     data implementation
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of part
//Post:an implementation of function sort sort_arrow is added to *p_data_decls

static ATermAppl apply_op_id_to_vars(ATermAppl op_id, ATermList *p_args,
                                   ATermList *p_vars, ATerm context);
//Pre: op_id is an operation identifier
//     p_args points to a list
//     p_vars points to a list, possibly containing DataVarIds
//     context is some term
//Post:*p_args contains a subset of *p_vars
//     *p_vars is extended with newly introduced variables (which do not occur
//     in context
//Ret: op_id applied to as much variables as possible, which are listed (in
//     order) in *p_args

static ATermList merge_list(ATermList l, ATermList m);
//Pre: l and m are two lists without duplicates
//Ret: a list with all elements of l and m precisely once

static ATermList subtract_list(ATermList l, ATermList m);
//Pre: l and m are two lists
//Ret: a copy of l without elements that occur in m

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

static ATermAppl impl_sort_list(ATermAppl sort_list, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_list is a list sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_list
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_list
//Post:an implementation of sort_list is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_list

static ATermAppl impl_sort_set(ATermAppl sort_set, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_set is a set sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_set
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_set
//Post:an implementation of sort_set is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_set

static ATermAppl impl_sort_bag(ATermAppl sort_bag, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_bag is a bag sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_bag
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_bag
//Post:an implementation of sort_bag is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_bag

static ATermAppl impl_sort_arrow_prod(ATermAppl sort_arrow_prod);
//Pre: sort_arrow_prod represents a function sort that adheres to the syntax
//     after data implementation
//Ret: an implementation of sort_arrow_prod
  
static void split_sort_decls(ATermList sort_decls, ATermList *p_sort_ids,
  ATermList *p_sort_refs);
//Pre: sort_decls is a list of sort_id's and sort_ref's
//Post:*p_sort_ids and *p_sort_refs contain the sort_id's and sort_ref's from
//     sort_decls, in the same order

static const char *struct_prefix = "Struct@";
static const char *list_prefix   = "List@";
static const char *set_prefix    = "Set@";
static const char *bag_prefix    = "Bag@";
static const char *lambda_prefix = "lambda@";

static ATermAppl make_fresh_struct_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: sort identifier for the implementation of a structured sort with prefix
//     struct_prefix, that does not occur in term

static ATermAppl make_fresh_list_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a list sort with prefix
//     list_prefix, that does not occur in term

static ATermAppl make_fresh_set_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a set sort with prefix
//     set_prefix, that does not occur in term

static ATermAppl make_fresh_bag_sort_id(ATerm term);
//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a bag sort with prefix
//     bag_prefix, that does not occur in term

static ATermAppl make_fresh_lambda_op_id(ATermAppl sort_expr, ATerm term);
//Pre: sort_expr is a sort expression
//     term is not NULL
//Ret: operation identifier op_id(n, s) for the implementation of a lambda
//     abstraction, where s is sort_expr and n is a name with prefix
//     lambda_prefix, that does not occur in term

static bool is_struct_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a structured sort

static bool is_list_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a list sort

static bool is_set_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a set sort

static bool is_bag_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a bag sort

//static bool is_lambda_op_id(ATermAppl data_expr);
////Pre: data_expr is a data expression
////Ret: data_expr is an operation identifier for the implementation of a lambda
////     abstraction

static void impl_sort_bool(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Bool is added to *p_data_decls

static void impl_sort_pos(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Pos is added to *p_data_decls

static void impl_sort_nat(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Nat is added to *p_data_decls

static void impl_sort_nat_pair(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort PairNat is added to *p_data_decls

static void impl_sort_int(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Int is added to *p_data_decls

static void impl_sort_real(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Real is added to *p_data_decls


//implementation
//--------------


ATermAppl implement_data_spec(ATermAppl spec)
{
  return impl_exprs_spec(spec);
}

ATermAppl implement_data_sort_expr(ATermAppl sort_expr,
  lpe::specification &lpe_spec)
{
  return impl_exprs(sort_expr, lpe_spec);
}

ATermAppl implement_data_data_expr(ATermAppl data_expr,
  lpe::specification &lpe_spec)
{
  return impl_exprs(data_expr, lpe_spec);
}

ATermAppl implement_data_mult_act(ATermAppl mult_act,
  lpe::specification &lpe_spec)
{
  return impl_exprs(mult_act, lpe_spec);
}

ATermAppl implement_data_proc_expr(ATermAppl proc_expr,
  lpe::specification &lpe_spec)
{
  return impl_exprs(proc_expr, lpe_spec);
}

ATermAppl implement_data_state_frm(ATermAppl state_frm,
  lpe::specification &lpe_spec)
{
  return impl_exprs(state_frm, lpe_spec);
}

ATermAppl impl_exprs_spec(ATermAppl spec)
{
  assert(gsIsSpecV1(spec));
  int occ = gsCount((ATerm) gsMakeUnknown(), (ATerm) spec);
  if (occ > 0) {
    gsErrorMsg("specification contains %d unknown type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }
  //implement system sort and data expressions occurring in spec
  ATermList substs     = ATmakeList0();
  t_data_decls data_decls;
  data_decls.sorts     = ATmakeList0();
  data_decls.cons_ops  = ATmakeList0();
  data_decls.ops       = ATmakeList0();
  data_decls.data_eqns = ATmakeList0();
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
  //implement sort references
  spec = impl_sort_refs(spec);
  //implement function sorts
  spec = impl_function_sorts_spec(spec);
  return spec;
}

static ATermAppl impl_exprs(ATermAppl expr, lpe::specification &lpe_spec)
{
  int occ = gsCount((ATerm) gsMakeUnknown(), (ATerm) expr);
  if (occ > 0) {
    gsErrorMsg("specification contains %d unknown type%s\n", occ, (occ != 1)?"s":"");
    return NULL;
  }
  //implement system sort and data expressions occurring in expr
  ATermList substs     = ATmakeList0();
  t_data_decls old_data_decls = get_data_decls(lpe_spec);
  t_data_decls data_decls = old_data_decls;
  expr = impl_exprs_appl(expr, &substs, &data_decls);
  //perform substitutions on data declarations
  data_decls.sorts     = gsSubstValues_List(substs, data_decls.sorts,     true);
  data_decls.cons_ops  = gsSubstValues_List(substs, data_decls.cons_ops,  true);
  data_decls.ops       = gsSubstValues_List(substs, data_decls.ops,       true);
  data_decls.data_eqns = gsSubstValues_List(substs, data_decls.data_eqns, true);
  //update data declarations in lpe_spec
  set_data_decls(lpe_spec, data_decls);
  //store new declarations in new_decls
  ATermList new_decls = ATmakeList0();
  ATermList sorts = data_decls.sorts;
  while (!ATisEqual(sorts, old_data_decls.sorts)) {
    new_decls = ATinsert(new_decls, ATgetFirst(sorts));
    sorts = ATgetNext(sorts);
  }
  ATermList cons_ops = data_decls.cons_ops;
  while (!ATisEqual(cons_ops, old_data_decls.cons_ops)) {
    new_decls = ATinsert(new_decls, ATgetFirst(cons_ops));
    cons_ops = ATgetNext(cons_ops);
  }
  ATermList ops = data_decls.ops;
  while (!ATisEqual(ops, old_data_decls.ops)) {
    new_decls = ATinsert(new_decls, ATgetFirst(ops));
    ops = ATgetNext(ops);
  }
  ATermList data_eqns = data_decls.data_eqns;
  while (!ATisEqual(data_eqns, old_data_decls.data_eqns)) {
    new_decls = ATinsert(new_decls, ATgetFirst(data_eqns));
    data_eqns = ATgetNext(data_eqns);
  }
  new_decls = ATreverse(new_decls);
  //implement function sorts in expr and the new declarations
  impl_function_sorts((ATerm) ATinsert(new_decls, (ATerm) expr), lpe_spec);
  return expr;
}

ATermAppl impl_sort_refs(ATermAppl spec)
{
  assert(gsIsSpecV1(spec));
  //get sort declarations
  ATermAppl data_spec = ATAgetArgument(spec, 0);
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
  spec = ATsetArgument(spec, (ATerm) data_spec, 0);
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

ATermAppl impl_function_sorts_spec(ATermAppl spec)
{
  assert(gsIsSpecV1(spec));
  //get function sorts occurring in spec
  ATermList func_sorts = get_function_sorts((ATerm) spec);
  //get operation declarations
  ATermAppl data_spec = ATAgetArgument(spec, 0);
  ATermAppl op_spec = ATAgetArgument(data_spec, 2);
  ATermList op_decls = ATLgetArgument(op_spec, 0);
  //initalise data declarations
  t_data_decls data_decls;
  data_decls.sorts     = ATmakeList0();
  data_decls.cons_ops  = ATmakeList0();
  data_decls.ops       = ATmakeList0();
  data_decls.data_eqns = ATmakeList0();
  //implement function sorts that are not already implemented
  while (!ATisEmpty(func_sorts))
  {
    ATermAppl func_sort = ATAgetFirst(func_sorts);
    if (ATindexOf(op_decls, (ATerm) gsMakeOpIdEq(func_sort), 0) == -1) {
      impl_function_sort(func_sort, &data_decls);
    }
    func_sorts = ATgetNext(func_sorts);
  }
  spec = add_data_decls(spec, data_decls);
  return spec;
}

void impl_function_sorts(ATerm term, lpe::specification &lpe_spec)
{
  //get function sorts occurring in term
  ATermList func_sorts = get_function_sorts(term);
  //get data declarations from lpe_spec
  t_data_decls data_decls = get_data_decls(lpe_spec);
  //implement function sorts that are not already implemented
  while (!ATisEmpty(func_sorts))
  {
    ATermAppl func_sort = ATAgetFirst(func_sorts);
    if (ATindexOf(data_decls.ops, (ATerm) gsMakeOpIdEq(func_sort), 0) == -1) {
      impl_function_sort(func_sort, &data_decls);
    }
    func_sorts = ATgetNext(func_sorts);
  }
  //update data declarations in lpe_spec
  set_data_decls(lpe_spec, data_decls);
}

ATermAppl add_data_decls(ATermAppl spec, t_data_decls data_decls)
{
  assert(gsIsSpecV1(spec));
  assert(data_decls_is_initialised(data_decls));
  ATermAppl data_spec = ATAgetArgument(spec, 0);
  //add sort declarations
  ATermAppl sort_spec  = ATAgetArgument(data_spec, 0);
  ATermList sort_decls = ATLgetArgument(sort_spec, 0);
  sort_decls = ATconcat(data_decls.sorts, sort_decls);
  sort_spec = ATsetArgument(sort_spec, (ATerm) sort_decls, 0);  
  data_spec = ATsetArgument(data_spec, (ATerm) sort_spec, 0);
  //add constructor operation declarations
  ATermAppl cons_spec  = ATAgetArgument(data_spec, 1);
  ATermList cons_decls = ATLgetArgument(cons_spec, 0);
  cons_decls = ATconcat(data_decls.cons_ops, cons_decls);
  cons_spec = ATsetArgument(cons_spec, (ATerm) cons_decls, 0);  
  data_spec = ATsetArgument(data_spec, (ATerm) cons_spec, 1);
  //add operation declarations
  ATermAppl map_spec  = ATAgetArgument(data_spec, 2);
  ATermList map_decls = ATLgetArgument(map_spec, 0);
  map_decls = ATconcat(data_decls.ops, map_decls);
  map_spec = ATsetArgument(map_spec, (ATerm) map_decls, 0);  
  data_spec = ATsetArgument(data_spec, (ATerm) map_spec, 2);
  //add data equation declarations
  ATermAppl data_eqn_spec  = ATAgetArgument(data_spec, 3);
  ATermList data_eqn_decls = ATLgetArgument(data_eqn_spec, 0);
  data_eqn_decls = ATconcat(data_decls.data_eqns, data_eqn_decls);
  data_eqn_spec = ATsetArgument(data_eqn_spec, (ATerm) data_eqn_decls, 0);  
  data_spec = ATsetArgument(data_spec, (ATerm) data_eqn_spec, 3);
  //return the new specification
  spec = ATsetArgument(spec, (ATerm) data_spec, 0);
  return spec;
}

static t_data_decls get_data_decls(lpe::specification &lpe_spec)
{
  t_data_decls data_decls;
  data_decls.sorts     = (ATermList) lpe_spec.data().sorts();
  data_decls.cons_ops  = (ATermList) lpe_spec.data().constructors();
  data_decls.ops       = (ATermList) lpe_spec.data().mappings();
  data_decls.data_eqns = (ATermList) lpe_spec.data().equations();
  return data_decls;
}

static void set_data_decls(lpe::specification &lpe_spec, t_data_decls data_decls)
{
  assert(data_decls_is_initialised(data_decls));
  lpe::data_specification data(data_decls.sorts, data_decls.cons_ops, data_decls.ops, data_decls.data_eqns);
  lpe_spec = lpe::set_data_specification(lpe_spec, data);
}

ATermAppl impl_exprs_appl(ATermAppl part, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  bool recursive = true;
  //perform substitutions from *p_substs on part
  part = gsSubstValues_Appl(*p_substs, part, false);
  //replace part by an implementation if the head of part is a special
  //expression
  if (gsIsSortArrowProd(part)) {
    //part is a product arrow sort; replace by arrow sorts
    part = impl_sort_arrow_prod(part);
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
  } else if (gsIsDataApplProd(part)) {
    //part is a product data application; replace by data applications
    ATermList l = ATLgetArgument(part, 1);
    part = ATAgetArgument(part, 0);
    while (!ATisEmpty(l))
    {
      part = gsMakeDataAppl(part, ATAgetFirst(l));
      l = ATgetNext(l);
    }
  } else if (gsIsNumber(part)) {
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
  } else if (gsIsListEnum(part)) {
    //part is a list enumeration; replace by its internal representation
    ATermList elts = ATLgetArgument(part, 0);
    ATermAppl sort = ATAgetArgument(part, 1);
    if (ATgetLength(elts) == 0) {
      //enumeration consists of 0 elements
      gsWarningMsg(
        "%P can not be implemented because it has 0 elements\n", part);
    } else {
      //make cons list
      elts = ATreverse(elts);
      part = gsMakeDataExprEmptyList(sort);
      while (!ATisEmpty(elts))
      {
        part = gsMakeDataExprCons(ATAgetFirst(elts), part);
        elts = ATgetNext(elts);
      }
    }
  } else if (gsIsSetEnum(part) || gsIsBagEnum(part)) {
    //part is a set/bag enumeration; replace by a set/bag comprehension
    ATermList elts = ATLgetArgument(part, 0);
    ATermAppl sort = ATAgetArgument(part, 1);
    if (ATgetLength(elts) == 0) {
      //enumeration consists of 0 elements
      gsWarningMsg(
        "%P can not be implemented because it has 0 elements\n", part);
    } else {
      if (gsIsSetEnum(part)) {
        part = impl_set_enum(elts, sort);
      } else { //gsIsBagEnum(part)
        part = impl_bag_enum(elts, sort);
      }
    }
  } else if (gsIsSetBagComp(part)) {
    //part is a set/bag comprehension; replace by its implementation
    ATermAppl var = ATAgetArgument(part, 0);
    ATermAppl body = ATAgetArgument(part, 1);
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
        part = gsMakeDataExprSetComp(gsMakeLambda(ATmakeList1((ATerm) var),
          body), gsMakeSortExprSet(var_sort));
      } else {
        //part is a bag comprehension
        part = gsMakeDataExprBagComp(gsMakeLambda(ATmakeList1((ATerm) var),
          body), gsMakeSortExprBag(var_sort));
      }
    }
  } else if (gsIsForall(part) || gsIsExists(part)) {
    //part is a quantification; replace by its implementation
    ATermList vars = ATreverse(ATLgetArgument(part, 0));
    bool is_forall = gsIsForall(part);
    part = gsMakeLambda(ATmakeList1(ATgetFirst(vars)), ATAgetArgument(part, 1));
    part = is_forall?gsMakeDataExprForall(part):gsMakeDataExprExists(part);
    vars = ATgetNext(vars);
    while (!ATisEmpty(vars))
    {
      part = gsMakeLambda(ATmakeList1(ATgetFirst(vars)), part);
      part = is_forall?gsMakeDataExprForall(part):gsMakeDataExprExists(part);
      vars = ATgetNext(vars);      
    }
  } else if (gsIsLambda(part)) {
    //part is a lambda abstraction; replace by a named function
    //implement the body, the bound variables and the free variables
    ATermList bound_vars = impl_exprs_list(ATLgetArgument(part, 0),
      p_substs, p_data_decls);
    ATermAppl body = impl_exprs_appl(ATAgetArgument(part, 1),
      p_substs, p_data_decls);
    ATermList FreeVars = impl_exprs_list(get_free_vars(part),
      p_substs, p_data_decls);
    ATermList vars = ATconcat(FreeVars, bound_vars);
    //create sort for the new operation identifier
    ATermAppl op_id_sort = gsGetSort(body);
    ATermList l = ATreverse(vars);
    while (!ATisEmpty(l))
    {
      op_id_sort = gsMakeSortArrow(ATAgetArgument(ATAgetFirst(l), 1), op_id_sort);
      l = ATgetNext(l);
    }
    //create new operation identifier
    ATermAppl op_id = make_fresh_lambda_op_id(op_id_sort, (ATerm) p_data_decls->ops);
    //add operation identifier to the data declarations
    p_data_decls->ops = ATinsert(p_data_decls->ops, (ATerm) op_id);
    //add data equation for the operation to the data declarations
    p_data_decls->data_eqns = ATinsert(p_data_decls->data_eqns, (ATerm)
      gsMakeDataEqn(vars, gsMakeNil(), gsMakeDataApplList(op_id, vars), body));
    //replace part
    ATermAppl new_part = gsMakeDataApplList(op_id, FreeVars);
    *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(part, new_part), *p_substs);
    part = new_part;
    recursive = false;
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
        ATermAppl var_name = ATAgetArgument(whr_decl, 0);
        ATermAppl expr = ATAgetArgument(whr_decl, 1);
        vars = ATinsert(vars,
          (ATerm) gsMakeDataVarId(var_name, gsGetSort(expr)));
        exprs = ATinsert(exprs, (ATerm) expr);
        whr_decls = ATgetNext(whr_decls);
      }
      //replace part
      part = gsMakeDataApplList(gsMakeLambda(vars, body), exprs);
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
  result = gsMakeLambda(ATmakeList1((ATerm) var), result);
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
      gsGetSort(ATAgetArgument(ATAgetFirst(elts),0)));
  //make body for the lambda abstraction
  elts = ATreverse(elts);
  ATermAppl elt = ATAgetFirst(elts);
  result = gsMakeDataExprIf(gsMakeDataExprEq(var, ATAgetArgument(elt, 0)),
    ATAgetArgument(elt, 1), gsMakeDataExprC0());
  elts = ATgetNext(elts);
  while (!ATisEmpty(elts)) {
    elt = ATAgetFirst(elts);
    result = gsMakeDataExprAdd(
      gsMakeDataExprIf(gsMakeDataExprEq(var, ATAgetArgument(elt, 0)),
      ATAgetArgument(elt, 1), gsMakeDataExprC0()), result);
    elts = ATgetNext(elts);
  }
  //make lambda abstraction
  result = gsMakeLambda(ATmakeList1((ATerm) var), result);
  //make bag comprehension
  result = gsMakeDataExprBagComp(result, sort_expr);
  //return result
  return result;
}

ATermList get_free_vars(ATermAppl data_expr)
{
  ATermList result = ATmakeList0();
  get_free_vars_appl(data_expr, ATmakeList0(), &result);
  return ATreverse(result);
}

void get_free_vars_appl(ATermAppl data_expr, ATermList bound_vars,
  ATermList* p_free_vars)
{
  if (gsIsDataVarId(data_expr)) {
    //data_expr is a data variable; add it to *p_free_vars if it does not occur in
    //bound_vars or *p_free_vars
    if ((ATindexOf(bound_vars, (ATerm) data_expr, 0) == -1) &&
        (ATindexOf(*p_free_vars, (ATerm) data_expr, 0) == -1)) {
      *p_free_vars = ATinsert(*p_free_vars, (ATerm) data_expr);
    }
  } else if (gsIsOpId(data_expr) || gsIsNumber(data_expr)) {
    //data_expr is an operation identifier or a number; do nothing
  } else if (gsIsDataAppl(data_expr) || gsIsBagEnumElt(data_expr)) {
    //data_expr is a data application or a bag enumeration element; get free
    //variables from the arguments
    get_free_vars_appl(ATAgetArgument(data_expr, 0), bound_vars, p_free_vars);
    get_free_vars_appl(ATAgetArgument(data_expr, 1), bound_vars, p_free_vars);
  } else if (gsIsDataApplProd(data_expr)) {
    //data_expr is a product data application; get free variables from the
    //arguments
    get_free_vars_appl(ATAgetArgument(data_expr, 0), bound_vars, p_free_vars);
    get_free_vars_list(ATLgetArgument(data_expr, 1), bound_vars, p_free_vars);
  } else if (gsIsListEnum(data_expr) || gsIsSetEnum(data_expr) ||
      gsIsBagEnum(data_expr)) {
    //data_expr is an enumeration; get free variables from the elements
    get_free_vars_list(ATLgetArgument(data_expr, 0), bound_vars, p_free_vars);
  } else if (gsIsSetBagComp(data_expr)) {
    //data_expr is a set of bag comprehension; get free variables from the body
    //where bound_vars is extended with the variable declaration
    ATermAppl var = ATAgetArgument(data_expr, 0);
    if (ATindexOf(bound_vars, (ATerm) var, 0) == -1) {
      bound_vars = ATinsert(bound_vars, (ATerm) var);
    }
    get_free_vars_appl(ATAgetArgument(data_expr, 1), bound_vars, p_free_vars);
  } else if (gsIsLambda(data_expr) || gsIsForall(data_expr) ||
      gsIsExists(data_expr)) {
    //data_expr is a lambda abstraction or a quantification; get free variables
    //from the body where bound_vars is extended with the variable declaration
    ATermList vars = ATLgetArgument(data_expr, 0);
    while (!ATisEmpty(vars)) {
      ATermAppl var = ATAgetFirst(vars);
      if (ATindexOf(bound_vars, (ATerm) var, 0) == -1) {
        bound_vars = ATinsert(bound_vars, (ATerm) var);
      }
      vars = ATgetNext(vars);
    }
    get_free_vars_appl(ATAgetArgument(data_expr, 1), bound_vars, p_free_vars);
  } else if (gsIsWhr(data_expr)) {
    //data_expr is a where clause; get free variables from the rhs's of the
    //where clause declarations and from the body where bound_vars is extended
    //with the lhs's of the where clause declarations
    ATermList whr_decls = ATLgetArgument(data_expr, 1);
    //get free variables from the rhs's of the where clause declarations
    while (!ATisEmpty(whr_decls)) {
      get_free_vars_appl(ATAgetArgument(ATAgetFirst(whr_decls), 1),
        bound_vars, p_free_vars);
      whr_decls = ATgetNext(whr_decls);
    }
    //get free variables from the body
    whr_decls = ATLgetArgument(data_expr, 1);
    while (!ATisEmpty(whr_decls)) {
      ATermAppl whr_decl = ATAgetFirst(whr_decls);
      ATermAppl var = gsMakeDataVarId(ATAgetArgument(whr_decl, 0),
        gsGetSort(ATAgetArgument(whr_decl, 1)));
      if (ATindexOf(bound_vars, (ATerm) var, 0) == -1) {
        bound_vars = ATinsert(bound_vars, (ATerm) var);
      }
      whr_decls = ATgetNext(whr_decls);
    }
    get_free_vars_appl(ATAgetArgument(data_expr, 0), bound_vars, p_free_vars);
  } else {
    gsErrorMsg("%P is not a data expression or a bag enumeration element\n",\
      data_expr);
  }
}

void get_free_vars_list(ATermList data_exprs, ATermList bound_vars,
  ATermList *p_free_vars)
{
  while (!ATisEmpty(data_exprs))
  {
    get_free_vars_appl(ATAgetFirst(data_exprs), bound_vars, p_free_vars);
    data_exprs = ATgetNext(data_exprs);
  }
}
 
ATermList get_function_sorts(ATerm term)
{
  ATermList func_sorts = ATmakeList0();
  if (ATgetType(term) == AT_APPL) {
    get_function_sorts_appl((ATermAppl) term, &func_sorts);
  } else { //ATgetType(term) == AT_LIST
    get_function_sorts_list((ATermList) term, &func_sorts);
  }
  return func_sorts;
}

void get_function_sorts_appl(ATermAppl part, ATermList *p_func_sorts)
{
  if (gsIsSortArrow(part) || gsIsSortArrowProd(part)) {
    if (ATindexOf(*p_func_sorts, (ATerm) part, 0) == -1) {
      *p_func_sorts = ATinsert(*p_func_sorts, (ATerm) part);
    }    
  }
  int nr_args = ATgetArity(ATgetAFun(part));      
  for (int i = 0; i < nr_args; i++) {
    ATerm arg = ATgetArgument(part, i);
    if (ATgetType(arg) == AT_APPL)
      get_function_sorts_appl((ATermAppl) arg, p_func_sorts);
    else //ATgetType(arg) == AT_LIST
      get_function_sorts_list((ATermList) arg, p_func_sorts);
  }
}
 
void get_function_sorts_list(ATermList parts, ATermList *p_func_sorts)
{
  while (!ATisEmpty(parts))
  {
    get_function_sorts_appl(ATAgetFirst(parts), p_func_sorts);
    parts = ATgetNext(parts);
  }
}
 
void impl_function_sort(ATermAppl sort_arrow, t_data_decls *p_data_decls)
{
  assert(gsIsSortArrow(sort_arrow));
  //Declare operations for sort sort_arrow
  p_data_decls->ops = ATconcat(ATmakeList(3,
      (ATerm) gsMakeOpIdEq(sort_arrow),
      (ATerm) gsMakeOpIdNeq(sort_arrow),
      (ATerm) gsMakeOpIdIf(sort_arrow)
    ), p_data_decls->ops);
  //Declare data equations for sort sort_arrow
  ATermAppl f_sort_arrow = gsMakeDataVarId(gsString2ATermAppl("f"), sort_arrow);
  ATermAppl g_sort_arrow = gsMakeDataVarId(gsString2ATermAppl("g"), sort_arrow);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList fl = ATmakeList1((ATerm) f_sort_arrow);
  ATermList fgl = ATmakeList2((ATerm) f_sort_arrow, (ATerm) g_sort_arrow);
  ATermList bfl = ATmakeList2((ATerm) b, (ATerm) f_sort_arrow);
  p_data_decls->data_eqns = ATconcat(ATmakeList(5,
      //equality (sort_arrow -> sort_arrow -> Bool)
      (ATerm) gsMakeDataEqn(fl, nil,
        gsMakeDataExprEq(f_sort_arrow, f_sort_arrow), t),
      //inequality (sort_arrow -> sort_arrow -> Bool)
      (ATerm) gsMakeDataEqn(fgl,nil,
        gsMakeDataExprNeq(f_sort_arrow, g_sort_arrow), 
        gsMakeDataExprNot(gsMakeDataExprEq(f_sort_arrow, g_sort_arrow))),
      //conditional (Bool -> sort_arrow -> sort_arrow -> sort_arrow)
      (ATerm) gsMakeDataEqn(fgl,nil,
        gsMakeDataExprIf(t, f_sort_arrow, g_sort_arrow),
        f_sort_arrow),
      (ATerm) gsMakeDataEqn(fgl,nil,
        gsMakeDataExprIf(f, f_sort_arrow, g_sort_arrow),
        g_sort_arrow),
      (ATerm) gsMakeDataEqn(bfl,nil,
        gsMakeDataExprIf(b, f_sort_arrow, f_sort_arrow),
        f_sort_arrow)
    ), p_data_decls->data_eqns);
}

ATermAppl apply_op_id_to_vars(ATermAppl op_id, ATermList *p_args,
  ATermList *p_vars, ATerm context)
{
  ATermAppl t = op_id;
  ATermAppl sort = ATAgetArgument(t,1);
  *p_args = ATmakeList0();
  ATermList tmpvars = *p_vars; // We only use variables once in a term
  while ( gsIsSortArrow(sort) )
  {
    // Find a variabele with the right sort...
    ATermAppl v = NULL;
    for (ATermList n=tmpvars; !ATisEmpty(n); n=ATgetNext(n))
    {
      if ( ATisEqual(ATgetArgument(ATAgetFirst(n),1),ATgetArgument(sort,0)) )
      {
        v = ATAgetFirst(n);
        tmpvars = ATremoveElement(tmpvars,(ATerm) v);
        break;
      }
    }
    // ...or create a new one
    if ( v == NULL )
    {
      v = gsMakeDataVarId(gsFreshString2ATermAppl("v",
            (ATerm) ATmakeList2((ATerm) (*p_vars),context),false),
                      ATAgetArgument(sort,0));
      // Add it to *p_vars
      *p_vars = ATinsert(*p_vars,(ATerm) v);
    }
    // Apply t to v and add v to *p_args
    t = gsMakeDataAppl(t,v);
    *p_args = ATinsert(*p_args,(ATerm) v);
    // Next
    sort = ATAgetArgument(sort,1);
  }
  *p_args = ATreverse(*p_args);
  *p_vars = ATreverse(*p_vars);
  return t;
}

ATermList merge_list(ATermList l, ATermList m)
{
  for (; !ATisEmpty(m); m=ATgetNext(m))
  {
    if ( ATindexOf(l,ATgetFirst(m),0) == -1 )
    {
      ATinsert(l,ATgetFirst(m));
    }
  }

  return l;
}

ATermList subtract_list(ATermList l, ATermList m)
{
  for (; !ATisEmpty(m); m=ATgetNext(m))
  {
    l = ATremoveAll(l,ATgetFirst(m));
  }

  return l;
}

ATermAppl impl_sort_arrow_prod(ATermAppl sort_arrow_prod)
{
  ATermList l = ATreverse(ATLgetArgument(sort_arrow_prod, 0));
  sort_arrow_prod = ATAgetArgument(sort_arrow_prod, 1);
  while (!ATisEmpty(l))
  {
    sort_arrow_prod = gsMakeSortArrow(ATAgetFirst(l), sort_arrow_prod);
    l = ATgetNext(l);
  }
  return sort_arrow_prod;
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
        proj_ops = ATinsert(proj_ops, (ATerm)
          gsMakeOpId(proj_name, gsMakeSortArrow(sort_id, proj_sort)));
        projs = ATinsert(projs, (ATerm) ATmakeList2(
          ATgetFirst(proj_ops),(ATerm) ATmakeInt(i)));
      }
      struct_projs = ATgetNext(struct_projs);
      i++;
    }
    struct_cons_sorts = ATreverse(struct_cons_sorts);
    //store constructor operation in cons_ops
    cons_ops = ATinsert(cons_ops, (ATerm)
      gsMakeOpId(cons_name, gsMakeSortArrowList(struct_cons_sorts, sort_id)));
    //store recogniser in rec_ops and recs
    if (!gsIsNil(rec_name)) {
      rec_ops = ATinsert(rec_ops, (ATerm)
        gsMakeOpId(rec_name, gsMakeSortArrow(sort_id, gsMakeSortExprBool())));
      recs = ATinsert(recs,
        (ATerm) ATmakeList2(ATgetFirst(rec_ops), ATgetFirst(cons_ops)));
    }
    //add constructor to projs
    ATermList tmpl = ATmakeList0();
    for (; !ATisEmpty(projs); projs=ATgetNext(projs))
    {
      tmpl = ATinsert(tmpl, (ATerm) ATappend(ATLgetFirst(projs),ATgetFirst(cons_ops)));
    }
    projs = ATreverse(tmpl);
    struct_conss = ATgetNext(struct_conss);
  }
  //add declarations for the constructo, projection and recogniser operations
  p_data_decls->cons_ops = ATconcat(ATreverse(cons_ops), p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(ATconcat(ATreverse(proj_ops), ATreverse(rec_ops)),
    p_data_decls->ops);
  //Declare standard structured sorts operations
  p_data_decls->ops = ATconcat(ATmakeList(3,
      (ATerm) gsMakeOpIdEq(sort_id),
      (ATerm) gsMakeOpIdNeq(sort_id),
      (ATerm) gsMakeOpIdIf(sort_id)
    ), p_data_decls->ops);
  //Declare data equations for structured sort
  ATermList op_eqns = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  // XXX more intelligent variable names would be nice
  ATermAppl s1 = gsMakeDataVarId(gsString2ATermAppl("s"), sort_id);
  ATermAppl s2 = gsMakeDataVarId(gsString2ATermAppl("t"), sort_id);
  ATermList ssl = ATmakeList2((ATerm) s1, (ATerm) s2);
  ATermList bssl = ATmakeList3((ATerm) b, (ATerm) s1, (ATerm) s2);
  ATermList vars = bssl;
  ATermList rhsv;
  ATermList lhsv;
  ATermList id_ctx = ATconcat(p_data_decls->sorts,
                      ATconcat(p_data_decls->ops,p_data_decls->cons_ops));
  //store equations for projections in op_eqns
  for (; !ATisEmpty(projs); projs=ATgetNext(projs))
  {
    ATermList l = ATLgetFirst(projs);
    // Name of constructor
    ATermAppl s = ATAgetFirst(ATgetNext(ATgetNext(l)));
    // Number of projected argument
    int i = ATgetInt((ATermInt) ATgetFirst(ATgetNext(l)));
    // Start with the constructor operation
    ATermAppl t = s;
    // Apply constructor t to (fresh) variables and store its
    // arguments in lhsv
    t = apply_op_id_to_vars(t,&lhsv,&vars,(ATerm) id_ctx);
    // Apply projection function to t
    t = gsMakeDataAppl(ATAgetFirst(l),t);
    // Add equation
    op_eqns = ATinsert(op_eqns,
      (ATerm) gsMakeDataEqn(lhsv, nil, t, ATAelementAt(lhsv,i)));
  }
  //store equations for recognition in op_eqns
  for (; !ATisEmpty(recs); recs=ATgetNext(recs))
  {
    ATermList l = ATLgetFirst(recs);
    // Name of constructor
    ATermAppl s = ATAgetFirst(ATgetNext(l));
    ATermAppl t;
    // Add equation for every constructor
    for (ATermList m=cons_ops; !ATisEmpty(m); m=ATgetNext(m))
    {
      t = ATAgetFirst(m);
      // Apply constructor t to (fresh) variables and store its
      // arguments in lhsv
      t = apply_op_id_to_vars(t,&lhsv,&vars,(ATerm) id_ctx);
      // Apply recognition function to t
      t = gsMakeDataAppl(ATAgetFirst(l),t);
      // Add right equation to op_eqns
      if ( ATisEqual(ATAgetFirst(m),s) )
      {
        op_eqns = ATinsert(op_eqns, (ATerm) gsMakeDataEqn(lhsv, nil, t, gsMakeDataExprTrue()));
      } else {
        op_eqns = ATinsert(op_eqns, (ATerm) gsMakeDataEqn(lhsv, nil, t, gsMakeDataExprFalse()));
      }
    }
  }
  //store equations for equalities in op_eqns
  //one equation for every pair of constructors
  op_eqns = ATinsert(op_eqns,
    (ATerm) gsMakeDataEqn(ATmakeList1((ATerm) s1), nil, gsMakeDataExprEq(s1, s1), t));
  for (ATermList l=cons_ops; !ATisEmpty(l); l=ATgetNext(l))
  {
    for (ATermList m=cons_ops; !ATisEmpty(m); m=ATgetNext(m))
    {
      ATermAppl t,u,r;
      ATermList vs,tmpvars;
      // Save vars list
      // Apply constructor in l to (fresh) variables and store its
      // arguments in lhsv
      t = apply_op_id_to_vars(ATAgetFirst(l),&lhsv,&vars,(ATerm) id_ctx);
      // Apply constructor in m to (fresh) variables and store its
      // arguments in rhsv (making sure we don't use the vars that occur in t)
      tmpvars = subtract_list(vars,lhsv);
      u = apply_op_id_to_vars(ATAgetFirst(m),&rhsv,&tmpvars,(ATerm) ATconcat(lhsv,id_ctx));
      // Update vars
      vars = merge_list(vars,rhsv);
      // Combine variable lists of lhs and rhs
      vs = ATconcat(lhsv,rhsv);
      // Create right result
      if ( ATisEqual(ATgetFirst(l),ATgetFirst(m)) )
      {
        // Constructors are the same, so match all variables
        r = NULL;
        for (; !ATisEmpty(lhsv); lhsv=ATgetNext(lhsv),rhsv=ATgetNext(rhsv))
        {
          if ( r == NULL )
          {
            r = gsMakeDataExprEq(ATAgetFirst(lhsv),ATAgetFirst(rhsv));
          } else {
            r = gsMakeDataExprAnd(r,gsMakeDataExprEq(ATAgetFirst(lhsv),ATAgetFirst(rhsv)));
          }
        }
        if ( r == NULL )
        {
          r = gsMakeDataExprTrue();
        }
      } else {
        // Different constructor, so not equal
        r = gsMakeDataExprFalse();
      }
      // Add equation to op_ids
      op_eqns = ATinsert(op_eqns, (ATerm) gsMakeDataEqn(vs,nil,gsMakeDataExprEq(t,u),r));
    }
  }
  //store equation for inequality in op_eqns
  op_eqns = ATinsert(op_eqns, (ATerm) gsMakeDataEqn(ssl, nil,
    gsMakeDataExprNeq(s1,s2), gsMakeDataExprNot(gsMakeDataExprEq(s1,s2))));
  //store equations for 'if' in op_eqns
  op_eqns = ATconcat(ATmakeList(3,
      (ATerm) gsMakeDataEqn(ssl, nil, gsMakeDataExprIf(t,s1,s2),s1),
      (ATerm) gsMakeDataEqn(ssl, nil, gsMakeDataExprIf(f,s1,s2),s2),
      (ATerm) gsMakeDataEqn(bssl, nil, gsMakeDataExprIf(b,s1,s1),s1)
    ), op_eqns);
  //Add op_eqns to data_eqns
  p_data_decls->data_eqns = ATconcat(p_data_decls->data_eqns,op_eqns);

  return sort_id;
}

ATermAppl impl_sort_list(ATermAppl sort_list, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprList(sort_list));
  //implement expressions in the target sort of sort_list
  ATermAppl sort_elt = impl_exprs_appl(ATAgetArgument(sort_list, 1),
    p_substs, p_data_decls);
  //declare fresh sort identifier for sort_list
  ATermAppl sort_id = make_fresh_list_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for this identifier
  ATermAppl subst = gsMakeSubst_Appl(sort_list, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);
  //declare constructors for sort sort_id
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdEmptyList(sort_id),
      (ATerm) gsMakeOpIdCons(sort_elt, sort_id)
    ), p_data_decls->cons_ops);
  //Declare operations for sort sort_id
  p_data_decls->ops = ATconcat(ATmakeList(12,
      (ATerm) gsMakeOpIdEq(sort_id),
      (ATerm) gsMakeOpIdNeq(sort_id),
      (ATerm) gsMakeOpIdIf(sort_id),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_id),
      (ATerm) gsMakeOpIdListSize(sort_id),
      (ATerm) gsMakeOpIdSnoc(sort_id, sort_elt),
      (ATerm) gsMakeOpIdConcat(sort_id),
      (ATerm) gsMakeOpIdEltAt(sort_id, sort_elt),
      (ATerm) gsMakeOpIdHead(sort_id, sort_elt),
      (ATerm) gsMakeOpIdTail(sort_id),
      (ATerm) gsMakeOpIdRHead(sort_id, sort_elt),
      (ATerm) gsMakeOpIdRTail(sort_id)
    ), p_data_decls->ops);
  //Declare data equations for sort sort_id
  ATermList el = ATmakeList0();
  ATermAppl el_sort_id = gsMakeDataExprEmptyList(sort_id);
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_id);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_id);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl e_sort_elt = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl ds = gsMakeDataExprCons(d_sort_elt, s_sort_id);
  ATermAppl es = gsMakeDataExprCons(e_sort_elt, s_sort_id);
  ATermAppl et = gsMakeDataExprCons(e_sort_elt, t_sort_id);
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList dl = ATmakeList1((ATerm) d_sort_elt);
  ATermList sl = ATmakeList1((ATerm) s_sort_id);
  ATermList stl = ATmakeList2((ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList dsl = ATmakeList2((ATerm) d_sort_elt, (ATerm) s_sort_id);
  ATermList desl = ATmakeList3((ATerm) d_sort_elt, (ATerm) e_sort_elt,
    (ATerm) s_sort_id);
  ATermList dstl = ATmakeList3((ATerm) d_sort_elt, (ATerm) s_sort_id,
    (ATerm) t_sort_id);
  ATermList destl = ATmakeList4((ATerm) d_sort_elt, (ATerm) e_sort_elt,
    (ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList dspl = ATmakeList3((ATerm) d_sort_elt, (ATerm) s_sort_id, (ATerm) p);
  ATermList bsl = ATmakeList2((ATerm) b, (ATerm) s_sort_id);
  p_data_decls->data_eqns = ATconcat(ATmakeList(26,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(el_sort_id, el_sort_id), t),
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(el_sort_id, ds), f),
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(ds, el_sort_id), f),
      (ATerm) gsMakeDataEqn(destl, nil,
        gsMakeDataExprEq(ds, et),
        gsMakeDataExprAnd(
          gsMakeDataExprEq(d_sort_elt, e_sort_elt),
          gsMakeDataExprEq(s_sort_id, t_sort_id))),
      (ATerm) gsMakeDataEqn(sl, nil, gsMakeDataExprEq(s_sort_id, s_sort_id), t),
      //inequality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprNeq(s_sort_id, t_sort_id), 
        gsMakeDataExprNot(gsMakeDataExprEq(s_sort_id, t_sort_id))),
      //conditional (Bool -> sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(t, s_sort_id, t_sort_id),
        s_sort_id),
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(f, s_sort_id, t_sort_id),
        t_sort_id),
      (ATerm) gsMakeDataEqn(bsl,nil,
        gsMakeDataExprIf(b, s_sort_id, s_sort_id),
        s_sort_id),
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
          gsMakeDataExprRTail(gsMakeDataExprCons(e_sort_elt, s_sort_id))))
    ), p_data_decls->data_eqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;
}

ATermAppl impl_sort_set(ATermAppl sort_set, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprSet(sort_set));
  //implement expressions in the target sort of sort_set
  ATermAppl sort_elt = impl_exprs_appl(ATAgetArgument(sort_set, 1),
    p_substs, p_data_decls);
  //declare fresh sort identifier for sort_set
  ATermAppl sort_id = make_fresh_set_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for this identifier
  ATermAppl subst = gsMakeSubst_Appl(sort_set, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);
  //declare operations for sort sort_id
  p_data_decls->ops = ATconcat(ATmakeList(12,
      (ATerm) gsMakeOpIdEq(sort_id),
      (ATerm) gsMakeOpIdNeq(sort_id),
      (ATerm) gsMakeOpIdIf(sort_id),
      (ATerm) gsMakeOpIdSetComp(sort_elt, sort_id),
      (ATerm) gsMakeOpIdEmptySet(sort_id),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_id),
      (ATerm) gsMakeOpIdSubSetEq(sort_id),
      (ATerm) gsMakeOpIdSubSet(sort_id),
      (ATerm) gsMakeOpIdSetUnion(sort_id),
      (ATerm) gsMakeOpIdSetDiff(sort_id),
      (ATerm) gsMakeOpIdSetIntersect(sort_id),
      (ATerm) gsMakeOpIdSetCompl(sort_id)
    ), p_data_decls->ops);
  //declare data equations for sort sort_id
  ATermAppl sort_func = gsMakeSortArrow(sort_elt, gsMakeSortExprBool());
  ATermList el = ATmakeList0();
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_id);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_id);
  ATermAppl f_sort_func = gsMakeDataVarId(gsString2ATermAppl("f"), sort_func);
  ATermAppl g_sort_func = gsMakeDataVarId(gsString2ATermAppl("g"), sort_func);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl x_sort_elt = gsMakeDataVarId(gsString2ATermAppl("x"), sort_elt);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList stl = ATmakeList2((ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList bsl = ATmakeList2((ATerm) b, (ATerm) s_sort_id);
  ATermList fl = ATmakeList1((ATerm) f_sort_func);
  ATermList dfl = ATmakeList2((ATerm) d_sort_elt, (ATerm) f_sort_func);
  ATermList fgl = ATmakeList2((ATerm) f_sort_func, (ATerm) g_sort_func);
  ATermAppl false_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt), f), p_substs, p_data_decls);
  ATermAppl imp_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprImp(
        gsMakeDataAppl(f_sort_func, x_sort_elt),
        gsMakeDataAppl(g_sort_func, x_sort_elt)
      )
    ), p_substs, p_data_decls);
  ATermAppl OrFunc = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprOr(
        gsMakeDataAppl(f_sort_func, x_sort_elt),
        gsMakeDataAppl(g_sort_func, x_sort_elt)
      )
    ), p_substs, p_data_decls);
  ATermAppl and_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprAnd(
        gsMakeDataAppl(f_sort_func, x_sort_elt),
        gsMakeDataAppl(g_sort_func, x_sort_elt)
      )
    ), p_substs, p_data_decls);
  ATermAppl not_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprNot(gsMakeDataAppl(f_sort_func, x_sort_elt))
    ), p_substs, p_data_decls);
  p_data_decls->data_eqns = ATconcat(ATmakeList(13,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprSetComp(f_sort_func, sort_id),
          gsMakeDataExprSetComp(g_sort_func, sort_id)), 
        gsMakeDataExprEq(f_sort_func, g_sort_func)),
      //inequality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprNeq(s_sort_id, t_sort_id), 
        gsMakeDataExprNot(gsMakeDataExprEq(s_sort_id, t_sort_id))),
      //conditional (Bool -> sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(t, s_sort_id, t_sort_id),
        s_sort_id),
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(f, s_sort_id, t_sort_id),
        t_sort_id),
      (ATerm) gsMakeDataEqn(bsl,nil,
        gsMakeDataExprIf(b, s_sort_id, s_sort_id),
        s_sort_id),
      //empty set (sort_id)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptySet(sort_id),
        gsMakeDataExprSetComp(false_func, sort_id)),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprEltIn(d_sort_elt, gsMakeDataExprSetComp(f_sort_func, sort_id)),
        gsMakeDataAppl(f_sort_func, d_sort_elt)),
      //subset or equal (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubSetEq(
          gsMakeDataExprSetComp(f_sort_func, sort_id),
          gsMakeDataExprSetComp(g_sort_func, sort_id)), 
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
          gsMakeDataExprSetComp(f_sort_func, sort_id),
          gsMakeDataExprSetComp(g_sort_func, sort_id)), 
        gsMakeDataExprSetComp(OrFunc, sort_id)),
      //difference (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSetDiff(s_sort_id, t_sort_id),
        gsMakeDataExprSetInterSect(s_sort_id, gsMakeDataExprSetCompl(t_sort_id))),
      //intersection (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetInterSect(
          gsMakeDataExprSetComp(f_sort_func, sort_id),
          gsMakeDataExprSetComp(g_sort_func, sort_id)), 
        gsMakeDataExprSetComp(and_func, sort_id)),
      //complement (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fl, nil,
        gsMakeDataExprSetCompl(gsMakeDataExprSetComp(f_sort_func, sort_id)),
        gsMakeDataExprSetComp(not_func, sort_id))
      //simplification of combinations of functions false, not, imp, and, or
      ////left unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), false_func, f_sort_func),
      //  f_sort_func),
      ////right unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), f_sort_func, false_func),
      //  f_sort_func)
    ), p_data_decls->data_eqns);
  return sort_id;
}

ATermAppl impl_sort_bag(ATermAppl sort_bag, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprBag(sort_bag));
  //implement expressions in the target sort of sort_bag
  ATermAppl sort_elt = impl_exprs_appl(ATAgetArgument(sort_bag, 1),
    p_substs, p_data_decls);
  //add implementation of sort Set(sort_elt), if necessary
  // XXX This piece of code should somehow be moved to the end of the function
  ATermAppl sort_set = gsMakeSortExprSet(sort_elt);
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
  p_data_decls->ops = ATconcat(ATmakeList(14,
      (ATerm) gsMakeOpIdEq(sort_id),
      (ATerm) gsMakeOpIdNeq(sort_id),
      (ATerm) gsMakeOpIdIf(sort_id),
      (ATerm) gsMakeOpIdBagComp(sort_elt, sort_id),
      (ATerm) gsMakeOpIdEmptyBag(sort_id),
      (ATerm) gsMakeOpIdCount(sort_elt, sort_id),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_id),
      (ATerm) gsMakeOpIdSubBagEq(sort_id),
      (ATerm) gsMakeOpIdSubBag(sort_id),
      (ATerm) gsMakeOpIdBagUnion(sort_id),
      (ATerm) gsMakeOpIdBagDiff(sort_id),
      (ATerm) gsMakeOpIdBagIntersect(sort_id),
      (ATerm) gsMakeOpIdBag2Set(sort_id, sort_set_impl),
      (ATerm) gsMakeOpIdSet2Bag(sort_set_impl, sort_id)
    ), p_data_decls->ops);
  //declare data equations for sort sort_id
  ATermAppl sort_func = gsMakeSortArrow(sort_elt, gsMakeSortExprNat());
  ATermList el = ATmakeList0();
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_id);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_id);
  ATermAppl f_sort_func = gsMakeDataVarId(gsString2ATermAppl("f"), sort_func);
  ATermAppl g_sort_func = gsMakeDataVarId(gsString2ATermAppl("g"), sort_func);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl x_sort_elt = gsMakeDataVarId(gsString2ATermAppl("x"), sort_elt);
  ATermAppl u_sort_set_impl = gsMakeDataVarId(gsString2ATermAppl("u"), sort_set_impl);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl m_name = gsString2ATermAppl("m");
  ATermAppl n_name = gsString2ATermAppl("n");
  ATermAppl m = gsMakeDataVarId(m_name, gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(n_name, gsMakeSortExprNat());
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList sl = ATmakeList1((ATerm) s_sort_id);
  ATermList stl = ATmakeList2((ATerm) s_sort_id, (ATerm) t_sort_id);
  ATermList dsl = ATmakeList2((ATerm) d_sort_elt, (ATerm) s_sort_id);
  ATermList ul = ATmakeList1((ATerm) u_sort_set_impl);
  ATermList bsl = ATmakeList2((ATerm) b, (ATerm) s_sort_id);
  ATermList dfl = ATmakeList2((ATerm) d_sort_elt, (ATerm) f_sort_func);
  ATermList fgl = ATmakeList2((ATerm) f_sort_func, (ATerm) g_sort_func);
  ATermAppl zero_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt), zero), p_substs, p_data_decls);
  ATermAppl lte_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprLTE(
        gsMakeDataAppl(f_sort_func, x_sort_elt),
        gsMakeDataAppl(g_sort_func, x_sort_elt)
      )
    ), p_substs, p_data_decls);
  ATermAppl add_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprAdd(
        gsMakeDataAppl(f_sort_func, x_sort_elt),
        gsMakeDataAppl(g_sort_func, x_sort_elt)
      )
    ), p_substs, p_data_decls);
  ATermAppl subt_max0_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeWhr(
        gsMakeDataExprIf(
          gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
        ), ATmakeList2(
          (ATerm) gsMakeWhrDecl(m_name, gsMakeDataAppl(f_sort_func, x_sort_elt)),
          (ATerm) gsMakeWhrDecl(n_name, gsMakeDataAppl(g_sort_func, x_sort_elt))
        )
      )
    ), p_substs, p_data_decls);
  ATermAppl min_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprMin(
        gsMakeDataAppl(f_sort_func, x_sort_elt),
        gsMakeDataAppl(g_sort_func, x_sort_elt)
      )
    ), p_substs, p_data_decls);
  ATermAppl bag2set_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprEltIn(x_sort_elt, s_sort_id)
    ), p_substs, p_data_decls);
  ATermAppl set2bag_func = impl_exprs_appl(
    gsMakeLambda(ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprIf(
        gsMakeDataExprEltIn(x_sort_elt, u_sort_set_impl),
        gsMakeDataExprNat_int(1),
        gsMakeDataExprNat_int(0)
      )
    ), p_substs, p_data_decls);
  p_data_decls->data_eqns = ATconcat(ATmakeList(15,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprBagComp(f_sort_func, sort_id),
          gsMakeDataExprBagComp(g_sort_func, sort_id)), 
        gsMakeDataExprEq(f_sort_func, g_sort_func)),
      //inequality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprNeq(s_sort_id, t_sort_id), 
        gsMakeDataExprNot(gsMakeDataExprEq(s_sort_id, t_sort_id))),
      //conditional (Bool -> sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(t, s_sort_id, t_sort_id),
        s_sort_id),
      (ATerm) gsMakeDataEqn(stl,nil,
        gsMakeDataExprIf(f, s_sort_id, t_sort_id),
        t_sort_id),
      (ATerm) gsMakeDataEqn(bsl,nil,
        gsMakeDataExprIf(b, s_sort_id, s_sort_id),
        s_sort_id),
      //empty bag (sort_id)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptyBag(sort_id),
        gsMakeDataExprBagComp(zero_func, sort_id)),
      //count (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprCount(d_sort_elt, gsMakeDataExprBagComp(f_sort_func, sort_id)),
        gsMakeDataAppl(f_sort_func, d_sort_elt)),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprEltIn(d_sort_elt, s_sort_id),
        gsMakeDataExprGT(gsMakeDataExprCount(d_sort_elt, s_sort_id), zero)),
      //subbag or equal (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubBagEq(
          gsMakeDataExprBagComp(f_sort_func, sort_id),
          gsMakeDataExprBagComp(g_sort_func, sort_id)), 
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
          gsMakeDataExprBagComp(f_sort_func, sort_id),
          gsMakeDataExprBagComp(g_sort_func, sort_id)), 
        gsMakeDataExprBagComp(add_func, sort_id)),
      //difference (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagDiff(
          gsMakeDataExprBagComp(f_sort_func, sort_id),
          gsMakeDataExprBagComp(g_sort_func, sort_id)), 
        gsMakeDataExprBagComp(subt_max0_func, sort_id)),
      //intersection (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagInterSect(
          gsMakeDataExprBagComp(f_sort_func, sort_id),
          gsMakeDataExprBagComp(g_sort_func, sort_id)), 
        gsMakeDataExprBagComp(min_func, sort_id)),
      //Bag2Set (sort_id -> sort_set_impl)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprBag2Set(s_sort_id, sort_set_impl),
        gsMakeDataExprSetComp(bag2set_func, sort_set_impl)),
      //Set2Bag (sort_set_impl -> sort_id)
      (ATerm) gsMakeDataEqn(ul, nil,
        gsMakeDataExprSet2Bag(u_sort_set_impl, sort_id),
        gsMakeDataExprBagComp(set2bag_func, sort_id))
    ), p_data_decls->data_eqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;
}

ATermAppl make_fresh_struct_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(struct_prefix, term, false));
}
 
ATermAppl make_fresh_list_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(list_prefix, term, false));
}

ATermAppl make_fresh_set_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(set_prefix, term, false));
}

ATermAppl make_fresh_bag_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(bag_prefix, term, false));
}

ATermAppl make_fresh_lambda_op_id(ATermAppl sort_expr, ATerm term)
{
  return gsMakeOpId(gsFreshString2ATermAppl(lambda_prefix, term, false),
    sort_expr);
}

bool is_struct_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      struct_prefix,
      ATgetName(ATgetAFun(ATAgetArgument(sort_expr, 0))),
      strlen(struct_prefix)) == 0;
  } else {
    return false;
  }
}

bool is_list_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      list_prefix,
      ATgetName(ATgetAFun(ATAgetArgument(sort_expr, 0))),
      strlen(list_prefix)) == 0;
  } else {
    return false;
  }
}

bool is_set_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      set_prefix,
      ATgetName(ATgetAFun(ATAgetArgument(sort_expr, 0))),
      strlen(set_prefix)) == 0;
  } else {
    return false;
  }
}

bool is_bag_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      bag_prefix,
      ATgetName(ATgetAFun(ATAgetArgument(sort_expr, 0))),
      strlen(bag_prefix)) == 0;
  } else {
    return false;
  }
}

//bool is_lambda_op_id(ATermAppl data_expr)
//{
//  if (gsIsOpId(data_expr)) {
//    return strncmp(
//      lambda_prefix,
//      ATgetName(ATgetAFun(ATAgetArgument(data_expr, 0))),
//      strlen(lambda_prefix)) == 0;
//  } else {
//    return false;
//  }
//}

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

void impl_sort_bool(t_data_decls *p_data_decls)
{
  //Declare sort Bool
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdBool());
  //Declare constructors for sort Bool
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdTrue(),
      (ATerm) gsMakeOpIdFalse()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Bool
  ATermAppl se_bool = gsMakeSortExprBool();
  p_data_decls->ops = ATconcat(ATmakeList(7,
      (ATerm) gsMakeOpIdNot(),
      (ATerm) gsMakeOpIdAnd(),
      (ATerm) gsMakeOpIdOr(),
      (ATerm) gsMakeOpIdImp(),
      (ATerm) gsMakeOpIdEq(se_bool),
      (ATerm) gsMakeOpIdNeq(se_bool),
      (ATerm) gsMakeOpIdIf(se_bool)
    ), p_data_decls->ops);
  //Declare data equations for sort Bool
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), se_bool);
  ATermList bl = ATmakeList1((ATerm) b);
  ATermList bcl = ATmakeList2((ATerm) b, (ATerm) c);
  p_data_decls->data_eqns = ATconcat(ATmakeList(24,
      //logical negation (Bool -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(f), t),
      (ATerm) gsMakeDataEqn(bl, nil,
                     gsMakeDataExprNot(gsMakeDataExprNot(b)), b),
      //conjunction (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, t), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, f), f),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(f, b), f),
      //disjunction (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, f), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(t, b), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(f, b), b),
      //implication (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, f),
                                            gsMakeDataExprNot(b)),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(f, b), t),
      //equality (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, t), t),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(t, f), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(f, f), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(b, b), t),
      //inequality (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprNeq(b, c), 
                      gsMakeDataExprNot(gsMakeDataExprEq(b, c))),
      //conditional (Bool -> Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprIf(t, b, c), b),
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprIf(f, b, c), c),
      (ATerm) gsMakeDataEqn(bcl,nil, gsMakeDataExprIf(b, c, c), c)
    ), p_data_decls->data_eqns);
}

void impl_sort_pos(t_data_decls *p_data_decls)
{
  //Declare sort Pos
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdPos());
  //Declare constructors for sort Pos
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdC1(),
      (ATerm) gsMakeOpIdCDub()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Pos
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_bool = gsMakeSortExprBool();
  p_data_decls->ops = ATconcat(ATmakeList(16,
      (ATerm) gsMakeOpIdEq(se_pos),
      (ATerm) gsMakeOpIdNeq(se_pos),
      (ATerm) gsMakeOpIdIf(se_pos),
      (ATerm) gsMakeOpIdLTE(se_pos),
      (ATerm) gsMakeOpIdLT(se_pos),
      (ATerm) gsMakeOpIdGTE(se_pos),
      (ATerm) gsMakeOpIdGT(se_pos),
      (ATerm) gsMakeOpIdMax(se_pos, se_pos),
      (ATerm) gsMakeOpIdMin(se_pos),
      (ATerm) gsMakeOpIdAbs(se_pos),
      (ATerm) gsMakeOpIdSucc(se_pos),
      (ATerm) gsMakeOpIdDub(se_pos),
      (ATerm) gsMakeOpIdAdd(se_pos, se_pos),
      (ATerm) gsMakeOpIdAddC(),
      (ATerm) gsMakeOpIdMult(se_pos),
      (ATerm) gsMakeOpIdMultIR()
    ), p_data_decls->ops);
  //Declare data equations for sort Pos
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), se_pos);
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), se_pos);
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), se_pos);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), se_bool);
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pqrl = ATmakeList3((ATerm) p, (ATerm) q, (ATerm) r);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) b, (ATerm) c, (ATerm) p, (ATerm) q);
  ATermList bpqrl = ATmakeList4((ATerm) b, (ATerm) p, (ATerm) q, (ATerm) r);
  p_data_decls->data_eqns = ATconcat(ATmakeList(45,
      //equality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(one, one), t),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprCDub(b, p)), f),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), one), f),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         f),
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprEq(p, p), t),
      //inequality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprNeq(p, q), gsMakeDataExprNot(gsMakeDataExprEq(p, q))),
      //conditional (Bool -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprIf(t, p, q), p),
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprIf(f, p, q), q),
      (ATerm) gsMakeDataEqn(bpl,nil, gsMakeDataExprIf(b, p, p), p),
      //less than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLTE(one, p), t),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), one), f),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprLT(p, q)),
      //less than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLT(p, one), f),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprCDub(b, p)), t),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprLT(p, q)),
      //greater than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGTE(p, q), gsMakeDataExprLTE(q, p)),
      //greater than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGT(p, q), gsMakeDataExprLT(q, p)),
      //maximum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMax(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), q, p)),
      //minimum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMin(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), p, q)),
      //absolute value (Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprAbs(p), p),
      //successor (Pos -> Pos)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprSucc(one), gsMakeDataExprCDub(f, one)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCDub(f, p)),
         gsMakeDataExprCDub(t, p)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCDub(t, p)),
         gsMakeDataExprCDub(f, gsMakeDataExprSucc(p))),
      //double (Pos -> Pos)
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDub(b, p), gsMakeDataExprCDub(b, p)),
      //addition (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdAdd(se_pos, se_pos),
         gsMakeDataAppl(gsMakeOpIdAddC(), gsMakeOpIdFalse())),
      //addition with carry (Bool -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(f, one, p), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(t, one, p),
         gsMakeDataExprSucc(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(f, p, one), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(t, p, one),
         gsMakeDataExprSucc(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(bcpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(c, p), gsMakeDataExprCDub(c, q)),
         gsMakeDataExprCDub(b, gsMakeDataExprAddC(c, p, q))),
      (ATerm) gsMakeDataEqn(bpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprCDub(gsMakeDataExprNot(b), gsMakeDataExprAddC(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprCDub(gsMakeDataExprNot(b), gsMakeDataExprAddC(b, p, q))),
      //multiplication (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprMult(p, q),
         gsMakeDataExprMultIR(f, one, p, q)),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprGT(p, q),
         gsMakeDataExprMult(p, q),
         gsMakeDataExprMultIR(f, one, q, p)),
      //multiplication with intermediate result
      //  (Bool -> Pos -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMultIR(f, p, one, q), q),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMultIR(t, p, one, q), gsMakeDataExprAddC(f, p, q)),
      (ATerm) gsMakeDataEqn(bpqrl,nil, 
         gsMakeDataExprMultIR(b, p, gsMakeDataExprCDub(f, q), r),
         gsMakeDataExprMultIR(b, p, q, gsMakeDataExprCDub(f, r))),
      (ATerm) gsMakeDataEqn(pqrl,nil, 
         gsMakeDataExprMultIR(f, p, gsMakeDataExprCDub(t, q), r),
         gsMakeDataExprMultIR(t, r, q, gsMakeDataExprCDub(f, r))),
      (ATerm) gsMakeDataEqn(pqrl,nil, 
         gsMakeDataExprMultIR(t, p, gsMakeDataExprCDub(t, q), r),
         gsMakeDataExprMultIR(t, gsMakeDataExprAddC(f, p, r), q,
           gsMakeDataExprCDub(f, r)))
    ), p_data_decls->data_eqns);
}

void impl_sort_nat(t_data_decls *p_data_decls)
{
  //add implementation of sort NatPair, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNatPair(), 0) == -1) {
    impl_sort_nat_pair(p_data_decls);
  }
  //Declare sort Nat
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdNat());
  //Declare constructors for sort Nat
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdC0(),
      (ATerm) gsMakeOpIdCNat()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Nat
  ATermAppl se_nat = gsMakeSortExprNat();
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_bool = gsMakeSortExprBool();
  p_data_decls->ops = ATconcat(ATmakeList(31,
      (ATerm) gsMakeOpIdEq(se_nat),
      (ATerm) gsMakeOpIdNeq(se_nat),
      (ATerm) gsMakeOpIdIf(se_nat),
      (ATerm) gsMakeOpIdPos2Nat(),
      (ATerm) gsMakeOpIdNat2Pos(),
      (ATerm) gsMakeOpIdLTE(se_nat),
      (ATerm) gsMakeOpIdLT(se_nat),
      (ATerm) gsMakeOpIdGTE(se_nat),
      (ATerm) gsMakeOpIdGT(se_nat),
      (ATerm) gsMakeOpIdMax(se_pos, se_nat),
      (ATerm) gsMakeOpIdMax(se_nat, se_pos),
      (ATerm) gsMakeOpIdMax(se_nat, se_nat),
      (ATerm) gsMakeOpIdMin(se_nat),
      (ATerm) gsMakeOpIdAbs(se_nat),
      (ATerm) gsMakeOpIdSucc(se_nat),
      (ATerm) gsMakeOpIdPred(se_pos),
      (ATerm) gsMakeOpIdDub(se_nat),
      (ATerm) gsMakeOpIdAdd(se_pos, se_nat),
      (ATerm) gsMakeOpIdAdd(se_nat, se_pos),
      (ATerm) gsMakeOpIdAdd(se_nat, se_nat),
      (ATerm) gsMakeOpIdGTESubt(se_pos),
      (ATerm) gsMakeOpIdGTESubt(se_nat),
      (ATerm) gsMakeOpIdGTESubtB(),
      (ATerm) gsMakeOpIdMult(se_nat),
      (ATerm) gsMakeOpIdDiv(se_pos),
      (ATerm) gsMakeOpIdDiv(se_nat),
      (ATerm) gsMakeOpIdMod(se_pos),
      (ATerm) gsMakeOpIdMod(se_nat),
      (ATerm) gsMakeOpIdExp(se_pos),
      (ATerm) gsMakeOpIdExp(se_nat),
      (ATerm) gsMakeOpIdEven()
    ), p_data_decls->ops);
  //Declare data equations for sort Nat
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), se_bool);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), se_pos);
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), se_pos);
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), se_nat);
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), se_nat);
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) b, (ATerm) c, (ATerm) p, (ATerm) q);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  p_data_decls->data_eqns = ATconcat(ATmakeList(68,
      //equality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEq(zero, zero), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(zero, gsMakeDataExprCNat(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprEq(n, n), t),
      //inequality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprNeq(m, n), gsMakeDataExprNot(gsMakeDataExprEq(m, n))),
      //conditional (Bool -> Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprIf(t, m, n), m),
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprIf(f, m, n), n),
      (ATerm) gsMakeDataEqn(bnl,nil, gsMakeDataExprIf(b, n, n), n),
      //convert Pos to Nat (Pos -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeOpIdPos2Nat(), gsMakeOpIdCNat()),
      //convert Nat to Pos (Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNat2Pos(gsMakeDataExprCNat(p)), p),
      //less than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLTE(zero, n), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLTE(p, q)),
      //less than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLT(n, zero), f),
      (ATerm) gsMakeDataEqn(pl, nil, 
         gsMakeDataExprLT(zero, gsMakeDataExprCNat(p)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLT(p, q)),
      //greater than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGTE(m, n), gsMakeDataExprLTE(n, m)),
      //greater than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGT(m, n), gsMakeDataExprLT(n, m)),
      //maximum (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprMax(p, zero), p),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCNat(q)),
         gsMakeDataExprMax(p, q)),
      //maximum (Nat -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprMax(zero, p), p),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(gsMakeDataExprCNat(p), q),
         gsMakeDataExprMax(p, q)),
      //maximum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMax(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), n, m)),
      //minimum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMin(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), m, n)),
      //absolute value (Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprAbs(n), n),
      //successor (Nat -> Pos)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprSucc(zero), one),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNat(p)), gsMakeDataExprSucc(p)),
      //predecessor (Pos -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprPred(one), zero),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCDub(t, p)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCDub(f, p)),
         gsMakeDataExprDub(t, gsMakeDataExprPred(p))),
      //double (Bool -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprDub(f, zero), zero),
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprDub(t, zero), gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDub(b, gsMakeDataExprCNat(p)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))),
      //addition (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprAdd(p, zero), p),
      (ATerm) gsMakeDataEqn(pql, nil,
        gsMakeDataExprAdd(p, gsMakeDataExprCNat(q)),
        gsMakeDataExprAdd(p, q)),
      //addition (Nat -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprAdd(zero, p), p),
      (ATerm) gsMakeDataEqn(pql, nil,
        gsMakeDataExprAdd(gsMakeDataExprCNat(p), q),
        gsMakeDataExprAdd(p, q)),
      //addition (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(zero, n), n),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(n, zero), n),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprAdd(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprAddC(f, p, q))),
      //GTE subtraction (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprGTESubt(p, q),
         gsMakeDataExprGTESubtB(f, p, q)),
      //GTE subtraction (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprGTESubt(n, zero), n),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprGTESubt(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprGTESubt(p, q)),
      //GTE subtraction with borrow (Bool -> Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprGTESubtB(f, p, one),
         gsMakeDataExprPred(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprGTESubtB(t, p, one),
         gsMakeDataExprPred(gsMakeDataExprNat2Pos(gsMakeDataExprPred(p)))),
      (ATerm) gsMakeDataEqn(bcpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(c, p),
           gsMakeDataExprCDub(c, q)),
         gsMakeDataExprDub(b, gsMakeDataExprGTESubtB(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(f, p),
           gsMakeDataExprCDub(t, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b),
           gsMakeDataExprGTESubtB(t, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(t, p),
           gsMakeDataExprCDub(f, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b),
           gsMakeDataExprGTESubtB(f, p, q))),
      //multiplication (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(zero, n), zero),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(n, zero), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMult(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprMult(p, q))),
      //exponentiation (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprExp(p, zero), one),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(p, gsMakeDataExprCNat(one)), p),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprCDub(f, q))),
         gsMakeDataExprExp(gsMakeDataExprMultIR(f, one, p, p),
           gsMakeDataExprCNat(q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprCDub(t, q))),
         gsMakeDataExprMultIR(f, one, p,
           gsMakeDataExprExp(gsMakeDataExprMultIR(f, one, p, p),
             gsMakeDataExprCNat(q)))),
      //exponentiation (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprExp(n, zero),
        gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(zero, gsMakeDataExprCNat(p)), zero),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprExp(gsMakeDataExprCNat(p), n),
         gsMakeDataExprCNat(gsMakeDataExprExp(p, n))),
      //even (Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEven(zero), t),
      (ATerm) gsMakeDataEqn(pl, nil,
        gsMakeDataExprEven(gsMakeDataExprCNat(one)), f),
      (ATerm) gsMakeDataEqn(bpl, nil,
        gsMakeDataExprEven(gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))),
        gsMakeDataExprNot(b)),
      //quotient after division (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql, nil,         
         gsMakeDataExprDiv(p, q),
         gsMakeDataExprFirst(gsMakeDataExprDivMod(p, q))),
      //old implementation of div
      //(ATerm) gsMakeDataEqn(pql,
      //   gsMakeDataExprGTE(p, q),
      //   gsMakeDataExprDiv(p, q),
      //   gsMakeDataExprCNat(gsMakeDataExprSucc(
      //     gsMakeDataExprDiv(gsMakeDataExprGTESubt(p, q), q)))),
      //(ATerm) gsMakeDataEqn(pql,
      //   gsMakeDataExprLT(p, q),
      //   gsMakeDataExprDiv(p, q),
      //   gsMakeDataExprC0()),
      //quotient after division (Nat -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprDiv(zero, p), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprDiv(gsMakeDataExprCNat(p), q),
         gsMakeDataExprDiv(p, q)),
      //remainder after division (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql, nil,         
         gsMakeDataExprMod(p, q),
         gsMakeDataExprLast(gsMakeDataExprDivMod(p, q))),
      //old implementation of mod
      //(ATerm) gsMakeDataEqn(pql,
      //   gsMakeDataExprGTE(p, q),
      //   gsMakeDataExprMod(p, q),
      //   gsMakeDataExprMod(gsMakeDataExprGTESubt(p, q), q)),
      //(ATerm) gsMakeDataEqn(pql,
      //   gsMakeDataExprLT(p, q),
      //   gsMakeDataExprMod(p, q),
      //   gsMakeDataExprCNat(p)),
      //remainder after division (Nat -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprMod(zero, p), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMod(gsMakeDataExprCNat(p), q),
         gsMakeDataExprMod(p, q))
    ), p_data_decls->data_eqns);
}

void impl_sort_nat_pair(t_data_decls *p_data_decls)
{
  //Declare sort NatPair
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdNatPair());
  //Declare constructors for sort NatPair
  p_data_decls->cons_ops =
    ATconcat(ATmakeList1((ATerm) gsMakeOpIdCPair()), p_data_decls->cons_ops);
  //Declare operations for sort NatPair
  ATermAppl se_bool = gsMakeSortExprBool();
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_nat = gsMakeSortExprNat();
  ATermAppl se_nat_pair = gsMakeSortExprNatPair();
  p_data_decls->ops = ATconcat(ATmakeList(8,
      (ATerm) gsMakeOpIdEq(se_nat_pair),
      (ATerm) gsMakeOpIdNeq(se_nat_pair),
      (ATerm) gsMakeOpIdIf(se_nat_pair),
      (ATerm) gsMakeOpIdFirst(),
      (ATerm) gsMakeOpIdLast(),
      (ATerm) gsMakeOpIdDivMod(),
      (ATerm) gsMakeOpIdGDivMod(),
      (ATerm) gsMakeOpIdGGDivMod()
    ), p_data_decls->ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), se_pos);
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), se_pos);
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), se_nat);
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), se_nat);
  ATermAppl m_ = gsMakeDataVarId(gsString2ATermAppl("m_"), se_nat);
  ATermAppl n_ = gsMakeDataVarId(gsString2ATermAppl("n_"), se_nat);
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), se_nat_pair);
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), se_nat_pair);
  ATermList mnm_n_l = ATmakeList4((ATerm) m, (ATerm) n, (ATerm) m_, (ATerm) n_);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList bxl = ATmakeList2((ATerm) b, (ATerm) x);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bpmnl = ATmakeList4((ATerm) b, (ATerm) p, (ATerm) m, (ATerm) n);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList pqnl = ATmakeList3((ATerm) p, (ATerm) q, (ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  p_data_decls->data_eqns = ATconcat(ATmakeList(14,
      //equality (NatPair -> NatPair -> Bool)
      (ATerm) gsMakeDataEqn(mnm_n_l,nil, 
         gsMakeDataExprEq(gsMakeDataExprCPair(m, n), gsMakeDataExprCPair(m_, n_)),
         gsMakeDataExprAnd(gsMakeDataExprEq(m, m_), gsMakeDataExprEq(n, n_))),
      //inequality (NatPair -> NatPair -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprNeq(x, y), gsMakeDataExprNot(gsMakeDataExprEq(x, y))),
      //conditional (Bool -> NatPair -> NatPair -> NatPair)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprIf(t, x, y), x),
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprIf(f, x, y), y),
      (ATerm) gsMakeDataEqn(bxl,nil, gsMakeDataExprIf(b, x, x), x),
      //first (NatPair -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprFirst(gsMakeDataExprCPair(m, n)), m),
      //last (NatPair -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprLast(gsMakeDataExprCPair(m, n)), n),
      //quotient and remainder after division (Pos -> Pos -> NatPair)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprDivMod(one, one),
         gsMakeDataExprCPair(gsMakeDataExprCNat(one), zero)),
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDivMod(one, gsMakeDataExprCDub(b, p)),
         gsMakeDataExprCPair(zero, gsMakeDataExprCNat(one))),
      (ATerm) gsMakeDataEqn(bpql,nil,
         gsMakeDataExprDivMod(gsMakeDataExprCDub(b, p), q),
         gsMakeDataExprGDivMod(gsMakeDataExprDivMod(p, q), b, q)),
      //generalised quotient and remainder after division (NatPair -> Bool -> Pos -> NatPair)
      (ATerm) gsMakeDataEqn(bpmnl,nil,
         gsMakeDataExprGDivMod(gsMakeDataExprCPair(m, n), b, p),
         gsMakeDataExprGGDivMod(gsMakeDataExprDub(b, n), m, p)),
      //generalised generalised quotient and remainder after division (Nat -> Nat -> Pos -> NatPair)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprGGDivMod(zero, n, p),
         gsMakeDataExprCPair(gsMakeDataExprDub(f, n), zero)),
      (ATerm) gsMakeDataEqn(pqnl,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprGGDivMod(gsMakeDataExprCNat(p), n, q),
         gsMakeDataExprCPair(gsMakeDataExprDub(f, n), gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pqnl,
         gsMakeDataExprGTE(p, q),
         gsMakeDataExprGGDivMod(gsMakeDataExprCNat(p), n, q),
         gsMakeDataExprCPair(gsMakeDataExprDub(t, n), gsMakeDataExprGTESubtB(f, p, q)))
    ), p_data_decls->data_eqns);
}

void impl_sort_int(t_data_decls *p_data_decls)
{
  //Declare sort Int
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdInt());
  //Declare constructors for sort Int
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdCInt(),
      (ATerm) gsMakeOpIdCNeg()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Int
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_nat = gsMakeSortExprNat();
  ATermAppl se_int = gsMakeSortExprInt();
  p_data_decls->ops = ATconcat(ATmakeList(33,
      (ATerm) gsMakeOpIdEq(se_int),
      (ATerm) gsMakeOpIdNeq(se_int),
      (ATerm) gsMakeOpIdIf(se_int),
      (ATerm) gsMakeOpIdNat2Int(),
      (ATerm) gsMakeOpIdInt2Nat(),
      (ATerm) gsMakeOpIdPos2Int(),
      (ATerm) gsMakeOpIdInt2Pos(),
      (ATerm) gsMakeOpIdLTE(se_int),
      (ATerm) gsMakeOpIdLT(se_int),
      (ATerm) gsMakeOpIdGTE(se_int),
      (ATerm) gsMakeOpIdGT(se_int),
      (ATerm) gsMakeOpIdMax(se_pos, se_int),
      (ATerm) gsMakeOpIdMax(se_int, se_pos),
      (ATerm) gsMakeOpIdMax(se_nat, se_int),
      (ATerm) gsMakeOpIdMax(se_int, se_nat),
      (ATerm) gsMakeOpIdMax(se_int, se_int),
      (ATerm) gsMakeOpIdMin(se_int),
      (ATerm) gsMakeOpIdAbs(se_int),
      (ATerm) gsMakeOpIdNeg(se_pos),
      (ATerm) gsMakeOpIdNeg(se_nat),
      (ATerm) gsMakeOpIdNeg(se_int),
      (ATerm) gsMakeOpIdSucc(se_int),
      (ATerm) gsMakeOpIdPred(se_nat),
      (ATerm) gsMakeOpIdPred(se_int),
      (ATerm) gsMakeOpIdDub(se_int),
      (ATerm) gsMakeOpIdAdd(se_int, se_int),
      (ATerm) gsMakeOpIdSubt(se_pos),
      (ATerm) gsMakeOpIdSubt(se_nat),
      (ATerm) gsMakeOpIdSubt(se_int),
      (ATerm) gsMakeOpIdMult(se_int),
      (ATerm) gsMakeOpIdDiv(se_int),
      (ATerm) gsMakeOpIdMod(se_int),
      (ATerm) gsMakeOpIdExp(se_int)
    ), p_data_decls->ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) b, (ATerm) n);
  ATermList pnl = ATmakeList2((ATerm) p, (ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList bxl = ATmakeList2((ATerm) b, (ATerm) x);
  ATermList xl  = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  p_data_decls->data_eqns = ATconcat(ATmakeList(69,
      //equality (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprEq(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprEq(x, x), t),
      //inequality (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprNeq(x, y), gsMakeDataExprNot(gsMakeDataExprEq(x, y))),
      //conditional (Bool -> Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprIf(t, x, y), x),
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprIf(f, x, y), y),
      (ATerm) gsMakeDataEqn(bxl,nil, gsMakeDataExprIf(b, x, x), x),
      //convert Nat to Int (Nat -> Int)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdNat2Int(), gsMakeOpIdCInt()),
      //convert Int to Nat (Int -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprInt2Nat(gsMakeDataExprCInt(n)), n),
      //convert Pos to Int (Pos -> Int)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprPos2Int(p), gsMakeDataExprCInt(gsMakeDataExprCNat(p))),
      //convert Int to Pos (Int -> Pos)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprInt2Pos(gsMakeDataExprCInt(n)),
         gsMakeDataExprNat2Pos(n)),
      //less than or equal (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprLTE(q, p)),
      //less than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprLT(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprLT(q, p)),
      //greater than or equal (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGTE(x, y), gsMakeDataExprLTE(y, x)),
      //greater than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGT(x, y), gsMakeDataExprLT(y, x)),
      //maximum (Pos -> Int -> Pos)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCInt(n)),
         gsMakeDataExprMax(p, n)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCNeg(q)), p),
      //maximum (Int -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCInt(n), p),
         gsMakeDataExprMax(n, p)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(gsMakeDataExprCNeg(q), p), p),
      //maximum (Nat -> Int -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprMax(m, gsMakeDataExprCInt(n)),
         gsMakeDataExprMax(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(n, gsMakeDataExprCNeg(p)), n),
      //maximum (Int -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCInt(m), n),
         gsMakeDataExprMax(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCNeg(p), n), n),
      //maximum (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprMax(x, y),
         gsMakeDataExprIf(gsMakeDataExprLTE(x, y), y, x)),
      //minimum (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprMin(x, y),
         gsMakeDataExprIf(gsMakeDataExprLTE(x, y), x, y)),
      //absolute value (Int -> Nat)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCInt(n)), n),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNat(p)),
      //negation (Pos -> Int)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(p), gsMakeDataExprCNeg(p)),
      //negation (Nat -> Int)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprNeg(zero), gsMakeDataExprCInt(zero)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCNat(p)),
         gsMakeDataExprCNeg(p)),
      //negation (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCInt(n)), gsMakeDataExprNeg(n)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(p))),
      //successor (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprSucc(n)))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNeg(p)),
           gsMakeDataExprNeg(gsMakeDataExprPred(p))),      
      //predecessor (Nat -> Int)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprPred(zero), gsMakeDataExprCNeg(one)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCNat(p)),
         gsMakeDataExprCInt(gsMakeDataExprPred(p))),
      //predecessor (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPred(gsMakeDataExprCInt(n)), gsMakeDataExprPred(n)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNeg(gsMakeDataExprSucc(p))),
      //double (Bool -> Int -> Int)
      (ATerm) gsMakeDataEqn(bnl,nil,
         gsMakeDataExprDub(b, gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprDub(b, n))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprDub(f, gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNeg(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprDub(t, gsMakeDataExprCNeg(p)),
         gsMakeDataExprNeg(gsMakeDataExprPred(gsMakeDataExprCDub(f, p)))),
      //addition (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(mnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprAdd(m, n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)),
         gsMakeDataExprSubt(n, gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)),
         gsMakeDataExprSubt(n, gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprAdd(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprCNeg(gsMakeDataExprAddC(f, p, q))),
      //subtraction (Pos -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprGTE(p, q),
         gsMakeDataExprSubt(p, q),
         gsMakeDataExprCInt(gsMakeDataExprGTESubt(p, q))),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprSubt(p, q),
         gsMakeDataExprNeg(gsMakeDataExprGTESubt(q, p))),      
      //subtraction (Nat -> Nat -> Int)
      (ATerm) gsMakeDataEqn(mnl,
         gsMakeDataExprGTE(m, n),
         gsMakeDataExprSubt(m, n),
         gsMakeDataExprCInt(gsMakeDataExprGTESubt(m, n))),
      (ATerm) gsMakeDataEqn(mnl,
         gsMakeDataExprLT(m, n),
         gsMakeDataExprSubt(m, n),
         gsMakeDataExprNeg(gsMakeDataExprGTESubt(n, m))),      
      //subtraction (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprSubt(x, y), gsMakeDataExprAdd(x, gsMakeDataExprNeg(y))),
      //multiplication (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(mnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprMult(m, n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)),
         gsMakeDataExprNeg(gsMakeDataExprMult(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)),
         gsMakeDataExprNeg(gsMakeDataExprMult(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMult(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p, q)))),
      //quotient after division (Int -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprDiv(gsMakeDataExprCInt(n), p),
         gsMakeDataExprCInt(gsMakeDataExprDiv(n, p))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprDiv(gsMakeDataExprCNeg(p), q),
         gsMakeDataExprCNeg(gsMakeDataExprSucc(
           gsMakeDataExprDiv(gsMakeDataExprPred(p), q)))),
      //remainder after division (Int -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMod(gsMakeDataExprCInt(n), p),
         gsMakeDataExprMod(n, p)),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMod(gsMakeDataExprCNeg(p), q),
         gsMakeDataExprInt2Nat(gsMakeDataExprSubt(q, gsMakeDataExprSucc(
           gsMakeDataExprMod(gsMakeDataExprPred(p), q))))),
      //exponentiation (Int -> Nat -> Int)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprExp(gsMakeDataExprCInt(m), n),
         gsMakeDataExprCInt(gsMakeDataExprExp(m, n))),
      (ATerm) gsMakeDataEqn(pnl,
         gsMakeDataExprEven(n),
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p, n)))),
      (ATerm) gsMakeDataEqn(pnl,
         gsMakeDataExprNot(gsMakeDataExprEven(n)),
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprCNeg(gsMakeDataExprExp(p, n)))
    ), p_data_decls->data_eqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
}

void impl_sort_real(t_data_decls *p_data_decls)
{
  //Declare sort Real
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdReal());
  //Declare constructors for sort Real
  p_data_decls->cons_ops =
    ATconcat(ATmakeList1((ATerm) gsMakeOpIdCReal()), p_data_decls->cons_ops);
  //Declare operations for sort Real
  ATermAppl se_real = gsMakeSortExprReal();
  p_data_decls->ops = ATconcat(ATmakeList(23,
      (ATerm) gsMakeOpIdEq(se_real),
      (ATerm) gsMakeOpIdNeq(se_real),
      (ATerm) gsMakeOpIdIf(se_real),
      (ATerm) gsMakeOpIdPos2Real(),
      (ATerm) gsMakeOpIdNat2Real(),
      (ATerm) gsMakeOpIdInt2Real(),
      (ATerm) gsMakeOpIdReal2Pos(),
      (ATerm) gsMakeOpIdReal2Nat(),
      (ATerm) gsMakeOpIdReal2Int(),
      (ATerm) gsMakeOpIdLTE(se_real),
      (ATerm) gsMakeOpIdLT(se_real),
      (ATerm) gsMakeOpIdGTE(se_real),
      (ATerm) gsMakeOpIdGT(se_real),
      (ATerm) gsMakeOpIdMax(se_real, se_real),
      (ATerm) gsMakeOpIdMin(se_real),
      (ATerm) gsMakeOpIdAbs(se_real),
      (ATerm) gsMakeOpIdNeg(se_real),
      (ATerm) gsMakeOpIdSucc(se_real),
      (ATerm) gsMakeOpIdPred(se_real),
      (ATerm) gsMakeOpIdAdd(se_real, se_real),
      (ATerm) gsMakeOpIdSubt(se_real),
      (ATerm) gsMakeOpIdMult(se_real),
      (ATerm) gsMakeOpIdExp(se_real)
    ), p_data_decls->ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), gsMakeSortExprReal());
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), gsMakeSortExprReal());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList brl = ATmakeList2((ATerm) b, (ATerm) r);
  ATermList xl  = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList nxl = ATmakeList2((ATerm) n, (ATerm) x);
  ATermList rl  = ATmakeList1((ATerm) r);
  ATermList rsl = ATmakeList2((ATerm) r, (ATerm) s);
  p_data_decls->data_eqns = ATconcat(ATmakeList(26,
      //equality (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprEq(x, y)),
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprEq(r, r), t),
      //inequality (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprNeq(r, s), gsMakeDataExprNot(gsMakeDataExprEq(r, s))),
      //conditional (Bool -> Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprIf(t, r, s), r),
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprIf(f, r, s), s),
      (ATerm) gsMakeDataEqn(brl,nil, gsMakeDataExprIf(b, r, r), r),
      //convert Int to Real (Int -> Real)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdInt2Real(), gsMakeOpIdCReal()),
      //convert Nat to Real (Nat -> Real)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprNat2Real(n),
         gsMakeDataExprCReal(gsMakeDataExprCInt(n))),
      //convert Pos to Real (Pos -> Real)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprPos2Real(p),
         gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(p)))),
      //convert Real to Int (Real -> Int)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Int(gsMakeDataExprCReal(x)), x),
      //convert Real to Nat (Real -> Nat)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Nat(gsMakeDataExprCReal(x)),
         gsMakeDataExprInt2Nat(x)),
      //convert Real to Pos (Real -> Pos)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Pos(gsMakeDataExprCReal(x)),
         gsMakeDataExprInt2Pos(x)),
      //less than or equal (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprLTE(x, y)),
      //less than (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprLT(x, y)),
      //greater than or equal (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprGTE(r, s), gsMakeDataExprLTE(s, r)),
      //greater than (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprGT(r, s), gsMakeDataExprLT(s, r)),
      //maximum (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprMax(r, s),
         gsMakeDataExprIf(gsMakeDataExprLTE(r, s), s, r)),
      //minimum (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprMin(r, s),
         gsMakeDataExprIf(gsMakeDataExprLTE(r, s), r, s)),
      //absolute value (Real -> Real) 
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprAbs(x)))),
      //negation (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprNeg(x))),
      //successor (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprSucc(x))),
      //predecessor (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprPred(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprPred(x))),
      //addition (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprAdd(x, y))),
      //subtraction (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprSubt(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprSubt(x, y))),
      //multiplication (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprMult(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprMult(x, y))),
      //exponentiation (Real -> Nat -> Real)
      (ATerm) gsMakeDataEqn(nxl,nil,
         gsMakeDataExprExp(gsMakeDataExprCReal(x), n),
         gsMakeDataExprCReal(gsMakeDataExprExp(x, n)))
    ), p_data_decls->data_eqns);
  //add implementation of sort Int, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
    impl_sort_int(p_data_decls);
  }
}
