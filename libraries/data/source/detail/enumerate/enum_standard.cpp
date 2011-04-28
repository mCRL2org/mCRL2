// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/core/detail/memory_utility.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/messaging.h"
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
  else if (info.eqs.find(t(0)) != info.eqs.end())  // Does term t have an equality as its function symbol?
  {
    const atermpp::aterm_appl a1 = t(1); 
    const atermpp::aterm_appl a2 = t(2); 
    if (a1!=a2)
    {
      if (is_variable(a1) && (find(vars.begin(),vars.end(),a1)!=vars.end()) && 
                               !gsOccurs((ATerm)(ATermAppl) a1,(ATerm)(ATermAppl) a2))
      {
        v = a1;
        e = a2; 
        return true;
      }
      if (is_variable(a2) && (find(vars.begin(),vars.end(),a2)!=vars.end()) &&
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
  variable_list substituted_vars = e.substituted_vars();
  atermpp::term_list< atermpp::aterm_appl > vals = e.vals();
  atermpp::aterm_appl expr = e.expr();

  variable var;
  atermpp::aterm_appl val;
  while (!vars.empty() && EnumeratorSolutionsStandard::FindInnerCEquality(expr,vars,var,val))
  {
    vars = (variable_list)ATremoveElement((ATermList)vars, (ATerm)(ATermAppl)var);
    info.rewr_obj->setSubstitutionInternal((ATermAppl) var,(ATerm)(ATermAppl)val);
    substituted_vars=push_front(substituted_vars,var);
    vals = push_front(vals,val);
    expr = (atermpp::aterm_appl)info.rewr_obj->rewriteInternal((ATerm)(ATermAppl)expr);
    info.rewr_obj->clearSubstitution((ATermAppl) var);
  }

  e=fs_expr(vars,substituted_vars,vals,expr);
}

atermpp::aterm_appl EnumeratorSolutionsStandard::build_solution_single(
                 const atermpp::aterm_appl t,                  
                 variable_list substituted_vars,
                 atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  assert(substituted_vars.size()==exprs.size());
  while (!substituted_vars.empty() && t!=substituted_vars.front())
  {
    substituted_vars=pop_front(substituted_vars); 
    exprs=pop_front(exprs); 
  }

  if (substituted_vars.empty())
  {
    return t;
  }
  else
  {
    return build_solution_aux_innerc(exprs.front(),pop_front(substituted_vars),pop_front(exprs));
  }
}

atermpp::aterm_appl EnumeratorSolutionsStandard::build_solution_aux_innerc(
                 const atermpp::aterm_appl t,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  if (is_variable(t))
  {
    return build_solution_single(t,substituted_vars,exprs);
  }
  else
  {
    atermpp::aterm_appl head = t(0);
    size_t arity = t.size();
    size_t extra_arity = 0;

    if (!ATisInt((ATerm)(ATermAppl)head))
    {
      head = build_solution_single(head,substituted_vars,exprs);
      if (!is_variable(head))
      {
        extra_arity = head.size()-1; 
      }
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm_appl,arity+extra_arity);
    AFun fun = ATgetAFun((ATermAppl) t);
    size_t k = 1;

    if (!ATisInt((ATerm)(ATermAppl)head) && !gsIsDataVarId((ATermAppl) head))
    {
      fun = ATmakeAFun("@appl_bs@",arity+extra_arity,false);
      k = extra_arity+1;
      for (size_t i=1; i<k; i++)
      {
        args[i] = head(i);
      }
      head = head(0);
    }

    args[0] = head;
    for (size_t i=1; i<arity; i++,k++)
    {
      args[k] = build_solution_aux_innerc(t(i),substituted_vars,exprs);
    }

    ATerm r = (ATerm) ATmakeApplArray(fun,(ATerm *)args);
    return r;
  }
}

atermpp::term_list < atermpp::aterm_appl> EnumeratorSolutionsStandard::build_solution2(
                 const variable_list vars, 
                 const variable_list substituted_vars, 
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  if (vars.empty())
  {
    return atermpp::term_list < atermpp::aterm_appl>();
  }
  else
  {
    return push_front(build_solution2(pop_front(vars),substituted_vars,exprs),
           (atermpp::aterm_appl)info.rewr_obj->rewriteInternal((ATerm)(ATermAppl)build_solution_single(vars.front(),substituted_vars,exprs)));
  }
}

atermpp::term_list < atermpp::aterm_appl> EnumeratorSolutionsStandard::build_solution(
                 const variable_list vars, 
                 const variable_list substituted_vars, 
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  return build_solution2(vars,reverse(substituted_vars),reverse(exprs));
}

bool EnumeratorSolutionsStandard::next(atermpp::term_list<atermpp::aterm_appl> &solution)
{
  while (ss_stack.empty() && !fs_stack.empty())
  {
    const fs_expr e=fs_stack.front();
    fs_stack.pop_front();
    assert(!e.vars().empty());

    variable_list variables_in_use=e.vars();
    const variable var = e.vars().front();
    const sort_expression sort = var.sort();
    variable_list uvars = pop_front(e.vars());

    if (is_function_sort(sort))
    {
      // HIER MOETEN FUNCTIETERMEN WORDEN TOEGEVOEGD.
      fs_stack.clear();
      throw mcrl2::runtime_error("cannot enumerate all elements of functions sort " + pp(sort));
    }
    else
    {
      const function_symbol_vector &constructors_for_sort = info.m_data_spec.constructors(sort);
      function_symbol_vector::const_iterator it=constructors_for_sort.begin();
      
      if ( it == constructors_for_sort.end() )
      {
        assert(!it->empty());
        fs_stack.clear(); 
        throw mcrl2::runtime_error("cannot enumerate elements of sort " + pp(sort) + " as it does not have constructor functions");
      }
      for( ; it!=constructors_for_sort.end() ; ++it)
      {
        // Construct the domain and target sort for the constructor.
        sort_expression target_sort=it->sort();
        sort_expression_list domain_sorts;
        if (is_function_sort(target_sort))
        { 
          domain_sorts=function_sort(target_sort).domain();
          target_sort=function_sort(target_sort).codomain();
        }
        assert(target_sort==sort);

        variable_list var_list;
        for (sort_expression_list::const_iterator i=domain_sorts.begin(); i!=domain_sorts.end(); ++i) 
        {
          const variable fv = variable(gsMakeDataVarId(gsFreshString2ATermAppl("@enum@",(ATerm)(ATermList)variables_in_use,false),*i));
          var_list = push_front(var_list,fv);
          variables_in_use=push_front(variables_in_use,fv);

          used_vars++;
          if (used_vars > max_vars)
          {
            cerr << "need more than " << max_vars << " variables to find all valuations of ";
            for (variable_list::const_iterator k=enum_vars.begin(); k!=enum_vars.end(); ++k)
            {
              if (k != enum_vars.begin())
              {
                cerr << ", ";
              }
              cerr << pp(*k) << ":" << pp(k->sort());
            }
            cerr << " that satisfy " << pp(info.rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)enum_expr)) << endl;
            max_vars *= MAX_VARS_FACTOR;
          }
        }
        data_expression cons_term;
        if (var_list.empty())
        { 
          cons_term=*it;
        }
        else
        {
          cons_term = application(*it, reverse(var_list));
        }
        atermpp::aterm_appl term_rf = info.rewr_obj->rewriteInternal(info.rewr_obj->toRewriteFormat(cons_term));

        info.rewr_obj->setSubstitutionInternal(var,(ATerm)(ATermAppl)term_rf);
        ATerm new_expr = info.rewr_obj->rewriteInternal((ATerm)(ATermAppl)e.expr());

        if ((m_not_equal_to_false && new_expr!=info.rewr_false) ||
            (!m_not_equal_to_false && new_expr!=info.rewr_true))
        {
          fs_stack.push_back(fs_expr(
                                uvars+var_list,
                                push_front(e.substituted_vars(),var),
                                push_front(e.vals(),term_rf),
                                (atermpp::aterm_appl)new_expr));
          if ((fs_stack.back().vars().empty()) || 
                   (EliminateVars(fs_stack.back()), (fs_stack.back().vars().empty())) || 
                   (m_not_equal_to_false && fs_stack.back().expr()==info.rewr_false)  ||
                   (!m_not_equal_to_false && fs_stack.back().expr()==info.rewr_true))
          {
            if ((m_not_equal_to_false && fs_stack.back().expr()!=info.rewr_false) ||
                (!m_not_equal_to_false && fs_stack.back().expr()!=info.rewr_true))
            {
              ss_stack.push_back(build_solution(enum_vars,fs_stack.back().substituted_vars(),fs_stack.back().vals()));
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
    solution = ss_stack.back();
    ss_stack.pop_back();
    return true;
  }
  else
  {
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
  max_vars = MAX_VARS_INIT;

  fs_stack.push_back(fs_expr(enum_vars,variable_list(),atermpp::term_list< atermpp::aterm_appl>(),enum_expr));
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
      ss_stack.push_back(build_solution(enum_vars,fs_stack.front().substituted_vars(),fs_stack.front().vals()));
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
  max_vars(other.max_vars),
  fs_stack(other.fs_stack),
  ss_stack(other.ss_stack)
{
  enum_vars.protect();
  enum_expr.protect();
}

EnumeratorSolutionsStandard::~EnumeratorSolutionsStandard()
{
  enum_vars.unprotect();
  enum_expr.unprotect();

}

EnumeratorStandard::EnumeratorStandard(const mcrl2::data::data_specification &data_spec, Rewriter* r, const bool clean_up_rewriter):
  info(data_spec)
{
  info.rewr_obj = r;
  clean_up_rewr_obj = clean_up_rewriter;

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
}

EnumeratorStandard::~EnumeratorStandard()
{
  info.rewr_true.unprotect();
  info.rewr_false.unprotect();

  ATunprotect(&info.opidAnd);

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
