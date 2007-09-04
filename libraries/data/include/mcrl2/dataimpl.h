// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file dataimpl.h

#ifndef MCRL2_DATAIMPL_H
#define MCRL2_DATAIMPL_H

#include <aterm2.h>
#include "mcrl2/data_common.h"
#include "mcrl2/lps/specification.h"

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl implement_data_spec(ATermAppl spec);
/*Pre: spec represents a specification that adheres to the internal ATerm
 *     structure after the type checking phase.
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types.
 *Ret: if the data implementation went ok, an equivalent version of spec is
 *     returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_sort_expr(ATermAppl sort_expr,
  lps::specification &lps_spec);
/*Pre: sort_expr represents a sort expression that adheres to the internal
 *     ATerm structure after the type checking phase.
 *     lps_spec represents an LPS specification
 *Post:The datatypes of sort_expr are implemented as higher-order abstract data
 *     types in lps_spec
 *Ret: if the data implementation went ok, an equivalent version of sort_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_data_expr(ATermAppl data_expr,
  lps::specification &lps_spec);
/*Pre: data_expr represents a data expression that adheres to the internal
 *     ATerm structure after the type checking phase.
 *     lps_spec represents an LPS specification
 *Post:The datatypes of data_expr are implemented as higher-order abstract data
 *     types in lps_spec
 *Ret: if the data implementation went ok, an equivalent version of data_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_mult_act(ATermAppl mult_act,
  lps::specification &lps_spec);
/*Pre: mult_act represents a multiaction that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     lps_spec represents an LPS specification
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types in lps_spec
 *Ret: if the data implementation went ok, an equivalent version of mult_act
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_proc_expr(ATermAppl proc_expr,
  lps::specification &lps_spec);
/*Pre: proc_expr represents a process expression that adheres to the internal
 *     ATerm structure after the type checking phase.
 *     lps_spec represents an LPS specification
 *Post:The datatypes of proc_expr are implemented as higher-order abstract data
 *     types in lps_spec
 *Ret: if the data implementation went ok, an equivalent version of proc_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_state_frm(ATermAppl state_frm,
  lps::specification &lps_spec);
/*Pre: state_frm represents a state formula that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     lps_spec represents an LPS specification
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types in lps_spec
 *Ret: if the data implementation went ok, an equivalent version of state_frm
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_action_rename_spec(ATermAppl ar_spec,
  lps::specification &lps_spec);
/*Pre: ar_spec represents an action rename specification that
 *     adheres to the internal ATerm structure after the type checking fase.
 *     lps_spec represents an LPS specification
 *Post:The datatypes in action_rename_spec are implemented as higher-order
 *     abstract data types types and the data types of lps_spec are added
 *     to the data types of ar_spec.
 *Ret: if the data implementation went ok, an equivalent version of
 *     ar_spec is returned that adheres to the internal ATerm
 *     structure after data implementation, also containing the data
 *     specification of lps_spec
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

/* ============================================================================
 * Detailed data implementation, this is used by data reconstruction only!
 * ============================================================================
 */
ATermAppl impl_sort_list(ATermAppl sort_list, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_list is a list sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_list
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_list
//Post:an implementation of sort_list is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_list

ATermAppl impl_sort_set(ATermAppl sort_set, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_set is a set sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_set
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_set
//Post:an implementation of sort_set is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_set

ATermAppl impl_sort_bag(ATermAppl sort_bag, ATermList *p_substs,
  t_data_decls *p_data_decls);
//Pre: sort_bag is a bag sort
//     p_substs is a pointer to a list of substitutions induced by the context
//     of sort_bag
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of sort_bag
//Post:an implementation of sort_bag is added to *p_data_decls and new induced
//     substitutions are added *p_substs
//Ret: a sort identifier which is the implementation of sort_bag

void impl_sort_bool(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Bool is added to *p_data_decls

void impl_sort_pos(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Pos is added to *p_data_decls

void impl_sort_nat(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Nat is added to *p_data_decls

void impl_sort_nat_pair(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort PairNat is added to *p_data_decls

void impl_sort_int(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Int is added to *p_data_decls

void impl_sort_real(t_data_decls *p_data_decls);
//Pre: p_data_decls represents a pointer to new data declarations
//Post:an implementation of sort Real is added to *p_data_decls

void impl_function_sort(ATermAppl sort_arrow, t_data_decls *p_data_decls);
//Pre: sort_arrow is an arrow sort that adheres to the internal syntax after
//     data implementation
//     p_data_decls represents a pointer to new data declarations, induced by
//     the context of part
//Post:an implementation of function sort sort_arrow is added to *p_data_decls

#endif // MCRL2_DATAIMPL_H
