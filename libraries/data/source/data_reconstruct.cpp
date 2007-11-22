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
static ATermAppl reconstruct_exprs_appl(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec = NULL);

//ret: The reconstructed version of Parts.
static ATermList reconstruct_exprs_list(ATermList Parts, ATermList* p_substs, const ATermAppl Spec = NULL);

//pre: Part is a data expression
//ret: The reconstructed version of Part.
static ATermAppl reconstruct_data_expr(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec, bool* recursive);

//TODO: Describe prototype
static ATermAppl reconstruct_pos_mult(ATermAppl PosExpr, char const* Mult);

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

//pre: data_expr is a data expression
//ret: true if data_expr is of the form v_1 == w_1 && ... && v_n == w_n, where
//     v_i, w_j are data variables,
//     false, otherwise
static bool is_and_of_data_var_equalities(ATermAppl data_expr);

//ret: true if all elements of l are DataVarIds,
//     false otherwise
static bool is_list_of_data_var_ids(ATermList l);

//ret: list l from which all elements that occur in t are removed
static ATermList filter_table_elements_from_list(ATermList l, atermpp::table& t);

//pre: data_expr is a data expression
//ret: true if data_expr is a system defined function on lists
static bool is_list_operator(ATermAppl data_expr);

//pre: data_expr is a data expression
//ret: true if data_expr is a system defined function on sets
static bool is_set_operator(ATermAppl data_expr);

//pre: data_expr is a data expression
//ret: true if data_expr is a system defined function on bags
static bool is_bag_operator(ATermAppl data_expr);

//pre: data_eqn is a data equation, sort_constructors holds the constructors
//     for all relevant sorts.
//ret: true if data_eqn has the form such that it belongs to a constructor
//     false otherwise.
static bool is_constructor_induced_equation(ATermAppl data_eqn, atermpp::map<ATermAppl, atermpp::indexed_set>& sort_constructors);

//pre: data_eqn is a data equation
//ret: true if data_eqn has the form of an equation for a recogniser function.
static bool is_recogniser_equation(ATermAppl data_eqn);

//pre: data_eqn is a data equation
//ret: true if data_eqn has the form of an equation for a projection function.
static bool is_projection_equation(ATermAppl data_eqn);

//pre: op is an OpId, sort is a sort expression, sort_mappings contains mappings
//     of sort->indexed set of functions, map_equations contains mappings of
//     functions -> indexed set of equations. num_map_equations[s] ==
//     |map_equations[s]|
static void remove_mapping_not_list(ATermAppl op,
                        ATermAppl sort,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& sort_mappings,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& map_equations,
                        atermpp::map<ATermAppl, int>& num_map_equations);

//pre: generic_list is a list of terms which may be annotated with label @dummy
//ret: true if there is a match for term in generic_list
static bool find_match_in_list(ATermAppl term, ATermList generic_list);

//pre: aterm_ann is a term which may have annotation with label @dummy,
//     aterm is a term, p_substs is a list with substitutions, induced by adding
//     equations for annotated terms with terms from term.
//ret: true if aterm_ann with p_substs applied, and aterm match.
static bool match(ATerm aterm_ann, ATerm aterm, ATermList* p_substs);

//similar to match
static bool match_appl(ATermAppl aterm_ann, ATermAppl aterm, ATermList* p_substs);

//similar to match
static bool match_list(ATermList aterm_ann, ATermList aterm, ATermList* p_substs);

//post: p_substs is extended with substitutions for the implementations of the
//      lambda expressions in p_data_decls->data_eqns
static void calculate_lambda_expressions(t_data_decls* p_data_decls, ATermList* p_substs);

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
ATermList substs = ATmakeList0();
if (ATgetType(Part) == AT_APPL) {
  Result = (ATerm) reconstruct_exprs_appl((ATermAppl) Part, &substs, Spec);
  } else { //(ATgetType(Part) == AT_LIST) {
    Result = (ATerm) reconstruct_exprs_list((ATermList) Part, &substs, Spec);
  }
  gsDebugMsg("Finished data reconstruction\n");
  return Result;
}

ATermAppl reconstruct_exprs_appl(ATermAppl Part, ATermList* p_substs, const ATermAppl Spec)
{
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));
  bool recursive = true;

  if((gsIsSpecV1(Part) || gsIsPBES(Part)) && (Spec != NULL)) {
    gsDebugMsg("Removing headers from specification\n");
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
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));

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
  assert ((Spec == NULL) || gsIsSpecV1(Spec) || gsIsPBES(Spec));
  assert(gsIsDataExpr(Part));

  if (gsIsDataExprBagComp(Part)) {
    gsDebugMsg("Reconstructing implementation of bag comprehension\n", Part);
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
    Part = gsSubstValues_Appl(*p_substs, Part, true);
    Part = (ATermAppl) beta_reduce_term((ATerm) Part);
    *recursive = false;
    Part = reconstruct_exprs_appl(Part, p_substs, Spec);
  } else if (is_lambda_op_id(Part)) {
    gsDebugMsg("Reconstructing implementation of a lambda op id\n");
    Part = gsSubstValues_Appl(*p_substs, Part, true);
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
    value = reconstruct_exprs_appl(value, p_substs, Spec);
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
    value = reconstruct_exprs_appl(value, p_substs, Spec);
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
    value = reconstruct_exprs_appl(value, p_substs, Spec);
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
      ATermAppl Sort = gsGetSortExprResult(gsGetSort(IR));
      Part = gsMakeDataExprAdd(
               gsMakeDataExprMult(bool_to_numeric(Bit, Sort), IR),
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
        ATermAppl Sort = gsGetSortExprResult(gsGetSort(PosArg));
        return gsMakeDataExprAdd(PosArg, bool_to_numeric(BoolArg, Sort));
      } else {
        //Mult*v(b)
        ATermAppl Sort = gsGetSortExprResult(gsGetSort(PosArg));
        return gsMakeDataExprAdd(PosArg, 
                 gsMakeDataExprMult(gsMakeOpId(gsString2ATermAppl(Mult), 
                                      Sort), 
                                    bool_to_numeric(BoolArg, Sort)));
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
  return is_lambda_op_id(Part);
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
  reconstruct_data_decls(&data_decls, p_substs);

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

void reconstruct_data_decls(t_data_decls* p_data_decls, ATermList* p_substs)
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

  calculate_lambda_expressions(p_data_decls, p_substs);
  ATermList lambda_substs = *p_substs; // Copy needed for interfering substitutions that
                                       // are added in processing the mappings!
  ATermList sorts = ATconcat(get_sorts((ATerm) p_data_decls->sorts),
                    ATconcat(get_sorts((ATerm) p_data_decls->cons_ops),
                    ATconcat(get_sorts((ATerm) p_data_decls->ops),
                             get_sorts((ATerm) p_data_decls->data_eqns))));

  // Initialisation
  for (ATermList l = sorts; !ATisEmpty(l); l = ATgetNext(l))
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
  // sorts sort_constructors(s) contains exactly the
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
    // Special cases for set and bag comprehension, the only interesting
    // constant functions for data reconstruction
    if (gsIsSortId(sort)) {
      if ((ATisEqual(gsMakeOpIdNameEmptyBag(), gsGetName(op)) ||
           ATisEqual(gsMakeOpIdNameEmptySet(), gsGetName(op))) &&
           (struct_sorts_table.get(sort) != NULL)) {
        put_result = sort_mappings[sort].put(op);
        if (put_result.second) {
          num_sort_mappings[sort]++;
          map_equations.insert(std::make_pair(op, atermpp::indexed_set(6, 50)));
          num_map_equations.insert(std::make_pair(op, 0));
        }
      }
    } else if (gsIsSortArrow(sort)) {
      ATermList sort_domain = ATLgetArgument(sort, 0);
      ATermAppl sort_range = gsGetSortExprResult(sort);
      ATermAppl dom_sort = ATAgetFirst(sort_domain);
      if (ATisEqual(gsMakeOpIdNameIf(), gsGetName(op)) ||
          ATisEqual(gsMakeOpIdNameEltIn(), gsGetName(op)) ||
          ATisEqual(gsMakeOpIdNameCount(), gsGetName(op))) {
        dom_sort = ATAgetFirst(ATgetNext(sort_domain));
      } else if (ATisEqual(gsMakeOpIdNameSetComp(), gsGetName(op)) ||
                 ATisEqual(gsMakeOpIdNameBagComp(), gsGetName(op))) {
        dom_sort = sort_range;
      }
      if (gsIsOpId(op) && (struct_sorts_table.get(dom_sort) != NULL)) {
        if (ATisEqual(gsMakeOpIdIf(dom_sort), op)
          || ATisEqual(gsMakeOpIdEq(dom_sort), op)
          || ATisEqual(gsMakeOpIdNeq(dom_sort), op)
          || is_list_operator(op)
          || is_set_operator(op)
          || is_bag_operator(op)
          || (ATgetLength(sort_domain) == 1 && sort_range == gsMakeSortExprBool())
          || (ATgetLength(sort_domain) == 1  /*TODO: strengthen condition for projs*/)) {
          put_result = sort_mappings[dom_sort].put(op);
          if (put_result.second) {
            num_sort_mappings[dom_sort]++;
            map_equations.insert(std::make_pair(op, atermpp::indexed_set(6, 50)));
            num_map_equations.insert(std::make_pair(op, 0));
          }
          if (ATisEqual(gsMakeOpIdNameSetComp(), gsGetName(op))) {
            ATermList sort_elt = ATLgetArgument(gsGetSort(op), 0);
            ATermAppl sort_set = ATAgetArgument(gsGetSort(op), 1);
            assert(ATgetLength(sort_elt) == 1);
            *p_substs = gsAddSubstToSubsts(
              gsMakeSubst_Appl(sort_set,
                               gsMakeSortExprSet(ATAgetFirst(sort_elt))),
              *p_substs);
          } else if (ATisEqual(gsMakeOpIdNameBagComp(), gsGetName(op))) {
            ATermList sort_elt = ATLgetArgument(gsGetSort(op), 0);
            ATermAppl sort_bag = ATAgetArgument(gsGetSort(op), 1);
            assert(ATgetLength(sort_elt) == 1);
            *p_substs = gsAddSubstToSubsts(
              gsMakeSubst_Appl(sort_bag,
                               gsMakeSortExprBag(ATAgetFirst(sort_elt))),
              *p_substs);
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
    data_eqn = gsSubstValues_Appl(lambda_substs, data_eqn, true);
    data_eqn = (ATermAppl) beta_reduce_term((ATerm) data_eqn);
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
      ATermAppl op = ATAgetArgument(data_eqn_lhs, 0);
      ATermAppl op_sort = gsGetSort(op);
      ATermAppl sort = ATAgetFirst(ATLgetArgument(op_sort, 0));
      ATermList args = ATLgetArgument(data_eqn_lhs, 1);
      ATermAppl arg0 = ATAgetFirst(args);
      ATermAppl arg1 = ATAgetFirst(ATgetNext(args));
      assert(gsIsDataExpr(arg0) && gsIsDataExpr(arg1));
      // Special case, data_eqn is of the form var == var = true;
      if (gsIsDataVarId(arg0) && gsIsDataVarId(arg1) && ATisEqual(arg0, arg1) &&
          ATisEqual(data_eqn_rhs, gsMakeDataExprTrue())) {
        put_result = map_equations[op].put(data_eqn);
        if (put_result.second) {
          num_map_equations[op]++;
        }
      } else if (is_constructor_induced_equation(data_eqn, sort_constructors)
        || find_match_in_list(data_eqn, generic_equations)) {
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
      } else if (find_match_in_list(data_eqn, generic_equations)) {
        put_result = map_equations[head].put(data_eqn);
        if (put_result.second) {
          num_map_equations[head]++;
        }
      } else {
        recognises.erase(head);
        remove_mapping_not_list(head, sort, sort_mappings, map_equations, num_map_equations);
      }
    } else if (gsIsOpId(data_eqn_lhs)) {
      if (find_match_in_list(data_eqn, generic_equations)) {
        put_result = map_equations[data_eqn_lhs].put(data_eqn);
        if (put_result.second) {
          num_map_equations[data_eqn_lhs]++;
        }
      } else {
        ATermAppl sort = gsGetSort(data_eqn_lhs);
        //XXX hackish solution
        if (gsIsSortArrow(sort)) {
          sort = ATAgetFirst(ATLgetArgument(sort, 0));
        }
        recognises.erase(data_eqn_lhs);
        remove_mapping_not_list(data_eqn_lhs, sort, sort_mappings, map_equations, num_map_equations);
      }
    }
  }

  gsDebugMsg("Determined equations\n");

  // Check for completeness and remove structured sort from data declarations
  ATermList non_struct_sorts = ATmakeList0();
  for (ATermList l = sorts; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl sort = ATAgetFirst(l);
    if (!is_bag_sort_id(sort) && !is_set_sort_id(sort) && !is_list_sort_id(sort)) {
      if ((num_sort_cons_equations[sort] != (num_sort_constructors[sort] * num_sort_constructors[sort]))) {
        if (ATindexOf(p_data_decls->sorts, (ATerm) sort, 0) != -1) {
          non_struct_sorts = ATinsert(non_struct_sorts, (ATerm) sort);
        }
        struct_sorts_table.remove(sort);
        sort_constructors.erase(sort);
        num_sort_constructors.erase(sort);
        sort_mappings.erase(sort);
        num_sort_mappings.erase(sort);
        sort_cons_equations.erase(sort);
        num_sort_cons_equations.erase(sort);
      }
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
        if (gsIsSortArrow(gsGetSort(map))) {
          if(ATisEqual(gsGetSortExprResult(gsGetSort(map)), gsMakeSortExprBool())) {
            if(num_map_equations[map] != num_sort_constructors[sort]) {
              remove_mapping_not_list(map, sort, sort_mappings, map_equations, num_map_equations);
            } else {
              is_recognised_by[recognises[map]] = map;
            }
          } else if (num_map_equations[map] != 1) {
            remove_mapping_not_list(map, sort, sort_mappings, map_equations, num_map_equations);
          }
        } else {
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
    if (!ATisEqual(struct_mappings.get(op), ATtrue) &&
        !is_lambda_op_id(op)) {
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
    ATermAppl lhs = ATAgetArgument(data_eqn, 2);
    // Unconditionally remove equations if the lhs is a lambda opid
    if (!is_lambda_expr(lhs)) {
      data_eqn = gsSubstValues_Appl(lambda_substs, data_eqn, true);
      data_eqn = (ATermAppl) beta_reduce_term((ATerm) data_eqn);
      if(!ATisEqual(struct_equations.get(data_eqn), ATtrue)) {
        data_eqns = ATinsert(data_eqns, (ATerm) data_eqn);
      }
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

    if(num_sort_constructors[sort] != 0) {
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
  return ATisEqual(name, gsMakeOpIdNameEltIn())    ||
         ATisEqual(name, gsMakeOpIdNameListSize()) ||
         ATisEqual(name, gsMakeOpIdNameSnoc())     ||
         ATisEqual(name, gsMakeOpIdNameConcat())   ||
         ATisEqual(name, gsMakeOpIdNameEltAt())    ||
         ATisEqual(name, gsMakeOpIdNameHead())     ||
         ATisEqual(name, gsMakeOpIdNameTail())     ||
         ATisEqual(name, gsMakeOpIdNameRHead())    ||
         ATisEqual(name, gsMakeOpIdNameRTail());
}

bool is_set_operator(ATermAppl data_expr)
{
  if (!gsIsOpId(data_expr)) return false;
  ATermAppl name = gsGetName(data_expr);
  return ATisEqual(name, gsMakeOpIdNameSetComp())     ||
         ATisEqual(name, gsMakeOpIdNameEmptySet())    ||
         ATisEqual(name, gsMakeOpIdNameEltIn())       ||
         ATisEqual(name, gsMakeOpIdNameSubSetEq())    ||
         ATisEqual(name, gsMakeOpIdNameSubSet())      ||
         ATisEqual(name, gsMakeOpIdNameSetUnion())    ||
         ATisEqual(name, gsMakeOpIdNameSetDiff())     ||
         ATisEqual(name, gsMakeOpIdNameSetIntersect())||
         ATisEqual(name, gsMakeOpIdNameSetCompl());
}

bool is_bag_operator(ATermAppl data_expr)
{
  if (!gsIsOpId(data_expr)) return false;
  ATermAppl name = gsGetName(data_expr);
  return ATisEqual(name, gsMakeOpIdNameBagComp())     ||
         ATisEqual(name, gsMakeOpIdNameEmptyBag())    ||
         ATisEqual(name, gsMakeOpIdNameCount())       ||
         ATisEqual(name, gsMakeOpIdNameEltIn())       ||
         ATisEqual(name, gsMakeOpIdNameSubBagEq())    ||
         ATisEqual(name, gsMakeOpIdNameSubBag())      ||
         ATisEqual(name, gsMakeOpIdNameBagUnion())    ||
         ATisEqual(name, gsMakeOpIdNameBagDiff())     ||
         ATisEqual(name, gsMakeOpIdNameBagIntersect())||
         ATisEqual(name, gsMakeOpIdNameBag2Set())     ||
         ATisEqual(name, gsMakeOpIdNameSet2Bag());
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

void remove_mapping_not_list(ATermAppl op,
                        ATermAppl sort,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& sort_mappings,
                        atermpp::map<ATermAppl, atermpp::indexed_set>& map_equations,
                        atermpp::map<ATermAppl, int>& num_map_equations)
{
  if (!is_list_operator(op) && !is_set_operator(op) && !is_bag_operator(op)) {
    map_equations.erase(op);
    num_map_equations.erase(op);
    if (sort_mappings[sort].index(op) >= 0) {
      sort_mappings[sort].remove(op);
    }
  }
}

bool find_match_in_list(ATermAppl term, ATermList generic_list)
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

bool match_appl(ATermAppl aterm_ann, ATermAppl aterm, ATermList* p_substs)
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

void calculate_lambda_expressions(t_data_decls* p_data_decls, ATermList* p_substs)
{
  for (ATermList l = p_data_decls->data_eqns; !ATisEmpty(l); l = ATgetNext(l))
  {
    ATermAppl data_eqn = ATAgetFirst(l);
    ATermAppl expr = ATAgetArgument(data_eqn, 2);
    while (gsIsDataAppl(expr)) {
      ATermList bound_vars = ATLgetArgument(expr, 1);
      expr = ATAgetArgument(expr, 0);
      if (is_lambda_op_id(expr)) {
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

