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

#include "data_reconstruct.h"
#include "libstruct.h"
#include "libstruct_ir.h"
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

//pre: Spec is a Specication
//ret: Spec from which system defined functions and equations
//     have been removed.
static ATermAppl remove_headers_from_spec(ATermAppl Spec);

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
//ret: The beta reduced version of DataExpr.
static ATermAppl beta_reduce(ATermAppl DataExpr, ATermList* Context);

//pre: Context is the substitution context to use.
//ret: List with beta reduction performed on the parts for which it is appropriate.
static ATermList beta_reduce_list(ATermList List, ATermList* Context);

//pre: Context is the substitution context to use.
//ret: Part with beta reduction performed on the parts for which it is appropriate.
static ATermAppl beta_reduce_part(ATermAppl Part, ATermList* Context);


// implementation
// ----------------------------------------------
ATerm reconstruct_exprs(ATerm Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec));
  if (Spec == NULL) {
    gsVerboseMsg("No specification given, "
                 "therefore not all components can be reconstructed\n");
  } else {
    gsVerboseMsg("Specification provided, performing full reconstruction\n");
  }

  ATerm Result;
  if (ATgetType(Part) == AT_APPL) {
    Result = (ATerm) reconstruct_exprs_appl((ATermAppl) Part, Spec);
  } else if (ATgetType(Part) == AT_LIST) {
    Result = (ATerm) reconstruct_exprs_list((ATermList) Part, Spec);
  } else {
    assert(false);
  }
  Result = beta_reduce_term(Result);
  gsVerboseMsg("Finished data reconstruction\n");
  return Result;
}

ATermAppl reconstruct_exprs_appl(ATermAppl Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec));

  if(gsIsSpecV1(Part) && (Spec != NULL)) {
    gsVerboseMsg("Removing headers from specification\n");
    Part = remove_headers_from_spec(Part);
  }

  bool recursive = true;
  if (gsIsConsSpec(Part) || gsIsMapSpec(Part)) {
    //recursive = false;
  } else if (gsIsDataExprBagComp(Part)) {
    gsDebugMsg("Reconstructing implementation of bag comprehension\n");
    //part is an implementation of a bag comprehension;
    //replace by a bag comprehension.
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl Body = ATAelementAt(Args, 0);
    ATermList Vars = ATmakeList0();
    ATermList BodySortDomain = ATLgetArgument(gsGetSort(Body), 0);
    // The length of the Domain should be 1 (enforced by constructor)
    assert(ATgetLength(BodySortDomain) == 1);
    ATermAppl Var = gsMakeDataVarId(
                      gsFreshString2ATermAppl("x", (ATerm) Body, true),
                      ATAgetFirst(BodySortDomain));
    Vars = ATinsert(Vars, (ATerm) Var);
    Body = gsMakeDataAppl1(Body, Var);
    Part = gsMakeBinder(gsMakeBagComp(),Vars, Body);
  }
  else if (gsIsDataExprSetComp(Part)) {
    gsDebugMsg("Reconstructing implementation of set comprehension\n");
    //part is an implementation of a set comprehension;
    //replace by a set comprehension.
    ATermList Args = ATLgetArgument(Part, 1);
    ATermAppl Body = ATAelementAt(Args, 0);
    ATermList Vars = ATmakeList0();
    ATermList BodySortDomain = ATLgetArgument(gsGetSort(Body), 0);
    assert(ATgetLength(BodySortDomain) == 1);
    ATermAppl Var = gsMakeDataVarId(
                      gsFreshString2ATermAppl("x", (ATerm) Body, true),
                      ATAgetFirst(BodySortDomain));
    Vars = ATinsert(Vars, (ATerm) Var);
    Body = gsMakeDataAppl1(Body, Var);
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
    //assert(false);
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
    recursive = false;
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
            if(!gsOccurs((ATerm) Var, (ATerm) BoundVars)) {
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

bool is_lambda_expr(ATermAppl Part)
{
  if(gsIsDataAppl(Part)) {
    return is_lambda_op_id(ATAgetArgument(Part, 0));
  } else {
    return false;
  }
}

ATermAppl remove_headers_from_spec(ATermAppl Spec)
{
  gsDebugMsg("Removing headers from specification\n");
  assert(gsIsSpecV1(Spec));
  // Superfluous declarations are in MapSpec and DataEqnSpec
  ATermAppl DataSpec = ATAgetArgument(Spec, 0);
  ATermAppl MapSpec = ATAgetArgument(DataSpec, 2);
  ATermAppl DataEqnSpec = ATAgetArgument(DataSpec, 3);
  
  // Process MapSpec
  ATermList OpIds = ATLgetArgument(MapSpec, 0);
  ATermList NewOpIds = ATmakeList0();
  gsDebugMsg("Removing headers from MapSpec\n");
  while (!ATisEmpty(OpIds))
  {
    ATermAppl OpId = ATAgetFirst(OpIds);
    if(!is_lambda_op_id(OpId)) {
      NewOpIds = ATinsert(NewOpIds, (ATerm) OpId);
    } else {
      gsDebugMsg("Removing lambda map %T\n", OpId);
    }
    OpIds = ATgetNext(OpIds);
  }
  NewOpIds = ATreverse(NewOpIds);
  MapSpec = gsMakeMapSpec(NewOpIds);

  // Process DataEqnSpec
  ATermList DataEqns = ATLgetArgument(DataEqnSpec, 0);
  ATermList NewDataEqns = ATmakeList0();
  gsDebugMsg("Removing headers from DataEqnSpec\n");
  while (!ATisEmpty(DataEqns)) {
    ATermAppl DataEqn = ATAgetFirst(DataEqns);
    ATermAppl LHS = ATAgetArgument(DataEqn, 2);
    while (gsIsDataAppl(LHS)) {
      LHS = ATAgetArgument(LHS, 0);
    }
    if (!is_lambda_op_id(LHS)) {
      NewDataEqns = ATinsert(NewDataEqns, (ATerm) DataEqn);
    } else {
      gsDebugMsg("Removing lambda equation %T\n", DataEqn);
    }
    DataEqns = ATgetNext(DataEqns);
  }
  NewDataEqns = ATreverse(NewDataEqns);
  DataEqnSpec = gsMakeDataEqnSpec(NewDataEqns);

  // Construct new DataSpec and Specification
  DataSpec = gsMakeDataSpec(ATAgetArgument(DataSpec, 0),
                            ATAgetArgument(DataSpec, 1),
                            MapSpec,
                            DataEqnSpec);

  Spec = gsMakeSpecV1(DataSpec,
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
    return Part;
    //return gsMakeDataAppl(LHS, NewRHS);
  } else if (gsIsBinder(Part)) {
    if (gsOccurs((ATerm) OldValue, ATgetArgument(Part, 1))) {
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
        if (gsOccurs((ATerm) Var, (ATerm) FreeVars)) {
          // Bound variable also accours as free variable in Expr,
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

ATermAppl beta_reduce(ATermAppl DataExpr, ATermList* Context)
{
  // If the expression is a lambda expression perform beta reduction, i.e.
  // replace (lambda x_0, ..., x_n . e)(e_0, ..., e_n) with 
  // e[e_0/x_0][...][e_n/x_n]
  assert(gsIsDataExpr(DataExpr));
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl LHS = ATAgetArgument(DataExpr, 0);
    ATermList RHS = ATLgetArgument(DataExpr, 1);
    if (gsIsBinder(LHS)) {
      ATermAppl BindingOp = ATAgetArgument(LHS, 0);
      ATermList Vars = ATLgetArgument(LHS, 1);
      ATermAppl Expr = ATAgetArgument(LHS, 2);
      if (gsIsLambda(BindingOp)) {
        // Note that first the right hand side needs to be done because of the
        // substitution order!
        RHS = beta_reduce_list(RHS, Context);
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
  bool recursive = true;

  if (gsIsConsSpec(Part) || gsIsMapSpec(Part)) {
    recursive = false;
  } else if (gsIsDataExpr(Part)) {
    Part = beta_reduce(Part, Context);
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
          args[i] = (ATerm) beta_reduce_part((ATermAppl) arg, Context);
        else //ATgetType(arg) == AT_LIST
          args[i] = (ATerm) beta_reduce_list((ATermList) arg, Context);
      }
      Part = ATmakeApplArray(head, args);
      FREE_A(args);
    }
  }

  return Part;
}

