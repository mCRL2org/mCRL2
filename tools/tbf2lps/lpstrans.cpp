// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpstrans.cpp

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
#include <assert.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

bool is_mCRL_spec(ATermAppl spec)
{
  return ATgetAFun(spec) == ATmakeAFun("spec2gen", 2, ATfalse);
}

static void add_id(ATermList *ids, ATermAppl id)
{
	if ( ATindexOf(*ids,(ATerm) id,0) == -1 )
	{
		*ids = ATappend(*ids,(ATerm) id);
	}
}

static ATbool is_domain(ATermList args, ATermAppl sort)
{
	if ( !gsIsSortId(sort) )
	{
		ATermList dom = ATLgetArgument(sort,0);
		if ( ATgetLength(args) != ATgetLength(dom) )
		{
			return ATfalse;
		} else {
			for (; !ATisEmpty(dom); dom=ATgetNext(dom),args=ATgetNext(args))
			{
				if ( !ATisEqual(gsGetSort(ATAgetFirst(args)),ATgetFirst(dom)) )
				{
					return ATfalse;
				}
			}
			return ATtrue;
		}
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
	
	t2 = ATmakeAppl0(ATmakeAFun(ATgetName(ATgetAFun(t)),0,ATtrue));

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

                r = gsMakeDataAppl(r,m);
	}
	
	return r;
}

static ATermList get_lps_acts(ATermAppl lps, ATermList *ids)
{
	ATermList acts = ATmakeList0();
	ATermList sums = ATLgetArgument(lps,2);

	for (; !ATisEmpty(sums); sums=ATgetNext(sums))
	{
		if ( !ATisEmpty(ATLgetArgument(ATAgetArgument(ATAgetFirst(sums),2),0)) )
		{
			ATermAppl a = ATAgetArgument(ATAgetFirst(ATLgetArgument(ATAgetArgument(ATAgetFirst(sums),2),0)),0);
			if ( ATindexOf(acts,(ATerm) a,0) == -1 )
			{
				acts = ATinsert(acts,(ATerm) a);
				add_id(ids,ATAgetArgument(a,0));
			}
		}
	}

	return ATreverse(acts);
}

static ATermList get_substs(ATermList ids)
{
	ATermIndexedSet used = ATindexedSetCreate(1000,50);
	ATermList substs = ATmakeList0();

	for (; !ATisEmpty(ids); ids=ATgetNext(ids))
	{
		char s[100], *t;

		t = ATgetName(ATgetAFun(ATAgetFirst(ids)));
		if ( t[0] >= '0' && t[0] <= '9' )
		{
			s[0] = '_';
			strncpy(s+1,t,100);
		} else {
			strncpy(s,t,100);
		}

		s[99] = '#';
		for (t=s; *t && (*t)!='#'; t++)
		{
			if ( !( (*t >= 'A' && *t <= 'Z') ||
				(*t >= 'a' && *t <= 'z') ||
				(*t >= '0' && *t <= '9') ||
				*t == '_' ) )
			{
			  *t = '_';
			}
		}
		*t = 0;

		unsigned int i = 0;
		ATermAppl new_id;
		while ( ATindexedSetGetIndex(used,(ATerm) (new_id = ATmakeAppl0(ATmakeAFun(s,0,ATtrue)))) >= 0 )
		{
			sprintf(t,"%i",i);
			i++;
		}

		ATbool b;
		ATindexedSetPut(used,(ATerm) new_id,&b);

		substs = ATinsert(substs,(ATerm) gsMakeSubst(ATgetFirst(ids),(ATerm) new_id));
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
	ATermList r,l,m, sorts;
	ATermAppl sort;

	r = ATmakeList0();
	for (; !ATisEmpty(funcs); funcs=ATgetNext(funcs))
	{
		add_id(ids,ATAgetArgument(ATAgetFirst(funcs),0));

		m = ATmakeList0();
		l = ATreverse(ATLgetArgument(ATAgetFirst(funcs),1));
                sorts = ATmakeList0();
		sort = gsMakeSortId(ATAgetArgument(ATAgetFirst(funcs),2));
                for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			sorts = ATinsert(sorts, (ATerm) gsMakeSortId(ATAgetFirst(l)));
		}
		if ( !ATisEmpty(sorts) )
		{
			sort = gsMakeSortArrow(sorts, sort);
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

static ATermAppl convert_lps(ATermAppl spec, ATermList typelist, ATermList *ids)
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
			as = ATinsert(as,(ATerm) gsGetSort(ATAgetFirst(al)));
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
			ATermAppl par = ATAgetFirst(o2);
			ATermAppl val = dataterm2ATermAppl(ATAgetFirst(l),o,typelist);
			if ( !ATisEqual(par,val) )
			{
				n = ATinsert(n,(ATerm) gsMakeDataVarIdInit(par,val));
			}
		}
		n = ATreverse(n);

		smds = ATinsert(smds,
			(ATerm) gsMakeLinearProcessSummand(m,c,a,gsMakeNil(),n)
		);
	}

	return gsMakeLinearProcess(ATmakeList0(),pars,ATreverse(smds));
}

static ATermList convert_init(ATermAppl spec, ATermList typelist, ATermList * /*ids*/)
{
	ATermList vars = ATLgetArgument(ATAgetArgument(spec,1),1);
	ATermList vals = ATLgetArgument(ATAgetArgument(spec,1),0);
	ATermList l = ATmakeList0();

	for (; !ATisEmpty(vars); vars=ATgetNext(vars),vals=ATgetNext(vals))
	{
		ATermAppl v = ATAgetFirst(vars);
		l = ATinsert(l,
			(ATerm) gsMakeDataVarIdInit(
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

ATermAppl translate(ATermAppl spec, bool convert_bools, bool convert_funcs)
{
        assert(is_mCRL_spec(spec));
	ATermAppl sort_spec,cons_spec,map_spec,data_eqn_spec,data_spec,act_spec,lps,init;
	ATermList typelist, ids;

	ids = ATmakeList0();

	gsVerboseMsg("converting sort declarations...\n");
	sort_spec = gsMakeSortSpec(convert_sorts(spec,&ids));

	gsVerboseMsg("converting constructor function declarations...\n");
	cons_spec = gsMakeConsSpec(convert_cons(spec,&ids));

	gsVerboseMsg("converting mapping declarations...\n");
	map_spec = gsMakeMapSpec(convert_maps(spec,&ids));

	typelist = ATconcat(ATLgetArgument(cons_spec,0),ATLgetArgument(map_spec,0));

	gsVerboseMsg("converting data equations...\n");
	data_eqn_spec = gsMakeDataEqnSpec(convert_datas(spec,typelist,&ids));

        data_spec = gsMakeDataSpec(sort_spec, cons_spec, map_spec, data_eqn_spec);

	gsVerboseMsg("converting initial LPE state...\n");
	init = gsMakeLinearProcessInit(ATmakeList0(),convert_init(spec,typelist,&ids));
	
	gsVerboseMsg("converting LPE...\n");
	lps = convert_lps(spec,typelist,&ids);
	
	gsVerboseMsg("constructing action declarations...\n");
	act_spec = gsMakeActSpec(get_lps_acts(lps,&ids));

	ATermAppl r = gsMakeLinProcSpec(data_spec, act_spec, lps, init);

	ATermList substs = ATmakeList0();

	if ( convert_bools )
	{
		substs = ATinsert(substs,
			(ATerm) gsMakeSubst(
				(ATerm) gsMakeOpId(gsString2ATermAppl("F"),gsMakeSortIdBool()),
				(ATerm) gsMakeDataExprFalse()
				)
			);
		substs = ATinsert(substs,
			(ATerm) gsMakeSubst(
				(ATerm) gsMakeOpId(gsString2ATermAppl("T"),gsMakeSortIdBool()),
				(ATerm) gsMakeOpIdTrue()
				)
			);
	}

	if ( convert_funcs )
	{
		ATermAppl bool_func_sort = gsGetSort(gsMakeOpIdAnd());

		substs = ATinsert(substs,
			(ATerm) gsMakeSubst(
				(ATerm) gsMakeOpId(gsString2ATermAppl("and"),bool_func_sort),
				(ATerm) gsMakeOpIdAnd()
				)
			);
		substs = ATinsert(substs,
			(ATerm) gsMakeSubst(
				(ATerm) gsMakeOpId(gsString2ATermAppl("or"),bool_func_sort),
				(ATerm) gsMakeOpIdOr()
				)
			);

		ATermAppl eq_str = gsString2ATermAppl("eq");
		ATermAppl s_bool = gsMakeSortIdBool();

		for (ATermList l=ATLgetArgument(sort_spec,0); !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl s = ATAgetFirst(l);
			substs = ATinsert(substs,(ATerm) gsMakeSubst(
						(ATerm) gsMakeOpId(eq_str,gsMakeSortArrow2(s,s,s_bool)),
						(ATerm) gsMakeOpIdEq(s)
						));
		}
	}

	r = (ATermAppl) gsSubstValues(substs,(ATerm) r,true);


	substs = get_substs(ids);

	r = (ATermAppl) gsSubstValues(substs,(ATerm) r,true);

	
	return r;
}
