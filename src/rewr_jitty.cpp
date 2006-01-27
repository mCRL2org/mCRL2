#define NAME "rewr_jitty"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aterm2.h>
#include <assert.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "rewr_jitty.h"

static AFun nilAFun;
static AFun opidAFun;
static unsigned int is_initialised = 0;

#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))
#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisInt(x) (ATgetType(x) == AT_INT)

#define is_nil(x) (ATisList(x)?false:(ATgetAFun((ATermAppl) x) == nilAFun))


static void initialise_common()
{
	if ( is_initialised == 0 )
	{
		nilAFun = ATgetAFun(gsMakeNil());
		ATprotectAFun(nilAFun);
		opidAFun = ATgetAFun(gsMakeDataExprTrue());
		ATprotectAFun(opidAFun);
	}

	is_initialised++;
}

static void finalise_common()
{
	assert(is_initialised > 0 );
	is_initialised--;

	if ( is_initialised == 0 )
	{
		ATunprotectAFun(opidAFun);
		ATunprotectAFun(nilAFun);
	}
}


ATerm RewriterJitty::OpId2Int(ATermAppl Term, bool add_opids)
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

#define getAppl(x) ((x < num_apples)?apples[x]:getAppl2(x))
static AFun getAppl2(int arity)
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

ATermAppl RewriterJitty::toInner(ATermAppl Term, bool add_opids)
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

ATermAppl RewriterJitty::fromInner(ATermAppl Term)
{
	int arity = ATgetArity(ATgetAFun(Term));
	ATermAppl a;
	ATerm t;

//gsprintf("in: %T\n\n",Term);
	if ( gsIsDataVarId(Term) )
	{
//gsprintf("out: %T\n\n",Term);
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

//gsprintf("out: %T\n\n",a);
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

static ATermList create_strategy(ATermList rules)
{
	ATermList strat = ATmakeList0();
	unsigned int arity;

//gsfprintf(stderr,"rules: %T\n\n",rules);
	unsigned int max_arity = 0;
	for (ATermList l=rules; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2))) > max_arity + 1 )
		{
			max_arity = ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1;
		}
	}
	DECL_A(used,bool,max_arity);
	for (unsigned int i = 0; i < max_arity; i++)
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

		for (unsigned int i = 0; i < arity; i++)
		{
			args[i] = -1;
		}

		for (; !ATisEmpty(rules); rules=ATgetNext(rules))
		{
			if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(rules),2))) == arity + 1 )
			{
				ATermAppl cond = ATAelementAt(ATLgetFirst(rules),1);
				ATermList vars = gsIsNil(cond)?ATmakeList1((ATerm) ATmakeList0()):ATmakeList1((ATerm) get_vars((ATerm) cond));
				ATermAppl pars = ATAelementAt(ATLgetFirst(rules),2);

//gsfprintf(stderr,"rule: %T\n",ATgetFirst(rules));
//gsfprintf(stderr,"rule: %T\n",ATAelementAt(ATgetFirst(rules),2));
//gsfprintf(stderr,"rule: "); PrintPart_C(stderr,fromInner(ATAelementAt(ATgetFirst(rules),2))); gsfprintf(stderr,"\n");
//gsprintf("pars: %T\n",pars);

				for (unsigned int i = 0; i < arity; i++)
				{
					bs[i] = false;
				}

				for (unsigned int i = 0; i < arity; i++)
				{
					if ( !gsIsDataVarId(ATAgetArgument(pars,i+1)) )
					{
						bs[i] = true;
						vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
					} else {
						unsigned int j = -1;
						bool b = false;
						for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
						{
							if ( ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) >= 0 )
							{
								if ( j >= 0 )
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
				for (unsigned int i = 0; i < arity; i++)
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
//gsfprintf(stderr,"\n");
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
//gsprintf("add: %T\n",ATgetFirst(ATgetNext(ATLgetFirst(m))));
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

			for (unsigned int i = 0; i < arity; i++)
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
//gsprintf("add: %T\n",k);
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

//gsfprintf(stderr,"strat: %T\n\n",ATreverse(strat));

	FREE_A(used);
	return ATreverse(strat);
}

RewriterJitty::RewriterJitty(ATermAppl DataEqnSpec)
{
	ATermList l,n;
	ATermTable tmp_eqns;
	ATermInt i;

	initialise_common();

	tmp_eqns = ATtableCreate(100,50); // XXX would be nice to know the number op OpIds
	term2int = ATtableCreate(100,50);
	
	num_opids = 0;
	max_vars = 0;

	jitty_true = toInner(gsMakeDataExprTrue(),true);
	ATprotectAppl(&jitty_true);

/*	l = opid_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
		ATtablePut(tmp_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
	}

	l = dataappl_eqns;*/
	l = ATLgetArgument(DataEqnSpec,0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl u = toInner(ATAgetArgument(ATAgetFirst(l),2),true);

		if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetArgument(u,0))) == NULL )
		{
			n = ATmakeList0();
		}
		if ( ATgetLength(ATgetArgument(ATAgetFirst(l),0)) > max_vars)
		{
			max_vars = ATgetLength(ATgetArgument(ATAgetFirst(l),0));
		}
		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) u,(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		ATtablePut(tmp_eqns,ATgetArgument(u,0),(ATerm) n);
	}

	int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	memset(int2term,0,num_opids*sizeof(ATermAppl));
	ATprotectArray((ATerm *) int2term,num_opids);
	jitty_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
	memset(jitty_eqns,0,num_opids*sizeof(ATermList));
	ATprotectArray((ATerm *) jitty_eqns,num_opids);


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
//gsfprintf(stderr,"%T\n",ATAgetFirst(l));
			jitty_eqns[ATgetInt(i)] = create_strategy(ATreverse(n));
		}
	}

	ATtableDestroy(tmp_eqns);
}

RewriterJitty::~RewriterJitty()
{
	ATunprotectArray((ATerm *) jitty_eqns);
	free(jitty_eqns);
	ATunprotectArray((ATerm *) int2term);
	free(int2term);
	ATunprotectAppl(&jitty_true);
	ATtableDestroy(term2int);

	finalise_common();
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
		gsfprintf(stderr,"%s: variable %T not assigned\n",NAME,t);
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
//gsfprintf(stderr,"match_jitty(  %T  ,  %T  ,  %T   )\n\n",t,p,*vars);
        if ( ATisInt(p) )
	{
		return ATisEqual(p,t);
	} else if ( gsIsDataVarId((ATermAppl) p) )
	{
//		t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
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
//		t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
		if ( ATisInt(t) || gsIsDataVarId((ATermAppl) t) )
		{
			return false; 
		}
/*		ATerm head = ATgetArgument((ATermAppl) t, 0);
		if ( !ATisInt(head) )
		{
			head = RWapplySubstitution(head);
			if ( ATisInt(head) )
			{
				t = (ATerm) ATsetArgument((ATermAppl) t,head,0);
			} else if ( !gsIsDataVarId((ATermAppl) head) )
			{
				int ar1 = ATgetArity(ATgetAFun((ATermAppl) head));
				int ar2 = ATgetArity(ATgetAFun((ATermAppl) t));

				if ( ar1+ar2-1 != ATgetArity(ATgetAFun(p)) )
				{
					return false;
				}

				for (int i=0; i<ar1; i++)
				{
					if ( !match_jitty(ATgetArgument((ATermAppl) head,i),ATgetArgument((ATermAppl) p,i),vars,vals,len) )
					{
						return false;
					}
				}
				for (int i=1; i<ar2; i++)
				{
					if ( !match_jitty(ATgetArgument((ATermAppl) t,i),ATgetArgument((ATermAppl) p,ar1+i-1),vars,vals,len) )
					{
						return false;
					}
				}
				return true;
			}
		}*/
		if ( !ATisEqualAFun(ATgetAFun((ATermAppl) p),ATgetAFun((ATermAppl) t)) )
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

ATermAppl RewriterJitty::rewrite_aux(ATermAppl Term)
{
//gsfprintf(stderr,"rewrite(%T)\n\n",Term);
//gsfprintf(stderr,"rewrite(  %P  )\n\n",fromInner(Term));
	if ( gsIsDataVarId(Term) )
	{
//gsfprintf(stderr,"return %T\n\n",Term);
//gsfprintf(stderr,"return1  %P\n\n",fromInner((ATermAppl) lookupSubstitution(Term)));
		return (ATermAppl) lookupSubstitution(Term);
	} else {
		ATerm op = ATgetArgument(Term,0);
		ATerm head;
		ATermList strat;
		int head_arity = 0;
		int arity = ATgetArity(ATgetAFun(Term));

		if ( !ATisInt(op) )
		{
			head = lookupSubstitution((ATermAppl) op);
			if ( ATisInt(head) )
			{
				op = head;
			} else if ( !gsIsDataVarId((ATermAppl) head) )
			{
				op = ATgetArgument((ATermAppl) head,0);
				head_arity = ATgetArity(ATgetAFun(head));
				arity = arity-1 + head_arity;
			}
		}

		DECL_A(rewritten,ATerm,arity);
		DECL_A(args,ATermAppl,arity);

		if ( head_arity > 0 )
		{
			head_arity--;
		}
		for (int i=1; i<arity; i++)
		{
			rewritten[i] = NULL;
			if ( i < head_arity+1 )
			{
				args[i] = ATAgetArgument((ATermAppl) head,i);
			} else {
				args[i] = ATAgetArgument(Term,i-head_arity);
			}
		}
		
		if ( ATisInt(op) && ((strat = jitty_eqns[ATgetInt((ATermInt) op)]) != NULL) )
		{
//gsfprintf(stderr,"strat: %T\n\n",strat);
			for (; !ATisEmpty(strat); strat=ATgetNext(strat))
			{
//gsfprintf(stderr,"strat action: %T\n\n",ATgetFirst(strat));
				if ( ATisInt(ATgetFirst(strat)) )
				{
					int i = ATgetInt((ATermInt) ATgetFirst(strat))+1;
					if ( i < arity )
					{
						rewritten[i] = (ATerm) rewrite_aux(args[i]);
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
						if ( !match_jitty((rewritten[i]==NULL)?((ATerm) args[i]):rewritten[i],ATgetArgument(lhs,i),vars,vals,&len) )
						{
							matches = false;
							break;
						}
					}

//if ( matches && !gsIsNil(ATAelementAt(rule,1)) )
//{
//gsfprintf(stderr,"%T --> %T (%T)\n\n",ATelementAt(rule,1),rewrite_aux((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1))),jitty_true);
//}
					if ( matches && (gsIsNil(ATAelementAt(rule,1)) || ATisEqual(rewrite_aux((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1))),jitty_true)) )
					{
						ATermAppl rhs = ATAelementAt(rule,3);

						if ( arity == rule_arity )
						{
							return rewrite_aux((ATermAppl) subst_values(vars,vals,len,(ATerm) rhs));
						}

						int rhs_arity;
						int new_arity;
						ATerm arg0;

						if ( gsIsDataVarId(rhs) )
						{
							arg0 = subst_values(vars,vals,len,(ATerm) rhs);
							if ( gsIsDataVarId((ATermAppl) arg0) )
							{
								rhs_arity = 0;
								new_arity = 1+arity-rule_arity;
							} else {
								rhs_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
								new_arity = rhs_arity+arity-rule_arity;
							}
						} else {
							rhs_arity = ATgetArity(ATgetAFun(rhs));
							new_arity = rhs_arity+arity-rule_arity;
							arg0 = subst_values(vars,vals,len,ATgetArgument(rhs,0));
							if ( !(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)))
							{
								new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
							}
						}
						DECL_A(newargs,ATerm,new_arity);
						int i;
						if ( gsIsDataVarId(rhs) )
						{
							if ( gsIsDataVarId((ATermAppl) arg0) )
							{
								newargs[0] = arg0;
								i = 1;
							} else {
								i = 0;
								while ( i < rhs_arity )
								{
									newargs[i] = ATgetArgument((ATermAppl) arg0,i);
									i++;
								}
							}
						} else {
							if ( ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0))
							{
								newargs[0] = arg0;
								i = 1;
							} else {
								i = 0;
								int arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
								while ( i < arg0_arity ) 
								{
									newargs[i] = ATgetArgument((ATermAppl) arg0,i);
									i++;
								}
							}
							for (int j=1; j<rhs_arity; j++)
							{
//gsfprintf(stderr,"pre %T\n\n",ATgetArgument(rhs,i));
								newargs[i] = subst_values(vars,vals,len,ATgetArgument(rhs,j));
								i++;
//gsfprintf(stderr,"post %T\n\n",args[i]);
							}
						}

						for (int j=0; j<arity-rule_arity; j++)
						{
							newargs[i] = (ATerm) args[rule_arity+j];
							i++;
						}

						ATermAppl a = ATmakeApplArray(getAppl(new_arity),newargs);

						FREE_A(newargs);
						FREE_A(vals);
						FREE_A(vars);

						ATermAppl aa = rewrite_aux(a);
//gsfprintf(stderr,"return %T\n\n",aa);
//gsfprintf(stderr,"return2  %P\n\n",fromInner(aa));
						return aa;
					}

					FREE_A(vals);
					FREE_A(vars);
				}
			}
		}
//gsfprintf(stderr,"done with strat\n\n");

		rewritten[0] = op;
		for (int i=1; i<arity; i++)
		{
			if ( rewritten[i] == NULL )
			{
				rewritten[i] = (ATerm) rewrite_aux(args[i]);
			}
		}

		ATermAppl a = ATmakeApplArray(ATgetAFun(Term),rewritten);

		FREE_A(args);
		FREE_A(rewritten);
//gsfprintf(stderr,"return %T\n\n",a);
//gsfprintf(stderr,"return3  %P\n\n",fromInner(a));
		return a;
	}
}

ATerm RewriterJitty::toRewriteFormat(ATermAppl Term)
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
		for (int k = old_opids; k < num_opids; k++) int2term[k] = NULL;
		ATprotectArray((ATerm *) int2term,num_opids);
		ATunprotectArray((ATerm *) jitty_eqns);
		jitty_eqns = (ATermList *) realloc(jitty_eqns,num_opids*sizeof(ATermList));
		for (int k = old_opids; k < num_opids; k++) jitty_eqns[k] = NULL;
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

ATermAppl RewriterJitty::fromRewriteFormat(ATerm Term)
{
	return fromInner((ATermAppl) Term);
}

ATermAppl RewriterJitty::rewrite(ATermAppl Term)
{
	return fromInner(rewrite_aux((ATermAppl) toRewriteFormat(Term)));
}

ATerm RewriterJitty::rewriteInternal(ATerm Term)
{
	return (ATerm) rewrite_aux((ATermAppl) Term);
}

RewriteStrategy RewriterJitty::getStrategy()
{
	return GS_REWR_JITTY;
}
