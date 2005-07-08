
void RWsetVariable(ATerm v, ATerm t);
/* The variable v, which must be of the form DataVarId("name"),
   the term t, which must be in internal rewriting format, is
   associated. When rewriting, variable v will be replaced by
   term t. The variable v is replaced by all subsequent rewrites,
   until v is explicitly cleared */

void RWclearVariable(ATerm v);
/* The variable v will not be replaced by a term, until a 
   RWsetVariable(v,t) is executed. */

void RWclearAllVariables();
/* No variable v will be replaced by a term, until a 
   RWsetVariable(v,t) is executed. */

ATerm RWapplySubstitution(ATerm v);
/* Return the term to be substituted for variable v, or variable
   v itself if no value is associated with v. */
