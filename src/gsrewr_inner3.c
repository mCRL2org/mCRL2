/* $Id: gsrewr_inner3.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

//#define GS_CHECK_NFS

#define NAME "rewr_inner3"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_inner3.h"
#include "libgsparse.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable term2int;
static unsigned int num_opids;
static ATermAppl *int2term;
static ATermList *inner3_eqns;
static ATermInt trueint;
static bool isprotected = false;

static bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static bool ATisInt(ATerm a)
{
	return (ATgetType(a) == AT_INT);
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

static ATerm OpId2Int(ATermAppl Term, bool add_opids)
{
	ATermInt i;

	if ( (i = (ATermInt) ATtableGet(term2int,(ATerm) Term)) == NULL )
	{
		if ( !add_opids )
		{
			return (ATerm) Term;
		}
		i = ATmakeInt(num_opids);
		ATtablePut(term2int,(ATerm) Term,(ATerm) i);
		num_opids++;
	}

	return (ATerm) i;
}

static ATerm toInner(ATermAppl Term, bool add_opids)
{
	ATermList l;

	if ( !gsIsDataAppl(Term) )
	{
		if ( gsIsOpId(Term) )
		{
			return (ATerm) OpId2Int(Term,add_opids);
		} else {
			return (ATerm) Term;
		}
	}

	l = ATmakeList0();
	while ( gsIsDataAppl(Term) )
	{
		l = ATinsert(l,(ATerm) toInner(ATAgetArgument((ATermAppl) Term,1),add_opids));
		Term = ATAgetArgument(Term,0);
	}
	if ( gsIsOpId(Term) )
	{
		l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
	} else {
		l = ATinsert(l,(ATerm) Term);
	}

	return (ATerm) l;
}

static ATermAppl fromInner(ATerm Term)
{
	ATermList l;
	ATerm t;
	ATermAppl a;

	if ( !ATisList(Term) )
	{
		if ( ATisInt(Term) )
		{
			return int2term[ATgetInt((ATermInt) Term)];
		} else {
#ifdef GS_CHECK_NFS
			return (ATermAppl) ATremoveAnnotation(Term,gsMakeNil());
#else
			return (ATermAppl) Term;
#endif
		}
	}

	if ( ATisEmpty((ATermList) Term) )
	{
		ATfprintf(stderr,"%s: invalid inner format term (%t)\n",NAME,Term);
		exit(1);
	}
	
	l = (ATermList) Term;
#ifdef GS_CHECK_NFS
	t = ATremoveAnnotation(ATgetFirst(l),gsMakeNil());
#else
	t = ATgetFirst(l);
#endif
	if ( ATisInt(t) )
	{
		a = int2term[ATgetInt((ATermInt) t)];
	} else {
		a = (ATermAppl) t;
	}
	l = ATgetNext(l);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = gsMakeDataAppl(a,fromInner(ATgetFirst(l)));
	}

	return a;
}

void rewrite_init_inner3()
{
	ATermList l,m,n;
	ATermTable tmp_eqns;
	ATermInt i;

	tmp_eqns = ATtableCreate(100,100); // XXX would be nice to know the number op OpIds
	term2int = ATtableCreate(100,100);


#ifdef GS_CHECK_NFS
	trueint = (ATermInt) ATsetAnnotation(OpId2Int(gsMakeDataExprTrue(),true),gsMakeNil(),gsMakeNil());
#else
	trueint = (ATermInt) OpId2Int(gsMakeDataExprTrue(),true);
#endif
	if ( !isprotected )
	{
		ATprotectInt(&trueint);
	}

	l = opid_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
		ATtablePut(tmp_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
	}

	l = dataappl_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = (ATermList) toInner(ATAgetArgument(ATAgetFirst(l),2),true);
		if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetFirst(m))) == NULL )
		{
			n = ATmakeList0();
		}
		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATgetNext(m),toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		ATtablePut(tmp_eqns,ATgetFirst(m),(ATerm) n);
	}

	int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	inner3_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));

	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		int2term[ATgetInt(i)] = ATAgetFirst(l);
		if ( (m = (ATermList) ATtableGet(tmp_eqns,(ATerm) i)) == NULL )
		{
			inner3_eqns[ATgetInt(i)] = NULL;
		} else {
			inner3_eqns[ATgetInt(i)] = ATreverse(m);
		}
	}

	if ( !isprotected )
	{
		ATprotectArray((ATerm *) int2term,num_opids);
		ATprotectArray((ATerm *) inner3_eqns,num_opids);
	}

	ATtableDestroy(tmp_eqns);

	isprotected = true;
}

void rewrite_add_inner3(ATermAppl eqn)
{
	ATermList l,m;
	ATermAppl a;
	ATermInt i,j;
	unsigned int old_num;

	old_num = num_opids;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		j = (ATermInt) OpId2Int(a,true);
		m = ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
	} else {
		l = (ATermList) toInner(a,true);
		j = (ATermInt) ATgetFirst(l);
		m = ATmakeList4(ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
	}

	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		if ( ATgetInt(i) >= old_num )
		{
			int2term[ATgetInt(i)] = ATAgetFirst(l);
			inner3_eqns[ATgetInt(i)] = NULL;
		}
	}
	if ( inner3_eqns[ATgetInt(j)] == NULL )
	{
		inner3_eqns[ATgetInt(j)] = ATmakeList1((ATerm) m);
	} else {
		inner3_eqns[ATgetInt(j)] = ATappend(inner3_eqns[ATgetInt(j)],(ATerm) m);
	}
}

void rewrite_remove_inner3(ATermAppl eqn)
{
	ATermList l,m;
	ATermAppl a;
	ATerm t;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		t = OpId2Int(a,false);
		m = ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
	} else {
		l = (ATermList) toInner(a,false);
		t = ATgetFirst(l);
		m = ATmakeList4(ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
	}

	if ( ATisInt(t) )
	{
		l = inner3_eqns[ATgetInt((ATermInt) t)];
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( !ATisEqual(m,ATgetFirst(l)) )
			{
				ATinsert(m,ATgetFirst(l));
			}
		}
		if ( ATisEmpty(m) )
		{
			inner3_eqns[ATgetInt((ATermInt) t)] = NULL;
		} else {
			inner3_eqns[ATgetInt((ATermInt) t)] = ATreverse(m);
		}
	}
}

static ATerm subst_values(ATermList s, ATerm t)
{
	ATerm a,h;
	ATermList l,m;

	h = NULL;
#ifdef GS_CHECK_NFS
	ATremoveAnnotation(t,gsMakeNil());
#endif

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
	} else if ( ATisInt(p) || gsIsOpId((ATermAppl) p) )
	{
#ifdef GS_CHECK_NFS
		return ATisEqualModuloAnnotations(t,p);
#else
		return ATisEqual(t,p);
#endif
	} else /* if ( gsIsDataVarId((ATermAppl) p) ) */ {
		if ( ATindexOf(*vars,(ATerm) gsMakeSubst(p,t),0) >=0 )
		{
			return true;
		} else {
			// XXX Check type!! (?)
			l = *vars;
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
#ifdef GS_CHECK_NFS
				if ( ATisEqualModuloAnnotations(p,ATgetArgument(ATAgetFirst(l),0)) )
#else
				if ( ATisEqual(p,ATgetArgument(ATAgetFirst(l),0)) )
#endif
				{
					return false;
				}
			}
			*vars = ATappend(*vars,(ATerm) gsMakeSubst(p,t));
			return true;
		}
	}
}

#ifdef GS_CHECK_NFS
static ATerm setnormal(ATerm Term)
{
	return ATsetAnnotation(Term,(ATerm) gsMakeNil(),(ATerm) gsMakeNil());
}

static bool isnormal(ATerm Term)
{
	return ATgetAnnotation(Term,(ATerm) gsMakeNil()) != NULL;
}
#endif

static ATerm rewrite(ATerm Term, int *b, ATermList vars)
{
	ATerm t;
	ATermList l,l2,m,m2,n,o,o2,p;
//	ATermAppl v,s,t;
	int i,len,c,d,e;
	bool x;

#ifdef GS_CHECK_NFS
	if ( isnormal(Term) )
		return Term;
#endif

//ATfprintf(stderr,"rewrite(%t)\n\n",Term);
//ATfprintf(stderr,"rewrite(");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,") %p\n\n",&t);

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
			if ( !ATisInt(t) || (m = inner3_eqns[ATgetInt((ATermInt) t)]) == NULL )
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
						if ( is_nil(ATelementAt(ATLgetFirst(m),1)) || ATisEqual(rewrite(subst_values(p,ATelementAt(ATLgetFirst(m),1)),&e,ATmakeList0()),trueint) )
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
#ifdef GS_CHECK_NFS
		return setnormal(Term);
#else
		return Term;
#endif
	} else if ( ATisInt(Term) )
	{
		if ( (m = inner3_eqns[ATgetInt((ATermInt) Term)]) != NULL )
		{
			for (; !ATisEmpty(m); m=ATgetNext(m))
			{
				if ( ATisEmpty(ATLelementAt(ATLgetFirst(m),2)) )
				{
					if ( is_nil(ATelementAt(ATLgetFirst(m),1)) || ATisEqual(rewrite(ATelementAt(ATLgetFirst(m),1),&e,ATmakeList0()),trueint) )
					{
						*b = 1;
//ATfprintf(stderr,"return: %t\n\n",ATelementAt(ATLgetFirst(m),3));
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(ATelementAt(ATLgetFirst(m),3)),false,0);ATfprintf(stderr,"\n\n");
#ifdef GS_CHECK_NFS
						return setnormal(ATelementAt(ATLgetFirst(m),3));
#else
						return ATelementAt(ATLgetFirst(m),3);
#endif
					}		
					
				}
			}
		}
//ATfprintf(stderr,"return: %t\n\n",Term);
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,"\n\n");
#ifdef GS_CHECK_NFS
		return setnormal(Term);
#else
		return Term;
#endif
	} else {
//ATfprintf(stderr,"return: %t\n\n",Term);
//ATfprintf(stderr,"return: ");gsPrintPart(stderr,fromInner(Term),false,0);ATfprintf(stderr,"\n\n");
#ifdef GS_CHECK_NFS
		return setnormal(Term);
#else
		return Term;
#endif
	}
}

ATerm rewrite_inner3(ATerm Term, int *b)
{
	ATermList l;
	int c;
//ATfprintf(stderr,"input: %t\n",Term);
	if ( ATisList(Term) )
	{
		l = ATmakeList0();
		*b = 0;
		for (; !ATisEmpty((ATermList) Term); Term=(ATerm)ATgetNext((ATermList) Term))
		{
			l = ATinsert(l,(ATerm) fromInner(rewrite(toInner((ATermAppl) ATgetFirst((ATermList) Term),false),&c,ATmakeList0())));
			*b |= c;
		}
		return (ATerm) ATreverse(l);
	}

	return (ATerm) fromInner(rewrite(toInner((ATermAppl) Term,false),b,ATmakeList0()));
}

#ifdef __cplusplus
}
#endif
