/* $Id: libgsrewrite.c,v 1.3 2005/03/22 13:26:16 muck Exp $ */

#define NAME "rewr_inner"

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_inner.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;


static ATbool ATisAppl(ATerm a)
{
	return (ATgetType(a) == AT_APPL);
}
static ATbool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static ATbool match(ATerm a, ATerm m, ATermList *l)
{
	ATermList n;

	if ( ATisAppl(m) )
	{
		if ( !ATisAppl(a) )
		{
			return ATfalse;
		}
		if ( gsIsDataVarId((ATermAppl) m) )
		{
			// XXX Check type!! (?)
			if ( ATindexOf(*l,(ATerm) gsMakeSubst(m,a),0) >=0 )
			{
				return ATtrue;
			} else {
				n = *l;
				for (; !ATisEmpty(n); n=ATgetNext(n))
				{
					if ( ATisEqual(m,ATgetArgument(ATAgetFirst(n),0)) )
					{
						return ATfalse;
					}
				}
				*l = ATappend(*l,(ATerm) gsMakeSubst(m,a));
				return ATtrue;
			}
		}
		else
		if ( gsIsOpId((ATermAppl) m) )
		{
			return ATisEqual(m,a);
		}
		else
		if ( gsIsDataAppl((ATermAppl) m) )
		{
			if ( gsIsDataAppl((ATermAppl) a) )
			{
				if ( match(ATgetArgument((ATermAppl) a,0),ATgetArgument((ATermAppl) m,0),l) )
				{
					return match(ATgetArgument((ATermAppl) a,1),ATgetArgument((ATermAppl) m,1),l);
				} else {
					return ATfalse;
				}
			} else {
				return ATfalse;
			}
		}
		else
		{
			if ( ATisEqualAFun(ATgetAFun((ATermAppl) m),ATgetAFun((ATermAppl) a)) )
			{
				return match((ATerm) ATgetArguments((ATermAppl) m),(ATerm) ATgetArguments((ATermAppl) a),l);
			} else {
				return ATfalse;
			}
		}
	} else if ( ATisList(m) )
	{
		if ( !ATisList(a) || (ATgetLength((ATermList) m) != ATgetLength((ATermList) a)) )
		{
			return ATfalse;
		}
		while ( !ATisEmpty((ATermList) m) )
		{
			if ( !match(ATgetFirst((ATermList) m),ATgetFirst((ATermList) a),l) )
			{
				return ATfalse;
			}
			m = (ATerm) ATgetNext((ATermList) m);
			a = (ATerm) ATgetNext((ATermList) a);
		}
		return ATtrue;
	}
	
	ATfprintf(stderr,"%s: unknown expression (%t)\n",NAME,m);
	return ATfalse;
}

static ATermList merge_list(ATermList l, ATermList m)
{
	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		if ( ATindexOf(l,ATgetFirst(m),0) == -1 )
		{
			l = ATappend(l,ATgetFirst(m));
		}
	}

	return l;
}

static ATermList occurs_in(ATerm Term, ATermList l)
{
	ATermList r,m;

	if ( ATisAppl(Term) )
	{
		if ( gsIsDataVarId((ATermAppl) Term) )
		{
			if ( ATindexOf(l,Term,0) >= 0 )
			{
				return ATmakeList1(Term);
			} else {
				return ATmakeList0();
			}
		} else {
			return occurs_in((ATerm) ATgetArguments((ATermAppl) Term),l);
		}
	} else if ( ATisList(Term) )
	{
		r = ATmakeList0();
		m = (ATermList) Term;
		for (; !ATisEmpty(m); m=ATgetNext(m))
		{
			r = merge_list(r,occurs_in(ATgetFirst(m),l));
		}
		return r;
	}
	
	ATfprintf(stderr,"%s: unknown expression (%t)\n",NAME,Term);
	return ATfalse;
}

static ATbool is_and(ATermAppl d)
{
	if ( gsIsDataAppl(d) )
	{
		if ( gsIsDataAppl(ATAgetArgument(d,0)) )
		{
			return ATisEqual(ATAgetArgument(ATAgetArgument(d,0),0),gsMakeOpIdAnd());
		}
	}

	return ATfalse;
}

static ATbool check_eq(ATermAppl d, ATermAppl *s)
{
	ATermAppl sort;

	if ( gsIsOpId(d) )
	{
		sort = ATAgetArgument(d,1);
		if ( gsIsSortArrow(sort) )
		{
			if ( gsIsSortArrow(ATAgetArgument(sort,1)) )
			{
				*s = ATAgetArgument(sort,0);
				return ATtrue;
			}
		}
	}

	return ATfalse;
}

static ATbool is_eq(ATermAppl d, ATermList l, ATermAppl *v, ATermAppl *s)
{
	ATermAppl sort;

	if ( gsIsDataAppl(d) )
	{
		if ( gsIsDataAppl(ATAgetArgument(d,0)) )
		{
			if ( check_eq(ATAgetArgument(ATAgetArgument(d,0),0),&sort) )
			{
				if ( ATisEqual(ATAgetArgument(ATAgetArgument(d,0),0),gsMakeOpIdEq(sort)) )
				{
					if ( ATindexOf(l,ATgetArgument(d,1),0) >= 0 )
					{
						*v = ATAgetArgument(d,1);
						*s = ATAgetArgument(ATAgetArgument(d,0),1);
						return ATtrue;
					} else if ( ATindexOf(l,ATgetArgument(ATAgetArgument(d,0),1),0) >= 0 )
					{
						*v = ATAgetArgument(ATAgetArgument(d,0),1);
						*s = ATAgetArgument(d,1);
						return ATtrue;
					}
				}
			}
		}
	}

	return ATfalse;
}

static ATbool find_eq(ATermAppl d, ATermList l, ATermAppl *v, ATermAppl *s)
{
	if ( is_and(d) )
	{
		return find_eq(ATAgetArgument(ATAgetArgument(d,0),1),l,v,s) || find_eq(ATAgetArgument(d,1),l,v,s);
	} else if ( is_eq(d,l,v,s) )
	{
		return ATtrue;
	} else {
		return ATfalse;
	}
}

void rewrite_init_inner()
{
}

ATerm rewrite_inner(ATerm Term, int *b, ATermList vars)
{
	ATermList l,m;
	int c,d;

	if ( ATisAppl(Term) )
	{
		if ( gsIsDataVarId((ATermAppl) Term) )
		{
			*b = 0;
			return Term;
		} else if ( gsIsOpId((ATermAppl) Term) )
		{
			l = opid_eqns;
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				if ( ATisEqual(Term,ATAgetArgument(ATAgetFirst(l),2)) )
				{
					if ( gsIsNil(ATAgetArgument(ATAgetFirst(l),1)) || ATisEqual(ATAgetArgument(ATAgetFirst(l),1),gsMakeDataExprTrue()) )
					{
						*b = 1;
						return ATgetArgument(ATAgetFirst(l),3);
					}
				}
			}
			*b = 0;
			return Term;
		} else if ( gsIsDataAppl((ATermAppl) Term) )
		{
			*b = 0;
			c = 1;
			while ( c && gsIsDataAppl((ATermAppl) Term) ) 
			{
				Term = (ATerm) ATsetArgument((ATermAppl) Term,rewrite_inner(ATgetArgument((ATermAppl) Term,0),&c,vars),0);
				Term = (ATerm) ATsetArgument((ATermAppl) Term,rewrite_inner(ATgetArgument((ATermAppl) Term,1),&d,vars),1);
				c |= d;
				if ( c )
				{
					*b = 1;
				}
				c = 0;
				l = dataappl_eqns;
				for (; !ATisEmpty(l); l=ATgetNext(l))
				{
					m = ATmakeList0();
					if ( match(Term,ATgetArgument(ATAgetFirst(l),2),&m) )
					{
						if ( gsIsNil(ATAgetArgument(ATAgetFirst(l),1)) || ATisEqual(gsSubstValues(m,ATgetArgument(ATAgetFirst(l),1),1),gsMakeDataExprTrue()) )
						{
							*b = 1;
							c = 1;
							Term = gsSubstValues(m,ATgetArgument(ATAgetFirst(l),3),1);
						}
					}
				}
			}
			if ( gsIsDataVarId((ATermAppl) Term) )
			{
				l = opid_eqns;
				for (; !ATisEmpty(l); l=ATgetNext(l))
				{
					if ( ATisEqual(Term,ATAgetArgument(ATAgetFirst(l),2)) )
					{
						if ( gsIsNil(ATAgetArgument(ATAgetFirst(l),1)) || ATisEqual(ATAgetArgument(ATAgetFirst(l),1),gsMakeDataExprTrue()) )
						{
							return ATgetArgument(ATAgetFirst(l),3);
						}
					}
				}
			}
			return Term;
		} else {
			return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) Term),(ATermList) rewrite_inner((ATerm) ATgetArguments((ATermAppl) Term),b,vars));
		}
	} else if ( ATisList(Term) )
	{
		l = ATmakeList0();
		*b = 0;
		for (; !ATisEmpty((ATermList) Term); Term=(ATerm) ATgetNext((ATermList) Term))
		{
			l = ATinsert(l,(ATerm) rewrite_inner(ATgetFirst((ATermList) Term),&c,vars));
			*b |= c;
		}
		return (ATerm) ATreverse(l);
	}

	ATfprintf(stderr,"%s: term is not a ATermAppl or ATermList (%t)\n",NAME,Term);
	exit(1);
}
