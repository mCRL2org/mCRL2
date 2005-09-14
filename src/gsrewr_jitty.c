/* $Id: gsrewr_jitty.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "rewr_jitty"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_jitty.h"
#include "libgsparse.h"
#include "gssubstitute.h"

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable term2int;
static unsigned int num_opids;
static ATermAppl *int2term;
static ATermList *jitty_eqns;
static ATermAppl jitty_true;
static AFun nilAFun;
static AFun opidAFun;
static int max_vars;
static bool isprotected = false;

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
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


static AFun *apples;
static int num_apples = 0;

static AFun getAppl(int arity)
{
	int old_apples = num_apples;

	if ( old_apples == 0 )
	{
		while ( num_apples <= arity )
		{
			num_apples += 10;
		}

		apples = (AFun *) malloc(num_apples*sizeof(AFun));
	} else if ( old_apples <= arity )
	{
		while ( num_apples <= arity )
		{
			num_apples += 10;
		}

		apples = (AFun *) realloc(apples,num_apples*sizeof(AFun));
	}

	for (int i=old_apples; i<num_apples; i++)
	{
		apples[i] = ATmakeAFun("@REWR@",i,ATfalse);
		ATprotectAFun(apples[i]);
	}

	return apples[arity];
}

static ATermAppl toInner(ATermAppl Term, bool add_opids)
{
	ATermList l;

	if ( gsIsNil(Term) || gsIsDataVarId(Term) )
	{
		return Term;
	}

	l = ATmakeList0();
	while ( gsIsDataAppl(Term) )
	{
		l = ATinsert(l,(ATerm) toInner(ATAgetArgument(Term,1),add_opids));
		Term = ATAgetArgument(Term,0);
	}
	if ( gsIsOpId(Term) )
	{
		l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
	} else {
		l = ATinsert(l,(ATerm) Term);
	}

	return ATmakeApplList(getAppl(ATgetLength(l)),l);
}

static ATermAppl fromInner(ATermAppl Term)
{
	int arity = ATgetArity(ATgetAFun(Term));
	ATermAppl a;
	ATerm t;

//ATprintf("in: %t\n\n",Term);
	if ( gsIsDataVarId(Term) )
	{
//ATprintf("out: %t\n\n",Term);
		return Term;
	}

	t = ATgetArgument(Term,0);
	if ( ATisInt(t) )
	{
		a = int2term[ATgetInt((ATermInt) t)];
	} else {
		a = (ATermAppl) t;
	}
	for (int i=1; i<arity; i++)
	{
		a = gsMakeDataAppl(a,fromInner(ATAgetArgument(Term,i)));
	}

//ATprintf("out: %t\n\n",a);
	return a;
}

static ATermList get_vars(ATerm a)
{
	if ( gsIsDataVarId((ATermAppl) a) )
	{
		return ATmakeList1(a);
	} else {
		ATermList l = ATmakeList0();
		ATermList m = ATgetArguments((ATermAppl) a);
		for (;!ATisEmpty(m); m=ATgetNext(m))
		{
			l = ATconcat(l,get_vars(ATgetFirst(m)));
		}
		return l;
	}
}

ATermList create_strategy(ATermList rules)
{
	ATermList strat = ATmakeList0();
	int arity;

//ATfprintf(stderr,"rules: %t\n\n",rules);
	int max_arity = 0;
	for (ATermList l=rules; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1 > max_arity )
		{
			max_arity = ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1;
		}
	}
	DECL_A(used,bool,max_arity);
	for (int i=0; i<max_arity; i++)
	{
		used[i] = false;
	}

	arity = 0;
	while ( !ATisEmpty(rules) )
	{
		ATermList l = ATmakeList0();
		ATermList m = ATmakeList0();
		DECL_A(args,int,arity);
		DECL_A(bs,bool,arity);
//printf("arity = %i\n",arity);

		for (int i=0; i<arity; i++)
		{
			args[i] = -1;
		}

		for (; !ATisEmpty(rules); rules=ATgetNext(rules))
		{
			if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(rules),2)))-1 == arity )
			{
				ATermList vars = ATmakeList0();
				ATermAppl pars = ATAelementAt(ATLgetFirst(rules),2);

//ATfprintf(stderr,"rule: %t\n",ATgetFirst(rules));
//ATfprintf(stderr,"rule: %t\n",ATAelementAt(ATgetFirst(rules),2));
//ATfprintf(stderr,"rule: "); gsPrintPart(stderr,fromInner(ATAelementAt(ATgetFirst(rules),2)),0,0); ATfprintf(stderr,"\n");
//ATprintf("pars: %t\n",pars);

				for (int i=0; i<arity; i++)
				{
					bs[i] = false;
				}

				for (int i=0; i<arity; i++)
				{
					if ( !gsIsDataVarId(ATAgetArgument(pars,i+1)) )
					{
						bs[i] = true;
						vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
					} else {
						int j = 0;
						bool b = false;
						for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
						{
							if ( ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) >= 0 )
							{
								bs[j] = true;
								b = true;
							}
							j++;
						}
						if ( b )
						{
							bs[i] = true;
						}
						vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
					}
				}

				ATermList deps = ATmakeList0();
				for (int i=0; i<arity; i++)
				{
					if ( bs[i] && !used[i] )
					{
						deps = ATinsert(deps,(ATerm) ATmakeInt(i));
						args[i] += 1;
//fprintf(stderr,"dep of arg %i\n",i);
					}
				}
				deps = ATreverse(deps);

				m = ATinsert(m,(ATerm) ATmakeList2((ATerm) deps,ATgetFirst(rules)));
//ATfprintf(stderr,"\n");
			} else {
				l = ATinsert(l,ATgetFirst(rules));
			}
		}

		while ( !ATisEmpty(m) )
		{
			ATermList m2 = ATmakeList0();
			for (; !ATisEmpty(m); m=ATgetNext(m))
			{
				if ( ATisEmpty(ATLgetFirst(ATLgetFirst(m))) )
				{
//ATprintf("add: %t\n",ATgetFirst(ATgetNext(ATLgetFirst(m))));
					strat = ATinsert(strat, ATgetFirst(ATgetNext(ATLgetFirst(m))));
				} else {
					m2 = ATinsert(m2,ATgetFirst(m));
				}
			}
			m = ATreverse(m2);

			if ( ATisEmpty(m) )
			{
				break;
			}

			int max = -1;
			int maxidx = -1;

			for (int i=0; i<arity; i++)
			{
				if ( args[i] > max )
				{
					maxidx = i;
					max = args[i];
				}
			}

			if ( maxidx >= 0 )
			{
				args[maxidx] = -1;
				used[maxidx] = true;

				ATermInt k = ATmakeInt(maxidx);
//ATprintf("add: %t\n",k);
				strat = ATinsert(strat,(ATerm) k);
				m2 = ATmakeList0();
				for (; !ATisEmpty(m); m=ATgetNext(m))
				{
					m2 = ATinsert(m2,(ATerm) ATinsert(ATgetNext(ATLgetFirst(m)),(ATerm) ATremoveElement(ATLgetFirst(ATLgetFirst(m)),(ATerm) k)));
				}
				m = ATreverse(m2);
			}
		}

		FREE_A(bs);
		FREE_A(args);
		rules = ATreverse(l);
		arity++;
	}

	//XXX Add unused, so we don't need to check all args during rewriting

//ATfprintf(stderr,"strat: %t\n\n",ATreverse(strat));

	FREE_A(used);
	return ATreverse(strat);
}

void rewrite_init_jitty()
{
	ATermList l,n;
	ATermTable tmp_eqns;
	ATermInt i;
	ATermAppl m;

	tmp_eqns = ATtableCreate(100,50); // XXX would be nice to know the number op OpIds
	term2int = ATtableCreate(100,50);

	max_vars = 0;

	jitty_true = toInner(gsMakeDataExprTrue(),true);
	if ( !isprotected )
	{
		ATprotectAppl(&jitty_true);
	}

	nilAFun = ATgetAFun(gsMakeNil());
	ATprotectAFun(nilAFun);
	opidAFun = ATgetAFun(gsMakeDataExprTrue());
	ATprotectAFun(opidAFun);

	l = opid_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
		ATtablePut(tmp_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
	}

	l = dataappl_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = toInner(ATAgetArgument(ATAgetFirst(l),2),true);
		if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetArgument(m,0))) == NULL )
		{
			n = ATmakeList0();
		}
		if ( ATgetLength(ATgetArgument(ATAgetFirst(l),0)) > max_vars)
		{
			max_vars = ATgetLength(ATgetArgument(ATAgetFirst(l),0));
		}
		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) m,(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		ATtablePut(tmp_eqns,ATgetArgument(m,0),(ATerm) n);
	}

	int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	memset(int2term,0,num_opids*sizeof(ATermAppl));
	jitty_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
	memset(jitty_eqns,0,num_opids*sizeof(ATermList));
	if ( !isprotected )
	{
		ATprotectArray((ATerm *) int2term,num_opids);
		ATprotectArray((ATerm *) jitty_eqns,num_opids);
	}


	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermList n;

		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		int2term[ATgetInt(i)] = ATAgetFirst(l);
		if ( (n = (ATermList) ATtableGet(tmp_eqns,(ATerm) i)) == NULL )
		{
			jitty_eqns[ATgetInt(i)] = NULL;
		} else {
//ATfprintf(stderr,"%t\n",ATAgetFirst(l));
			jitty_eqns[ATgetInt(i)] = create_strategy(ATreverse(n));
		}
	}


	ATtableDestroy(tmp_eqns);

	isprotected = true;
}

void rewrite_add_jitty(ATermAppl eqn)
{
/*	ATermList l,m;
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
			jitty_eqns[ATgetInt(i)] = NULL;
		}
	}
	if ( jitty_eqns[ATgetInt(j)] == NULL )
	{
		jitty_eqns[ATgetInt(j)] = ATmakeList1((ATerm) m);
	} else {
		jitty_eqns[ATgetInt(j)] = ATappend(jitty_eqns[ATgetInt(j)],(ATerm) m);
	}*/
}

void rewrite_remove_jitty(ATermAppl eqn)
{
/*	ATermList l,m;
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
		l = jitty_eqns[ATgetInt((ATermInt) t)];
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
			jitty_eqns[ATgetInt((ATermInt) t)] = NULL;
		} else {
			jitty_eqns[ATgetInt((ATermInt) t)] = ATreverse(m);
		}
	}*/
}

static ATerm subst_values(ATermAppl *vars, ATerm *vals, int len, ATerm t)
{
	if ( ATisInt(t) )
	{
		return t;
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
		int arity = ATgetArity(ATgetAFun((ATermAppl) t));
		int new_arity = arity;
		ATerm arg0 = subst_values(vars,vals,len,ATgetArgument((ATermAppl) t,0));
		if ( !(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)) )
		{
			new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
		}
		DECL_A(args,ATerm,new_arity);
		int i;
		if ( ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0) )
		{
			args[0] = arg0;
			i = 1;
		} else {
			i = 0;
			int arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
			while ( i < arg0_arity )
			{
				args[i] = ATgetArgument((ATermAppl) arg0,i);
				i++;
			}
		}

		for (int j=1; j<arity; j++)
		{
			args[i] = subst_values(vars,vals,len,ATgetArgument((ATermAppl) t,j));
			i++;
		}

		if ( arity == new_arity )
		{
			return (ATerm) ATmakeApplArray(ATgetAFun((ATermAppl) t),args);
		} else {
			return (ATerm) ATmakeApplArray(getAppl(new_arity),args);
		}
	}
}

static bool match_jitty(ATerm t, ATerm p, ATermAppl *vars, ATerm *vals, int *len)
{
//ATfprintf(stderr,"match_inner(  %t  ,  %t  ,  %t   )\n\n",t,p,*vars);
        if ( ATisInt(p) )
	{
		return ATisEqual(p,t);
	} else if ( gsIsDataVarId((ATermAppl) p) )
	{
		t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
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
	} else {
		t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
		if ( ATisInt(t) || gsIsDataVarId((ATermAppl) t) || !ATisEqualAFun(ATgetAFun((ATermAppl) p),ATgetAFun((ATermAppl) t)) )
		{
			return false; 
		}

		int arity = ATgetArity(ATgetAFun((ATermAppl) p));

		for (int i=0; i<arity; i++)
		{
			if ( !match_jitty(ATgetArgument((ATermAppl) t,i),ATgetArgument((ATermAppl) p,i),vars,vals,len) )
			{
				return false;
			}
		}

		return true;
	}
}

static ATermAppl rewrite(ATermAppl Term)
{
//ATfprintf(stderr,"rewrite(%t)\n\n",Term);
//fprintf(stderr,"rewrite(  ");gsPrintPart(stderr,fromInner(Term),false,0);fprintf(stderr,"  )\n\n");
	if ( gsIsDataVarId(Term) )
	{
//ATfprintf(stderr,"return %t\n\n",Term);
//fprintf(stderr,"return  ");gsPrintPart(stderr,fromInner((ATermAppl) RWapplySubstitution((ATerm) Term)),false,0);fprintf(stderr,"\n\n");
		return (ATermAppl) RWapplySubstitution((ATerm) Term);
	} else {
		ATerm op = ATgetArgument(Term,0);
		ATermList strat;
		int arity = ATgetArity(ATgetAFun(Term));
		DECL_A(rewritten,ATerm,arity);

		for (int i=1; i<arity; i++)
		{
			rewritten[i] = NULL;
		}
		
		if ( ATisInt(op) && ((strat = jitty_eqns[ATgetInt((ATermInt) op)]) != NULL) )
		{
//ATfprintf(stderr,"strat: %t\n\n",strat);
			for (; !ATisEmpty(strat); strat=ATgetNext(strat))
			{
//ATfprintf(stderr,"strat action: %t\n\n",ATgetFirst(strat));
				if ( ATisInt(ATgetFirst(strat)) )
				{
					int i = ATgetInt((ATermInt) ATgetFirst(strat))+1;
					if ( i < arity )
					{
						rewritten[i] = (ATerm) rewrite(ATAgetArgument(Term,i));
					} else {
						break;
					}
				} else {
					ATermList rule = ATLgetFirst(strat);
					ATermAppl lhs = ATAelementAt(rule,2);
					int rule_arity = ATgetArity(ATgetAFun(lhs));

					if ( rule_arity > arity )
					{
						break;
					}

					int max_len = ATgetLength(ATLgetFirst(rule));
					DECL_A(vars,ATermAppl,max_len);
					DECL_A(vals,ATerm,max_len);
					int len = 0;
					bool matches = true;

					for (int i=1; i<rule_arity; i++)
					{
						if ( !match_jitty((rewritten[i]==NULL)?ATgetArgument(Term,i):rewritten[i],ATgetArgument(lhs,i),vars,vals,&len) )
						{
							matches = false;
							break;
						}
					}

//if ( matches && !gsIsNil(ATAelementAt(rule,1)) )
//{
//ATprintf("%t --> %t\n\n",ATelementAt(rule,1),rewrite((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1)),&e));
//}
					if ( matches && (gsIsNil(ATAelementAt(rule,1)) || ATisEqual(rewrite((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1))),jitty_true)) )
					{
						ATermAppl rhs = ATAelementAt(rule,3);

						if ( gsIsDataVarId(rhs) )
						{
							return rewrite((ATermAppl) subst_values(vars,vals,len,(ATerm) rhs));
						}

						int rhs_arity = ATgetArity(ATgetAFun(rhs));
						int new_arity = rhs_arity+arity-rule_arity;
						ATerm arg0 = subst_values(vars,vals,len,ATgetArgument(rhs,0));
						if ( !(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)))
						{
							new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
						}
						DECL_A(args,ATerm,rhs_arity+arity-rule_arity);
						int i;
						if ( ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0))
						{
							args[0] = arg0;
							i = 1;
						} else {
							i = 0;
							int arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
							while ( i < arg0_arity ) 
							{
								args[0] = ATgetArgument((ATermAppl) arg0,i);
								i++;
							}
						}

						for (int j=1; j<rhs_arity; j++)
						{
//ATfprintf(stderr,"pre %t\n\n",ATgetArgument(rhs,i));
							args[i] = subst_values(vars,vals,len,ATgetArgument(rhs,j));
							i++;
//ATfprintf(stderr,"post %t\n\n",args[i]);
						}
						for (int j=0; j<arity-rule_arity; j++)
						{
							args[i] = ATgetArgument(Term,rule_arity+j);
							i++;
						}

						ATermAppl a = ATmakeApplArray(getAppl(new_arity),args);

						FREE_A(args);
						FREE_A(vals);
						FREE_A(vars);

						ATermAppl aa = rewrite(a);
//ATfprintf(stderr,"return %t\n\n",aa);
//fprintf(stderr,"return  ");gsPrintPart(stderr,fromInner(aa),false,0);fprintf(stderr,"\n\n");
						return aa;
					}

					FREE_A(vals);
					FREE_A(vars);
				}
			}
		}
//ATfprintf(stderr,"done with strat\n\n");

		rewritten[0] = op;
		for (int i=1; i<arity; i++)
		{
			if ( rewritten[i] == NULL )
			{
				rewritten[i] = (ATerm) rewrite(ATAgetArgument(Term,i));
			}
		}

		ATermAppl a = ATmakeApplArray(ATgetAFun(Term),rewritten);

		FREE_A(rewritten);
//ATfprintf(stderr,"return %t\n\n",a);
//fprintf(stderr,"return  ");gsPrintPart(stderr,fromInner(a),false,0);fprintf(stderr,"\n\n");
		return a;
	}
}

/*ATerm rewrite_jitty(ATerm Term, int *b)
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
			l = ATinsert(l,(ATerm) fromInner(rewrite(toInner((ATermAppl) ATgetFirst((ATermList) Term),true),&c)));
			*b |= c;
		}
		return (ATerm) ATreverse(l);
	}

	return (ATerm) fromInner(rewrite(toInner((ATermAppl) Term,true),b));
}*/

ATerm to_rewrite_format_jitty(ATermAppl Term)
{
	int old_opids;
	ATermAppl a;
	int c;
	ATermList l;
	ATermInt i;

	old_opids = num_opids;
	a = toInner((ATermAppl) Term,true);
	if ( old_opids < num_opids )
	{
		ATunprotectArray((ATerm *) int2term);
		int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
		for (int k=old_opids; k<num_opids; k++) int2term[k] = NULL;
		ATprotectArray((ATerm *) int2term,num_opids);
		ATunprotectArray((ATerm *) jitty_eqns);
		jitty_eqns = (ATermList *) realloc(jitty_eqns,num_opids*sizeof(ATermList));
		for (int k=old_opids; k<num_opids; k++) jitty_eqns[k] = NULL;
		ATprotectArray((ATerm *) jitty_eqns,num_opids);
		l = ATtableKeys(term2int);
		c = 0;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
			if ( ATgetInt(i) >= old_opids )
			{
				int2term[ATgetInt(i)] = ATAgetFirst(l);
				c++;
				jitty_eqns[ATgetInt(i)] = NULL;
			}
		}
	}

	return (ATerm) a;
}

ATermAppl from_rewrite_format_jitty(ATerm Term)
{
	return fromInner((ATermAppl) Term);
}

ATerm rewrite_jitty(ATerm Term)
{
/*fprintf(stderr,"rewrite(  ");gsPrintPart(stderr,fromInner(Term),false,0);fprintf(stderr,"  )\n\n");
ATermAppl a = rewrite((ATermAppl) Term);
fprintf(stderr,"return  ");gsPrintPart(stderr,fromInner(a),false,0);fprintf(stderr,"  )\n\n");
return (ATerm) a;*/
	return (ATerm) rewrite((ATermAppl) Term);
}
