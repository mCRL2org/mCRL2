#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl gsTypeCheck(ATermAppl Spec);
/*Pre: spec represents a specification that adheres to the initial internal
 *     ATerm structure.
 *Post:spec is type checked.
 *Ret: if the type checking went ok, an equivalent version of spec is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appriopriate error message is printed and
 *     NULL is returned.
 */

#ifdef __cplusplus
}
#endif
