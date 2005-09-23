/* <muCRL LPE> ::= spec2gen(<DataSpec>,<LPE>)
 * <LPE>       ::= initprocspec(<DataExpr>*,<Var>*,<Sum>*)
 * <DataSpec>  ::= d(<DataDecl>,<EqnDecl>*)
 * <DataDecl>  ::= s(<Id>*,<Func>*,<Func>*)
 * <EqnDecl>   ::= e(<Var>*,<DataExpr>,<DataExpr>)
 * <Sum>       ::= smd(<Var>*,<Id>,<Id>*,<NextState>,<DataExpr>)
 * <NextState> ::= i(<DataExpr>*)
 * <Func>      ::= f(<Id>,<Id>*,<Id>)
 * <Var>       ::= v(<Id>,<Id>)
 * <DataExpr>  ::= <Id> | <Id>(<Id>,...,<Id>)
 * <Id>        ::= <String>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <aterm2.h>
#include "gsfunc.h"
#include "gslowlevel.h"
#include "libprint_c.h"

static void add_id(ATermList *ids, ATermAppl id)
{
	if ( ATindexOf(*ids,(ATerm) id,0) == -1 )
	{
		*ids = ATappend(*ids,(ATerm) id);
	}
}

static ATbool is_domain(ATermList args, ATermAppl sort)
{
	while ( !gsIsSortId(sort) )
	{
		if ( ATisEmpty(args) )
		{
			return ATfalse;
		} else {
			if ( !ATisEqual(gsGetSort(ATAgetFirst(args)),ATgetArgument(sort,0)) )
			{
				return ATfalse;
			}
		}
		sort = ATAgetArgument(sort,1);
		args = ATgetNext(args);
	}
	if ( ATisEmpty(args) )
	{
		return ATtrue;
	} else {
		return ATfalse;
	}
}

static ATermAppl find_type(ATermAppl a, ATermList args, ATermList typelist)
{
	for (; !ATisEmpty(typelist); typelist=ATgetNext(typelist))
	{
		if ( !strcmp(ATgetName(ATgetAFun(a)),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(typelist),0)))) )
		{
			if ( is_domain(args,ATAgetArgument(ATAgetFirst(typelist),1)) )
			{
				return ATAgetArgument(ATAgetFirst(typelist),1);
			}
		}
	}

	return NULL;
}

static ATermAppl dataterm2ATermAppl(ATermAppl t, ATermList args, ATermList typelist)
{
	ATermList l = ATgetArguments(t);
	ATermList m;
	ATermAppl t2,r;
	
	t2 = ATmakeAppl0(ATmakeAFun(ATgetName(ATgetAFun(t)),0,true));

	if ( ATisEmpty(l) )
	{
		r = find_type(t,ATmakeList0(),args);
		if ( r == NULL )
		{
			r = gsMakeOpId(t2,find_type(t,ATmakeList0(),typelist));
		} else {
			r = gsMakeDataVarId(t2,r);
		}
	} else {
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATappend(m,(ATerm) dataterm2ATermAppl(ATAgetFirst(l),args,typelist));
		}
		r = gsMakeOpId(t2,find_type(t,m,typelist));
		for (; !ATisEmpty(m); m=ATgetNext(m))
		{
			r = gsMakeDataAppl(r,ATAgetFirst(m));
		}
	}
	
	return r;
}

static ATermList get_lpe_acts(ATermAppl lpe, ATermList *ids)
{
	ATermList acts = ATmakeList0();
	ATermList sums = ATLgetArgument(lpe,2);

	for (; !ATisEmpty(sums); sums=ATgetNext(sums))
	{
		if ( !ATisEmpty(ATLgetArgument(ATAgetArgument(ATAgetFirst(sums),2),0)) )
		{
			ATermAppl a = ATAgetArgument(ATAgetFirst(ATLgetArgument(ATAgetArgument(ATAgetFirst(sums),2),0)),0);
			acts = ATinsert(acts,(ATerm) a);
			add_id(ids,ATAgetArgument(a,0));
		}
	}

	return ATreverse(acts);
}

static ATermList get_substs(ATermList ids)
{
	// XXX check identifier syntax!

	ATermList substs = ATmakeList0();

	for (; !ATisEmpty(ids); ids=ATgetNext(ids))
	{
		char s[100], *t;

		strncpy(s,ATgetName(ATgetAFun(ATAgetFirst(ids))),100);

		s[99] = '#';
		for (t=s; (*t)!='#'; t++);
		*t = 0;

		substs = ATinsert(substs,(ATerm) gsMakeSubst(ATgetFirst(ids),(ATerm) ATmakeAppl0(ATmakeAFun(s,0,true))));
	}

	return substs;
}



/*****************************************************
 ************* Main conversion functions *************
 *****************************************************/

static ATermList convert_sorts(ATermAppl spec, ATermList *ids)
{
	ATermList sorts = ATLgetArgument(ATAgetArgument(ATAgetArgument(spec,0),0),0);
	ATermList r;

	r = ATmakeList0();
	for (; !ATisEmpty(sorts); sorts=ATgetNext(sorts))
	{
		add_id(ids,ATAgetFirst(sorts));
		r = ATappend(r,(ATerm) gsMakeSortId(ATAgetFirst(sorts)));
	}

	return r;
}

static ATermList convert_funcs(ATermList funcs, ATermList *ids)
{
	ATermList r,l,m;
	ATermAppl sort;

	r = ATmakeList0();
	for (; !ATisEmpty(funcs); funcs=ATgetNext(funcs))
	{
		add_id(ids,ATAgetArgument(ATAgetFirst(funcs),0));

		m = ATmakeList0();
		l = ATreverse(ATLgetArgument(ATAgetFirst(funcs),1));
		sort = gsMakeSortId(ATAgetArgument(ATAgetFirst(funcs),2));
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			sort = gsMakeSortArrow(gsMakeSortId(ATAgetFirst(l)),sort);
		}

		r = ATappend(r,(ATerm) gsMakeOpId(ATAgetArgument(ATAgetFirst(funcs),0),sort));
	}

	return r;
}

static ATermList convert_cons(ATermAppl spec, ATermList *ids)
{
	return convert_funcs(ATLgetArgument(ATAgetArgument(ATAgetArgument(spec,0),0),1),ids);
}

static ATermList convert_maps(ATermAppl spec, ATermList *ids)
{
	return convert_funcs(ATLgetArgument(ATAgetArgument(ATAgetArgument(spec,0),0),2),ids);
}

static ATermList convert_datas(ATermAppl spec, ATermList typelist, ATermList *ids)
{
	ATermList eqns = ATLgetArgument(ATAgetArgument(spec,0),1);
	ATermList l,args,r;
	ATermAppl lhs,rhs;

	r = ATmakeList0();
	for (; !ATisEmpty(eqns); eqns=ATgetNext(eqns))
	{
		l = ATLgetArgument(ATAgetFirst(eqns),0);
		args = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			args = ATappend(args,(ATerm) gsMakeDataVarId(ATAgetArgument(ATAgetFirst(l),0),gsMakeSortId(ATAgetArgument(ATAgetFirst(l),1))));
			add_id(ids,ATAgetArgument(ATAgetFirst(l),0));
		}
		lhs = dataterm2ATermAppl(ATAgetArgument(ATAgetFirst(eqns),1),args,typelist);
		rhs = dataterm2ATermAppl(ATAgetArgument(ATAgetFirst(eqns),2),args,typelist);
		r = ATappend(r,(ATerm) gsMakeDataEqn(args,gsMakeNil(),lhs,rhs));
	}

	return r;
}

static ATermAppl convert_lpe(ATermAppl spec, ATermList typelist, ATermList *ids)
{
	ATermList vars = ATLgetArgument(ATAgetArgument(spec,1),1);
	ATermList sums = ATLgetArgument(ATAgetArgument(spec,1),2);
	ATermList pars = ATmakeList0();
	ATermList smds = ATmakeList0();

	for (; !ATisEmpty(vars); vars=ATgetNext(vars))
	{
		ATermAppl v = ATAgetFirst(vars);
		pars = ATinsert(pars,
			(ATerm) gsMakeDataVarId(ATAgetArgument(v,0),gsMakeSortId(ATAgetArgument(v,1)))
		);
		add_id(ids,ATAgetArgument(v,0));
	}
	pars = ATreverse(pars);

	for (; !ATisEmpty(sums); sums=ATgetNext(sums))
	{
		ATermAppl s = ATAgetFirst(sums);

		ATermList l = ATreverse(ATLgetArgument(s,0));
		ATermList m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,
				(ATerm) gsMakeDataVarId(ATAgetArgument(ATAgetFirst(l),0),gsMakeSortId(ATAgetArgument(ATAgetFirst(l),1)))
			);
			add_id(ids,ATAgetArgument(ATAgetFirst(l),0));
		}

		l = ATLgetArgument(s,0);
		ATermList o = pars;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			o = ATinsert(o,
				(ATerm) gsMakeDataVarId(ATAgetArgument(ATAgetFirst(l),0),gsMakeSortId(ATAgetArgument(ATAgetFirst(l),1)))
			);
		}
		ATermAppl c = dataterm2ATermAppl(ATAgetArgument(s,4),o,typelist);

		l = ATreverse(ATLgetArgument(s,2));
		ATermList al = ATmakeList0();
		ATermList as = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			al = ATinsert(al,
				(ATerm) dataterm2ATermAppl(ATAgetFirst(l),o,typelist)
			);
			as = ATinsert(as,ATgetArgument(ATAgetFirst(al),1));
		}
		ATermAppl a = gsMakeAction(gsMakeActId(ATAgetArgument(s,1),as),al);
		if ( ATisEmpty(as) && !strcmp("tau",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(a,0),0)))) )
		{
			a = gsMakeMultAct(ATmakeList0());
		} else {
			a = gsMakeMultAct(ATmakeList1((ATerm) a));
		}

		l = ATLgetArgument(ATAgetArgument(s,3),0);
		ATermList o2 = pars;
		ATermList n = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l),o2=ATgetNext(o2))
		{
			n = ATinsert(n,
				(ATerm) gsMakeAssignment(
					ATAgetFirst(o2),
					dataterm2ATermAppl(ATAgetFirst(l),o,typelist)
				)
			);
		}
		n = ATreverse(n);

		smds = ATinsert(smds,
			(ATerm) gsMakeLPESummand(m,c,a,gsMakeNil(),n)
		);
	}

	return gsMakeLPE(ATmakeList0(),pars,ATreverse(smds));
}

static ATermList convert_init(ATermAppl spec, ATermList typelist, ATermList *ids)
{
	ATermList vars = ATLgetArgument(ATAgetArgument(spec,1),1);
	ATermList vals = ATLgetArgument(ATAgetArgument(spec,1),0);
	ATermList l = ATmakeList0();

	for (; !ATisEmpty(vars); vars=ATgetNext(vars),vals=ATgetNext(vals))
	{
		ATermAppl v = ATAgetFirst(vars);
		l = ATinsert(l,
			(ATerm) gsMakeAssignment(
				gsMakeDataVarId(ATAgetArgument(v,0),gsMakeSortId(ATAgetArgument(v,1))),
				dataterm2ATermAppl(ATAgetFirst(vals),ATmakeList0(),typelist)
			)
		);
	}

	return ATreverse(l);
}



/*****************************************************
 ******************* Main function *******************
 *****************************************************/

ATermAppl translate(ATermAppl spec)
{
	ATermAppl sorts,cons,maps,datas,acts,lpe,init;
	ATermList typelist, ids;

	ids = ATmakeList0();

	gsVerboseMsg("converting sort declarations...\n");
	sorts = gsMakeSortSpec(convert_sorts(spec,&ids));

	gsVerboseMsg("converting constructor function declarations...\n");
	cons = gsMakeConsSpec(convert_cons(spec,&ids));

	gsVerboseMsg("converting mapping declarations...\n");
	maps = gsMakeMapSpec(convert_maps(spec,&ids));

	typelist = ATconcat(ATLgetArgument(cons,0),ATLgetArgument(maps,0));

	gsVerboseMsg("converting data equations...\n");
	datas = gsMakeDataEqnSpec(convert_datas(spec,typelist,&ids));

	gsVerboseMsg("converting initial LPE state...\n");
	init = gsMakeLPEInit(ATmakeList0(),convert_init(spec,typelist,&ids));
	
	gsVerboseMsg("converting LPE...\n");
	lpe = convert_lpe(spec,typelist,&ids);
	
	gsVerboseMsg("constructing action declarations...\n");
	acts = gsMakeActSpec(get_lpe_acts(lpe,&ids));

	ATermAppl r = gsMakeSpecV1(sorts,cons,maps,datas,acts,lpe,init);

	r = (ATermAppl) gsSubstValues(get_substs(ids),(ATerm) r,true);

	r = (ATermAppl) gsSubstValues(
			 ATmakeList2(
				(ATerm) gsMakeSubst(
					(ATerm) gsMakeOpId(gsString2ATermAppl("T"),gsMakeSortId(gsString2ATermAppl("Bool"))),
					(ATerm) gsMakeDataExprTrue()
					),
				(ATerm) gsMakeSubst(
					(ATerm) gsMakeOpId(gsString2ATermAppl("F"),gsMakeSortId(gsString2ATermAppl("Bool"))),
					(ATerm) gsMakeDataExprFalse()
					)
				),
	 		(ATerm) r,
			true
			);

	return r;
}
