/* $Id: gsrewr_inner3.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "rewr_inner3"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_inner3.h"
#include "libgsparse.h"
#include "gssubstitute.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable term2int;
static unsigned int num_opids;
static ATermAppl *int2term;
static ATermList *inner3_eqns;
static ATermInt trueint;
static AFun nilAFun;
static AFun opidAFun;
static int max_vars;
static bool is_initialised = false;

//static ATermTable subst_table = NULL;
//static bool subst_is_inner = false;

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)

#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisInt(x) (ATgetType(x) == AT_INT)

/*static bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static bool ATisInt(ATerm a)
{
	return (ATgetType(a) == AT_INT);
}*/

#define is_nil(x) (ATisList(x)?false:(ATgetAFun((ATermAppl) x) == nilAFun))

/*static bool is_nil(ATerm t)
{
	if ( ATisList(t) )
	{
		return false;
	} else {
		return gsIsNil((ATermAppl) t);
	}
}*/

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
			return (ATermAppl) Term;
		}
	}

	if ( ATisEmpty((ATermList) Term) )
	{
		ATfprintf(stderr,"%s: invalid inner format term (%t)\n",NAME,Term);
		exit(1);
	}
	
	l = (ATermList) Term;
	t = ATgetFirst(l);
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

	if ( is_initialised )
	{
		ATtableDestroy(term2int);
		ATunprotectInt(&trueint);
		ATunprotectAFun(nilAFun);
		ATunprotectAFun(opidAFun);
		ATunprotectArray((ATerm *) int2term);
		ATunprotectArray((ATerm *) inner3_eqns);
	}
	is_initialised = true;

	tmp_eqns = ATtableCreate(100,100); // XXX would be nice to know the number op OpIds
	term2int = ATtableCreate(100,100);

	max_vars = 0;

	trueint = (ATermInt) OpId2Int(gsMakeDataExprTrue(),true);
	ATprotectInt(&trueint);

	nilAFun = ATgetAFun(gsMakeNil());
	ATprotectAFun(nilAFun);
	opidAFun = ATgetAFun(gsMakeDataExprTrue());
	ATprotectAFun(opidAFun);

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
		if ( ATgetLength(ATgetArgument(ATAgetFirst(l),0)) > max_vars)
		{
			max_vars = ATgetLength(ATgetArgument(ATAgetFirst(l),0));
		}
		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATgetNext(m),toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		ATtablePut(tmp_eqns,ATgetFirst(m),(ATerm) n);
	}

	int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	inner3_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
	for (int i=0; i<num_opids; i++)
	{
		int2term[i] = NULL;
		inner3_eqns[i] = NULL;
	}
	ATprotectArray((ATerm *) int2term,num_opids);
	ATprotectArray((ATerm *) inner3_eqns,num_opids);

	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		int2term[ATgetInt(i)] = ATAgetFirst(l);
		if ( (m = (ATermList) ATtableGet(tmp_eqns,(ATerm) i)) != NULL )
		{
			inner3_eqns[ATgetInt(i)] = ATreverse(m);
		}
	}

	ATtableDestroy(tmp_eqns);
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

	if ( num_opids > old_num )
	{
		ATunprotectArray((ATerm *) int2term);
		ATunprotectArray((ATerm *) inner3_eqns);

		int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
		inner3_eqns = (ATermList *) realloc(inner3_eqns,num_opids*sizeof(ATermList));
		for (int i=old_num; i<num_opids; i++)
		{
			int2term[i] = NULL;
			inner3_eqns[i] = NULL;
		}
		ATprotectArray((ATerm *) int2term,num_opids);
		ATprotectArray((ATerm *) inner3_eqns,num_opids);

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
	}

	if ( ATgetLength(ATgetArgument(eqn,0)) > max_vars)
	{
		max_vars = ATgetLength(ATgetArgument(eqn,0));
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

static ATerm subst_values(ATermAppl *vars, ATerm *vals, int len, ATerm t)
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
			a = subst_values(vars,vals,len,ATgetFirst(l));
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
		for (int i=0; i<len; i++)
		{
			if ( ATisEqual(t,vars[i]) )
			{
				return vals[i];
			}
		}
		ATfprintf(stderr,"%s: variable %t not assigned\n",NAME,t);
		exit(1);
	} else {
		return t;
	}
}

static bool match_inner(ATerm t, ATerm p, ATermAppl *vars, ATerm *vals, int *len)
{
	bool b;

	if ( ATisList(p) )
	{
		if ( ATisList(t) )
		{
			if ( ATgetLength((ATermList) t) == ATgetLength((ATermList) p) )
			{
				b = true;
				for (; !ATisEmpty((ATermList) t) && b; t=(ATerm) ATgetNext((ATermList) t),p=(ATerm) ATgetNext((ATermList) p))
				{
					b &= match_inner(ATgetFirst((ATermList) t),ATgetFirst((ATermList) p),vars,vals,len);
				}
				return b;
			}
		}
		return false;
	} else if ( ATisInt(p) || gsIsOpId((ATermAppl) p) )
	{
		return ATisEqual(t,p);
	} else /* if ( gsIsDataVarId((ATermAppl) p) ) */ {
		for (int i=0; i<*len; i++)
		{
			if ( ATisEqual(p,vars[i]) )
			{
				if ( ATisEqual(t,vals[i]) )
				{
					return true;
				} else {
					return false;
				}
			}
		}
		vars[*len] = (ATermAppl) p;
		vals[*len] = t;
		(*len)++;
		return true;
	}
}

static ATerm rewrite_func(ATermInt op, ATermList args);
static ATerm rewrite(ATerm Term);

static ATerm build(ATerm Term, int buildargs, ATermAppl *vars, ATerm *vals, int len)
{
//ATfprintf(stderr,"build(%t,%i)\n\n",Term,buildargs);

	if ( ATisList(Term) )
	{
		ATerm head = ATgetFirst((ATermList) Term);
		ATermList args = ATgetNext((ATermList) Term);
		ATermList l;

		if ( buildargs == -1 )
		{
			buildargs = ATgetLength(args);
		}

		l = ATmakeList0();
		for (int i=0; i<buildargs; i++)
		{
			l = ATinsert(l,build(ATgetFirst(args),-1,vars,vals,len));
			args = ATgetNext(args);
		}
		for (;!ATisEmpty(args);args=ATgetNext(args))
		{
			l = ATinsert(l,ATgetFirst(args));
		}
		args = ATreverse(l);

		int b = 1;
		while ( !ATisInt(head) && b )
		{
			for (int i=0; i<len; i++)
			{
				if ( ATisEqual(head,vars[i]) )
				{
					if ( ATisList(vals[i]) )
					{
						head = ATgetFirst((ATermList) vals[i]);
						args = ATconcat(ATgetNext((ATermList) vals[i]),args);
					} else {
						head = vals[i];
					}
					break;
				}
				b = 0;
			}
		}
		if ( ATisInt(head) )
		{
			return rewrite_func((ATermInt) head,args);
		} else {
			return (ATerm) ATinsert(args,head);
		}
	} else if ( ATisInt(Term) ) {
		return rewrite_func((ATermInt) Term, ATmakeList0());
	} else {
		for (int i=0; i<len; i++)
		{
			if ( ATisEqual(Term,vars[i]) )
			{
				return vals[i];
			}
		}
		return Term;
	}
}

static ATerm rewrite_func(ATermInt op, ATermList args)
{
	ATermList m;
	DECL_A(vars,ATermAppl,max_vars);
	DECL_A(vals,ATerm,max_vars);
	int pos;
//ATfprintf(stderr,"rewrite_func(%t,%t)\n\n",op,args);

	if ( (m = inner3_eqns[ATgetInt(op)]) != NULL )
	{
		for (; !ATisEmpty(m); m=ATgetNext(m))
		{
			ATermList rule = ATgetNext(ATLgetFirst(m));
			ATerm cond = ATgetFirst(rule); rule=ATgetNext(rule);
			ATermList rargs = ATLgetFirst(rule); rule=ATgetNext(rule);
			ATerm rslt = ATgetFirst(rule);
			ATermList l2 = args;

			bool match = true;
			pos = 0;
			while ( !ATisEmpty(rargs) )
			{
				if ( ATisEmpty(l2) )
				{
					match = false;
					break;
				}
	
				if ( !match_inner(ATgetFirst(l2),ATgetFirst(rargs),vars,vals,&pos) )
				{
					match = false;
					break;
				}
	
				rargs = ATgetNext(rargs);
				l2 = ATgetNext(l2);
			}
			if ( match )
			{
				if ( is_nil(cond) || ATisEqual(build(cond,-1,vars,vals,pos),trueint) )
				{
					int rslt_len;
					if ( ATisList(rslt) )
					{
						rslt_len = ATgetLength(rslt)-1;
						if ( !ATisEmpty(l2) )
						{
							rslt = (ATerm) ATconcat((ATermList) rslt,l2);
						}
					} else {
						rslt_len = 0;
						if ( !ATisEmpty(l2) )
						{
							rslt = (ATerm) ATinsert(l2,rslt);
						}
					}
					ATerm t = build(rslt,rslt_len,vars,vals,pos);
					FREE_A(vals)
					FREE_A(vars)
					return t;
				}		
			}
		}
	}

	FREE_A(vals)
	FREE_A(vars)

	if ( ATisEmpty(args) )
	{
		return (ATerm) op;
	} else {
		return (ATerm) ATinsert(args,(ATerm) op);
	}
}

static ATerm rewrite(ATerm Term)
{
///ATfprintf(stderr,"rewrite(%t)\n\n",Term);
	if ( ATisList(Term) )
	{
		ATermList l = ATgetNext((ATermList) Term);
		ATermList m;

		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,rewrite(ATgetFirst(l)));
		}
		l = ATreverse(m);

		if ( ATisInt(ATgetFirst((ATermList) Term)) )
		{
			Term = rewrite_func((ATermInt) ATgetFirst((ATermList) Term), l);
		} else {
			// XXX is Term in subst_table!!! XXX
			if ( (ATgetType(ATgetFirst((ATermList) Term)) == AT_APPL) && gsIsDataVarId(ATAgetFirst((ATermList) Term)) ) 
				printf("EERRRRRROORRRR!!!\n\n\n\n");
			Term = (ATerm) ATinsert(l,ATgetFirst((ATermList) Term));
		}

		return Term;
	} else if ( ATisInt(Term) )
	{
		return rewrite_func((ATermInt) Term, ATmakeList0());
	} else {
		return (ATerm) RWapplySubstitution((ATermAppl) Term);
	}
}

ATerm rewrite_inner3(ATerm Term)
{
	return rewrite(Term);
}

ATerm to_rewrite_format_inner3(ATermAppl Term)
{
	return toInner(Term,false);
}

ATermAppl from_rewrite_format_inner3(ATerm Term)
{
	return fromInner(Term);
}

#ifdef __cplusplus
}
#endif
