#include <aterm2.h>
#include "lpe/specification.h"

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl gsImplementData(ATermAppl spec);
/*Pre: Spec represents a specification that adheres to the internal ATerm
 *     structure after the type checking phase.
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types.
 *Ret: if the data implementation went ok, an equivalent version of spec is
 *     returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl implement_data_state_formula(ATermAppl state_frm,
  lpe::specification &lpe_spec);
/*Pre: state_frm represents a state formula that adheres to the internal ATerm
 *     structure after the type checking phase.
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types.
 *Ret: if the data implementation went ok, an equivalent version of state_frm
 *     is returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */
