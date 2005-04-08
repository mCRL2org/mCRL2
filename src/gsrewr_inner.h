/* $Id: gsrewr_inner.h,v 1.2 2005/04/08 12:33:51 muck Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

void rewrite_init_inner();

void rewrite_add_inner(ATermAppl eqn);
void rewrite_remove_inner(ATermAppl eqn);

ATerm rewrite_inner(ATerm Term, int *b);
