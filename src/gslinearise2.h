/* $Id: gslinearise2.h,v 1.3 2005/03/03 18:37:46 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

ATermAppl gsLinearise2_nolpe(ATermAppl Spec);
ATermAppl gsLinearise2_nolpe_subst(ATermAppl Spec, int reuse_cycles);
ATermAppl gsLinearise2(ATermAppl Spec, int cluster);

#ifdef __cplusplus
}
#endif
