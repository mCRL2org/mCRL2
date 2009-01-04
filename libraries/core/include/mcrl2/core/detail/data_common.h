// Author(s): Aad Mathijssen, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_common.h

#ifndef MCRL2_DATA_COMMON_H
#define MCRL2_DATA_COMMON_H

#include <aterm2.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct.h"

namespace mcrl2 {
  namespace core {
    namespace detail {

// --------------------------
// Auxiliary list operations
// --------------------------

/// \pre l and m are two lists without duplicates
/// \ret a list with all elements of l and m precisely once
ATermList merge_list(ATermList l, ATermList m);

/// \pre l and m are two lists
/// \ret a copy of l without elements that occur in m
ATermList subtract_list(ATermList l, ATermList m);

// ---------------------------------------------
// Auxiliary functions for system defined sorts
// ---------------------------------------------

/// \ret prefix for the name of an implementation of a structured sort.
inline const char* struct_prefix() { return "Struct@"; }

/// \ret prefix for the name of an implementation of a list sort.
inline const char* list_prefix()   { return "List@";   }

/// \ret prefix for the name of an implementation of a set sort.
inline const char* set_prefix()    { return "Set@";    }

/// \ret prefix for the name of an implementation of a bag sort.
inline const char* bag_prefix()    { return "Bag@";    }

/// \ret prefix for the name of an implementation of a lambda expression.
inline const char* lambda_prefix() { return "lambda@"; }

/// \pre sort_expr is sort expression
/// \ret sort_expr is the implementation of a structured sort
inline bool is_struct_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      struct_prefix(),
      ATgetName(ATgetAFun(mcrl2::core::ATAgetArgument(sort_expr, 0))),
      strlen(struct_prefix())) == 0;
  } else {
    return false;
  }
}

/// \pre sort_expr is sort expression
/// \ret sort_expr is the implementation of a list sort
inline bool is_list_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      list_prefix(),
      ATgetName(ATgetAFun(mcrl2::core::ATAgetArgument(sort_expr, 0))),
      strlen(list_prefix())) == 0;
  } else {
    return false;
  }
}

/// \pre sort_expr is sort expression
/// \ret sort_expr is the implementation of a set sort
inline bool is_set_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      set_prefix(),
      ATgetName(ATgetAFun(mcrl2::core::ATAgetArgument(sort_expr, 0))),
      strlen(set_prefix())) == 0;
  } else {
    return false;
  }
}

/// \pre sort_expr is sort expression
/// \ret sort_expr is the implementation of a bag sort
inline bool is_bag_sort_id(ATermAppl sort_expr)
{
  if (gsIsSortId(sort_expr)) {
    return strncmp(
      bag_prefix(),
      ATgetName(ATgetAFun(mcrl2::core::ATAgetArgument(sort_expr, 0))),
      strlen(bag_prefix())) == 0;
  } else {
    return false;
  }
}

/// \pre data_expr is a data expression
/// \ret data_expr is an operation identifier for the implementation of a lambda
//     abstraction
inline bool is_lambda_op_id(ATermAppl data_expr)
{
  if (gsIsOpId(data_expr)) {
    return strncmp(
      lambda_prefix(),
      ATgetName(ATgetAFun(mcrl2::core::ATAgetArgument(data_expr, 0))),
      strlen(lambda_prefix())) == 0;
  } else {
    return false;
  }
}

/// \pre data_expr is a data expression
/// \ret data_expr is the implementation of a list enumeration
bool is_list_enum_impl(ATermAppl data_expr);

// ---------------------------------------------------------
// Definition and auxiliary functions for data declarations
// ---------------------------------------------------------

/// \brief The type t_data_decls represents data declarations, i.e. sort, constructor,
//         operation and data equation declarations
typedef struct {
  ATermList sorts;
  ATermList cons_ops;
  ATermList ops;
  ATermList data_eqns;
} t_data_decls;

/// \post all fields of p_data_decls have been initialized with the empty list.
void inline initialize_data_decls(t_data_decls *p_data_decls)
{
  p_data_decls->sorts     = ATmakeList0();
  p_data_decls->cons_ops  = ATmakeList0();
  p_data_decls->ops       = ATmakeList0();
  p_data_decls->data_eqns = ATmakeList0();
}

/// \ret indicates whether the elements of data_decls are initialised
#define data_decls_is_initialised(data_decls)\
(data_decls.sorts != NULL && data_decls.cons_ops  != NULL &&\
 data_decls.ops   != NULL && data_decls.data_eqns != NULL)

/// \pre substs is a list of substitutions
//     recursive denotes wheter to apply substitutions recursively through the terms
/// \post p_data_decls in which substs have been applied to sorts, cons_ops, ops and data_eqns
void inline subst_values_list_data_decls(ATermList substs, t_data_decls *p_data_decls, bool recursive)
{
  p_data_decls->sorts     = gsSubstValues_List(substs, p_data_decls->sorts,     recursive);
  p_data_decls->cons_ops  = gsSubstValues_List(substs, p_data_decls->cons_ops,  recursive);
  p_data_decls->ops       = gsSubstValues_List(substs, p_data_decls->ops,       recursive);
  p_data_decls->data_eqns = gsSubstValues_List(substs, p_data_decls->data_eqns, recursive);
}

/// \pre *p_data_decls_1 = v_data_decls_1, *p_data_decls_2 = v_data_decls_2
/// \post *p_data_decls_1 = v_data_decls_1 + v_data_decls_2, *p_data_decls_2 = v_data_decls_2
void inline concat_data_decls(t_data_decls *p_data_decls_1, t_data_decls *p_data_decls_2)
{
  p_data_decls_1->sorts     = ATconcat(p_data_decls_1->sorts,     p_data_decls_2->sorts);
  p_data_decls_1->cons_ops  = ATconcat(p_data_decls_1->cons_ops,  p_data_decls_2->cons_ops);
  p_data_decls_1->ops       = ATconcat(p_data_decls_1->ops,       p_data_decls_2->ops);
  p_data_decls_1->data_eqns = ATconcat(p_data_decls_1->data_eqns, p_data_decls_2->data_eqns);
}

/// \pre *p_data_decls_1 = v_data_decls_1, *p_data_decls_2 = v_data_decls_2
/// \post *p_data_decls_1 = v_data_decls_1 from which everything that is also in v_data_decls_2
//      has been removed.
//      *p_data_decls_2 = v_data_decls_2
void inline subtract_data_decls(t_data_decls *p_data_decls_1, t_data_decls *p_data_decls_2)
{
  p_data_decls_1->sorts     = subtract_list(p_data_decls_1->sorts,     p_data_decls_2->sorts);
  p_data_decls_1->cons_ops  = subtract_list(p_data_decls_1->cons_ops,  p_data_decls_2->cons_ops);
  p_data_decls_1->ops       = subtract_list(p_data_decls_1->ops,       p_data_decls_2->ops);
  p_data_decls_1->data_eqns = subtract_list(p_data_decls_1->data_eqns, p_data_decls_2->data_eqns);
}

/// \ret data_decls1 is equal to data_decls2
inline bool data_decls_equal(t_data_decls data_decls1,
  t_data_decls data_decls2)
{
  return
    ATisEqual(data_decls1.sorts, data_decls2.sorts) &&
    ATisEqual(data_decls1.cons_ops, data_decls2.cons_ops) &&
    ATisEqual(data_decls1.ops, data_decls2.ops) &&
    ATisEqual(data_decls1.data_eqns, data_decls2.data_eqns);
}

/// \ret data declarations of spec
inline t_data_decls get_data_decls(ATermAppl spec)
{
  t_data_decls data_decls;
  ATermAppl data_spec =  ATAgetArgument(spec, 0);
  data_decls.sorts =     ATLgetArgument(ATAgetArgument(data_spec, 0), 0);
  data_decls.cons_ops =  ATLgetArgument(ATAgetArgument(data_spec, 1), 0);
  data_decls.ops =       ATLgetArgument(ATAgetArgument(data_spec, 2), 0);
  data_decls.data_eqns = ATLgetArgument(ATAgetArgument(data_spec, 3), 0);
  return data_decls;
}

/// \ret spec in which the data declarations are replaced by data_decls
inline ATermAppl set_data_decls(ATermAppl spec, t_data_decls data_decls)
{
  assert(gsIsProcSpec(spec) || gsIsLinProcSpec(spec) || gsIsPBES(spec) || gsIsActionRenameSpec(spec) || gsIsDataSpec(spec));
  assert(data_decls_is_initialised(data_decls));
  if (!data_decls_equal(data_decls, get_data_decls(spec))) {
    ATermAppl sorts = gsMakeSortSpec(data_decls.sorts);
    ATermAppl cons_ops = gsMakeConsSpec(data_decls.cons_ops);
    ATermAppl ops = gsMakeMapSpec(data_decls.ops);
    ATermAppl data_eqns = gsMakeDataEqnSpec(data_decls.data_eqns);
    ATermAppl data_spec = gsMakeDataSpec(sorts, cons_ops, ops, data_eqns);
    if (gsIsDataSpec(spec)) {
      spec = data_spec;
    } else {
      spec = ATsetArgument(spec, (ATerm) data_spec, 0);
    }
  }
  return spec;
}

/// \pre spec is a specification that adheres to the internal syntax of an
//     arbitary phase
/// \ret spec in which the data declarations from data_decls are added
ATermAppl add_data_decls(ATermAppl spec, t_data_decls data_decls);

// --------------------
// Auxiliary functions
// --------------------

/// \pre BoolExpr is a boolean expression, SortExpr is of type Pos, Nat, Int or
//     Real.
/// \ret if(BoolExpr, 1, 0) of sort SortExpr
inline ATermAppl bool_to_numeric(ATermAppl BoolExpr, ATermAppl SortExpr)
{
  // TODO Maybe enforce that SortExpr is a PNIR sort
  return gsMakeDataExprIf(BoolExpr,
           gsMakeOpId(gsString2ATermAppl("1"), SortExpr),
           gsMakeOpId(gsString2ATermAppl("0"), SortExpr));
}

/// \pre data_expr is a data expression that adheres to the internal syntax after
//     type checking
/// \ret The free variables in data_expr
ATermList get_free_vars(ATermAppl data_expr);

/// \pre term adheres to the internal format
/// \ret a list of all sorts occuring in term, where each element is unique
ATermList get_sorts(ATerm term);

/// \pre Term to perform beta reduction on,
//     this is the top-level function, which should be used when
//     there is no appropriate context.
/// \ret Term with beta reduction performed on it.
ATerm beta_reduce_term(ATerm Term);

/// \ret Part with capture avoiding substitution of the substitutions in Substs
//       performed on it.
ATermAppl capture_avoiding_substitutions(ATermAppl Part, ATermList Substs);

    }
  }
} 

#endif //MCRL2_DATA_COMMON_H

