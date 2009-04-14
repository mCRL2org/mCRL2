// Author(s): Aad Mathijssen, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_common.cpp

#include <aterm2.h>

#include "mcrl2/core/detail/data_common.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"

#include "workarounds.h" // DECL_A

using namespace mcrl2::core;

namespace mcrl2 {
  namespace core {
    namespace detail {

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

static void get_sorts_appl(ATermAppl part, ATermList *p_sorts);
//Pre: part is a part of a specification
//     *p_sorts represents the sorts that are already found
//Post:*p_sorts is extended with the sorts in part that did not
//     already occur in *p_sorts

static void get_sorts_list(ATermList parts, ATermList *p_sorts);
//Pre: parts is a list of parts of a specification
//     *p_sorts represents the sorts that are already found
//Post:*p_sorts is extended with the sorts in parts that did not
//     already occur in *p_sorts

//pre: DataExpr is a data expression,
//     Context is the substitution context to use.
//ret: The beta reduced version of DataExpr, if !recursive, then only
//     the highest level expression is evaluated, otherwise beta reduction
//     is performed recursively on sub expressions.
static ATermAppl beta_reduce(ATermAppl DataExpr, ATermList* Context, bool recursive);

//pre: Context is the substitution context to use.
//ret: List with beta reduction performed on the parts for which it is appropriate.
static ATermList beta_reduce_list(ATermList List, ATermList* Context);

//pre: Context is the substitution context to use.
//ret: Part with beta reduction performed on the parts for which it is appropriate.
static ATermAppl beta_reduce_part(ATermAppl Part, ATermList* Context);
//pre: Part is data expression,
//     OldValue is a data variable,
//     NewValue is a data expression,
//     Context is the context used for creating fresh variables
//ret: if Part is an OpId, Part,
//     if Part is a DataVarId: if Part == OldValue, then NewValue,
//                             otherwise, Part
//     if Part is a DataAppl LHS(RHS):
//         capture_avoiding_subst(LHS)(capture_avoiding_subst(RHS,...))
//     if Part is a Binder(BindingOp, Vars, Expr):
//         if OldValue is a Bound variable: Part,
//         if !(Vars in free variables of NewValue):
//             Binder(BindingOp, Vars, capture_avoiding_subst(Expr,...)
//         else: Fresh variables are introduced for the bound variables occurring in
//               the free variables of NewValue, and then substitution is performed, i.e.
//               Binder(BindingOp, NewVars,
//                      capture_avoiding_subst(
//                        capture_avoiding_subst(Expr,BoundVar,NewBoundVar,...),...)

static ATermAppl capture_avoiding_subst(ATermAppl Part,
                                        ATermAppl OldValue,
                                        ATermAppl NewValue,
                                        ATermList* Context);

//pre: Part is a data expression,
//     Substs is a list containing lists of length 2,
//       for each of these lists in Substs it holds that the first
//       argument is a data variable, and the second argument is a data expression.
//     Context is the substitution context to use.
//ret: Part in which all substitutions in Substs have been performed using
//     capture avoiding substitution.
static ATermAppl capture_avoiding_substs(ATermAppl Part,
                                             ATermList Substs,
                                             ATermList* Context);
// implementation
// -------------------------------------------------------------

// --------------------------
// Auxiliary list operations
// --------------------------

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

// ---------------------------------------------
// Auxiliary functions for system defined sorts
// ---------------------------------------------

bool is_list_enum_impl(ATermAppl data_expr)
{
  if (!gsIsDataAppl(data_expr) && !gsIsOpId(data_expr)) return false;
  ATermAppl HeadName = ATAgetArgument(gsGetDataExprHead(data_expr), 0);
  if (ATisEqual(HeadName, gsMakeOpIdNameCons())) {
    ATermList Args = gsGetDataExprArgs(data_expr);
    if (ATgetLength(Args) == 2) {
      return is_list_enum_impl(ATAelementAt(Args, 1));
    } else {
      return false;
    }
  } else {
    return ATisEqual(HeadName, gsMakeOpIdNameEmptyList());
  }
}

// ------------------------------------------
// Auxiliary functions for data declarations
// ------------------------------------------

ATermAppl add_data_decls(ATermAppl spec, t_data_decls data_decls)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsActionRenameSpec(spec) || gsIsDataSpec(spec));
  assert(data_decls_is_initialised(data_decls));
  ATermAppl data_spec;
  if (gsIsDataSpec(spec)) {
    data_spec = spec;
  } else {
    data_spec = ATAgetArgument(spec, 0);
  }
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
  if (gsIsDataSpec(spec)) {
    spec = data_spec;
  } else {
    spec = ATsetArgument(spec, (ATerm) data_spec, 0);
  }
  return spec;
}

// --------------------
// Auxiliary functions
// --------------------

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

ATermList get_sorts(ATerm term)
{
  ATermList sorts = ATmakeList0();
  if (ATgetType(term) == AT_APPL) {
    get_sorts_appl((ATermAppl) term, &sorts);
  } else { //ATgetType(term) == AT_LIST
    get_sorts_list((ATermList) term, &sorts);
  }
  return sorts;

}

void get_sorts_appl(ATermAppl part, ATermList *p_sorts)
{
  // Do not consider sorts specified in if, ==, != as these are
  // system defined any way.
  if (gsIsOpId(part))
  {
    ATermAppl Name = gsGetName(part);
    if (ATisEqual(Name, gsMakeOpIdNameIf()) ||
        ATisEqual(Name, gsMakeOpIdNameEq()) ||
        ATisEqual(Name, gsMakeOpIdNameNeq()))
    {
      return;
    }
  }

  if (gsIsSortExpr(part)) {
    if (ATindexOf(*p_sorts, (ATerm) part, 0) == -1) {
      *p_sorts = ATinsert(*p_sorts, (ATerm) part);
    }
  }
  int nr_args = ATgetArity(ATgetAFun(part));
  for (int i = 0; i < nr_args; i++) {
    ATerm arg = ATgetArgument(part, i);
    if (ATgetType(arg) == AT_APPL)
      get_sorts_appl((ATermAppl) arg, p_sorts);
    else //ATgetType(arg) == AT_LIST
      get_sorts_list((ATermList) arg, p_sorts);
  }
}

void get_sorts_list(ATermList parts, ATermList *p_sorts)
{
  while (!ATisEmpty(parts))
  {
    get_sorts_appl(ATAgetFirst(parts), p_sorts);
    parts = ATgetNext(parts);
  }
}

ATerm beta_reduce_term(ATerm Term)
{
  if (ATgetType(Term) == AT_APPL) {
    ATermList Context = ATmakeList1(Term);
    return (ATerm) beta_reduce_part((ATermAppl) Term, &Context);
  } else {
    // ATgetType(Term) == AT_LIST
    ATermList Context = (ATermList) Term;
    return (ATerm) beta_reduce_list((ATermList) Term, &Context);
  }
}

ATermAppl beta_reduce(ATermAppl DataExpr, ATermList* Context, bool recursive)
{
  assert(gsIsDataExpr(DataExpr));
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl LHS = ATAgetArgument(DataExpr, 0);
    if (gsIsBinder(LHS)) {
      ATermAppl BindingOp = ATAgetArgument(LHS, 0);
      if (gsIsLambda(BindingOp)) {
        ATermList Vars = ATLgetArgument(LHS, 1);
        ATermAppl Expr = ATAgetArgument(LHS, 2);
        ATermList RHS = ATLgetArgument(DataExpr, 1);
        // Note that first the right hand side needs to be done because of the
        // substitution order!
        if (recursive) {
          RHS = beta_reduce_list(RHS, Context);
        }
        assert(ATgetLength(RHS) == ATgetLength(Vars));
        ATermList Substs = ATmakeList0();
        while(!ATisEmpty(RHS)) {
          Substs = ATinsert(Substs, (ATerm) ATmakeList2(ATgetFirst(Vars),
                                                        ATgetFirst(RHS)));
          Vars = ATgetNext(Vars);
          RHS = ATgetNext(RHS);
        }
        Substs = ATreverse(Substs);
        DataExpr = capture_avoiding_substs(Expr, Substs, Context);
        ATermAppl tmp = beta_reduce(DataExpr, Context, true);
        while (!ATisEqual(DataExpr, tmp)) {
          DataExpr = beta_reduce(tmp, Context, true);
          tmp = beta_reduce(DataExpr, Context, true);
        }
      }
    }
  }

  if (recursive) {
    if (gsIsDataAppl(DataExpr)) {
      ATermList args = ATLgetArgument(DataExpr, 1);
      ATermList new_args = ATmakeList0();
      while(!ATisEmpty(args))
      {
        new_args = ATinsert(new_args, (ATerm) beta_reduce(ATAgetFirst(args), Context, recursive));
        args = ATgetNext(args);
      }
      new_args = ATreverse(new_args);
      DataExpr = gsMakeDataAppl(beta_reduce(ATAgetArgument(DataExpr, 0), Context, recursive), new_args);
    } else if (gsIsBinder(DataExpr)) {
      ATermAppl new_body = beta_reduce(ATAgetArgument(DataExpr, 2), Context, recursive);
      DataExpr = gsMakeBinder(ATAgetArgument(DataExpr, 0), ATLgetArgument(DataExpr, 1), new_body);
    } else if (gsIsWhr(DataExpr)) {
      ATermAppl new_expr = beta_reduce(ATAgetArgument(DataExpr, 0), Context, recursive);
      ATermList new_decls = ATmakeList0();
      ATermList decls = ATLgetArgument(DataExpr, 1);
      while(!ATisEmpty(decls)) {
        ATermAppl decl = ATAgetFirst(decls);
        ATermAppl var = ATAgetArgument(decl, 0);
        ATermAppl new_val = beta_reduce(ATAgetArgument(decl, 1), Context, recursive);
        assert(gsIsDataVarIdInit(decl));
        new_decls = ATinsert(new_decls, (ATerm) gsMakeDataVarIdInit(var, new_val));
        decls = ATgetNext(decls);
      }
      new_decls = ATreverse(decls);
      DataExpr = gsMakeWhr(new_expr, new_decls);
    }
  }

  return DataExpr;
}

ATermList beta_reduce_list(ATermList List, ATermList* Context)
{
  ATermList result = ATmakeList0();
  while(!ATisEmpty(List)) {
    result = ATinsert(result, (ATerm) beta_reduce_part(ATAgetFirst(List), Context));
    List = ATgetNext(List);
  }
  result = ATreverse(result);
  return result;
}

ATermAppl beta_reduce_part(ATermAppl Part, ATermList* Context)
{
  if (gsIsDataExpr(Part)) {
    Part = beta_reduce(Part, Context, true);
  }

  //reconstruct expressions in the arguments of part
  // If the top-level term is annotated with @dummy, then preserve annotation,
  // this is needed for matching data expressions.
  static ATerm dummy = (ATerm) gsString2ATermAppl("@dummy");
  ATerm dummy_ann = ATgetAnnotation((ATerm) Part, dummy);
  AFun head = ATgetAFun(Part);
  int nr_args = ATgetArity(head);
  if (nr_args > 0) {
    DECL_A(args,ATerm,nr_args);
    for (int i = 0; i < nr_args; i++) {
      ATerm arg = ATgetArgument(Part, i);
      if (ATgetType(arg) == AT_APPL)
        args[i] = (ATerm) beta_reduce_part((ATermAppl) arg, Context);
      else //ATgetType(arg) == AT_LIST
        args[i] = (ATerm) beta_reduce_list((ATermList) arg, Context);
    }
    Part = ATmakeApplArray(head, args);
    FREE_A(args);
  }
  if (dummy_ann != NULL) {
    Part = (ATermAppl) ATsetAnnotation((ATerm) Part, dummy, dummy);
  }

  return Part;
}

ATermAppl capture_avoiding_subst(ATermAppl Part, ATermAppl OldValue,
                                 ATermAppl NewValue, ATermList* Context)
{
  //gsDebugMsg("Performing capture avoiding substitution on %T with OldValue %T and NewValue %T\n",
  //           Part, OldValue, NewValue);

  // Slight performance enhancement
  if (ATisEqual(OldValue, NewValue)) {
    return Part;
  }

  // Substitute NewValue for OldValue (return Part[NewValue / OldValue])
  if(gsIsOpId(Part)) {
    return Part;
  } else if (gsIsDataVarId(Part)) {
    if (ATisEqual(Part, OldValue)) {
      return NewValue;
    } else {
      return Part;
    }
  } else if (gsIsDataAppl(Part)) {
    // Part is a data application, distribute substitution over LHS and RHS.
    // Recurse on left hand side of the expression.
    ATermAppl LHS = ATAgetArgument(Part, 0);
    LHS = capture_avoiding_subst(LHS, OldValue, NewValue, Context);

    // Recurse on all parts in right hand side of the expression.
    ATermList RHS = ATLgetArgument(Part, 1);
    ATermList NewRHS = ATmakeList0();
    while (!ATisEmpty(RHS)) {
      ATermAppl First = ATAgetFirst(RHS);
      First = capture_avoiding_subst(First, OldValue, NewValue, Context);
      NewRHS = ATinsert(NewRHS, (ATerm) First);
      RHS = ATgetNext(RHS);
    }
    NewRHS = ATreverse(NewRHS);
    // Construct new application.
    Part = gsMakeDataAppl(LHS, NewRHS);

    if (gsIsBinder(LHS) || !ATisEqual(RHS, NewRHS)) {
      Part = beta_reduce(Part, Context, true);
    }

    return Part;
    //return gsMakeDataAppl(LHS, NewRHS);
  } else if (gsIsBinder(Part)) {
    if (ATindexOf(ATLgetArgument(Part, 1), (ATerm) OldValue, 0) != -1) {
      // OldValue occurs as a bound variable
      return Part;
    } else {
      ATermAppl BindingOp = ATAgetArgument(Part, 0);
      ATermList BoundVars = ATLgetArgument(Part, 1);
      ATermAppl Expr = ATAgetArgument(Part, 2);
      ATermList FreeVars = get_free_vars(Expr);
      ATermList NewVars = BoundVars; // List to contain the new bound variables

      // Resolve name conflicts by substituting bound variables for fresh ones
      while(!ATisEmpty(BoundVars)) {
        ATermAppl Var = ATAgetFirst(BoundVars);
        if (ATindexOf(FreeVars, (ATerm) Var, 0) != -1) {
          // Bound variable also occurs as free variable in Expr,
          // resolve the conflict by renaming the bound variable.
          // Introduce fresh variable
          ATermAppl Name = ATAgetArgument(Var, 0);
          ATermAppl Sort = ATAgetArgument(Var, 1);
          Name = gsFreshString2ATermAppl(gsATermAppl2String(Name),
                                         (ATerm) *Context, true);
          ATermAppl NewVar = gsMakeDataVarId(Name, Sort);
          *Context = ATinsert(*Context, (ATerm) NewVar);
          // Substitute: Vars[NewVar/Var]
          ATermList Vars = NewVars; // Temp list for traversal
          NewVars = ATmakeList0();
          while(!ATisEmpty(Vars)) {
            ATermAppl FirstVar = ATAgetFirst(Vars);
            FirstVar = capture_avoiding_subst(FirstVar, Var, NewVar, Context);
            NewVars = ATinsert(NewVars, (ATerm) FirstVar);
            Vars = ATgetNext(Vars);
          }
          // Expr[NewVar/Var]
          Expr = capture_avoiding_subst(Expr, Var, NewVar, Context);
        }
        BoundVars = ATgetNext(BoundVars);
      }
      Expr = capture_avoiding_subst(Expr, OldValue, NewValue, Context);
      return gsMakeBinder(BindingOp, NewVars, Expr);
    }
  } else if (gsIsWhr(Part)) {
    // After data implementation Whr does not occur,
    // therefore we do not handle this case.
    gsWarningMsg("Currently not substituting Whr expression\n");
    return Part;
  } else {
    gsWarningMsg("Unknown part %T\n");
    return Part;
  }
}

ATermAppl capture_avoiding_substs(ATermAppl Part, ATermList Substs,
                                  ATermList* Context)
{
  while(!ATisEmpty(Substs)) {
    ATermList Subst = ATLgetFirst(Substs);
    Part = capture_avoiding_subst(Part, ATAelementAt(Subst, 0),
                                  ATAelementAt(Subst, 1), Context);
    Substs = ATgetNext(Substs);
  }
  return Part;
}

ATermAppl capture_avoiding_substitutions(ATermAppl Part, ATermList Substs)
{
  ATermList context = ATmakeList0();
  return capture_avoiding_substs(Part, Substs, &context);
}
    } // namespace detail
  }   // namespace core
}     // namespace mcrl2
