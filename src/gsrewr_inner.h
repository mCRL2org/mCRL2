#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"

void rewrite_init_inner();
void rewrite_finalise_inner();

void rewrite_add_inner(ATermAppl eqn);
void rewrite_remove_inner(ATermAppl eqn);

ATerm rewrite_inner(ATerm Term);

ATerm to_rewrite_format_inner(ATermAppl Term);
ATermAppl from_rewrite_format_inner(ATerm Term);

#ifdef __cplusplus
}
#endif
