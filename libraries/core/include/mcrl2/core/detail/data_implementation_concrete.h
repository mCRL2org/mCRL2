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
  namespace core {
    namespace detail {

//\pre spec represents an mCRL2 data, linear process, process or PBES
//     specification that adheres to the internal ATerm structure after the
//     type checking phase.
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

/// \pre  sort_struct is a structured sort
///       sort_id is a fresh sort identifier (not occurring in p_data_decls->sorts)
///       p_substs is a pointer to a list of substitutions induced by the context
///       of sort_struct
///       p_data_decls represents a pointer to new data declarations, induced by
///       the context of sort_struct
/// \post an implementation of sort_struct represented by sort sort_id is added to
///       *p_data_decls and new induced substitutions are added *p_substs
void impl_sort_struct(ATermAppl sort_struct, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls);

/// \pre sort_elt and sort_list are sort expressions.
/// \return the list of data equations belonging to the list sort sort_list, with
//     elements sort_elt.
ATermList build_list_equations(ATermAppl sort_elt, ATermAppl sort_expression_list);

/// \pre  sort_list is a list sort
///       sort_id is a fresh sort identifier (not occurring in p_data_decls->sorts)
///       p_substs is a pointer to a list of substitutions induced by the context
///       of sort_expression_list
///       p_data_decls represents a pointer to new data declarations, induced by
///       the context of sort_expression_list
/// \post an implementation of sort_list represented by sort sort_id is added to
///       *p_data_decls and new induced substitutions are added *p_substs
void impl_sort_list(ATermAppl sort_expression_list, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls);

/// \pre sort_elt and sort_list are sort expressions
/// \return the list of data equations belonging to the set sort_set, with elements
///     of sort_elt
ATermList build_set_equations(ATermAppl sort_elt, ATermAppl sort_set);

/// \pre  sort_set is a set sort
///       sort_id is a fresh sort identifier (not occurring in p_data_decls->sorts)
///       p_substs is a pointer to a list of substitutions induced by the context
///       of sort_set
///       p_data_decls represents a pointer to new data declarations, induced by
///       the context of sort_set
/// \post an implementation of sort_set represented by sort sort_id is added to
///       *p_data_decls and new induced substitutions are added *p_substs
void impl_sort_set(ATermAppl sort_set, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls);

/// \pre sort_elt and sort_list are sort expressions
/// \return the list of data equations belonging to the set sort_set, with elements
///     of sort_elt
ATermList build_bag_equations(ATermAppl sort_elt, ATermAppl sort_bag, ATermAppl sort_set);

/// \pre  sort_bag is a bag sort
///       sort_id is a fresh sort identifier (not occurring in p_data_decls->sorts)
///       p_substs is a pointer to a list of substitutions induced by the context
///       of sort_bag
///       p_data_decls represents a pointer to new data declarations, induced by
///       the context of sort_bag
/// \post an implementation of sort_bag represented by sort sort_id is added to
///       *p_data_decls and new induced substitutions are added *p_substs
void impl_sort_bag(ATermAppl sort_bag, ATermAppl sort_id,
  ATermList *p_substs, t_data_decls *p_data_decls);

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

/// \pre    op_id is an operation identifier of sort s_op_id; here s_op_id is:
///         - either a sort identifier
///         - or it is of the form s_0 x ... x s_n -> s, where the s_i and s are
///           sort expressions
///         p_vars points to a list of DataVarIds
///         context is some term
/// \return [], if s_op_id is a sort identifier
///         [v_0,...,v_n], if s_op_id is of the form s_0 x ... x s_n -> s;
///         here for 1 <= i <= n, v_i is a data variable of sort s_i different from
///         the other v_j, and either
///         - v_i occurs in *p_vars
///         - v_i does not occur in *p_vars and context
/// \post   *p_vars is extended with newly introduced v_i
///         (which did not occur in *p_vars and context)
ATermList create_op_id_args(ATermAppl op_id, ATermList *p_vars, ATerm context);

    }
  }
}

#endif //MCRL2_DATA_RECONSTRUCT_CONCRETE_H

