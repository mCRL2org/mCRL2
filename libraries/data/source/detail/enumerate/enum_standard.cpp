// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include <cstdlib>
#include <sstream>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/detail/enum/standard.h"

#include "workarounds.h" // DECL_A

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisInt(x) (ATgetType(x) == AT_INT)

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))

#define MAX_VARS_INIT   1000
#define MAX_VARS_FACTOR 5

using namespace std;

namespace mcrl2 {
  namespace data {
    namespace detail {

void EnumeratorSolutionsStandard::fs_reset()
{
        fs_stack_pos = 0;
}

void EnumeratorSolutionsStandard::fs_push(ATermList vars, ATermList vals, ATerm expr)
{
        if  ( fs_stack_size <= fs_stack_pos )
        {
                int i = fs_stack_size;
                if ( fs_stack_size == 0 )
                {
                        fs_stack_size = 4;
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

void EnumeratorSolutionsStandard::fs_pop(fs_expr *e)
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


void EnumeratorSolutionsStandard::ss_reset()
{
        ss_stack_pos = 0;
}

void EnumeratorSolutionsStandard::ss_push(ATermList s)
{
        if  ( ss_stack_size <= ss_stack_pos )
        {
                int i = ss_stack_size;
                if ( ss_stack_size == 0 )
                {
                        ss_stack_size = 4;
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

ATermList EnumeratorSolutionsStandard::ss_pop()
{
        ss_stack_pos--;

        ATermList r = ss_stack[ss_stack_pos];

        ss_stack[ss_stack_pos] = NULL;

        return r;
}

#define ss_filled() (ss_stack_pos > 0)


bool EnumeratorSolutionsStandard::IsInner3Eq(ATerm a)
{
        return ATindexedSetGetIndex(info.eqs,a) >= 0;
}

bool EnumeratorSolutionsStandard::FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
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

                if ( ATisEqual(ATgetFirst((ATermList) a),info.opidAnd) )
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

bool EnumeratorSolutionsStandard::IsInnerCEq(ATermAppl a)
{
        return ATindexedSetGetIndex(info.eqs,ATgetArgument(a,0)) >= 0;
}

static struct { ATermList vars; ATerm *v; ATerm *e; } FindInnerCEquality_struct;
bool EnumeratorSolutionsStandard::FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e)
{
        FindInnerCEquality_struct.vars = vars;
        FindInnerCEquality_struct.v = v;
        FindInnerCEquality_struct.e = e;
        return FindInnerCEquality_aux(t);
}

bool EnumeratorSolutionsStandard::FindInnerCEquality_aux(ATerm t)
{
        if ( gsIsDataVarId((ATermAppl) t) || (ATgetArity(ATgetAFun((ATermAppl) t)) != 3) )
        {
                return false;
        }

        if ( ATisEqual(ATgetArgument((ATermAppl) t,0),info.opidAnd) )
        {
                return FindInnerCEquality_aux(ATgetArgument((ATermAppl) t,1))
                    || FindInnerCEquality_aux(ATgetArgument((ATermAppl) t,2));
        } else if ( IsInnerCEq((ATermAppl) t) ) {
                ATermAppl a1 = (ATermAppl) ATgetArgument((ATermAppl) t,1);
                ATermAppl a2 = (ATermAppl) ATgetArgument((ATermAppl) t,2);
                if ( !ATisEqual(a1,a2) )
                {
                        if ( gsIsDataVarId(a1) && (ATindexOf(FindInnerCEquality_struct.vars,(ATerm) a1,0) >= 0) && !gsOccurs((ATerm) a1,(ATerm) a2) )
                        {
                                *FindInnerCEquality_struct.v = (ATerm) a1;
                                *FindInnerCEquality_struct.e = (ATerm) a2;
                                return true;
                        }
                        if ( gsIsDataVarId(a2) && (ATindexOf(FindInnerCEquality_struct.vars,(ATerm) a2,0) >= 0) && !gsOccurs((ATerm) a2,(ATerm) a1) )
                        {
                                *FindInnerCEquality_struct.v = (ATerm) a2;
                                *FindInnerCEquality_struct.e = (ATerm) a1;
                                return true;
                        }
                }
        }

        return false;
}

void EnumeratorSolutionsStandard::EliminateVars(fs_expr *e)
{
        ATermList vars = e->vars;
        ATermList vals = e->vals;
        ATerm expr = e->expr;

        ATerm var,val;
        while ( !ATisEmpty(vars) && (this->*info.FindEquality)(expr,vars,&var,&val) )
        {
                vars = ATremoveElement(vars, var);
                info.rewr_obj->setSubstitutionInternal((ATermAppl) var,val);
                vals = ATinsert(vals,(ATerm) ATmakeAppl2(info.tupAFun,var,val));
                expr = info.rewr_obj->rewriteInternal(expr);
                info.rewr_obj->clearSubstitution((ATermAppl) var);
        }

        e->vars = vars;
        e->vals = vals;
        e->expr = expr;
}

ATerm EnumeratorSolutionsStandard::build_solution_single(ATerm t, ATermList substs)
{
        while ( !ATisEmpty(substs) && !ATisEqual(t, ATgetArgument((ATermAppl) ATgetFirst(substs),0)) )
        {
                substs = ATgetNext(substs);
        }

        if ( ATisEmpty(substs) )
        {
                return t;
        } else {
                return (this->*info.build_solution_aux)(ATgetArgument((ATermAppl) ATgetFirst(substs),1),ATgetNext(substs));
        }
}

ATerm EnumeratorSolutionsStandard::build_solution_aux_inner3(ATerm t, ATermList substs)
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

ATerm EnumeratorSolutionsStandard::build_solution_aux_innerc(ATerm t, ATermList substs)
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

ATermList EnumeratorSolutionsStandard::build_solution2(ATermList vars, ATermList substs)
{
        if ( ATisEmpty(vars) )
        {
                return vars;
        } else {
                return ATinsert(build_solution2(ATgetNext(vars),substs),(ATerm) gsMakeSubst(ATgetFirst(vars),info.rewr_obj->rewriteInternal(build_solution_single(ATgetFirst(vars),substs))));
        }
}
ATermList EnumeratorSolutionsStandard::build_solution(ATermList vars, ATermList substs)
{
        return build_solution2(vars,ATreverse(substs));
}

bool EnumeratorSolutionsStandard::next(ATermList *solution)
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
                        ATermList l = (ATermList) ATtableGet(info.constructors,(ATerm) sort);
                        if (l == NULL)
                        {
                          l = ATempty;
			}
                        if ( ATisEmpty(l) )
                        {
                                gsErrorMsg("cannot enumerate elements of sort %P; it does not have constructor functions\n",sort);
                                error = true;
                                fs_reset();
                        }
                        for (; !ATisEmpty(l); l=ATgetNext(l))
                        {
                                ATermAppl cons_tup = (ATermAppl) ATgetFirst(l);
                                ATermAppl cons_term = (ATermAppl) ATgetArgument(cons_tup,0);
                                ATermList rdom_sorts = (ATermList) ATgetArgument(cons_tup,1);

                                ATermList uvars = ATreverse(e.vars);

                                for (; !ATisEmpty(rdom_sorts); rdom_sorts=ATgetNext(rdom_sorts))
                                {
                                        ATermList rev_dom_sorts1 = ATLgetFirst(rdom_sorts);
                                        ATermList var_list = ATmakeList0();
                                        for (; !ATisEmpty(rev_dom_sorts1); rev_dom_sorts1=ATgetNext(rev_dom_sorts1))
                                        {
                                                ATermAppl fv = gsMakeDataVarId(gsFreshString2ATermAppl("@enum@",(ATerm) uvars,false),ATAgetFirst(rev_dom_sorts1));
                                                var_list = ATinsert(var_list,(ATerm) fv);
                                                uvars = ATinsert(uvars,(ATerm) fv);

                                                used_vars++;
                                                if ( used_vars > *info.max_vars )
                                                {
                                                        stringstream msg;
                                                        msg << "need more than " << *info.max_vars << " variables to find all valuations of ";
                                                        for (ATermList k=enum_vars; !ATisEmpty(k); k=ATgetNext(k))
                                                        {
                                                                if ( k != enum_vars )
                                                                {
                                                                        msg << ", ";
                                                                }
                                                                PrintPart_CXX(msg,ATgetFirst(k),ppDefault);
                                                                msg << ": ";
                                                                PrintPart_CXX(msg,ATgetArgument((ATermAppl) ATgetFirst(k),1),ppDefault);
                                                        }
                                                        msg << " that satisfy ";
                                                        PrintPart_CXX(msg,(ATerm) info.rewr_obj->fromRewriteFormat(enum_expr),ppDefault);
                                                        msg << endl;
                                                        gsWarningMsg("%s",msg.str().c_str());
                                                        *info.max_vars *= MAX_VARS_FACTOR;
                                                }
                                        }
                                        cons_term = gsMakeDataAppl(cons_term, var_list);
                                }
                                ATerm term_rf = info.rewr_obj->rewriteInternal(info.rewr_obj->toRewriteFormat(cons_term));

                                info.rewr_obj->setSubstitutionInternal(var,term_rf);
                                ATerm new_expr = info.rewr_obj->rewriteInternal(e.expr);
                                if ( !ATisEqual(new_expr,info.rewr_false) )
                                {
                                        fs_push(ATgetNext(ATreverse(uvars)),ATinsert(e.vals,(ATerm) ATmakeAppl2(info.tupAFun,(ATerm) var,(ATerm) term_rf)),new_expr);
                                        if ( ATisEmpty(fs_top().vars) || (EliminateVars(&fs_top()), ATisEmpty(fs_top().vars) || ATisEqual(fs_top().expr,info.rewr_false)) )
                                        {
                                                if ( !ATisEqual(fs_top().expr,info.rewr_false) )
                                                {
                                                        if ( check_true && !ATisEqual(fs_top().expr,info.rewr_true) )
                                                        {
                                                                gsErrorMsg("term does not evaluate to true or false: %P\n",info.rewr_obj->fromRewriteFormat(fs_top().expr));
                                                                error = true;
                                                                fs_reset();
                                                                info.rewr_obj->clearSubstitution(var);
                                                                break;
                                                        } else {
                                                                ss_push(build_solution(enum_vars,fs_top().vals));
                                                        }
                                                }
                                                fs_pop(NULL);
                                        }
                                }
                                info.rewr_obj->clearSubstitution(var);
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

bool EnumeratorSolutionsStandard::errorOccurred()
{
        return error;
}

void EnumeratorSolutionsStandard::reset(ATermList Vars, ATerm Expr, bool true_only)
{
        enum_vars = Vars;
        enum_expr = info.rewr_obj->rewriteInternal(Expr);
        check_true = true_only;

        fs_reset();
        ss_reset();

        error = false;

        used_vars = 0;

        fs_push(enum_vars,ATmakeList0(),enum_expr);
        if ( !ATisEmpty(enum_vars) )
        {
                EliminateVars(&fs_bottom());
        }

        if ( ATisEqual(fs_bottom().expr,info.rewr_false) )
        {
                fs_pop();
        } else if ( ATisEmpty(fs_bottom().vars) )
        {
                if ( check_true && !ATisEqual(fs_bottom().expr,info.rewr_true) )
                {
                        gsErrorMsg("term does not evaluate to true or false: %P\n",info.rewr_obj->fromRewriteFormat(fs_bottom().expr));
                        error = true;
                } else {
                        ss_push(build_solution(enum_vars,fs_bottom().vals));
                }
                fs_pop();
        }
}

EnumeratorSolutionsStandard::EnumeratorSolutionsStandard(ATermList Vars, ATerm Expr, bool true_only, enumstd_info &Info)
{
        info = Info; // ATerms inside need not to be protected as they should already
                     // be protected by the Enumerator and this object should not live
                     // after the dead of the Enumerator

        fs_stack = NULL;
        fs_stack_size = 0;
        fs_stack_pos = 0;
        ss_stack = NULL;
        ss_stack_size = 0;
        ss_stack_pos = 0;

        enum_vars = NULL;
        enum_expr = NULL;
        ATprotectList(&enum_vars);
        ATprotect(&enum_expr);

        reset(Vars,Expr,true_only);
}

EnumeratorSolutionsStandard::~EnumeratorSolutionsStandard()
{
        ATunprotectList(&enum_vars);
        ATunprotect(&enum_expr);

        if ( ss_stack_size != 0 )
                ATunprotectArray((ATerm *) ss_stack);
        free(ss_stack);
        if ( fs_stack_size != 0 )
                ATunprotectArray((ATerm *) fs_stack);
        free(fs_stack);
}



static ATermList map_ATreverse(ATermList l)
{
  if ( ATisEmpty(l) )
  {
    return l;
  } else {
    return ATinsert( map_ATreverse(ATgetNext(l)), (ATerm) ATreverse((ATermList) ATgetFirst(l)) );
  }
}

EnumeratorStandard::EnumeratorStandard(mcrl2::data::data_specification const& data_spec, Rewriter *r, bool clean_up_rewriter)
{
        info.rewr_obj = r;
        clean_up_rewr_obj = clean_up_rewriter;

        max_vars = MAX_VARS_INIT;
        info.max_vars = &max_vars;

        info.rewr_true = info.rewr_obj->toRewriteFormat(sort_bool_::true_());
        ATprotect(&info.rewr_true);
        info.rewr_false = info.rewr_obj->toRewriteFormat(sort_bool_::false_());
        ATprotect(&info.rewr_false);

        if ( (info.rewr_obj->getStrategy() == GS_REWR_INNER) || (info.rewr_obj->getStrategy() == GS_REWR_INNER_P) )
        {
          info.FindEquality = &EnumeratorSolutionsStandard::FindInner3Equality;
          info.build_solution_aux = &EnumeratorSolutionsStandard::build_solution_aux_inner3;
          info.opidAnd = info.rewr_obj->toRewriteFormat(gsMakeOpIdAnd());
          ATprotect(&info.opidAnd);
          info.eqs = ATindexedSetCreate(100,50);
          for (data_specification::mappings_const_range r(data_spec.mappings()); !r.empty(); r.advance_begin(1))
          {
            if (r.front().name() == "==")
            {
              ATbool b;
              ATindexedSetPut(info.eqs,info.rewr_obj->toRewriteFormat(r.front()),&b);
            }
          }
        } else {
          info.FindEquality = &EnumeratorSolutionsStandard::FindInnerCEquality;
          info.build_solution_aux = &EnumeratorSolutionsStandard::build_solution_aux_innerc;
          info.opidAnd = ATgetArgument((ATermAppl) info.rewr_obj->toRewriteFormat(gsMakeOpIdAnd()),0);
          ATprotect(&info.opidAnd);
          info.eqs = ATindexedSetCreate(100,50);
          for (data_specification::mappings_const_range r(data_spec.mappings()); !r.empty(); r.advance_begin(1))
          {
            if (r.front().name() == "==")
            {
              ATbool b;
              ATindexedSetPut(info.eqs,info.rewr_obj->toRewriteFormat(r.front()),&b);
            }
          }
        }

        info.tupAFun = ATmakeAFun("@tup@",2,ATfalse);
        ATprotectAFun(info.tupAFun);

        info.constructors = ATtableCreate(boost::distance(data_spec.sorts()),50);
        for (data_specification::sorts_const_range r(data_spec.sorts()); !r.empty(); r.advance_begin(1))
        {
          sort_expression target((r.front().is_alias()) ? mcrl2::data::alias(r.front()).name() : r.front());

          atermpp::aterm_list constructors;

          for (data_specification::constructors_const_range rc(data_spec.constructors(target)); !rc.empty(); rc.advance_begin(1))
          {
            constructors = atermpp::push_front(constructors,
              atermpp::aterm(ATmakeAppl2(info.tupAFun,
                reinterpret_cast< ATerm >(static_cast< ATermAppl >(rc.front())),
                  (ATerm) map_ATreverse(gsGetSortExprDomains(rc.front().sort())))));
          }

          ATtablePut(info.constructors,
            reinterpret_cast< ATerm >(static_cast< ATermAppl >(target)),
              reinterpret_cast< ATerm >(static_cast< ATermList >(constructors)));
        }
}

EnumeratorStandard::~EnumeratorStandard()
{
        ATunprotect(&info.rewr_true);
        ATunprotect(&info.rewr_false);

        ATunprotect(&info.opidAnd);
        ATindexedSetDestroy(info.eqs);

        ATunprotectAFun(info.tupAFun);

        ATtableDestroy(info.constructors);

        if ( clean_up_rewr_obj )
        {
                delete info.rewr_obj;
        }
}

EnumeratorSolutions *EnumeratorStandard::findSolutions(ATermList vars, ATerm expr, bool true_only, EnumeratorSolutions *old)
{
        if ( old == NULL )
        {
                return new EnumeratorSolutionsStandard(vars,expr,true_only,info);
        } else {
                ((EnumeratorSolutionsStandard *) old)->reset(vars,expr,true_only);
                return old;
        }
}

EnumeratorSolutions *EnumeratorStandard::findSolutions(ATermList vars, ATerm expr, EnumeratorSolutions *old)
{
        return findSolutions(vars,expr,true,old);
}

ATermList EnumeratorStandard::FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f)
{
        EnumeratorSolutions *sols = findSolutions(Vars,Expr);
        ATermList r = ATmakeList0();

        ATermList l;
        while ( sols->next(&l) && !sols->errorOccurred() )
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

Rewriter* EnumeratorStandard::getRewriter()
{
        return info.rewr_obj;
}

Enumerator* createEnumerator(mcrl2::data::data_specification const& data_spec, Rewriter *r, bool clean_up_rewriter, EnumerateStrategy strategy)
{
	switch ( strategy )
	{
		case ENUM_STANDARD:
			return new EnumeratorStandard(data_spec, r,clean_up_rewriter);
		default:
			return NULL;
	}
}
    } // namespace detail
  } // namespace data
} // namespace mcrl2
