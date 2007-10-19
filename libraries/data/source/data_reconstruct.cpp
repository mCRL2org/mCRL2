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
#include "atermpp/indexed_set.h"
#include "atermpp/map.h"
#include "atermpp/table.h"

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

//pre: Part is a data expression
//ret: The reconstructed version of Part.
static ATermAppl reconstruct_data_expr(ATermAppl Part, const ATermAppl Spec);

//pre: Part is a sort expression
//ret: The reconstructed version of Part.
static ATermAppl reconstruct_sort_expr(ATermAppl Part, const ATermAppl Spec);

//TODO: Describe prototype
static ATermAppl reconstruct_pos_mult(ATermAppl PosExpr, char const* Mult);

//pre: OpId is an OpId of the form lambda@x (with x a natural number),
//     Spec is a SpecV1
//ret: The lambda expression from which Part originates.
static ATermAppl reconstruct_lambda_op(const ATermAppl Part, const ATermAppl Spec);

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
static inline bool is_lambda_expr(const ATermAppl Part);

//pre: Spec is a Specification
//ret: Spec from which system defined functions and equations
//     have been removed.
static ATermAppl remove_headers_without_binders_from_spec(ATermAppl Spec, ATermList* p_substs);

//pre: Spec is a Specification after data reconstruction
//ret: Spec from which set and bag functions have been removed.
static ATermAppl remove_headers_with_binders_from_spec(ATermAppl Spec, ATermAppl OrigSpec, ATermList* p_substs);

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

//post: p_data_decls constains the reconstructed versions of the structured
//      sorts in p_data_decls.
//      p_substs is extended with the substitutions induced by the context of
//      the structured sorts.
static void reconstruct_structured_sorts(t_data_decls* p_data_decls, ATermList* p_substs);

//pre: data_expr is a data expression
//ret: true if data_expr is of the form v_1 == w_1 && ... && v_n == w_n, where
//     v_i, w_j are data variables,
//     false, otherwise
static bool is_and_of_data_var_equalities(ATermAppl data_expr);

//ret: true if all elements of l are DataVarIds,
//     false otherwise
static bool is_list_of_data_var_ids(ATermList l);

static ATermList filter_table_elements_from_list(ATermList l, atermpp::table& t);

//pre: data_expr is a data expression
//ret: true if data_expr is a function on lists
static bool is_list_operator(ATermAppl data_expr);

//pre: data_eqn is a data equation, sort_constructors holds the constructors
//     for all relevant sorts.
//ret: true if data_eqn has the form such that it belongs to a constructor
//     false otherwise.
static bool is_constructor_induced_equation(ATermAppl data_eqn, atermpp::map<ATermAppl, atermpp::indexed_set>& sort_constructors);

static bool is_list_equation(ATermAppl data_eqn);

static bool is_recogniser_equation(ATermAppl data_eqn);

static bool is_projection_equation(ATermAppl data_eqn);

static void remove_mapping_not_list(ATermAppl op,
                        ATermAppl sort,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& sort_mappings,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& map_equations,
                        atermpp::map<ATermAppl, int>& num_map_equations);

// implementation
// ----------------------------------------------
ATerm reconstruct_exprs(ATerm Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));
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
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));
  ATermList substs = ATmakeList0();

  if((gsIsSpecV1(Part) || gsIsPBES(Part)) && (Spec != NULL)) {
    gsDebugMsg("Removing headers from specification\n");
    Part = remove_headers_without_binders_from_spec(Part, &substs);
  }

  if (gsIsDataExpr(Part)) {
    // Reconstruct Data Expressions
    Part = reconstruct_data_expr(Part, Spec);
  } else if (gsIsSortExpr(Part)) {
    Part = reconstruct_sort_expr(Part, Spec);
  }

  // Reconstruct expressions in the arguments of part
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

  if ((Spec != NULL) && gsIsDataAppl(Part)) {
    // Beta reduction if possible
    Part = (ATermAppl) beta_reduce_term((ATerm) Part);
  }

  // For set and bag enumerations to be reconstructed,
  // recursive reconstruction and beta reduction must have already been
  // performed. This is the case at this point, so do this reconstruction here.
  if (gsIsDataExprSetComp(Part) || gsIsDataExprBagComp(Part)) {
    Part = reconstruct_set_bag_enum(Part);
  }

  if ((gsIsSpecV1(Part) || gsIsPBES(Part)) && Spec != NULL) {
    Part = remove_headers_with_binders_from_spec(Part, Spec, &substs);
    // Perform substitutions
    Part = gsSubstValues_Appl(substs, Part, true);
  }

  return Part;
}

ATermList reconstruct_exprs_list(ATermList Parts, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));

  ATermList result = ATmakeList0();
  while (!ATisEmpty(Parts))
  {
    result = ATinsert(result, (ATerm)
      reconstruct_exprs_appl(ATAgetFirst(Parts), Spec));
    Parts = ATgetNext(Parts);
  }
  return ATreverse(result);
}

ATermAppl reconstruct_data_expr(ATermAppl Part, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));
  assert(gsIsDataExpr(Part));
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
    Body = (ATermAppl) beta_reduce_term((ATerm) Body);
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
    Body = (ATermAppl) beta_reduce_term((ATerm) Body);
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
    gsDebugMsg("Reconstructing implementation of %T\n", Part);
    Part = gsMakeOpId(gsString2ATermAppl("0"), gsMakeSortExprNat());
  } else if (gsIsDataExprCNat(Part) || gsIsDataExprPos2Nat(Part)) {
    gsDebugMsg("Reconstructing implementation of CNat or Pos2Nat (%T)\n", Part);
    ATermAppl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_exprs_appl(value, Spec);
    Part = gsMakeDataExprPos2Nat(value);
    if (gsIsOpId(value)) {
      ATermAppl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name))) {
        Part = gsMakeOpId(name, gsMakeSortExprNat());
      }
    }
  } else if (gsIsDataExprCPair(Part)) {
    gsDebugMsg("Currently not reconstructing implementation of CPair (%T)\n", Part);
  } else if (gsIsDataExprCNeg(Part)) {
    gsDebugMsg("Reconstructing implementation of CNeg (%T)\n", Part);
    Part = gsMakeDataExprNeg(ATAgetFirst(ATLgetArgument(Part, 1)));
  } else if (gsIsDataExprCInt(Part) || gsIsDataExprNat2Int(Part)) {
    gsDebugMsg("Reconstructing implementation of CInt or Nat2Int (%T)\n", Part);
    ATermAppl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_exprs_appl(value, Spec);
    Part = gsMakeDataExprNat2Int(value);
    if (gsIsOpId(value)) {
      ATermAppl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name))) {
        Part = gsMakeOpId(name, gsMakeSortExprInt());
      }
    }
  } else if (gsIsDataExprCReal(Part) || gsIsDataExprInt2Real(Part)) {
    gsDebugMsg("Reconstructing implementation of CReal or Int2Real (%T)\n", Part);
    ATermAppl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_exprs_appl(value, Spec);
    Part = gsMakeDataExprInt2Real(value);
    if (gsIsOpId(value)) {
      ATermAppl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name))) {
        Part = gsMakeOpId(name, gsMakeSortExprReal());
      }
    }
  } else if (gsIsDataExprDub(Part)) {
    gsDebugMsg("Reconstructing implementation of Dub (%T)\n", Part);
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
      Part = gsMakeDataExprAdd(Mult, bool_to_numeric(BoolArg, Sort));
    }
  } else if (gsIsDataExprAddC(Part)) {
    gsDebugMsg("Reconstructing implementation of AddC (%T)\n", Part);
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
      Part = gsMakeDataExprAdd(Sum, bool_to_numeric(BoolArg, Sort));
    }
  } else if (gsIsDataExprGTESubt(Part)) {
    gsDebugMsg("Reconstructing implementation of GTESubt (%T)\n", Part);
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
    gsDebugMsg("Reconstructing implementation of GTESubtB (%T)\n", Part);
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
      Part = gsMakeDataExprSubt(Part, bool_to_numeric(BoolArg, Sort));
    }
    Part = gsMakeDataExprInt2Nat(Part);
  } else if (gsIsDataExprMultIR(Part)) {
    gsDebugMsg("Reconstructing implementation of MultIR (%T)\n", Part);
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
               gsMakeDataExprMult(bool_to_numeric(Bit, gsMakeSortExprPos()), IR),
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
    gsDebugMsg("Reconstructing implementation of even (%T)\n", Part);
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

ATermAppl reconstruct_sort_expr(ATermAppl Part, const ATermAppl Spec)
{
  assert(gsIsSortExpr(Part));
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));

  // Reconstruct sort expressions
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
    while (!ATisEmpty(ops) && !found) {
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
    while (!ATisEmpty(ops) && !found) {
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
  } /*
  else if (gsIsSortId(Part)) {
    ATermAppl cons_spec = ATAgetArgument(ATAgetArgument(Spec, 0), 1);
    ATermList cons_ops = ATLgetArgument(cons_spec, 0);
    bool empty_list = false;
    bool cons = false;
    ATermList sort_domain = ATmakeList0();
    while (!ATisEmpty(cons_ops) && !empty_list && !cons) {
      ATermAppl cons_op = ATAgetFirst(cons_ops);
      if (ATisEqual(cons_op, gsMakeOpIdEmptyList(Part))) {
        empty_list = true;
      } else if (ATisEqual(gsGetName(cons_op), gsMakeOpIdNameCons())) {
        sort_domain = ATLgetArgument(gsGetSort(cons_op), 0);
        if (ATisEqual(Part, ATAelementAt(sort_domain, 1))) {
          cons = true;
        }
        cons_ops = ATgetNext(cons_ops);
      }
    }
    if (cons && empty_list) {
      Part = gsMakeSortExprList(ATAgetFirst(ATgetNext(sort_domain)));
    }
  }
  */
  return Part;
}

ATermAppl reconstruct_pos_mult(ATermAppl PosExpr, char const* Mult)
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
        return gsMakeDataExprAdd(PosArg, 
                 gsMakeOpId(gsString2ATermAppl(Mult), gsMakeSortExprPos()));
      } else if (strcmp(Mult, "1") == 0) {
        //Mult*v(b) = v(b)
        return gsMakeDataExprAdd(PosArg, bool_to_numeric(BoolArg, gsMakeSortExprNat()));
      } else {
        //Mult*v(b)
        return gsMakeDataExprAdd(PosArg, 
                 gsMakeDataExprMult(gsMakeOpId(gsString2ATermAppl(Mult), 
                                      gsMakeSortExprPos()), 
                                    bool_to_numeric(BoolArg, gsMakeSortExprNat())));
      }
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

ATermAppl reconstruct_lambda_op(const ATermAppl Part, const ATermAppl Spec)
{
  assert(gsIsSpecV1(Spec) || gsIsPBES(Spec));
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
          //There are free variables in the expression, construct an additional 
          //lambda expression
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

ATermAppl reconstruct_set_bag_enum(ATermAppl data_expr)
{
  assert (gsIsDataExprSetComp(data_expr) || gsIsDataExprBagComp(data_expr));
  ATermAppl body = ATAgetArgument(data_expr, 2);
  ATermList enumeration = ATmakeList0();
  if (gsIsDataExprSetComp(data_expr)) {
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

bool is_lambda_expr(const ATermAppl Part)
{
  if(gsIsDataAppl(Part)) {
    return is_lambda_op_id(ATAgetArgument(Part, 0));
  } else {
    return false;
  }
}

ATermAppl remove_headers_without_binders_from_spec(ATermAppl Spec, ATermList* p_substs)
{
  gsDebugMsg("Removing headers from specification\n");
  assert(gsIsSpecV1(Spec) || gsIsPBES(Spec));
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

  atermpp::table sorts_table(15, 75);
  atermpp::table ops_table(100, 75);
  atermpp::table data_eqns_table(100, 75);

  atermpp::table superfluous_sorts(10, 75);
  atermpp::table superfluous_cons_ops(100, 75);
  atermpp::table superfluous_ops(100,75);
  atermpp::table superfluous_data_eqns(100,75);

  for (ATermList l = data_decls.sorts; !ATisEmpty(l); l = ATgetNext(l)) {
    sorts_table.put(ATgetFirst(l), (ATerm) ATtrue);
  }
  for (ATermList l = data_decls.ops; !ATisEmpty(l); l = ATgetNext(l)) {
    ops_table.put(ATgetFirst(l), (ATerm) ATtrue);
  }
  for (ATermList l = data_decls.data_eqns; !ATisEmpty(l); l = ATgetNext(l)) {
    data_eqns_table.put(ATgetFirst(l), (ATerm) ATtrue);
  }

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
      is_function_sort_impl = (ops_table.get(ATgetFirst(ops)) != NULL);
    }
    for(ATermList data_eqns = function_decls.data_eqns; is_function_sort_impl && !(ATisEmpty(data_eqns));
        data_eqns = ATgetNext(data_eqns))
    {
      is_function_sort_impl = (data_eqns_table.get(ATgetFirst(data_eqns)) != NULL);
    }

    if (is_function_sort_impl) {
      while(!ATisEmpty(function_decls.ops)) {
        superfluous_ops.put(ATAgetFirst(function_decls.ops), (ATerm) ATtrue);
        function_decls.ops = ATgetNext(function_decls.ops);
      }
      while (!ATisEmpty(function_decls.data_eqns)) {
        superfluous_data_eqns.put(ATAgetFirst(function_decls.data_eqns), (ATerm) ATtrue);
        function_decls.data_eqns = ATgetNext(function_decls.data_eqns);
      }
    }
    function_sorts = ATgetNext(function_sorts);
  }

  // Construct lists of data declarations for system defined sorts
  t_data_decls data_decls_impl;
  initialize_data_decls(&data_decls_impl);

  gsDebugMsg("Removing system defined sorts from data declarations\n");
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

  //subtract_data_decls(&data_decls, &data_decls_impl);
  data_decls.sorts = filter_table_elements_from_list(data_decls.sorts, superfluous_sorts);
  data_decls.cons_ops = filter_table_elements_from_list(data_decls.cons_ops, superfluous_cons_ops);
  data_decls.ops = filter_table_elements_from_list(data_decls.ops, superfluous_ops);
  data_decls.data_eqns = filter_table_elements_from_list(data_decls.data_eqns, superfluous_data_eqns);

  // Additional processing of data declarations by manually recognising
  // system defined sorts, operators and data equations
  // these are removed from their respective parts of the data declarations
  // on the fly.

  // Additional processing of Sorts
  /*
  for (ATermList sorts = data_decls.sorts; !ATisEmpty(sorts); sorts = ATgetNext(sorts))
  {
    ATermAppl sort = ATAgetFirst(sorts);
    if (is_list_sort_id(sort)) {
      remove_list_sort_from_data_decls(sort, &data_decls);
    }
  }
  */

  reconstruct_structured_sorts(&data_decls, p_substs);

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
  if (gsIsSpecV1(Spec)) {
    Spec        = gsMakeSpecV1(DataSpec,
                               ATAgetArgument(Spec, 1),
                               ATAgetArgument(Spec, 2),
                               ATAgetArgument(Spec, 3));
  } else { //gsIsPBES(Spec)
    Spec = gsMakePBES(DataSpec,
                      ATAgetArgument(Spec, 1),
                      ATAgetArgument(Spec, 2));
  }
  return Spec;
}

ATermAppl remove_headers_with_binders_from_spec(ATermAppl Spec, ATermAppl OrigSpec, ATermList* p_substs)
{
  assert((gsIsSpecV1(Spec) && gsIsSpecV1(OrigSpec))
    || (gsIsPBES(Spec) && gsIsPBES(OrigSpec)));

  // DataSpec is the same argument for SpecV1 as well as PBES!
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
  if (gsIsSpecV1(Spec)) {
    Spec        = gsMakeSpecV1(DataSpec,
                               ATAgetArgument(Spec, 1),
                               ATAgetArgument(Spec, 2),
                               ATAgetArgument(Spec, 3));
  } else { //gsIsPBES(Spec)
    Spec = gsMakePBES(DataSpec,
                      ATAgetArgument(Spec, 1),
                      ATAgetArgument(Spec, 2));
  }
  return Spec;
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
  assert(gsIsSpecV1(spec) || gsIsPBES(spec));

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
  assert(gsIsSpecV1(spec) || gsIsPBES(spec));

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

void reconstruct_structured_sorts(t_data_decls* p_data_decls, ATermList* p_substs)
{
  assert(p_substs != NULL);
  gsDebugMsg("Reconstructing structured sorts\n");
  atermpp::table struct_sorts_table(10,50);
  // for each sort the constructors for that sort
  atermpp::map<ATermAppl, atermpp::indexed_set> sort_constructors;
  // inv: num_sort_constructors[sort] == |sort_constructors[sort]|
  atermpp::map<ATermAppl, int>             num_sort_constructors;
  // for each sort the (possibly relevant) maps for that sort
  atermpp::map<ATermAppl, atermpp::indexed_set> sort_mappings;
  // inv: num_sort_mappings[sort] == |sort_mappings[sort]|
  atermpp::map<ATermAppl, int>             num_sort_mappings;
  // for each sort the data equations induced by the constructors of that sort
  atermpp::map<ATermAppl, atermpp::indexed_set> sort_cons_equations;
  // inv: num_sort_cons_equations[sort] == |sort_cons_equations[sort]|
  atermpp::map<ATermAppl, int>             num_sort_cons_equations;
  //  for each mapping, if it is (possibly) a constructor, the equations for
  //  that mapping
  atermpp::map<ATermAppl, atermpp::indexed_set> map_equations;
  // inv: num_map_equations[map] == |map_equations[map]|
  atermpp::map<ATermAppl, int>             num_map_equations;
  // for each mapping f that is a recogniser, recognises[f] = the constructor
  // for which f is the recogniser
  atermpp::map<ATermAppl, ATermAppl>       recognises;
  atermpp::map<std::pair<ATermAppl, int>, ATermAppl> projects;
  std::pair<long, bool> put_result; // result of atermpp::indexed_set::put()

  // Initialisation
  for (ATermList l = p_data_decls->sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    struct_sorts_table.put         (sort, (ATerm) ATtrue);
    sort_constructors.insert       (std::make_pair(sort, atermpp::indexed_set(20,50)));
    num_sort_constructors.insert   (std::make_pair(sort, 0));
    sort_mappings.insert           (std::make_pair(sort, atermpp::indexed_set(20,50)));
    num_sort_mappings.insert       (std::make_pair(sort, 0));
    sort_cons_equations.insert     (std::make_pair(sort, atermpp::indexed_set(20,50)));
    num_sort_cons_equations.insert (std::make_pair(sort, 0));
  }

  // Initialise sort_constructors, such that for each sort s in
  // p_data_decls->sorts sort_constructors(s) contains exactly the
  // constructors of s.
  for (ATermList l = p_data_decls->cons_ops; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl constructor = ATAgetFirst(l);
    ATermAppl sort = gsGetSortExprResult(gsGetSort(constructor));
    if((struct_sorts_table.get(sort) != NULL)) {
      // Else we are dealing with a system defined sort
      assert(gsIsOpId(constructor));
      put_result = sort_constructors[sort].put(constructor);
      if (put_result.second) {
        num_sort_constructors[sort]++;
      }
    }
  }

  // Traverse mappings to find possible recogniser and projection functions.
  for (ATermList l = p_data_decls->ops; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl op = ATAgetFirst(l);
    ATermAppl sort = gsGetSort(op);
    if (gsIsSortArrow(sort)) {
      ATermList sort_domain = ATLgetArgument(sort, 0);
      ATermAppl sort_range = gsGetSortExprResult(sort);
      ATermAppl dom_sort = ATAgetFirst(sort_domain);
      if (ATisEqual(gsMakeOpIdNameIf(), gsGetName(op)) ||
          ATisEqual(gsMakeOpIdNameEltIn(), gsGetName(op))) {
        dom_sort = ATAgetFirst(ATgetNext(sort_domain));
      }
      if (gsIsOpId(op) && (struct_sorts_table.get(dom_sort) != NULL)) {
        if (ATisEqual(gsMakeOpIdIf(dom_sort), op)
          || ATisEqual(gsMakeOpIdEq(dom_sort), op)
          || ATisEqual(gsMakeOpIdNeq(dom_sort), op)
          || is_list_operator(op)
          || (ATgetLength(sort_domain) == 1 && sort_range == gsMakeSortExprBool())
          || (ATgetLength(sort_domain) == 1  /*TODO: strengthen condition for projs*/)) {
          put_result = sort_mappings[dom_sort].put(op);
          if (put_result.second) {
            num_sort_mappings[dom_sort]++;
            map_equations.insert(std::make_pair(op, atermpp::indexed_set(6, 50)));
            num_map_equations.insert(std::make_pair(op, 0));
          }
        }
      }
    }
  }

  gsDebugMsg("Inititialization done, traverse equations\n");

  // Traverse data equations to identify structured sorts
  for (ATermList data_eqns = p_data_decls->data_eqns; !ATisEmpty(data_eqns);
       data_eqns = ATgetNext(data_eqns))
  {
    ATermAppl data_eqn = ATAgetFirst(data_eqns);
    ATermAppl data_eqn_lhs = ATAgetArgument(data_eqn, 2);
    ATermAppl data_eqn_rhs = ATAgetArgument(data_eqn, 3);
    if (gsIsDataExprNeq(data_eqn_lhs)) {
    // Special case, inequality: x != y == !(x==y)
      ATermAppl op_sort = gsGetSort(ATAgetArgument(data_eqn_lhs, 0));
      ATermAppl sort = ATAgetFirst(ATLgetArgument(op_sort, 0));
      if (struct_sorts_table.get(sort) != NULL) {
        ATermList args = ATLgetArgument(data_eqn_lhs, 1);
        ATermAppl arg0 = ATAgetFirst(args);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(args));
        if (gsIsDataVarId(arg0) && gsIsDataVarId(arg1)) {
          ATermAppl neq = gsMakeDataExprNot(gsMakeDataExprEq(arg0, arg1));
          if (ATisEqual(data_eqn_rhs, neq)) {
            assert(gsIsDataEqn(data_eqn));
            ATermAppl map = gsMakeOpIdNeq(sort);
            assert(sort_mappings[sort].index(map) >= 0);
            put_result = map_equations[map].put(data_eqn);
            if (put_result.second) {
              num_map_equations[map]++;
            }
          }
        }
      }
    } else if (gsIsDataExprIf(data_eqn_lhs)) {
    // Special case, if: if(true, x, y) == x
    //                   if(false, x, y) == y
    //                   if(b, x, x) == x
      ATermAppl op_sort = gsGetSort(ATAgetArgument(data_eqn_lhs, 0));
      // op_sort is Bool x S x S -> S, find out S
      ATermAppl sort = ATAgetFirst(ATgetNext(ATLgetArgument(op_sort, 0)));
      if (struct_sorts_table.get(sort) != NULL) {
        ATermList args = ATLgetArgument(data_eqn_lhs, 1);
        ATermAppl arg0 = ATAgetFirst(args);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(args));
        ATermAppl arg2 = ATAgetFirst(ATgetNext(ATgetNext(args)));
        if ((gsIsDataExprTrue(arg0) && ATisEqual(data_eqn_rhs, arg1))
          ||(gsIsDataExprFalse(arg0) && ATisEqual(data_eqn_rhs, arg2))
          ||(gsIsDataVarId(arg0) && ATisEqual(arg1, arg2)
             && ATisEqual(data_eqn_rhs, arg1))) {
          assert(gsIsDataEqn(data_eqn));
          ATermAppl map = gsMakeOpIdIf(sort);
          assert(sort_mappings[sort].index(map) >= 0);
          put_result = map_equations[map].put(data_eqn);
          if (put_result.second) {
            num_map_equations[map]++;
          }
        }
      }
    } else if (gsIsDataExprEq(data_eqn_lhs)) {
      ATermAppl op_sort = gsGetSort(ATAgetArgument(data_eqn_lhs, 0));
      ATermAppl sort = ATAgetFirst(ATLgetArgument(op_sort, 0));
      ATermList args = ATLgetArgument(data_eqn_lhs, 1);
      ATermAppl arg0 = ATAgetFirst(args);
      ATermAppl arg1 = ATAgetFirst(ATgetNext(args));
      assert(gsIsDataExpr(arg0) && gsIsDataExpr(arg1));
      // Special case, data_eqn is of the form var == var = true;
      if ((gsIsDataVarId(arg0) && gsIsDataVarId(arg1) && ATisEqual(arg0, arg1) &&
          ATisEqual(data_eqn_rhs, gsMakeDataExprTrue()))
        || is_constructor_induced_equation(data_eqn, sort_constructors)) {
        put_result = sort_cons_equations[sort].put(data_eqn);
        if (put_result.second) {
          num_sort_cons_equations[sort]++;
        }
      }
    } else if (gsIsDataAppl(data_eqn_lhs)) {
      // data_eqn may be an equation for a recogniser function
      ATermAppl head = ATAgetArgument(data_eqn_lhs, 0);
      // args should be of length 1 for a recogniser function.
      // say, arg0;
      ATermList args = ATLgetArgument(data_eqn_lhs, 1);
      ATermAppl arg0 = ATAgetFirst(args);
      //TODO: clean this up a bit
      if (gsIsDataAppl(arg0)) {
        args = ATLgetArgument(arg0, 1);
        arg0 = ATAgetArgument(arg0, 0);
      } else if (gsIsOpId(arg0)) {
        args = ATmakeList0();
      }
      ATermAppl head_sort = gsGetSort(head);
      assert(gsIsSortArrow(head_sort));
      ATermAppl sort = ATAgetFirst(ATLgetArgument(head_sort, 0));

      if (is_recogniser_equation(data_eqn)) {
        if((struct_sorts_table.get(sort) != NULL) &&
          (sort_constructors[sort].index(arg0) >= 0) &&
          (sort_mappings[sort].index(head) >= 0)) {
          put_result = map_equations[head].put(data_eqn);
          if (put_result.second) {
            num_map_equations[head]++;
          }
          if (ATisEqual(data_eqn_rhs, gsMakeDataExprTrue())) {
            recognises[head] = arg0;
          }
        } else {
          recognises.erase(head);
          remove_mapping_not_list(head, sort, sort_mappings, map_equations, num_map_equations);
        }
      } else if (is_projection_equation(data_eqn) &&
                 (struct_sorts_table.get(sort) != NULL) &&
                 (sort_constructors[sort].index(arg0) >= 0) &&
                 (sort_mappings[sort].index(head) >= 0)) {
        /* head could be a projection function for arg0 */
        if (num_map_equations[head] > 0) {
          projects.erase(std::make_pair(arg0, ATindexOf(args, (ATerm) data_eqn_rhs, 0)));
          remove_mapping_not_list(head, sort, sort_mappings, map_equations, num_map_equations);
        } else {
          put_result = map_equations[head].put(data_eqn);
          projects[std::make_pair(arg0, ATindexOf(args, (ATerm) data_eqn_rhs, 0))] = head;
          if (put_result.second) {
            num_map_equations[head]++;
          }
        }
      } else if (is_list_equation(data_eqn)) {
        put_result = map_equations[head].put(data_eqn);
        if (put_result.second) {
          num_map_equations[head]++;
        }
      } else {
        recognises.erase(head);
        if(!is_list_operator(head)) {
          remove_mapping_not_list(head, sort, sort_mappings, map_equations, num_map_equations);
          /*
          map_equations.erase(head);
          num_map_equations.erase(head);
          if ((struct_sorts_table.get(sort) != NULL) &&
            sort_mappings[sort].index(head) >= 0 ) {
            sort_mappings[sort].remove(head);
          }
          */
        }
      }
    }
  }

  gsDebugMsg("Determined equations\n");

  // Check for completeness and remove structured sort from data declarations
  ATermList non_struct_sorts = ATmakeList0();
  for (ATermList l = p_data_decls->sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    if ((num_sort_cons_equations[sort] != ((num_sort_constructors[sort] * num_sort_constructors[sort]) + 1))) {
      non_struct_sorts = ATinsert(non_struct_sorts, (ATerm) sort);
      struct_sorts_table.remove(sort);
      sort_constructors.erase(sort);
      num_sort_constructors.erase(sort);
      sort_mappings.erase(sort);
      num_sort_mappings.erase(sort);
      sort_cons_equations.erase(sort);
      num_sort_cons_equations.erase(sort);
    }
  }
  p_data_decls->sorts = ATreverse(non_struct_sorts);
  ATermList struct_sorts = struct_sorts_table.table_keys();

  gsDebugMsg("Determining recognisers\n");

  // Check for equations that have sufficient equations to be a recogniser
  // or projection function
  // and schedule the equations and mappings for these for removal
  atermpp::map<ATermAppl, ATermAppl> is_recognised_by;
  for (ATermList l = struct_sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    assert(sort_mappings.count(sort) == 1);
    ATermList maps = sort_mappings[sort].elements();
    while(!ATisEmpty(maps)) {
      ATermAppl map = ATAgetFirst(maps);
      assert(gsIsOpId(map));
      if (!ATisEqual(gsMakeOpIdNameIf(), gsGetName(map)) &&
          !ATisEqual(gsMakeOpIdNameEq(), gsGetName(map)) &&
          !ATisEqual(gsMakeOpIdNameNeq(), gsGetName(map))) {
        // map is possibly a recogniser
        assert(gsIsSortArrow(gsGetSort(map)));
        //assert(ATisEqual(ATAgetFirst(ATLgetArgument(gsGetSort(map), 0)), sort));
        if(ATisEqual(gsGetSortExprResult(gsGetSort(map)), gsMakeSortExprBool())) {
          if(num_map_equations[map] != num_sort_constructors[sort]) {
            remove_mapping_not_list(map, sort, sort_mappings, map_equations, num_map_equations);
          } else {
            is_recognised_by[recognises[map]] = map;
          }
        } else if (num_map_equations[map] != 1) {
          remove_mapping_not_list(map, sort, sort_mappings, map_equations, num_map_equations);
        }
      }
      maps = ATgetNext(maps);
    }
  }

  // At this point, all sorts that are still in sort_cons_equations and
  // sort_constructors are structured sorts.
  atermpp::table struct_mappings(25, 50);
  atermpp::table struct_equations(100, 50);

  gsDebugMsg("Flatten equations\n");

  // Flatten the indexed sets from sort_constructors into a hash table,
  // so that we can easily check for presence in the next iteration.
  // Also flatten the indexed sets from sort_mappings into a hash table.
  for (ATermList l = struct_sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    // Mappings
    ATermList elts = sort_mappings[ATAgetFirst(l)].elements();
    while (!ATisEmpty(elts)) {
      struct_mappings.put(ATAgetFirst(elts), (ATerm) ATtrue);
      elts = ATgetNext(elts);
    }
    // Constructor induced equations
    elts = sort_cons_equations[ATAgetFirst(l)].elements();
    while(!ATisEmpty(elts)) {
      struct_equations.put(ATgetFirst(elts), (ATerm) ATtrue);
      elts = ATgetNext(elts);
    }
    // Per mapping equations for sort
    ATermList mappings = sort_mappings[ATAgetFirst(l)].elements();
    while(!ATisEmpty(mappings)) {
      elts = map_equations[ATAgetFirst(mappings)].elements();
      while (!ATisEmpty(elts)) {
        struct_equations.put(ATgetFirst(elts), (ATerm) ATtrue);
        elts = ATgetNext(elts);
      }
      mappings = ATgetNext(mappings);
    }
  }

  gsDebugMsg("Remove constructors\n");

  // Remove constructors for structured sorts from declarations
  ATermList constructors = ATmakeList0();
  while (!ATisEmpty(p_data_decls->cons_ops)) {
    ATermAppl cons_op = ATAgetFirst(p_data_decls->cons_ops);
    ATermAppl sort = gsGetSortExprResult(gsGetSort(cons_op));
    if (struct_sorts_table.get(sort) == NULL) {
      // Sort is not a structured sort
      constructors = ATinsert(constructors, (ATerm) cons_op);
    }
    p_data_decls->cons_ops = ATgetNext(p_data_decls->cons_ops);
  }
  p_data_decls->cons_ops = ATreverse(constructors);

  gsDebugMsg("Remove mappings\n");

  // Remove mappings from declarations
  ATermList mappings = ATmakeList0();
  while (!ATisEmpty(p_data_decls->ops)) {
    ATermAppl op = ATAgetFirst(p_data_decls->ops);
    if (!ATisEqual(struct_mappings.get(op), ATtrue)) {
      mappings = ATinsert(mappings, (ATerm) op);
    }
    p_data_decls->ops = ATgetNext(p_data_decls->ops);
  }
  p_data_decls->ops = ATreverse(mappings);

  gsDebugMsg("Remove equations\n");

  // Remove equations for structured sorts
  ATermList data_eqns = ATmakeList0();
  while(!ATisEmpty(p_data_decls->data_eqns)) {
    ATermAppl data_eqn = ATAgetFirst(p_data_decls->data_eqns);
    if(!ATisEqual(struct_equations.get(data_eqn), ATtrue)) {
      data_eqns = ATinsert(data_eqns, (ATerm) data_eqn);
    }
    p_data_decls->data_eqns = ATgetNext(p_data_decls->data_eqns);
  }
  p_data_decls->data_eqns = ATreverse(data_eqns);

  gsDebugMsg("Building structured sort declarations\n");

  // Insert struct sort declarations
  for (ATermList l = struct_sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    // Build up struct
    ATermList struct_constructors = ATmakeList0();
    ATermAppl sort = ATAgetFirst(l);
    ATermList constructors = sort_constructors[sort].elements();

    if(num_sort_constructors[sort] == 2 &&
       (ATindexOf(constructors, (ATerm) gsMakeOpIdEmptyList(sort), 0) != -1)) {
      // sort is a list sort
      ATermAppl element_sort = NULL;
      while (!ATisEmpty(constructors)) {
        ATermAppl constructor = ATAgetFirst(constructors);
        if (ATisEqual(gsGetName(constructor), gsMakeOpIdNameCons())) {
          element_sort = ATAgetFirst(ATLgetArgument(gsGetSort(constructor), 0));
        }
        constructors = ATgetNext(constructors);
      }
      if (element_sort != NULL) {
        if (is_list_sort_id(sort)) {
          *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(sort, gsMakeSortExprList(element_sort)), *p_substs);
        } else {
          ATermAppl sort_name = ATAgetArgument(sort, 0);
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
            if (projects.count(std::make_pair(constructor, index)) > 0) {
              proj_name = gsGetName(projects[std::make_pair(constructor, index)]);
            }
            struct_projs = ATinsert(struct_projs, (ATerm) gsMakeStructProj(proj_name, ATAgetFirst(cons_sort_domain)));
            cons_sort_domain = ATgetNext(cons_sort_domain);
            index++;
          }
          struct_projs = ATreverse(struct_projs);
        }
        ATermAppl recogniser = gsMakeNil();
        if (is_recognised_by.count(constructor) > 0) {
          recogniser = gsGetName(is_recognised_by[constructor]);
        }
        ATermAppl struct_cons = gsMakeStructCons(gsGetName(constructor), struct_projs, recogniser);
        struct_constructors = ATinsert(struct_constructors, (ATerm) struct_cons);
        constructors = ATgetNext(constructors);
      }
      struct_constructors = ATreverse(struct_constructors);
      ATermAppl struct_sort = gsMakeSortStruct(struct_constructors);
      assert(gsIsSortId(sort));
      if (is_struct_sort_id(sort)) {
        *p_substs = gsAddSubstToSubsts(gsMakeSubst_Appl(sort, struct_sort), *p_substs);
      } else {
        ATermAppl sort_name = ATAgetArgument(sort, 0);
        p_data_decls->sorts = ATinsert(p_data_decls->sorts,
                                (ATerm) gsMakeSortRef(sort_name, struct_sort));
      }
    }
  }
  gsDebugMsg("Done reconstructing structured sorts\n");
}

bool is_and_of_data_var_equalities(ATermAppl data_expr)
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

bool is_list_operator(ATermAppl data_expr)
{
  if (!gsIsOpId(data_expr)) return false;
  ATermAppl name = gsGetName(data_expr);
  return ATisEqual(name, gsMakeOpIdNameEltIn()) ||
         ATisEqual(name, gsMakeOpIdNameListSize()) ||
         ATisEqual(name, gsMakeOpIdNameSnoc()) ||
         ATisEqual(name, gsMakeOpIdNameConcat()) ||
         ATisEqual(name, gsMakeOpIdNameEltAt()) ||
         ATisEqual(name, gsMakeOpIdNameHead()) ||
         ATisEqual(name, gsMakeOpIdNameTail()) ||
         ATisEqual(name, gsMakeOpIdNameRHead()) ||
         ATisEqual(name, gsMakeOpIdNameRTail());
}

bool is_constructor_induced_equation(ATermAppl data_eqn, atermpp::map<ATermAppl, atermpp::indexed_set>& sort_constructors)
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
  return false;
}

bool is_recogniser_equation(ATermAppl data_eqn)
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

bool is_projection_equation(ATermAppl data_eqn)
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

bool is_list_equation(ATermAppl data_eqn)
{
  assert(gsIsDataEqn(data_eqn));
  ATermAppl lhs = ATAgetArgument(data_eqn, 2);
  if (gsIsDataAppl(lhs)) {
    ATermAppl head = ATAgetArgument(lhs, 0);
    if (gsIsOpId(head)) {
      return is_list_operator(head);
    }
  }
  return false;
}

void remove_mapping_not_list(ATermAppl op,
                        ATermAppl sort,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& sort_mappings,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& map_equations,
                        atermpp::map<ATermAppl, int>& num_map_equations)
{
  if (!is_list_operator(op)) {
    map_equations.erase(op);
    num_map_equations.erase(op);
    if (sort_mappings[sort].index(op) >= 0) {
      sort_mappings[sort].remove(op);
    }
  }
}

