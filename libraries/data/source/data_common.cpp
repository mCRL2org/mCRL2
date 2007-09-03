// Author(s): Aad Mathijssen, Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libstruct_ir.cpp

#include <aterm2.h>

#include "mcrl2/data_common.h"
#include "libstruct.h"
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace ::mcrl2::utilities;

// Static declarations
// -------------------
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

// implementation
// -------------------------------------------------------------
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
    l = ATremoveElement(l,ATgetFirst(m));
  }

  return l;
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
  } else if (gsIsOpId(data_expr)) {
    //data_expr is an operation identifier or a number; do nothing
  } else if (gsIsDataAppl(data_expr)) {
    //data_expr is a product data application; get free variables from the
    //arguments
    get_free_vars_appl(ATAgetArgument(data_expr, 0), bound_vars, p_free_vars);
    get_free_vars_list(ATLgetArgument(data_expr, 1), bound_vars, p_free_vars);
  } else if (gsIsBinder(data_expr)) {
    ATermAppl binding_operator = ATAgetArgument(data_expr, 0);
    if (gsIsSetBagComp(binding_operator) || gsIsSetComp(binding_operator)
        || gsIsBagComp(binding_operator)) {
      //data_expr is a set or bag comprehension; get free variables from the body
      //where bound_vars is extended with the variable declaration
      ATermList vars = ATLgetArgument(data_expr, 1);
      ATermAppl var = ATAgetFirst(vars);
      if (ATindexOf(bound_vars, (ATerm) var, 0) == -1) {
        bound_vars = ATinsert(bound_vars, (ATerm) var);
      }
      get_free_vars_appl(ATAgetArgument(data_expr, 2), bound_vars, p_free_vars);
    } else if (gsIsLambda(binding_operator) || gsIsForall(binding_operator) ||
      gsIsExists(binding_operator)) {
      //data_expr is a lambda abstraction or a quantification; get free variables
      //from the body where bound_vars is extended with the variable declaration
      ATermList vars = ATLgetArgument(data_expr, 1);
      while (!ATisEmpty(vars)) {
        ATermAppl var = ATAgetFirst(vars);
        if (ATindexOf(bound_vars, (ATerm) var, 0) == -1) {
          bound_vars = ATinsert(bound_vars, (ATerm) var);
        }
        vars = ATgetNext(vars);
      }
      get_free_vars_appl(ATAgetArgument(data_expr, 2), bound_vars, p_free_vars);
    }
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
      ATermAppl var = ATAgetArgument(whr_decl, 0);
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
  if (gsIsSortArrow(part)) {
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

