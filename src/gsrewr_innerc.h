/* $Id: gsrewr_innerc.h,v 1.2 2005/04/08 12:33:51 muck Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

void RWrewrite_init_innerc();

void RWrewrite_add_innerc(ATermAppl eqn);
void RWrewrite_remove_innerc(ATermAppl eqn);

ATermAppl RWrewrite_innerc(ATermAppl);
ATermList RWrewritelist_innerc(ATermList);

ATerm to_rewrite_format_innerc(ATermAppl);

ATermAppl from_rewrite_format_innerc(ATerm);

