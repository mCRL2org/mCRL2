/* $Id: gsrewr_inner2.h,v 1.1 2005/04/08 09:49:58 muck Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

void rewrite_init_inner2();

void rewrite_add_inner2(ATermAppl eqn);
void rewrite_remove_inner2(ATermAppl eqn);

ATerm rewrite_inner2(ATerm Term, int *b);
