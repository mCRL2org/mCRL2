#include <aterm2.h>

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl translate_reg_frms(ATermAppl state_frm);
/*Pre: state_frm represents a state formula that adheres to the internal ATerm
 *     structure after the data implementation phase.
 *Ret: state_frm in which all regular formulas are translated in
 *     terms of state and action formulas
 */
