#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>
#include "lin_types.h"

ATermAppl linearise_std(ATermAppl spec, t_lin_options lin_options);
//Pre:  spec_term is an mCRL2 specification that adheres to the internal
//      structure after data implementation
//      lin_options represents the options that should be used during
//      linearisation
//Post: spec_term is linearised with the standard lineariser using options
//      lin_options
//Ret:  the linearised specification is everything went ok,
//      NULL, otherwise

#ifdef __cplusplus
}
#endif

