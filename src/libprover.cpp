#include <stdlib.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "librewrite_c.h"
#include "libprover.h"

#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisInt(x) (ATgetType(x) == AT_INT)

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))

bool FindSolutionsError;

static ATermAppl current_spec;
static ATermTable constructors;
static ATerm gsProverTrue, gsProverFalse;

static int used_vars;

#define MAX_VARS_INIT	1000
#define MAX_VARS_FACTOR	5
static int max_vars = MAX_VARS_INIT;

static bool (*FindEquality)(ATerm,ATermList,ATerm*,ATerm*);
static ATerm (*build_solution_aux)(ATerm,ATermList);
static bool FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
static bool FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
static ATerm build_solution_aux_inner3(ATerm t, ATermList substs);
static ATerm build_solution_aux_innerc(ATerm t, ATermList substs);
static ATerm opidAnd,eqstr;
static AFun tupAFun;

typedef struct {
	ATermList vars;
	ATermList vals;
	ATerm expr;
} fs_expr;

fs_expr *fs_stack;
int fs_stack_size = 0;
int fs_stack_pos = 0;

static void fs_reset()
{
	fs_stack_pos = 0;
}

static void fs_push(ATermList vars, ATermList vals, ATerm expr)
{
	if  ( fs_stack_size <= fs_stack_pos )
	{
		int i = fs_stack_size;
		if ( fs_stack_size == 0 )
		{
			fs_stack_size = 512;
		} else {
			fs_stack_size = fs_stack_size * 2;
			ATunprotectArray((ATerm *) fs_stack);
		}
		fs_stack = (fs_expr *) realloc(fs_stack,fs_stack_size*sizeof(fs_expr));
		for (; i<fs_stack_size; i++)
		{
			fs_stack[i].vars = NULL;
			fs_stack[i].vals = NULL;
			fs_stack[i].expr = NULL;
		}
		ATprotectArray((ATerm *) fs_stack,3*fs_stack_size);
	}

	fs_stack[fs_stack_pos].vars = vars;
	fs_stack[fs_stack_pos].vals = vals;
	fs_stack[fs_stack_pos].expr = expr;
	fs_stack_pos++;
}

static void fs_pop(fs_expr *e)
{
	fs_stack_pos--;
	if ( e != NULL )
	{
		e->vars = fs_stack[fs_stack_pos].vars;
		e->vals = fs_stack[fs_stack_pos].vals;
		e->expr = fs_stack[fs_stack_pos].expr;
	}
	fs_stack[fs_stack_pos].vars = NULL;
	fs_stack[fs_stack_pos].vals = NULL;
	fs_stack[fs_stack_pos].expr = NULL;
}

#define fs_bottom() (fs_stack[0])
#define fs_top() (fs_stack[fs_stack_pos-1])

#define fs_filled() (fs_stack_pos > 0)


void gsProverInit(ATermAppl Spec, RewriteStrategy strat)
{
	current_spec = Spec;
	ATprotectAppl(&current_spec);
	gsRewriteInit(ATAgetArgument(Spec,3), strat);
	gsProverTrue = gsToRewriteFormat(gsMakeDataExprTrue());
	ATprotect(&gsProverTrue);
	gsProverFalse = gsToRewriteFormat(gsMakeDataExprFalse());
	ATprotect(&gsProverFalse);

	if ( strat == GS_REWR_INNER3 || strat == GS_REWR_INNER )
	{
		FindEquality = FindInner3Equality;
		build_solution_aux = build_solution_aux_inner3;
		opidAnd = gsToRewriteFormat(gsMakeOpIdAnd());
		ATprotect(&opidAnd);
		eqstr = (ATerm) gsString2ATermAppl("==");
		ATprotect(&eqstr);
	} else {
		FindEquality = FindInnerCEquality;
		build_solution_aux = build_solution_aux_innerc;
		opidAnd = ATgetArgument((ATermAppl) gsToRewriteFormat(gsMakeOpIdAnd()),0);
		ATprotect(&opidAnd);
		eqstr = (ATerm) gsString2ATermAppl("==");
		ATprotect(&eqstr);
	}

	tupAFun = ATmakeAFun("@tup@",2,ATfalse);
	ATprotectAFun(tupAFun);

	constructors = ATtableCreate(ATgetLength(ATLgetArgument(ATAgetArgument(Spec,0),0)),50);
	for (ATermList sorts=ATLgetArgument(ATAgetArgument(Spec,0),0); !ATisEmpty(sorts); sorts=ATgetNext(sorts))
	{
		ATtablePut(constructors,ATgetFirst(sorts),(ATerm) ATmakeList0());
	}
	for (ATermList conss = ATLgetArgument(ATAgetArgument(Spec,1),0); !ATisEmpty(conss); conss=ATgetNext(conss))
	{
		ATermAppl cons = ATAgetFirst(conss);
		ATerm sort = (ATerm) gsGetSortExprResult(ATAgetArgument(cons,1));
		ATtablePut(constructors,sort,(ATerm) ATinsert((ATermList) ATtableGet(constructors,sort),(ATerm) ATmakeAppl2(tupAFun,(ATerm) cons,(ATerm) gsGetSortExprDomain(ATAgetArgument(cons,1)))));
	}
}

void gsProverFinalise()
{
	ATunprotectAppl(&current_spec);
	gsRewriteFinalise();
	ATunprotect(&gsProverTrue);
	ATunprotect(&gsProverFalse);

	ATunprotect(&opidAnd);
	ATunprotect(&eqstr);
	
	ATunprotectAFun(tupAFun);

	ATtableDestroy(constructors);
}

static bool IsInner3Eq(ATerm a)
{
	if ( ATisInt(a) )
	{
		a = (ATerm) gsFromRewriteFormat(a);
	}

	if ( ATisEqual(ATgetArgument((ATermAppl) a,0),eqstr) )
	{
		return true;
	} else {
		return false;
	}
}

static bool FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
{
	ATermList s;
	ATerm a;

	s = ATmakeList1(t);
	while ( !ATisEmpty(s) )
	{
		ATerm a1,a2;

		a = ATgetFirst(s);
		s = ATgetNext(s);

		if ( !ATisList(a) )
		{
			continue;
		}

		if ( ATisEqual(ATgetFirst((ATermList) a),opidAnd) )
		{
			s = ATconcat(s,ATgetNext((ATermList) a));
		} else if ( IsInner3Eq(ATgetFirst((ATermList) a)) ) {
			a1 = ATgetFirst(ATgetNext((ATermList) a));
			a2 = ATgetFirst(ATgetNext(ATgetNext((ATermList) a)));
			if ( !ATisEqual(a1,a2) )
			{
				if ( ATisAppl(a1) && gsIsDataVarId((ATermAppl) a1) && (ATindexOf(vars, a1,0) >= 0) && !gsOccurs(a1,a2) )
				{
					*v = a1;
					*e = a2;
					return true;
				}
				if ( ATisAppl(a2) && gsIsDataVarId((ATermAppl) a2) && (ATindexOf(vars, a2,0) >= 0) && !gsOccurs(a2,a1) )
				{
					*v = a2;
					*e = a1;
					return true;
				}
			}
		}
	}

	return false;
}

//static ATerm *ceqs = NULL; // UNPROTECTED!!! (should be safe though)
//static int num_ceqs;
//static int ceqs_size;
//#define CEQS_STEP 20
static bool IsInnerCEq(ATermAppl a)
{
/*	if ( (ceqs == NULL) )
	{
		ceqs = (ATerm *) malloc(CEQS_STEP*sizeof(ATerm));
		ceqs_size = CEQS_STEP;
		num_ceqs = 0;
	}

	ATerm b = ATgetArgument(a,0);
	if ( ATisAppl(b) && gsIsDataVarId((ATermAppl) b) )
	{
		return false;
	}
	for (int i=0; i<num_ceqs; i++)
	{
		if ( ATisEqual(b,ceqs[i]) )
		{
			return true;
		}
	}
*/
	a = gsFromRewriteFormat((ATerm) a);
	a = (ATermAppl) ATgetArgument(a,0);
	a = (ATermAppl) ATgetArgument(a,0);

	if ( ATisEqual(ATgetArgument(a,0),eqstr) )
	{
/*		if ( num_ceqs >= ceqs_size )
		{
			ceqs_size += CEQS_STEP;
			ceqs = (ATerm *) realloc(ceqs,ceqs_size*sizeof(ATerm));
		}
		ceqs[num_ceqs] = (ATerm) a;
		num_ceqs++;
*/
		return true;
	} else {
		return false;
	}
}

static bool FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
{
	ATermList s;
	ATermAppl a;

	s = ATmakeList1((ATerm) t);
	while ( !ATisEmpty(s) )
	{
		ATermAppl a1,a2;

		a = (ATermAppl) ATgetFirst(s);
		s = ATgetNext(s);

		if ( gsIsDataVarId(a) || (ATgetArity(ATgetAFun(a)) != 3) )
		{
			continue;
		}

		if ( ATisEqual(ATgetArgument(a,0),opidAnd) )
		{
			s = ATinsert(s,ATgetArgument(a,2));
			s = ATinsert(s,ATgetArgument(a,1));
		} else if ( IsInnerCEq(a) ) {
			a1 = (ATermAppl) ATgetArgument(a,1);
			a2 = (ATermAppl) ATgetArgument(a,2);
			if ( !ATisEqual(a1,a2) )
			{
				if ( gsIsDataVarId(a1) && (ATindexOf(vars,(ATerm) a1,0) >= 0) && !gsOccurs((ATerm) a1,(ATerm) a2) )
				{
					*v = (ATerm) a1;
					*e = (ATerm) a2;
					return true;
				}
				if ( gsIsDataVarId(a2) && (ATindexOf(vars,(ATerm) a2,0) >= 0) && !gsOccurs((ATerm) a2,(ATerm) a1) )
				{
					*v = (ATerm) a2;
					*e = (ATerm) a1;
					return true;
				}
			}
		}
	}

	return false;
}

static void EliminateVars(fs_expr *e)
{
	ATermList vars = e->vars;
	ATermList vals = e->vals;
	ATerm expr = e->expr;

	ATerm var,val;
	while ( !ATisEmpty(vars) && FindEquality(expr,vars,&var,&val) )
	{
		vars = ATremoveElement(vars, var);
		RWsetVariable(var,val);
		vals = ATinsert(vals,(ATerm) ATmakeAppl2(tupAFun,var,val));		
		expr = gsRewriteInternal(expr);
		RWclearVariable(var);
	}

	e->vars = vars;
	e->vals = vals;
	e->expr = expr;
}

static ATerm build_solution_single(ATerm t, ATermList substs)
{
	while ( !ATisEmpty(substs) && !ATisEqual(t, ATgetArgument((ATermAppl) ATgetFirst(substs),0)) )
	{
		substs = ATgetNext(substs);
	}

	if ( ATisEmpty(substs) )
	{
		return t;
	} else {
		return build_solution_aux(ATgetArgument((ATermAppl) ATgetFirst(substs),1),ATgetNext(substs));
	}
}

static ATerm build_solution_aux_inner3(ATerm t, ATermList substs)
{
	if ( ATisInt(t) )
	{
		return t;
	} else if ( ATisList(t) )
	{
		ATerm head = ATgetFirst((ATermList) t);
		ATermList args = ATmakeList0();

		if ( !ATisInt(head) )
		{
			head = build_solution_single(head,substs);
			if ( ATisList(head) )
			{
				for (ATermList l=ATgetNext((ATermList) head); !ATisEmpty(l); l=ATgetNext(l))
				{
					args = ATinsert(args, ATgetFirst(l));
				}
				head = ATgetFirst((ATermList) head);
			}
		}

		for (ATermList l=ATgetNext((ATermList) t); !ATisEmpty(l); l=ATgetNext(l))
		{
			args = ATinsert(args,build_solution_aux_inner3(ATgetFirst(l),substs));
		}

		return (ATerm) ATinsert(ATreverse(args),head);
	} else {
		if ( gsIsDataVarId((ATermAppl) t) )
		{
			return build_solution_single(t,substs);
		} else {
			return t;
		}
	}
}

static ATerm build_solution_aux_innerc(ATerm t, ATermList substs)
{
	if ( gsIsDataVarId((ATermAppl) t) )
	{
		return build_solution_single(t,substs);
	} else {
		ATerm head = ATgetArgument((ATermAppl) t,0);
		int arity = ATgetArity(ATgetAFun((ATermAppl) t));
		int extra_arity = 0;

		if ( !ATisInt(head) )
		{
			head = build_solution_single(head,substs);
			if ( !gsIsDataVarId((ATermAppl) head) )
			{
				extra_arity = ATgetArity(ATgetAFun((ATermAppl) head))-1;
			}
		}

		DECL_A(args,ATerm,arity+extra_arity);
		AFun fun = ATgetAFun((ATermAppl) t);
		int k = 1;

		if ( !ATisInt(head) && !gsIsDataVarId((ATermAppl) head) )
		{
			fun = ATmakeAFun("@appl_bs@",arity+extra_arity,ATfalse);
			k = extra_arity+1;
			for (int i=1; i<k; i++)
			{
				args[i] = ATgetArgument((ATermAppl) head,i);
			}
			head = ATgetArgument((ATermAppl) head,0);
		}

		args[0] = head;
		for (int i=1; i<arity; i++,k++)
		{
			args[k] = build_solution_aux_innerc(ATgetArgument((ATermAppl) t,i),substs);
		}

		ATerm r = (ATerm) ATmakeApplArray(fun,args);
		FREE_A(args);
		return r;
	}
}

static ATermList build_solution2(ATermList vars, ATermList substs)
{
	if ( ATisEmpty(vars) )
	{
		return vars;
	} else {
		return ATinsert(build_solution2(ATgetNext(vars),substs),(ATerm) gsMakeSubst(ATgetFirst(vars),build_solution_single(ATgetFirst(vars),substs)));
	}
}
static ATermList build_solution(ATermList vars, ATermList substs)
{
	return build_solution2(vars,ATreverse(substs));
}

ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f)
{
	FindSolutionsError = false;
	used_vars = 0;
	fs_reset();

	fs_push(Vars,ATmakeList0(),gsRewriteInternal(Expr));
	if ( !ATisEmpty(Vars) )
	{
		EliminateVars(&fs_bottom());
	}

	if ( ATisEqual(fs_bottom().expr,gsProverFalse) )
	{
		return ATmakeList0();
	}

	if ( ATisEmpty(fs_bottom().vars) )
	{
		if ( ATisEqual(fs_bottom().expr,gsProverTrue) )
		{
			if ( f == NULL )
			{
				return ATmakeList1((ATerm) build_solution(Vars,fs_bottom().vals));
			} else {
				f(build_solution(Vars,fs_bottom().vals));
				return ATmakeList0();
			}
		} else {
			if ( !ATisEqual(fs_bottom().expr,gsProverFalse) )
			{
				gsfprintf(stderr,"Term does not evaluate to true or false: %P\n",gsFromRewriteFormat(fs_bottom().expr));
				FindSolutionsError = true;
			}
			return ATmakeList0();
		}
	}

	ATermList r = ATmakeList0();

	while ( fs_filled() )
	{
		fs_expr e;

		fs_pop(&e);

		ATermAppl var = (ATermAppl) ATgetFirst(e.vars);
		ATermAppl sort = (ATermAppl) ATgetArgument(var,1);

		if ( gsIsSortArrow(sort) )
		{
			gsErrorMsg("cannot enumerate all elements of functions sorts\n");
			FindSolutionsError = true;
			return ATmakeList0();
		}

		ATermList r = ATmakeList0();

		for (ATermList l=(ATermList) ATtableGet(constructors,(ATerm) sort); !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl cons_tup = (ATermAppl) ATgetFirst(l);
			ATermAppl cons_term = (ATermAppl) ATgetArgument(cons_tup,0);
			ATermList dom_sorts = (ATermList) ATgetArgument(cons_tup,1);
			
			ATermList uvars = ATreverse(e.vars);
			
			for (; !ATisEmpty(dom_sorts); dom_sorts=ATgetNext(dom_sorts))
			{
				ATermAppl fv = gsMakeDataVarId(gsFreshString2ATermAppl("@enum@",(ATerm) uvars,false),ATAgetFirst(dom_sorts));
				
				uvars = ATinsert(uvars,(ATerm) fv);
				cons_term = gsMakeDataAppl(cons_term,fv);
				
				used_vars++;
				if ( used_vars > max_vars )
				{
					gsfprintf(stderr,"warning: Need more than %i variables to find all valuations of ",max_vars);
					for (ATermList k=Vars; !ATisEmpty(k); k=ATgetNext(k))
					{
						if ( k != Vars )
						{
							gsfprintf(stderr,", ");
						}
						gsfprintf(stderr,"%P: %P",ATgetFirst(k),ATgetArgument((ATermAppl) ATgetFirst(k),1));
					}
					gsfprintf(stderr," that satisfy %P\n",gsFromRewriteFormat(gsRewriteInternal(Expr)));
					max_vars *= MAX_VARS_FACTOR;
				}
			}
			ATerm term_rf = gsRewriteInternal(gsToRewriteFormat(cons_term));
			
			RWsetVariable((ATerm) var,term_rf);
			ATerm new_expr = gsRewriteInternal(e.expr);
			if ( !ATisEqual(new_expr,gsProverFalse) )
			{
				fs_push(ATgetNext(ATreverse(uvars)),ATinsert(e.vals,(ATerm) ATmakeAppl2(tupAFun,(ATerm) var,(ATerm) term_rf)),new_expr);
				if ( ATisEmpty(fs_top().vars) || (EliminateVars(&fs_top()), ATisEmpty(fs_top().vars)) )
				{
					if ( ATisEqual(fs_top().expr,gsProverTrue) )
					{
						if ( f == NULL )
						{
							r = ATinsert(r,(ATerm) build_solution(Vars,fs_top().vals));
						} else {
							f(build_solution(Vars,fs_top().vals));
						}
					} else {
						if ( !ATisEqual(fs_top().expr,gsProverFalse) )
						{
							gsfprintf(stderr,"Term does not evaluate to true or false: %P\n",gsFromRewriteFormat(fs_top().expr));
							FindSolutionsError = true;
							return ATmakeList0();
						}
					}
					fs_pop(NULL);
				}
			}
			RWclearVariable((ATerm) var);
		}
		
	}

	return r;
}
