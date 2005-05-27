/* $Id: gsrewr_inner2.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "rewr_inner2"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_inner2.h"
#include "libgsparse.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable inner2_eqns;

static ATbool ATisList(ATerm a)
{
	return (ATbool) (ATgetType(a) == AT_LIST);
}

static bool is_nil(ATerm t)
{
	if ( ATisList(t) )
	{
		return false;
	} else {
		return gsIsNil((ATermAppl) t);
	}
}

static ATerm toInner(ATermAppl Term)
{
	ATermList l;

	if ( !gsIsDataAppl(Term) )
	{
		return (ATerm) Term;
	}

	l = ATmakeList0();
	while ( gsIsDataAppl(Term) )
	{
		l = ATinsert(l,(ATerm) toInner(ATAgetArgument((ATermAppl) Term,1)));
		Term = ATAgetArgument(Term,0);
	}
	l = ATinsert(l,(ATerm) Term);

	return (ATerm) l;
}

static ATermAppl fromInner(ATerm Term)
{
	ATermList l;
	ATermAppl a;

	if ( !ATisList(Term) )
	{
		return (ATermAppl) Term;
	}

	if ( ATisEmpty((ATermList) Term) )
	{
		ATfprintf(stderr,"%s: invalid inner format term (%t)\n",NAME,Term);
		exit(1);
	}
	
	l = (ATermList) Term;
	a = ATAgetFirst(l);
	l = ATgetNext(l);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = gsMakeDataAppl(a,fromInner(ATgetFirst(l)));
	}

	return a;
}

void rewrite_init_inner2()
{
	ATermList l,m,n;

	inner2_eqns = ATtableCreate(100,100); // XXX would be nice to know the number op OpIds

	l = opid_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
		ATtablePut(inner2_eqns,ATgetArgument(ATAgetFirst(l),2),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),1)),(ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),3)))));
	}

	l = dataappl_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = (ATermList) toInner(ATAgetArgument(ATAgetFirst(l),2));
		if ( (n = (ATermList) ATtableGet(inner2_eqns,ATgetFirst(m))) == NULL )
		{
			n = ATmakeList0();
		}
		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),toInner(ATAgetArgument(ATAgetFirst(l),1)),(ATerm) ATgetNext(m),toInner(ATAgetArgument(ATAgetFirst(l),3))));
		ATtablePut(inner2_eqns,ATgetFirst(m),(ATerm) n);
	}

	l = ATtableKeys(inner2_eqns);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX Perhaps sort on number of parameters?
		ATtablePut(inner2_eqns,ATgetFirst(l),(ATerm) ATreverse((ATermList) ATtableGet(inner2_eqns,ATgetFirst(l))));
	}
}

void rewrite_add_inner2(ATermAppl eqn)
{
	ATermList l,m;
	ATermAppl a;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		m = ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1)),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3)));
	} else {
		l = (ATermList) toInner(a);
		a = ATAgetFirst(l);
		m = ATmakeList4(ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1)),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3)));
	}

	if ( (l = (ATermList) ATtableGet(inner2_eqns,(ATerm) a)) == NULL )
	{
		ATtablePut(inner2_eqns,(ATerm) a,(ATerm) ATmakeList1((ATerm) m));
	} else {
		l = ATappend(l,(ATerm) m); 
		ATtablePut(inner2_eqns,(ATerm) a,(ATerm) l);
	}
}


void rewrite_remove_inner2(ATermAppl eqn)
{
	ATermList l,m,n;
	ATermAppl a;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		m = ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1)),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3)));
	} else {
		l = (ATermList) toInner(a);
		a = ATAgetFirst(l);
		m = ATmakeList4(ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1)),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3)));
	}

	if ( (l = (ATermList) ATtableGet(inner2_eqns,(ATerm) a)) != NULL )
	{
		n = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( !ATisEqual(m,ATgetFirst(l)) )
			{
				n = ATinsert(n,ATgetFirst(l));
			}
		}
		if ( ATisEmpty(n) )
		{
			ATtableRemove(inner2_eqns,(ATerm) a);
		} else {
			ATtablePut(inner2_eqns,(ATerm)a,(ATerm) ATreverse(n));
		}
	}
}

static ATerm subst_values(ATermList s, ATerm t)
{
	ATerm a,h;
	ATermList l,m;

	h = NULL;

	if ( ATisList(t) )
	{
		if ( ATisEmpty((ATermList) t) )
		{
			return t;
		}
		l = ATreverse((ATermList) t);
		m = ATmakeList0();
		while ( !ATisEmpty(l) )
		{
			a = subst_values(s,ATgetFirst(l));
			l = ATgetNext(l);
			if ( ATisEmpty(l) )
			{
				h = a;
			} else {
				m = ATinsert(m,a);
			}
		}
		if ( ATisList(h) )
		{
			return (ATerm) ATconcat((ATermList) h,m);
		} else {
			return (ATerm) ATinsert(m,h);
		}
	} else if ( gsIsDataVarId((ATermAppl) t) )
	{
		for (; !ATisEmpty(s); s=ATgetNext(s))
		{
			if ( ATisEqual(t,ATgetArgument(ATAgetFirst(s),0)) )
			{
				return ATgetArgument(ATAgetFirst(s),1);
			}
		}
		ATfprintf(stderr,"%s: variable %t not assigned\n",NAME,t);
		exit(1);
	} else {
		return t;
	}
}

static bool match_inner(ATerm t, ATerm p, ATermList *vars)
{
	bool b;
	ATermList l;

//ATfprintf(stderr,"match_inner(  %t  ,  %t  ,  %t   )\n\n",t,p,*vars);
	if ( ATisList(p) )
	{
		if ( ATisList(t) )
		{
			if ( ATgetLength((ATermList) t) == ATgetLength((ATermList) p) )
			{
				b = true;
				for (; !ATisEmpty((ATermList) t) && b; t=(ATerm) ATgetNext((ATermList) t),p=(ATerm) ATgetNext((ATermList) p))
				{
					b &= match_inner(ATgetFirst((ATermList) t),ATgetFirst((ATermList) p),vars);
				}
				return b;
			}
		}
		return false;
	} else if ( gsIsOpId((ATermAppl) p) )
	{
		return ATisEqual(t,p);
	} else /* if ( gsIsDataVarId((ATermAppl) p) ) */ {
		if ( ATindexOf(*vars,(ATerm) gsMakeSubst(p,t),0) >=0 )
		{
			return true;
		} else {
			// XXX Check type!! (?)
			l = *vars;
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				if ( ATisEqual(p,ATgetArgument(ATAgetFirst(l),0)) )
				{
					return false;
				}
			}
			*vars = ATappend(*vars,(ATerm) gsMakeSubst(p,t));
			return true;
		}
	}
}

static ATerm rewrite(ATerm Term, int *b, ATermList vars)
{
	ATerm t;
	ATermList l,l2,m,m2,n,o,o2,p;
//	ATermAppl v,s,t;
	int i,len,c,d,e;
	bool x;

//ATfprintf(stderr,"rewrite(%t)\n\n",Term);
//ATfprintf(stderr,"rewrite(");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,")\n\n");

	l2 = NULL;

	if ( ATisList(Term) )
	{
		x = true;
		while ( x )
		{
//ATfprintf(stderr,"loop: %t\n\n",Term);
//ATfprintf(stderr,"loop: ");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,"\n\n");
			x = false;
			d = 0;
			t = rewrite(ATgetFirst((ATermList) Term),&d,vars);
			l = ATgetNext((ATermList) Term);
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,rewrite(ATgetFirst(l),&c,vars));
				d |= c;
			}
			l = ATreverse(m);
			if ( d ) 
			{
				*b = 1;
			}
			if ( ATisList(t) )
			{
				l = ATconcat(ATgetNext((ATermList) t),l);
				t = ATgetFirst((ATermList) t);
			}
			Term = (ATerm) ATinsert(l,t);
			if ( (m = (ATermList) ATtableGet(inner2_eqns,(ATerm) t)) == NULL )
			{
				break;
			}
			o = ATmakeList0();
			len = ATgetLength(m);
			for (i=0; i<len; i++)
			{
				o = ATinsert(o,(ATerm) ATmakeList0());
			}
//ATfprintf(stderr,"Trying %t...\n\n",Term);
			while ( !ATisEmpty(m) )
			{
//ATfprintf(stderr,"%t matches? %t\n\n",l,m);
				if ( !ATisEmpty(l) )
				{
					t = ATgetFirst(l);
					l2 = ATgetNext(l);
				}
				n = ATmakeList0();
				o2 = ATmakeList0();
				for (; !ATisEmpty(m); m=ATgetNext(m),o=ATgetNext(o))
				{
					p = ATLgetFirst(o);
					if ( ATisEmpty(ATLelementAt(ATLgetFirst(m),2)) )
					{
						// XXX check ATisEmpty(l)
						if ( is_nil(ATelementAt(ATLgetFirst(m),1)) || ATisEqual(rewrite(subst_values(p,ATelementAt(ATLgetFirst(m),1)),&e,ATmakeList0()),gsMakeDataExprTrue()) )
						{
//ATfprintf(stderr,"apply %t\n\n",ATgetFirst(m));
							*b = 1;
							Term = subst_values(p,ATelementAt(ATLgetFirst(m),3));
							if ( !ATisEmpty(l) )
							{
								if ( ATisList(Term) )
								{
									Term = (ATerm) ATconcat((ATermList) Term,l);
								} else {
									Term = (ATerm) ATinsert(l,Term);
								}
							}
							if ( ATisList(Term) )
							{
								x = true;
							} else {
								Term = rewrite(Term,&c,vars);
							}
							n = ATmakeList0();
							break;
						}		
					} else if ( !ATisEmpty(l) ) {
						m2 = ATLelementAt(ATLgetFirst(m),2);
						if ( match_inner(t,ATgetFirst(m2),&p) )
						{
							n = ATinsert(n,(ATerm) ATreplace(ATLgetFirst(m),(ATerm) ATgetNext(m2),2));
							o2 = ATinsert(o2,(ATerm) p);
						}
					}
				}
				m = ATreverse(n);
				o = ATreverse(o2);
				if ( !ATisEmpty(l) ) 
				{
					l = l2;
				}
			}
		}
//ATfprintf(stderr,"return: %t\n\n",Term);
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,"\n\n");
		return Term;
	} else if ( gsIsOpId((ATermAppl) Term) )
	{
		if ( (m = (ATermList) ATtableGet(inner2_eqns,Term)) != NULL )
		{
			for (; !ATisEmpty(m); m=ATgetNext(m))
			{
				if ( ATisEmpty(ATLelementAt(ATLgetFirst(m),2)) )
				{
					if ( is_nil(ATelementAt(ATLgetFirst(m),1)) || ATisEqual(rewrite(ATelementAt(ATLgetFirst(m),1),&e,ATmakeList0()),gsMakeDataExprTrue()) )
					{
						*b = 1;
//ATfprintf(stderr,"return: %t\n\n",ATelementAt(ATLgetFirst(m),3));
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(ATelementAt(ATLgetFirst(m),3)),false,0);ATfprintf(stderr,"\n\n");
						return ATelementAt(ATLgetFirst(m),3);
					}		
					
				}
			}
		}
//ATfprintf(stderr,"return: %t\n\n",Term);
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,"\n\n");
		return Term;
	} else {
//ATfprintf(stderr,"return: %t\n\n",Term);
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,"\n\n");
		return Term;
	}
}

ATerm rewrite_inner2(ATerm Term, int *b)
{
	ATermList l;
	int c;

	if ( ATisList(Term) )
	{
		l = ATmakeList0();
		*b = 0;
		for (; !ATisEmpty((ATermList) Term); Term=(ATerm)ATgetNext((ATermList) Term))
		{
			l = ATinsert(l,(ATerm) fromInner(rewrite(toInner((ATermAppl) ATgetFirst((ATermList) Term)),&c,ATmakeList0())));
			*b |= c;
		}
		return (ATerm) ATreverse(l);
	}

	return (ATerm) fromInner(rewrite(toInner((ATermAppl) Term),b,ATmakeList0()));
}

#ifdef __cplusplus
}
#endif
