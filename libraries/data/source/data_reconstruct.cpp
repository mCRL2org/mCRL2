// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct.cpp
//
//This file contains the implementation of data reconstruction. I.e.
//it attempts to revert the data implementation.

#include <assert.h>
#include <aterm2.h>

#include "mcrl2/data_reconstruct.h"
#include "mcrl2/data_common.h"
#include "mcrl2/dataimpl.h"
#include "libstruct.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/numeric_string.h"
#include "print/messaging.h"

using namespace ::mcrl2::utilities;

// declarations
// ----------------------------------------------

//ret: The reconstructed version of Part.
static ATermAppl reconstruct_exprs_appl(ATermAppl Part, const ATermAppl Spec = NULL);

//ret: The reconstructed version of Parts.
static ATermList reconstruct_exprs_list(ATermList Parts, const ATermAppl Spec = NULL);

//TODO: Describe prototype
static ATermAppl reconstruct_pos_mult(ATermAppl PosExpr, char* Mult);

//pre: OpId is an OpId of the form lambda@x (with x a natural number),
//     Spec is a SpecV1
//ret: The lambda expression from which Part originates.
static ATermAppl reconstruct_lambda_op(const ATermAppl Part, const ATermAppl Spec);

//pre: Part is a data expression
//ret: true if Part is a lambda expression (DataAppl(lambda@x, [...])), 
//     false otherwise.
static inline bool is_lambda_expr(const ATermAppl Part);

//pre: Spec is a Specification
//ret: Spec from which system defined functions and equations
//     have been removed.
static ATermAppl remove_headers_without_binders_from_spec(ATermAppl Spec);

//pre: Spec is a Specification after data reconstruction
//ret: Spec from which set and bag functions have been removed.
static ATermAppl remove_headers_with_binders_from_spec(ATermAppl Spec, ATermAppl OrigSpec);

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

//pre: Term to perform beta reduction on,
//     this is the top-level function, which should be used when
//     there is no appropriate context.
//ret: Term with beta reduction performed on it.
static ATerm beta_reduce_term(ATerm Term);

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

//ret: l from which the part starting with the first element of m, of size(m) has been removed.
static ATermList subtract_list_slice(ATermList l, ATermList m);

//pre: list_sort is a sort for a list implementation
//post:p_data_decls from which the implementation of list_sort has been removed
static void remove_list_sort_from_data_decls(ATermAppl list_sort, t_data_decls* p_data_decls);

//pre: set_sort is a sort for a set implementation
//     spec is a Specification
//post:p_data_decls from which the implementation of set_sort has been removed
static void remove_set_sort_from_data_decls(ATermAppl set_sort, t_data_decls* p_data_decls, ATermAppl spec);

//pre: bag_sort is a sort for a bag implementation
//     spec is a Specification
//post:p_data_decls from which the implementation of bag_sort has been removed
static void remove_bag_sort_from_data_decls(ATermAppl bag_sort, t_data_decls* p_data_decls, ATermAppl spec);

//pre: list_impl_sort is a sort expression for a list implementation
//ret: The sort of the elements of list_impl_sort.
static ATermAppl find_elt_sort_for_list_impl(ATermAppl list_impl_sort, t_data_decls* p_data_decls);

//ret: p_data_decls_1 from which from each of the fields the contiguous block starting with
//     the first element of the corresponding field of p_data_decls_2 has been removed with
//     subtract_list_slice.
static inline void subtract_slice_data_decls(t_data_decls* p_data_decls_1, t_data_decls* p_data_decls_2)
{
  p_data_decls_1->sorts = subtract_list_slice(p_data_decls_1->sorts, p_data_decls_2->sorts);
  p_data_decls_1->ops = subtract_list_slice(p_data_decls_1->ops, p_data_decls_2->ops);
  p_data_decls_1->cons_ops = subtract_list_slice(p_data_decls_1->cons_ops, p_data_decls_2->cons_ops);
  p_data_decls_1->data_eqns = subtract_list_slice(p_data_decls_1->data_eqns, p_data_decls_2->data_eqns);
}

// implementation
// ----------------------------------------------
ATerm reconstruct_exprs(ATerm Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec));
  if (Spec == NULL) {
    gsDebugMsg("No specification given, "
                 "therefore not all components can be reconstructed\n");
  } else {
    gsDebugMsg("Specification provided, performing full reconstruction\n");
  }

  ATerm Result;
  if (ATgetType(Part) == AT_APPL) {
    Result = (ATerm) reconstruct_exprs_appl((ATermAppl) Part, Spec);
  } else { //(ATgetType(Part) == AT_LIST) {
    Result = (ATerm) reconstruct_exprs_list((ATermList) Part, Spec);
  }
  gsDebugMsg("Finished data reconstruction\n");
  return Result;
}

ATermAppl reconstruct_exprs_appl(ATermAppl Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec));

  if(gsIsSpecV1(Part) && (Spec != NULL)) {
    gsDebugMsg("Removing headers from specification\n");
    Part = remove_headers_without_binders_from_spec(Part);
  }

  bool recursive = true;
  // Reconstruct Data Expressions
  if (gsIsDataExpr(Part)) {
    if (gsIsDataExprBagComp(Part)) {
      gsDebugMsg("Reconstructing implementation of bag comprehension\n");
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
      gsDebugMsg("Reconstructing implementation of set comprehension\n");
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
      gsDebugMsg("Reconstructing implementation of universal quantification\n");
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
      gsDebugMsg("Reconstructing implementation of existential quantification\n");
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
      gsDebugMsg("Reconstructing implementation of a lambda expression\n");
      if (Spec) {
        ATermAppl OpId = ATAgetArgument(Part, 0);
        ATermList Args = ATLgetArgument(Part, 1);
        Part = gsMakeDataAppl(reconstruct_lambda_op(OpId, Spec), Args);
      }
    } else if (is_lambda_op_id(Part)) {
      gsDebugMsg("Reconstructing implementation of a lambda operator\n");
      if (Spec) {
        Part = reconstruct_lambda_op(Part, Spec);
      }
    } else if (gsIsDataExprC1(Part) || gsIsDataExprCDub(Part)) {
      gsDebugMsg("Reconstructing implementation of a positive number (%T)\n", Part);
      if (gsIsPosConstant(Part)) {
        Part = gsMakeOpId(gsString2ATermAppl(gsPosValue(Part)), gsMakeSortExprPos());
      } else {
        Part = reconstruct_pos_mult(Part, "1");
      }
    } else if (gsIsDataExprC0(Part)) {
      Part = gsMakeOpId(gsString2ATermAppl("0"), gsMakeSortExprNat());
    } else if (gsIsDataExprCNat(Part)) {
      Part = gsMakeDataExprPos2Nat(ATAgetFirst(ATLgetArgument(Part, 1)));
    } else if (gsIsDataExprCInt(Part)) {
      Part = gsMakeDataExprNat2Int(ATAgetFirst(ATLgetArgument(Part, 1)));
    } else if (gsIsDataExprCReal(Part)) {
      Part = gsMakeDataExprInt2Real(ATAgetFirst(ATLgetArgument(Part, 1)));
    } else if (gsIsDataExprCNeg(Part)) {
      Part = gsMakeDataExprNeg(ATAgetFirst(ATLgetArgument(Part, 1)));
    }
  } else if (gsIsSortExpr(Part)) {
    // Reconstruct sort expressions if needed
    if (is_list_sort_id(Part) && Spec != NULL) {
      ATermAppl cons_spec = ATAgetArgument(ATAgetArgument(Spec,0), 1);
      ATermList cons_ops = ATLgetArgument(cons_spec, 0);
      bool found = false;
      while (!ATisEmpty(cons_ops) && !found) {
        ATermAppl cons_op = ATAgetFirst(cons_ops);
        if (ATisEqual(gsGetName(cons_op), gsMakeOpIdNameCons())) {
          ATermList sort_domain = ATLgetArgument(gsGetSort(cons_op), 0);
          if (ATisEqual(Part, ATAelementAt(sort_domain, 1))) {
            Part = gsMakeSortExprList(ATAgetFirst(sort_domain));
            found = true;
          }
        }
        cons_ops = ATgetNext(cons_ops);
      }
    } else if (is_set_sort_id(Part) && Spec != NULL) {
      ATermAppl map_spec = ATAgetArgument(ATAgetArgument(Spec, 0), 2);
      ATermList ops = ATLgetArgument(map_spec, 0);
      bool found = false;
      while (!ATisEmpty(ops)) {
        ATermAppl op = ATAgetFirst(ops);
        if (ATisEqual(gsGetName(op), gsMakeOpIdNameSetComp())) {
          ATermAppl op_sort = gsGetSort(op);
          if (ATisEqual(Part, ATAgetArgument(op_sort, 1))) {
            ATermList sort_domain = ATLgetArgument(op_sort, 0);
            assert(ATgetLength(sort_domain) == 1); //Per construction
            Part = gsMakeSortExprSet(ATAgetFirst(sort_domain));
            found = true;
          }
        }
        ops = ATgetNext(ops);
      }
    } else if (is_bag_sort_id(Part) && Spec != NULL) {
      ATermAppl map_spec = ATAgetArgument(ATAgetArgument(Spec, 0), 2);
      ATermList ops = ATLgetArgument(map_spec, 0);
      bool found = false;
      while (!ATisEmpty(ops)) {
        ATermAppl op = ATAgetFirst(ops);
        if (ATisEqual(gsGetName(op), gsMakeOpIdNameBagComp())) {
          ATermAppl op_sort = gsGetSort(op);
          if (ATisEqual(Part, ATAgetArgument(op_sort, 1))) {
            ATermList sort_domain = ATLgetArgument(op_sort, 0);
            assert(ATgetLength(sort_domain) == 1); //Per construction
            Part = gsMakeSortExprBag(ATAgetFirst(sort_domain));
            found = true;
          }
        }
        ops = ATgetNext(ops);
      }
    }
  }

  //reconstruct expressions in the arguments of part
  if (recursive) {
    AFun head = ATgetAFun(Part);
    int nr_args = ATgetArity(head);
    if (nr_args > 0) {
      DECL_A(args,ATerm,nr_args);
      for (int i = 0; i < nr_args; i++) {
        ATerm arg = ATgetArgument(Part, i);
        if (ATgetType(arg) == AT_APPL)
          args[i] = (ATerm) reconstruct_exprs_appl((ATermAppl) arg, Spec);
        else //ATgetType(arg) == AT_LIST
          args[i] = (ATerm) reconstruct_exprs_list((ATermList) arg, Spec);
      }
      Part = ATmakeApplArray(head, args);
      FREE_A(args);
    }
  }

  if ((Spec != NULL) && gsIsDataAppl(Part)) {
    // Beta reduction if possible
    ATermList Context = ATmakeList1((ATerm) Part);
    Part = beta_reduce(Part, &Context, false);
  } 

  if (gsIsSpecV1(Part) && Spec != NULL) {
    Part = remove_headers_with_binders_from_spec(Part, Spec);
  }

  return Part;
}

ATermList reconstruct_exprs_list(ATermList Parts, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec));

  ATermList result = ATmakeList0();
  while (!ATisEmpty(Parts))
  {
    result = ATinsert(result, (ATerm)
      reconstruct_exprs_appl(ATAgetFirst(Parts), Spec));
    Parts = ATgetNext(Parts);
  }
  return ATreverse(result);
}

ATermAppl bool_to_nat(ATermAppl BoolExpr)
{
  return gsMakeDataExprIf(BoolExpr, gsMakeOpId(gsString2ATermAppl("1"), gsMakeSortExprNat()), gsMakeOpId(gsString2ATermAppl("0"), gsMakeSortExprNat()));
}

ATermAppl reconstruct_pos_mult(ATermAppl PosExpr, char* Mult)
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
    } else {
      //Mult*v(b) > 0
      if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
        //Mult*v(b) = Mult
        return gsMakeDataExprAdd(PosArg, gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos()));
      } else if (strcmp(Mult, "1") == 0) {
        //Mult*v(b) = v(b)
        return gsMakeDataExprAdd(PosArg, bool_to_nat(BoolArg));
      } else {
        //Mult*v(b)
        return gsMakeDataExprAdd(PosArg, gsMakeDataExprMult(gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos()), 
                                                            bool_to_nat(BoolArg)));
      }
    }
  } else {
    //PosExpr is not a Pos constructor
    if (strcmp(Mult, "1") == 0) {
      return PosExpr;
    } else {
      return gsMakeDataExprMult(gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos()), PosExpr);
    }
  }
}

ATermAppl reconstruct_lambda_op(const ATermAppl Part, const ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec));
  assert(is_lambda_op_id(Part));
  gsDebugMsg("Reconstructing lambda operator %T\n", Part);

  ATermAppl DataSpec = ATAgetArgument(Spec, 0);
  ATermAppl DataEqnSpec = ATAgetArgument(DataSpec, 3);
  ATermList DataEqns = ATLgetArgument(DataEqnSpec, 0);
  ATermAppl DataEqn;

  while(!ATisEmpty(DataEqns)) {
    DataEqn = ATAgetFirst(DataEqns);
    DataEqns = ATgetNext(DataEqns);
    ATermAppl Expr = ATAgetArgument(DataEqn, 2);
    while (gsIsDataAppl(Expr)) {
      ATermList BoundVars = ATLgetArgument(Expr, 1);
      Expr = ATAgetArgument(Expr, 0);
      if (ATisEqual(Expr, Part)) {
        ATermList Vars = ATLgetArgument(DataEqn, 0);
        ATermAppl Body = ATAgetArgument(DataEqn, 3);
        if (ATgetLength(Vars) != ATgetLength(BoundVars)) {
          //There are free variables in the expression, construct an additional lambda expression
          ATermList FreeVars = ATmakeList0();
          while(!ATisEmpty(Vars)) {
            ATermAppl Var = ATAgetFirst(Vars);
            if(ATindexOf(BoundVars, (ATerm) Var, 0) == -1) {
              FreeVars = ATinsert(FreeVars, (ATerm) Var);
            }
            Vars = ATgetNext(Vars);
          }
          Body = gsMakeBinder(gsMakeLambda(), FreeVars, Body);  
        }
        return gsMakeBinder(gsMakeLambda(), BoundVars, Body);
      }
    }
  }

  // This should not be reached
  gsWarningMsg("No equation found for %T\n", Part);
  return Part;
}

bool is_lambda_expr(const ATermAppl Part)
{
  if(gsIsDataAppl(Part)) {
    return is_lambda_op_id(ATAgetArgument(Part, 0));
  } else {
    return false;
  }
}

ATermAppl remove_headers_without_binders_from_spec(ATermAppl Spec)
{
  gsDebugMsg("Removing headers from specification\n");
  assert(gsIsSpecV1(Spec));
  // Superfluous declarations are in MapSpec and DataEqnSpec
  ATermAppl DataSpec = ATAgetArgument(Spec, 0);

  gsDebugMsg("Dissecting data specification\n");
  // Dissect Data specification
  ATermAppl SortSpec    = ATAgetArgument(DataSpec, 0);
  ATermAppl ConsSpec    = ATAgetArgument(DataSpec, 1);
  ATermAppl MapSpec     = ATAgetArgument(DataSpec, 2);
  ATermAppl DataEqnSpec = ATAgetArgument(DataSpec, 3);

  // Get the lists for data declarations
  gsDebugMsg("Retrieving data declarations\n");
  t_data_decls data_decls;
  data_decls.sorts     = ATLgetArgument(SortSpec, 0);
  data_decls.cons_ops  = ATLgetArgument(ConsSpec, 0);
  data_decls.ops       = ATLgetArgument(MapSpec, 0);
  data_decls.data_eqns = ATLgetArgument(DataEqnSpec, 0);

  // Removing function sorts
  // (this is using the knowledge that function sorts occur before everything
  // else in the specification, therefor this order is most efficient)
  gsDebugMsg("Removing implementation of function sorts\n");
  ATermList function_sorts = get_function_sorts((ATerm) data_decls.ops);
  while(!ATisEmpty(function_sorts))
  {
    bool is_function_sort_impl = true;
    t_data_decls function_decls;
    initialize_data_decls(&function_decls);
    impl_function_sort(ATAgetFirst(function_sorts), &function_decls);
    //function_decls contains the system defined part for first(function_sorts)

    // Make sure that the sort we are evaluating really is a system defined sort.
    for(ATermList ops = function_decls.ops; is_function_sort_impl && !(ATisEmpty(ops)); 
        ops = ATgetNext(ops))
    {
      is_function_sort_impl = (ATindexOf(data_decls.ops, ATgetFirst(ops), 0) != -1);
    }
    for(ATermList data_eqns = function_decls.data_eqns; is_function_sort_impl && !(ATisEmpty(data_eqns));
        data_eqns = ATgetNext(data_eqns))
    {
      is_function_sort_impl = (ATindexOf(data_decls.data_eqns, ATgetFirst(data_eqns), 0) != -1);
    }

    if (is_function_sort_impl) {
      data_decls.ops = subtract_list_slice(data_decls.ops, function_decls.ops);
      data_decls.data_eqns = subtract_list_slice(data_decls.data_eqns, function_decls.data_eqns);
    }
    function_sorts = ATgetNext(function_sorts);
  }

  // Construct lists of data declarations for system defined sorts
  t_data_decls data_decls_impl;
  initialize_data_decls(&data_decls_impl);

  gsDebugMsg("Removing system defined sorts from data declarations\n");
  if (ATindexOf(data_decls.sorts, (ATerm) gsMakeSortExprBool(), 0) != -1) {
    impl_sort_bool    (&data_decls_impl);
    subtract_slice_data_decls(&data_decls, &data_decls_impl);
    initialize_data_decls(&data_decls_impl);
  }
  if (ATindexOf(data_decls.sorts, (ATerm) gsMakeSortExprPos(), 0) != -1) {
    impl_sort_pos     (&data_decls_impl);
    subtract_slice_data_decls(&data_decls, &data_decls_impl);
    initialize_data_decls(&data_decls_impl);
  }
  if (ATindexOf(data_decls.sorts, (ATerm) gsMakeSortExprNat(), 0) != -1) {
    // Nat is included in the implementation of other sorts, as well as that it
    // includes the implementation of natpair, so needs to be
    // removed with the rest of these.
    impl_sort_nat     (&data_decls_impl);
  }
  if (ATindexOf(data_decls.sorts, (ATerm) gsMakeSortExprNatPair(), 0) != -1) {
    // NatPair includes implementation of Nat, so it needs to be included in a larger
    // batch.
    impl_sort_nat_pair(&data_decls_impl);
  }
  if (ATindexOf(data_decls.sorts, (ATerm) gsMakeSortExprInt(), 0) != -1) {
    // Int includes implementation of Nat, so it needs to be included in a 
    // larger batch.
    impl_sort_int     (&data_decls_impl);
  }
  if (ATindexOf(data_decls.sorts, (ATerm) gsMakeSortExprReal(), 0) != -1) {
    // Real includes implementation of Int, so it needs to be included in a
    // larger batch.
    impl_sort_real    (&data_decls_impl);
  }

  subtract_data_decls(&data_decls, &data_decls_impl);

  // Additional processing of data declarations by manually recognising
  // system defined sorts, operators and data equations
  // these are removed from their respective parts of the data declarations
  // on the fly.

  // Additional processing of Sorts
  for (ATermList sorts = data_decls.sorts; !ATisEmpty(sorts); sorts = ATgetNext(sorts))
  {
    ATermAppl sort = ATAgetFirst(sorts);
    if (is_struct_sort_id(sort)) {
      //TODO
    } else if (is_list_sort_id(sort)) {
      remove_list_sort_from_data_decls(sort, &data_decls);
    }
  }

/*
  // Additional processing of ConsOps
  // TODO
*/

  // Additional processing of ops
  gsDebugMsg("Removing implementations of operators\n");
  ATermList ops = data_decls.ops;
  data_decls.ops = ATmakeList0();
  while (!ATisEmpty(ops))
  {
    ATermAppl OpId = ATAgetFirst(ops);
    if (!is_lambda_op_id(OpId)) {
      data_decls.ops = ATinsert(data_decls.ops, (ATerm) OpId);
    }
    ops = ATgetNext(ops);
  }
  data_decls.ops = ATreverse(data_decls.ops);

  // Additional processing of data equations
  gsDebugMsg("Removing data equations\n");
  ATermList data_eqns = data_decls.data_eqns;
  data_decls.data_eqns = ATmakeList0();
  while(!ATisEmpty(data_eqns))
  {
    ATermAppl DataEqn = ATAgetFirst(data_eqns);
    ATermAppl LHS = ATAgetArgument(DataEqn, 2);
    while (gsIsDataAppl(LHS)) {
      LHS = ATAgetArgument(LHS, 0);
    }
    if (!is_lambda_op_id(LHS)) {
      data_decls.data_eqns = ATinsert(data_decls.data_eqns, 
        (ATerm) DataEqn);
    }
    data_eqns = ATgetNext(data_eqns);
  }
  data_decls.data_eqns = ATreverse(data_decls.data_eqns);

  // Construct new DataSpec and Specification
  SortSpec    = gsMakeSortSpec   (data_decls.sorts);
  ConsSpec    = gsMakeConsSpec   (data_decls.cons_ops);
  MapSpec     = gsMakeMapSpec    (data_decls.ops);
  DataEqnSpec = gsMakeDataEqnSpec(data_decls.data_eqns);
  DataSpec    = gsMakeDataSpec   (SortSpec, ConsSpec, MapSpec, DataEqnSpec);
  Spec        = gsMakeSpecV1(DataSpec,
                             ATAgetArgument(Spec, 1),
                             ATAgetArgument(Spec, 2),
                             ATAgetArgument(Spec, 3));
  return Spec;
}

ATermAppl remove_headers_with_binders_from_spec(ATermAppl Spec, ATermAppl OrigSpec)
{
  assert(gsIsSpecV1(Spec));
  assert(gsIsSpecV1(OrigSpec));
  // Superfluous declarations are in DataSpec
  ATermAppl DataSpec = ATAgetArgument(Spec, 0);

  gsDebugMsg("Remove headers with binders: dissecting data specification\n");
  // Dissect Data specification
  ATermAppl SortSpec    = ATAgetArgument(DataSpec, 0);
  ATermAppl ConsSpec    = ATAgetArgument(DataSpec, 1);
  ATermAppl MapSpec     = ATAgetArgument(DataSpec, 2);
  ATermAppl DataEqnSpec = ATAgetArgument(DataSpec, 3);

  // Get the lists for data declarations
  gsDebugMsg("Remove headers with binders: retrieving data declarations\n");
  t_data_decls data_decls;
  data_decls.sorts     = ATLgetArgument(SortSpec, 0);
  data_decls.cons_ops  = ATLgetArgument(ConsSpec, 0);
  data_decls.ops       = ATLgetArgument(MapSpec, 0);
  data_decls.data_eqns = ATLgetArgument(DataEqnSpec, 0);

  for (ATermList sorts = data_decls.sorts; !ATisEmpty(sorts); sorts = ATgetNext(sorts))
  {
    ATermAppl sort = ATAgetFirst(sorts);
    if (gsIsSortExprSet(sort)) {
      remove_set_sort_from_data_decls(sort, &data_decls, OrigSpec);
    } else if (gsIsSortExprBag(sort)) {
      remove_bag_sort_from_data_decls(sort, &data_decls, OrigSpec);
    }
  }

  // Construct new DataSpec and Specification
  SortSpec    = gsMakeSortSpec   (data_decls.sorts);
  ConsSpec    = gsMakeConsSpec   (data_decls.cons_ops);
  MapSpec     = gsMakeMapSpec    (data_decls.ops);
  DataEqnSpec = gsMakeDataEqnSpec(data_decls.data_eqns);
  DataSpec    = gsMakeDataSpec   (SortSpec, ConsSpec, MapSpec, DataEqnSpec);
  Spec        = gsMakeSpecV1(DataSpec,
                             ATAgetArgument(Spec, 1),
                             ATAgetArgument(Spec, 2),
                             ATAgetArgument(Spec, 3));
  return Spec;
}

ATermAppl capture_avoiding_subst(ATermAppl Part, ATermAppl OldValue, 
                                 ATermAppl NewValue, ATermList* Context)
{
  gsDebugMsg("Performing capture avoiding substitution on %T with OldValue %T and NewValue %T\n",
             Part, OldValue, NewValue);

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

    if (gsIsBinder(LHS)) {
      Part = beta_reduce(Part, Context, false);
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
      }
    }
  }

  if (recursive) {
    // Recursively handle all parts of the expression.
    AFun head = ATgetAFun(DataExpr);
    int nr_args = ATgetArity(head);
    if (nr_args > 0) {
      DECL_A(args,ATerm,nr_args);
      for (int i = 0; i < nr_args; i++) {
        ATerm arg = ATgetArgument(DataExpr, i);
        if (ATgetType(arg) == AT_APPL)
          args[i] = (ATerm) beta_reduce_part((ATermAppl) arg, Context);
        else //ATgetType(arg) == AT_LIST
          args[i] = (ATerm) beta_reduce_list((ATermList) arg, Context);
      }
      DataExpr = ATmakeApplArray(head, args);
      FREE_A(args);
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

  return Part;
}

ATermList subtract_list_slice(ATermList l, ATermList m)
{
  ATermList list = ATmakeList0();
  ATerm first = ATgetFirst(m);
  if (ATindexOf(l, first, 0) != -1) {
    assert(ATgetLength(l) > ATgetLength(m));
    while(!ATisEqual(ATgetFirst(l), first))
    {
      list = ATinsert(list, ATgetFirst(l));
      l = ATgetNext(l);
    }
    assert(ATgetLength(l) > ATgetLength(m));
    while(!ATisEmpty(m))
    {
      assert(ATisEqual(ATgetFirst(l), ATgetFirst(m)));
      l = ATgetNext(l);
      m = ATgetNext(m);
    }
    while(!ATisEmpty(list))
    {
      l = ATinsert(l, ATgetFirst(list));
      list = ATgetNext(list);
    }
  }
  return l;
}

void remove_list_sort_from_data_decls(ATermAppl list_sort, t_data_decls* p_data_decls)
{
  assert(is_list_sort_id(list_sort));
  gsDebugMsg("Removing implementation of list sort %T from specification\n", list_sort);
  ATermAppl elt_sort = find_elt_sort_for_list_impl(list_sort, p_data_decls);

  ATermAppl sort_list = gsMakeSortExprList(elt_sort);
  t_data_decls list_decls;
  initialize_data_decls(&list_decls);
  ATermList dummy_substs = ATmakeList0(); // Needed in order to use impl_sort_list, 
                                          // but not used in this function
  ATermAppl impl_sort = impl_sort_list(sort_list, &dummy_substs, &list_decls);
  ATermList substs = ATmakeList1((ATerm) gsMakeSubst_Appl(impl_sort, list_sort));
  subst_values_list_data_decls(substs, &list_decls, true);
  subtract_data_decls(p_data_decls, &list_decls);
}

void remove_set_sort_from_data_decls(ATermAppl set_sort, t_data_decls* p_data_decls, ATermAppl spec)
{
  gsDebugMsg("Removing implementation of set sort %T from specification\n", set_sort);
  assert(gsIsSortExprSet(set_sort));
  assert(spec != NULL);
  assert(gsIsSpecV1(spec));
  
  t_data_decls set_decls;
  initialize_data_decls(&set_decls);
  ATermList dummy_substs = ATmakeList0();
  impl_sort_set(set_sort, &dummy_substs, &set_decls);
  // Add new data declarations to spec, this is needed in order to be able
  // to reconstruct expressions that have been introduced in the implementation
  spec = add_data_decls(spec, set_decls);

  set_decls.sorts = reconstruct_exprs_list(set_decls.sorts, spec);
  set_decls.cons_ops = reconstruct_exprs_list(set_decls.cons_ops, spec);
  set_decls.ops = reconstruct_exprs_list(set_decls.ops, spec);
  set_decls.data_eqns = reconstruct_exprs_list(set_decls.data_eqns, spec);

  subtract_data_decls(p_data_decls, &set_decls);
}

void remove_bag_sort_from_data_decls(ATermAppl bag_sort, t_data_decls* p_data_decls, ATermAppl spec)
{
  gsDebugMsg("Removing implementation of bag sort %T from specification\n", bag_sort);
  assert(gsIsSortExprBag(bag_sort));
  assert(spec != NULL);
  assert(gsIsSpecV1(spec));
 
  // Initialize implementation
  t_data_decls bag_decls;
  initialize_data_decls(&bag_decls);
  ATermList dummy_substs = ATmakeList0();
  impl_sort_bag(bag_sort, &dummy_substs, &bag_decls);

  // Add new data declarations to spec, this is needed in order to be able
  // to reconstruct expressions that have been introduced in the implementation
  spec = add_data_decls(spec, bag_decls);

  bag_decls.sorts = reconstruct_exprs_list(bag_decls.sorts, spec);
  bag_decls.cons_ops = reconstruct_exprs_list(bag_decls.cons_ops, spec);
  bag_decls.ops = reconstruct_exprs_list(bag_decls.ops, spec);
  bag_decls.data_eqns = reconstruct_exprs_list(bag_decls.data_eqns, spec);

  subtract_data_decls(p_data_decls, &bag_decls);
}

ATermAppl find_elt_sort_for_list_impl(ATermAppl list_impl_sort, t_data_decls* p_data_decls)
{
  gsDebugMsg("Finding element sort for list implementation %T\n", list_impl_sort);
  assert(is_list_sort_id(list_impl_sort));
  ATermAppl result = NULL;
  ATermList cons_ops = p_data_decls->cons_ops;
  while(result == NULL && !ATisEmpty(cons_ops))
  {
    ATermAppl cons_op = ATAgetFirst(cons_ops);
    assert(gsIsOpId(cons_op));
    if (ATisEqual(gsGetName(cons_op), gsMakeOpIdNameCons())) {
      ATermList sort_domain = ATLgetArgument(gsGetSort(cons_op), 0);
      if (ATisEqual(list_impl_sort, ATAelementAt(sort_domain, 1))) {
        result = ATAgetFirst(sort_domain);
      }
    }
    cons_ops = ATgetNext(cons_ops);
  }
  return result;
}

