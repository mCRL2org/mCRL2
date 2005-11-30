#include <stdlib.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "librewrite.h"
#include "enum_standard.h"

#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisInt(x) (ATgetType(x) == AT_INT)

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))

#define MAX_VARS_INIT	1000
#define MAX_VARS_FACTOR	5

void EnumeratorStandard::fs_reset()
{
	fs_stack_pos = 0;
}

void EnumeratorStandard::fs_push(ATermList vars, ATermList vals, ATerm expr)
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

void EnumeratorStandard::fs_pop(fs_expr *e)
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


void EnumeratorStandard::ss_reset()
{
	ss_stack_pos = 0;
}

void EnumeratorStandard::ss_push(ATermList s)
{
	if  ( ss_stack_size <= ss_stack_pos )
	{
		int i = ss_stack_size;
		if ( ss_stack_size == 0 )
		{
			ss_stack_size = 32;
		} else {
			ss_stack_size = ss_stack_size * 2;
			ATunprotectArray((ATerm *) ss_stack);
		}
		ss_stack = (ATermList *) realloc(ss_stack,ss_stack_size*sizeof(ATermList));
		for (; i<ss_stack_size; i++)
		{
			ss_stack[i] = NULL;
		}
		ATprotectArray((ATerm *) ss_stack,ss_stack_size);
	}

	ss_stack[ss_stack_pos] = s;
	ss_stack_pos++;
}

ATermList EnumeratorStandard::ss_pop()
{
	ss_stack_pos--;
	
	ATermList r = ss_stack[ss_stack_pos];
	
	ss_stack[ss_stack_pos] = NULL;

	return r;
}

#define ss_filled() (ss_stack_pos > 0)


bool EnumeratorStandard::IsInner3Eq(ATerm a)
{
	if ( ATisInt(a) )
	{
		a = (ATerm) rewr_obj.fromRewriteFormat(a);
	}

	if ( ATisEqual(ATgetArgument((ATermAppl) a,0),eqstr) )
	{
		return true;
	} else {
		return false;
	}
}

bool EnumeratorStandard::FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
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
bool EnumeratorStandard::IsInnerCEq(ATermAppl a)
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
	a = rewr_obj.fromRewriteFormat((ATerm) a);
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

bool EnumeratorStandard::FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
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

void EnumeratorStandard::EliminateVars(fs_expr *e)
{
	ATermList vars = e->vars;
	ATermList vals = e->vals;
	ATerm expr = e->expr;

	ATerm var,val;
	while ( !ATisEmpty(vars) && (this->*FindEquality)(expr,vars,&var,&val) )
	{
		vars = ATremoveElement(vars, var);
		rewr_obj.setSubstitution((ATermAppl) var,val);
		vals = ATinsert(vals,(ATerm) ATmakeAppl2(tupAFun,var,val));		
		expr = rewr_obj.rewriteInternal(expr);
		rewr_obj.clearSubstitution((ATermAppl) var);
	}

	e->vars = vars;
	e->vals = vals;
	e->expr = expr;
}

ATerm EnumeratorStandard::build_solution_single(ATerm t, ATermList substs)
{
	while ( !ATisEmpty(substs) && !ATisEqual(t, ATgetArgument((ATermAppl) ATgetFirst(substs),0)) )
	{
		substs = ATgetNext(substs);
	}

	if ( ATisEmpty(substs) )
	{
		return t;
	} else {
		return (this->*build_solution_aux)(ATgetArgument((ATermAppl) ATgetFirst(substs),1),ATgetNext(substs));
	}
}

ATerm EnumeratorStandard::build_solution_aux_inner3(ATerm t, ATermList substs)
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

ATerm EnumeratorStandard::build_solution_aux_innerc(ATerm t, ATermList substs)
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

ATermList EnumeratorStandard::build_solution2(ATermList vars, ATermList substs)
{
	if ( ATisEmpty(vars) )
	{
		return vars;
	} else {
		return ATinsert(build_solution2(ATgetNext(vars),substs),(ATerm) gsMakeSubst(ATgetFirst(vars),rewr_obj.rewriteInternal(build_solution_single(ATgetFirst(vars),substs))));
	}
}
ATermList EnumeratorStandard::build_solution(ATermList vars, ATermList substs)
{
	return build_solution2(vars,ATreverse(substs));
}

EnumeratorStandard::EnumeratorStandard(ATermAppl spec, Rewriter &r) :
	rewr_obj(r)
{
	clean_up_rewr_obj = false;

	initialiseEnumerator(spec);
}

EnumeratorStandard::EnumeratorStandard(ATermAppl spec, Rewriter *r, bool clean_up_rewriter) :
	rewr_obj(*r)
{
	prewr_obj = r;
	clean_up_rewr_obj = clean_up_rewriter;

	initialiseEnumerator(spec);
}

EnumeratorStandard::~EnumeratorStandard()
{
	ATunprotectList(&enum_vars);
	ATunprotect(&enum_expr);

	ATunprotect(&rewr_true);
	ATunprotect(&rewr_false);

	ATunprotect(&opidAnd);
	ATunprotect(&eqstr);
	
	ATunprotectAFun(tupAFun);

	ATtableDestroy(constructors);

	free(ss_stack);
	free(fs_stack);

	if ( clean_up_rewr_obj )
	{
		delete prewr_obj;
	}
}

void EnumeratorStandard::initialiseEnumerator(ATermAppl spec)
{
	max_vars = MAX_VARS_INIT;
	fs_stack = NULL;
	fs_stack_size = 0;
	fs_stack_pos = 0;
	ss_stack = NULL;
	ss_stack_size = 0;
	ss_stack_pos = 0;

	rewr_true = rewr_obj.toRewriteFormat(gsMakeDataExprTrue());
	ATprotect(&rewr_true);
	rewr_false = rewr_obj.toRewriteFormat(gsMakeDataExprFalse());
	ATprotect(&rewr_false);

	if ( rewr_obj.getStrategy() == GS_REWR_INNER )
	{
		FindEquality = &EnumeratorStandard::FindInner3Equality;
		build_solution_aux = &EnumeratorStandard::build_solution_aux_inner3;
		opidAnd = rewr_obj.toRewriteFormat(gsMakeOpIdAnd());
		ATprotect(&opidAnd);
		eqstr = (ATerm) gsString2ATermAppl("==");
		ATprotect(&eqstr);
	} else {
		FindEquality = &EnumeratorStandard::FindInnerCEquality;
		build_solution_aux = &EnumeratorStandard::build_solution_aux_innerc;
		opidAnd = ATgetArgument((ATermAppl) rewr_obj.toRewriteFormat(gsMakeOpIdAnd()),0);
		ATprotect(&opidAnd);
		eqstr = (ATerm) gsString2ATermAppl("==");
		ATprotect(&eqstr);
	}

	tupAFun = ATmakeAFun("@tup@",2,ATfalse);
	ATprotectAFun(tupAFun);

	constructors = ATtableCreate(ATgetLength(ATLgetArgument(ATAgetArgument(spec,0),0)),50);
	for (ATermList sorts=ATLgetArgument(ATAgetArgument(spec,0),0); !ATisEmpty(sorts); sorts=ATgetNext(sorts))
	{
		ATtablePut(constructors,ATgetFirst(sorts),(ATerm) ATmakeList0());
	}
	for (ATermList conss = ATLgetArgument(ATAgetArgument(spec,1),0); !ATisEmpty(conss); conss=ATgetNext(conss))
	{
		ATermAppl cons = ATAgetFirst(conss);
		ATerm sort = (ATerm) gsGetSortExprResult(ATAgetArgument(cons,1));
		ATtablePut(constructors,sort,(ATerm) ATinsert((ATermList) ATtableGet(constructors,sort),(ATerm) ATmakeAppl2(tupAFun,(ATerm) cons,(ATerm) gsGetSortExprDomain(ATAgetArgument(cons,1)))));
	}
	
	enum_vars = NULL;
	enum_expr = NULL;
	ATprotectList(&enum_vars);
	ATprotect(&enum_expr);
}

void EnumeratorStandard::initialise(ATermList vars, ATerm expr)
{
	enum_vars = vars;
	enum_expr = expr;

	error = false;
	used_vars = 0;
	fs_reset();
	ss_reset();

	fs_push(vars,ATmakeList0(),rewr_obj.rewriteInternal(expr));
	if ( !ATisEmpty(vars) )
	{
		EliminateVars(&fs_bottom());
	}
	
	if ( ATisEmpty(fs_bottom().vars) )
	{
		if ( ATisEqual(fs_bottom().expr,rewr_true) )
		{
			ss_push(build_solution(vars,fs_bottom().vals));
		} else {
			if ( !ATisEqual(fs_bottom().expr,rewr_false) )
			{
				gsfprintf(stderr,"Term does not evaluate to true or false: %P\n",rewr_obj.fromRewriteFormat(fs_bottom().expr));
				error = true;
			}
		}
		fs_pop();
	}
}

bool EnumeratorStandard::next(ATermList *solution)
{
	while ( !ss_filled() && fs_filled() )
	{
		fs_expr e;

		fs_pop(&e);

		ATermAppl var = (ATermAppl) ATgetFirst(e.vars);
		ATermAppl sort = (ATermAppl) ATgetArgument(var,1);

		if ( gsIsSortArrow(sort) )
		{
			gsErrorMsg("cannot enumerate all elements of functions sorts\n");
			error = true;
			fs_reset();
		} else {
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
						for (ATermList k=enum_vars; !ATisEmpty(k); k=ATgetNext(k))
						{
							if ( k != enum_vars )
							{
								gsfprintf(stderr,", ");
							}
							gsfprintf(stderr,"%P: %P",ATgetFirst(k),ATgetArgument((ATermAppl) ATgetFirst(k),1));
						}
						gsfprintf(stderr," that satisfy %P\n",rewr_obj.fromRewriteFormat(rewr_obj.rewriteInternal(enum_expr)));
						max_vars *= MAX_VARS_FACTOR;
					}
				}
				ATerm term_rf = rewr_obj.rewriteInternal(rewr_obj.toRewriteFormat(cons_term));
			
				rewr_obj.setSubstitution(var,term_rf);
				ATerm new_expr = rewr_obj.rewriteInternal(e.expr);
				if ( !ATisEqual(new_expr,rewr_false) )
				{
					fs_push(ATgetNext(ATreverse(uvars)),ATinsert(e.vals,(ATerm) ATmakeAppl2(tupAFun,(ATerm) var,(ATerm) term_rf)),new_expr);
					if ( ATisEmpty(fs_top().vars) || (EliminateVars(&fs_top()), ATisEmpty(fs_top().vars)) )
					{
						if ( ATisEqual(fs_top().expr,rewr_true) )
						{
							ss_push(build_solution(enum_vars,fs_top().vals));
						} else {
							if ( !ATisEqual(fs_top().expr,rewr_false) )
							{
								gsfprintf(stderr,"Term does not evaluate to true or false: %P\n",rewr_obj.fromRewriteFormat(fs_top().expr));
								error = true;
								fs_reset();
								rewr_obj.clearSubstitution(var);
								break;
							}
						}
						fs_pop(NULL);
					}
				}
				rewr_obj.clearSubstitution(var);
			}
		}
	}

	if ( ss_filled() )
	{
		*solution = ss_pop();
		return true;
	} else {
		*solution = NULL;
		return false;
	}
}

bool EnumeratorStandard::errorOccurred()
{
	return error;
}

ATermList EnumeratorStandard::FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f)
{
	initialise(Vars,Expr);
	ATermList r = ATmakeList0();

	ATermList l;
	while ( next(&l) && !error )
	{
		if ( f == NULL )
		{
			ATinsert(r,(ATerm) l);
		} else {
			f(l);
		}
	}

	return r;
}
