#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl gsImplementData(ATermAppl Spec);
/*Pre: Spec represents a specification that adheres to the internal ATerm
 *     structure after the type checking phase.
 *Post:The datatypes of spec are implemented as higher-order abstract data
 *     types.
 *Ret: if the data implementation went ok, an equivalent version spec is
 *     returned that adheres to the internal ATerm structure after data
 *     implementation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

#ifdef __cplusplus
}
#endif
