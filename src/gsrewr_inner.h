/* $Id: libgsrewrite.c,v 1.3 2005/03/22 13:26:16 muck Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"

void rewrite_init_inner();

ATerm rewrite_inner(ATerm Term, int *b, ATermList vars);
