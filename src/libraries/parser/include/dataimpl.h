#include <aterm2.h>
#include "lpe/specification.h"

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

ATermAppl implement_data_data_expr(ATermAppl data_expr,
  lpe::specification &lpe_spec);
/*Pre: data_expr represents a state formula that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     lpe_spec represents an LPE specification
 *Post:The datatypes of data_expr are implemented as higher-order abstract data
 *     types in lpe_spec
 *Ret: if the data implementation went ok, an equivalent version of data_expr
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_mult_act(ATermAppl mult_act,
  lpe::specification &lpe_spec);
/*Pre: mult_act represents a multiaction that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     lpe_spec represents an LPE specification
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types in lpe_spec
 *Ret: if the data implementation went ok, an equivalent version of mult_act
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_state_frm(ATermAppl state_frm,
  lpe::specification &lpe_spec);
/*Pre: state_frm represents a state formula that adheres to the internal ATerm
 *     structure after the type checking phase.
 *     lpe_spec represents an LPE specification
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types in lpe_spec
 *Ret: if the data implementation went ok, an equivalent version of state_frm
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */
