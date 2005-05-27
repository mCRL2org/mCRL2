/* $Id: gsrewr_inner.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "rewr_inner"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_inner.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;


static ATbool ATisAppl(ATerm a)
{
	return (ATbool) (ATgetType(a) == AT_APPL);
}
static ATbool ATisList(ATerm a)
{
	return (ATbool) (ATgetType(a) == AT_LIST);
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

void rewrite_init_inner()
{
}

void rewrite_add_inner(ATermAppl eqn)
{
}

void rewrite_remove_inner(ATermAppl eqn)
{
}


ATerm rewrite_inner(ATerm Term, int *b)
{
	ATermList l,m;
	int c,d,e;

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
					if ( gsIsNil(ATAgetArgument(ATAgetFirst(l),1)) || ATisEqual(rewrite_inner(ATgetArgument(ATAgetFirst(l),1),&e),gsMakeDataExprTrue()) )
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
				Term = (ATerm) ATsetArgument((ATermAppl) Term,rewrite_inner(ATgetArgument((ATermAppl) Term,0),&c),0);
				Term = (ATerm) ATsetArgument((ATermAppl) Term,rewrite_inner(ATgetArgument((ATermAppl) Term,1),&d),1);
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
						if ( gsIsNil(ATAgetArgument(ATAgetFirst(l),1)) || ATisEqual(rewrite_inner(gsSubstValues(m,ATgetArgument(ATAgetFirst(l),1),1),&e),gsMakeDataExprTrue()) )
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
						if ( gsIsNil(ATAgetArgument(ATAgetFirst(l),1)) || ATisEqual(rewrite_inner(ATgetArgument(ATAgetFirst(l),1),&e),gsMakeDataExprTrue()) )
						{
							return ATgetArgument(ATAgetFirst(l),3);
						}
					}
				}
			}
			return Term;
		} else {
			*b = 0;
			c = 1;
			Term = (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) Term),(ATermList) rewrite_inner((ATerm) ATgetArguments((ATermAppl) Term),&d));
			*b = d;
			return Term;
		}
	} else if ( ATisList(Term) )
	{
		l = ATmakeList0();
		*b = 0;
		for (; !ATisEmpty((ATermList) Term); Term=(ATerm) ATgetNext((ATermList) Term))
		{
			l = ATinsert(l,(ATerm) rewrite_inner(ATgetFirst((ATermList) Term),&c));
			*b |= c;
		}
		return (ATerm) ATreverse(l);
	}

	ATfprintf(stderr,"%s: term is not a ATermAppl or ATermList (%t)\n",NAME,Term);
	exit(1);
}

#ifdef __cplusplus
}
#endif
