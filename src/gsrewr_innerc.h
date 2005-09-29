#ifndef NO_DYNLOAD
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

#define USING_INNERC

void RWrewrite_init_innerc();

void RWrewrite_add_innerc(ATermAppl eqn);
void RWrewrite_remove_innerc(ATermAppl eqn);

ATermAppl RWrewrite_innerc(ATermAppl);
ATermList RWrewritelist_innerc(ATermList);

ATerm to_rewrite_format_innerc(ATermAppl);

ATermAppl from_rewrite_format_innerc(ATerm);

#ifdef __cplusplus
}
#endif
#endif
