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

ATermAppl implement_data_action_rename(ATermAppl action_rename,
  lps::specification &lps_spec);
/*Pre: action_rename represents a action rename file that adheres to the
 *     internal ATerm structure after the type checking fase. lps_spec
 *     represents an LPS specification
 *Post:The datatypes in action_rename are implemented as higher-order
 *     abstract data and the datatypes are merged with the datatypes in
 *     lps_spec.
 *Ret: if the data implementation went ok, an equivalent version of lps_spec
 *     is returned that adheres to the internal ATerm structure containing
 *     all the original data and all data from action_rename
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

#endif // MCRL2_DATAIMPL_H
