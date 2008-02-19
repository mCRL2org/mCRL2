// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_implementation.h
///
/// \brief A library for specifying data types in a 
/// specification as higher order data types.
///
/// \pre Global preconditions:
///      - the ATerm library has been initialised

#ifndef MCRL2_DATAIMPL_H
#define MCRL2_DATAIMPL_H

#include <aterm2.h>
#include "mcrl2/core/detail/data_common.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {
  namespace core {
    namespace detail {


/// \pre spec represents an mCRL2 process specification that adheres to the
///     internal ATerm structure after the type checking phase.
/// \post The datatypes of spec are implemented as higher-order abstract data
///     types.
/// \return if the data implementation went well, an equivalent version of spec is
///     returned that adheres to the internal ATerm structure after data
///     implementation.
///     If something went wrong, an appropriate error message is printed and
///     NULL is returned.
ATermAppl implement_data_proc_spec(ATermAppl spec);

/** \pre sort_expr represents a sort expression that adheres to the internal
 *     ATerm structure after the type checking phase.
 *     lps_spec represents an LPS specification
 * \post The datatypes of sort_expr are implemented as higher-order abstract data
 *     types in lps_spec
 * \return if the data implementation went well, an equivalent version of sort_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 **/
ATermAppl implement_data_sort_expr(ATermAppl sort_expr,
  lps::specification &lps_spec);

/** \pre data_expr represents a data expression that adheres to the internal
 *     ATerm structure after the type checking phase.
 *     lps_spec represents an LPS specification
 * \post The datatypes of data_expr are implemented as higher-order abstract data
 *     types in lps_spec
 * \return if the data implementation went well, an equivalent version of data_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 **/
ATermAppl implement_data_data_expr(ATermAppl data_expr,
  lps::specification &lps_spec);

/** \pre mult_act represents a multiaction that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     lps_spec represents an LPS specification
 * \post The datatypes of spec are implemented as higher-order abstract data
 *     types in lps_spec
 * \return if the data implementation went well, an equivalent version of mult_act
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 **/
ATermAppl implement_data_mult_act(ATermAppl mult_act,
  lps::specification &lps_spec);

/** \pre proc_expr represents a process expression that adheres to the internal
 *     ATerm structure after the type checking phase.
 *     lps_spec represents an LPS specification
 * \post The datatypes of proc_expr are implemented as higher-order abstract data
 *     types in lps_spec
 * \return if the data implementation went well, an equivalent version of proc_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 **/
ATermAppl implement_data_proc_expr(ATermAppl proc_expr,
  lps::specification &lps_spec);

/** \pre state_frm represents a state formula that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     spec represents either an LPS specification, a PBES or a data
 *     specification, all before data implementation
 * \post The datatypes of spec are implemented as higher-order abstract data
 *     types in spec
 * \return if the data implementation went well, an equivalent version of state_frm
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 **/
ATermAppl implement_data_state_frm(ATermAppl state_frm,
  ATermAppl& spec);

/** \pre ar_spec represents an action rename specification that
 *     adheres to the internal ATerm structure after the type checking fase.
 *     lps_spec represents an LPS specification
 * \post The datatypes in action_rename_spec are implemented as higher-order
 *     abstract data types types and the data types of lps_spec are added
 *     to the data types of ar_spec.
 * \return if the data implementation went well, an equivalent version of
 *     ar_spec is returned that adheres to the internal ATerm
 *     structure after data implementation, also containing the data
 *     specification of lps_spec
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 **/
ATermAppl implement_data_action_rename_spec(ATermAppl ar_spec,
  lps::specification &lps_spec);

/* ============================================================================
 * Detailed data implementation, this is used by data reconstruction only!
 * ============================================================================
 */

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


    }
  }
}
#endif // MCRL2_DATAIMPL_H
