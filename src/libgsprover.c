#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsrewrite.h"
#include "libgsprover.h"
#include "gssubstitute.h"

#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisInt(x) (ATgetType(x) == AT_INT)

bool FindSolutionsError;

static ATermAppl current_spec;
static ATerm gsProverTrue, gsProverFalse;

static int used_vars;

#define MAX_VARS_INIT	1000
#define MAX_VARS_FACTOR	5
static int max_vars = MAX_VARS_INIT;

static bool (*FindEquality)(ATerm,ATermList,ATerm*,ATerm*);
static bool FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
static bool FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
static ATerm opidAnd,eqstr;

void gsProverInit(ATermAppl Spec, int RewriteStrategy)
{
	current_spec = Spec;
	ATprotectAppl(&current_spec);
	gsRewriteInit(ATAgetArgument(Spec,3),RewriteStrategy);
	gsProverTrue = gsToRewriteFormat(gsMakeDataExprTrue());
	ATprotect(&gsProverTrue);
	gsProverFalse = gsToRewriteFormat(gsMakeDataExprFalse());
	ATprotect(&gsProverFalse);

	if ( RewriteStrategy == GS_REWR_INNER3 )
	{
		FindEquality = FindInner3Equality;
		opidAnd = gsToRewriteFormat(gsMakeOpIdAnd());
		ATprotect(&opidAnd);
		eqstr = (ATerm) gsString2ATermAppl("==");
		ATprotect(&eqstr);
	} else {
		FindEquality = FindInnerCEquality;
		opidAnd = ATgetArgument((ATermAppl) gsToRewriteFormat(gsMakeOpIdAnd()),0);
		ATprotect(&opidAnd);
		eqstr = (ATerm) gsString2ATermAppl("==");
		ATprotect(&eqstr);
	}
}

void gsProverFinalise()
{
	ATunprotectAppl(&current_spec);
	gsRewriteFinalise();
	ATunprotect(&gsProverTrue);
	ATunprotect(&gsProverFalse);

	ATunprotect(&opidAnd);
	ATunprotect(&eqstr);
}

static ATermAppl gsGetResult(ATermAppl sort)
{
	while ( gsIsSortArrow(sort) )
	{
		sort = ATAgetArgument(sort,1);
	}

	return sort;
}

static ATermList gsGetDomain(ATermAppl sort)
{
	ATermList l;

	l = ATmakeList0();
	while ( gsIsSortArrow(sort) )
	{
		l = ATinsert(l,ATgetArgument(sort,0));
		sort = ATAgetArgument(sort,1);
	}
	l = ATreverse(l);

	return l;
}

static ATermList calcNext(ATermList l)
{
	ATermList a1,a2,m,r,s,d,na1;
	ATermAppl var,sort,t;
	ATerm e,a3;

	a1 = ATLgetFirst(l);
	l = ATgetNext(l);
	a2 = ATLgetFirst(l);
	l = ATgetNext(l);
	a3 = ATgetFirst(l);

	var = ATAgetFirst(a1); // XXX is in internal format!
	a1 = ATgetNext(a1);
	sort = ATAgetArgument(var,1);

	if ( gsIsSortArrow(sort) )
	{
		gsErrorMsg("cannot enumerate all elements of functions sorts\n");
		exit(1);
	}

	r = ATmakeList0();

	m = ATLgetArgument(ATAgetArgument(current_spec,1),0);
	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		if ( ATisEqual(gsGetResult(ATAgetArgument(ATAgetFirst(m),1)),sort) )
		{
			na1 = a1;
			d = gsGetDomain(ATAgetArgument(ATAgetFirst(m),1));
			t = ATAgetFirst(m);
			for (; !ATisEmpty(d); d=ATgetNext(d))
			{
				ATermAppl v = gsMakeDataVarId(gsFreshString2ATermAppl("s",(ATerm) na1,false),ATAgetFirst(d));
				used_vars++;
				na1 = ATappend(na1,(ATerm) v);
				t = gsMakeDataAppl(t,v);
			}
			ATerm t_rf = gsToRewriteFormat(t);
			RWsetVariable((ATerm) var,t_rf);
			e = gsRewriteInternal(a3);
			RWclearVariable((ATerm) var);
			if ( !ATisEqual(e,gsProverFalse) )
			{
				s = ATmakeList1((ATerm) gsMakeSubst((ATerm) var, t_rf));
				r = ATinsert(r,(ATerm) ATmakeList3((ATerm) na1,gsSubstValues(s,(ATerm) a2,true),(ATerm) e));
			}
		}
	}
	r = ATreverse(r);

	return r;
}

static bool IsInner3Eq(ATerm a)
{
	if ( ATisInt(a) )
	{
		a = (ATerm) gsFromRewriteFormat(a);
	}

	if ( ATisEqual(ATgetArgument((ATermAppl) a,0),eqstr) )
	{
		return true;
	} else {
		return false;
	}
}

static bool FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
{
	ATermList s;
	ATerm a;

	s = ATmakeList1(t);
	while ( !ATisEmpty(s) )
	{
		ATerm a1,a2;

		a = ATgetFirst(s);
		s = ATgetNext(s);

		if ( !ATisList(a) )
		{
			continue;
		}

		if ( ATisEqual(ATgetFirst((ATermList) a),opidAnd) )
		{
			s = ATconcat(s,ATgetNext((ATermList) a));
		} else if ( IsInner3Eq(ATgetFirst((ATermList) a)) ) {
			a1 = ATgetFirst(ATgetNext((ATermList) a));
			a2 = ATgetFirst(ATgetNext(ATgetNext((ATermList) a)));
			if ( !ATisEqual(a1,a2) )
			{
				if ( ATisAppl(a1) && gsIsDataVarId((ATermAppl) a1) && (ATindexOf(vars, a1,0) >= 0) && !gsOccurs(a1,a2) )
				{
					*v = a1;
					*e = a2;
					return true;
				}
				if ( ATisAppl(a2) && gsIsDataVarId((ATermAppl) a2) && (ATindexOf(vars, a2,0) >= 0) && !gsOccurs(a2,a1) )
				{
					*v = a2;
					*e = a1;
					return true;
				}
			}
		}
	}

	return false;
}

//static ATerm *ceqs = NULL; // UNPROTECTED!!! (should be safe though)
//static int num_ceqs;
//static int ceqs_size;
//#define CEQS_STEP 20
static bool IsInnerCEq(ATermAppl a)
{
/*	if ( (ceqs == NULL) )
	{
		ceqs = (ATerm *) malloc(CEQS_STEP*sizeof(ATerm));
		ceqs_size = CEQS_STEP;
		num_ceqs = 0;
	}

	ATerm b = ATgetArgument(a,0);
	if ( ATisAppl(b) && gsIsDataVarId((ATermAppl) b) )
	{
		return false;
	}
	for (int i=0; i<num_ceqs; i++)
	{
		if ( ATisEqual(b,ceqs[i]) )
		{
			return true;
		}
	}
*/
	a = gsFromRewriteFormat((ATerm) a);
	a = (ATermAppl) ATgetArgument(a,0);
	a = (ATermAppl) ATgetArgument(a,0);

	if ( ATisEqual(ATgetArgument(a,0),eqstr) )
	{
/*		if ( num_ceqs >= ceqs_size )
		{
			ceqs_size += CEQS_STEP;
			ceqs = (ATerm *) realloc(ceqs,ceqs_size*sizeof(ATerm));
		}
		ceqs[num_ceqs] = (ATerm) a;
		num_ceqs++;
*/
		return true;
	} else {
		return false;
	}
}

static bool FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
{
	ATermList s;
	ATermAppl a;

	s = ATmakeList1((ATerm) t);
	while ( !ATisEmpty(s) )
	{
		ATermAppl a1,a2;

		a = (ATermAppl) ATgetFirst(s);
		s = ATgetNext(s);

		if ( gsIsDataVarId(a) || (ATgetArity(ATgetAFun(a)) != 3) )
		{
			continue;
		}

		if ( ATisEqual(ATgetArgument(a,0),opidAnd) )
		{
			s = ATinsert(s,ATgetArgument(a,2));
			s = ATinsert(s,ATgetArgument(a,1));
		} else if ( IsInnerCEq(a) ) {
			a1 = (ATermAppl) ATgetArgument(a,1);
			a2 = (ATermAppl) ATgetArgument(a,2);
			if ( !ATisEqual(a1,a2) )
			{
				if ( gsIsDataVarId(a1) && (ATindexOf(vars,(ATerm) a1,0) >= 0) && !gsOccurs((ATerm) a1,(ATerm) a2) )
				{
					*v = (ATerm) a1;
					*e = (ATerm) a2;
					return true;
				}
				if ( gsIsDataVarId(a2) && (ATindexOf(vars,(ATerm) a2,0) >= 0) && !gsOccurs((ATerm) a2,(ATerm) a1) )
				{
					*v = (ATerm) a2;
					*e = (ATerm) a1;
					return true;
				}
			}
		}
	}

	return false;
}

static ATermList EliminateVars(ATermList l)
{
	ATermList vars,vals,removed_vars,m;
	ATerm t, v, e;

	vars = ATLgetFirst(l);
	m = ATgetNext(l);
	vals = ATLgetFirst(m);
	m = ATgetNext(m);
	t = ATgetFirst(m);

//	t = gsRewriteInternal(t);
//	removed_vars = ATmakeList0();
	while ( !ATisEmpty(vars) && FindEquality(t,vars,&v,&e) )
	{
		vars = ATremoveElement(vars, v);
		RWsetVariable(v,e);
//		removed_vars = ATinsert(removed_vars,v);
//		vals = (ATermList) gsSubstValues(ATmakeList1((ATerm) gsMakeSubst(v,e)),(ATerm) vals,true);
		vals = gsRewriteInternals(vals);		
//		t = gsSubstValues(ATmakeList1((ATerm) gsMakeSubst(v,e)),t,true);
		t = gsRewriteInternal(t);
		RWclearVariable(v);
	}

/*	if ( ATisEmpty(removed_vars) )
	{
		return l;
	}*/
//	l = ATmakeList3((ATerm) vars, (ATerm) gsRewriteInternals(vals), (ATerm) t);
	l = ATmakeList3((ATerm) vars, (ATerm) vals, (ATerm) t);
/*	for (; !ATisEmpty(removed_vars); removed_vars=ATgetNext(removed_vars))
	{
		RWclearVariable(ATgetFirst(removed_vars));
	}*/
	return l;
}

static ATermList makeSubsts(ATermList vars, ATermList exprs)
{
	ATermList l;

	l = ATmakeList0();
	for (; !ATisEmpty(vars); vars=ATgetNext(vars),exprs=ATgetNext(exprs))
	{
		l = ATinsert(l,(ATerm) gsMakeSubst(ATgetFirst(vars),ATgetFirst(exprs)));
	}
	l = ATreverse(l);

	return l;
}

ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f)
{
	ATermList l,t,m,n,o;

	FindSolutionsError = false;

	used_vars = 0;

	if ( ATisEmpty(Vars) )
	{
		Expr = gsRewriteInternal(Expr);
		if ( ATisEqual(Expr,gsProverTrue) )
		{
			if ( f == NULL )
			{
				return ATmakeList1((ATerm) ATmakeList0());
			} else {
				f(ATmakeList0());
				return ATmakeList0();
			}
		} else {
			if ( !ATisEqual(Expr,gsProverFalse) )
			{
//				gsWarningMsg("term does not evaluate to true or false (%t)\n",ATgetFirst(ATgetNext(o)));
				ATfprintf(stderr,"Term does not evaluate to true or false: ");gsPrintPart(stderr,gsFromRewriteFormat(Expr),false,0);ATfprintf(stderr,"\n");
				FindSolutionsError = true;
			}
			return ATmakeList0();
		}
	}

//	o = ATmakeList3((ATerm) Vars,(ATerm) Vars,(ATerm) Expr);
	o = ATmakeList3((ATerm) Vars,(ATerm) Vars,gsRewriteInternal((ATerm) Expr));
	o = EliminateVars(o);
	if ( ATisEmpty(ATLgetFirst(o)) )
	{
		o = ATgetNext(o);
		if ( ATisEqual(ATgetFirst(ATgetNext(o)),gsProverTrue) )
		{
			if ( f == NULL )
			{
				return ATmakeList1((ATerm) makeSubsts(Vars,ATLgetFirst(o)));
			} else {
				f(makeSubsts(Vars,ATLgetFirst(o)));
				return ATmakeList0();
			}
		} else {
			if ( !ATisEqual(ATgetFirst(ATgetNext(o)),gsProverFalse) )
			{
//				gsWarningMsg("term does not evaluate to true or false (%t)\n",ATgetFirst(ATgetNext(o)));
				ATfprintf(stderr,"Term does not evaluate to true or false: ");gsPrintPart(stderr,gsFromRewriteFormat(ATgetFirst(ATgetNext(o))),false,0);ATfprintf(stderr,"\n");
				FindSolutionsError = true;
			}
			return ATmakeList0();
		}
	}
	l = ATmakeList1((ATerm) o);
	m = ATmakeList0();
	while ( !ATisEmpty(l) )
	{
		t = l;
		l = ATmakeList0();
		for (; !ATisEmpty(t); t=ATgetNext(t))
		{
			n = calcNext(ATLgetFirst(t));
			if ( used_vars > max_vars )
			{
				fprintf(stderr,"warning: Need more than %i variables to find all solutions for ",max_vars);gsPrintPart(stderr,gsFromRewriteFormat(Expr),false,0);fprintf(stderr,"\n");
				max_vars *= MAX_VARS_FACTOR;
			}
			for (; !ATisEmpty(n); n=ATgetNext(n))
			{
				o = ATLgetFirst(n);
				if ( ATisEmpty(ATLgetFirst(o)) )
				{
					o = ATgetNext(o);
					if ( ATisEqual(ATgetFirst(ATgetNext(o)),gsProverTrue) )
					{
						if ( f == NULL )
						{
							m = ATinsert(m,(ATerm) makeSubsts(Vars,ATLgetFirst(o)));
						} else {
							f(makeSubsts(Vars,ATLgetFirst(o)));
						}
					} else {
						if ( !ATisEqual(ATgetFirst(ATgetNext(o)),gsProverFalse) )
						{
//							ATfprintf(stderr,"Term does not evaluate to true or false (%t)\n",ATgetFirst(ATgetNext(o)));
							ATfprintf(stderr,"Term does not evaluate to true or false: ");gsPrintPart(stderr,gsFromRewriteFormat(ATgetFirst(ATgetNext(o))),false,0);ATfprintf(stderr,"\n");
							FindSolutionsError = true;
						}
					}
				} else {
					o = EliminateVars(o);
					l = ATinsert(l,(ATerm) o);
				}
			}
		}
	}
	m = ATreverse(m);

	return m;
}

#ifdef __cplusplus
}
#endif
