/* $Id: gsrewr_jitty.h,v 1.1 2005/04/08 09:49:58 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"

void rewrite_init_jitty();

void rewrite_add_jitty(ATermAppl eqn);
void rewrite_remove_jitty(ATermAppl eqn);

ATerm rewrite_jitty(ATerm Term, int *b);

#ifdef __cplusplus
}
#endif
