/* $Id: gsrewr_innerc.h,v 1.2 2005/04/08 12:33:51 muck Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

void rewrite_init_innerc();

void rewrite_add_innerc(ATermAppl eqn);
void rewrite_remove_innerc(ATermAppl eqn);

ATerm rewrite_innerc(ATerm Term, int *b);
