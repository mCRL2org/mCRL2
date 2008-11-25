// Author(s): Aad Mathijssen, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_implementation_concrete.h

#ifndef MCRL2_DATA_IMPLEMENTATION_CONCRETE_H
#define MCRL2_DATA_IMPLEMENTATION_CONCRETE_H

#include <aterm2.h>
#include "mcrl2/core/detail/data_common.h"

namespace mcrl2 {
  namespace data {
    namespace detail {

using namespace mcrl2::core::detail;
using namespace mcrl2::core;

//\pre spec represents an mCRL2 data, process or PBES specification that
//     adheres to the internal ATerm structure after the type checking phase.
//\post The datatypes of spec are implemented as higher-order abstract data
//     types.
//\return if the data implementation went well, an equivalent version of spec is
//     returned that adheres to the internal ATerm structure after data
//     implementation.
//     If something went wrong, an appropriate error message is printed and
//     NULL is returned.
ATermAppl implement_data_spec(ATermAppl spec);

//\pre part is an expression that adheres to the internal syntax after type
//     checking.
//     spec represents an LPS specification in the internal syntax after type
//     checking.
//\post The datatypes of spec are implemented as higher-order abstract data
//     types in spec
//\return if the data implementation went well, an equivalent version of part
//     is returned that adheres to the internal ATerm structure after data
//     implementation.
//     If something went wrong, an appropriate error message is printed and
//     NULL is returned.
ATermAppl impl_exprs_with_spec(ATermAppl part, ATermAppl& spec);

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
ATermAppl impl_exprs_appl(ATermAppl part, ATermList *p_substs,
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
ATermList impl_exprs_list(ATermList parts, ATermList *p_substs,
  t_data_decls *p_data_decls);

/** \brief     Implement data types of a type checked mCRL2 action rename
 *             specification with respect to a type checked mCRL2 linear
 *             process specification (LPS).
 *             This is a detailed implemenation, in order to prevent cluttering
 *             of the actual interface of data_implementation_concrete.h
 *  \param[in] ar_spec An ATerm representation of an mCRL2 action rename
 *             specification that adheres to the internal ATerm
 *             structure after the type checking phase.
 *  \param[in] spec An ATerm representation of an mCRL2 LPS that adheres
 *             to the internal ATerm structure after the type checking phase.
 *  \post      The data types in action_rename_spec are implemented as
 *             higher-order abstract data types types and the data types
 *             of lps_spec are added to the data types of ar_spec.  The
 *             datatypes of lps_spec are implemented as higher-order
 *             abstract data types in lps_spec
 *  \return    If the data implementation went well, an equivalent
 *             version of ar_spec is returned that adheres to the
 *             internal ATerm structure after data implementation, also
 *             containing the data specification of lps_spec If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
ATermAppl impl_data_action_rename_spec_detail(ATermAppl ar_spec, ATermAppl& lps_spec);

/// \pre sort_elt and sort_list are sort expressions.
/// \return the list of data equations belonging to the list sort sort_list, with
//     elements sort_elt.
ATermList build_list_equations(ATermAppl sort_elt, ATermAppl sort_expression_list);

/// \pre sort_list is a list sort
///     p_substs is a pointer to a list of substitutions induced by the context
///     of sort_expression_list
///     p_data_decls represents a pointer to new data declarations, induced by
///     the context of sort_expression_list
/// \post an implementation of sort_list is added to *p_data_decls and new induced
///     substitutions are added *p_substs
/// \return a sort identifier which is the implementation of sort_list
ATermAppl impl_sort_list(ATermAppl sort_expression_list, ATermList *p_substs,
  t_data_decls *p_data_decls);

/// \pre sort_elt and sort_list are sort expressions
/// \return the list of data equations belonging to the set sort_set, with elements
///     of sort_elt
ATermList build_set_equations(ATermAppl sort_elt, ATermAppl sort_set);

/// \pre sort_set is a set sort
///     p_substs is a pointer to a list of substitutions induced by the context
///     of sort_set
///     p_data_decls represents a pointer to new data declarations, induced by
///     the context of sort_set
/// \post an implementation of sort_set is added to *p_data_decls and new induced
///     substitutions are added *p_substs
/// \return a sort identifier which is the implementation of sort_set
ATermAppl impl_sort_set(ATermAppl sort_set, ATermList *p_substs,
  t_data_decls *p_data_decls);

/// \pre sort_elt and sort_list are sort expressions
/// \return the list of data equations belonging to the set sort_set, with elements
///     of sort_elt
ATermList build_bag_equations(ATermAppl sort_elt, ATermAppl sort_bag, ATermAppl sort_set);

/// \pre sort_bag is a bag sort
///     p_substs is a pointer to a list of substitutions induced by the context
///     of sort_bag
///     p_data_decls represents a pointer to new data declarations, induced by
///     the context of sort_bag
/// \post an implementation of sort_bag is added to *p_data_decls and new induced
///     substitutions are added *p_substs
/// \return a sort identifier which is the implementation of sort_bag
ATermAppl impl_sort_bag(ATermAppl sort_bag, ATermList *p_substs,
  t_data_decls *p_data_decls);

/// \pre p_data_decls represents a pointer to new data declarations
/// \post an implementation of sort Bool is added to *p_data_decls
void impl_sort_bool(t_data_decls *p_data_decls);

/// \pre p_data_decls represents a pointer to new data declarations
/// \post an implementation of sort Pos is added to *p_data_decls
void impl_sort_pos(t_data_decls *p_data_decls);

/// \pre p_data_decls represents a pointer to new data declarations
/// \post an implementation of sort Nat is added to *p_data_decls
void impl_sort_nat(t_data_decls *p_data_decls);

/// \pre p_data_decls represents a pointer to new data declarations
/// \post an implementation of sort PairNat is added to *p_data_decls
void impl_sort_nat_pair(t_data_decls *p_data_decls);

/// \pre p_data_decls represents a pointer to new data declarations
/// \post an implementation of sort Int is added to *p_data_decls
void impl_sort_int(t_data_decls *p_data_decls);

/// \pre p_data_decls represents a pointer to new data declarations
/// \post an implementation of sort Real is added to *p_data_decls
void impl_sort_real(t_data_decls *p_data_decls);

/// \pre sort is a sort expression that adheres to the internal syntax after
///     data implementation
///     p_data_decls represents a pointer to data declarations
/// \post an implementation of equality, inequality and if for sort expression
///     sort is added to *p_data_decls
void impl_standard_functions_sort(ATermAppl sort, t_data_decls *p_data_decls);

/// \pre op_id is an operation identifier of sort s_op_id; here s_op_id is:
///     - either a sort identifier
///     - or it is of the form s_0 x ... x s_n -> s, where the s_i and s are
///       sort expressions
///     p_args points to a list
///     p_vars points to a list of DataVarIds
///     context is some term
/// \return op_id, if s_op_id is a sort identifier
///     op_id(v_0,...,v_n), if s_op_id is of the form s_0 x ... x s_n -> s;
///     here for 1 <= i <= n, v_i is a data variable of sort s_i different from
///     the other v_j, and either
///     - v_i occurs in *p_vars
///     - v_i does not occur in *p_vars and context
/// \post *p_args = [v_0,...,v_n]
///     *p_vars is extended with newly introduced v_i (which did not occur in
///     *p_vars and context)
ATermAppl apply_op_id_to_vars(ATermAppl op_id, ATermList *p_args,
                                   ATermList *p_vars, ATerm context);

//Pre: term is not NULL
//Ret: sort identifier for the implementation of a structured sort with prefix
//     struct_prefix, that does not occur in term
inline
ATermAppl make_fresh_struct_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(struct_prefix(), term, false));
}

//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a list sort with prefix
//     list_prefix, that does not occur in term
inline
ATermAppl make_fresh_list_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(list_prefix(), term, false));
}

//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a set sort with prefix
//     set_prefix, that does not occur in term
inline
ATermAppl make_fresh_set_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(set_prefix(), term, false));
}

//Pre: term is not NULL
//Ret: fresh sort identifier for the implementation of a bag sort with prefix
//     bag_prefix, that does not occur in term
inline
ATermAppl make_fresh_bag_sort_id(ATerm term)
{
  return gsMakeSortId(gsFreshString2ATermAppl(bag_prefix(), term, false));
}

//Pre: sort_expr is a sort expression
//     term is not NULL
//Ret: operation identifier op_id(n, s) for the implementation of a lambda
//     abstraction, where s is sort_expr and n is a name with prefix
//     lambda_prefix, that does not occur in term
inline
ATermAppl make_fresh_lambda_op_id(ATermAppl sort_expr, ATerm term)
{
  return gsMakeOpId(gsFreshString2ATermAppl(lambda_prefix(), term, false),
    sort_expr);
}

    }
  }
}

#endif //MCRL2_DATA_RECONSTRUCT_CONCRETE_H

