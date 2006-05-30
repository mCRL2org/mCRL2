#include <aterm2.h>
#include "lin_types.h"

ATermAppl linearise_alt(ATermAppl Spec, t_lin_options lin_options);
//Pre:  spec_term is an mCRL2 specification that adheres to the internal
//      structure after data implementation
//      lin_options represents the options that should be used during
//      linearisation
//Post: spec_term is linearised with the alternative lineariser using options
//      lin_options
//Ret:  the linearised specification is everything went ok,
//      NULL, otherwise

//Linearisation functions needed by the experimental tool lin
ATermAppl linearise_alt_nolpe(ATermAppl Spec);
ATermAppl linearise_alt_nolpe_subst(ATermAppl Spec, int reuse_cycles);
ATermAppl linearise_alt_statespace(ATermAppl Spec, bool lpe);
