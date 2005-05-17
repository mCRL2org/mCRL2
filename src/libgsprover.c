#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsrewrite.h"

static ATermAppl current_spec;
static ATermAppl gsProverTrue, gsProverFalse;

void gsProverInit(ATermAppl Spec)
{
	current_spec = Spec;
	gsRewriteInit(ATAgetArgument(Spec,3),GS_REWR_INNER3);
	gsProverTrue = gsMakeDataExprTrue();
	gsProverFalse = gsMakeDataExprFalse();
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
	ATermAppl a3,var,sort,e,t;

	a1 = ATLgetFirst(l);
	l = ATgetNext(l);
	a2 = ATLgetFirst(l);
	l = ATgetNext(l);
	a3 = ATAgetFirst(l);

	var = ATAgetFirst(a1);
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
				na1 = ATappend(na1,(ATerm) v);
				t = gsMakeDataAppl(t,v);
			}
			s = ATmakeList1((ATerm) gsMakeSubst((ATerm) var,(ATerm) t));
			e = gsRewriteTerm((ATermAppl) gsSubstValues(s,(ATerm) a3,true));
			if ( !ATisEqual(e,gsProverFalse) )
			{
				r = ATinsert(r,(ATerm) ATmakeList3((ATerm) na1,gsSubstValues(s,(ATerm) a2,true),(ATerm) e));
			}
		}
	}
	r = ATreverse(r);

	return r;
}

static bool gsIsDataExprAnd(ATermAppl a, ATermAppl *a1, ATermAppl *a2)
{
	if ( gsIsDataAppl(a) )
	{
		ATermAppl h = ATAgetArgument(a,0);
		if ( gsIsDataAppl(h) )
		{
			ATermAppl o = ATAgetArgument(h,0);
			if ( gsIsOpId(o) )
			{
				*a1 = ATAgetArgument(h,1);
				*a2 = ATAgetArgument(a,1);
				return ATisEqual(o,gsMakeOpIdAnd());
			}
		}
	}

	return false;
}

static bool gsIsDataExprEq(ATermAppl a, ATermAppl *a1, ATermAppl *a2)
{
	if ( gsIsDataAppl(a) )
	{
		ATermAppl h = ATAgetArgument(a,0);
		if ( gsIsDataAppl(h) )
		{
			ATermAppl o = ATAgetArgument(h,0);
			if ( gsIsOpId(o) )
			{
				*a1 = ATAgetArgument(h,1);
				*a2 = ATAgetArgument(a,1);
				return ATisEqual(o,gsMakeOpIdEq(gsGetSort(*a1)));
			}
		}
	}

	return false;
}

static bool FindEquality(ATermAppl t, ATermList vars, ATermAppl *v, ATermAppl *e)
{
	ATermList s;
	ATermAppl a;

	s = ATmakeList1((ATerm) t);
	while ( !ATisEmpty(s) )
	{
		ATermAppl a1,a2;

		a = ATAgetFirst(s);
		s = ATgetNext(s);

		if ( gsIsDataExprAnd(a,&a1,&a2) )
		{
			s = ATinsert(s,(ATerm) a2);
			s = ATinsert(s,(ATerm) a1);
		} else if ( gsIsDataExprEq(a,&a1,&a2) ) {
			if ( !ATisEqual(a1,a2) )
			{
				if ( gsIsDataVarId(a1) && (ATindexOf(vars,(ATerm) a1,0) >= 0) )
				{
					*v = a1;
					*e = a2;
					return true;
				}
				if ( gsIsDataVarId(a2) && (ATindexOf(vars,(ATerm) a2,0) >= 0) )
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

static ATermList EliminateVars(ATermList l)
{
	ATermList vars,vals;
	ATermAppl t, v, e;

	vars = ATLgetFirst(l);
	l = ATgetNext(l);
	vals = ATLgetFirst(l);
	l = ATgetNext(l);
	t = ATAgetFirst(l);

	t = gsRewriteTerm(t);
	while ( !ATisEmpty(vars) && FindEquality(t,vars,&v,&e) )
	{
		vars = ATremoveElement(vars,(ATerm) v);
		vals = (ATermList) gsSubstValues(ATmakeList1((ATerm) gsMakeSubst((ATerm) v,(ATerm) e)),(ATerm) vals,true);
		t = (ATermAppl) gsSubstValues(ATmakeList1((ATerm) gsMakeSubst((ATerm) v,(ATerm) e)),(ATerm) t,true);
	t = gsRewriteTerm(t);
	}

	return ATmakeList3((ATerm) vars, (ATerm) gsRewriteTerms(vals), (ATerm) gsRewriteTerm(t));
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

ATermList FindSolutions(ATermList Vars, ATermAppl Expr)
{
	ATermList l,t,m,n,o;

	if ( ATisEmpty(Vars) )
	{
		if ( ATisEqual(gsRewriteTerm(Expr),gsProverTrue) )
		{
			return ATmakeList1((ATerm) ATmakeList0());
		} else {
			return ATmakeList0();
		}
	}

	o = ATmakeList3((ATerm) Vars,(ATerm) Vars,(ATerm) Expr);
	o = EliminateVars(o);
	if ( ATisEmpty(ATLgetFirst(o)) )
	{
		o = ATgetNext(o);
		if ( ATisEqual(ATgetFirst(ATgetNext(o)),gsProverTrue) )
		{
			return ATmakeList1((ATerm) makeSubsts(Vars,ATLgetFirst(o)));
		} else {
			if ( !ATisEqual(ATgetFirst(ATgetNext(o)),gsProverFalse) )
			{
				gsWarningMsg("term does not evaluate to true or false (%t)\n",ATgetFirst(ATgetNext(o)));
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
			for (; !ATisEmpty(n); n=ATgetNext(n))
			{
				o = ATLgetFirst(n);
				o = EliminateVars(o);
				if ( ATisEmpty(ATLgetFirst(o)) )
				{
					o = ATgetNext(o);
					if ( ATisEqual(ATgetFirst(ATgetNext(o)),gsProverTrue) )
					{
						m = ATinsert(m,(ATerm) makeSubsts(Vars,ATLgetFirst(o)));
					} else {
						if ( !ATisEqual(ATgetFirst(ATgetNext(o)),gsProverFalse) )
						{
//							ATfprintf(stderr,"Term does not evaluate to true or false (%t)\n",ATgetFirst(ATgetNext(o)));
							ATfprintf(stderr,"Term does not evaluate to true or false (");gsPrintPart(stderr,ATgetFirst(ATgetNext(o)),0,0);ATfprintf(stderr,")\n");
						}
					}
				} else {
					l = ATinsert(l,(ATerm) o);
				}
			}
		}
	}
	m = ATreverse(m);

	return m;
}
