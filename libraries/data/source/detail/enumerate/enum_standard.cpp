// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <sstream>
#include "aterm2.h"
#include "mcrl2/core/detail/memory_utility.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/messaging.h"
#include <aterm_ext.h>
#include "mcrl2/core/print.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/detail/enum/standard.h"
#include "mcrl2/data/bool.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

#define MAX_VARS_INIT   1000
#define MAX_VARS_FACTOR 5

using namespace std;

namespace mcrl2
{
namespace data
{
namespace detail
{

bool EnumeratorSolutionsStandard::IsInnerCEq(ATermAppl a)
{
  return info.eqs.find(ATgetArgument(a,0)) != info.eqs.end() ;
}

bool EnumeratorSolutionsStandard::FindInnerCEquality(
                        const atermpp::aterm_appl t,
                        const mcrl2::data::variable_list vars, 
                        mcrl2::data::variable &v, 
                        atermpp::aterm_appl &e)
{
  if (t(0) == info.opidAnd)
  {
    return FindInnerCEquality(t(1),vars,v,e) || FindInnerCEquality(t(2),vars,v,e);
  }
  else if (IsInnerCEq((ATermAppl) t))
  {
    const atermpp::aterm_appl a1 = t(1); 
    const atermpp::aterm_appl a2 = t(2); 
    if (a1!=a2)
    {
      if (gsIsDataVarId(a1) && (find(vars.begin(),vars.end(),a1)!=vars.end()) && 
                               !gsOccurs((ATerm)(ATermAppl) a1,(ATerm)(ATermAppl) a2))
      {
        v = a1;
        e = a2; 
        return true;
      }
      if (gsIsDataVarId(a2) && (find(vars.begin(),vars.end(),a2)!=vars.end()) &&
                               !gsOccurs((ATerm)(ATermAppl) a2,(ATerm)(ATermAppl) a1))
      {
        v = a2;
        e = a1;
        return true;
      }
    }
  }

  return false;
}

void EnumeratorSolutionsStandard::EliminateVars(fs_expr &e)
{
  variable_list vars = e.vars();
  atermpp::term_list< atermpp::aterm_appl > vals = e.vals();
  atermpp::aterm_appl expr = e.expr();

  variable var;
  atermpp::aterm_appl val;
  while (!vars.empty() && EnumeratorSolutionsStandard::FindInnerCEquality(expr,vars,var,val))
  {
    vars = (variable_list)ATremoveElement((ATermList)vars, (ATerm)(ATermAppl)var);
    info.rewr_obj->setSubstitutionInternal((ATermAppl) var,(ATerm)(ATermAppl)val);
    vals = ATinsert(vals,(ATerm) ATmakeAppl2(info.tupAFun,(ATerm)(ATermAppl)var,(ATerm)(ATermAppl)val));
    expr = (atermpp::aterm_appl)info.rewr_obj->rewriteInternal((ATerm)(ATermAppl)expr);
    info.rewr_obj->clearSubstitution((ATermAppl) var);
  }

  e=fs_expr(vars,vals,expr);
}

ATerm EnumeratorSolutionsStandard::build_solution_single(ATerm t, ATermList substs)
{
  while (!ATisEmpty(substs) && !ATisEqual(t, ATgetArgument((ATermAppl) ATgetFirst(substs),0)))
  {
    substs = ATgetNext(substs);
  }

  if (ATisEmpty(substs))
  {
    return t;
  }
  else
  {
    return build_solution_aux_innerc(ATgetArgument((ATermAppl) ATgetFirst(substs),1),ATgetNext(substs));
  }
}

ATerm EnumeratorSolutionsStandard::build_solution_aux_innerc(ATerm t, ATermList substs)
{
  if (gsIsDataVarId((ATermAppl) t))
  {
    return build_solution_single(t,substs);
  }
  else
  {
    ATerm head = ATgetArgument((ATermAppl) t,0);
    size_t arity = ATgetArity(ATgetAFun((ATermAppl) t));
    size_t extra_arity = 0;

    if (!ATisInt(head))
    {
      head = build_solution_single(head,substs);
      if (!gsIsDataVarId((ATermAppl) head))
      {
        extra_arity = ATgetArity(ATgetAFun((ATermAppl) head))-1;
      }
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,ATerm,arity+extra_arity);
    AFun fun = ATgetAFun((ATermAppl) t);
    int k = 1;

    if (!ATisInt(head) && !gsIsDataVarId((ATermAppl) head))
    {
      fun = ATmakeAFun("@appl_bs@",arity+extra_arity,false);
      k = extra_arity+1;
      for (int i=1; i<k; i++)
      {
        args[i] = ATgetArgument((ATermAppl) head,i);
      }
      head = ATgetArgument((ATermAppl) head,0);
    }

    args[0] = head;
    for (size_t i=1; i<arity; i++,k++)
    {
      args[k] = build_solution_aux_innerc(ATgetArgument((ATermAppl) t,i),substs);
    }

    ATerm r = (ATerm) ATmakeApplArray(fun,args);
    return r;
  }
}

ATermList EnumeratorSolutionsStandard::build_solution2(ATermList vars, ATermList substs)
{
  if (ATisEmpty(vars))
  {
    return vars;
  }
  else
  {
    return ATinsert(build_solution2(ATgetNext(vars),substs),
           (ATerm) gsMakeSubst(ATgetFirst(vars),info.rewr_obj->rewriteInternal(build_solution_single(ATgetFirst(vars),substs))));
  }
}
ATermList EnumeratorSolutionsStandard::build_solution(ATermList vars, ATermList substs)
{
  return build_solution2(vars,ATreverse(substs));
}

bool EnumeratorSolutionsStandard::next(ATermList* solution)
{
  while (ss_stack.empty() && !fs_stack.empty())
  {
    const fs_expr e=fs_stack.back();
    fs_stack.pop_back();

    const variable var = e.vars().front();
    const sort_expression sort = var.sort();

    if (is_function_sort(sort))
    {
      // HIER MOETEN FUNCTIETERMEN WORDEN TOEGEVOEGD.
      fs_stack.clear();
      throw mcrl2::runtime_error("cannot enumerate all elements of functions sort " + pp(sort));
    }
    else
    {
      atermpp::map<ATermAppl, ATermList>::iterator it = info.constructors.find(sort);
      // function_symbol_vector &constructors_for_sort = data_spec.constructors(sort);
      // function_symbol_vector::const_iterator it=constructors_for_sort.begin();
      
      if ( it == info.constructors.end() || ATisEmpty(it->second) )
      {
        fs_stack.clear(); 
        throw mcrl2::runtime_error("cannot enumerate elements of sort " + pp(sort) + " as it does not have constructor functions");
      }
      ATermList l = it->second;

      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        ATermAppl cons_tup = (ATermAppl) ATgetFirst(l);
        ATermAppl cons_term = (ATermAppl) ATgetArgument(cons_tup,0);
        ATermList rdom_sorts = (ATermList) ATgetArgument(cons_tup,1);

        variable_list uvars = reverse(e.vars());

        for (; !ATisEmpty(rdom_sorts); rdom_sorts=ATgetNext(rdom_sorts))
        {
          ATermList rev_dom_sorts1 = ATLgetFirst(rdom_sorts);
          ATermList var_list = ATmakeList0();
          for (; !ATisEmpty(rev_dom_sorts1); rev_dom_sorts1=ATgetNext(rev_dom_sorts1))
          {
            ATermAppl fv = gsMakeDataVarId(gsFreshString2ATermAppl("@enum@",(ATerm)(ATermList)uvars,false),ATAgetFirst(rev_dom_sorts1));
            var_list = ATinsert(var_list,(ATerm) fv);
            uvars = ATinsert(uvars,(ATerm) fv);

            used_vars++;
            if (used_vars > *info.max_vars)
            {
              stringstream msg;
              msg << "need more than " << *info.max_vars << " variables to find all valuations of ";
              for (ATermList k=enum_vars; !ATisEmpty(k); k=ATgetNext(k))
              {
                if (k != enum_vars)
                {
                  msg << ", ";
                }
                PrintPart_CXX(msg,ATgetFirst(k),ppDefault);
                msg << ": ";
                PrintPart_CXX(msg,ATgetArgument((ATermAppl) ATgetFirst(k),1),ppDefault);
              }
              msg << " that satisfy ";
              PrintPart_CXX(msg,(ATerm) info.rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)enum_expr),ppDefault);
              msg << endl;
              gsWarningMsg("%s",msg.str().c_str());
              *info.max_vars *= MAX_VARS_FACTOR;
            }
          }
          cons_term = gsMakeDataAppl(cons_term, var_list);
        }
        ATerm term_rf = info.rewr_obj->rewriteInternal(info.rewr_obj->toRewriteFormat(cons_term));

        info.rewr_obj->setSubstitutionInternal(var,term_rf);
        ATerm new_expr = info.rewr_obj->rewriteInternal((ATerm)(ATermAppl)e.expr());


        if ((m_not_equal_to_false && new_expr!=info.rewr_false) ||
            (!m_not_equal_to_false && new_expr!=info.rewr_true))
        {
          fs_stack.push_back(fs_expr(
                                pop_front(reverse(uvars)),
                                push_front(e.vals(),(atermpp::aterm_appl)ATmakeAppl2(info.tupAFun,(ATerm) (ATermAppl)var,(ATerm) term_rf)),
                                (atermpp::aterm_appl)new_expr));
          if ((fs_stack.back().vars().empty()) || 
                   (EliminateVars(fs_stack.back()), (fs_stack.back().vars().empty()) || 
                   (fs_stack.back().expr()==info.rewr_false)))
          {
            if ((m_not_equal_to_false && fs_stack.back().expr()!=info.rewr_false) ||
                (!m_not_equal_to_false && fs_stack.back().expr()!=info.rewr_true))
            {
ATfprintf(stderr,"SOLUTIONnext %t\n%t\n",(ATermList)enum_vars,(ATermList)fs_stack.front().vals());
              ss_stack.push_back(build_solution(enum_vars,fs_stack.back().vals()));
            }
            fs_stack.pop_back();
          }
        }
        info.rewr_obj->clearSubstitution(var);
      }
    }
  }

  if (!ss_stack.empty())
  {
    *solution = ss_stack.back();
    ss_stack.pop_back();
    return true;
  }
  else
  {
    *solution = NULL;
    return false;
  }
}

void EnumeratorSolutionsStandard::reset(const variable_list &Vars, const atermpp::aterm_appl &Expr, const bool netf)
{
  enum_vars = Vars;
  enum_expr = (atermpp::aterm_appl)info.rewr_obj->rewriteInternal((ATerm)(ATermAppl)Expr);
  m_not_equal_to_false = netf;

  fs_stack.clear();

  used_vars = 0;

  fs_stack.push_back(fs_expr(enum_vars,atermpp::term_list< atermpp::aterm_appl>(),enum_expr));
  if (!enum_vars.empty())
  {
    EliminateVars(fs_stack.front());
  }

  if (fs_stack.front().expr()==info.rewr_false)
  {
    fs_stack.pop_back();
  }
  else if (fs_stack.front().vars().empty())
  {
    if ((m_not_equal_to_false && fs_stack.front().expr()!=info.rewr_true) ||
        (!m_not_equal_to_false && fs_stack.front().expr()!=info.rewr_false))
    {
      throw mcrl2::runtime_error("term does not evaluate to true or false " +
                                 pp(info.rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)fs_stack.front().expr())));
    }
    else
    {
ATfprintf(stderr,"SOLUTION %t\n%t\n",(ATermList)enum_vars,(ATermList)fs_stack.front().vals());
      ss_stack.push_back(build_solution(enum_vars,fs_stack.front().vals()));
    }
    fs_stack.pop_back();
  }
}

EnumeratorSolutionsStandard::EnumeratorSolutionsStandard(EnumeratorSolutionsStandard const& other) :
  info(other.info), 
  enum_vars(other.enum_vars), 
  enum_expr(other.enum_expr),
  m_not_equal_to_false(other.m_not_equal_to_false), 
  used_vars(other.used_vars),
  fs_stack(other.fs_stack)
{
  ss_stack=other.ss_stack; 
  enum_vars.protect();
  enum_expr.protect();
}

EnumeratorSolutionsStandard::~EnumeratorSolutionsStandard()
{
  enum_vars.unprotect();
  enum_expr.unprotect();

}

static ATermList map_ATreverse(ATermList l)
{
  if (ATisEmpty(l))
  {
    return l;
  }
  else
  {
    return ATinsert(map_ATreverse(ATgetNext(l)), (ATerm) ATreverse((ATermList) ATgetFirst(l)));
  }
}

///\pre    SortExpr is a sort expression
///\return The domains of the sort expression, in the following sense:
///  \li if SortExpr is not an arrow sort, then the domains is the empty list []
///  \li if SortExpr is an arrow sort, i.e. an expression of the form <tt>SortArrow([S0,...,Sn], S)</tt>,
///      then the domains is the list <tt>[S0,...,Sn]</tt> inserted at the head of the domains of <tt>S</tt>
///
///\detail Some example arguments and return values,
///  where <tt>A</tt>,<tt>B</tt>,<tt>C</tt>,<tt>A0</tt>,...,<tt>An</tt> and <tt>B0</tt>,...,<tt>Bm</tt> are all non-arrow sorts:
///  \li <tt>A</tt>: returns <tt>[]</tt>
///  \li <tt>SortArrow([A0,...An], B)</tt>: returns <tt>[[A0,...,An]]</tt>
///  \li <tt>SortArrow([A0,...An], SortArrow([B0,...,Bm], C))</tt>: returns <tt>[[A0,...,An],[B0,...,Bm]]</tt>
// For backwards compatibility, copied with struct.h removal
static
ATermList gsGetSortExprDomains(ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  ATermList l = ATmakeList0();
  while (gsIsSortArrow(SortExpr))
  {
    ATermList dom = ATLgetArgument(SortExpr,0);
    l = ATinsert(l, (ATerm) dom);
    SortExpr = ATAgetArgument(SortExpr,1);
  }
  l = ATreverse(l);
  return l;
}

EnumeratorStandard::EnumeratorStandard(const mcrl2::data::data_specification &data_spec, Rewriter* r, const bool clean_up_rewriter)
{
  info.rewr_obj = r;
  clean_up_rewr_obj = clean_up_rewriter;

  max_vars = MAX_VARS_INIT;
  info.max_vars = &max_vars;

  info.rewr_true.protect();
  info.rewr_true = (atermpp::aterm_appl)info.rewr_obj->toRewriteFormat(sort_bool::true_());
  info.rewr_false.protect();
  info.rewr_false = (atermpp::aterm_appl)info.rewr_obj->toRewriteFormat(sort_bool::false_());

  info.opidAnd = NULL;
  ATprotect(&info.opidAnd);
  if ((info.rewr_obj->getStrategy() == GS_REWR_INNER) || (info.rewr_obj->getStrategy() == GS_REWR_INNER_P))
  {
    throw mcrl2::runtime_error("The classic enumerator does not work (anymore) with the INNER and INNER_P rewriter.");
  }
  else
  {
    info.opidAnd = ATgetArgument((ATermAppl) info.rewr_obj->toRewriteFormat(sort_bool::and_()),0);

    const function_symbol_vector mappings(data_spec.mappings());
    for (function_symbol_vector::const_iterator i = mappings.begin(); i != mappings.end(); ++i)
    {
      if (i->name() == "==")
      {
        info.eqs.insert( ATgetArgument((ATermAppl) info.rewr_obj->toRewriteFormat(*i),0) );
      }
    }
  }

  info.tupAFun = 0;
  ATprotectAFun(info.tupAFun);
  info.tupAFun = ATmakeAFun("@tup@",2,false);

  const atermpp::vector<sort_expression> sorts = data_spec.sorts();
  for (atermpp::vector<sort_expression>::const_iterator r = sorts.begin(); r != sorts.end(); ++r)
  {
    atermpp::aterm_list constructors;

    function_symbol_vector r_constructors(data_spec.constructors(*r));
    for (function_symbol_vector::const_iterator i = r_constructors.begin(); i != r_constructors.end(); ++i)
    {
      constructors = atermpp::push_front(constructors,
                                         atermpp::aterm(ATmakeAppl2(info.tupAFun,
                                             reinterpret_cast< ATerm >(static_cast< ATermAppl >(*i)),
                                             (ATerm) map_ATreverse(gsGetSortExprDomains(i->sort())))));
    }

    info.constructors[*r] = constructors;
  }
}

EnumeratorStandard::~EnumeratorStandard()
{
  info.rewr_true.unprotect();
  info.rewr_false.unprotect();

  ATunprotect(&info.opidAnd);

  ATunprotectAFun(info.tupAFun);

  if (clean_up_rewr_obj)
  {
    delete info.rewr_obj;
  }
}

EnumeratorSolutionsStandard *EnumeratorStandard::findSolutions(ATermList vars, ATerm expr, bool not_equal_to_false, EnumeratorSolutionsStandard* old)
{
  if (old == NULL)
  {
    return new EnumeratorSolutionsStandard(vars,expr,not_equal_to_false,info);
  }
  else
  {
    ((EnumeratorSolutionsStandard*) old)->reset(vars,expr,not_equal_to_false);
    return old;
  }
}

Rewriter* EnumeratorStandard::getRewriter()
{
  return info.rewr_obj;
}


} // namespace detail
} // namespace data
} // namespace mcrl2
