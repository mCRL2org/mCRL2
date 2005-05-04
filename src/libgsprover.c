#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsrewrite.h"

static ATermAppl current_spec;
static ATermAppl gsProverTrue, gsProverFalse;

void gsProverInit(ATermAppl Spec)
{
	current_spec = Spec;
	gsRewriteInit(ATAgetArgument(Spec,3),GS_REWR_INNER);
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

	l = ATmakeList1((ATerm) ATmakeList3((ATerm) Vars,(ATerm) Vars,(ATerm) Expr));
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
				if ( ATisEmpty(ATLgetFirst(o)) )
				{
					o = ATgetNext(o);
					if ( ATisEqual(ATgetFirst(ATgetNext(o)),gsProverTrue) )
					{
						m = ATinsert(m,(ATerm) makeSubsts(Vars,ATLgetFirst(o)));
					} else {
						if ( !ATisEqual(ATgetFirst(ATgetNext(o)),gsProverFalse) )
						{
							ATfprintf(stderr,"Term does not evaluator to true or false (%t)\n",ATgetFirst(ATgetNext(o)));
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
